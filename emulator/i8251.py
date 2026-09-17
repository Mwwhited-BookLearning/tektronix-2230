"""
Intel 8251 USART core, ported from MAME's `i8251.cpp`/`i8251.h`
(https://github.com/mamedev/mame/blob/master/src/devices/machine/i8251.cpp,
license: BSD-3-Clause, copyright-holders: smf, Robbbert) - fetched
2026-09-16 rather than reconstructed from memory, since register-level
UART behavior (exactly which status bits gate RxRDY/TxRDY, what a
Command-register write does and doesn't reset, how overrun is
detected) is precisely the kind of detail worth getting from a real,
tested reference instead of guessing.

This project's comm option uses an **82C52**, a second-sourced 8251A
clone (confirmed via schematic trace - see `MEMORY_MAP.md`'s RS-232
option board section) - same register-level programming model.

**Deliberate simplification from the MAME source**: MAME's version
simulates the chip bit-by-bit against real TxC/RxC serial clocks
(`transmit_clock`/`receive_clock`/`sync1_rxc`/`sync2_rxc`, the
`device_serial_interface` shift-register machinery) because MAME is
modeling a real serial *line*, bit by bit, at real UART timing. This
emulator only ever injects/observes whole bytes (`serial <text>` at
the REPL/TUI), never individual RS-232 bits, so all of that bit-level
clocking is deliberately left out - `receive_byte()` goes straight to
what MAME's `receive_character()` does (the "a complete byte has just
arrived" step), and `data_w()` (TX) is treated as completing
"instantly" rather than being paced by a transmit clock. **Every
register-level semantic that matters for firmware behavior is ported
faithfully**: the Mode/Sync1/Sync2/Command write-sequencing state
machine, the exact status bits (TxRDY/RxRDY/TxEMPTY/error flags) and
what sets/clears each one, and RxRDY/TxRDY's masking by the Receive/
Transmit-Enable command bits - only the "how does a byte's individual
bits travel across a wire" layer (irrelevant to a whole-byte injection
model) is omitted.
"""

# Status register bits (I8251_STATUS_* in MAME's i8251.h)
STATUS_TX_READY = 0x01
STATUS_RX_READY = 0x02
STATUS_TX_EMPTY = 0x04
STATUS_PARITY_ERROR = 0x08
STATUS_OVERRUN_ERROR = 0x10
STATUS_FRAMING_ERROR = 0x20
STATUS_SYNDET = 0x40

# m_flags - which byte a control-register write is currently expecting
NEXT_COMMAND = 0
NEXT_MODE = 1
NEXT_SYNC1 = 2
NEXT_SYNC2 = 3


class I8251:
    """A byte-oriented port of MAME's `i8251_device`. `rxrdy_handler`/
    `txrdy_handler`/`txempty_handler`, if set, are called with the
    current signal level (0/1) every time it's recomputed - exactly
    matching MAME's own unconditional-call convention (`m_rxrdy_
    handler(rxrdy_r())`), not just on a change."""

    def __init__(self):
        self.rxrdy_handler = None
        self.txrdy_handler = None
        self.txempty_handler = None
        self.tx_byte_handler = None  # called with each byte data_w() sends
        self.reset()

    def reset(self):
        self.flags = NEXT_MODE
        self.sync_byte_count = 0
        self.sync1 = 0
        self.sync2 = 0
        self.status = STATUS_TX_EMPTY | STATUS_TX_READY
        self.command = 0
        self.mode_byte = 0
        self.rx_data = 0
        self.tx_data = 0
        self.dsr = 1  # matches MAME's constructor default (no modem wired)
        self._update_tx_empty()
        self._update_rx_ready()
        self._update_tx_ready()

    # ---- signal helpers, exactly matching MAME's rxrdy_r/txrdy_r ----

    def _is_tx_enabled(self):
        # MAME: BIT(m_command, 0) && !m_cts - this project has no CTS
        # line wired (no external modem), so treat CTS as permanently
        # asserted (0) the way a direct-connect RS-232 cable would.
        return bool(self.command & 0x01)

    def rxrdy_r(self):
        if not (self.command & 0x04):  # Rx enable (command bit 2)
            return 0
        return 1 if (self.status & STATUS_RX_READY) else 0

    def txrdy_r(self):
        return 1 if (self._is_tx_enabled() and (self.status & STATUS_TX_READY)) else 0

    def _update_rx_ready(self):
        if self.rxrdy_handler:
            self.rxrdy_handler(self.rxrdy_r())

    def _update_tx_ready(self):
        if self.txrdy_handler:
            self.txrdy_handler(self.txrdy_r())

    def _update_tx_empty(self):
        if self.txempty_handler:
            self.txempty_handler(1 if (self.status & STATUS_TX_EMPTY) else 0)

    # ---- register writes: mode/sync/command sequencing --------------

    def control_w(self, data):
        """Write to the control/status register address. Which byte
        this actually is (Mode, Sync1, Sync2, or Command) depends on
        `self.flags` - exactly MAME's `control_w` dispatch."""
        if self.flags == NEXT_SYNC1:
            self._sync1_w(data)
        elif self.flags == NEXT_SYNC2:
            self._sync2_w(data)
        elif self.flags == NEXT_MODE:
            self._mode_w(data)
        else:
            self._command_w(data)

    def _mode_w(self, data):
        self.mode_byte = data
        self.flags = NEXT_COMMAND
        self.sync_byte_count = 0
        if data & 0x03:
            pass  # asynchronous mode - baud-rate factor/stop bits/parity
                  # only matter for real bit-level framing, not modeled here
        else:
            # synchronous mode - expects 1 or 2 sync-character writes next
            self.flags = NEXT_SYNC2 if (data & 0x80) else NEXT_SYNC1
            self.sync_byte_count = 1 if (data & 0x80) else 2

    def _sync1_w(self, data):
        self.sync1 = data
        self.flags = NEXT_SYNC2

    def _sync2_w(self, data):
        self.sync2 = data
        self.flags = NEXT_COMMAND

    def _command_w(self, data):
        self.command = data

        if data & 0x10:  # error reset
            self.status &= ~(STATUS_PARITY_ERROR | STATUS_OVERRUN_ERROR | STATUS_FRAMING_ERROR)

        if data & 0x40:  # internal reset - "returns to mode format", not a full chip reset
            self.flags = NEXT_MODE

        if data & 0x80:  # hunt mode - sync-mode only, not modeled at the bit level here
            pass

        self._update_rx_ready()
        self._update_tx_ready()
        self._update_tx_empty()

    # ---- data register --------------------------------------------

    def data_w(self, data):
        """TX data write. Real hardware paces this via a transmit
        clock; this model completes it "instantly" (see module
        docstring) - status goes not-ready then immediately back to
        ready/empty, and the byte is handed to `tx_byte_handler` right
        away, matching how `write_readout_port_byte`'s already-
        confirmed diagnostic-text output behaves (each byte written
        and immediately followed by the next, paced by the firmware's
        own generic tick busy-wait, not by this chip)."""
        self.tx_data = data
        self.status &= ~STATUS_TX_READY
        self._update_tx_ready()

        if self.tx_byte_handler:
            self.tx_byte_handler(data)

        self.status |= STATUS_TX_READY | STATUS_TX_EMPTY
        self._update_tx_ready()
        self._update_tx_empty()

    def data_r(self):
        """RX data read - clears RX_READY, exactly matching MAME's
        `data_r()` (guarded there by `side_effects_disabled()`, which
        only matters for a debugger memory-inspection read; every read
        this emulator makes is a real firmware read, so always clear)."""
        value = self.rx_data
        self.status &= ~STATUS_RX_READY
        self._update_rx_ready()
        return value

    def status_r(self):
        """Status register read - syndet always clears on a status
        read (sync-mode only, irrelevant here but kept for fidelity)."""
        value = (self.dsr << 7) | self.status
        self.status &= ~STATUS_SYNDET
        return value

    # ---- whole-byte receive path (this emulator's equivalent of ----
    # ---- MAME's bit-by-bit receive_clock -> receive_character) -----

    def receive_byte(self, byte):
        """Deliver one complete received byte - this emulator's
        equivalent of MAME's `receive_character()`, reached in the
        real chip only after `receive_clock()` has shifted in a whole
        frame's worth of bits. Sets overrun if the previous byte was
        never read, exactly as MAME does."""
        if self.status & STATUS_RX_READY:
            self.status |= STATUS_OVERRUN_ERROR
        self.rx_data = byte
        self.status |= STATUS_RX_READY
        self._update_rx_ready()
