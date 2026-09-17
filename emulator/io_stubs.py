"""
I/O register stubs - see emulator/docs/design.md's "I/O port and MMIO
stub strategy". Each stub models just enough real hardware behavior to
let a specific piece of firmware logic reach a real, meaningful
conclusion instead of reading inert RAM - see individual docstrings for
which real register(s) each one is standing in for and why.
"""
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
    # clearing it back down, so a poll loop hangs/times out without an
    # active force-to-0 hook (this is the leading suspect for the
    # already-observed "Display controller : TIMEOUT" self-test
    # failure - plain RAM alone can't reproduce "hardware clears this
    # bit after the operation completes").
    (0x41000, 0x00, "display chip interrupt reset"),
    (0x42000, 0x00, "display chip next frame"),
]

COMM_OPTION_STUBS = [
    # comm_stat (Option Status Latch, U1223) - real captured value,
    # identical on both physical units: 01111101 = 0x7D. Bit map per
    # MEMORY_MAP.md's RS-232 option board section: BD0=PWR INT(1),
    # BD1=INTR+DR(0), BD2=TBRE(1), BD3-5=PARAMETERS DIP 8-10(111),
    # BD6=diagnostic(1), BD7=DCD(0).
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

    def __init__(self, sink=print):
        from i8251 import I8251
        self.chip = I8251()
        self.chip.rxrdy_handler = self._on_rxrdy
        self.chip.tx_byte_handler = self._on_tx_byte
        self.queue = bytearray()
        self.rx_log = []   # bytes actually popped by a real read (confirms consumption)
        self.tx_log = []   # bytes the chip has transmitted
        self._emu = None
        self.sink = sink

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

    def inject(self, text):
        self.queue.extend(text.encode("ascii", errors="replace"))
        self._pump_queue()

    def _pump_queue(self):
        """Hand the next queued byte to the chip once its 1-byte RX
        holding register is free - real 8251-family hardware has no
        FIFO, so a second byte arriving before the first is read is a
        genuine overrun (`self.chip.receive_byte` already models
        this), not something to queue past."""
        from i8251 import STATUS_RX_READY
        if self.queue and not (self.chip.status & STATUS_RX_READY):
            byte = self.queue.pop(0)
            self.chip.receive_byte(byte)

    def _on_rxrdy(self, level):
        if level:
            self._try_fire_interrupt()

    def _on_tx_byte(self, byte):
        self.tx_log.append(byte)

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
        self._pump_queue()
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
        return (f"{rx}, {len(self.rx_log)} consumed, {len(self.tx_log)} TX byte(s) seen, "
                f"chip status=0x{self.chip.status:02X}")

    def incoming_text(self):
        """Everything still sitting in the RX queue, not yet consumed
        by a real read - same rendering convention as `outgoing_text`."""
        out = []
        for b in self.queue:
            if b in (13, 10, 9) or 32 <= b < 127:
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
