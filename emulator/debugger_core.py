"""
Shared debugger engine for both `interactive.py` (REPL) and `tui.py`
(Textual dashboard) - the CPU/memory/stub setup, stepping/running,
register/instruction formatting, and command dispatch all live here so
the two front ends can't drift apart. Each front end supplies its own
`output` callable (`print` for the REPL, a widget-append function for
the TUI) - every line this module or the stubs it installs wants to
show goes through that one callable, never a raw `print()` buried
somewhere, so a Textual app (which fully owns the terminal) never gets
its display corrupted by a stray direct print.
"""
import ctypes
import re

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
    def __init__(self, args, output=print, on_tx=None):
        self.args = args
        self.output = output
        self.emu = uc.Uc(uc.UC_ARCH_X86, uc.UC_MODE_16)
        self.count = 0
        self.breakpoints = set()
        self.trace = False
        self.continue_length = args.continue_length
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
        self.md.detail = True  # needed to inspect memory operands for `trace`
        self.ticker = TickScheduler(args.tick_interval) if args.tick_interval else None
        self.diag = DiagnosticTextCapture(sink=output)
        self.front_panel = InteractiveFrontPanel()
        # `on_tx`: optional live per-byte callback for a front end that
        # wants to display outgoing serial data as it happens (e.g.
        # tui.py's dedicated outgoing panel) rather than only on demand
        # via `uart.outgoing_text()`.
        self.uart = InteractiveUartMock(sink=output, on_tx=on_tx)
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
            self.output(f"{ANSI_GRAY}{self.trace_line()}{ANSI_RESET}")
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
        insn, _ = self._decode_current()
        return f"{insn.mnemonic} {insn.op_str}".strip() if insn else "<undecodable>"

    def snapshot(self):
        """Everything needed to render a status block/panel, as plain
        data rather than pre-formatted text - the REPL and the TUI each
        format this their own way (a text block vs. a table widget)."""
        phys, cs, ip = self.physical_ip()
        regs = {name: self.emu.reg_read(const) for name, const in REG_CONST.items()}
        fl = self.emu.reg_read(x86.UC_X86_REG_EFLAGS)
        stop_reason, self._stop_reason = self._stop_reason, None
        return {
            "count": self.count, "cs": cs, "ip": ip, "phys": phys,
            "regs": regs, "flags": flags_str(fl),
            "front_panel": self.front_panel.status(),
            "uart": self.uart.status(),
            "instruction": self.current_instruction_text(),
            "stop_reason": stop_reason,
        }

    def status_lines(self):
        """REPL-style multi-line text block - see `snapshot()` for the
        structured version a TUI widget would want instead."""
        s = self.snapshot()
        r = s["regs"]
        lines = [
            f"[{s['count']} instrs]  CS:IP={s['cs']:04X}:{s['ip']:04X}  "
            f"(phys 0x{s['phys']:06X})   FLAGS: {s['flags']}",
            f"  AX={r['AX']:04X} BX={r['BX']:04X} CX={r['CX']:04X} DX={r['DX']:04X}"
            f"  SI={r['SI']:04X} DI={r['DI']:04X} BP={r['BP']:04X} SP={r['SP']:04X}",
            f"  DS={r['DS']:04X} ES={r['ES']:04X} SS={r['SS']:04X}"
            f"  |  {s['front_panel']}  |  {s['uart']}",
            f"  -> {s['instruction']}",
        ]
        if s["stop_reason"]:
            lines.append(f"  [stopped: {s['stop_reason']}]")
        return lines

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
                     `incoming`/`outgoing`)
  incoming           show the actual byte contents still queued for RX
                     (not yet consumed by a real read)
  outgoing           show every byte captured on the (write) side of
                     the same mock UART data register - this overlaps
                     with `diag` today since write_readout_port_byte is
                     the only confirmed writer of that address so far.
                     Real \\r/\\n/tab bytes print as real line breaks/
                     tabs, not escaped text
  outgoing clear     discard everything captured so far on that side
  diag               show all captured diagnostic-text lines so far
  help, ?            show this text
  quit, exit, q      leave the debugger
"""


class QuitRequested(Exception):
    """Raised by dispatch_command for quit/exit/q - both front ends
    catch this rather than checking a magic return value."""


def parse_command_line(line):
    """Split a raw input line into (cmd, rest_args) - `serial` is
    special-cased to take the rest of the line as raw text (its own
    \\n/\\r/\\t escapes, not shell quoting): `shlex.split` would
    silently eat a literal backslash outside quotes before
    `decode_escapes` ever saw it. No `.strip()` on the payload itself -
    only `split()`'s own leading-whitespace skip, so a genuine trailing
    `\\r` the user typed isn't silently eaten either."""
    import shlex
    first_split = line.split(None, 1)
    if not first_split:
        return None, []
    if first_split[0].lower() == "serial":
        return "serial", ([first_split[1]] if len(first_split) > 1 else [])
    try:
        parts = shlex.split(line)
    except ValueError as e:
        raise ValueError(f"parse error: {e}") from e
    return parts[0].lower(), parts[1:]


def dispatch_command(dbg, line):
    """Execute one already-typed command line against `dbg` and return
    the response as a list of lines - the single source of truth for
    command behavior, shared by interactive.py's REPL loop and tui.py's
    input handler. Any *streaming* output during a step/run (trace
    lines, diagnostic text, serial events) already went out through
    `dbg.output` by the time this returns - this return value is just
    the final response line(s) for the command itself.

    Raises QuitRequested for quit/exit/q; raises ValueError for a
    malformed command (bad hex, missing argument, etc.) with a message
    suitable for direct display."""
    cmd, rest = parse_command_line(line)
    if cmd is None:
        return []

    if cmd in ("quit", "exit", "q"):
        raise QuitRequested()
    if cmd in ("help", "?"):
        return HELP.splitlines()
    if cmd == "step":
        n = int(rest[0]) if rest else 1
        dbg.step(n)
        return dbg.status_lines()
    if cmd == "run":
        n = int(rest[0]) if rest else 100000
        dbg.run(n)
        return dbg.status_lines()
    if cmd in ("continue", "c"):
        lines = []
        if rest:
            dbg.continue_length = int(rest[0])
            lines.append(f"continue length set to {dbg.continue_length}")
        dbg.run(dbg.continue_length)
        return lines + dbg.status_lines()
    if cmd == "trace":
        if not rest or rest[0].lower() not in ("on", "off"):
            return ["usage: trace on|off"]
        dbg.trace = rest[0].lower() == "on"
        return [f"trace {'enabled' if dbg.trace else 'disabled'}"]
    if cmd == "break":
        if not rest:
            return ["usage: break <hex addr>"]
        addr = int(rest[0], 16)
        dbg.breakpoints.add(addr)
        return [f"breakpoint set at 0x{addr:06X}"]
    if cmd == "delete":
        if not rest:
            return ["usage: delete <hex addr>"]
        addr = int(rest[0], 16)
        dbg.breakpoints.discard(addr)
        return [f"breakpoint removed at 0x{addr:06X}"]
    if cmd == "breakpoints":
        if not dbg.breakpoints:
            return ["(none)"]
        return [f"  0x{addr:06X}" for addr in sorted(dbg.breakpoints)]
    if cmd == "regs":
        return dbg.status_lines()
    if cmd == "mem":
        if not rest:
            return ["usage: mem <hex addr> [len]"]
        addr = int(rest[0], 16)
        length = int(rest[1]) if len(rest) > 1 else 16
        try:
            data = dbg.emu.mem_read(addr, length)
            return [" ".join(f"{b:02X}" for b in data)]
        except uc.UcError as e:
            return [f"read failed: {e}"]
    if cmd == "press":
        if not rest:
            return [f"usage: press <BUTTON>  (one of: {', '.join(InteractiveFrontPanel.BUTTONS)})"]
        try:
            dbg.front_panel.set_button(rest[0], True)
            return [f"pressed {rest[0].upper()} - {dbg.front_panel.status()}"]
        except KeyError:
            return [f"unknown button {rest[0]!r} - see `buttons`"]
    if cmd == "release":
        if not rest:
            return [f"usage: release <BUTTON>  (one of: {', '.join(InteractiveFrontPanel.BUTTONS)})"]
        try:
            dbg.front_panel.set_button(rest[0], False)
            return [f"released {rest[0].upper()} - {dbg.front_panel.status()}"]
        except KeyError:
            return [f"unknown button {rest[0]!r} - see `buttons`"]
    if cmd == "buttons":
        return [dbg.front_panel.status()] + [f"  {name}" for name in InteractiveFrontPanel.BUTTONS]
    if cmd == "serial":
        if not rest:
            return ["usage: serial <text>  (\\n \\r \\t \\0 \\\\ escapes supported)"]
        text = decode_escapes(rest[0])
        dbg.uart.inject(text)
        return [f"queued {text!r} ({len(text)} bytes) - {dbg.uart.status()}"]
    if cmd == "uart":
        return [dbg.uart.status()]
    if cmd == "incoming":
        text = dbg.uart.incoming_text()
        return [text if text else "(RX queue empty)"]
    if cmd == "outgoing":
        if rest and rest[0].lower() == "clear":
            n = dbg.uart.clear_outgoing()
            return [f"cleared {n} outgoing byte(s)"]
        text = dbg.uart.outgoing_text()
        return [text if text else "(nothing written yet)"]
    if cmd == "diag":
        if not dbg.diag.lines:
            return ["(no diagnostic text captured yet)"]
        return [f"  {line_text!r}" for line_text in dbg.diag.lines]

    return [f"unknown command {cmd!r} - type `help`"]
