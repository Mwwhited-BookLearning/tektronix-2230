"""
I/O register stubs - see emulator/docs/design.md's "I/O port and MMIO
stub strategy". Each stub models just enough real hardware behavior to
let a specific piece of firmware logic reach a real, meaningful
conclusion instead of reading inert RAM - see individual docstrings for
which real register(s) each one is standing in for and why.
"""
import collections

from unicorn import x86_const as x86

# Shared ANSI colors so interactive.py's trace scroll (gray - meant to
# fade into the background) stays visually distinct from diagnostic/
# serial traffic (white - meant to stand out) when both are streaming
# at once. Plain terminal escapes, no color library dependency; if a
# terminal doesn't support them the raw codes are relatively harmless
# noise rather than a crash.
ANSI_GRAY = "\033[90m"
ANSI_WHITE = "\033[97m"
ANSI_RESET = "\033[0m"


class FixedByteRead:
    """Forces every read of a single byte address to return a fixed
    value, regardless of what's actually stored there - for registers
    where plain RAM's default "reads back whatever was last written"
    behavior is wrong, because the real hardware doesn't just store
    what the CPU last wrote (a busy/status bit that only real hardware
    clears, or a live external signal the CPU never itself sets).

    Confirmed this actually overrides the value the CPU sees for the
    *same* read (not just a future one) - `mem_write` from inside a
    `UC_HOOK_MEM_READ` callback lands before the instruction finishes
    fetching, verified directly with a throwaway `mov al,[addr]` test
    before relying on this for every stub below."""

    def __init__(self, address, value, label=""):
        self.address = address
        self.value = value & 0xFF
        self.label = label or f"0x{address:X}"

    def install(self, emu, uc_module):
        emu.hook_add(uc_module.UC_HOOK_MEM_READ, self._on_read,
                     None, self.address, self.address)

    def _on_read(self, uc_eng, access, address, size, value, user_data):
        uc_eng.mem_write(address, bytes([self.value]) * size)
        return True


# Registers this project has a real, captured hardware value for (the
# `/DIAGNOSTICS/EXERCISERS/IO/INPUT_PORTS` screen, photographed on both
# physical test units - see MEMORY_MAP.md's "New exerciser screen"
# section) - using the real byte instead of an arbitrary guess wherever
# one exists, per design.md's "start every stub as the value that lets
# the firmware move on" rule.
DISPLAY_CHIP_STUBS = [
    # Design.md's original plan, never actually implemented until now:
    # "reads return 0 (never 'still busy')" - plain RAM already defaults
    # to 0, but only until the firmware itself writes a nonzero
    # trigger/busy bit and then polls waiting for it to clear again;
    # nothing in this emulator models real display-chip hardware
    # clearing it back down without an active force-to-0 hook. This
    # register's read value itself was never the actual "Display
    # controller : TIMEOUT" cause, though - see DisplayChipIrqStub
    # below (installed after this) for the real fix: the read's *side
    # effect* (arming an interrupt that selftest_display_irq_active
    # busy-polls for), not this fixed return value.
    (0x41000, 0x00, "display chip interrupt reset"),
    (0x42000, 0x00, "display chip next frame"),
]

COMM_OPTION_STUBS = [
    # comm_stat (Option Status Latch, U1223) - real captured value,
    # identical on both physical units: 01111101 = 0x7D. Bit map per
    # MEMORY_MAP.md's RS-232 option board section: BD0=PWR INT(1),
    # BD1=INTR+DR(0), BD2=TBRE(1), BD3-5=PARAMETERS DIP 8-10(111),
    # BD6=diagnostic(1), BD7=DCD(0).
    #
    # **Tried a static override to 0xD0 first, 2026-09-17** (user: "can
    # you just hardcode that value for a test pass?") to get past
    # `selftest_comm_readback`'s (0xE20B0) exact-match check - traced
    # through the actual disassembly and found that's mathematically
    # impossible: the check reads this byte *twice* (before and after
    # toggling Interrupt Mask Latch output 3D at 0x406FB 0->1), masks
    # each read to bits 0xC0, shifts the *first* read's mask right by
    # 2, and ORs the two together - `(read1&0xC0)>>2 | (read2&0xC0)`.
    # For a byte that never changes between the two reads, this can
    # only ever produce 0x00/0x50/0xA0/0xF0, never the required 0xD0,
    # for any fixed value at all. Solving backwards from what *would*
    # produce 0xD0 shows it needs bit7 to be 0 on the first read and 1
    # on the second - i.e. this genuinely requires a write-then-
    # readback coupling between 0x406FB and this byte's bit7 (see
    # DiagCommLatchLoopback below), not a static value. Kept at the
    # real captured 0x7D baseline; DiagCommLatchLoopback overlays bit7
    # dynamically on top of it.
    (0x4067C, 0x7D, "comm_stat / Option Status Latch (U1223)"),
    # comm_param (Option Parameter Latch, U1222) - real captured value
    # (one of the two scopes' readings; BD7=UART SDO, a live transmit
    # bit, is the one bit documented to differ instant-to-instant - see
    # MEMORY_MAP.md's resolution of the "differing last bit" puzzle).
    (0x406BC, 0xF8, "comm_param / Option Parameter Latch (U1222)"),
]

FRONT_PANEL_STUBS = [
    # fp_ad_data (U6102, front-panel ADC results buffer) - real
    # captured value from the exerciser screen: 01100010 = 0x62
    # (photo partly obscured, but this is what was legible).
    (0x437FA, 0x62, "fp_ad_data / Front Panel A/D data (U6102)"),
    # fp_intstat (U6103, Main Front Panel Input) - real captured value:
    # 10011011 = 0x9B, identical on both scopes.
    (0x437FB, 0x9B, "fp_intstat / Main Front Panel Input (U6103)"),
]

# SWB1/SWB2 (front-panel button registers) are listed separately from
# FRONT_PANEL_STUBS above - interactive.py replaces these two with
# InteractiveFrontPanel (a live, console-controllable model) instead of
# a fixed constant, so `install_all_fixed_reads` still covers them for
# the plain headless tracer (emu.py), but interactive runs use the
# mutable version instead. Same real captured idle baselines either way
# (VARIABLES.md, 2026-09-13 live-hardware test) - an interactive run
# with nothing pressed behaves identically to the headless default.
FRONT_PANEL_BUTTON_STUBS = [
    (0x43FFA, 0x08, "SWB2 / Front Panel Buffer (U9301)"),
    (0x43FFB, 0x44, "SWB1 / Front Panel Buffer (U9302)"),
]


def install_all_fixed_reads(emu, uc_module, verbose=True):
    """Install every FixedByteRead stub above - the registers this
    project can back with a real captured or documented value, as
    opposed to CommPresenceProbe's coupling behavior below (which needs
    to react to a write, not just return a constant)."""
    for address, value, label in (DISPLAY_CHIP_STUBS + COMM_OPTION_STUBS
                                   + FRONT_PANEL_STUBS
                                   + FRONT_PANEL_BUTTON_STUBS):
        FixedByteRead(address, value, label).install(emu, uc_module)
        if verbose:
            print(f"stubbed {label:45s} @ 0x{address:06X} -> "
                  f"0x{value:02X} (fixed read)")


class CommPresenceProbe:
    """Models detect_comm_option_hw's (physical 0xE75C0) hardware
    presence test: it writes a bit to the confirmed "Time Base Mode
    Register" (MEMORY_MAP.md, physical 0x4007DE) and reads back a bit
    of the confirmed "Acquisition Memory Address Buffer" (physical
    0x4377E), expecting the two to be coupled on real hardware when the
    comm option is installed (some specific wiring quirk the manual
    excerpts read so far don't detail - not needed to model here, only
    the observable behavior matters).

    With no coupling at all (plain RAM, this project's default stub),
    the probe deterministically reads back "not installed" - confirmed
    live 2026-09-16 (see docs/display/vector-display-and-stroke-font.md
    "Live emulation confirms..."). This stub adds the coupling so the
    probe can conclude "installed" instead, matching the real physical
    test units this project has hardware access to (both confirmed
    Option 12 - RS-232 - equipped)."""

    # 0x407DE, not the "0x4007DE" MEMORY_MAP.md's own row header shows -
    # that's a pre-existing typo in the doc (its own body text says
    # "Table 3-1, address 0x407DE" one sentence later, and the real
    # disassembly confirms it: es=0x4000, di=0x7de -> 0x4000*16+0x7de =
    # 0x407DE). Caught here because a wrong 7-hex-digit constant made
    # this stub's hook silently target an address detect_comm_option_hw
    # never touches, so the fix had no observable effect at all until
    # traced back to this typo. Also fixed in MEMORY_MAP.md itself.
    WRITE_ADDR = 0x407DE
    READBACK_ADDR = 0x4377E
    PRESENCE_BIT = 0x1000

    def install(self, emu, uc_module):
        emu.hook_add(uc_module.UC_HOOK_MEM_WRITE, self._on_write,
                     None, self.WRITE_ADDR, self.WRITE_ADDR)

    def _on_write(self, uc_eng, access, address, size, value, user_data):
        bit = self.PRESENCE_BIT if (value & 1) else 0
        current = int.from_bytes(uc_eng.mem_read(self.READBACK_ADDR, 2), "little")
        new_value = (current & ~self.PRESENCE_BIT) | bit
        uc_eng.mem_write(self.READBACK_ADDR, new_value.to_bytes(2, "little"))


class DiagCommLatchLoopback:
    """Couples writes to the Interrupt Mask Latch's diagnostic output
    3D (physical `0x406FB`) into bit `0x80` of the Option Status
    Latch's own readback (`0x4067C`, `comm_stat`) - the same "no
    coupling at all -> deterministically reads back the wrong thing"
    gap `CommPresenceProbe` already fixed for a different register
    pair, found here 2026-09-17 while investigating `selftest_comm_
    readback` (physical `0xE20B0`).

    **Derived from the actual disassembly**: the self-test writes `0`
    then `1` to `0x406FB`, reading `comm_stat` once after each write,
    and combines the two reads as `(read1 & 0xC0) >> 2 | (read2 &
    0xC0)`, requiring the result to be exactly `0xD0` to fully pass.
    Solving that equation backwards (see `COMM_OPTION_STUBS`'s own
    comment on why a *static* `comm_stat` value can never satisfy it -
    confirmed by first trying exactly that) shows it needs bit `0x40`
    fixed at 1 and bit `0x80` to go `0`->`1` exactly when `3D` goes
    `0`->`1`.

    **Known conflict with the real Options manual, found 2026-09-17/18
    - NOT resolved, flagged rather than silently trusted**: the
    Options manual's own Table 7-36 ("RS-232-C Status Buffer
    Functions," see `docs/options.md`/`MEMORY_MAP.md`) names bit
    `0x40` as `DIAG` (Interrupt mask latch `3D` - i.e. the bit this
    class should be toggling, per the manual) and bit `0x80` as
    `/DCD2` (data carrier detect - an unrelated modem-status signal
    with no documented connection to `3D` at all). That's the *opposite*
    of what the disassembly-derived equation above requires. Toggling
    bit `0x40` instead (matching the manual) produces `0x40` for the
    combined result, not `0xD0` - genuinely fails the self-test, not
    just a different passing value. Since bit `0x80` is what actually
    makes the firmware's own self-test pass (confirmed end-to-end: `
    [0x1BF9]`/`[0x1BFA]` both end up set, and the self-test genuinely
    progresses further instead of stopping at "not installed"), this
    class keeps that behavior - but it means either the manual's bit
    assignment doesn't apply exactly as read, or there's a real hardware
    coupling between `3D` and `/DCD2` this project hasn't found yet.
    Worth resolving with a real schematic trace (see `TODO.md`'s
    jumper-hunt item) rather than guessed away.

    **Real bug found and fixed while verifying this**: the first
    version tried to persist the loopback bit by writing it straight
    into `READBACK_ADDR`'s underlying RAM from the `0x406FB` write
    hook - didn't work, confirmed with an isolated unit test (the bit
    never showed up on the next read, at all). Root cause: `FixedByte
    Read`'s own read hook for `comm_stat` unconditionally *overwrites*
    that same underlying byte back to its fixed baseline on every
    single read, before this class's write-side change could ever be
    observed - the same "any stub's own write-back inside a read hook
    can stomp a change no matter when it was made" hazard already
    documented elsewhere in this file, just hit from the opposite
    direction. Fixed by keeping the loopback bit as this instance's
    own plain attribute (`self.bit_set`, exactly like `Interactive
    DipSwitches.switches`) and overlaying it via a *read* hook on
    `READBACK_ADDR` too, installed after `FixedByteRead`'s so it runs
    on top of the baseline instead of getting overwritten by it."""

    WRITE_ADDR = 0x406FB
    READBACK_ADDR = 0x4067C
    LOOPBACK_BIT = 0x80

    def __init__(self):
        self.bit_set = False

    def install(self, emu, uc_module):
        emu.hook_add(uc_module.UC_HOOK_MEM_WRITE, self._on_write,
                     None, self.WRITE_ADDR, self.WRITE_ADDR)
        emu.hook_add(uc_module.UC_HOOK_MEM_READ, self._on_read,
                     None, self.READBACK_ADDR, self.READBACK_ADDR)

    def _on_write(self, uc_eng, access, address, size, value, user_data):
        self.bit_set = bool(value & 1)

    def _on_read(self, uc_eng, access, address, size, value, user_data):
        current = uc_eng.mem_read(address, 1)[0]
        new_value = (current | self.LOOPBACK_BIT) if self.bit_set else (current & ~self.LOOPBACK_BIT)
        uc_eng.mem_write(address, bytes([new_value]) * size)
        return True


class DisplayChipIrqStub:
    """Couples a read of the Display Chip Interrupt Reset register
    (physical `0x41000`, Table 3-1 - see `MEMORY_MAP.md`) into the
    pending-work flag `[0x1AF2]` (`DS=0x41` -> physical `0x1F02`) that
    `INT2_HANDLER_EARLY` (`0xE5D67`) ORs into `[0x1AEE]` (physical
    `0x1EFE`) on every NMI tick - the missing hardware coupling behind
    the "MI : Display controller : TIMEOUT" self-test failure.

    **Traced 2026-09-18, correcting a wrong prior attribution.** The
    project's own `FUNCTIONS.md`/`design.md` previously blamed
    `selftest_display_irq_idle` (`0xE3F2C`) for this message. A fresh
    instruction trace proved that function actually PASSES (`[0x1AEE]`
    is `0` both before and after its own `sti`/`cli` window) and falls
    straight through into a second, separate function, `selftest_
    display_irq_active` (`0xE3F99`) - decoding the two literal
    string-table print calls in its disassembly against `160-3532`'s
    string table (`0xFF7B0` base, offsets `0x4d1`/`0x4d4`/`0x4e9`)
    confirms its FIRST failure block is exactly "MI : Display
    controller : TIMEOUT" (its second, offsets `0x4f2`/`0x4f5`/`0x50a`,
    is a different, not-yet-observed message: "MI : Display
    controller : unable to reset mi[splay controller]").

    That function's real logic: after committing a test vector draw, it
    reads `0x41000` once (a helper at physical `0xE3B1:0x8FA`), then
    busy-polls `[0x1AEE]` for up to 100 iterations with interrupts
    enabled, printing the TIMEOUT message and failing if it never goes
    nonzero. `[0x1AEE]` only ever becomes nonzero via `INT2_HANDLER_
    EARLY` ORing `[0x1AF2]` into it - and nothing before this stub ever
    set `[0x1AF2]`, so the loop always exhausted its 100 tries.

    Sets *both* flags directly on the read (not just `[0x1AF2]`,
    waiting for the next NMI) since this emulator's INT2/NMI ticker
    fires on a synthetic, configurable instruction-count interval (see
    `timer.py`) with no reliable relationship to this test's
    ~100-iteration/~400-instruction polling window - on real hardware
    the interrupt is architecturally guaranteed to arrive well within
    that budget (that's the entire point of a TIMEOUT self-test), so
    waiting on the emulator's own arbitrary tick cadence would make the
    result depend on `--tick-interval` instead of on real firmware
    logic.

    Installed after `DISPLAY_CHIP_STUBS`'s own `FixedByteRead(0x41000,
    0x00, ...)` so the register itself still reads back a plain `0`
    (that part was already correct) - this only adds the side effect
    real hardware would have that a fixed-value stub alone can't."""

    REGISTER_ADDR = 0x41000
    PENDING_FLAG_ADDR = 0x1F02   # [0x1AF2], DS=0x41 -> physical 0x410+0x1AF2
    LATCHED_FLAG_ADDR = 0x1EFE   # [0x1AEE], DS=0x41 -> physical 0x410+0x1AEE

    def install(self, emu, uc_module):
        emu.hook_add(uc_module.UC_HOOK_MEM_READ, self._on_read,
                     None, self.REGISTER_ADDR, self.REGISTER_ADDR)

    def _on_read(self, uc_eng, access, address, size, value, user_data):
        uc_eng.mem_write(self.PENDING_FLAG_ADDR, (1).to_bytes(2, "little"))
        uc_eng.mem_write(self.LATCHED_FLAG_ADDR, (1).to_bytes(2, "little"))


class DiagnosticTextCapture:
    """Watches every byte `write_readout_port_byte` (physical 0xE0B50)
    sends out - this is the *only* channel this project's own emulation
    work already confirmed carries the self-test/POST diagnostic text
    on real (comm-equipped) hardware (see MEMORY_MAP.md's "Puzzle:
    write_readout_port_byte's address overlaps..." section, resolved
    2026-09-16). This instrumentation existed only as one-off scratch
    code during that investigation; made permanent here since watching
    this text is the main way to tell how much further a long emulator
    run actually got (self-test pass/fail lines, not just instruction
    counts).

    Hooks the exact instruction that moves the byte into the register
    (`mov byte ptr es:[di], bl` at physical 0xE0B63 - the function's own
    parameter, [bp+6], has already been loaded into BL by this point),
    not the function's entry point, since BP isn't set up yet at entry
    (see disasm/160-3633-14_readable.asm's write_readout_port_byte)."""

    WRITE_INSN_ADDR = 0xE0B63

    def __init__(self, sink=print):
        self.buffer = []
        self.lines = []
        self.sink = sink

    def install(self, emu, uc_module):
        emu.hook_add(uc_module.UC_HOOK_CODE, self._on_exec,
                     None, self.WRITE_INSN_ADDR, self.WRITE_INSN_ADDR)

    def _on_exec(self, uc_eng, address, size, user_data):
        bl = uc_eng.reg_read(x86.UC_X86_REG_BL)
        ch = chr(bl) if 32 <= bl < 127 else None
        if bl in (0, 13, 10) or ch is None:
            if self.buffer:
                self.lines.append("".join(self.buffer))
                self.sink(f"{ANSI_WHITE}[DIAG TEXT] {''.join(self.buffer)!r}{ANSI_RESET}")
                self.buffer = []
        else:
            self.buffer.append(ch)

    def flush(self):
        if self.buffer:
            self.lines.append("".join(self.buffer))
            self.sink(f"{ANSI_WHITE}[DIAG TEXT] {''.join(self.buffer)!r} (unterminated){ANSI_RESET}")
            self.buffer = []


class VectorDisplay:
    """Captures on-screen vector line-draw commands so a front end (the
    Tkinter TUI's canvas) can render what the firmware is actually
    plotting, instead of only its register state - user request
    2026-09-22: "can you replace the TUI with some other GUI ... that
    can have a canvas that can display the vector buffer?"

    Hooks only `plot_line_to`'s entry (physical `0xE7E0D`, `160-3633`) -
    not `update_plot_position` (`0xE7D7D`) too, and not either of their
    own higher-level callers (`draw_pending_line_segment`/`reset_plot_
    home_or_acq`) - because `plot_line_to` is the single choke point
    every draw command already funnels through: both functions dispatch
    on the same mode selector `[0x6ca]` and (mode 1 aside, see below)
    both end up writing their new, scaled position into the same two
    RAM cells, `[0x6b2]`/`[0x6b4]` - confirmed directly in `disasm/
    160-3633-14_readable.asm` (`update_plot_position`/`plot_line_to`,
    physical `0xE7D7D`/`0xE7E0D`). That means `[0x6b2]`/`[0x6b4]`, read
    live at the moment `plot_line_to` is entered, already hold exactly
    the "pen was last moved/drawn to here" position left behind by
    whatever called `update_plot_position` (a pen-up move) most
    recently - no separate position-tracking state needed on this
    class's own side, and no need to separately hook `update_plot_
    position` at all, since it never draws anything itself.

    **Physical addresses derived from the confirmed `DS=0x8F80` flat
    variable-space convention** this same file already relies on for
    the neighboring `[0x6D6]`/`[0x6E2]` far pointers (see `MASK_LATCH_
    FAR_PTR_ADDRS`'s docstring above - `[0x712]` independently confirmed
    RAM-identity-mapped at physical `0x8FF12` = `0x8F800 + 0x712`) -
    `[0x6ca]`/`[0x6b2]`/`[0x6b4]` sit in that exact same low-offset
    range and are referenced by the exact same ROM segment, so the same
    `0x8F800` base applies without needing a fresh derivation.

    **Stack-argument extraction happens at the function's raw entry
    point, before its own `push bp; mov bp, sp` prologue runs** - same
    situation `DiagnosticTextCapture`'s docstring already flags for a
    different function ("BP isn't set up yet at entry"). Both `x`/`y`
    are confirmed (`disasm/gen_disasm_x86.PARAMETER_NAMES`) to live at
    `[bp+6]`/`[bp+8]`, which - for a far call's 4-byte return address
    (`[bp+2]`=ret IP, `[bp+4]`=ret CS) - resolves to `SP+4`/`SP+6` at
    the raw entry point (`SP_entry = bp_final + 2`, since `push bp`
    hasn't yet consumed the word that becomes `[bp+0]`). Read directly
    off `SS:SP` the same way `find_block13_caller.py`'s one-off
    investigation script already read a return address off the stack -
    an established, working technique in this project, not a new one.

    **Mode 1 (the HPGL plotter passthrough - `PD%d,%d;` emitted via
    `format_string_va`) is deliberately excluded**: its branch never
    writes `[0x6b2]`/`[0x6b4]` at all, so it has no on-screen position
    effect to capture, and a real caller mixing mode-1 output with the
    other modes would otherwise produce a spurious segment jumping to
    wherever the pen was last left by a *different*, non-consecutive
    call. Every other mode (0/2/3/4/8) does update those two cells and
    is treated as a real segment - this hasn't been individually
    verified per-mode against real hardware, only derived from reading
    the shared dispatch shape both functions have in common.

    Segment coordinates are stored **unscaled** (each function's own
    first move: `shl` the raw `[bp+6]`/`[bp+8]` argument by 1 twice,
    i.e. `*4`, before storing into `[0x6b2]`/`[0x6b4]` - this class
    divides back out by 4 so a consumer works in the same raw ~0-1023
    coordinate space the firmware's own callers pass in, not the
    internal *4-scaled one)."""

    PLOT_LINE_TO_ADDR = 0xE7E0D
    VARSPACE_BASE = 0x8F800  # DS=0x8F80, see class docstring
    MODE_ADDR = VARSPACE_BASE + 0x6CA    # [0x6ca]
    CUR_X_ADDR = VARSPACE_BASE + 0x6B2   # [0x6b2]
    CUR_Y_ADDR = VARSPACE_BASE + 0x6B4   # [0x6b4]
    HPGL_PASSTHROUGH_MODE = 1

    def __init__(self, max_segments=4000):
        self.segments = collections.deque(maxlen=max_segments)
        self.max_segments = max_segments
        # Monotonically increasing count of every segment ever appended
        # (unlike `len(segments)`, never shrinks when the deque evicts
        # its oldest entry past `max_segments`) - lets a consumer like
        # the Tkinter canvas draw only the segments it hasn't already
        # drawn, instead of re-walking and redrawing the whole deque on
        # every refresh tick. `clear()` resets it back to 0 too, so a
        # consumer comparing against its own last-seen total can detect
        # "the buffer was cleared" (total went backwards) the same way
        # it detects "new segments arrived" (total went forwards).
        self.total = 0

    def install(self, emu, uc_module):
        emu.hook_add(uc_module.UC_HOOK_CODE, self._on_entry,
                     None, self.PLOT_LINE_TO_ADDR, self.PLOT_LINE_TO_ADDR)

    def _on_entry(self, uc_eng, address, size, user_data):
        mode = int.from_bytes(uc_eng.mem_read(self.MODE_ADDR, 2), "little")
        if mode == self.HPGL_PASSTHROUGH_MODE:
            return
        ss = uc_eng.reg_read(x86.UC_X86_REG_SS)
        sp = uc_eng.reg_read(x86.UC_X86_REG_SP)
        raw_x = int.from_bytes(uc_eng.mem_read((ss << 4) + ((sp + 4) & 0xFFFF), 2), "little")
        raw_y = int.from_bytes(uc_eng.mem_read((ss << 4) + ((sp + 6) & 0xFFFF), 2), "little")
        old_x = int.from_bytes(uc_eng.mem_read(self.CUR_X_ADDR, 2), "little") // 4
        old_y = int.from_bytes(uc_eng.mem_read(self.CUR_Y_ADDR, 2), "little") // 4
        self.segments.append((old_x, old_y, raw_x, raw_y))
        self.total += 1

    def clear(self):
        self.segments.clear()
        self.total = 0

    def status(self):
        return f"{len(self.segments)} line segment(s) captured (max {self.max_segments})"


class InteractiveFrontPanel:
    """Live, console-controllable SWB1/SWB2 front-panel registers - see
    interactive.py's `press`/`release` commands. Unlike FRONT_PANEL_
    BUTTON_STUBS' fixed constants, this holds mutable Python state the
    console can change at runtime; starts at the same real captured
    idle baselines (VARIABLES.md, 2026-09-13 live-hardware test) so an
    interactive run with nothing pressed behaves identically to the
    headless default.

    Bit maps and polarity from VARIABLES.md's `[0x758]`/`[0x759]`
    entries (`SWB2`/`SWB1`, bit0=LSB) - `A_ONLY`/`B_ONLY` are the two
    documented active-low bits, confirmed by live hardware test
    (`HORIZONTAL MODE`=A/BOTH/B gave `0x40`->`0x41`->`0x01`); every
    other button is active-high, also live-test-confirmed there."""

    SWB2_ADDR = 0x43FFA
    SWB1_ADDR = 0x43FFB
    SWB2_IDLE = 0x08
    SWB1_IDLE = 0x44

    # name: (register, bit index, active_low)
    BUTTONS = {
        "MEM1": ("SWB2", 1, False),
        "MEM2": ("SWB2", 5, False),
        "MEM3": ("SWB2", 0, False),
        "POS_SEL": ("SWB2", 2, False),
        "1K_4K": ("SWB2", 3, False),
        "MENU": ("SWB2", 4, False),
        "MENU_ADV": ("SWB2", 6, False),
        "SELECT_C1_C2": ("SWB2", 7, False),
        "A_ONLY": ("SWB1", 0, True),
        "PRE_POST": ("SWB1", 1, False),
        "HOR_CAL": ("SWB1", 2, False),
        "HOR_MAG": ("SWB1", 3, False),
        "ROLL": ("SWB1", 4, False),
        "HOLD": ("SWB1", 5, False),
        "B_ONLY": ("SWB1", 6, True),
        "STORE_ON": ("SWB1", 7, False),
    }

    def __init__(self):
        self.swb2 = self.SWB2_IDLE
        self.swb1 = self.SWB1_IDLE

    def install(self, emu, uc_module):
        emu.hook_add(uc_module.UC_HOOK_MEM_READ, self._on_read,
                     None, self.SWB2_ADDR, self.SWB2_ADDR)
        emu.hook_add(uc_module.UC_HOOK_MEM_READ, self._on_read,
                     None, self.SWB1_ADDR, self.SWB1_ADDR)

    def _on_read(self, uc_eng, access, address, size, value, user_data):
        current = self.swb2 if address == self.SWB2_ADDR else self.swb1
        uc_eng.mem_write(address, bytes([current]) * size)
        return True

    def set_button(self, name, pressed):
        key = name.upper().replace("-", "_")
        if key not in self.BUTTONS:
            raise KeyError(name)
        reg, bit, active_low = self.BUTTONS[key]
        mask = 1 << bit
        active = (not pressed) if active_low else pressed
        if reg == "SWB2":
            self.swb2 = (self.swb2 | mask) if active else (self.swb2 & ~mask & 0xFF)
        else:
            self.swb1 = (self.swb1 | mask) if active else (self.swb1 & ~mask & 0xFF)

    def status(self):
        return f"SWB2=0x{self.swb2:02X}  SWB1=0x{self.swb1:02X}"


class InteractiveUartMock:
    """A real 8251-family register model (`i8251.I8251`, ported from
    MAME's `i8251.cpp` - see that module's docstring) standing in for
    the comm option's UART, for interactive serial-message injection
    (`serial <text>` in interactive.py/tui.py).

    **Register mapping** (best-justified, not independently confirmed
    against a live capture): `0x406F0` = Data register, `0x406F1` =
    Control/Status register - the standard 8251-family convention
    (`BA0` selects data vs. control), matching this project's own
    confirmed `BA0`->`A0` UART wiring (`MEMORY_MAP.md`'s RS-232 option
    board section). This makes `write_readout_port_byte`'s already-
    confirmed diagnostic-text writes to `0x406F0` a genuine, literal
    UART TX data write under this model - consistent with, not
    contradicting, that earlier finding. The other 6 addresses in the
    `0x406F0`-`0x406F7` block are left unmodeled (plausibly the GPIB
    controller's own registers on a GPIB-optioned unit, per this
    project's own still-open notes on that address range).

    **RxRDY/TxRDY are also mirrored into the Option Status Latch**
    (`0x4067C`, `BD1`="UART INTR+DR", `BD2`="UART TBRE" per the
    schematic-traced bit map) on top of `COMM_OPTION_STUBS`' fixed
    baseline - real firmware may poll status this way instead of (or
    in addition to) reading the UART's own status register directly.

    **INT255 on RxRDY**, same mechanism as before: real hardware
    doesn't just leave a status bit lying around for software to poll
    - the UART actively asserts its interrupt line the moment a byte
    arrives. Firing is gated on the Option Interrupt Mask Latch's `0D`
    output (`0x406F8`) being nonzero - confirmed empirically 2026-09-16
    that a purely passive stub (the pre-i8251-core version of this
    class) never gets consumed even 40M+ instructions after injection,
    exactly what you'd expect if the real path is interrupt-driven.
    **The exact bit-level write convention for that mask output hasn't
    been re-traced** - `set_comm_queue_busy` (`0x8009B`) writes it
    through a RAM-resident far pointer (`[0x6E2]`) whose own
    initialization wasn't tracked down here, so this checks "is the
    byte nonzero," a reasonable approximation rather than a
    re-confirmed bit convention."""

    DATA_ADDR = 0x406F0
    CONTROL_ADDR = 0x406F1
    STATE_BUFFER_ADDR = 0x4067C
    RX_MIRROR_BIT = 0x02  # BD1 = "UART INTR+DR"
    TX_MIRROR_BIT = 0x04  # BD2 = "UART TBRE"
    MASK_LATCH_ADDR = 0x406F8

    def __init__(self, sink=print, on_tx=None):
        from i8251 import I8251
        self.chip = I8251()
        self.chip.rxrdy_handler = self._on_rxrdy
        self.chip.tx_byte_handler = self._on_tx_byte
        self.queue = bytearray()
        self.rx_log = []   # bytes actually popped by a real read (confirms consumption)
        self.tx_log = []   # bytes the chip has transmitted
        self._emu = None
        self.sink = sink
        self.on_tx = on_tx  # optional live callback, one call per TX byte -
                             # for a front end that wants to display outgoing
                             # serial data as it happens, not just on demand
                             # via outgoing_text()
        self.instructions_per_byte = self.DEFAULT_INSTRUCTIONS_PER_BYTE
        self._next_delivery_count = 0  # eligible immediately on first inject

    def install(self, emu, uc_module):
        self._emu = emu
        emu.hook_add(uc_module.UC_HOOK_MEM_READ, self._on_data_read,
                     None, self.DATA_ADDR, self.DATA_ADDR)
        emu.hook_add(uc_module.UC_HOOK_MEM_WRITE, self._on_data_write,
                     None, self.DATA_ADDR, self.DATA_ADDR)
        emu.hook_add(uc_module.UC_HOOK_MEM_READ, self._on_control_read,
                     None, self.CONTROL_ADDR, self.CONTROL_ADDR)
        emu.hook_add(uc_module.UC_HOOK_MEM_WRITE, self._on_control_write,
                     None, self.CONTROL_ADDR, self.CONTROL_ADDR)
        # Mirror RxRDY/TxRDY into the State Buffer on top of whatever
        # COMM_OPTION_STUBS's fixed comm_stat value already set -
        # Unicorn calls same-address read hooks in installation order,
        # so this only adjusts bits, it doesn't need to be installed
        # in any particular order relative to that stub (it re-reads
        # the current byte each time rather than assuming a baseline).
        emu.hook_add(uc_module.UC_HOOK_MEM_READ, self._on_state_buffer_read,
                     None, self.STATE_BUFFER_ADDR, self.STATE_BUFFER_ADDR)

    # Instructions between successive queued-byte deliveries - a crude
    # proxy for real elapsed time, since this project hasn't confirmed
    # this CPU's real clock rate precisely enough to derive a genuine
    # baud-accurate instruction count. Deliberately tunable (`serial-
    # rate <n>` at the REPL/TUI) rather than hardcoded to one "correct"
    # value - the point is to be able to explore whether a *faster*
    # delivery rate causes overrun once the interrupt path is reachable
    # (see the 2026-09-16 user hypothesis below), not to claim this
    # default is calibrated to a specific real baud rate.
    DEFAULT_INSTRUCTIONS_PER_BYTE = 50000

    def inject(self, text):
        """Queues bytes for **paced** delivery (see `pump_paced`) -
        deliberately does *not* dump the whole string into the chip's
        holding register at once. User's own diagnosis, 2026-09-16:
        "this is probably read in by interrupts which is what could
        cause the 9600 to not operate correctly... to make this work
        you need to take the serial input and send it one character at
        a time into the interrupt through the 8251/82C52" - exactly
        right, and it exposed a real gap this mock had: before this,
        a freed RX register was refilled *instantly* the moment the
        CPU read it, with no time delay standing in for the real
        per-byte serial arrival interval - meaning overrun could never
        actually occur here regardless of how slowly firmware serviced
        bytes, which made it impossible to explore the 9600-vs-1200-
        baud reliability difference this project's own live-hardware
        testing already found (`docs/comm-rom/rs232-breakthrough.md`)."""
        self.queue.extend(text.encode("ascii", errors="replace"))

    def pump_paced(self, current_count):
        """Call once per emulated instruction (see `Debugger._on_code`)
        - delivers the next queued byte once `current_count` reaches
        the scheduled delivery time, **unconditionally** (not gated on
        the RX holding register being free). Real hardware doesn't wait
        for the CPU's convenience either - a byte arrives at the fixed
        baud-derived cadence regardless of whether the previous one was
        read, and `self.chip.receive_byte` already implements the
        resulting overrun correctly (matches MAME's own model) if it
        wasn't."""
        if not self.queue:
            return
        if current_count < self._next_delivery_count:
            return
        byte = self.queue.pop(0)
        self.chip.receive_byte(byte)
        self._next_delivery_count = current_count + self.instructions_per_byte

    def _on_rxrdy(self, level):
        if level:
            self._try_fire_interrupt()

    def _on_tx_byte(self, byte):
        self.tx_log.append(byte)
        if self.on_tx:
            self.on_tx(byte)

    def _try_fire_interrupt(self):
        """Simulate the real UART asserting its interrupt line the
        moment RxRDY goes high, gated on the DR-mask output being
        unmasked - see the class docstring for what's approximated
        here vs. fully re-confirmed."""
        if self._emu is None:
            return
        from timer import fire_interrupt  # local import: avoid a hard
                                           # module-load dependency for
                                           # code paths that never inject
        try:
            mask_byte = self._emu.mem_read(self.MASK_LATCH_ADDR, 1)[0]
        except Exception:
            mask_byte = 0
        if not mask_byte:
            self.sink(f"{ANSI_WHITE}[SERIAL] DR interrupt masked "
                      f"(0x{self.MASK_LATCH_ADDR:X}=0) - byte(s) queued but "
                      f"no INT255 fired{ANSI_RESET}")
            return
        if fire_interrupt(self._emu, 255, respect_if=True):
            self.sink(f"{ANSI_WHITE}[SERIAL] fired INT255 (UART DR) to "
                      f"signal new data{ANSI_RESET}")
        else:
            self.sink(f"{ANSI_WHITE}[SERIAL] INT255 not delivered (IF clear "
                      f"or vector not installed){ANSI_RESET}")

    def _on_data_read(self, uc_eng, access, address, size, value, user_data):
        byte = self.chip.data_r()
        uc_eng.mem_write(address, bytes([byte]) * size)
        self.rx_log.append(byte)
        ch = chr(byte) if 32 <= byte < 127 else f"\\x{byte:02x}"
        self.sink(f"{ANSI_WHITE}[SERIAL RX] firmware read {ch!r} (0x{byte:02X}) - "
                  f"{len(self.queue)} byte(s) still queued{ANSI_RESET}")
        # No immediate refill here - the next queued byte arrives on its
        # own paced schedule (see `pump_paced`), not the instant this
        # one is read out.
        return True

    def _on_data_write(self, uc_eng, access, address, size, value, user_data):
        self.chip.data_w(value & 0xFF)
        return True

    def _on_control_read(self, uc_eng, access, address, size, value, user_data):
        status = self.chip.status_r()
        uc_eng.mem_write(address, bytes([status]) * size)
        return True

    def _on_control_write(self, uc_eng, access, address, size, value, user_data):
        self.chip.control_w(value & 0xFF)
        return True

    def _on_state_buffer_read(self, uc_eng, access, address, size, value, user_data):
        current = uc_eng.mem_read(address, 1)[0]
        current = (current | self.RX_MIRROR_BIT) if self.chip.rxrdy_r() else (current & ~self.RX_MIRROR_BIT)
        current = (current | self.TX_MIRROR_BIT) if self.chip.txrdy_r() else (current & ~self.TX_MIRROR_BIT)
        uc_eng.mem_write(address, bytes([current]) * size)
        return True

    def clear_outgoing(self):
        n = len(self.tx_log)
        self.tx_log = []
        return n

    def status(self):
        rx = f"RX queue: {len(self.queue)} byte(s) pending, next={chr(self.queue[0])!r}" if self.queue else "RX queue empty"
        # No real backlog concept on the TX side - data_w() completes
        # "instantly" (see i8251.py's docstring on why), so there's
        # never more than the single byte in the chip's own TX holding
        # register at once. "TX queue" here means the same thing
        # `outgoing_text()` shows: everything ever transmitted.
        tx = f"TX queue: {len(self.tx_log)} byte(s) sent"
        return (f"{rx}, {len(self.rx_log)} consumed | {tx} | "
                f"chip status=0x{self.chip.status:02X}, "
                f"pacing={self.instructions_per_byte} instrs/byte")

    def chip_detail(self):
        """The i8251 core's own programmed configuration and internal
        state (mode/command register decode, rx/tx holding registers,
        which control byte it's expecting next) - see `i8251.I8251.
        describe()`. Distinct from `status()` above, which is about
        this mock's own queue/pacing bookkeeping, not the chip itself."""
        d = self.chip.describe()
        return (f"mode=0x{d['mode_byte']:02X} ({d['mode_desc']}) | "
                f"command=0x{d['command']:02X} "
                f"(TxEN={int(d['tx_enable'])} RxEN={int(d['rx_enable'])} "
                f"DTR={int(d['dtr'])} RTS={int(d['rts'])} "
                f"BRK={int(d['send_break'])} HUNT={int(d['hunt_mode'])}) | "
                f"rx_data=0x{d['rx_data']:02X} tx_data=0x{d['tx_data']:02X} | "
                f"next control byte: {d['next_control_byte']}")

    def incoming_text(self):
        """Everything still sitting in the RX queue, not yet consumed
        by a real read. Unlike `outgoing_text` (a scrolling log, where
        a real line break/tab is fine to render as one), this is shown
        in a small fixed-height status box (`tui.py`'s `#incoming`
        panel) - a literal control byte here (even `\\r`/`\\n`/`\\0`)
        would silently eat one of its few visible lines instead of
        showing up as content, exactly the kind of invisible-content
        bug already hit once with the registers panel. So every
        non-printable byte gets the same `\\xNN` hex form - the whole
        point being that you can always see everything actually
        queued, in full, on one line."""
        out = []
        for b in self.queue:
            if 32 <= b < 127:
                out.append(chr(b))
            else:
                out.append(f"\\x{b:02x}")
        return "".join(out)

    def outgoing_text(self):
        """Real `\\r`/`\\n`/tab pass through as actual control bytes (so
        they render as real line breaks/tabs when printed) - only
        genuinely unprintable bytes get the `\\xNN` escape form."""
        out = []
        for b in self.tx_log:
            if b in (13, 10, 9) or 32 <= b < 127:
                out.append(chr(b))
            else:
                out.append(f"\\x{b:02x}")
        return "".join(out)


# Physical addresses of 2 RAM-resident far-pointer variables in the
# comm ROM's own flat variable space (`[0x6D6]`, `[0x6E2]` - ES=0x8F80
# convention, confirmed via `[0x712]` already living at physical
# 0x8FF12) that `set_comm_flow_hold`/`set_comm_queue_busy`/`update_
# comm_tx_ready_flag` all dereference to reach the Option Interrupt
# Mask Latch (`0x406F8`-`0x406FB`) at offsets +0/+1/+3 - matching all
# 3 of that latch's software-driven outputs (`0D`/`1D`/`3D`; `2D` is a
# hardware-driven modem-control line, never toggled by code, per the
# schematic trace). **No write to either variable exists anywhere in
# this project's currently-decoded comm ROM** - confirmed by grepping
# the full disassembly, only the 2 far-pointer *reads* (`les di, [...]`)
# are ever found. This project's own account of why: `0x88000-0x8FFFF`
# is genuine battery-backed nonvolatile RAM (Table 3-1) - a real,
# already-in-service unit almost certainly has this pointer written
# once at manufacturing/first power-up and never again, relying on the
# battery to hold it forever. This emulator has no NVRAM-persistence
# model, so a fresh run starts both addresses at zero - confirmed
# directly 2026-09-16 (`mem 8fed6 4` / `mem 8fee2 4` both read `00 00
# 00 00`) - meaning any comm-ROM code that dereferences these pointers
# would silently corrupt the Interrupt Vector Table (physical
# `0x00000`) instead of reaching the Interrupt Mask Latch at all.
MASK_LATCH_FAR_PTR_ADDRS = (0x8FED6, 0x8FEE2)
MASK_LATCH_SEGMENT = 0x4000
MASK_LATCH_OFFSET = 0x06F8


def seed_comm_nvram_defaults(emu):
    """Writes the plausible factory-calibrated far-pointer value real,
    already-in-service hardware would have persisted in nonvolatile
    RAM - see `MASK_LATCH_FAR_PTR_ADDRS`'s comment for why this is
    needed at all. **This is the single most load-bearing, least-
    confirmed assumption in this fix** - the segment:offset value
    itself (`0x4000:0x06F8`) is inferred from this project's own
    established `ES=0x4000` I/O-window convention (used 31+ times
    elsewhere per `MEMORY_MAP.md`) plus the already-confirmed `0x406F8`
    physical address, not read from a real captured NVRAM dump. If
    comm-ROM execution reaches these functions and still behaves
    wrong, this exact value is the first thing to question - flag it
    for a live-hardware NVRAM read if a way to capture one ever comes
    up (e.g. an EPROM/RAM reader on the physical `U1242`/Option Memory
    board chips)."""
    ptr_bytes = (MASK_LATCH_OFFSET.to_bytes(2, "little")
                 + MASK_LATCH_SEGMENT.to_bytes(2, "little"))
    for addr in MASK_LATCH_FAR_PTR_ADDRS:
        emu.mem_write(addr, ptr_bytes)


class InteractiveDipSwitches:
    """Live, console/UI-controllable comm-option PARAMETERS DIP
    switches (1-10) - see `MEMORY_MAP.md`'s RS-232 option board section
    for the confirmed bit map: switches 1-7 are Parameter Buffer
    (`0x406BC`) bits 0-6, switches 8-10 are State Buffer (`0x4067C`)
    bits 3-5 - **but NOT in sequential order**, confirmed 2026-09-17
    against the Options manual's Table 7-36 ("RS-232-C Status Buffer
    Functions"): bit 3 = switch 8, bit 4 = **switch 10**, bit 5 =
    **switch 9** (switches 9 and 10 are swapped relative to the naive
    bit-order guess - an earlier version of this stub had them backwards,
    a real bug that stayed invisible until a switch other than 8 was
    actually toggled, since the default pattern has all 3 off). The
    same manual's Table 7-11 also gives the *meaning* of switches 1-8
    (baud rate, parity enable, parity select, line terminator) -
    beyond that, switches 9-10's own meaning (printer/plotter device
    selection per Table 7-11, footnote c) and the exact GPIB-side
    switch semantics are still open (see `STILL_PENDING_DECODE.md`).

    **Polarity is active-LOW, confirmed 2026-09-17** against a real
    exerciser-screen photo showing `comm_param` reading `0xF3` with a
    different physical switch pattern than the earlier `0xF8`/`0xF9`
    baseline capture - user: "when the comm parameter dip switches are
    1 they show as 0 on these numbers." A switch physically ON pulls
    its bit to 0; OFF leaves it (pulled up) at 1. `self.switches[i] =
    True` still means "switch i is ON" - only the bit-level read hooks
    translate that to 0 now, the opposite of this class's original,
    wrong active-high version.

    Installed *after* `COMM_OPTION_STUBS`'s fixed baseline values
    (`comm_param`=`0xF8`, `comm_stat`=`0x7D` - real captured values,
    see `MEMORY_MAP.md`'s exerciser-screen section) so it only
    overrides the specific switch bit positions, leaving every other
    bit (Parameter Buffer's UART SDO bit, State Buffer's PWR INT/
    INTR+DR/TBRE/diagnostic/DCD bits) at its real captured baseline."""

    PARAM_ADDR = 0x406BC
    STATUS_ADDR = 0x4067C

    def __init__(self):
        # User-specified default, 2026-09-17: "0011000000" (switch 1
        # through switch 10, left to right) - only switches 3 and 4 on.
        # This *replaces* COMM_OPTION_STUBS's own baseline-byte-derived
        # pattern (which was [F,F,F,T,T,T,T,T,T,T]) as the switch-bit
        # default; the non-switch bits in comm_param/comm_stat (UART
        # SDO, PWR INT, INTR+DR, TBRE, diagnostic, DCD) are untouched
        # either way, since this class only ever overrides the switch
        # bit positions on top of that fixed baseline.
        self.switches = [False, False, True, True, False, False, False, False, False, False]

    def install(self, emu, uc_module):
        emu.hook_add(uc_module.UC_HOOK_MEM_READ, self._on_param_read,
                     None, self.PARAM_ADDR, self.PARAM_ADDR)
        emu.hook_add(uc_module.UC_HOOK_MEM_READ, self._on_status_read,
                     None, self.STATUS_ADDR, self.STATUS_ADDR)

    def _on_param_read(self, uc_eng, access, address, size, value, user_data):
        # Switches are active-LOW (confirmed 2026-09-17 against a real
        # exerciser-screen photo: "when the comm parameter dip switches
        # are 1 they show as 0 on these numbers") - a switch physically
        # ON pulls its bit to 0; OFF floats/pulls the bit to 1. Start
        # with bits 0-6 all set (every switch "off") and clear one bit
        # per switch that's actually on - the opposite of the original,
        # wrong active-high version this replaced.
        current = (uc_eng.mem_read(address, 1)[0] & 0x80) | 0x7F  # keep bit7 (UART SDO)
        for i in range(7):
            if self.switches[i]:
                current &= ~(1 << i)
        uc_eng.mem_write(address, bytes([current]) * size)
        return True

    # Bit-to-switch map for the State/Status Buffer's 3 switch bits,
    # confirmed 2026-09-17 against the Options manual's Table 7-36
    # ("RS-232-C Status Buffer Functions") - deliberately NOT
    # sequential (bit4 is switch 10, bit5 is switch 9, not the other
    # way around as an earlier version of this stub assumed):
    # bit 3 = switch 8, bit 4 = switch 10, bit 5 = switch 9.
    _STATUS_BIT_TO_SWITCH = {3: 8, 4: 10, 5: 9}

    def _on_status_read(self, uc_eng, access, address, size, value, user_data):
        # Same active-LOW polarity as _on_param_read above.
        current = uc_eng.mem_read(address, 1)[0] | 0x38  # bits 3-5 all "off" (1) by default
        for bit, switch_num in self._STATUS_BIT_TO_SWITCH.items():
            if self.switches[switch_num - 1]:
                current &= ~(1 << bit)
        uc_eng.mem_write(address, bytes([current]) * size)
        return True

    def set_switch(self, number, on):
        """`number` is 1-indexed (switch 1 through switch 10)."""
        self.switches[number - 1] = bool(on)

    def status(self):
        bits = "".join("1" if s else "0" for s in self.switches)
        return f"DIP switches 1-10: {bits}"


class AccessCounter:
    """Per-address memory read/write counts and per-port I/O in/out
    counts, for empirically answering "does a real run actually touch
    this address/port" instead of guessing from static disassembly
    alone. User request 2026-09-17, prompted by real doubt about this
    project's own assumed 'outgoing serial' address: the self-test
    diagnostic text written via `write_readout_port_byte` (physical
    `0x406F0`) never appears on a real hardware serial capture at
    boot, so that address is suspected wrong despite living in the
    same block as the confirmed UART wiring - see `TODO.md`'s
    `write_readout_port_byte` entry.

    Plain dicts, keyed by address/port - nothing is pre-allocated for
    the full address space, an entry only exists once that exact
    address/port is actually touched (per a later user request:
    "just create a dictionary structure so you only track addresses
    as they are touched instead of every address").

    **Real, serious Unicorn bug confirmed 2026-09-17 - memory read
    tracking is opt-in and OFF by default because of it**: installing
    a plain `UC_HOOK_MEM_READ` hook that overlaps the stack corrupts
    real CPU execution in this Unicorn version (2.1.4) - not just a
    performance cost, an actual correctness bug. Isolated with a
    minimal repro: a real boot trace that runs 5M+ instructions clean
    with no hook, or with only the write/IO hooks installed, crashes
    at the *exact same* instruction count every time a MEM_READ hook
    is added, even one covering nothing but a 256-byte slice of the
    stack and nothing else. Traced via `trace on` to the exact failure:
    a `retf` pops `CS=0x0000` instead of the real return segment,
    right after a completely ordinary table-walk loop and an ordinary
    nested `retf`; the CPU then runs off into low RAM misinterpreted
    as code and crashes writing to ROM a few instructions later.
    Confirmed the hook's own logic isn't the cause (tried a no-op
    `return True` version - identical crash, identical instruction
    count) and confirmed it's specifically about *which addresses* are
    hooked, not global-vs-bounded-range syntax (an explicit `0-
    0xFFFFF` range crashes identically to the `begin=1,end=0` "whole
    space" convention; a range covering only `0x40000+`, nowhere near
    where the stack actually was at the time, never crashes at all).
    **Conclusion: never install `install_watch_range` (or any
    MEM_READ hook) over a range that might include wherever SS:SP
    happens to point during the run** - since that can move around
    over a boot sequence (this trace's stack was in low RAM early on,
    not the `0x40000` I/O window), there is no address range that's
    provably safe for the whole run without also verified against
    where the stack lives at every point in time. Until this is
    understood well enough to work around, treat any `install_watch_
    range` call as diagnostic-only, for a short/targeted run, never
    for a run whose actual completion you need to trust.

    Memory *write* tracking and I/O port tracking do not exhibit this
    - verified over a full 25M-instruction run with no MEM_READ hook
    present.

    **Real gotcha confirmed while testing this (independent of the
    above bug)**: any register backed by a read-override stub in this
    file (`FixedByteRead`, `InteractiveFrontPanel`, `InteractiveDip
    Switches`, the UART's control/data/state-buffer reads - all of
    which call `uc_eng.mem_write()` from *inside* their own `UC_HOOK_
    MEM_READ` callback, to make the CPU's read see the stubbed value)
    makes that stub's own write-back count as a "write" here too -
    confirmed directly: watching SWB2 (`0x43FFA`) over a 2M-instruction
    run showed `read=120027 write=120027`, an exact match that's the
    stub reflecting its value on every read, not firmware ever writing
    to what's a hardware *input* register. Only trust a nonzero write
    count as "firmware actually wrote here" for an address that has no
    such stub installed."""

    def __init__(self):
        self.mem_reads = {}
        self.mem_writes = {}
        self.io_in = {}
        self.io_out = {}
        self._watch_hook_ids = []
        self._emu = None

    def install_io(self, emu, uc_module):
        """Safe to leave on for every run - I/O port in/out hooks and
        memory *write* hooks don't exhibit the stack-corruption bug
        documented in this class's own docstring."""
        emu.hook_add(uc_module.UC_HOOK_MEM_WRITE, self._on_mem_write, None)
        emu.hook_add(uc_module.UC_HOOK_INSN, self._on_in, None, 1, 0, x86.UC_X86_INS_IN)
        emu.hook_add(uc_module.UC_HOOK_INSN, self._on_out, None, 1, 0, x86.UC_X86_INS_OUT)

    def install_watch_range(self, emu, uc_module, start, end):
        """Opt-in, and only ever call this for a short/targeted
        diagnostic run - see this class's docstring for the confirmed
        stack-corruption bug a MEM_READ hook triggers if the range
        happens to overlap wherever the stack is at the time. Adds one
        more watched range each call; `unwatch` clears all of them."""
        self._emu = emu
        hook_id = emu.hook_add(uc_module.UC_HOOK_MEM_READ, self._on_mem_read, None, start, end)
        self._watch_hook_ids.append(hook_id)

    def unwatch(self):
        if self._emu is not None:
            for hook_id in self._watch_hook_ids:
                self._emu.hook_del(hook_id)
        self._watch_hook_ids = []

    def clear(self):
        self.mem_reads.clear()
        self.mem_writes.clear()
        self.io_in.clear()
        self.io_out.clear()

    def _on_mem_read(self, uc_eng, access, address, size, value, user_data):
        self.mem_reads[address] = self.mem_reads.get(address, 0) + 1

    def _on_mem_write(self, uc_eng, access, address, size, value, user_data):
        self.mem_writes[address] = self.mem_writes.get(address, 0) + 1

    def _on_in(self, uc_eng, port, size, user_data):
        self.io_in[port] = self.io_in.get(port, 0) + 1
        return 0  # matches Unicorn's own default for an unhooked `in` -
                  # confirmed directly (AL comes back 0 with no hook at
                  # all installed) so adding this hook can't itself
                  # change what any firmware `in` instruction observes

    def _on_out(self, uc_eng, port, size, value, user_data):
        self.io_out[port] = self.io_out.get(port, 0) + 1

    def status(self):
        return (f"distinct addresses touched: "
                f"{len(set(self.mem_reads) | set(self.mem_writes))} | "
                f"distinct I/O ports touched: {len(set(self.io_in) | set(self.io_out))}")

    def summary(self, top=40):
        """`top=None` returns every touched address instead of just the
        busiest ones - used when saving a full access-log file
        (`dump`'s companion `.mem` file) rather than a quick on-screen
        glance."""
        lines = []
        if self.io_in or self.io_out:
            ports = sorted(set(self.io_in) | set(self.io_out))
            lines.append(f"I/O ports touched ({len(ports)}):")
            for p in ports:
                lines.append(f"  port 0x{p:X}: in={self.io_in.get(p, 0)} "
                              f"out={self.io_out.get(p, 0)}")
        else:
            lines.append("I/O ports touched: none")
        addrs = sorted(set(self.mem_reads) | set(self.mem_writes))
        if addrs:
            ranked = sorted(addrs, key=lambda a: -(self.mem_reads.get(a, 0) +
                                                     self.mem_writes.get(a, 0)))
            shown = len(ranked) if top is None else min(top, len(ranked))
            lines.append(f"Memory addresses touched: {len(addrs)} distinct "
                         f"(showing top {shown} by access count)")
            for a in ranked[:top]:
                lines.append(f"  0x{a:06X}: read={self.mem_reads.get(a, 0)} "
                              f"write={self.mem_writes.get(a, 0)}")
        else:
            lines.append("Memory addresses touched: none")
        return lines
