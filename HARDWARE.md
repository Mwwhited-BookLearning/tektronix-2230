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

**Confirmed by the user**: this is just the *same* serial port wired
out to two connectors (DTE and DCE pinouts) for cabling convenience —
plug into whichever matches the far end, no null-modem adapter needed.
Not two UARTs and not a firmware-visible mode select; `[0x629]` (the
mode flag gating several comm-ROM routines) is *not* a DTE/DCE switch.
This is still a strong physical confirmation of the **RS-232
flow-control cluster** found in the comm ROM this session
(`get_xon_xoff_byte`, `enqueue_comm_char`, `service_comm_rx_queue`) —
real RS-232 hardware, not just a GPIB-only board — but `[0x629]` stays
open as "GPIB vs RS-232" (or something else), just not DTE/DCE.

**PARAMETERS — a 10-position DIP switch**, individually numbered 1-10,
each a 0/1 slide. **Found and confirmed**: `read_dip_switches_serial_
config` and `read_dip_switches_gpib_config` (comm ROM) both read two
hardware switch bytes via far pointers `[0x6DE]`/`[0x6DA]` (inverted -
active-low switches read as 0 when on) and decode them into exactly
the settings a config DIP bank would carry - a baud-rate-like code
(`[0x4EC]`), parity mode (`[0x4ED]`, the same byte `enqueue_comm_char`
uses), a CR/LF option (`[0x4EF]`, the same byte `send_serial_newline`
uses), a 5-bit GPIB primary address 0-30 (`[0x4F0]`), and further mode
flag bits (`[0x4F1]`/`[0x461]`). This is almost certainly the firmware
side of this exact switch bank. Not yet done: mapping each decoded
value back to a specific switch-position meaning (e.g. which 2 of the
10 switches select parity, which set the baud rate) - the bit-field
widths above (4 bits baud, 1+2 bits parity, 1 bit CR/LF, 5 bits GPIB
address, assorted mode bits) add up close to 10-13 bits total, roughly
consistent with two switch bytes feeding a 10-switch panel with some
switches shared/overlapping between the two decode passes.

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
