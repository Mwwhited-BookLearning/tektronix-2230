# Hardware reference photos

Primary-source photos of the physical unit, taken by the user, to
ground the firmware reverse-engineering against real controls and
connectors instead of guesswork. Images live in `hardware/photos/`.
Add more files here (or split this doc further) as more photos come
in — don't let it become one unmanageable page.

## Comm option module rear panel (`hardware/photos/comm_option_rear_panel.jpg`)

This is the rear-panel plate for the GPIB/RS-232/plotter option board
— i.e. the physical side of `160-2998` (comm ROM), which the
disassembly work has been analyzing purely from code so far.

**RS-232-C PORT — two 25-pin D-sub connectors, not one:**
- `RS232 DTE` (male pins)
- `RS232 DCE` (female socket)

Having both a DTE and a DCE connector wired to the same UART is a
common way to let the scope connect directly to either a DTE device
(a PC, via a straight-through cable) or a DCE device (a modem, via the
other connector) without needing a null-modem adapter. This is a
strong physical confirmation of the **RS-232 flow-control cluster**
found in the comm ROM this session (`get_xon_xoff_byte`,
`enqueue_comm_char`, `service_comm_rx_queue`) — real RS-232 hardware,
not just a GPIB-only board. Worth checking whether `[0x629]` (the mode
flag gating several comm-ROM routines, tentatively "GPIB vs RS-232")
might actually be a DTE-vs-DCE port select instead, or a third
independent thing — not yet re-examined against this.

**PARAMETERS — a 10-position DIP switch**, individually numbered 1-10,
each a 0/1 slide. This is almost certainly the hardware source of some
of the comm ROM's configuration bytes read at startup (candidates:
baud rate, parity mode — ties to `enqueue_comm_char`'s `[0x4ED]`
parity-mode byte and `send_serial_newline`'s `[0x4EF]` CR/LF option —
data bits, stop bits, GPIB primary address, DTE/DCE select, talk-only/
listen-only mode). **Not yet traced**: no code found so far that reads
a literal DIP-switch I/O port — worth searching for an 8-bit input
port read early in the comm ROM's init path (`init_comm_channel_state_a`/
`init_comm_rx_queue_and_ready_flags`, or the comm ROM's own boot stub)
that could source `[0x4ED]`/`[0x4EF]`/`[0x629]`/the GPIB address table
`SUB_97905` reads from.

**AUXILIARY CONNECTOR — 9-pin D-sub**, pins labeled: `RELAY N.O.`,
`RELAY COMM`, `RELAY N.C.`, `+4.2 VDC`, `SIG GND`, `SHIELD GND`,
`EXT CLK`, `X`, `Y`. This is an **analog X-Y plotter/chart-recorder
interface** — a relay (presumably pen-lift, normally-open/normally-
closed/common contacts) plus separate analog `X`/`Y` output pins. This
directly corroborates the **HPGL/plot rendering pipeline** already
traced in the main ROM: `scale_and_plot_point`, `update_plot_position`/
`plot_line_to` (which track HPGL PU/PD — pen-up/pen-down — state in
`[0x6CA]`), and `reset_all_channel_plot_caches`. The pen-lift relay is
the real-hardware realization of "PU"/"PD": when the firmware's PU/PD
state changes, it very likely also toggles this relay via one of the
still-unidentified I/O ports/shift-register writes (`write_hw_shift_
register`, ports `0xD1`/`0xC4`, is the leading candidate — worth
revisiting with "drives the AUX connector's pen relay" as a specific
hypothesis, rather than the vaguer "front-panel setting" guess
currently in `MEMORY_MAP.md`). `EXT CLK` and `+4.2 VDC` aren't yet
tied to anything in the disassembly.

**Caution label**: "≤25Vpk and <100mA ABS, MAX applied to any
connector" — a hardware safety limit, not firmware-relevant.

## Front panel (`hardware/photos/front_panel.jpg`)

Two stacked Tektronix 2230 units are visible (plus an unrelated bench
DMM above them — not part of the 2230, ignore for this project). Key
control groups, useful for cross-referencing the self-test/menu work:

- **VERTICAL MODE**: `CH1` / `BOTH` / `CH2`, `X-Y`, `BW LIMIT`,
  `ADD` / `ALT` / `CHOP` — the `X-Y` switch is the physical control
  behind the `[0x6C1]`/`[0x6C0]`-driven X-Y plotting path already found
  in `SUB_F0C2A` (`and ax,3; cmp ax,2` → X-Y mode) and `reset_plot_
  home_or_acq`. Confirms X-Y is a real, physically-selected vertical
  mode, not just an HPGL output quirk.
- **HORIZONTAL MODE**: `A` / `BOTH` / `B` — ties to the dual-timebase
  (`A`/`B` sweep) logic referenced throughout the acquisition code
  (e.g. `[0x54A]`/`[0x1BEC]` timeout/buffer-size handling in
  `handle_acq_mode_change`).
- **ACQUISITION**: `1K`/`4K` (record length), `ROLL`/`SCAN`,
  `PRETRIG`/`POST TRIG`, `SAVE`/`CONTINUE`, `STORE`/`NON STORE` — good
  candidates for several still-unnamed acquisition-mode flag bits.
- **MEMORY**: `WAVEFORM REFERENCE`, `MENU SELECT` (with `DISPLAY ON/
  OFF` for waveforms 1/2/3 and `SAVE REF`) — likely ties to the menu-
  navigation cursor (`update_menu_position`) and the reference-waveform
  storage the acquisition code manages.
- **CURSORS**: `SELECT` (C1/C2), `POSITION CURS`, `SELECT WAVEFORM`.
- **A/B TRIGGER**: `SLOPE`, `LEVEL`, source (`INT`/`LINE`/`EXT`),
  coupling (`AC`/`DC`/`REJ`), `P-P AUTO`/`NORM`/`TV LINE`/`TV FIELD`,
  `SGL SWP` (`RESET`/`READY`), `TRIG'D` indicator.
- **CH1/CH2 VOLTS/DIV** (with `CAL` pull for 1X/10X probe), **A AND B
  SEC/DIV** (with `CAL`, `DLY'D SWEEP PULL x10`, `STORE ONLY B
  DISABLED`, `REPETITIVE STORE`).
- Misc: `INTENSITY A-B`, `TRACE ROTATION`, `BEAM FIND`, `STORAGE/
  READOUT INTENSITY`, `FOCUS`, `VAR HOLDOFF` (`NORM`/`MAX`), `POWER`.

**Not yet reconciled**: `TODO.md`'s open question of which physical
control each of the 3 front-panel-switch range-scan self-tests
(`selftest_front_panel_switch_a`/`_b`, `selftest_comm_option_switch`)
maps to. This photo is a good reference for that once someone traces
which `[0x4E7]`/`[0x4E8]` (front-panel button/encoder state, read by
`update_menu_position`) bit ranges correspond to which physical
switch bank.
