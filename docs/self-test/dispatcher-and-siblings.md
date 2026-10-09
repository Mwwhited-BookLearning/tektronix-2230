# Self-test dispatcher and sibling subroutines

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Found: the self-test dispatcher

**CORRECTION (this session): `self_test_dispatcher` was misnamed.**
Tracing what actually calls `SUB_E094B` (now `init_selftest_report_record`, see next section) led to
reading `0xE416F` (`160-3633`) end-to-end for the first time instead
of just skimming its call list, and it turned out to contain **zero**
OR-fold test calls - it's entirely print/banner setup: `SUB_E3567`
(now `init_print_region`, clear/init a line buffer region), `SUB_E3930`
(now `plot_readout_point`), `SUB_E3854` (now `draw_readout_char`), a
helper that prints two far-pointer strings (`SUB_E4217`, now
`print_banner_line`, itself calling `SUB_E3821`, now
`print_readout_string`, then `print_string_far` twice), then the mirror-image
sequence again, ending with `mov word [0x1B10], 3` and `retf`. This
routine is now renamed `print_selftest_banner`.

The *actual* self-test dispatcher - the one with the ~14-call
OR-fold-into-`[bp-0xA]` pattern originally described here - is the
**next function**, `0xE4244`, called from a completely different
place (`160-3633:0x3DEE`, not from `print_selftest_banner` at all).
It's now renamed `self_test_dispatcher` (taking over the name from the
routine above). `check_comm_option_installed`'s option-detection call
is one link in this chain but notably does NOT get OR'd into the same
accumulator the way its neighbors do - consistent with "is an option
installed" being informational rather than a pass/fail test that
could error out. One test (`SUB_E252A`, now `selftest_tb_divider`) is conditionally skipped based
on `[0x1B83]==0x1E` - the same "comm option RAM/IO confirmed" config
byte `check_comm_option_installed` checks, so this dispatcher already
adapts its own test list based on what `check_comm_option_installed`
found.

`self_test_dispatcher` (`0xE4244`) is called unconditionally from
`0xE3DEE`; there's no `[0x1B10]==0` gate directly on it (that gate is
on the OLD, differently-purposed `print_selftest_banner` call site at
`0x07F8`, which is a sibling/neighbor call in the same outer routine,
not this dispatcher's caller).

**Update: all ~19 sibling test calls now individually identified** (a
later session re-read `self_test_dispatcher`'s body end-to-end more
carefully than the first pass here, which had actually missed a chunk
- see "Identified self_test_dispatcher's sibling subroutines" below
for the full corrected list and how each was found). The earlier text
in this section claiming `SUB_E28FE` (now `selftest_hs_acq`)/`SUB_E227E`
(now `selftest_front_panel_switch_a`)/`SUB_E26D6` (now `selftest_mm_acq`)/
`SUB_E286C` (now `selftest_xy_acq`)/`SUB_E2CEC` (now
`selftest_cursor_delta_time`) were *not* test calls, but part of the
surrounding caller instead, **was wrong** - they are genuine direct
test calls inside `self_test_dispatcher`, confirmed by a full re-read.

`wait_readout_tick` (`0xE0ADD`) throttles `print_string_far`'s
character-output loop to the readout hardware's actual pace, and
`clear_selftest_status_flags` (`0xE0E56`) resets a small group of
status bytes tied to the far-pointer table `SUB_E4443` (now
`init_selftest_register_group`) sets up.

`SUB_E374E` (now `close_print_record`), `SUB_E3821` (now
`print_readout_string`), and `SUB_E0AF5` (now `print_string_far`) -
previously listed here as unidentified sibling test subroutines - are
**not tests at all**. They're display/print primitives, found by
tracing the actual bytes each is called with (far pointers into a
fixed string-table segment, `0xFF7B:xxxx`, landing in `160-3532`) - see
"The readout/CRT display memory" below. This was the actual
correction that triggered re-examining `self_test_dispatcher` in the
first place.

Variables seen so far associated with this self-test machinery (roles
inferred from usage, not confirmed):
- `[0x1B10]` - gates whether `print_selftest_banner` runs at all this
  call (0 → run + print pre-test banner; >0xF → skip both banner and
  everything through label `L_E0894`); also used as a countdown/index
  in a `imul 0x32` calculation in the surrounding caller, role there
  still unclear.
- `[0x1B18]` - written `1` immediately before nearly every individual
  test call inside `self_test_dispatcher`; exact role (progress
  indicator? "last test index"? always the literal `1`, so maybe not
  an index) not yet confirmed.
- `[bp-0xA]` (a caller-local, not a fixed address, inside
  `self_test_dispatcher`) - accumulates OR'd return codes from each
  test into an overall self-test result, returned in `ax`.
- `[0x1BF9]` - the option-board-presence/RAM status byte set by
  `check_comm_option_installed` specifically (bit 1 = valid header found, bit 2 = also
  RAM/IO-backed).
- `[0x1B83]` - a config byte both `check_comm_option_installed` and
  `self_test_dispatcher` check against `0x1E`/`0x14` respectively as
  part of confirming/gating the comm-option RAM/IO result; role
  otherwise unconfirmed.
## Found the top-level self-test orchestrator

`run_selftest_sequence` (`0xE3B12`) ties together nearly everything
this project has learned about the self-test subsystem across
multiple sessions, in one function: it writes a marker byte at
physical `0x00000` (plausibly an `iret`/iret-like opcode planted at
address 0 to make an accidental null-pointer call harmlessly return,
a common embedded-firmware defensive trick), initializes the readout
vector display-list buffer (`[0x1AF4]`/`[0x1AF6]` from `[0x1CC4]`,
`[0x1C02]=0x8000` - the dual-plane offset), runs setup via `SUB_E4443`/
`SUB_E75C0`/`SUB_E128D` (now `init_selftest_register_group`/
`detect_comm_option_hw`/`verify_adc_control_toggle`), picks a
test-mode byte `[0x1B48]` based on
`[0x758]`, calls `init_selftest_report_screen`, and then calls
`print_selftest_report_line` exactly once immediately followed by
`self_test_dispatcher` itself. This is the actual entry point that
starts a self-test run - found by tracing print_selftest_report_line's
callers, which had been an open thread since it was first identified.

Also found this session: the HPGL plotter output driver
(`update_plot_position`/`plot_line_to`, emitting `PU%d,%d;`/
`PD%d,%d;` via `format_string_va`, plus `SP1;SC0,1023,0,1023;` and
`ESC*rB`-style HP-GL2 escape sequences seen nearby) sits directly
around the already-documented I/O port `0x83` write in `160-3532` -
a strong candidate for that port being the GPIB/plotter output,
though not confirmed against a schematic. And `verify_adc_calibration`
(`0xE30B5`) is a calibration-check sibling to `run_adc_selftest`,
comparing the `[0x322]` status register's 12-bit value field against
an expected reference and reporting an "uncaled" error in hex if it
doesn't match.

## Identified self_test_dispatcher's sibling subroutines

**The single biggest lever this project has had for matching self-test
subroutines to real peripherals**: rather than guessing from call
order, went back through each of `self_test_dispatcher`'s ~19 sibling
calls and searched its body for a `mov <reg>, 0xFF7B` (the fixed
string-table segment) paired with an offset, then read the actual
bytes at that physical address (`0xFF7B0 + offset` in `160-3532`).
Nearly every one directly references one of the diagnostic labels
already catalogued in `STRINGS.md` - a much stronger identification
than call-order proximity ever gave. Also corrected: the earlier read
of `self_test_dispatcher` (documented just above) had missed a whole
chunk of its body, wrongly attributing 5 of its real test calls to the
surrounding caller instead - a full re-read fixed this.

Full corrected call list, all now renamed in `gen_disasm_x86.
FUNCTIONAL_NAMES`:

| Call | String found | Test |
|---|---|---|
| `selftest_hs_acq` (`0xE28FE`) | `HS_ACQ` | High-speed acquisition mode |
| `selftest_front_panel_switch_a` (`0xE227E`) | (none - see below) | Front-panel control, range 0-8 |
| `selftest_mm_acq` (`0xE26D6`) | `MM_ACQ` | Min-max acquisition mode |
| `selftest_xy_acq` (`0xE286C`) | `XY_ACQ` | X-Y acquisition mode |
| `selftest_cursor_delta_time` (`0xE2CEC`) | `CDT` / `PRE-DETRIG` / `TIME-OUT` | Cursor delta-time measurement |
| `selftest_acq_ab_addr_walk` (`0xE2FC8`, **RENAMED 2026-10-09** from `selftest_front_panel_switch_b`) | `ACQ_AB` (via its step helper's sole callee, `verify_adc_calibration`) | Acquisition memory address-bus walk (U3423-U3425 -> U3427/U3428), NOT a front-panel control - see correction below |
| an **inline block** (no separate sub) | (none) | Runs `configure_measurement_hw`+poll+`clear_selftest_status_flags` directly in `self_test_dispatcher`'s own body, gated on `[0x1B7A]!=1` - result NOT OR-folded (informational, like `check_comm_option_installed`) |
| `selftest_tb_divider` (`0xE252A`, conditional) | `TB_DIVIDER` (via `HARDWARE.md` photo, not a code string) | Timebase divider, range 0-0x18, only tested if comm option's RAM/IO confirmed |
| `selftest_measure_and_report` (`0xE0FD0`) | (none) | Enable/run/disable measurement (identified previous session) |
| `check_comm_option_installed` (`0xE44F1`) | (n/a, not OR-folded) | Comm/GPIB option detect |
| `selftest_rom_checksum` (`0xE16EA`) | `ROMS` / `MISMATCH` | Main ROM checksum |
| `selftest_comm_rom` (`0xE1E3E`) | `COMM_ROM` | Comm ROM checksum (both its `0x80000` real address and `0x90000` alias) |
| `selftest_comm_loopback_a` (`0xE1D28`) | `COMM_LB` (via `selftest_comm_readback`) | Comm-board loopback, phase A |
| `selftest_comm_loopback_b` (`0xE1DB3`) | `COMM_LB` / `FGET NOT SET` / `FGET NOT CLEAR` (via `selftest_comm_fget_flag`) | Comm-board loopback, phase B |
| `selftest_comm_ram` (`0xE1E90`) | `COMM_RAM` / `CMOS NOT SUPPORTED` | Comm-board RAM |
| `selftest_cmos` (`0xE1F18`) | `CMOS` / `reformated` / `recovered` | CMOS/NVRAM (with recovery) |

(Table order above is by identification method, not call order - see
the raw disassembly for the exact sequence, documented in
`gen_disasm_x86.FUNCTIONAL_NAMES`'s comments.)

The three tests originally grouped together as "front-panel-control
tests" (`selftest_front_panel_switch_a`/`_b`, and the third one) were
identified by a shared distinctive *shape*, not a diagnostic string
directly: each scans `update_menu_position` across a fixed range (0-8,
0-0x15, 0-0x18 respectively) via a small step-helper, exactly the same
mechanism the real menu-navigation cursor uses (see "Menu navigation"
in `VARIABLES.md`). **As of 2026-10-09 this is now known to be too
broad a generalization for all three** - see the correction below:
the middle one (`_b`) turns out to be the `ACQ_AB` acquisition-memory
address-bus test, not a front-panel control at all, even though it
happens to reuse the same `update_menu_position` range-scan shape for
unrelated bookkeeping. The shape match alone was a reasonable starting
heuristic but not sufficient on its own, which is exactly why
`CLAUDE.md`'s convention is to check for a string reference first.

**Update - the third one is `selftest_tb_divider`, not a comm-option
switch.** Tracing `0xE252A`'s step helper (`step_tb_divider_test`,
`0xE255E`) further shows it calls `verify_timebase_prc` with a
shifted `0xFFF` threshold mask - matching the real on-screen name
`TB_DIVIDER` confirmed in the `HARDWARE.md` hardware photos (under
`DIAGNOSTICS/TESTS/ACQUISITION`). Renamed from the earlier
`selftest_comm_option_switch` guess. It's still gated on
`[0x1B83]==0x1E` (comm option confirmed) - possibly the timebase
divider circuit under test needs the comm board's `EXT CLK` line
(see the AUX connector in `HARDWARE.md`), but this isn't confirmed;
the gate doesn't mean the *control* being swept is on the comm board.

**Also found**: `selftest_front_panel_switch_a`'s own step helper
(`run_adc_selftest_range`, `0xE22AF`) is the *same* function later
also confirmed as part of the dedicated `A_TO_D_TESTS` A/D exerciser
(calling `run_indexed_adc_selftest`) - meaning `selftest_front_panel_
switch_a` verifies its swept control via **ADC readback**, not just a
digital position read. This is a real clue for "which control": it's
very plausibly an **analog/potentiometer-based** front-panel control
(read through the A/D converter) rather than a purely digital rotary
switch - VOLTS/DIV is the leading candidate given its 9-ish detent
positions matching the 0-8 sweep range. `selftest_front_panel_
switch_b`'s step helper (`step_front_panel_switch_b_test`, 0xE2FFC)
turns out to use the exact same shape - it also calls an ADC
verification routine (`verify_adc_calibration`) rather than a digital
read, over its 0-0x15 (21-position) range. ~~So both of the still-open
front-panel-control tests are ADC-verified analog controls, not
digital switches - VOLTS/DIV (CH1) and VOLTS/DIV (CH2) is now a more
likely pairing than VOLTS/DIV + TIME/DIV, though not confirmed.~~
**Superseded - see correction immediately below: `_b` isn't a
front-panel control at all.**

**CORRECTION, 2026-10-09: `selftest_front_panel_switch_b` /
`step_front_panel_switch_b_test` are actually the real `ACQ_AB`
self-test, not a front-panel control - renamed to
`selftest_acq_ab_addr_walk` / `step_acq_ab_addr_walk`.** This came out
of the emulator work tracing the power-up self-test log's `ACQ_AB :
read-back 0 <> N` failures (`N` = `2, 6, E, 1E, 3E, 7E, FE, 1FE, 3FE,
7FE, FFE` - a classic address-line walking pattern; see
`emulator/docs/design.md`). Following `CLAUDE.md`'s own
string-cross-reference convention (check the referenced string before
anything else) rather than trusting the earlier shape-only
identification:

- `verify_adc_calibration` (`step_front_panel_switch_b_test`'s sole
  callee, `ref_count: 1`) builds its failure message from a
  **hardcoded literal string "ACQ_AB"** - confirmed by reading the raw
  bytes at `160-3532-14.bin` file offset `0xFD3C` directly (`b'ACQ_AB
  \x00...'`), not a caller-supplied or dynamic label. It also
  references the literal strings "read-back" (`0xFD2E`) and " <>"
  (`0xFD38`) from the same `0xFF7B0`-based string table, exactly
  reproducing the emulator's captured `"ACQ_AB : read-back 0 <> N"`
  message format.
- It compares the far ptr `[0x322]` - which `init_selftest_register_
  group`'s `group==1` branch sets exactly once to the fixed value
  `0x4000:0x377E` (physical `0x4377E`) - against an expected value
  masked with `0xFFE`. `0x4377E` is the already-CONFIRMED (via the
  real service manual's Table 3-1, see `docs/comm-rom/option-
  detection.md`) **"Acquisition Memory Address Buffer Low bits
  U3427."**
- `step_front_panel_switch_b_test` scans positions `0`-`0x15` (22
  positions) computing a walking-bit pattern (`0xFFE` shifted left/
  right by `|position - 0xB|`, re-masked with `0xFFE`) as the expected
  value - this reproduces the emulator's observed 11-entry failure
  sequence `2,6,E,1E,3E,7E,FE,1FE,3FE,7FE,FFE` exactly (shift amounts
  10 down to 0), and the 0-0x15 position count matches the service
  manual's own description of the real `ACQ_AB` test almost exactly:
  *"This test checks the address bus of the acquisition memory.
  Twenty one unique patterns are written into the address counters
  (U3423 U3424 and U3425) and read back through the acquisition
  address buffers (U3427 U3428)."*

This resolves (corrects) the "VOLTS/DIV (CH1) and VOLTS/DIV (CH2)"
guess struck through above - `_b` was never a VOLTS/DIV control at
all. `selftest_front_panel_switch_a` is unaffected by this correction
(different step helper, different callee, no `ACQ_AB`-string match
found for it) and remains the sole still-open "which analog front-
panel control" question, with VOLTS/DIV still the leading candidate
for it alone.

**Still genuinely unresolved**: the *write* side of the real ACQ_AB
test (driving the walking pattern onto the address counters U3423-
U3425 the manual describes) was NOT found anywhere in `step_acq_ab_
addr_walk` or its caller `selftest_acq_ab_addr_walk`. The only
hardware touch in this call chain is a constant `mov byte [es:di], 0`
to physical `0x437BE` (far ptr `[0x326]`) - always writes `0`, never a
per-position pattern - and `update_menu_position` itself (confirmed by
a full read of its body) does no hardware I/O at all, only touching
software bookkeeping variables (`[0x1B50]`, `[0x1B51]`, `[0x1B18]`,
`[0x4E7]`, `[0x4E8]`). `0x437BE` is itself a separately-CONFIRMED,
already-named register (`MEMORY_MAP.md`: **"Acquisition Mode Register
U3310"**) - so this write is plausibly resetting/re-arming the
acquisition hardware into a known mode each step (which might
indirectly reset the address counters on real hardware as a side
effect), rather than writing a per-position address pattern directly;
not confirmed. Also checked `init_selftest_register_group`
(`0xE4443`, the function that sets up `[0x322]` and friends) for a
write hiding there instead - it's pure RAM pointer-table setup (8
far-pointer pairs written to `[0x31E]`-`[0x32C]`), no hardware I/O at
all. So the real write-then-readback coupling mechanism this self-test
depends on is still untraced - consistent with `emulator/docs/
design.md`'s existing conclusion that this specific failure can't yet
be given a faithful (non-static) emulator stub.

The `selftest_display_irq_idle`/`selftest_display_irq_active`
(`0xE3F2C`/`0xE3F99`) pair - referenced from a *different* part of the
self-test flow, not `self_test_dispatcher`'s main OR-fold list, but
found during the same string cross-reference sweep - test the readout/
CRT display controller's interrupt line (see "The readout vector
display list" above): idle-state check (`line stuck high`) and an
active check after drawing a shape and expecting an interrupt within a
timeout (`TIMEOUT`/`unable to reset`).

**Follow-up sweep found 4 more, closing most of the remaining
questions**: wrote a one-off scanner (`disasm/scan_string_refs.py`,
not part of the regular pipeline, kept for reuse) that automates the
same technique across every still-unnamed function at once - for each
one, find any `mov reg, 0xFF7B` within 2 lines of another `mov reg,
0xNNNN`, and read the string at that computed address. Found:
- `format_selftest_result_string` (`0xE0C3D`) - **this is the missing
  link** noted in earlier sessions ("the actual message printing
  happening elsewhere, not yet found"). Builds `PASSED`/`FAILED`/
  `UNTESTED`/`Not installed` text from a status-bits argument:
  `0x20`=not installed, `0x02`=failed, `0x01`=passed, else untested -
  this is the bit encoding used throughout `self_test_dispatcher`'s
  OR-folded `[bp-0xA]` accumulator.
- `selftest_front_panel_adc` (`0xE296E`) - references `FP_a2d`,
  identifying the peripheral behind the `configure_measurement_hw`/
  `run_adc_selftest`/`selftest_measure_and_report` cluster: **the
  front-panel A/D converter**, not an acquisition-channel ADC as
  originally guessed.
- `ram_pattern_test` (`0xE1B89`) - a generic memory test engine
  (alternating `0xAA`/`0x55` write, then masked read-back verify) -
  the likely shared implementation behind the `SYS_RAM`/`NIB_RAM`/
  `ACQ_RAM`/`COMM_RAM` self-tests. Identified from its code shape
  alone, no string needed.
- `selftest_comm_readback` (`0xE20B0`) - the 2nd phase called by
  `selftest_comm_loopback_a`, references `COMM_RB`/`rb(1)=`/`rb(0)=`.
  It reads/writes physical `0x40000+0x67C`/`0x6F8` - **previously
  flagged as "surprising and unreconciled" (nowhere near the comm ROM's
  `0x80000` address), now fully resolved with the real service manual
  (provided 2026-09-13)**: `0x4067C` is the comm option's own **"Option
  Status Latch (in)"** and `0x406F8` its **"Option Interrupt Mask Latch
  (out)"** (service manual Table 3-1, "Memory Space Allocation") - both
  genuine, confirmed comm-option registers. "COMM" in `COMM_RB` really
  does mean the GPIB/RS-232 option board after all; the earlier
  confusion was just not realizing how much *other* hardware (front-
  panel, acquisition, and comm-option registers alike) is scattered
  through this same `0x40000-0x437FF` "I/O Main Image" window alongside
  the readout/CRT registers - it's a shared I/O address space, not
  readout-specific memory that the comm option happens to also use.
