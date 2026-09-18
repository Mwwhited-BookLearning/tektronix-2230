# Service manual Section 9 (Diagrams): page survey

This is a **survey/index**, not a full transcription, of 34 specific
pages from the service manual's Section 9 (the full electrical
schematics), rendered as page images at
`scratchpad/diagrams_pages/page_NNN.png` (not committed - regenerate
from the source PDF if needed). It was produced to hunt for jumper
`P9107` (see `docs/f10-f12-option-installation.md` and
`TODO.md`'s jumper-hunt item) and any other hardware jumpers useful
for debugging/configuration, and to look for schematic-level
confirmation of the Interrupt Mask Latch (U1235/U1236)/Status Buffer
(U1223) wiring relevant to the DIAG-vs-`/DCD2` open question (see
`docs/options.md`).

**Calibration/limitation notice**: schematic pages were read at 4x-zoom
scan resolution with dense, crossing signal lines. This survey reports
only what's printed and legible - diagram numbers/titles, board
names, component designators, jumper labels, and block-level signal
names. It does **not** attempt wire-level netlist tracing (following a
specific trace through crossings to its far end) - that is unreliable
from a scanned schematic at this resolution and was explicitly out of
scope. Where a reading is uncertain, it's flagged as such rather than
stated as fact.

**Diagram numbering** (confirmed from the two detailed block-diagram
foldouts, Figures 9-4 and 9-5, found among these pages - see below):

| # | Title | Board |
|---|-------|-------|
| 1 | CH1 and CH2 Attenuators | A2 Attenuator |
| 2 | Vertical Preamplifiers and Channel Switching | A1 Main |
| 3 | Vertical Output Amplifier | A1 Main |
| 4 | Triggering | A1 Main / A3 Front Panel |
| 5 | A Sweep Generator and Logic | A13 Sweep Interface / A4 Timing |
| 6 | B Timing and Alternate B Sweep | A13 / A5 Alternate Sweep |
| 7 | Horizontal Output Amplifier / Probe Adjust / XY Amplifier | A1 / A16 Sweep Reference |
| 8 | Power Input, Preregulator and Inverter | A1 / A6 Line Filter / A18 Thermal |
| 9 | Power Supply Secondaries, Z-Axis and CRT | A1 |
| 10 | Analog Power Distribution | (multiple) |
| 11/12 | Storage/I-O wiring interconnect (unlabeled on the pages read) | A1-A3 / A10 |
| 13 | Storage Power Distribution | A10 Storage |
| 14 | Microprocessor and Store Panel Controls | A10 Storage |
| 15 | Digital Display | A10 Storage |
| 16 | Storage Acquisition | A10 Storage |
| 17 | Acquisition Memory | A10 Storage |
| 18 | Digital Timebase | A10 Storage |
| 19/21 | Status ADC and Bus Interface / I-O + Vector Gen power distribution | A11A1 I/O |
| 20 | Vector Generator | A11A2 Vector Generator |
| 22 | XY Plotter Board | A20 |
| 23 | RS-232 Option Board | A21 |
| 24 | GPIB Option Board | A22 |
| 25 | Option Memory | A23 |

## Page-by-page notes

### page_008.png - Diagram 1: CH1 and CH2 Attenuators
Board A2 Attenuator (A14/A15 CH1/CH2 Logic referenced). CH1/CH2
High-Z attenuators and input coupling, Buffer Amplifiers, Low-Z
attenuators, Paraphase amplifiers. Clearly legible. No jumpers. No
comm/Interrupt Mask Latch content.

### page_015.png - Diagram 2: Vertical Preamplifiers and Channel Switching
Board A1 Main (A17 Position Interface, A3 Front Panel referenced).
CH1/CH2 preamps (U130/U180), position signal conditioning, alt sync
mux (U7201). Clearly legible. No jumpers. No comm content.

### page_018.png - Diagram 3: Vertical Output Amplifier
Board A1 Main. Delay line driver/delay line, vertical output
amplifier, A/B sweep separator, BW limit. Clearly legible. No
jumpers. No comm content.

### page_023.png - Diagram 4: Triggering
Board A1 Main / A3 Front Panel. CH1/CH2 trigger amplifiers, trigger
switch, internal trigger amplifier, P-P auto trigger level, A trigger
level comparator, external trigger amplifier. Clearly legible. No
jumpers. No comm content.

### page_029.png - Diagram 5: A Sweep Generator and Logic
Board A13 Sweep Interface / A4 Timing. A sweep logic, A timing switch
and RC, auto baseline, A Miller sweep, holdoff timing, A end-of-sweep
comparator. Clearly legible. No jumpers. No comm content.

### page_036.png - Diagram 6: B Timing and Alternate B Sweep
Board A13 / A5 Alternate Sweep. B sweep logic, B trigger level
comparator/Schmitt trigger, B timing switch and RC, B Miller sweep,
delay time position comparator, run-after-delay. Clearly legible. No
jumpers. No comm content.

### page_040.png - Diagram 7: Horizontal Output Amplifier
Board A1 Main / A16 Sweep Reference. Sweep ref, horizontal preamp,
horizontal mux, horizontal output amplifier, XY amplifier, probe
adjust. Clearly legible. No jumpers. No comm content.

### page_045.png - Diagram 8: Power Input, Preregulator and Inverter
Board A1 Main / A6 Line Filter / A18 Thermal. Line filter, bridge
rectifier, preregulator (oscillator U930), inverter. Includes a
mains-safety WARNING block (source AC potential present). Clearly
legible. No jumpers (no signal/config jumpers - this is the mains
power stage). No comm content.

### page_050.png - Diagram 9: Power Supply Secondaries, Z-Axis and CRT
Board A1 Main. Unblanking logic, Z-axis amp, HV multiplier, DC
restorer, LV power supplies, intensity/focus. Clearly legible. No
jumpers. No comm content.

### page_052.png - Diagram 10: Analog Power Distribution
Multi-board power-rail fan-out (A1/A2/A3/A4/A5/A13 references).
Clearly legible. No jumpers (distribution wiring only). No comm
content.

### page_054.png - Diagram 13: Storage Power Distribution
Board A10 Storage. Includes the lithium-battery WARNING block
(BT1101, 3.0V) and RAM address decoding table. Clearly legible. No
jumpers. No comm content.

### page_056.png - Storage Wiring Interconnect
Board A10 Storage. Pure interconnect/pinout page (signal-name/
conductor-number cross-reference table for cable W9000 etc.) - the
diagram-number diamond was not clearly legible on this tile, but
content and position place it between diagrams 10 and 13. Clearly
legible otherwise. No jumpers. No comm content.

### page_058.png - Input/Output Wiring Interconnect
Boards A1 Main / A3 Front Panel. Another pure interconnect/pinout
page (signal-name/conductor-number table for connectors P8110-P8130
etc.), diagram-number diamond not clearly legible. Clearly legible
otherwise. No jumpers. No comm content.

### page_065.png - Diagram 14: Microprocessor and Store Panel Controls
Board A10 Storage. **Jumper-relevant page** - see "Jumper findings"
below for P9107/P9105 detail. Contains U9111 (8-bit microprocessor),
U9109/U9110 (ROM), U9104 (Clock Generator and Osc), U9104 MPU
Clock/Reset/Timer block, decoder/latch-and-buffer logic. Mostly
legible; the jumper cluster near U9104/U9103 required a closer second
pass (see below) due to component density at this zoom level. No
comm/Interrupt-Mask-Latch content (that circuitry is comm-ROM-only,
found later on page 114).

### page_068.png - Diagram 15: Digital Display
Board A10 Storage. Display controller, XDAC/YDAC, RAM (U9201-3,
U9231-3 etc.), memory chip-select decode table. Clearly legible. No
jumpers. No comm content.

### page_072.png - Figure 9-5 tile: Storage Acquisition (16) / Vector Gen Board (20)
Block-diagram foldout tile (not a full schematic) showing the
Storage Acquisition block (channel switch, S/H, A/D converter, strobe
gen) and the Vector Gen Board block (horizontal/vertical vector
generators, plot drive, signal conditioning). Clearly legible. No
jumpers. No comm content (but establishes the block-level layout used
to number later diagrams).

### page_073.png - Figure 9-5 tile: Acquisition Memory (17) / I/O Board (19)/(21)
Block-diagram foldout tile. Acquisition Mode Register, Acquisition
Clock Decoder, Swap Registers & Acquisition Memory, Clock
Generator/Divider Register/B Delay Timer, and the I/O Board's
Vertical/Horizontal Stat Mux, Status A/D, Instrument Status Port
blocks (diagram 19/21 label visible at bottom). Clearly legible. No
jumpers. No comm content.

### page_074.png - Figure 9-5 tile: Digital Timebase (18) / Communication Option (22-25) header
Block-diagram foldout tile. Digital Timebase block (record counter,
address decode, time base divider, interrupt logic) on the left; on
the right, the **top of the Communication Option/XY Plotter block**
begins, listing its bus interface signals (RD, WR, INTR, RESET, DEN,
IRST, RDVI, A0-A13/A18, A DATA BUS, BLK0-3/COMM SEG/IO SEG, EXT CLOCK,
PLOT SIGNALS, 6.7 MHZCLK) but not yet its internal detail. Clearly
legible. No jumpers.

### page_075.png - Figure 9-5 tile (title page): Communication Option/XY Plotter (22/23/24/25) / Microprocessor (14) / Digital Display (15)
The full title block "**Figure 9-5. Detailed Storage block diagram**"
appears here, confirming the diagram-numbering scheme. The
Communication Option/XY Plotter block is shown as a single box
labeled with **all four diamonds `22 23 24 25`** together (i.e. this
one functional block covers all four detailed schematics read later:
XY Plotter, RS-232, GPIB, and Option Memory), with external I/O only
(EXT CLOCK, X/Y, PEN DOWN) - no internal detail. Also shows
Microprocessor and Store Panel Controls (14) and Digital Display (15)
blocks. Clearly legible. No jumpers.

### page_076.png - Figure 9-4 tile: CH1/CH2 Attenuators (1) / A Sweep (5) / B Timing (6)
Block-diagram foldout tile (a second, separate foldout - "Detailed
**Analog** block diagram", distinct from the Figure 9-5 Storage one
above). Clearly legible. No jumpers. No comm content.

### page_077.png - Figure 9-4 tile: Vertical Preamps (2) / Vertical Output (3) / Sweep Ref (7)
Block-diagram foldout tile, includes the "Figure 9-4. Detailed Analog
block diagram" caption at bottom. Clearly legible. No jumpers. No
comm content.

### page_078.png - Figure 9-4 tile: Triggering (4) / Power Input-Preregulator-Inverter / XY Amplifier (7)
Block-diagram foldout tile. Clearly legible. No jumpers. No comm
content.

### page_079.png - Figure 9-4 tile (title page): CRT / Z-Axis Interface (9) / Power Supply
Title block "4999-21" visible; completes the Figure 9-4 foldout
(CRT V9870, Z-axis interface, HV multiplier, transformer/LV power
supplies). Clearly legible. No jumpers. No comm content.

### page_084.png - Diagram 16: Storage Acquisition
Board A10 Storage. Channel Switch, Sample-and-Hold, A/D Converter,
Strobe Generator, Diagnostics Code Generator. Clearly legible. No
jumpers. No comm content.

### page_088.png - Diagram 17: Acquisition Memory
Board A10 Storage. Swap Registers, MIN/MAX Registers, Acquisition
Mode Register, Acquisition Clock Decoder, Microprocessor Access.
Clearly legible. No jumpers. No comm content.

### page_091.png - Diagram 18 support: parts list/cross-reference + SN-restricted partial schematic
Board A10 Storage. A parts-list/cross-reference table plus a partial
schematic variant marked "EFF: SN B020099 & BELOW" (i.e. an
early-serial-number-only circuit variant). Legible. No jumpers. No
comm content.

### page_092.png - Diagram 18: Digital Timebase
Board A10 Storage. Clock Generator, TB-Mode Register, Divider
Register, Time Base Divider, Interrupt Logic, B Delay Timer. Clearly
legible. No jumpers. No comm content.

### page_097.png - Diagram 19: Status ADC and Bus Interface
Board A11A1 Input/Output. Vertical/Horizontal Status Mux, A/D
Converter (U6105 ADC1001), A/D Control Register and Logic, Instrument
Status Port. Clearly legible. No jumpers. No comm/Interrupt-Mask-
Latch content (this is the analog-status ADC subsystem, not the comm
option).

### page_103.png - Diagram 20: Vector Generator
Board A11A2 Vector Generator. Vertical/Horizontal Vector Amp,
Vertical/Horizontal Vector Generator, Plot Drive, Signal Conditioning.
Clearly legible. No jumpers. No comm content.

### page_106.png - Diagram 21: Input/Output Board and Vector Generator Board Power Distribution
Boards A11A1/A11A2. Power-rail distribution only. Clearly legible.
No jumpers. No comm content.

### page_111.png - Diagram 22: XY Plotter Board
Board A20 XY Plotter. External Clock, Pen-Down Circuit, X/Y
Amplifiers, Power Supplies. Auxiliary connector J1011. Clearly
legible. **No jumper symbols found** (J1011 is a fixed connector, not
a jumper). No Interrupt-Mask-Latch/Status-Buffer content (that's on
the RS-232 board, page 114).

### page_114.png - Diagram 23: RS-232 Option Board - **key page for the DIAG/DCD2 question**
Board A21 RS-232 Option. See "Interrupt Mask Latch / Status Buffer
findings" below - this page shows the actual **U1235 Interrupt Mask
Latch** (labeled 74HCT259 on this print) and **U1223 Status Buffer**
(74LS541) blocks, plus Interrupt Circuitry, Parameter Buffer, UART
(U1251, 8251), Line Drivers, and the RS-232 DCE connector J1211.
Legible overall, though the interrupt-circuitry signal-name cluster
required close reading. **No jumper symbols found** on this page.

### page_117.png - Diagram 24: GPIB Option Board
Board A22 GPIB Option. GPIB Controller (U1351, TMS9914), S-Line
Drivers, Status Buffer, Parameter Buffer, Clock Divider and
Diagnostic Latch, GPIB connector J1321. Clearly legible. **No jumper
symbols found.** No Interrupt Mask Latch on this board (GPIB uses a
different interrupt scheme via the TMS9914 controller itself).

### page_121.png - Diagram 25: Option Memory
Board A23 Option Memory. RAM array (U1118/U1128/U1138/U1148, 8K x 8
each), Power-Sense circuit, lithium battery (BT1101, 3.0V) with a
WARNING block, address decoding, memory map table (8000-BFFF /
0000-3FFF / 8000-FFFF ranges shown). Clearly legible. No jumpers. No
Interrupt-Mask-Latch/Status-Buffer content (this board is pure
RAM/battery-backup, shared by whichever comm option is installed).

## Jumper findings

**P9107 and P9105** (Diagram 14, page 65, Microprocessor and Store
Panel Controls, board A10) - found near the MPU Clock/Reset/Timer
block (U9104 Clock Generator and Osc, with associated R9107 270K /
C9107 3.3µF RC network matching the reset-timing description already
in `docs/theory-of-operation.md`):

- **P9107**: a jumper with printed **ON**/**OFF** position labels,
  sitting directly next to the U9104 reset/clock RC network
  (R9107/C9107). This is consistent with `f10-f12-option-installation.
  md`'s note that P9107 gets moved when a comm option is installed,
  and with it being reset/clock-timing-related rather than a data
  jumper. Read with reasonable confidence on a closer second pass, but
  the surrounding area is visually dense (many small components in a
  tight cluster) - a physical-board photo cross-check would raise
  confidence further before treating this as settled.
- **P9105**: a separate jumper near U9103A/B, printed with **TEST**/
  **NORM** labels and an accompanying note "**REMOVE TEST FROM P9105
  IF 1 AND 3 SWAPPED**" (implying a 3-pin header where pins 1 and 3
  can be swapped, selecting between test and normal operation for a
  signal feeding toward the display controller path, e.g. `ALE TO
  U920B-2`).

**P9104 was not found** as a jumper designator on any of the 34 pages
surveyed. (U9104 is a component - the Clock Generator/Oscillator IC -
not a jumper; nothing labeled `P9104` was seen.) It's possible this
designator doesn't exist and the earlier reference conflated it with
P9105/P9107, or it exists on a page outside this 34-page selection.

No other jumper symbols (P-prefixed 2/3-pin headers with position
labels) were found on any of the remaining 32 pages - the analog
boards (diagrams 1-10) contain only fixed components and cable
connectors, and the comm-option boards (22-25) likewise show only
fixed connectors (J1011, J1211, J1321) with no jumpers.

## Interrupt Mask Latch / Status Buffer findings (DIAG vs /DCD2 question)

Page 114 (Diagram 23, RS-232 Option Board, A21) is the schematic that
actually contains this circuitry. Findings, stated with the netlist
caveat above in mind (labels/designators are trustworthy; exact pin-
to-pin wire tracing was not attempted):

- The **Interrupt Mask Latch** block is built around **U1235**,
  printed as a **74HCT259** (an 8-bit addressable latch) on this
  schematic - one digit off from this project's prior references to
  "U1236" in `docs/options.md`/disassembly notes. This is either a
  transcription slip in the earlier primary-source OCR, a genuine
  off-by-one in a different manual revision, or two adjacent parts
  that serve related roles - worth a targeted re-check against the
  options-manual OCR (`docs/options.md`) rather than assuming the
  schematic is authoritative over the manual text.
- The **Status Buffer** block is built around **U1223**, printed as a
  **74LS541** - this matches the project's existing findings exactly
  (no discrepancy here).
- In the signal-name cluster between the Interrupt Circuitry and
  Status Buffer blocks, the following individual signal names are
  legible: **`TBRE`**, **`RLSD`**, and **`DIAG`** (plus `DTR`/`INTR
  MASK`-related labels near U1224). These appear as separate named
  signals feeding toward the Status Buffer, consistent with
  `docs/options.md`'s Table 7-36 status-byte bit map (bit2=TBRE,
  bit6=DIAG, bit7=`/DCD2`).
- **`RLSD`** ("Received Line Signal Detect") is the standard RS-232
  name for what the firmware/manual calls `/DCD2` (Data Carrier
  Detect) - so this schematic corroborates that `DIAG` and the
  DCD-related signal (`RLSD`) are two genuinely distinct, separately-
  named signal lines on the board, not the same net under two names.
  That supports the existing hypothesis that bit 6 (DIAG) and bit 7
  (`/DCD2`/RLSD) are electrically independent - but this survey did
  **not** trace either signal's exact wire path back to a specific
  Interrupt-Mask-Latch output pin (e.g. confirming "DIAG = latch
  output 3D" at the wire level) - that would require the netlist-
  tracing effort explicitly out of scope here. It does **not** resolve
  which one actually toggles during the self-test's read-write-read
  sequence; it only confirms both signals exist as separate, named
  nets on this schematic, right next to each other in the same signal
  cluster.
- The GPIB Option Board (Diagram 24, page 117) has its own Status
  Buffer/Parameter Buffer but **no** Interrupt Mask Latch - its
  interrupt handling goes through the TMS9914 GPIB controller (U1351)
  directly, confirming the Interrupt Mask Latch is RS-232-specific,
  as the existing docs already state.
