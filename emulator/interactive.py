"""
Interactive REPL debugger for the Tek 2230 firmware emulator - shows
live register/flag state and the current instruction after every stop,
and lets you single-step, set breakpoints, and simulate front-panel
button presses or incoming serial bytes while the firmware runs.

Built on the same memory map and stubs as emu.py (the headless batch
tracer) - use emu.py for long, unattended runs (it's much faster with
no per-step Python round-trip) and this one for interactive exploration
of a specific point in execution. Not a full-screen TUI (no curses
dependency, so it works the same in PowerShell and Git Bash) - a
classic scrollback-log-style debugger prompt instead.

Usage:
    python interactive.py [--revision 13|14] [--tick-interval N]

Type `help` at the `(tek2230)` prompt for the command list.
"""
import argparse
import ctypes
import re
import shlex

import capstone
import unicorn as uc
from unicorn import x86_const as x86

import memory_map as mm
from timer import TickScheduler
from io_stubs import (CommPresenceProbe, DiagnosticTextCapture,
                       DISPLAY_CHIP_STUBS, COMM_OPTION_STUBS, FRONT_PANEL_STUBS,
                       FixedByteRead, InteractiveFrontPanel, InteractiveUartMock,
                       ANSI_GRAY, ANSI_RESET)

HMA_ALIAS_BASE = 0x100000
HMA_ALIAS_SIZE = 0x10000

FLAG_BITS = [("CF", 0x1), ("PF", 0x4), ("AF", 0x10), ("ZF", 0x40),
             ("SF", 0x80), ("TF", 0x100), ("IF", 0x200), ("DF", 0x400),
             ("OF", 0x800)]

REGS = ["AX", "BX", "CX", "DX", "SI", "DI", "BP", "SP", "CS", "DS", "ES", "SS"]
REG_CONST = {name: getattr(x86, f"UC_X86_REG_{name}") for name in REGS}

# For resolving a traced instruction's memory operand back to a live
# register value (16-bit real mode addressing: base/index registers
# only, scale is always 1) - capstone reports operand register names
# lowercase (e.g. "bx", "si"); map straight into REG_CONST.
_OPERAND_REG_CONST = {name.lower(): const for name, const in REG_CONST.items()}

_ESCAPES = {"n": "\n", "r": "\r", "t": "\t", "0": "\0", "\\": "\\"}


def decode_escapes(s):
    """`serial hello\\n` types the 2 literal characters backslash+n at
    the prompt (there's no way to type a real newline into a single
    input() line) - expand the handful of escapes a real terminal
    program would let you send, same idea as `echo -e`/`printf`.
    Unrecognized `\\x` sequences pass through unchanged."""
    return re.sub(r"\\(.)", lambda m: _ESCAPES.get(m.group(1), m.group(0)), s)


def flags_str(fl):
    return " ".join(name for name, bit in FLAG_BITS if fl & bit) or "-"


class Debugger:
    def __init__(self, args):
        self.args = args
        self.emu = uc.Uc(uc.UC_ARCH_X86, uc.UC_MODE_16)
        self.count = 0
        self.breakpoints = set()
        self.trace = False
        self.continue_length = args.continue_length
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
        self.md.detail = True  # needed to inspect memory operands for `trace`
        self.ticker = TickScheduler(args.tick_interval) if args.tick_interval else None
        self.diag = DiagnosticTextCapture()
        self.front_panel = InteractiveFrontPanel()
        self.uart = InteractiveUartMock()
        self._low_ram_buffer = None
        self._stop_reason = None
        self._setup_memory()
        self._setup_stubs()
        self.emu.hook_add(uc.UC_HOOK_CODE, self._on_code, None)
        self.emu.hook_add(uc.UC_HOOK_MEM_INVALID, self._on_invalid, None)
        self.emu.reg_write(x86.UC_X86_REG_CS, 0xF000)
        self.emu.reg_write(x86.UC_X86_REG_IP, 0xFFF0)

    def _setup_memory(self):
        for region in mm.all_regions(self.args.revision):
            perms = uc.UC_PROT_READ | uc.UC_PROT_EXEC
            if region.writable:
                perms |= uc.UC_PROT_WRITE
            if region.name == "low_ram_ivt_and_vars":
                self._low_ram_buffer = ctypes.create_string_buffer(region.size)
                ptr = ctypes.addressof(self._low_ram_buffer)
                self.emu.mem_map_ptr(region.start, region.size, perms, ptr)
                self.emu.mem_map_ptr(HMA_ALIAS_BASE, HMA_ALIAS_SIZE, perms, ptr)
                continue
            self.emu.mem_map(region.start, region.size, perms)
            if region.data is not None:
                self.emu.mem_write(region.start, region.data)

    def _setup_stubs(self):
        for address, value, label in DISPLAY_CHIP_STUBS + COMM_OPTION_STUBS + FRONT_PANEL_STUBS:
            FixedByteRead(address, value, label).install(self.emu, uc)
        self.front_panel.install(self.emu, uc)
        if self.args.comm_installed:
            CommPresenceProbe().install(self.emu, uc)
        # Installed after COMM_OPTION_STUBS's fixed comm_stat value so
        # it only adjusts bit1 on top of that baseline - see
        # InteractiveUartMock's docstring.
        self.uart.install(self.emu, uc)
        self.diag.install(self.emu, uc)

    def _on_code(self, uc_eng, address, size, user_data):
        self.count += 1
        if self.trace:
            print(f"{ANSI_GRAY}{self.trace_line()}{ANSI_RESET}")
        if self.ticker is not None:
            self.ticker.step(uc_eng)
        if address in self.breakpoints:
            self._stop_reason = f"breakpoint hit at 0x{address:06X}"
            uc_eng.emu_stop()

    def _decode_current(self):
        phys, _, _ = self.physical_ip()
        try:
            code = self.emu.mem_read(phys, 16)
        except uc.UcError:
            return None, phys
        for insn in self.md.disasm(bytes(code), phys):
            return insn, phys
        return None, phys

    def _memory_operand_value(self, insn):
        """Resolve the first memory operand's effective address/value,
        e.g. for `mov al, byte ptr [bp+6]` show what's actually at that
        address right now - real-mode 16-bit addressing only ever uses
        base+index+disp (scale is always 1), so no scale handling
        needed."""
        for op in insn.operands:
            if op.type != capstone.x86.X86_OP_MEM:
                continue
            mem = op.mem
            base = self.emu.reg_read(_OPERAND_REG_CONST[insn.reg_name(mem.base)]) if mem.base else 0
            index = self.emu.reg_read(_OPERAND_REG_CONST[insn.reg_name(mem.index)]) if mem.index else 0
            offset = (base + index + mem.disp) & 0xFFFF
            if mem.segment:
                seg = self.emu.reg_read(_OPERAND_REG_CONST[insn.reg_name(mem.segment)])
            elif mem.base and insn.reg_name(mem.base) in ("bp", "sp"):
                seg = self.emu.reg_read(x86.UC_X86_REG_SS)
            else:
                seg = self.emu.reg_read(x86.UC_X86_REG_DS)
            phys = ((seg << 4) + offset) & 0xFFFFF
            size = op.size or 1
            try:
                raw = self.emu.mem_read(phys, size)
                value = int.from_bytes(raw, "little")
            except uc.UcError:
                return f"[{seg:04X}:{offset:04X}]=<unmapped>"
            return f"[{seg:04X}:{offset:04X}]=0x{value:0{size*2}X}"
        return None

    def trace_line(self):
        phys, cs, ip = self.physical_ip()
        regs = {name: self.emu.reg_read(const) for name, const in REG_CONST.items()}
        fl = self.emu.reg_read(x86.UC_X86_REG_EFLAGS)
        insn, _ = self._decode_current()
        text = f"{insn.mnemonic} {insn.op_str}".strip() if insn else "<undecodable>"
        data = self._memory_operand_value(insn) if insn else None
        line = (f"{self.count:>9} {cs:04X}:{ip:04X} p{phys:06X} "
                f"AX={regs['AX']:04X} BX={regs['BX']:04X} CX={regs['CX']:04X} DX={regs['DX']:04X} "
                f"SI={regs['SI']:04X} DI={regs['DI']:04X} BP={regs['BP']:04X} SP={regs['SP']:04X} "
                f"DS={regs['DS']:04X} ES={regs['ES']:04X} SS={regs['SS']:04X} "
                f"FL={flags_str(fl):<12} | {text}")
        if data:
            line += f"   ({data})"
        return line

    def _on_invalid(self, uc_eng, access, address, size, value, user_data):
        kind = {
            uc.UC_MEM_READ_UNMAPPED: "READ", uc.UC_MEM_WRITE_UNMAPPED: "WRITE",
            uc.UC_MEM_FETCH_UNMAPPED: "FETCH", uc.UC_MEM_READ_PROT: "READ(prot)",
            uc.UC_MEM_WRITE_PROT: "WRITE(prot)", uc.UC_MEM_FETCH_PROT: "FETCH(prot)",
        }.get(access, str(access))
        self._stop_reason = f"unmapped {kind} at 0x{address:06X} (size={size}, value=0x{value:X})"
        return False

    def physical_ip(self):
        cs = self.emu.reg_read(x86.UC_X86_REG_CS)
        ip = self.emu.reg_read(x86.UC_X86_REG_IP)
        return ((cs << 4) + ip) & 0xFFFFF, cs, ip

    def current_instruction_text(self):
        phys, cs, ip = self.physical_ip()
        try:
            code = self.emu.mem_read(phys, 16)
        except uc.UcError:
            return "<unreadable>"
        for insn in self.md.disasm(bytes(code), phys):
            return f"{insn.mnemonic} {insn.op_str}".strip()
        return "<undecodable>"

    def print_status(self):
        phys, cs, ip = self.physical_ip()
        regs = {name: self.emu.reg_read(const) for name, const in REG_CONST.items()}
        fl = self.emu.reg_read(x86.UC_X86_REG_EFLAGS)
        print(f"\n[{self.count} instrs]  CS:IP={cs:04X}:{ip:04X}  (phys 0x{phys:06X})   FLAGS: {flags_str(fl)}")
        print(f"  AX={regs['AX']:04X} BX={regs['BX']:04X} CX={regs['CX']:04X} DX={regs['DX']:04X}"
              f"  SI={regs['SI']:04X} DI={regs['DI']:04X} BP={regs['BP']:04X} SP={regs['SP']:04X}")
        print(f"  DS={regs['DS']:04X} ES={regs['ES']:04X} SS={regs['SS']:04X}"
              f"  |  {self.front_panel.status()}  |  {self.uart.status()}")
        print(f"  -> {self.current_instruction_text()}")
        if self._stop_reason:
            print(f"  [stopped: {self._stop_reason}]")
            self._stop_reason = None

    def step(self, n=1):
        phys, _, _ = self.physical_ip()
        try:
            self.emu.emu_start(phys, 0x100000, count=n)
        except uc.UcError as e:
            self._stop_reason = self._stop_reason or f"Unicorn error: {e}"

    def run(self, max_count):
        phys, _, _ = self.physical_ip()
        try:
            self.emu.emu_start(phys, 0x100000, count=max_count)
        except uc.UcError as e:
            self._stop_reason = self._stop_reason or f"Unicorn error: {e}"


HELP = """\
Commands:
  step [n]          execute n instructions (default 1)
  run [n]           execute up to n instructions (default 100000),
                     stopping early on a breakpoint or fault
  continue [n], c    run for the configured continue-length (default
                     25000000, see --continue-length); `continue <n>`
                     also updates that stored length for next time
  trace on|off       stream one line per executed instruction (all
                     registers + the current instruction + the live
                     value of its memory operand, if any) instead of
                     only printing a status block when you stop -
                     scrolls continuously during any step/run/continue
  break <hex addr>   add a breakpoint (physical address, e.g. E0AE9)
  delete <hex addr>  remove a breakpoint
  breakpoints        list current breakpoints
  regs               reprint the register/status block
  mem <hex addr> [n] hex-dump n bytes (default 16) at a physical address
  press <BUTTON>     press a front-panel button (see `buttons`)
  release <BUTTON>   release a front-panel button
  buttons            list button names and current SWB1/SWB2 values
  serial <text>      queue <text> as incoming bytes on the (experimental)
                     mock UART receive path - see InteractiveUartMock's
                     docstring for what this does and doesn't model.
                     Supports \\n \\r \\t \\0 \\\\ escapes (e.g.
                     `serial AT\\r\\n`) since there's no way to type a
                     real newline into a single input() line
  uart               show the mock UART's pending RX queue and captured
                     TX bytes (writes to the same register - see
                     `outgoing`)
  outgoing           show every byte captured on the (write) side of
                     the same mock UART data register - this overlaps
                     with `diag` today since write_readout_port_byte is
                     the only confirmed writer of that address so far.
                     Real \r/\n/tab bytes print as real line breaks/
                     tabs, not escaped text
  outgoing clear     discard everything captured so far on that side
  diag               show all captured diagnostic-text lines so far
  help, ?            show this text
  quit, exit, q      leave the debugger
"""


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                  formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--revision", default="14", choices=["13", "14"])
    ap.add_argument("--tick-interval", type=int, default=2000,
                     help="instructions between synthetic INT2 ticks, "
                          "0 disables the scheduler timer (default 2000)")
    ap.add_argument("--comm-installed", action=argparse.BooleanOptionalAction,
                     default=True,
                     help="stub detect_comm_option_hw's presence probe "
                          "as 'installed' - on by default, matching this "
                          "project's real physical test units; pass "
                          "--no-comm-installed for the 'not installed' "
                          "behavior - see io_stubs.CommPresenceProbe")
    ap.add_argument("--continue-length", type=int, default=25000000,
                     help="instruction count `continue`/`c` runs for "
                          "when called with no explicit argument "
                          "(default 25000000); `continue <n>` also "
                          "updates this for the rest of the session")
    args = ap.parse_args()

    dbg = Debugger(args)
    print("Tek 2230 interactive emulator debugger. Type `help` for commands.")
    dbg.print_status()

    while True:
        try:
            line = input("\n(tek2230) ").strip()
        except EOFError:
            print()
            break
        if not line:
            continue

        # `serial` takes the rest of the line as raw text (its own
        # \n/\r/\t escapes, not shell quoting) - shlex would silently
        # eat a literal `\n` outside quotes before decode_escapes ever
        # saw it, since backslash is shlex's own escape character too.
        # No .strip() on the payload itself - only split() 's own
        # leading-whitespace skip - a genuine trailing \r the user
        # typed shouldn't be silently eaten.
        first_split = line.split(None, 1)
        if first_split[0].lower() == "serial":
            cmd = "serial"
            rest = [first_split[1]] if len(first_split) > 1 else []
        else:
            try:
                parts = shlex.split(line)
            except ValueError as e:
                print(f"parse error: {e}")
                continue
            cmd, rest = parts[0].lower(), parts[1:]

        if cmd in ("quit", "exit", "q"):
            break
        elif cmd in ("help", "?"):
            print(HELP)
        elif cmd == "step":
            n = int(rest[0]) if rest else 1
            dbg.step(n)
            dbg.print_status()
        elif cmd in ("run",):
            n = int(rest[0]) if rest else 100000
            dbg.run(n)
            dbg.print_status()
        elif cmd in ("continue", "c"):
            if rest:
                dbg.continue_length = int(rest[0])
                print(f"continue length set to {dbg.continue_length}")
            dbg.run(dbg.continue_length)
            dbg.print_status()
        elif cmd == "trace":
            if not rest or rest[0].lower() not in ("on", "off"):
                print("usage: trace on|off")
                continue
            dbg.trace = rest[0].lower() == "on"
            print(f"trace {'enabled' if dbg.trace else 'disabled'}")
        elif cmd == "break":
            if not rest:
                print("usage: break <hex addr>")
                continue
            addr = int(rest[0], 16)
            dbg.breakpoints.add(addr)
            print(f"breakpoint set at 0x{addr:06X}")
        elif cmd == "delete":
            if not rest:
                print("usage: delete <hex addr>")
                continue
            addr = int(rest[0], 16)
            dbg.breakpoints.discard(addr)
            print(f"breakpoint removed at 0x{addr:06X}")
        elif cmd == "breakpoints":
            if not dbg.breakpoints:
                print("(none)")
            for addr in sorted(dbg.breakpoints):
                print(f"  0x{addr:06X}")
        elif cmd == "regs":
            dbg.print_status()
        elif cmd == "mem":
            if not rest:
                print("usage: mem <hex addr> [len]")
                continue
            addr = int(rest[0], 16)
            length = int(rest[1]) if len(rest) > 1 else 16
            try:
                data = dbg.emu.mem_read(addr, length)
                print(" ".join(f"{b:02X}" for b in data))
            except uc.UcError as e:
                print(f"read failed: {e}")
        elif cmd == "press":
            if not rest:
                print(f"usage: press <BUTTON>  (one of: {', '.join(InteractiveFrontPanel.BUTTONS)})")
                continue
            try:
                dbg.front_panel.set_button(rest[0], True)
                print(f"pressed {rest[0].upper()} - {dbg.front_panel.status()}")
            except KeyError:
                print(f"unknown button {rest[0]!r} - see `buttons`")
        elif cmd == "release":
            if not rest:
                print(f"usage: release <BUTTON>  (one of: {', '.join(InteractiveFrontPanel.BUTTONS)})")
                continue
            try:
                dbg.front_panel.set_button(rest[0], False)
                print(f"released {rest[0].upper()} - {dbg.front_panel.status()}")
            except KeyError:
                print(f"unknown button {rest[0]!r} - see `buttons`")
        elif cmd == "buttons":
            print(dbg.front_panel.status())
            for name in InteractiveFrontPanel.BUTTONS:
                print(f"  {name}")
        elif cmd == "serial":
            if not rest:
                print("usage: serial <text>  (\\n \\r \\t \\0 \\\\ escapes supported)")
                continue
            text = decode_escapes(rest[0])
            dbg.uart.inject(text)
            print(f"queued {text!r} ({len(text)} bytes) - {dbg.uart.status()}")
        elif cmd == "uart":
            print(dbg.uart.status())
        elif cmd == "outgoing":
            if rest and rest[0].lower() == "clear":
                n = dbg.uart.clear_outgoing()
                print(f"cleared {n} outgoing byte(s)")
                continue
            text = dbg.uart.outgoing_text()
            print(text if text else "(nothing written yet)")
        elif cmd == "diag":
            if not dbg.diag.lines:
                print("(no diagnostic text captured yet)")
            for line_text in dbg.diag.lines:
                print(f"  {line_text!r}")
        else:
            print(f"unknown command {cmd!r} - type `help`")


if __name__ == "__main__":
    main()
