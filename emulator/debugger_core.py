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
import os
import queue
import re
import time

import capstone
import unicorn as uc
from unicorn import x86_const as x86

import memory_map as mm
from timer import TickScheduler
from io_stubs import (CommPresenceProbe, DiagCommLatchLoopback, DiagnosticTextCapture,
                       DISPLAY_CHIP_STUBS, COMM_OPTION_STUBS, FRONT_PANEL_STUBS,
                       FixedByteRead, InteractiveFrontPanel, InteractiveUartMock,
                       InteractiveDipSwitches, AccessCounter, DisplayChipIrqStub,
                       ANSI_GRAY, ANSI_RESET, seed_comm_nvram_defaults)

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
    # How often (wall-clock) a long run offers `on_progress` a chance to
    # refresh a live display - checked only every PROGRESS_CHECK_EVERY
    # instructions (a cheap modulo test), not every single one, so this
    # doesn't add a per-instruction `time.monotonic()` call to the hot
    # path. 100ms is plenty smooth for a register readout a human is
    # watching, without flooding a terminal repaint faster than that.
    PROGRESS_CHECK_EVERY = 1000
    PROGRESS_INTERVAL_SECONDS = 0.1

    # How often (instruction count) the speed throttle re-checks wall-
    # clock time against the target rate - same reasoning as the
    # progress check above: a `time.monotonic()` call on every single
    # instruction would itself meaningfully slow the hot path down.
    SPEED_CHECK_EVERY = 1000

    def __init__(self, args, output=print, on_tx=None, on_progress=None):
        self.args = args
        self.output = output
        self.count = 0
        self.breakpoints = set()
        self.trace = False
        self.speed_limit = None  # instructions/second cap, None = unthrottled (as fast as possible)
        self._speed_start_time = None
        self._speed_start_count = None
        self.continue_length = args.continue_length
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
        self.md.detail = True  # needed to inspect memory operands for `trace`
        # `on_tx`: optional live per-byte callback for a front end that
        # wants to display outgoing serial data as it happens (e.g.
        # tui.py's dedicated outgoing panel) rather than only on demand
        # via `uart.outgoing_text()`. Kept as an instance attribute (not
        # just a local passed into InteractiveUartMock's constructor)
        # so `reset()` can wire it into the freshly-recreated uart mock
        # too.
        self._on_tx = on_tx
        # `on_progress`: optional callback fired periodically (wall-
        # clock throttled, see PROGRESS_INTERVAL_SECONDS) during any
        # step/run/continue, for a front end that wants to show live
        # register values while a long run is still in flight (e.g.
        # tui.py refreshing its panel) rather than only once it stops.
        self.on_progress = on_progress
        self._last_progress_time = time.monotonic()
        # `live` mode (see `run_live`): the CPU ticks continuously in
        # short bursts instead of one long emu_start(), so a front end
        # can keep taking input (button presses, serial injection,
        # arbitrary typed commands) while it runs - each queued action
        # is applied between bursts, the only point where nothing is
        # concurrently touching the Unicorn engine.
        self.live = False
        self._live_running = False  # true for as long as run_live's own thread is inside its loop
        self.pending_actions = queue.Queue()
        self._stop_reason = None
        self._boot()

    def _boot(self):
        """(Re)creates the CPU/memory/stubs from scratch and leaves the
        CPU sitting at the reset vector - everything a real power-on
        does, factored out of `__init__` so `reset()` (user request:
        "and you never added my reset button" - a way to restart a run
        from scratch without relaunching the app) can redo it in place.
        Deliberately leaves alone anything that's a debugger/session
        setting rather than emulated-machine state - `args`, `output`,
        breakpoints, `trace`, the speed cap, `continue_length`, and
        `live`/`pending_actions` (the caller is expected to have already
        called `stop_live()` before a reset, same as it would before
        tearing down for any other reason)."""
        self.emu = uc.Uc(uc.UC_ARCH_X86, uc.UC_MODE_16)
        self.count = 0
        self._stop_reason = None
        self.ticker = TickScheduler(self.args.tick_interval) if self.args.tick_interval else None
        self.diag = DiagnosticTextCapture(sink=self.output)
        self.front_panel = InteractiveFrontPanel()
        self.dip_switches = InteractiveDipSwitches()
        self.uart = InteractiveUartMock(sink=self.output, on_tx=self._on_tx)
        self.access_counter = AccessCounter()
        self._low_ram_buffer = None
        self._setup_memory()
        self._setup_stubs()
        # Only I/O port in/out counting installs unconditionally here -
        # see AccessCounter's docstring for why memory read/write
        # tracking is opt-in via `access mem on` instead: a MEM_READ
        # hook that overlaps the stack corrupts real CPU execution in
        # this Unicorn version (confirmed with a minimal repro), so it
        # must never be silently active on every run.
        self.access_counter.install_io(self.emu, uc)
        self.emu.hook_add(uc.UC_HOOK_CODE, self._on_code, None)
        self.emu.hook_add(uc.UC_HOOK_MEM_INVALID, self._on_invalid, None)
        self.emu.reg_write(x86.UC_X86_REG_CS, 0xF000)
        self.emu.reg_write(x86.UC_X86_REG_IP, 0xFFF0)

    def reset(self):
        """The actual reset - rebuilds the CPU/memory/stubs from
        scratch via `_boot()`. Refuses while `live` mode's own thread
        is still inside `run_live`'s loop (`_live_running`, distinct
        from `live` itself - `stop_live()` only requests the loop exit
        on its *next* check, it doesn't wait for it): rebuilding `self.
        emu` while another thread might still be mid-`emu_start` on the
        old one is exactly the kind of cross-thread engine access
        that's unsafe elsewhere in this codebase too. Raises ValueError
        (already handled uniformly by both front ends) rather than
        silently blocking or racing."""
        if self._live_running:
            raise ValueError("live mode is still running - 'live off' "
                              "first, wait for it to actually stop, then reset")
        self._boot()

    def dump_memory(self, path=None):
        """Writes a flat binary snapshot of the entire mapped address
        space - every ROM and RAM region from `memory_map.all_regions()`
        - to `path`, plus two companion text files (same path, different
        extensions): `.mem` with the full memory/IO access log
        (`AccessCounter.summary()`, every touched address/port, not
        just the top few), and `.log` with every diagnostic-text line
        captured so far (`self.diag.lines` - see `DiagnosticTextCapture`'s
        docstring for exactly which writes this is, and its own real
        caveats). User requests 2026-09-17: "there is a reason I want a
        log of 'hot' memory addresses... I would like dump updated to
        put dumps in ..\\scratchpad\\dump\\{instrs}.bin with an access
        log at ..\\scratchpad\\dump\\{instrs}.mem"; then, asking what
        writes the outgoing UART data register: "it would be nice to
        have those messages logged somewhere" - answered by the same
        `.log` file, since `write_readout_port_byte` (the confirmed
        sole writer of that register) is exactly what `diag.lines`
        already captures.

        `path=None` (the default) uses the same convention, named after
        the current instruction count - `../scratchpad/dump/<count>.
        bin`, relative to wherever the tool is run from (this project's
        own emulator/ directory, matching every dump the user has taken
        by hand so far). Each region's live bytes are placed at its own
        absolute physical address within the `.bin` (gaps between
        regions zero-filled), so file offset N is exactly what the CPU
        currently sees at physical address N - the same absolute-
        addressing convention every other artifact in this project
        already uses, rather than a compacted concatenation that would
        need a separate offset table to make sense of. ROM is included
        even though it's static (identical to the source `.bin` files
        unless code has self-modified it, which shouldn't happen) -
        user request 2026-09-17, correcting an earlier ROM-excluded
        version of this feature: "i would expect this memory dump to
        include the roms." Originally from `TODO.md`'s "Notes from the
        architect": "I would like a memory dump option... it should
        output the entire ram representation to a binary file."
        Returns (bytes_written, region_count, bin_path, mem_path,
        log_path) for the caller to report."""
        if path is None:
            path = f"../scratchpad/dump/{self.count}.bin"
        regions = mm.all_regions(self.args.revision)
        end = max(r.start + r.size for r in regions)
        buf = bytearray(end)
        for region in regions:
            buf[region.start:region.start + region.size] = self.emu.mem_read(region.start, region.size)
        parent = os.path.dirname(path)
        if parent:
            os.makedirs(parent, exist_ok=True)
        with open(path, "wb") as f:
            f.write(buf)

        base = os.path.splitext(path)[0]
        mem_path = base + ".mem"
        with open(mem_path, "w") as f:
            f.write(f"# access log at {self.count} instructions\n")
            f.write(self.access_counter.status() + "\n")
            for line in self.access_counter.summary(top=None):
                f.write(line + "\n")

        log_path = base + ".log"
        with open(log_path, "w") as f:
            f.write(f"# diagnostic-text log at {self.count} instructions "
                     f"({len(self.diag.lines)} line(s))\n")
            for line in self.diag.lines:
                f.write(line + "\n")

        return len(buf), len(regions), path, mem_path, log_path

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
        # See io_stubs.seed_comm_nvram_defaults's docstring - without
        # this, comm-ROM code that dereferences [0x6D6]/[0x6E2] would
        # corrupt the IVT instead of reaching the Interrupt Mask Latch,
        # since this emulator has no NVRAM-persistence model and a
        # fresh run starts that RAM at zero.
        seed_comm_nvram_defaults(self.emu)

    def _setup_stubs(self):
        for address, value, label in DISPLAY_CHIP_STUBS + COMM_OPTION_STUBS + FRONT_PANEL_STUBS:
            FixedByteRead(address, value, label).install(self.emu, uc)
        self.front_panel.install(self.emu, uc)
        if self.args.comm_installed:
            CommPresenceProbe().install(self.emu, uc)
        # Installed after COMM_OPTION_STUBS's fixed comm_stat baseline
        # so it only adjusts bit 0x80 on top - see its own docstring
        # for the disassembly-derived reasoning (selftest_comm_readback).
        DiagCommLatchLoopback().install(self.emu, uc)
        # Installed after DISPLAY_CHIP_STUBS's fixed 0x41000 baseline so
        # it only adds the pending-flag side effect on top - see its
        # own docstring for the disassembly-derived reasoning
        # (selftest_display_irq_active / "MI : Display controller :
        # TIMEOUT").
        DisplayChipIrqStub().install(self.emu, uc)
        # Installed after COMM_OPTION_STUBS's fixed comm_stat/comm_param
        # values so it only adjusts the switch bit positions on top of
        # that baseline - see InteractiveDipSwitches's docstring.
        self.dip_switches.install(self.emu, uc)
        # Installed after COMM_OPTION_STUBS's fixed comm_stat value so
        # it only adjusts bit1 on top of that baseline - see
        # InteractiveUartMock's docstring.
        self.uart.install(self.emu, uc)
        self.diag.install(self.emu, uc)

    def _on_code(self, uc_eng, address, size, user_data):
        # Found 2026-09-18 investigating a user report that SELECT C1/C2
        # inverts UART debug output: without this check, Unicorn just
        # silently returns from `emu_start` when it reaches a real
        # `hlt` opcode (0xF4) - no error, no stop reason, and (worse)
        # IP already advanced *past* it, as if it had executed like any
        # other instruction. Both the ordinary "PRESS MENU KEYS TO
        # CONTINUE" end-of-boot idle halt *and* the firmware's own
        # `assert_and_halt` panic trap (`halt_cpu`, physical 0xF1611 -
        # see `docs/hardware-io/shift-register-and-assert.md`) hit this.
        # Nothing told the caller a halt had happened, so a subsequent
        # `run`/`step` call (this project's own trace scripts did
        # exactly this, repeatedly) resumed from a CPU that real
        # hardware considers permanently stopped - Unicorn does not
        # cleanly support "restart after hlt" without re-seeding a
        # valid instruction stream, so continuing produced nonsense
        # register state (garbage CS/DS/ES) that looked exactly like a
        # wild jump into unmapped memory. That's what caused an entire
        # investigation into a nonexistent "crash." Unicorn's
        # UC_HOOK_INSN doesn't support hooking HLT directly on this
        # version (only IN/OUT/SYSCALL/SYSENTER/CPUID - confirmed by a
        # UC_ERR_ARG trying it), so this checks the raw opcode byte
        # here instead, in the one hook that already fires before every
        # instruction executes - stopping *before* the real `hlt` runs
        # means IP is left pointing *at* it, not past it.
        if uc_eng.mem_read(address, 1) == b"\xf4":
            self._stop_reason = f"halted (hlt) at 0x{address:06X}"
            uc_eng.emu_stop()
            return
        self.count += 1
        if self.trace:
            self.output(f"{ANSI_GRAY}{self.trace_line()}{ANSI_RESET}")
        if self.ticker is not None:
            self.ticker.step(uc_eng)
        self.uart.pump_paced(self.count)
        if self.on_progress and self.count % self.PROGRESS_CHECK_EVERY == 0:
            now = time.monotonic()
            if now - self._last_progress_time >= self.PROGRESS_INTERVAL_SECONDS:
                self._last_progress_time = now
                self.on_progress()
        if self.speed_limit and self.count % self.SPEED_CHECK_EVERY == 0:
            self._throttle_speed()
        if address in self.breakpoints:
            self._stop_reason = f"breakpoint hit at 0x{address:06X}"
            uc_eng.emu_stop()

    def _throttle_speed(self):
        """Sleeps just enough to bring the average rate since `speed_
        limit` was last set back down to the target - checked only
        every SPEED_CHECK_EVERY instructions, not every one, so this
        doesn't add per-instruction overhead. Never speeds anything up
        (a run that's behind schedule, e.g. because the host was briefly
        busy elsewhere, just proceeds at full speed until it catches up
        rather than trying to "make up time")."""
        now = time.monotonic()
        elapsed = now - self._speed_start_time
        expected_min_elapsed = (self.count - self._speed_start_count) / self.speed_limit
        if elapsed < expected_min_elapsed:
            time.sleep(expected_min_elapsed - elapsed)

    def set_speed(self, instructions_per_second):
        """`None` (or 0) means unthrottled - as fast as the host can
        run it, the default and previously the only mode. Any positive
        number caps the average rate, useful for watching `trace`
        output or live register updates at a human-readable pace
        instead of them flashing past. Resets the throttle's own
        reference point (`_speed_start_time`/`_count`) each time it's
        called, so changing speed mid-run doesn't try to "catch up" or
        "slow down" based on time elapsed under a *different* limit."""
        self.speed_limit = instructions_per_second or None
        self._speed_start_time = time.monotonic()
        self._speed_start_count = self.count

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

    def interrupts_status(self):
        """Interrupt-related state worth watching live: the CPU's own
        IF flag, the Option Interrupt Mask Latch's 4 outputs (`0D`-`3D`
        at `0x406F8`-`0x406FB` - `0D`=DR/RX mask, `1D`=TBRE/TX mask,
        per `docs/comm-rom/rs232-early-investigation.md`), the i8251
        chip's own Receive/Transmit-Enable command bits and resulting
        RxRDY/TxRDY pin signals, and the synthetic INT2/NMI ticker's
        fired/skipped counts."""
        fl = self.emu.reg_read(x86.UC_X86_REG_EFLAGS)
        try:
            mask_bytes = self.emu.mem_read(0x406F8, 4)
        except uc.UcError:
            mask_bytes = b"\xFF\xFF\xFF\xFF"  # unmapped - shouldn't happen, flagged visibly
        chip = self.uart.chip
        return {
            "if_flag": bool(fl & 0x200),
            "mask_0D_dr": mask_bytes[0], "mask_1D_tbre": mask_bytes[1],
            "mask_2D": mask_bytes[2], "mask_3D_diag": mask_bytes[3],
            "uart_rxen": bool(chip.command & 0x04),
            "uart_txen": bool(chip.command & 0x01),
            "uart_rxrdy": bool(chip.rxrdy_r()),
            "uart_txrdy": bool(chip.txrdy_r()),
            "uart_rx_ready_bit": bool(chip.status & 0x02),   # STATUS_RX_READY
            "uart_tx_ready_bit": bool(chip.status & 0x01),   # STATUS_TX_READY
            "uart_tx_empty_bit": bool(chip.status & 0x04),   # STATUS_TX_EMPTY
            "int2_fired": self.ticker.fired if self.ticker else None,
            "int2_skipped": self.ticker.skipped if self.ticker else None,
        }

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
            "interrupts": self.interrupts_status(),
            "instruction": self.current_instruction_text(),
            "stop_reason": stop_reason,
        }

    @staticmethod
    def _interrupts_line(iv):
        int2 = f"INT2 {iv['int2_fired']}/{iv['int2_skipped']}" if iv["int2_fired"] is not None else "INT2 off"
        return (f"  IF={int(iv['if_flag'])}  mask 0D={iv['mask_0D_dr']:02X} 1D={iv['mask_1D_tbre']:02X} "
                f"2D={iv['mask_2D']:02X} 3D={iv['mask_3D_diag']:02X}  "
                f"UART RxEN={int(iv['uart_rxen'])} TxEN={int(iv['uart_txen'])} "
                f"RxRDY={int(iv['uart_rxrdy'])} TxRDY={int(iv['uart_txrdy'])}  "
                f"[RX_READY={int(iv['uart_rx_ready_bit'])} TX_READY={int(iv['uart_tx_ready_bit'])} "
                f"TX_EMPTY={int(iv['uart_tx_empty_bit'])}]  {int2}")

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
            self._interrupts_line(s["interrupts"]),
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

    def queue_action(self, fn):
        """Thread-safe: enqueue a zero-arg callable to run on
        `run_live`'s own thread, between bursts - the only safe point
        to touch debugger/engine state from another thread while the
        CPU is ticking in the background, since nothing may call into
        Unicorn concurrently with an active `emu_start`."""
        self.pending_actions.put(fn)

    def run_live(self, burst=2000):
        """Runs continuously in short bursts (instead of one long
        emu_start call) so a front end can keep accepting front-panel/
        serial/console input while the CPU ticks in the background -
        user request 2026-09-17: "can we have a live mode where I can
        still push buttons and send serial commands but the system
        ticks away in the background." Anything queued via
        `queue_action` from another thread is drained and run here, in
        the gap between one burst finishing and the next starting -
        the only point at which nothing is running, so it's always
        safe regardless of what the action touches. `live` is a plain
        bool, safe to clear from another thread (see `stop_live`)
        without a lock since a single attribute read/write is already
        atomic under the GIL. Stops on `stop_live()` or as soon as a
        burst sets a stop reason (breakpoint hit, fault, etc.) - same
        as `run`."""
        self.live = True
        self._live_running = True
        self._stop_reason = None
        try:
            while self.live:
                while True:
                    try:
                        action = self.pending_actions.get_nowait()
                    except queue.Empty:
                        break
                    action()
                self.run(burst)
                if self._stop_reason:
                    self.live = False
        finally:
            self._live_running = False

    def stop_live(self):
        self.live = False


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
  speed [n|max]      show or set the execution rate cap, in
                     instructions/second - `speed max` (the default)
                     runs as fast as the host can go; a lower number
                     slows any step/run/continue down to a human-
                     watchable pace (e.g. for reading `trace` output
                     or live register updates as they happen)
  reset              reboot the CPU/memory/stubs back to power-up state
                     in place (the reset vector, count=0, front panel/
                     DIP switches/UART back to their idle defaults) -
                     breakpoints, trace mode, and the speed cap are
                     left alone. Refused while `live` mode is running -
                     stop it first (see `live`)
  dump [path]        write the entire mapped address space (every ROM
                     and RAM region) to a flat binary file - default
                     path is `../scratchpad/dump/<instruction count>.
                     bin` - each region at its own real physical
                     address (gaps zero-filled) so file offset N is
                     what's at physical address N. Also writes 2
                     companion text files (same path, different
                     extensions): `.mem` has the full memory/IO access
                     log (every touched address/port, not just the top
                     few shown by `access`); `.log` has every
                     diagnostic-text line captured so far (see `diag`)
  access             show every memory address and I/O port touched so
                     far (write/in/out counts always run automatically;
                     read counts only appear for a range added via
                     `watch` - see below). NOTE: a stubbed register's
                     own read-override write-back (see io_stubs.
                     AccessCounter's docstring) shows up as a "write"
                     here too - a nonzero write count only means real
                     firmware wrote there if the address isn't one of
                     this project's own read-stubs. Reset by `reset`/
                     a fresh run
  access clear       zero every access counter without touching which
                     range(s) `watch` is covering
  watch <start> <end> track *reads* to this hex physical-address range
                     too (writes/IO always run) - opt-in and NOT safe
                     to leave on for a run you need to trust: a real,
                     confirmed Unicorn bug corrupts CPU execution if
                     the watched range ever overlaps wherever the
                     stack happens to be (see io_stubs.AccessCounter's
                     docstring for the full repro) - use only for a
                     short, targeted diagnostic run
  unwatch            stop tracking reads on every `watch`ed range
  access clear       zero out every access counter without changing
                     which range(s) are being watched
  break <hex addr>   add a breakpoint (physical address, e.g. E0AE9)
  delete <hex addr>  remove a breakpoint
  breakpoints        list current breakpoints
  regs               reprint the register/status block
  mem <hex addr> [n] hex-dump n bytes (default 16) at a physical address.
                     NOTE: this is a raw peek and does not go through
                     any FixedByteRead/InteractiveUartMock/DipSwitches
                     read-hook - those only fire for reads the emulated
                     CPU itself performs during step/run/continue, so a
                     stubbed register will read back its unmodified
                     underlying RAM value here (often 0), not the
                     stubbed value real firmware execution would see
  press <BUTTON>     press a front-panel button (see `buttons`)
  release <BUTTON>   release a front-panel button
  buttons            list button names and current SWB1/SWB2 values
  dip [n] [on|off]   show all 10 comm-option PARAMETERS DIP switches,
                     or set switch n (1-10) on/off (default on). Most
                     switches' individual meaning beyond the confirmed
                     baud-rate nibble is still open - see
                     io_stubs.InteractiveDipSwitches's docstring
  serial <text>      queue <text> as incoming bytes, delivered one at a
                     time (paced by instruction count, see `serial-
                     rate`) into the (experimental) mock UART receive
                     path - see InteractiveUartMock's docstring for
                     what this does and doesn't model. Supports \\n \\r
                     \\t \\0 \\\\ escapes (e.g. `serial AT\\r\\n`) since
                     there's no way to type a real newline into a
                     single input() line
  serial-rate [n]    show or change how many instructions elapse
                     between queued-byte deliveries - lower = faster
                     (more overrun-prone if firmware can't keep up),
                     higher = slower/more forgiving
  uart               show the mock UART's pending RX queue/captured TX
                     bytes (see `incoming`/`outgoing`) plus the i8251
                     core's own decoded mode/command configuration and
                     rx/tx holding registers (see i8251.I8251.describe)
  live [off]         run continuously in short bursts, staying
                     responsive to input the whole time - unlike
                     step/run/continue, which occupy the debugger
                     until they stop, `live` lets you press front-
                     panel buttons/checkboxes, send `serial <text>`,
                     or type any other command while it's ticking;
                     each is queued and applied between bursts, the
                     only point where nothing else is touching the
                     CPU. `live off` (or the TUI's F6) stops it -
                     step/run/continue are refused while it's active
  interrupts         show the CPU's IF flag, the Option Interrupt Mask
                     Latch's 4 outputs, the UART's Rx/TxEN command bits
                     and resulting RxRDY/TxRDY signals, and the
                     synthetic INT2 ticker's fired/skipped counts -
                     also shown continuously in `regs`/the TUI panel
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
    `\\r` the user typed isn't silently eaten either.

    `dump` is special-cased the same way and for the same underlying
    reason - its argument is a Windows file path (this project's only
    supported platform), and `shlex.split` treats a bare backslash as
    an escape character outside quotes, silently mangling `dump
    C:\\Users\\...\\out.bin` into `C:UsersFoo...out.bin` (caught while
    testing the `dump` command itself, not hypothetically)."""
    import shlex
    first_split = line.split(None, 1)
    if not first_split:
        return None, []
    if first_split[0].lower() in ("serial", "dump"):
        return first_split[0].lower(), ([first_split[1]] if len(first_split) > 1 else [])
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
    if cmd == "reset":
        dbg.reset()
        return ["(reset - CPU back at the reset vector)"] + dbg.status_lines()
    if cmd == "dump":
        path = rest[0] if rest else None
        size, n, bin_path, mem_path, log_path = dbg.dump_memory(path)
        return [f"wrote {size} bytes ({n} region(s): ROM+RAM, gaps zero-filled) to {bin_path}",
                f"wrote the access log ({dbg.access_counter.status()}) to {mem_path}",
                f"wrote {len(dbg.diag.lines)} diagnostic-text line(s) to {log_path}"]
    if cmd == "access":
        if rest and rest[0].lower() == "clear":
            dbg.access_counter.clear()
            return ["access counters cleared"]
        return [dbg.access_counter.status()] + dbg.access_counter.summary()
    if cmd == "watch":
        if len(rest) != 2:
            return ["usage: watch <start hex addr> <end hex addr>  "
                     "- see 'help' for why this is opt-in, not automatic"]
        start, end = int(rest[0], 16), int(rest[1], 16)
        dbg.access_counter.install_watch_range(dbg.emu, uc, start, end)
        return [f"watching 0x{start:06X}-0x{end:06X} for reads (see 'access') - "
                 "diagnostic only, don't trust a run's own completion while this is on"]
    if cmd == "unwatch":
        dbg.access_counter.unwatch()
        return ["stopped watching all memory-read ranges "
                 "(write/IO port counts keep running)"]
    if cmd == "live":
        if rest and rest[0].lower() == "off":
            dbg.stop_live()
            return ["stopping live mode..."]
        if dbg.live:
            return ["already in live mode"]
        dbg.run_live()
        return ["(live mode ended)"] + dbg.status_lines()
    if cmd == "trace":
        if not rest or rest[0].lower() not in ("on", "off"):
            return ["usage: trace on|off"]
        dbg.trace = rest[0].lower() == "on"
        return [f"trace {'enabled' if dbg.trace else 'disabled'}"]
    if cmd == "speed":
        if not rest:
            current = f"{dbg.speed_limit} instructions/sec" if dbg.speed_limit else "unlimited (max)"
            return [f"current speed: {current}  (usage: speed <n>|max)"]
        if rest[0].lower() == "max":
            dbg.set_speed(None)
            return ["speed set to unlimited (max)"]
        try:
            n = int(rest[0])
        except ValueError:
            return ["usage: speed <n>|max  (n = instructions/second)"]
        if n <= 0:
            return ["speed must be a positive instruction/second count, or `max`"]
        dbg.set_speed(n)
        return [f"speed set to {n} instructions/sec"]
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
    if cmd == "dip":
        if not rest:
            return [dbg.dip_switches.status()]
        try:
            number = int(rest[0])
            on = rest[1].lower() in ("on", "1", "true") if len(rest) > 1 else True
        except (ValueError, IndexError):
            return ["usage: dip <1-10> [on|off]  (or `dip` alone to show all 10)"]
        if not 1 <= number <= 10:
            return ["switch number must be 1-10"]
        dbg.dip_switches.set_switch(number, on)
        return [f"switch {number} -> {'ON' if on else 'OFF'} - {dbg.dip_switches.status()}"]
    if cmd == "serial":
        if not rest:
            return ["usage: serial <text>  (\\n \\r \\t \\0 \\\\ escapes supported)"]
        text = decode_escapes(rest[0])
        dbg.uart.inject(text)
        return [f"queued {text!r} ({len(text)} bytes) - {dbg.uart.status()}"]
    if cmd == "serial-rate":
        if not rest:
            return [f"current pacing: {dbg.uart.instructions_per_byte} instructions/byte "
                    f"(usage: serial-rate <n> to change - lower = faster/less realistic "
                    f"servicing time, higher = slower/more overrun-prone)"]
        dbg.uart.instructions_per_byte = int(rest[0])
        return [f"pacing set to {dbg.uart.instructions_per_byte} instructions/byte"]
    if cmd == "uart":
        return [dbg.uart.status(), dbg.uart.chip_detail()]
    if cmd == "interrupts":
        return [dbg._interrupts_line(dbg.interrupts_status())]
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
