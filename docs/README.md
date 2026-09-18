# Tektronix 2230 disassembly notes

This is the running technical log for the firmware reverse-engineering
work, split by topic. It used to be one giant `disasm/NOTES.md`; that
file grew past 3700 lines and became hard to navigate, so its content
now lives here, organized by area. `disasm/NOTES.md` itself is now a
short pointer to this folder - don't add new findings there.

**Where to add new findings**: pick the area below that fits, or add a
new `docs/{area}/{feature}.md` file if none fits well (mirror the style
of the existing files - a top `#` title, then `##` sections, dated
where relevant). Update this README's index when you add a file.

For the living/current-state documents (not history), see the repo
root: `CONTEXT.md`, `HARDWARE.md`, `MEMORY_MAP.md`, `JUMP_MAP.md`,
`FUNCTIONS.md`, `VARIABLES.md`, `PSEUDOCODE.md`, `STRINGS.md`,
`TODO.md`. This `docs/` folder is the *investigation history* behind
those documents - the "how we found out," not the current-best-answer
itself.

## Primary source transcriptions

- [Service manual Section 3: Theory of Operation](theory-of-operation.md) - full OCR transcription (52 pages) of the analog/digital circuit descriptions, including Table 3-1's Memory Space Allocation map and the Time Base/Trigger Logic tables - a raw primary source, not yet cross-checked against or integrated into MEMORY_MAP.md/FUNCTIONS.md/VARIABLES.md
- [Service manual Section 6: Maintenance](maintenance.md) - full OCR transcription (46 pages) covering preventive maintenance, self-test/diagnostic error-code tables (HS_ACQ/MM_ACQ/XY_ACQ/CDT/FP_A2D acquisition tests, exerciser display formats), and corrective maintenance/board-removal procedures - a raw primary source, not yet cross-checked against or integrated into MEMORY_MAP.md/FUNCTIONS.md/VARIABLES.md/STILL_PENDING_DECODE.md
- [F10/F12 Option Installation Instructions (070-6090-00)](f10-f12-option-installation.md) - full OCR transcription (6 pages) of the standalone GPIB/RS-232 option install sheet - confirms U9109/U9110 ROM part numbers match this project's own main ROM chips, and documents jumper P9107 on the Storage board being moved when a comm option is installed (see TODO.md's jumper-hunt item)
- [Service manual Section 7: Options](options.md) - full OCR transcription (46 pages) of the GPIB (Option 10) and RS-232-C (Option 12) option manual - includes the full RS-232-C/GPIB command reference (Tables 7-13 through 7-30), the PARAMETERS DIP-switch tables (7-6 through 7-9), and - directly relevant to this project's own register-level findings - **"OPTION 10/12 THEORY OF OPERATION"** naming the exact same Interrupt Mask Latch (U1236, 4 outputs 0D-3D, "one for the RS-232-C port, one for diagnostics") this project independently found from disassembly, plus Table 7-36's RS-232-C Status Buffer bit map (comm_stat: bit0=/PWR INT, bit1=/DR+INTR, bit2=TBRE, bit3/4/5=PARAMETERS switch 8/10/9 in that exact order, bit6=DIAG (interrupt mask latch D3), bit7=/DCD2) - a raw primary source, not yet cross-checked against or integrated into MEMORY_MAP.md/FUNCTIONS.md/VARIABLES.md/STILL_PENDING_DECODE.md
- [Programming Manual](programming-manual.md) - full OCR transcription (42 pages, printed pages 7-10 through 7-50) of the RS-232-C/GPIB command reference for controllers - the full command-table set (Tables 7-13 through 7-33: waveform preamble fields, miscellaneous commands, service-request-group commands, RS-232-C specific commands) plus the status-byte/event-code tables (7-34, 7-35: command/execution/internal errors, system events, execution warnings) - a raw primary source, not yet cross-checked against or integrated into MEMORY_MAP.md/FUNCTIONS.md/VARIABLES.md/STILL_PENDING_DECODE.md
- [Section 9 (Diagrams): 34-page schematic survey](diagrams-index.md) - a page-by-page index/survey (not a netlist reconstruction) of the service manual's electrical schematics, covering the diagram-numbering scheme (foldout Figures 9-4/9-5), the P9107/P9105 jumpers near the A10 board's reset/clock circuit, and Diagram 23's Interrupt Mask Latch (U1235)/Status Buffer (U1223)/DIAG/RLSD signal names relevant to the DIAG-vs-`/DCD2` open question - a raw primary source, not yet cross-checked against or integrated into MEMORY_MAP.md/FUNCTIONS.md/VARIABLES.md/STILL_PENDING_DECODE.md/TODO.md

## Architecture and methodology

- [CPU identity and source language](architecture/cpu-and-language.md) - confirming the 8088/8086 CPU and that the firmware is compiled C, not hand-assembled
- [Disassembly tooling](architecture/tooling.md) - the recursive-descent disassembler, the loop/function-boundary separator tool
- [Validation and coverage](architecture/validation-and-coverage.md) - how the disassembly is checked for correctness, the heuristic layer, current code coverage, the NOP-aligned reconstruction
- [The decompile/ Ghidra project](architecture/ghidra-project.md) - a second, independent cross-check tool; the non-obvious segment-addressing convention and two real mistakes to avoid when scripting against it
- [Proposal: schematic-to-netlist extraction via SINA + Ollama](architecture/schematic-netlist-extraction-proposal.md) - not started - a phased plan for locally running SINA on the Section 9 schematics, converting its hardcoded OpenAI (`gpt-4o`) netlist-assembly call to a local Ollama model, with page 65 (P9107/P9105 jumpers) and page 114 (the DIAG-vs-bit-7 conflict) as the concrete success criteria
- [The emulator/ project (design phase)](../emulator/docs/design.md) - a headless Unicorn-Engine tracer for observing runtime behavior directly, motivated by the stroke-font hunt hitting static analysis's limits; see `emulator/README.md`

## Comm ROM (RS-232/GPIB option board)

- [Address map and CPU identity](comm-rom/address-map-and-cpu.md) - confirming the comm ROM shares the main CPU/address space (not a separate coprocessor), isn't bank-switched, and the `0x90000` alias resolution
- [Option-board presence detection](comm-rom/option-detection.md) - how the firmware detects whether the comm option is installed
- [RS-232: early hardware investigation](comm-rom/rs232-early-investigation.md) - the UART-register theory, `COMM_LOOPBACK` self-test tracing, the byte-dispatch/parser core, the interrupt mask latch
- [RS-232: the 2026-09-14 breakthrough](comm-rom/rs232-breakthrough.md) - first real command/response round-trip, and the resolution (a baud-rate reliability problem, not firmware)
- [RS-232: flow control and an open cross-ROM puzzle](comm-rom/rs232-flow-control-and-open-puzzle.md) - XON/XOFF software flow control, and an unresolved comm-ROM call into main-ROM plot-scaling code
- [RS-232: live session - waveform formats and the command-keyword table](comm-rom/rs232-live-session-2026-09-14.md) - confirming binary/hex/ASCII waveform transfer and checksums live, and finding the real command-keyword table
- [The full RS-232/GPIB command-keyword table](comm-rom/command-keyword-table.md) - every entry of the argument-keyword table, the dispatch/index table, and the top-level command-header table, extracted from the ROM
- [Comm ROM 160-2998-13 vs -14 byte diff](comm-rom/revision-13-vs-14-diff.md) - the two revisions' 133 differing bytes: a `-13`-only config-check routine replaced by a leftover copyright-string fragment, and a page-3 boot-stub jump that only exists in `-14`

See also `hardware/manuals/2230_programming/` for the transcribed
programming manual and the field-tested practical guide.

## Self-test system

- [Self-test dispatcher and sibling subroutines](self-test/dispatcher-and-siblings.md) - the top-level self-test orchestrator and its identified leaf tests
- [ADC/measurement hardware probes](self-test/hardware-probes.md) - self-test routines that exercise acquisition/measurement hardware
- [Front-panel switch bit validation](self-test/front-panel-switches.md) - confirming `SWB2`'s bit map against the front-panel self-tests

## Display / CRT readout

- [CRT readout display memory](display/readout-memory.md) - the physical `0x40000-0x4FFFF` readout memory window
- [Vector display list and the stroke-font hunt](display/vector-display-and-stroke-font.md) - the separate vector/stroke display-list buffer, and the still-unresolved search for the character glyph table
- [Rendered vector shapes, 0xAE64-0xB061](display/vector-icons/README.md) - committed SVG renders of a separate candidate vector shape table (icons or a rough font, unresolved which), plus `disasm/decode_vector_icons.py`

## Interrupts and scheduling

- [Interrupt vector table and INT 255](interrupts/ivt-and-int255.md) - real IVT entry points, and the correction that INT 255 is a real hardware maskable interrupt
- [The tick-driven task scheduler](interrupts/task-scheduler.md) - the scheduler driven by `INT2_HANDLER_LATE`

## Decode anomalies

- [Deliberate dual-entry-point byte sharing](decode-anomalies/dual-entry-points.md) - call targets that land 1-4 bytes into another instruction on purpose, not by accident
- [Landing artifacts and jump tables](decode-anomalies/landing-artifacts-and-jump-tables.md) - the systematic sweep for landing-artifact call targets, and indirect jump/call tables
- [Deep dive into UNKNOWN_DATA.md's blocks, 2026-09-15](decode-anomalies/unknown-data-deep-dive-2026-09-15.md) - a real ~100-entry jump table with 2 landing-artifact callers, a probable per-item position table, a mathematically-confirmed vector circle icon, and 2 other plausible-but-unconfirmed record tables

## Hardware I/O

- [Shift register and the assert/panic mechanism](hardware-io/shift-register-and-assert.md) - the hardware shift-register write (I/O ports `0xD1`/`0xC4`) and firmware's internal `assert()`/panic path

## Acquisition and plotting

- [Acquisition mode dispatch and print formatting](acquisition-and-plotting/mode-dispatcher-and-formatting.md) - the acquisition mode-change dispatcher, decimal-formatting engine, and print-record character-cell copy
- [The RAM far-pointer init table](acquisition-and-plotting/ram-far-pointer-table.md) - a family of functions only reachable via a data-driven init table, never proven to actually run

## Other

- [Open questions / next steps (historical)](open-questions.md) - a snapshot of open questions as of the last big documentation pass; **the live list is `TODO.md` at the repo root**, not this file
