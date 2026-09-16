# Hardware reference photos

Primary-source photos of the physical unit, taken by the user, to
ground the firmware reverse-engineering against real controls and
connectors instead of guesswork. Images live in `hardware/photos/`.
Add more files here (or split this doc further) as more photos come
in — don't let it become one unmanageable page.

## Manuals (`hardware/070-4998-02.pdf`, `hardware/2230 .pdf`, `hardware/070-6090-00.pdf`)

Provided by the user 2026-09-13 (tracked via git-lfs given their size -
~8.7MB and ~23MB): the **070-4998-02 Operator Manual** and the **2230
Service Manual** (Artek Media scan of the original 1986 Tektronix
manual, OCR'd - expect occasional OCR errors like "tlon" for "tion").
This is now the primary source for hardware facts, superseding
inference from code wherever the two disagree - see `MEMORY_MAP.md`'s
"Confirmed regions" table for the address/register map pulled from the
service manual's Section 3 (Theory of Operation, Table 3-1 "Memory
Space Allocation") and Section 6 (Maintenance, Tables 6-16 through
6-23, the front-panel/exerciser register tables). Read via `pdftotext
-layout` (no `pdfimages`/`pdfinfo` available in this environment, but
`pip install pymupdf` works and can render pages/extract images if a
markdown+figure transcription is ever built - see `TODO.md`).

Also provided same day: **`070-6090-00`**, the short (6-page) **F10
(GPIB)/F12 (RS-232) option installation instructions** - an
image-only scan with no text layer, so read via `pymupdf` page
rendering (`page.get_pixmap()`) rather than `pdftotext`. Key finds: the
"2 ROMs" shipped with either option are **replacement main system
ROMs** (`U9109`/`U9110`), not a second comm-board ROM - installing the
option requires the main-ROM firmware version to match; and installing
either option requires **moving jumper `P9107` on the Storage circuit
board** to a new position - a strong candidate for why live firmware
comm-detection might fail on a unit where this wasn't done correctly
(see `docs/comm-rom/rs232-early-investigation.md`'s "Follow-up live hardware session").

## Two physical units, running DIFFERENT ROM revisions - confirmed via `/DIAGNOSTICS/EXERCISERS/CONFIGURATION`

Photographed 2026-09-13, a menu screen not previously documented in
the menu tree below (add it there too - lives under `EXERCISERS`,
alongside `IO` and `A_TO_D_TESTS`). It prints each installed ROM's own
name/designator/part-revision directly off its self-ID header:

- **Scope 1** (referred to as "the first oscilloscope" in this
  session's live testing): `sys_rom_0`=`160-3633-13`, `sys_rom_1`=
  `160-3532-13`, `comm_rom_0`=`160-2998-13`. **Runs the `-13` revision
  of all three ROMs.**
- **Scope 2** ("the second oscilloscope"): `sys_rom_0`=`160-3633-14`,
  `sys_rom_1`=`160-3532-14`, `comm_rom_0`=`160-2998-14`. **Runs `-14`.**

**Why this matters**: this project's disassembly/annotation work
(`FUNCTIONS.md`, `VARIABLES.md`, all the named comm-ROM routines like
`detect_comm_option_hw`, `read_dip_switches_serial_config`, `selftest_
comm_loopback_a/b`) has been done exclusively against the **`-14`**
binaries (`disasm/gen_disasm_x86.py`'s chip config points at `-14`
explicitly). Per `docs/architecture/cpu-and-language.md`'s "Confirmed facts": the two main
ROMs (`3532`/`3633`) are **byte-identical between `-13`/`-14` except a
4-byte ID header** - so every main-ROM-based finding (front panel,
`SWB1`/`SWB2`, `AD DATA`, self-tests, menu system) applies equally to
both scopes regardless of revision, no caveat needed there. But the
**comm ROM (`2998`) genuinely differs between `-13`/`-14` in two
~16KB-aligned regions** - a real functional difference, not just a
header, and **not yet analyzed in x86 terms** (see `docs/architecture/
cpu-and-language.md`'s "Confirmed facts" section).

This means **Scope 1's actual running comm-ROM code (`-13`) has never
been disassembled by this project** - every comm-ROM address/function
name used in today's live RS-232 troubleshooting (see `disasm/
NOTES.md`'s "Follow-up live hardware session") was derived from `-14`,
which only Scope 2 actually runs. The comm-detection-failure theory
from that session is directly evidenced on Scope 2 (`-14`, the
analyzed revision) getting the same `UNTESTED`/silent results as
Scope 1 - consistent with the theory holding on the revision we can
actually verify - but Scope 1's specific failure could in principle
have a different root cause hiding in the unanalyzed `-13` diff
regions. Diffing `160-2998-13.bin` against `-14.bin` to see exactly
what changed is a good next step if the comm-detection puzzle is
picked up again - see `TODO.md`.

**Scope 1's `CH1`/`CH2` DAC connections are currently swapped, noted
2026-09-16**: after repairing Scope 1's power supply, the user
reconnected `CH1`/`CH2` to the acquisition DACs (see `TODO.md`'s
page-415 reminder - the acquisition memory logic these DACs feed)
swapped relative to each other. **The analog front-end stages
themselves are confirmed correct** - this is specifically a DAC-
connection swap, not a front-end/attenuator miswiring. Needs Scope 1
reopened to fix. Until then, treat any `CH1`-vs-`CH2`-specific
live-test result gathered from Scope 1 during this window with that
in mind - Scope 2 is unaffected.

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
(`get_xon_xoff_byte`, `enqueue_comm_char`, `service_comm_tx_queue`) —
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
side of this exact switch bank.

**Full switch mapping now RESOLVED - straight from the operator's
manual's Table 7-11/7-12/7-13 (Section 7, "Options and Accessories"),
not inferred from code.** RS-232 mode (`read_dip_switches_serial_
config`):

| Switch(es) | Function |
|---|---|
| 1,2,3,4 | Baud rate - 4-bit code, **switch 4 = MSB (weight 8), switch 1 = LSB (weight 1)**. See rate table below |
| 5 | Parity enable: `0`=disabled (8-bit data word, no parity), `1`=enabled (7-bit data + parity bit per switches 6/7) |
| 6,7 | Parity type when enabled (Table 7-13): `00`=ODD, `10`=EVEN, `01`=MARK, `11`=SPACE |
| 8 | Line terminator: `0`=**CR only** (accept only CR, send CR), `1`=**CR-LF** (accept CR-LF or LF, send CR-LF) |
| 9,10 | Printer/plotter device (power-on default only, changeable later via `COMM` menu): `00`=HP-GL, `10`=Epson, `01`=ThinkJet, `11`=X-Y Plotter |

**CORRECTED 2026-09-14** - the user found they'd been misreading Table
7-12's own column header, which lists the bit order as "`4321`"
(switch 4 first = MSB) rather than "switches 1 through 4" in reading
order. The original text below is kept struck through for the history
trail; the corrected weighting matches the GPIB address table's own
explicit `switch1=weight1`...`switch4=weight8` convention exactly (see
below) - both tables use the same ascending-from-switch-1 weighting,
which is the detail this project got backwards the first time.

Baud rate code (**switch 4,3,2,1 read in that order, MSB→LSB**) → rate:
`0000`=50, `0001`=75, `0010`=110, `0011`=134.5, `0100`=150, `0101`=300,
`0110`=**600**, `0111`=**1200**, `1000`=1800, `1001`=2000, `1010`=2400,
`1011`=3600, `1100`=4800, `1101`=7200, `1110`=**9600**, `1111`=Off Line.
~~Baud rate code (switches 1234, MSB-first) → rate: `0000`=50...~~ *(wrong
bit order, superseded above)*

**Live-tested, and this actually caught a real user error, 2026-09-14**:
the unit's switches read `0110000000` (switch4,3,2,1=`0110`=**600 baud**
- a palindrome, so it happened to read correctly under either bit-order
assumption) and, after flipping switch 1, `1110000000`. Read under the
**wrong** (original) assumption this looked like `1110`=**9600**; under
the **correct** order (switch4,3,2,1=`0111`) it's actually
**1200 baud** - and the user confirmed by direct observation that
`1110000000` really was running at 1200, not 9600. They corrected the
switches to `0111000000` (switch4,3,2,1=`1110`=**9600**, correctly this
time) and confirmed 9600 now reads correctly. The `0110000000`/600-baud
case being a bit-palindrome is exactly why the error went undetected
for several exchanges - it validated under both interpretations, while
the very next test (`1110000000`) silently broke without an obvious
symptom (no crash, just a real but wrong baud rate) until manually
checked against the actual reading. Also still resolves the earlier
open "how does the scope know CR vs LF" question: switch 8 was `0` in
both readings, meaning **CR-only** is the currently selected
terminator, unaffected by this baud-rate bit-order correction - exactly
what PuTTY sends by default, so terminator mismatch is still not what's
blocking command responses.

**The same physical 10-switch bank means something different in GPIB
mode** (Table 7-6, for **Option 10** boards - a *different* option
board/product variant than this unit's **Option 12** RS-232 board,
though the comm ROM's code contains both decoders, `read_dip_switches_
gpib_config` alongside `read_dip_switches_serial_config` - possibly
what the still-open `[0x1B83]` "which hardware variant" detection
question is actually distinguishing between):

| Switch(es) | Function |
|---|---|
| 1,2,3,4,5 | GPIB primary address, binary-weighted: switch1=weight1, switch2=weight2, switch3=weight4, switch4=weight8, switch5=weight16 (address 0-30) |
| 6 | Terminator: `0`=EOI only, `1`=LF or EOI |
| 7 | `0`=no function, `1`=**LON** (listen-only) |
| 8 | `0`=no function, `1`=**TON** (talk-only) |
| 9,10 | Printer/plotter device - same `00`/`10`/`01`/`11` = HP-GL/Epson/ThinkJet/X-Y Plotter mapping as the RS-232 table |

Talk/listen combinations: neither `LON` nor `TON` set = normal
talk-and-listen mode; `LON` only = listen-only; `TON` only = talk-only;
**both** set = OFF BUS (equivalent to address 31, taken off the bus
entirely).

Also in the manual (Appendix B, not yet transcribed here): ready-made
switch-position tables for specific common printers/plotters (HP-GL
compatible plotters, Epson printers, HP ThinkJet) - lower priority
since they're third-party compatibility presets rather than the core
switch-to-function mapping, but available in `hardware/070-4998-02.pdf`
if ever needed.

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

### Internal board layout — two physically separate boards, from the user's own service-manual schematic trace (2026-09-16)

The comm option is **two separate boards**, not one — confirmed by
directly tracing the service manual's own schematics (not just the
Theory of Operation prose used elsewhere in this file):

1. **"Option Memory" board** — a RAM board shared between the RS-232
   and GPIB riser options (the user reviewed only the RS-232 riser in
   detail — "I dont care about the GPIB riser as I dont have one").
   Carries the 4 static-RAM chips backing `MEMORY_MAP.md`'s confirmed
   `0x88000-0x8FFFF` "Option nonvolatile RAM" range, plus its own
   address-decode/power-sense support chips:

   | IC | Part | Function | Address range |
   |---|---|---|---|
   | `U118` | (static RAM) | RAM | `0x88000-0x89FFF` |
   | `U128` | (static RAM) | RAM | `0x8A000-0x8BFFF` |
   | `U138` | (static RAM) | RAM | `0x8C000-0x8DFFF` |
   | `U148` | (static RAM) | RAM | `0x8E000-0x8FFFF` |
   | `U1122` | **ICL8212** | programmable voltage comparator/detector - part of the power-sense circuit | — |
   | `U1132` | hex inverter | logic buffering; part of this chip is specifically used for **power-on glitch detection feeding the reset circuit** | — |
   | `U1142` | LM339 (quad comparator) | power sense - see below for the specific section-by-section breakdown | — |
   | `U1162` | 74LS139 | address decoder (this board's own, separate from the RS-232 board's `U1245`) | — |

   **Power-sense/interrupt/reset analog circuit, from the same trace
   (2026-09-16 follow-up)**: `/PWR INT` is generated by `U1142B`
   (one section of the `LM339` quad comparator) comparing the sensed
   power-supply line against a reference voltage from an **`LM329`**
   (a precision bandgap voltage reference). `INTR` and `/RST` are each
   generated by **similar analog comparator circuits** (presumably
   other `LM339` sections, not yet individually traced) - all 3 use
   the same "analog level vs. reference" technique, just against
   different signals/thresholds. `U1132`'s glitch-detection role feeds
   specifically into the reset path, filtering spurious resets from
   power-supply noise/transients rather than genuine power loss. Worth
   reconciling against this same board's earlier-recorded note "`PWR
   INT` is a logic OR of Chassis GND + `/WR`" (used as `U1235`'s own
   chip-select gating input, `G8`) - that may describe a *different*,
   later-stage combination of this analog signal with others rather
   than its ultimate origin, not yet fully worked out which.

2. **RS-232 option riser** — the RS-232-specific logic, including the
   UART and its register-buffer support chips:

   | IC | Part | Function |
   |---|---|---|
   | `U1242` | 6116P (2048x8 RAM) | comm-option RAM (separate from the shared Option Memory board above) |
   | `U1244` | 27512 (65536x8 EPROM) | the `160-2998` comm ROM itself |
   | `U1251` | **82C52** | the UART (`MEMORY_MAP.md`'s already-confirmed `U1251` — part number now identified) |
   | `U1225` | MC1489 | TTL/RS-232C transmitter |
   | `U1224` | MC1489 | TTL/RS-232C receiver |
   | `U1244F` | 74LS04A | UART reset |
   | `U1232` | 74LS245 | `AD0-7` -> `BD0-7` data-bus buffer |
   | `U1241` | 74LS541 | `A0-7` -> `BA0-7` address-bus buffer |
   | `U1233` | 74LS541 | `A8-13` -> `BA8-13` address-bus buffer |
   | `U1244A` | 74LS04A | `BA12` inverter -> `U1245` pin 14 |
   | `U1244B` | 74LS04A | `BA13` inverter -> `U1245` pin 13 |
   | `U1222` | 74LS541 | **Parameter Buffer** (`0x406BC`, "Option Parameters Latch") |
   | `U1223` | 74LS541 | **State Buffer** (`0x4067C`, "Option Status Latch") |
   | `U1235` | 74HCT259 | **Interrupt Mask Latch** (`U1236` in the service manual's own prose elsewhere — same chip, `0x406F8`-`0x406FB`) |
   | `U1245` | 74F548 | **the real address decoder** for this board's registers — see `MEMORY_MAP.md`'s "RS-232 option board" section for the full pin-out and truth table |

   See `MEMORY_MAP.md`'s "RS-232 option board: the actual chip-select
   decoder and register-buffer identities" section (added the same
   day) for the `U1245` decoder's full input/output pin mapping, the
   Parameter/State buffer bit maps, and the `U1235`/`U1236`
   Interrupt Mask Latch's `Q0`-`Q3` output mapping — all derived
   directly from this same schematic trace.

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

**Partially reconciled**: one of the 3 original "front-panel-switch
range-scan self-tests" turned out to be `selftest_tb_divider`
(`TB_DIVIDER` in the `DIAGNOSTICS/TESTS/ACQUISITION` menu above, not a
front-panel switch at all - corrected from an earlier `selftest_comm_
option_switch` guess). The remaining two, `selftest_front_panel_
switch_a`/`_b`, are still open, though `_a` is now confirmed to verify
its swept control via **ADC readback** (shares its step helper with
the dedicated `A_TO_D_TESTS` exerciser) - suggesting an analog/
potentiometer-based control, with VOLTS/DIV (9-ish detents, matching
its 0-8 sweep range) as the leading candidate. `_b` (0-0x15, 21
positions) remains unconfirmed - TIME/DIV is a plausible guess given
typically more positions, not verified. Tracing which `[0x4E7]`/
`[0x4E8]` (front-panel button/encoder state, read by `update_menu_
position`) bit ranges correspond to which physical switch bank would
settle this.

## Full menu tree walkthrough (`hardware/photos/`, 2026-09-11 batch)

The user photographed essentially the entire `ACQ_MODE_SETUP_TABLE`
menu tree and the `DIAGNOSTICS` self-test menu screen-by-screen (71
photos - see `hardware/photos/INVENTORY.md` for the per-file index).
This is by far the strongest confirmation yet of the menu-structure
and self-test-naming work done purely from disassembly. Full tree as
observed:

```
ACQ_MODE_SETUP_TABLE
├── SELECT_MODE      - UN-TRIG/TRIG x sweep-speed matrix of
│                      SAMPLE/ACCPEAK/AVERAGE/PEAKDET
├── SWP_LIMIT
├── WEIGHT
├── A_TRIG_POS       - "Points before trigger, PRE or POST" (0-4095/4K)
├── DISPLAY
│   ├── DELTA_T_MODE - DELTA_TIME / 1/DELTA_TIME
│   ├── VECTORS      - line vs. dots
│   └── SMOOTH
├── DEFAULT          - restores menu defaults
├── FORMATTING       - "Alter display of SAVE-REFs"
│   ├── TARGET_REFERENCE
│   ├── VGAIN        - (disabled in X-Y mode)
│   ├── VPOSITION
│   ├── HMAG         - 10X mag (disabled in X-Y mode)
│   └── MODE
├── PLOT             - "Control the XY plotter"
│   ├── SPEED        - pen speed
│   ├── START        - starts/stops a plot
│   ├── GRATICULE    - include graticule in plot
│   ├── AUTO         - auto-plot mode (needs XY or talk-only)
│   ├── FORMAT       - XY (analog plotter) / HPGL / EPS7 / EPS8 /
│   │                  TJET (HP ThinkJet) / RESERVED - all non-XY
│   │                  formats explicitly go out "COMM port"
│   └── SETUP        - plotter gain and offset
└── ADVANCED_FUNCTIONS
    ├── REFERENCE    - 26 non-volatile ref-waveform slots, A-Z
    │   ├── COPY
    │   ├── DELETE
    │   └── PROTECT  - LOCK/UNLOCK
    ├── COMM         - "Setup host communications"
    │   ├── DATA     - "Setup data transmission parameters"
    │   │   ├── ENCDG   - ASCII / BINARY(default) / HEX
    │   │   ├── SOURCE  - ACQ / REF1-REF3 / ...
    │   │   └── CHANNEL - CH1(default) / CH2
    │   ├── STOP_BITS
    │   └── FLOW     - flow control on/off
    ├── ACQ_MODE_SETUP_TREE - "Tree structured acquisition setup"
    │   ├── REPETITIVE        (.05-2 uS/Div)   - SAMPLE/ACCPEAK/AVERAGE
    │   ├── FAST_RECORD       (5-10 uS/Div)    - SAMPLE/ACCPEAK/AVERAGE
    │   ├── SLOW_RECORD       (20uS-50mS/Div)  - +PEAKDET
    │   ├── SLOW_TRIGGERED    (0.1-5 S/Div or EXT CLOCK) - +PEAKDET
    │   └── SLOW_UNTRIGGERED  (0.1-5 S/Div or EXT CLOCK) - SAMPLE/PEAKDET
    └── DIAGNOSTICS
        ├── TESTS
        │   ├── MEMORY     - SYS_ROM_0 / SYS_ROM_1 / COMM_ROM / ACQ_RAM
        │   ├── SYSTEM     - ACQ_ACCESS / PRC_READBACK / FP_AtoD /
        │   │                COMM_READBACK / COMM_LOOPBACK
        │   └── ACQUISITION - HS_ACQ / TB_DIVIDER / MAX_MIN_ACQ /
        │                     XY_ACQ / CLK_DELAY
        ├── CAL_AIDS   - BOX / CAL_V_POS / CAL_CLK_DLY
        └── EXERCISERS - CONFIGURATION / IO (FP_VALUES / OUT_PORTS /
                          INPUT_PORTS) / A_TO_D_TESTS (SAMPLES /
                          LINEARITY)
```

**Corrected 2026-09-14, confirmed live on a real Option-12 (RS-232)
unit**: `EXERCISERS` only has **3** children on this hardware -
`CONFIGURATION`, `IO`, `A_TO_D_TESTS` (screen photo confirms exactly
this list, `IO` expanded showing `FP_VALUES`/`OUT_PORTS`/
`INPUT_PORTS`) - this was actually already photographed 3 days earlier
(`hardware/photos/20260911_010554880_iOS.jpg`, inventoried as exactly
"DIAGNOSTICS/EXERCISERS: CONFIGURATION, IO, A_TO_D_TESTS") but never
cross-checked against the manual's claimed 5-entry list until now.
**`COM_OPTION_DEBUG` and `PICTURES` (both below, described from the
service manual text) do not exist on this unit's live menu** - the
service manual most likely covers the whole 2200 series (2211-2221,
plus 2230) in one combined document, and these two entries may be
specific to a different model, or to Option 10 (GPIB) rather than
Option 12 (RS-232) - not confirmed either way, but the manual's own
text never actually distinguishes which option a given `EXERCISERS`
entry requires. **This closes off `COM_OPTION_DEBUG` as a way to test
the `write_readout_port_byte`/UART theory on this specific hardware**
- see `docs/comm-rom/rs232-early-investigation.md`.

**Menu tree filled in further 2026-09-13**, from the service manual's
own exerciser-description text (Section 6, right after Tables 6-16/
6-17) and live photos of `CONFIGURATION`/`IO/INPUT_PORTS` (see the
section above):
- **`OUT_PORTS`**: *"All microprocessor output ports of the instrument
  are exercised by this exerciser. If entered from the menu, rotating
  the cursor knob selects either a single port or all ports at once...
  Test patterns used in each port are shown in Table 6-18 through
  Table 6-22"* (each of those tables lists a named port/register with
  its own bit-pattern sequence - e.g. Table 6-18 = `ACQ-MEM 0x48000`
  on `U3427`/`U3428`, Table 6-20 = `U4119` - the same "Time Base Mode
  Register" `detect_comm_option_hw` probes). Meant to be observed with
  an external logic probe/LED clip, not read off the CRT. Not yet
  traced to any code - no reference to its menu string found in the
  disassembly so far.
- **`COM_OPTION_DEBUG`** (manual text calls it "COM-OPTION/DEBUG"):
  *"This exerciser is used in debugging the communications option.
  Debug outputs a test message and displays any incoming messages
  (data) [on] the crt."* Would have been directly useful for this
  project's live RS-232 investigation (see `docs/comm-rom/rs232-early-
  investigation.md`) - showing incoming serial data directly on the
  scope's own screen sidesteps the adapter/cable/PC entirely as a
  diagnostic step. **Confirmed 2026-09-14: this entry does not exist
  in the live `EXERCISERS` menu** on a real Option-12 unit (see above)
  - dead end for that purpose, at least on this hardware/option
  combination.

### Menu navigation control scheme (from the operator's manual, 2026-09-13)

The operator's manual (`hardware/manuals/2230_operators/`, Section 3
"MENU SELECTED FUNCTIONS") spells out exactly which physical controls
drive `update_menu_position` (`0xE06B6`), the firmware's already-
identified menu-cursor tracker:

- **`WAVEFORM REFERENCE/MENU SELECT` switch**, held in the `MENU
  SELECT` position (button out) - while held this way, the 3 `MEMORY`
  switches (normally "display/save this reference waveform") stop
  doing that and control menu navigation instead. This is almost
  certainly the mode gate the firmware checks before treating those 3
  buttons' state as menu input.
- **`SAVE REF/►` switch** - enters the *next* (rightward/deeper) menu
  level.
- **The 3 `Menu Select/DISPLAY ON/OFF` switches** (i.e. the `MEMORY`
  switches, repurposed):
  - `◄` (leftward) - returns to the *previous* (higher) menu level.
  - `↑` - selects the previous entry in the current menu level.
  - `↓` - selects the next entry in the current menu level.

**Cross-reference to the service manual's front-panel bit names**
(`hardware/manuals/2230_service/`, Tables 6-16/6-17, `SWB2`/`U9301`):
`SWB2`'s bit names include `MENU ADV`, `MEM 1`, `MEM 2`, `MEM 3` -
`MENU ADV` is almost certainly the `SAVE REF/►` "enter next level"
button, and `MEM 1`/`MEM 2`/`MEM 3` are the 3 repurposed `Menu Select`
buttons (`◄`/`↑`/`↓`) described above. Not yet individually confirmed
against `[0x4E7]`/`[0x4E8]`'s specific bit positions in the
disassembly - see `TODO.md`'s front-panel-switch item for the next
step (trace `update_menu_position`'s bit tests against this mapping).

### Direct confirmations of existing firmware findings

- **`SYS_ROM_0`/`SYS_ROM_1`** in the `MEMORY` test menu match the ROM
  chips' own silkscreen names *exactly* (`160-3633`="sys_rom_0",
  `160-3532`="sys_rom_1") - settles any doubt about that mapping.
- **`COMM_ROM` PASSED** in the same menu is `selftest_comm_rom`'s real
  on-screen name; **`ACQ_RAM` PASSED** is `selftest_acq_ram`'s.
- **`FP_AtoD` PASSED** is `selftest_front_panel_adc`'s real name -
  confirms the front-panel A/D converter identification.
- **`COMM_READBACK` PASSED** / **`COMM_LOOPBACK` UNTESTED** are
  `selftest_comm_readback`/`selftest_comm_loopback_a`/`_b`'s real
  names - `COMM_LOOPBACK` reading UNTESTED on this unit (rather than
  PASSED/FAILED) matches the "Not installed"/untested status-bit
  encoding already confirmed in `format_selftest_result_string`.
- **`HS_ACQ`/`MAX_MIN_ACQ`/`XY_ACQ`** PASSED in the `ACQUISITION` test
  menu are `selftest_hs_acq`/`selftest_mm_acq`/`selftest_xy_acq`'s real
  names.
- **The `SAMPLE`/`ACCPEAK`/`AVERAGE`/`PEAKDET` x sweep-speed matrix**
  (`SELECT_MODE`, and its more detailed tree-structured twin
  `ACQ_MODE_SETUP_TREE`) is the on-screen shape of the acquisition-
  mode bit dispatch traced this session in `handle_acq_mode_change` -
  strong confirmation that function's bit-flag interpretation is on
  the right track.
- **`PLOT`/`FORMAT`'s non-XY options all say "COMM port"** - confirms
  HPGL/Epson/ThinkJet plot output goes out the RS-232/GPIB path (via
  `start_plot_output_task`'s `ESC @` reset - a real Epson/ThinkJet
  printer reset convention, now doubly confirmed), while only `XY`
  format drives the AUX connector's analog pen plotter directly.
- **`ACQ_MODE_SETUP_TREE`'s `EXT CLOCK` option** (on the two slowest
  sweep tiers) is the firmware-side use of the AUX connector's
  `EXT CLK` pin found in the rear-panel photo.

### New leads / open questions from this batch

- Self-test names seen here but not yet matched to a specific
  function: `TB_DIVIDER`, `CLK_DELAY` (both PASSED, under
  `DIAGNOSTICS/TESTS/ACQUISITION`), `ACQ_ACCESS`, `PRC_READBACK` (both
  PASSED, under `.../SYSTEM`), `CAL_AIDS`'s `BOX`/`CAL_V_POS`/
  `CAL_CLK_DLY`, and `EXERCISERS`'s `CONFIGURATION`/`IO`/
  `A_TO_D_TESTS`. `A_TO_D_TESTS` is a strong new lead for identifying
  the A/D converter I/O ports beyond what `selftest_front_panel_adc`
  already covers.
- `COMM/DATA/ENCDG` offers `ASCII`/`BINARY`/`HEX` waveform-data coding
  - `print_signed_decimal_serial`/`print_param_list_response` cover
  the ASCII path; no binary or hex waveform-transfer routine has been
  identified in the comm ROM yet - worth searching for one.
- `COMM/DATA/STOP_BITS`/`FLOW` and the rear-panel PARAMETERS DIP
  switch both appear to configure overlapping RS-232 parameters
  (parity, flow control) - not yet reconciled which one wins, or
  whether the DIP switch only sets power-on defaults that this menu
  can override at runtime (plausible given `set_comm_config_flag`'s
  config array is separate from the raw DIP-switch-read variables).

## Main system board interior (`hardware/photos/IMG_1412.jpg`)

A photo of the A10 main digital/acquisition board with its top board
lifted (silkscreened "REMOVE TO LIFT BOARD" / "BOARD LATCH"). Text on
densely-packed 1980s TTL silkscreens is small and photo-compression-
lossy, so treat exact chip designators below as "best read, not
service-manual-verified" until cross-checked against a clearer photo
or the schematic.

- **CPU confirmed as an Intel `P8088-2`, date-coded 1978** (the DIP-8
  chip marked `INTEL`, `P8088-2`, `1978`). The `-2` suffix is Intel's
  higher-speed grade (8 MHz vs. the base 8088's 5 MHz) - a firmware-
  invisible detail (doesn't change the ISA `disasm/` targets) but a
  nice concrete confirmation of `CONTEXT.md`'s "CPU: Intel 8088" entry
  beyond the TekWiki-sourced claim it was based on before.
- Two chips marked **Tektronix `160-3532-14`** and what reads as
  **`160-3633-14`** (the two main system ROMs this whole project
  disassembles) are visible and socketed near the CPU, exactly as
  expected.
- A **Sony-branded chip** (partial marking, something like `SONY
  20??7K`) sits near the bottom right, close to labeled `CH1`/`CH2`
  input traces - consistent with `CONTEXT.md`'s claimed Sony CX20052A
  A/D converter, but the visible part number doesn't cleanly read as
  `CX20052A` from this photo. **Update 2026-09-13**: the real service
  manual confirms the signal-acquisition ADC's schematic designator is
  **U2204**, with a spec description (8-bit, 20 Megasamples/second,
  ECL output) matching `CX20052A` exactly - this is almost certainly
  that chip, the photo's blurry part number just couldn't confirm the
  exact string. Still worth a clearer close-up if precision matters,
  but no longer a real open question.
- Hand-silkscreened **signal/test-point labels** are visible amid the
  wiring harness: `TRIGGERED`, `ACQEN A` (Acquisition Enable, channel
  A?), `EOR`, and something ending `...FULL` (partially obscured by a
  wire tie, possibly "POST FULL" or similar) - real hardware names for
  signals this project has only inferred from variable usage so far
  (e.g. `[0x54A]`'s acquisition-timeout countdown, the `PRE`/`POST`
  trigger-position wording confirmed in the `A_TRIG_POS` menu above).
  Worth a dedicated close-up photo of that harness area if the labels
  matter for future work.
- A PAL/GAL chip marked `PAL10L8-2CN` (or similar) provides custom
  glue logic - a candidate source for some of the address-decode
  behavior already inferred from the disassembly (e.g. the `0x90000`
  alias), though not confirmed.
- **Open**: the ROM sockets read as roughly `U3103`/`U3110` in this
  photo, which doesn't match `CONTEXT.md`'s TekWiki-sourced `U9109`/
  `U9110` designators - likely just a misread of small silkscreen text
  in a compressed photo, but flagged here rather than silently
  "corrected" without a clearer photo to check against.
- **No 8087 coprocessor chip observed on this board** (user direct
  visual inspection, 2026-09-13) - relevant to the still-open `TODO.md`
  question of whether `convert_sample_value`'s genuine `fmul`/`sdiv32`-
  adjacent x87 instruction implies real 8087 hardware. Since this was
  specifically the main digital/acquisition board (A10), this doesn't
  rule out an 8087 living on a different board (e.g. an acquisition
  daughter-board) if one exists - worth checking other boards' photos
  too before concluding there's no coprocessor anywhere in the unit.
  If confirmed absent everywhere, the leading interpretation is that
  `convert_sample_value`'s `fmul` path is genuinely unreachable in
  normal operation (it only runs as part of `assert_and_halt`'s panic-
  argument computation, see `docs/hardware-io/shift-register-and-
  assert.md` "Found: the firmware's assert()/panic mechanism") -
  consistent with this session's broader
  finding that this codebase has several real, unambiguous call paths
  into code/data that never actually executes on real hardware. A bare
  8086/8088 with no 8087 doesn't fault on an unrecognized `ESC` (x87)
  opcode - it's simply a no-op unless paired with `wait`/coprocessor-
  bus-cycle logic, so this wouldn't crash even if occasionally hit.
