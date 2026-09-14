# Tektronix 2230 firmware reverse-engineering notes

**This file's content moved to `docs/` on 2026-09-14** - it had grown
past 3700 lines and become hard to navigate. Start at
[`docs/README.md`](../docs/README.md) for the table of contents.

New findings should go into `docs/{area}/{feature}.md`, not here - see
`docs/README.md` for how to pick an area.

If you followed a link here from an old section reference (e.g. "see
`disasm/NOTES.md`'s 'BREAKTHROUGH, 2026-09-14' section"), find the
original section title below to get its new location:

| Old section | New location |
|---|---|
| Confirmed facts | [`docs/architecture/cpu-and-language.md`](../docs/architecture/cpu-and-language.md) |
| What language was this written in? | [`docs/architecture/cpu-and-language.md`](../docs/architecture/cpu-and-language.md) |
| Tooling | [`docs/architecture/tooling.md`](../docs/architecture/tooling.md) |
| Is the comm ROM its own CPU? | [`docs/comm-rom/address-map-and-cpu.md`](../docs/comm-rom/address-map-and-cpu.md) |
| The comm ROM is NOT bank-switched | [`docs/comm-rom/address-map-and-cpu.md`](../docs/comm-rom/address-map-and-cpu.md) |
| Comm ROM disassembly (160-2998-14) | [`docs/comm-rom/address-map-and-cpu.md`](../docs/comm-rom/address-map-and-cpu.md) |
| The 0x90000-0x97FFF region is fully resolved: an address-decode alias | [`docs/comm-rom/address-map-and-cpu.md`](../docs/comm-rom/address-map-and-cpu.md) |
| Found: the option-board presence/RAM-detection routine | [`docs/comm-rom/option-detection.md`](../docs/comm-rom/option-detection.md) |
| Found: the actual source of [0x1B83] (detect_comm_option_hw) | [`docs/comm-rom/option-detection.md`](../docs/comm-rom/option-detection.md) |
| The [0x4E0]-[0x4FC] cluster... | [`docs/comm-rom/option-detection.md`](../docs/comm-rom/option-detection.md) |
| Live hardware test: the write_readout_port_byte/UART theory does NOT hold up | [`docs/comm-rom/rs232-early-investigation.md`](../docs/comm-rom/rs232-early-investigation.md) |
| Follow-up live hardware session: cable fully validated, COMM_LOOPBACK proven internal-only | [`docs/comm-rom/rs232-early-investigation.md`](../docs/comm-rom/rs232-early-investigation.md) |
| Correction: COMM_LOOPBACK's UNTESTED result fully traced | [`docs/comm-rom/rs232-early-investigation.md`](../docs/comm-rom/rs232-early-investigation.md) |
| Traced the comm ROM's byte-dispatch/parser core | [`docs/comm-rom/rs232-early-investigation.md`](../docs/comm-rom/rs232-early-investigation.md) |
| Traced the interrupt mask latch's real outputs | [`docs/comm-rom/rs232-early-investigation.md`](../docs/comm-rom/rs232-early-investigation.md) |
| [0x758] bit-level validated as SWB2 | [`docs/self-test/front-panel-switches.md`](../docs/self-test/front-panel-switches.md) |
| Found: the self-test dispatcher | [`docs/self-test/dispatcher-and-siblings.md`](../docs/self-test/dispatcher-and-siblings.md) |
| The readout/CRT display memory (physical 0x40000-0x4FFFF) | [`docs/display/readout-memory.md`](../docs/display/readout-memory.md) |
| The readout vector display list | [`docs/display/vector-display-and-stroke-font.md`](../docs/display/vector-display-and-stroke-font.md) |
| Attempted: locating the stroke-font glyph table | [`docs/display/vector-display-and-stroke-font.md`](../docs/display/vector-display-and-stroke-font.md) |
| The 0x90000+ region: fully resolved (see above) | [`docs/comm-rom/address-map-and-cpu.md`](../docs/comm-rom/address-map-and-cpu.md) |
| Interrupt vector table entries (real code entry points) | [`docs/interrupts/ivt-and-int255.md`](../docs/interrupts/ivt-and-int255.md) |
| BREAKTHROUGH, 2026-09-14 | [`docs/comm-rom/rs232-breakthrough.md`](../docs/comm-rom/rs232-breakthrough.md) |
| RESOLVED, 2026-09-14: it was baud rate reliability all along | [`docs/comm-rom/rs232-breakthrough.md`](../docs/comm-rom/rs232-breakthrough.md) |
| MAJOR CORRECTION: INT 255 is NOT "a software-only vector" | [`docs/interrupts/ivt-and-int255.md`](../docs/interrupts/ivt-and-int255.md) |
| A small task scheduler, driven by INT2_HANDLER_LATE | [`docs/interrupts/task-scheduler.md`](../docs/interrupts/task-scheduler.md) |
| Validation status | [`docs/architecture/validation-and-coverage.md`](../docs/architecture/validation-and-coverage.md) |
| IP-wraparound branches was a validator bug (RESOLVED) | [`docs/architecture/validation-and-coverage.md`](../docs/architecture/validation-and-coverage.md) |
| Current coverage | [`docs/architecture/validation-and-coverage.md`](../docs/architecture/validation-and-coverage.md) |
| The main ROM has a heuristic layer too | [`docs/architecture/validation-and-coverage.md`](../docs/architecture/validation-and-coverage.md) |
| NOP-aligned readable reconstruction (binary/aligned/) | [`docs/architecture/validation-and-coverage.md`](../docs/architecture/validation-and-coverage.md) |
| Found the top-level self-test orchestrator | [`docs/self-test/dispatcher-and-siblings.md`](../docs/self-test/dispatcher-and-siblings.md) |
| Identified self_test_dispatcher's sibling subroutines | [`docs/self-test/dispatcher-and-siblings.md`](../docs/self-test/dispatcher-and-siblings.md) |
| Possible ADC/measurement self-test hardware | [`docs/self-test/hardware-probes.md`](../docs/self-test/hardware-probes.md) |
| Possible waveform acquisition buffer init | [`docs/self-test/hardware-probes.md`](../docs/self-test/hardware-probes.md) |
| Found: the hardware shift-register write (I/O ports 0xD1/0xC4) | [`docs/hardware-io/shift-register-and-assert.md`](../docs/hardware-io/shift-register-and-assert.md) |
| Found: the firmware's assert()/panic mechanism | [`docs/hardware-io/shift-register-and-assert.md`](../docs/hardware-io/shift-register-and-assert.md) |
| Open puzzle: comm ROM's compute_parity_mode_code calls scale_and_plot_point_default | [`docs/comm-rom/rs232-flow-control-and-open-puzzle.md`](../docs/comm-rom/rs232-flow-control-and-open-puzzle.md) |
| Resolved (partially): SUB_E90A5/SUB_E92B0 | [`docs/decode-anomalies/dual-entry-points.md`](../docs/decode-anomalies/dual-entry-points.md) |
| Found: RS-232 software flow control (XON/XOFF) | [`docs/comm-rom/rs232-flow-control-and-open-puzzle.md`](../docs/comm-rom/rs232-flow-control-and-open-puzzle.md) |
| A third decode anomaly: SUB_F6382 | [`docs/decode-anomalies/dual-entry-points.md`](../docs/decode-anomalies/dual-entry-points.md) |
| Found: the acquisition mode-change dispatcher (handle_acq_mode_change) | [`docs/acquisition-and-plotting/mode-dispatcher-and-formatting.md`](../docs/acquisition-and-plotting/mode-dispatcher-and-formatting.md) |
| A second, more puzzling decode anomaly: SUB_EAC86 | [`docs/decode-anomalies/dual-entry-points.md`](../docs/decode-anomalies/dual-entry-points.md) |
| Found: a decimal-formatting engine | [`docs/acquisition-and-plotting/mode-dispatcher-and-formatting.md`](../docs/acquisition-and-plotting/mode-dispatcher-and-formatting.md) |
| Found: the print-record character-cell-copy engine | [`docs/acquisition-and-plotting/mode-dispatcher-and-formatting.md`](../docs/acquisition-and-plotting/mode-dispatcher-and-formatting.md) |
| Found: a whole family of never-reached functions via the RAM far-pointer init table | [`docs/acquisition-and-plotting/ram-far-pointer-table.md`](../docs/acquisition-and-plotting/ram-far-pointer-table.md) |
| SUB_EAC86 fully resolved | [`docs/decode-anomalies/dual-entry-points.md`](../docs/decode-anomalies/dual-entry-points.md) |
| New tool: analyze_loops_vs_functions.py | [`docs/architecture/tooling.md`](../docs/architecture/tooling.md) |
| Found: indirect jumps/calls through computed pointers | [`docs/decode-anomalies/landing-artifacts-and-jump-tables.md`](../docs/decode-anomalies/landing-artifacts-and-jump-tables.md) |
| Systematic landing-artifact sweep (find_landing_artifacts.py) | [`docs/decode-anomalies/landing-artifacts-and-jump-tables.md`](../docs/decode-anomalies/landing-artifacts-and-jump-tables.md) |
| Open questions / next steps | [`docs/open-questions.md`](../docs/open-questions.md) (superseded by `TODO.md` at the repo root) |
| Live session, 2026-09-14 (continued): binary/hex CURVe? formats, command-keyword table | [`docs/comm-rom/rs232-live-session-2026-09-14.md`](../docs/comm-rom/rs232-live-session-2026-09-14.md) |
