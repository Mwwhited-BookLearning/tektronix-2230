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

## Architecture and methodology

- [CPU identity and source language](architecture/cpu-and-language.md) - confirming the 8088/8086 CPU and that the firmware is compiled C, not hand-assembled
- [Disassembly tooling](architecture/tooling.md) - the recursive-descent disassembler, the loop/function-boundary separator tool
- [Validation and coverage](architecture/validation-and-coverage.md) - how the disassembly is checked for correctness, the heuristic layer, current code coverage, the NOP-aligned reconstruction

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
