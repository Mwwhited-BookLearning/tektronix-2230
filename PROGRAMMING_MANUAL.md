# Tektronix 2230 Programming Manual (Options 10/12 command reference)

Markdown transcription of `hardware/Tektronix_2230_Programming_Manual.pdf`
(42 pages, image-only scan, no embedded text layer - transcribed by
reading each page image directly). Despite its standalone filename,
this document **is the "Options and Accessories" chapter (Section 7)
of the same 2230 Operator Manual** as `hardware/070-4998-02.pdf` -
page headers throughout read "Options and Accessories—2230 Operators"
and the page numbers continue that manual's own 7-17 through 7-50
sequence. It covers material common to **both Option 10 (GPIB) and
Option 12 (RS-232-C)** - the communication command set, waveform
transfer formats, and status/event codes.

This is the single most directly relevant primary source for the
project's live RS-232 command-parser investigation - see
`disasm/NOTES.md`'s "BREAKTHROUGH, 2026-09-14" section for how the
`STATUS 98` response the scope actually produced cross-references
Table 7-34 below.

## Command syntax rules

- **Character set**: Table 7-14 ("Readout/MESsage Command Character
  Set") and Table 7-15 (standard ASCII code chart, with GPIB
  address/command-type annotations) define the character encoding used
  by the `MESsage` command and general command parsing.
- **Abbreviation convention**: command tables show headers/arguments in
  a mix of upper/lower case. The instrument accepts any input containing
  at least the **upper-case** letters shown (case-insensitive), plus any
  subset of the shown lower-case letters appended to it - e.g. `VMO?`,
  `vmo?`, `VMOd?`, `vmod?`, `VMOde?`, `vmode?` are all equivalent. For a
  query, the `?` must immediately follow the header.
- **Headers**: a command consists of at least a header, which alone may
  be sufficient to invoke it (e.g. `INIt`, `OPC`).
- **Arguments**: if there's more to a command than the header (+ `?` for
  queries), the header must be followed by at least one space before the
  argument. A single-word argument example: `REFTo REF4`, `PLOt STArt`.
  A "link argument" (an argument of an argument) is separated from its
  parent by a colon: `ACQuisition REPetitive:SAMple`, `WFMpre XINcr:1.0E-3`.
  Multiple arguments/argument-pairs are comma-separated:
  `DATa ENCdg:BINary,CHAnnel:CH2`, `VMOde? CH1,CH2,ADD`.
- **Command separator**: multiple commands in one line are semicolon-separated,
  e.g. `DATa ENCdg:BINary,CHAnnel:CH2;WFMpre XINcr:1.0E-3`. **With Option 12,
  multiple commands (especially queries) per line are explicitly discouraged**
  (see NOTE below) - commands are interpreted and acted on as soon as a
  separator is recognized, not when the line terminator arrives, so a
  service-request response to an earlier command in the line can be lost
  or interleaved unpredictably. Multiple commands are more commonly used
  with Option 10 (GPIB).

  > **NOTE (verbatim, important for RS-232 programming)**: *"Commands
  > (and arguments to commands) are interpreted and acted on by the
  > oscilloscope as soon as a separator is recognized; the oscilloscope
  > does not wait for the message terminator (CR or CR-LF) to signal the
  > end of the command line. If one of the commands in a command line
  > requires a response for any reason (i.e., command error, illegal
  > command, or unable to do the command), the oscilloscope's
  > service-request status-byte response will be asynchronously sent. If
  > the service request is not handled correctly, the controller may not
  > be able to continue with its program."*

- **Message terminator**:
  - GPIB: EOI or LF (controller-selectable); with EOI selected, a data
    byte received with EOI asserted ends the input message, and EOI is
    asserted concurrently with the last output byte. With LF selected,
    LF (without EOI, or any EOI-asserted byte) ends an input message,
    and the instrument transmits CR-LF (LF with EOI asserted) to
    terminate its own messages.
  - RS-232-C: **CR** or **CR-LF**, selectable via the rear-panel
    PARAMETERS DIP switch (see `HARDWARE.md`). The instrument recognizes
    a **semicolon as the end-of-command terminator** and begins
    responding to the preceding command string immediately - it does
    not wait for CR/CR-LF. Messages should be limited to **one command
    per line**; incoming/outgoing messages are not stacked, and sending
    more than one command per line risks losing the response to all but
    the last command. Even single-command messages should not be
    terminated twice (e.g. don't send a trailing empty line) - the
    response to the command may be lost when the instrument sees the
    second terminator.
- **Command formatting**: commands may be upper or lower case; all data
  output from the instrument is upper case. Spaces may be used after
  commas/semicolons or after the header's own following space, for
  readability.
- **Numeric arguments** (Table 7-16):

  | Argument | Format | Examples |
  |---|---|---|
  | `<NR1>` | Integers | `+1, 2, -1, -10` |
  | `<NR2>` | Explicit decimal point | `-3.2, +5.1, 1.2` |
  | `<NR3>` | Floating point, scientific notation | `+1.E-2, 1.0E+2, 1.E-2, 0.02E+3` |

  Both signed and unsigned numbers are accepted; unsigned numbers are
  taken as positive.
- **Default/bracketed arguments**: `[default]` in square brackets means
  that argument may be omitted if the default mode is wanted.
  `<link1, link2, or link3>` in angle brackets lists valid link-argument
  choices.
- **Multiple queries in one GPIB command**: with GPIB, comma-separated
  arguments in a single query request only the named information, e.g.
  `CH1? VOLts,COUpling;`. **With RS-232-C, program controller routines to
  send only one command at a time with single arguments**, of the form
  `header argument:link argument;` - this lets the controller handle any
  asynchronous service request a command may generate before attempting
  a second command.
- **LONg mode**: with `LONg ON` (power-on default), query replies use
  full command words; with `LONg OFF`, replies use the short (capital-
  letter) form. `LONg?` returns the current state.

## Waveform transfers

The instrument transfers waveforms in **binary**, **hexadecimal**, or
**ASCII** encoding, at 8-bit or 16-bit point resolution. Record length is
1024 points (shortest) or 4096 points (longest); the largest curve
transfer is 8192 bytes (4K record, 2 bytes/point).

- **Preamble** (`WFMpre?`): attributes of the associated waveform data
  (point count, scale factors, vertical offsets, horizontal increment,
  scaling units, encoding) as an ASCII string. Example (Y/time-implied):
  `WFM WFI:"ACQ, CH1,0.5V,DC,0.2mS,SAMPLE,CRV# 1",NR.P:4096,PT.O:122,PT.F:Y,XMU:0.0E0,XOF:0,XUN:S,XIN:2.0E-6,YMU:20.0E-3,YOF:-20,YUN:V,ENC:HEX,BN.F:RP,BYT:1,BIT:8,CRV:CHK;`
- **Combined response** (`WAVfrm?`): preamble and curve data returned
  together, separated by a semicolon.
- The oscilloscope can respond with preamble only (`WFMpre?`), curve
  data only (`CURVe?`), or both together (`WAVfrm?`).

### Binary encoding

Form: `CURVE <space>%<Binary Count MSB><Binary Count LSB><Binary Data><Checksum><Terminator>`

- `CURVE` - literal string indicating curve data follows.
- `%` - header character marking the start of a binary block.
- `<Binary Count MSB/LSB>` - 2-byte length of the waveform in bytes,
  **plus the 1-byte checksum**.
- `<Binary Data>` - 256/512/1024/2048/4096 data points, each 1 byte
  (8-bit) or 2 bytes (16-bit, when averaged).
- `<Checksum>` - two's-complement of the modulo-256 sum of the
  preceding data bytes and the binary count.

Table 7-17 (8-bit) / 7-18 (16-bit) lay out a full byte-by-byte example
for a 4096-point waveform: bytes 1-7 are the literal `CURVE %` header
(ASCII `C,U,R,V,E,<SP>,%` = `67,85,82,86,69,32,37`), bytes 8-9 are the
binary count (`0x1001`=4097 for 8-bit, `0x2001`=8193 for 16-bit),
followed by the data points, then the checksum byte, then `<CR>`
(all RS-232/GPIB with `TERM=LF/EOI`) and/or `<LF>` (RS-232 with
`TERM=CR-LF`, or GPIB with `EOI` asserted on that last byte).

### Hexadecimal encoding

Same shape as binary, with these differences (Table 7-19/7-20):
1. Curve header is `"CURVE #H"` instead of `"CURVE %"`.
2. Each data point is 2 ASCII hex characters (8-bit transfer) or 4 (16-bit).
3. Byte count is sent as 4 successive ASCII hex characters (same
   underlying value as a comparable binary transfer).
4. Checksum is sent as 2 successive ASCII hex characters.

### ASCII encoding

Form: `CURVE<space>data,data,data,...,data<terminator>` - variable-length
decimal values, comma-separated (Table 7-21). Each value is 1-3 ASCII
characters (`Pt100`/`Pt10`/`Pt1` notation = hundreds/tens/units digit,
sent only as needed for the value's magnitude).

## Command tables (Tables 7-22 through 7-33)

Each command's applicability is noted above it in the source tables:
`ALL` (all 2200-family DSOs), or specific model numbers (`2230`,
`2221`, `2220`) when the command is instrument-specific. Full command
syntax is given; the fewest-characters-needed abbreviation is shown in
capitals within each header/argument word (e.g. `VMOde` - minimum `VMO`).

### Table 7-22: Vertical Commands

| Command | Description |
|---|---|
| `CH1?` *(2221/2230)* | Query only. Returns `CH1 VOL:<NR3>,COU:<AC,DC,or GND>`. `<NR3>` is VOLTS/DIV. |
| `CH1? VOLts` | Query only. Returns `CH1 VOL:<NR3>` (VOLTS/DIV incl. probe attenuation). Execution warning if the VOLTS/DIV `CAL` knob isn't in detent. |
| `CH1? COUpling` | Query only. Returns `COU:<AC, GND, or DC>`. |
| `CH2?`, `CH2? VOLts`, `CH2? COUpling` | Same as CH1 equivalents. |
| `CH2? INVert` | Query only. Returns `CH2 INV:<ON or OFF>`. |
| `VMOde?` *(ALL)* | Query only. Returns `VMO:<CH1, CH2, ADD, CHOp, ALT, or XY>`. |
| `PROBe? <CH1 or CH2>` *(2221/2230)* | Query only. Returns `CH<1 or 2> PROB:<NR1>`. `<NR1>` may be 1000/100/10/1/-1(identify)/-2(unknown coding). |

### Table 7-23: Horizontal Commands

| Command | Description |
|---|---|
| `DELAy?` *(2230)* | Returns `DELA VAL:<NR3>,UNI:<S or DIV>`. |
| `DELAy? VALue` *(2230)* | Returns `DELA VAL:<NR3>` in the units from the UNIts query. |
| `DELAy? UNIts` *(2230)* | Returns `DELA UNI:<S or DIV>` (`DIV` when SEC/DIV = EXT CLK). |
| `HORizontal?` *(ALL)* | Returns all present horizontal settings. |
| `HORizontal? ASEdiv` *(ALL)* | Returns `HOR ASE:<NR3>` (A SEC/DIV; 0 when SEC/DIV=EXT CLK). |
| `HORizontal? BSEdiv` *(2230)* | Returns `HOR BSE:<NR3>` (B SEC/DIV). |
| `HORizontal? EXTclk` *(ALL)* | Returns `HOR EXT:<ON or OFF>`. |
| `HORizontal? HMAg` *(ALL)* | Returns `HOR HMA:<ON or OFF>` (X10 magnifier). |
| `HORizontal? MODe` *(2230)* | Returns `HOR MOD:<ASW, AIN, or BSW>`. |

### Table 7-24: Trigger Commands

| Command | Description |
|---|---|
| `ATRigger? [MODe]` *(ALL)* | Returns `ATR MOD:<NOR, PPA, or SGL>;` (PPA covers both Peak-to-Peak Auto and TV Field). |
| `SGLswp ARM` *(ALL)* | Rearms a completed single sweep. Execution error if not in SGL SWP mode; execution warning if already armed. With `OPC ON`, an operation-complete SRQ fires when the single sweep occurs. |
| `SGLswp?` *(ALL)* | Returns `SGL <ARM or DON>;` if SGL SWP is on, else `"SGL ;"` plus an execution warning. |
| `TRIggerd?` *(ALL)* | Returns `TRI <ON or OFF>;` (TRIG'D indicator state). |

### Table 7-25: Cursor Commands

| Command | Description |
|---|---|
| `CURSor CHAnnel:<CH1-CH2>` *(2221/2230)* | Selects the channel for `DELTAV?`'s voltage difference. No warning if directed to an undisplayed channel. |
| `CURSor POSition:<NR1>` *(2221/2230)* | Horizontal data-point position of the active cursor. 1K record: clamped to 1023, no warning. 4K record: past 4095 is a command error (ignored). |
| `CURSor SELect:<CURS1-CURS2>` *(2221/2230)* | Selects which cursor `CURSor POSition` moves. |
| `CURSor TARget:ACQuisition` *(2221/2230)* | Attaches cursors to the acquisition waveform. |
| `CURSor TARget:<REF1-REF3>` *(2230)* | Attaches cursors to the named reference; ignored if not displayed. |
| `CURSor TARget:REF4` *(2221/2230)* | Attaches cursors to REF4; execution error SRQ if REF4 is empty. |
| `CURSor?` *(2221/2230)* | Returns e.g. `CURS SEL:CH1,TAR:ACQ,CHA:CH1,POS:1047;`. Each argument may be queried individually, e.g. `CURSOR? TAR`. |
| `DELTAV?` *(2221/2230)* | Returns `DELTAV VAL:0.500E0,UNI:VOL;` (or `PERcent` if VOLTS/DIV variable knob is out of CAL). |
| `DELTAV? VALue` *(2221/2230)* | Returns `DELTAV VAL:<NR3>;` only. |
| `DELTAV? UNIts` *(2221/2230)* | Returns `DELTAV UNI:<V or PER>;` only. |
| `DELTAT?` *(2221/2230)* | Returns `DELTAT VAL:1.180E-3,UNI:SEC;` (units in `DIV` if SEC/DIV=EXT CLK). |
| `DELTAT? VALue` / `DELTAT? UNIts` *(2221/2230)* | Individual-field forms of the above. |

### Table 7-26: Display Commands

| Command | Description |
|---|---|
| `MESsage <NR1>:"message"` *(2221/2230)* | Writes text on readout row `<NR1>` (16=top to 1=bottom). `MESsage <16-1>` turns off normal readouts. `MES [0]` (0 may be omitted) turns the message off and restores normal readouts. Message must be quoted; >~40 chars runs off/truncates the CRT edge (SRQ if RQS on). Many message lines can flicker the display / exceed display memory. |
| `PLOt ABOrt` *(ALL)* | Stops an in-progress plot, returns to previous mode. **Only command/query the scope responds to during a plot.** Turns off the `AUTo` argument. |
| `PLOt AUTo:<ON or OFF>` *(2221/2230)* | Auto-plot each waveform after acquisition; graticule plotted once in AUTo if `GRAt` ON. |
| `PLOt FORmat:<[XY], HPGl, EPS7, EPS8, or TJEt>` *(ALL)* | Output format for the named plotter (default XY if none of the named plotters selected). `HPGl`=HP-GL compatible; `EPS7`/`EPS8`=7-bit/8-bit EPSON; `TJEt`=HP ThinkJet. With Option 10, a GPIB controller can address the plotter to listen then the scope to talk, then send `PLOt STArt`. |
| `PLOt GRAt:<ON or OFF>` *(ALL)* | Plotted graticule on/off. |
| `PLOt SPEed:<NR1>` *(ALL)* | Integer 1-10, analog plotter pen speed (~divisions/second). |
| `PLOt STArt` *(ALL)* | Starts a plot with the parameters from `PLOt FORmat`/`GRAt`/`SPEed`. While plotting, all commands/queries except `PLOt ABOrt` are ignored. |

### Table 7-27: Acquisition Commands

| Command | Description |
|---|---|
| `ACQuisition CURRent:<AVErage,[DEFault],PEAkdet,or SAMple>` *(ALL)* | Names the mode for the current acquisition type/SEC-DIV setting. Omitted mode arg -> default for current type/setting. SRQ if the requested mode is invalid for the current type/setting. |
| `ACQuisition CURRent:ACCpeak` *(2221/2230)* | Selects ACCpeak for the current acquisition type/setting. |
| `ACQuisition HSRec:<ACCpeak or AVErage>` *(2221/2230)* | Mode for 5/10 µs/div SEC-DIV settings. |
| `ACQuisition HSRec:[SAMple]` *(ALL)* | Default mode for 5/10 µs/div acquisitions. |
| `ACQuisition LSRec:<ACCpeak or AVErage>` *(2221/2230)* | Mode for 0.02ms/div-50ms/div acquisitions. |
| `ACQuisition LSRec:<[PEAkdet] or SAMple>` *(ALL)* | Mode for 0.02ms/div-50ms/div; PEAkdet is default if omitted. |
| `ACQuisition NUMsweeps:<NR3>` *(2221/2230)* | Sweeps before halting; 0 = continuous (never halt). |
| `ACQuisition REPetitive:<ACCpeak or SAMple>` *(2221/2230)* | Mode for repetitive acquisitions, 0.05-2 µs/div. |
| `ACQuisition REPetitive:[AVErage]` *(ALL)* | Default mode for repetitive acquisitions, 0.05-2 µs/div. |
| `ACQuisition RESet` *(ALL)* | Command only. Resets sampling at all SEC/DIV settings to default modes (shown bracketed above). |
| `ACQuisition ROLl:<[PEAkdet] or SAMple>` *(ALL)* | Mode for ROLl acquisitions, 0.1-5 sec/div. ROLl is untriggered. |
| `ACQuisition SCAn:<[PEAkdet] or SAMple>` *(ALL)* | Mode for SCAn acquisitions. |
| `ACQuisition SCAn:<ACCpeak or AVErage>` *(2221/2230)* | Mode for SCAn, 0.1-5 sec/div. Scope must be in NORM or SGL SWP trigger mode to see a READOUT change. |
| `ACQuisition SMOoth:<ON or OFF>` *(ALL)* | Smoothing on acquired waveform data. |
| `ACQuisition TRIGCount:<NR1>` *(2220)* | Data points before trigger. Range 16-2048 (post-trigger), 2048-4080 (pre/mid-trigger). Resolution 4. |
| `ACQuisition TRIGCount:<NR1>` *(2221)* | Range 16-4080 regardless of front-panel TRIG POS switch. Resolution 4. |
| `ACQuisition TRIGCount:<NR1>` *(2230)* | Range depends on record length/pre-post selection: pretrigger 4-512 (1K) / 16-2048 (4K); post-trigger 512-1020 (1K) / 2048-4080 (4K). Resolution ±4. |
| `ACQuisition VECtors:<ON or OFF>` *(ALL)* | Point-to-point display vectors on/off. |
| `ACQuisition WEIght:<NR1>` *(ALL)* | Acquisitions weighted into an AVEraged record: 1,2,4,8,16,32,64,128,256 only (else SRQ, command ignored). Omitted -> defaults to 4. |
| `ACQuisition?` *(ALL)* | Returns full acquisition-mode settings, short form, e.g.: `ACQ REP:AVE,HSR:SAM,LSR:PEA,SCA:PEA,ROL:PEA,SMO:ON,WEI:4,SWP:1037,NUM:0,POI:4096,TRIGM:POST,TRIGC:2000,SAV:OFF,DIS:SCA,VEC:ON;`. Each argument queryable individually. |
| `ACQuisition? DISplay` *(ALL)* | Returns `ACQ DIS:<ROL or SCA>;` (state of ROLL/SCAN button). |
| `ACQuisition? POInts` *(ALL)* | Returns `ACQ POI:<NR1>;` (data points in the record). |
| `ACQuisition? SAVE` *(ALL)* | Returns `ON` (SAVE) or `OFF` (CONTINUE) for the acquisition system state. |
| `ACQuisition? SWPcount` *(ALL)* | Returns `ACQ SWP:<NR1>;` (sweeps completed). |
| `ACQuisition? TRIGMode` *(ALL)* | Returns `ACQ TRIGM:<PRE or POST>;`. |
| `STORe?` *(ALL)* | Returns `STOR <ON or OFF>;` (STORE/NON-STORE button state). |

### Table 7-28: Save and Recall Reference Commands

| Command | Description |
|---|---|
| `REFFrom [ACQ]` *(ALL)* | Selects the acquisition as the `SAVeref` data source (default, brackets optional). Only valid source for 2220/2221. |
| `REFFrom REF<1-4>` *(2230)* | Names a numbered reference as the `SAVeref` source. ACQ waveforms must first be stored to a numbered reference (REF1-REF4) before being saved to a lettered one (REFA-REFZ). |
| `REFFrom REF<A-Z>` *(2230)* | Names an extended-memory reference (26 KB total; 1K-8K averaged-4K records) as the source. Nonvolatile lettered references can't be displayed/plotted/transmitted directly - must first move to a numbered reference via `REFFrom`+`SAVeref`. |
| `REFDisp REF<1-3>:<ON, OFF, or EMPTY>` *(2230)* | 2230 reference display on/off; `EMPTY` erases and turns off. REF1-3 are 1024-point memories. |
| `REFDisp REF4:<ON, OFF, or EMPTY>` *(ALL)* | REF4 is the only reference for 2220/2221 (4096-point); occupies REF1-3's slot on the 2230. |
| `REFDisp REF<A-Z>:EMPTY` *(2230)* | Erases the named reference if unprotected. Lettered refs can't be displayed directly - must move to a numbered slot first. |
| `REFProt REF<A-Z>:<LOCked, PERM, or UNLocked>` *(2230)* | Write protection for nonvolatile references. `LOCked`/`PERM` disable further storage/erasure; `PERM` also can't be overwritten from the front panel. |
| `REFOrmat CHAnnel:<[CH1] or CH2>` *(2230)* | Which channel of the saved reference to reformat. SRQ if no SAVE REF waveform for the named channel. XY: either channel valid. |
| `REFOrmat HMAg:<ON or OFF>` *(2230)* | 10x horizontal-gain increase (ON, affects vertical channels) / off (OFF) for the REFORMAT target. |
| `REFOrmat VGAin:<NR3>` *(2230)* | Vertical gain change for the REFORmat target+channel. Not valid for XY. Max ±3 VOLT/DIV detent positions (1-2-5 sequence); execution error if out of range or not a 1-2-5 setting. |
| `REFOrmat VPOsition:<NR2>` *(2230)* | Vertical position of the reformatting target, ±10 divisions from original position, 1-displayed-bit resolution. |
| `REFDisp?` *(ALL)* | Returns REF1's (2230) or REF4's (2220/2221) status: `ON`, `OFF`, or `EMPTY`. |
| `REFDisp? REF<1-3>` *(2230)* | Named 2230 reference's status. |
| `REFDisp? REF4` *(ALL)* | REF4's status (default arg for 2220/2221, no argument needed). |
| `REFFrom?` *(ALL)* | Returns the selected `SAVeref` data source: `ACQ` (2220/2221) or `ACQ`/`REF1-4`/`REFA-Z` (2230). |
| `REFOrmat?` *(2230)* | Returns REFOrmat settings, e.g.: `REFO TAR:REF4, CHA:CH2,VGA:0.5E+0, VPO:+3.96,HMA:OFF,BAS:0.2E+0,MOD:CH1;`. Each argument individually queryable. |
| `REFOrmat? BASegain` *(2230)* | Returns `<NR3>` vertical gain at which the REFOrmat target was acquired. |
| `REFOrmat? MODe` *(2230)* | Returns the vertical mode the REFOrmat target was acquired in (`CH1,CH2,ADD,CHOP,ALT,or XY`). |
| `REFStat? FIll` *(2230)* | 30-number string, fill status (Kbytes: 0/1/2/4/8) of REF1-REFZ. |
| `REFStat? FREe` *(2230)* | `<NR1>` 0-26: free Kbytes in nonvolatile reference memory. |
| `REFStat? PROTect` *(2230)* | 30-character string, protection status of REF1-REFZ (`U`=unlocked,`L`=locked,`P`=permanent). |
| `SAVeref REF<1-3>` *(2230)* | Command only. Saves the `REFFrom`-selected waveform into the named 1K reference; the 1K portion of a 4K source is chosen by the active cursor position. Also turns the saved reference display on. |
| `SAVeref REF4` *(ALL)* | Command only. Saves into the 4K REF4 (only reference for 2220/2221; argument may be omitted for those models). |
| `SAVeref REF<A-Z>` *(2230)* | Command only. Saves into the named lettered reference. 4K records stored this way can't be moved as 1K into REF1-3; must go to REF4 to display/transmit. |

### Table 7-29: Waveform Commands

| Command | Description |
|---|---|
| `CURVe` *(ALL)* | As a command: sends waveform data to the scope (`DATa TARget`=destination reference, `DATa CHAnnel`=destination channel [REF4 only for 2220/2221], `DATa ENCdg`=format). As a query: gets waveform data (`DATa SOUrce`+`DATa CHAnnel`=source). Data form: `CURVE <data>;` where `<data>` is `%<byte count><binary data><checksum>` (BIN), `#H<byte count><hex data><checksum>` (HEX), or comma-separated `<ascii data>` (ASCii). |
| `DATa CHAnnel:<[CH1] or CH2>` *(ALL)* | Channel for `CURve?`/`WAVfrm?`/`WFMpre?` source, and target channel for incoming waveform data. SRQ if no waveform in the named channel when queried. Power-on default CH1; CH1 required for an XY acquisition. |
| `DATa ENCdg:<ASCii,[BINary],or HEX>` *(ALL)* | Curve data encoding/decoding format. Power-on default `BINary`. Data points are unsigned integers in all formats. |
| `DATa SOUrce:<REF1,REF2,or REF3>` *(2230)* | Named reference as source for `WAV?`/`WFM?`/`CURV?`. |
| `DATa SOUrce:<[ACQ] or REF4>` *(ALL)* | ACQ (default) or REF4 as source. A saved 4K record is moved off-instrument via `REF4` as source. |
| `DATa TARget:<REF1,REF2,or REF3>` *(2230)* | Named reference to receive `CURVe`/`WFMpre` data. Power-on default REF1; no other default. |
| `DATa TARget:REF4` *(ALL)* | REF4 as the receiving reference (only choice for 2220/2221; required as the 2230's target for a 4K waveform in). |
| `DATa?` *(ALL)* | Returns source/target/channel/encoding, e.g.: `DAT SOU:ACQ,TAR:REF1,CHA:CH1,ENC:BIN;`. Individually queryable. |
| `WAVfrm?` *(ALL)* | Returns combined preamble+curve data: `WFM <ascii preamble>;CURV <waveform data>;`. |

### Table 7-30: Waveform Preamble Fields

> **NOTE**: this table's arguments are primarily for *interpreting* a
> `WFMpre?` response - they're not normally sent as individual commands,
> but grouped as a complete preamble. If sent individually, a value
> isn't accepted (except as noted for `ENCdg`) until the curve it
> belongs with is transferred to the `DATa TARget` reference. A size
> error in a preamble numeric argument is accepted at send time, but
> rejected (with a waveform-preamble-error SRQ) when the curve data
> itself arrives.

| Command | Description |
|---|---|
| `WFMpre ENCdg:<ASCii,[BINary],or HEX>` *(ALL)* | Curve data encoding format (identical to `DATa ENCdg`). |
| `WFMpre?` *(ALL)* | Full preamble, e.g.: `WFM WFI:"<identification string>",NR.P:2048,PT.O:256,PT.F:ENV,XMU:1.0E+3,XOF:0,XUN:S,XIN:10.0E-6,YMU:8.0E-3,YOF:0,YUN:V,ENC:ASC,BN.F:RP,BYT:1,BIT:8,CRV:CHK;`. Individually queryable. |
| `WFMpre? WFId` *(ALL)* | Identification string: acquisition source, channel, Volts/Div, input coupling, Sec/Div, acquisition mode, curve number. XY mode adds CH2 Volts/Div+coupling. Ignored if sent as a command. E.g. `WFM WFI:"ACQ, CH1, 0.2mV, DC, 0.5mS, AVERAGE, CRV# 3";` or (XY) `WFM WFI:"REF4, XY, 20mV, DC, 50mV, DC, 0.5mS, SAMPLE, CRV# 1";`. `DATa CHAnnel` must be CH1 to get XY info; all vertical info omitted for a 2220. |
| `WFMpre NR.Pts:<NR1>` *(ALL)* | Number of points in the waveform (single Y, X-Y pair, or Max-Min pair). Record length is 1024 or 4096, but `NR.Pts` may be 256/512/1024/2048/4096 depending on channel count, acquire mode, and SMOOTH - see the ratio table below. |
| `WFMpre PT.Off:<NR1>` *(ALL)* | Trigger position relative to the first data point. 1024-point record: 4 to 1024, step 4. Negative if the trigger was before the first point in the window; legal range −3096 to +4096 for any 1024-point window of a 4096-point record. `-10000` if unknown. |
| `WFMpre PT.Fmt:<Y, XY, or ENV>` *(ALL)* | `Y`: X-axis implied from preamble, values are vertical amplitude only. `XY`: X-Y pairs, X first. `ENV`: max-min pairs, sent **in reverse order** (`...,y1min,y1max,y2min,y2max,...`); valid for PEAkdet/ACCpeak with SMOoth OFF. |
| `WFMpre XUNits:<S or CLKs>` *(ALL)* | Units for `XINcr`. `S`=seconds; `CLKs`=unknown increment (returned when SEC/DIV=EXT CLK). |
| `WFMpre XINcr:<NR3>` *(ALL)* | Time between data points. Rejected (command-argument SRQ if RQS on) if it doesn't match a legitimate SEC/DIV setting for data being sent in. Queried value defaults to `1` (`0.1E+0`) if unknown (e.g. EXT CLK). |
| `WFMpre YUNits:<V or DIVs>` *(ALL)* | Units for `YMUlt`. `DIVs` returned when the `DATa CHAnnel`'s CAL knob isn't in detent; always `DIVs` for the 2220 (vertical scaling unknown). |
| `WFMpre YMUlt:<NR3>` *(ALL)* | Digitizer step size (volts between digitizer levels). Rejected (waveform-preamble-error SRQ) if it doesn't match a legitimate VOLTS/DIV setting for incoming data. Queried value is `40.0E-3` when the `DATa SOUrce`'s VOLTS/DIV CAL knob isn't in detent. |
| `WFMpre YOFf:<NR1>` *(ALL)* | Y coordinate of ground. `-10000` if ground level unknown. |
| `WFMpre XMUlt`, `WFMpre XOFf` *(ALL)* | X-axis analogs of `YMUlt`/`YOFf`, added for XY waveforms. `YUNits` applies to both X and Y for XY; `XUNits` is referenced to the sampling rate. |
| `WFMpre BN.Fmt:RP` *(ALL)* | Only valid argument; binary format is always right-justified, positive (unsigned) binary integers. |
| `WFMpre BYT/nr:<NR1>` *(ALL)* | `1` or `2`. Two bytes/point for AVErage mode, one byte otherwise (MSB first if 2). HEX format: 2 ASCII hex chars/point regardless. |
| `WFMpre BIT/nr:<NR1>` *(ALL)* | Data points are 8 or 16 bits. LSBs of a 16-bit waveform may or may not be valid depending on acquisitions averaged. |
| `WFMpre CRVchk:CHKsm0` *(ALL)* | `CHKsm0` indicates the last byte of a binary curve is a checksum (two's-complement modulo-256 sum of the binary count + curve data bytes; excludes the `CURVE %` word/symbol itself). |

**NR.Pts-to-record-length ratio table** (from `WFMpre NR.Pts`'s
description): `Rec/1` for 1 channel + `SAMple`/`AVErage` (smoothing N/A)
or 1 channel + `PEAkdet`/`ACCpeak` with `SMOOTH ON`; `Rec/2` for 2
channels + `SAMple`/`AVErage`, 2 channels + `PEAkdet`/`ACCpeak` with
`SMOOTH ON`, or 1 channel + `PEAkdet`/`ACCpeak` with `SMOOTH OFF`;
`Rec/4` for 2 channels + `PEAkdet`/`ACCpeak` with `SMOOTH OFF`. Example:
2 channels, peak-detect, smoothing off -> a 4Kbyte record yields
4096/4 = 1024 points per waveform.

### Table 7-31: Miscellaneous Commands

| Command | Description |
|---|---|
| `INIt` *(ALL)* | Command only. Reverts the oscilloscope to power-on default acquisition-mode states; also initializes the 2230 menu system. |
| `LONg <[ON] or OFF>` *(ALL)* | `ON` (power-on default): query replies use full command words. `OFF`: short form only (the capital letters shown in these tables - also the minimum characters accepted as valid input). `LONg?` returns state. |
| `ID?` *(ALL)* | **Returns the oscilloscope identification string: `ID TEK/2230,V81.1,VERS:09;`** (instrument type/version as appropriate). |
| `HELp?` *(ALL)* | Returns a list of all valid command headers in the queried instrument. All valid characters returned; short form (`LONG OFF`) in capitals. |
| `SET?` *(ALL)* | Returns an ASCII string of headers+arguments reflecting the present state of every setting controllable via the interface (query-only settings excluded). The returned string can be sent back as a command message to recreate those settings. `LONg` state affects reply length. Per Codes-and-Formats compliance, no header is sent back with the settings string itself. |

### Table 7-32: Service Request Group Commands

| Command | Description |
|---|---|
| `OPC <[ON] or OFF>` *(ALL)* | `ON`: sends an SRQ on completion of certain system events (if `RQS` also ON) - acquisition completed, plot completed. `OFF` (power-on default): no OPC-triggered SRQ. |
| `RQS <[ON] or OFF>` *(ALL)* | `ON` (power-on/default): sends SRQ when an event needs reporting. `OFF`: event codes still accumulate (retrievable via `EVEnt?`), but `STAtus?` always replies `0`. |
| `EVEnt?` *(ALL)* | Returns `<NR1>`: the oldest pending service-request event's code, or `0` if none pending. Repeated `EVEnt?` calls retrieve further distinct-priority events until `0` is returned. **Querying an event clears its service request.** |

### Table 7-33: RS-232-C Specific Commands

| Command | Description |
|---|---|
| `FLOw <[ON] or OFF>` *(ALL)* | `ON` (power-on default): enables DC1/DC3 (XON/XOFF) software flow control. **Binary data transfers cannot be made with FLOW ON** - send `FLOw OFF` first. With FLOW ON, `<control-S>` temporarily suspends output, `<control-Q>` resumes it, `<control-D>` aborts the current command/query, erases both I/O buffers, and resets the message processor. `FLOw?` returns state. |
| `REMote <[ON] or OFF>` *(ALL)* | Enables/disables setting of remote-controllable oscilloscope states. **An execution-error SRQ is sent if a control command is sent with `REM OFF`.** `REM?` returns state. |
| `STOP <1 or 2>` *(ALL)* | Stop bits for character transfer; usual value 1 (some printers need 2 at certain baud rates). Power-on default 1. `STOP?` returns 1 or 2. |
| `STAtus?` *(ALL)* | Returns the current status byte. If no service requests pending, indicates "No Status to Report" (`0`/`16` per Table 7-34, RQS-dependent). If `RQS` is off, use `EVEnt?` instead to learn whether/which event occurred - `EVEnt?` gives more specific information than the status byte alone. |

## Status bytes and event codes (Tables 7-34/7-35) — critical reference for the live RS-232 investigation

### Table 7-34: Status Event and Error Categories

The status byte returned by an asynchronous SRQ, or by `STAtus?`, is
built from a category value plus 2 modifier bits: `R` = 1 if `RQS` is
ON (else 0), `X` = 1 if the oscilloscope is busy processing a
command/query when the byte is read (else 0). Binary pattern shown as
`0R?X ????` (bit positions from Table 7-34's own `0R1X`/`0R0X`/`010X`/
`000X` notation).

| Category | Binary | RQS Off, Not Busy | RQS Off, Busy | RQS On, Not Busy | RQS On, Busy | Description |
|---|---|---|---|---|---|---|
| Command Error | `0R1X 0001` | 33 | 49 | **97** | 113 | Instrument received a command it cannot understand. |
| Execution Error | `0R1X 0010` | 34 | 50 | **98** | 114 | Instrument received a command it cannot execute (out-of-range arguments, or conflicting settings). |
| Internal Error | `0R1X 0011` | 35 | 51 | 99 | 115 | Hardware condition or firmware problem prevents operation. |
| Power On | `010X 0001` | 1 | 17 | 65 | 81 | Instrument power was turned on. |
| Operation Complete | `0R0X 0010` | 2 | 18 | 66 | 82 | Operation complete. |
| Execution Warning | `0R1X 0101` | 37 | 53 | 101 | 117 | Command executing but a potential problem exists (e.g. instrument out of range but sending a reading anyway). |
| No Status | `000X 0000` | 0 | 16 | 0 | 16 | Nothing to report. |

**`STATUS 98` - the byte this project's live testing actually
received in response to `ID?`/`SET?`/`STAtus?`/`HELp?`/garbage - is
"Execution Error, RQS On, Not Busy."** This means the command header
*was* recognized (a Command Error, 97, would mean otherwise), but
something prevented *executing* it - out-of-range arguments or a
conflicting setting, per the category description. Since this same
code came back for multiple different, individually-valid commands
(including queries the manual says should always be answerable), the
next diagnostic step is reading the *specific* `EVEnt?` code (see
Table 7-35 below) rather than just the status byte, to find the exact
201-263-range execution-error reason.

**Option 10 (GPIB) behavior**: if multiple different-priority events are
pending, the scope reasserts SRQ until all distinct-priority events are
reported (no duplicate-event or same-priority-level re-reports). Each
event clears automatically once its status byte/event code is read. A
GPIB Device Clear (DCL) clears all events except the power-on event.
With `RQS` off, all service requests are suppressed except the power-on
SRQ; the controller must poll via `STAtus?`/`EVEnt?` instead of waiting
for SRQ.

**Option 12 (RS-232) behavior**: same multi-event-priority reassertion
logic as GPIB, but described independently (no DCL equivalent
mentioned for RS-232 specifically - only "the controller can clear all
events by repeatedly sending `EVEnt?` until a zero status byte is
returned"). With `RQS` off, same STAtus?/EVEnt? polling requirement as
GPIB.

### Table 7-35: Event Codes (`EVEnt?` return values)

| Code | Instrument Status |
|---|---|
| 000 | No status to report |
| **Command Errors** | |
| 101 | Command header error |
| 102 | Header delimiter error |
| 103 | Command argument error |
| 104 | Argument delimiter error |
| 105 | Non-numeric argument, numeric expected |
| 106 | Missing argument |
| 107 | Invalid message-unit delimiter |
| 108 | Checksum error |
| 109 | Byte-count error |
| 151 | The argument is too large |
| 152 | Illegal hex character |
| 153 | Non-binary argument; binary or hex expected |
| 154 | Invalid numeric input |
| 155 | Unrecognized argument type |
| **Execution Errors** | |
| 201 | Command cannot be executed when in LOCAL |
| 203 | I/O buffers full, output dumped |
| 205 | Argument out of range, command ignored |
| 206 | Group execute trigger ignored |
| 251 | Illegal command |
| 252 | Integer overflow |
| 253 | Input buffer overflow |
| 254 | Invalid waveform preamble |
| 255 | Invalid instrument state |
| 256 | GPIB (Option 10) command not allowed |
| 257 | RS-232-C (Option 12) command not allowed |
| 258 | Command not allowed on 2220 or 2221 |
| 259 | Command not allowed on 2230 |
| 260 | Cannot execute command with RQS OFF |
| 261 | Reference memory busy with local (front-panel) command |
| 262 | Reference memory non-existent or specified as different size than selected waveform |
| 263 | Plot active; only PLOT ABORT allowed while plotting |
| **Internal Errors** | |
| 351 | Firmware failure. Contact your nearest Tektronix Service Center for assistance |
| **System Events** | |
| 401 | Power on |
| 451 | Parity error |
| 452 | Framing error |
| 453 | Carrier lost |
| 454 | End of acquisition OPC |
| 455 | End of plot OPC |
| 456 | Diagnostics test complete OPC |
| **Execution Warnings** | |
| 551 | Single sweep is already armed |
| 552 | No ground-dot measurement available |
| 553 | Invalid probe code or identify |
| 554 | Query not valid for current instrument state |
| 555 | Requested setting is out of detent (uncalibrated) |
| 556 | MESsage display buffer is full |
| 557 | Waveform preamble is incorrect, has been corrected |
| 558 | Waveform transfer ended abnormally |

**Live-testing candidates for the "why does every command fail with
Execution Error 98" puzzle**: given `ID?`/`SET?`/`STAtus?`/`HELp?` are
all documented, valid, no-argument queries and all failed identically,
the leading candidates from this table are:

- **201** ("Command cannot be executed when in LOCAL") - though the
  main Operator Manual (`070-4998-02.pdf`) separately states queries
  *should* be answered even with `REMOTE OFF`; if this firmware
  doesn't actually implement that carve-out, sending `REMote ON` first
  should resolve it. (Already tried live once with an ambiguous result
  - `ID?` right after `REMote ON` returned `READY;` alone, no `STATUS`
  prefix, but the very next `SET?` reverted to the `STATUS 98;READY;`
  pattern. Worth retrying with a clean, deliberate sequence.)
- **260** ("Cannot execute command with RQS OFF") - unlikely, since RQS
  defaults ON and nothing in the test sequence turned it off.
- **255** ("Invalid instrument state") - a generic catch-all, possible
  if some other precondition isn't met.

The concrete next live step (blocked as of 2026-09-14 by COM3 being
held open by another process) is: send `REMote ON`, drain the event
queue completely with repeated `EVEnt?` calls **reading the actual
3-digit codes** (not just the status byte), then retry `ID?` cleanly.

## Programming manual vs. this project's earlier command-language findings

This document **fully confirms and supersedes with primary-source
detail** the partial command list this project had already extracted
from `STRINGS.md`'s length-prefixed keyword table inside the comm
ROM binary - every keyword found there (`ID`, `SET`, `STAtus`, `HELp`,
`EVEnt`, `CURVe`, `WFMpre`, `DATa`, `ACQuisition`, etc.) matches a real,
now-fully-documented command here. It does **not** by itself explain
why the live hardware returns `STATUS 98` instead of real answers -
that remains an open, live-testable question (see `disasm/NOTES.md`'s
"BREAKTHROUGH, 2026-09-14" section and `TODO.md` for the current
state of that investigation).
