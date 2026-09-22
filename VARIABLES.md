# Variable list

One entry per memory location whose role is understood (fully or
partially), across all three ROMs. Companion to `FUNCTIONS.md` (which
covers routines) — together they're the human-readable index of
what's been identified so far. Add an entry whenever a variable's
usage becomes clear, even partially; mark inferred/guessed meanings as
such rather than stating them as confirmed.

Addresses are given as `[DS-relative or physical]` matching however
the code actually references them (most global variables are accessed
relative to `DS=0x0041`, i.e. physical `0x00410+offset` - see
`MEMORY_MAP.md`).

## Self-test / diagnostics

| Address | Role | Confidence |
|---|---|---|
| `[0x1B10]` | Gates whether `print_selftest_banner` (`0xE416F`) runs at all (`cmp word [0x1B10],0 / jne skip`); also used as a row-index in `print_selftest_report_line`'s `[0x1B10]*0x32` row-Y computation | Usage confirmed; exact meaning (mode flag? "self-test already ran this power-cycle"?) not confirmed |
| `[0x1B18]` | Written literal `1` after nearly every individual self-test call inside `self_test_dispatcher` (`0xE4244`) - **also reused outside the self-test context**, by `update_menu_position` (`0xE06B6`), suggesting it's a general "state changed"/"redraw needed" flag rather than something self-test-specific | Usage confirmed; general "changed" flag is the best-fit inference, not confirmed |
| `[bp-0xA]` (local, not fixed) | Accumulates OR'd return codes from each self-test call into an overall result word, inside `self_test_dispatcher` (`0xE4244`) | Confirmed |
| `[0x1BF9]` | Option-board presence/RAM status byte, set by `SUB_E44F1`: bit 1 = valid ROM header checksum found at the probed address, bit 2 = that address is also RAM/IO-backed | Confirmed |
| `[0x1BFA]` | **Traced 2026-09-13**: set to `0` at the very start of `selftest_comm_readback` (`0xE20B0`), then set to `1` only if that function's readback byte (from physical `0x4067C`, masked/shifted into `[bp-7]`) equals exactly `0xD0` (one of 5 values - `0x40`/`0x60`/`0xC0`/`0xD0`/`0xE0` - that all count as a passing readback, but only `0xD0` also sets this flag). Read by `selftest_comm_fget_flag` (`0xE1FBC`, the real body of `selftest_comm_loopback_b`): if nonzero, runs its own real hardware check; if zero, falls through to checking `[0x1B7A]` instead - see below for why this is the direct explanation for `COMM_LOOPBACK` showing `UNTESTED` on both live-tested units | Confirmed via full instruction trace |
| `[0x1B7A]` | **Traced 2026-09-13**: read (`==1`) at a very large number of self-test-adjacent sites throughout the main ROM, but only 2 write sites found (both in the `160-3532-14` half, heuristic-only): `FUNC_3532_7BA5` (`0xF7BA5`) sets it to `1` right at entry, `FUNC_3532_7D99` (`0xF7D99`) sets it to `0` right before calling `reset_display_and_notify_comm`. Best-fit interpretation: a "the full automated self-test sequence is actively running" flag (set on entry, cleared on exit/reset) - distinct from invoking a single test standalone from the `EXERCISERS` menu. This is the second half of the `COMM_LOOPBACK`/`UNTESTED` explanation: when `[0x1BFA]==0` (the common case - readback wasn't exactly `0xD0`) *and* `[0x1B7A]!=0` (running as part of the full sequence, which is how the user's live tests were invoked - see `HARDWARE.md`'s menu tree, `DIAGNOSTICS/TESTS/SYSTEM`), `selftest_comm_fget_flag` returns `0x80` - a value `format_selftest_result_string` (`0xE0C3D`) doesn't recognize as pass(`0x01`)/fail(`0x02`)/not-installed(`0x20`), so it prints as the generic "else" case: **`UNTESTED`**. Not yet confirmed whether running `COMM_LOOPBACK` standalone from `EXERCISERS` (rather than the full `TESTS/SYSTEM` battery) would show a real result instead, since that would need `[0x1B7A]==0` at call time - worth trying live | Mechanism and write sites confirmed (heuristic-reachability caveat: the 2 write sites aren't in the proven-only listing); "self-test sequence active" semantic is the best-fit inference, not independently confirmed by a named string or manual cross-reference |
| `[0x1B83]` | The comm-option-board detection result. Set by `detect_comm_option_hw` (`0xE75C0`) to `0x1E` or `0x14` based on a write-then-readback hardware probe (bit `0x1000` of a word at physical `0x40000+0x377E`, poked via a write to `0x40000+0x7DE`); checked `==0x1E` by `check_comm_option_installed`, `self_test_dispatcher` (gates one test), and elsewhere | Source function confirmed; which specific value definitively means "installed" vs "not installed" not fully resolved - `0x1E` results from two different branches with opposite-seeming conditions, see `docs/comm-rom/option-detection.md` |

## Task scheduler / hardware polling

| Address | Role | Confidence |
|---|---|---|
| `[0x752]` | Tick counter, incremented once per `INT2` timer interrupt by `scheduler_tick_service`; `wait_readout_tick` busy-waits for this to change | Confirmed |
| `[0x758]` | **Confirmed 2026-09-13: this is `SWB2` (service manual Table 6-16/6-17), the front-panel "Switch Bank 2" byte, physical `0x40000+0x3FFA`=`0x43FFA` = Front Panel Buffer `U9301`** (Table 3-1). Read every tick by `scheduler_tick_service`. Exact bit map (bit0=LSB): bit0=`MEM 3`, bit1=`MEM 1`, bit2=`POS/SEL`, bit3=`1K/4K`, bit4=`MENU`, bit5=`MEM 2`, bit6=`MENU ADV`, bit7=`SELECT C1/C2`. **Validated by comparing code structure to these exact bits**: the self-test report loop (`0xE3B6A` and 7 other sites near it) masks `[0x758] & 0x63` (binary `01100011` = bits 0,1,5,6 = `MEM 3`+`MEM 1`+`MEM 2`+`MENU ADV`) as one abort/branch condition and separately checks `& 0x80` (`SELECT C1/C2`) as another - this exactly matches the service manual's own documented behavior ("If the SELECT C1/C2 button is held in while the test is running, the test loops on the first error") plus the complete set of menu-navigation buttons (`MEM 1`/`2`/`3` = the repurposed `Menu Select` buttons, `MENU ADV` = `SAVE REF/►` - see `HARDWARE.md`'s "Menu navigation control scheme"). A precise, independently-confirmed match between the raw bit mask and the named switches. **Bit3 (`1K/4K`) independently confirmed by live hardware test 2026-09-13**: pushing the acquisition record-length button (`1K`/`4K`) toggled the `FP_VALUES` `dig=` field's 3rd octet exactly `0x08`↔`0x00` (`XOR=0x08`=bit3 alone) - an exact single-bit match | Confirmed; bits 0,1,3,5,6,7 now bit-level validated (code structure + live test), bits 2/4 (`POS/SEL`/`MENU`) not yet independently isolated |
| `[0x759]` | By the same address derivation, this should be `SWB1` (`U9302`, physical `0x43FFB` = Front Panel Buffer `U9302`): bit0=`A ONLY`(active-low), bit1=`PRE/POST`, bit2=`HOR CAL`, bit3=`HOR MAG`, bit4=`ROLL`, bit5=`HOLD`, bit6=`B ONLY`(active-low), bit7=`STORE ON`. **Independently confirmed by live hardware test 2026-09-13** (running `FP_VALUES`, reading its `dig=` field's 4th octet): `HORIZONTAL MODE` `A`/`BOTH`/`B` gave `0x40`→`0x41`→`0x01` (bit0=`A ONLY`, bit6=`B ONLY`, both active-low - exact match); **`ROLL`** toggled `0x44`↔`0x54` (`XOR=0x10`=bit4 alone - exact match); **`PRETRIG`** toggled `0x44`↔`0x46` (`XOR=0x02`=bit1 alone - exact match with `PRE/POST`); **`SAVE`** toggled `0x44`↔`0x64` (`XOR=0x20`=bit5 alone - exact match with `HOLD`, plausible if `SAVE` and hold-acquisition share a line); **`STORE`** toggled `0x44`↔`0xC4` (`XOR=0x80`=bit7 alone - exact match with `STORE ON`). Every tested bit landed on a clean single-bit XOR against the predicted name - strong confirmation of the whole byte, not just isolated bits | **Confirmed**: bits 0,1,4,5,6,7 bit-level validated by live test; only bit2 (`HOR CAL`) and bit3 (`HOR MAG`) remain untested |
| `[0x7B4]` | Previous snapshot of `[0x758]`, XOR'd against the new read each tick for edge/change detection | Confirmed |

## Menu navigation

| Address | Role | Confidence |
|---|---|---|
| `[0x1B50]` | Current menu/parameter cursor position, updated and clamped between caller-given min/max by `update_menu_position` (`0xE06B6`) | Confirmed |
| `[0x1B51]` | A flag `update_menu_position` sets/checks alongside `[0x1B50]` - looks like a "just wrapped" or "pending redraw" indicator for the cursor | Usage confirmed; exact meaning not confirmed |
| `[0x4E7]`, `[0x4E8]` | Front-panel button/control state bytes - bit 7 of each checked by `update_menu_position` for a simultaneous-both-held "accelerate" case (`al=[0x4E7]&0x80` then `al=[0x4E8]&0x80`, both must be set) | Usage confirmed; which physical controls these represent not confirmed. **Complication found 2026-09-13**: `[0x4E8]` is also the base of a small 4-entry byte array (indexed `[di+0x4E8]`, `di`=`index*4` for `index` 0-3, i.e. `[0x4E8]`/`[0x4EC]`/`[0x4F0]`/`[0x4F4]`, cleared in a loop at `0xE8E7E`) and is separately read with mask `0x73` (not `0x80`) at `0xE8E33` gating `handle_gpib_device_clear` - so `[0x4E8]` is overloaded: sometimes a per-control state byte (`update_menu_position`'s use), sometimes the head of an unrelated 4-entry array, sometimes read for other bits entirely. The "front-panel control" interpretation is solid for `update_menu_position`'s specific `&0x80` check; don't assume it holds for every reference to this address. Cross-reference attempt against the service manual's `SWB1`/`SWB2` bit names (see `HARDWARE.md` "Menu navigation control scheme") didn't reach a confirmed mapping - `SWB1`/`SWB2` are multi-bit hardware registers holding several distinct named switches, while `[0x4E7]`/`[0x4E8]`'s `&0x80` test looks more like a single "this control is currently active" flag per byte (e.g. a rotary encoder's own momentary-activity bit) - plausible these are a *derived*, software-side per-control byte pair fed by reading `SWB1`/`SWB2` elsewhere, not the same register directly, but that intermediate step hasn't been found. **New context found the same day**: a heuristic-only function (`FUNC_3633_92D1`, `0xE92D1` - not confirmed reachable, no call site to it found yet) initializes a whole cluster of neighboring bytes together - `[0x4E0]`, `[0x4E7]`, `[0x4E8]`, `[0x4EB]`, `[0x4EC]`, `[0x4F0]`, `[0x4F4]`, `[0x4F7]`, `[0x4F8]`, `[0x4FB]`, `[0x4FC]` - to different specific default patterns depending on which branch of the `[0x1B83]==0x14` comm-detection dispatch is taken, confirming these ~11 bytes really are one related group (not coincidentally adjacent), and tying their defaults to the same still-unresolved `[0x1B83]` hardware-variant question. See `docs/comm-rom/option-detection.md` "The `[0x4E0]`-`[0x4FC]` cluster" for the full byte-by-byte breakdown. Still open. |

## Front-panel A/D converter and switch banks (`FP_VALUES` exerciser findings)

The service manual's `FP_VALUES` exerciser (Table 6-16, "Display
Format") shows several distinct fields side by side on the CRT. Live
testing 2026-09-13 (the user running the exerciser and manipulating
one control at a time) has now sorted most of the raw hex changes into
four separate fields, cross-referenced below. `E`-prefixed designators
are schematic test points feeding an analog multiplexer ahead of the
front-panel ADC (`U6105`, per `MEMORY_MAP.md`'s "Two separate ADCs"
note).

### `AD DATA` (per-channel analog, 2 bytes/channel: `E114`/`E115` for `CH 1`, `E164`/`E165` for `CH 2`)

**Fully confirmed by direct manipulation**:
- Byte 1 (`E114`/`E164`) = **`POSITION`** - `CH1 POSITION` moves only
  `CH 1`'s byte 1; `CH2 POSITION` moves only `CH 2`'s byte 1.
- Byte 2 (`E115`/`E165`) = **`VOLTS/DIV`** - `CH1 VOLTS/DIV` moves only
  `CH 1`'s byte 2; `CH2 VOLTS/DIV` moves only `CH 2`'s byte 2. This
  directly confirms the `selftest_front_panel_switch_a`/`_b` hypothesis
  (see `docs/self-test/front-panel-switches.md`'s "Also found" paragraph) by live manipulation
  rather than code inference alone.

**Unresolved**: the CH1 **`VAR/CAL`** switch (calibrated detent)
changes the third nibble of a third octet somewhere in the display
line - Table 6-16's `AD DATA` column only lists 2 bytes per channel,
so this belongs to a different, not-yet-identified column (`ISTAT`?).

### `dig=` (digital switch bytes - **confirmed as `SWB2`/`SWB1` on octets 3/4**)

`dig=` octet 3 = **`SWB2`** (`[0x758]`), octet 4 = **`SWB1`**
(`[0x759]`) - confirmed by several independent single-bit toggles
landing exactly on the manual's named bits, not just address
adjacency (`0x43FFA`/`0x43FFB`):

| Octet | Bit | Name | Control tested | Values (XOR) |
|---|---|---|---|---|
| 4 (`SWB1`) | 0 | `A ONLY` (active-low) | `HORIZONTAL MODE`=A | `0x40` |
| 4 (`SWB1`) | 6 | `B ONLY` (active-low) | `HORIZONTAL MODE`=B | `0x40` |
| 4 (`SWB1`) | 1 | `PRE/POST` | `PRETRIG` | `0x44`↔`0x46` (`0x02`) |
| 4 (`SWB1`) | 4 | `ROLL` | `ROLL` | `0x44`↔`0x54` (`0x10`) |
| 4 (`SWB1`) | 5 | `HOLD` | `SAVE` | `0x44`↔`0x64` (`0x20`) |
| 4 (`SWB1`) | 7 | `STORE ON` | `STORE` | `0x44`↔`0xC4` (`0x80`) |
| 3 (`SWB2`) | 3 | `1K/4K` | acquisition `1K`/`4K` | `0x08`↔`0x00` (`0x08`) |
| 3 (`SWB2`) | 2 | `POS/SEL` | `WAVEFORM SELECT` | `0x08`↔`0x0C` (`0x04`) |
| 3 (`SWB2`) | 7 | `SELECT C1/C2` | `CURSOR` button push | `0x08`↔`0x88` (`0x80`) |

`SWB1` bits 2 (`HOR CAL`)/3 (`HOR MAG`) and `SWB2` bits 0,1,4,5,6
(already code-validated separately, see `[0x758]` above) remain the
only bits on octets 3/4 without their own live-test confirmation.

**Octet 1 - still unidentified** (not `SWB1`/`SWB2`, both now
well-accounted-for on octets 3/4): `VERT MODE` (CH1/BOTH/CH2) and `XY`
mode both changed this octet (`XY` toggled `0x73`/`0x63`), and `STORE`
combined with horizontal `B ONLY` toggled a clean single bit (`0x27`↔
`0x2F`, bit3). Likely a third digital status byte (`ISTAT`?) - not yet
cross-referenced to the manual, needs a slower one-control-at-a-time
re-test to isolate cleanly.

**Other raw/unresolved `dig=`-area findings**:
- Input coupling switch (`AC`/`GND`/`DC`) changed a byte's value
  (transcription uncertain - `0x12`-ish/`0x2AA`-ish/`0x3xE`-ish across
  the three positions); needs a repeat test reading the hex back
  digit-by-digit.
- Sweep **`VAR/CAL`** (uncalibrated `SEC/DIV` detent) moves octet 4
  (the same byte `HORIZONTAL MODE` moves) by more than one bit at once
  (`0x44`↔`0x4F` in one report, `0x40`↔`0x4F` in another - the two
  "in-cal" baselines disagree, likely a dictation slip). Since this is
  a multi-bit jump rather than a clean single-bit flip, it's probably
  not just `SWB1` bit2 (`HOR CAL`) toggling in isolation - needs an
  isolated re-test (nothing else touched, hex read back carefully)
  before drawing a bit-level conclusion.

### `cursor` (a separate ≥3-byte field, distinct from `dig=` and `AD DATA`)

- **`CURSOR` knob** (turning it, not the push-button above) changes
  the *first two* bytes - plausibly a 16-bit cursor position value.
- **`B DELAY`** (delayed-sweep time control) changes the *third* byte
  separately - consistent with `B DELAY` being read through its own
  ADC channel rather than a digital switch.
- Neither byte's `E`-designator/register is identified yet.

### `ASW`/`BSW` (confirmed: per-timebase `A`/`B` sweep-control status words)

- **`STORE`** changes both `ASW` and `BSW` (exact bits not yet
  reported).
- **`SEC/DIV`** changes `ASW` in its normal (not-pulled) position and
  `BSW` when pulled out - matching this scope's known pull-knob
  convention (pulled `SEC/DIV` engages delayed-sweep/`B` timebase
  control), confirming `ASW`/`BSW` really are the `A`/`B`-timebase
  sweep-control words rather than an unrelated pair.

## Acquisition/plot scaling

Found via `draw_pending_line_segment`'s (`0xF0C2A`) own previously-
unreached preamble (`0xF09C0`-`0xF0C29`), only made reachable this
session by decoding `init_far_pointer_table_sysrom`'s embedded RAM-init
table and adding 15 new `ENTRY_POINTS` - see `docs/decode-anomalies/landing-artifacts-and-jump-tables.md` "Found:
a whole family of never-reached functions via the RAM far-pointer init
table" and "Systematic landing-artifact sweep".

| Address | Role | Confidence |
|---|---|---|
| `[0x712]`, `[0x714]` | X-axis scale inputs - `[0x712]&0x3FF` used as the `imul` multiplicand against `[0x716]`, result `idiv`'d by `[0x71A]`, feeding `[0x6BC]` (plot X position). **Note**: the comm ROM's `process_gpib_command_byte`/`0x97905` *also* reference a `[0x712]` far pointer, but under `DS=0x8F80` - a completely different physical address (`0x8FF12`) than this main-ROM variable (which uses the main ROM's own `DS`) - purely a coincidental low-offset reuse across separate ROMs/data segments, not the same memory. See `docs/comm-rom/rs232-early-investigation.md`'s "Follow-up: found [0x712]'s actual contents" for the comm ROM's version, an ASCII-indexed byte-classification table | Usage confirmed; physical meaning (a sample index? a time value?) not confirmed |
| `[0x716]`, `[0x71A]` | Paired with `[0x712]`/`[0x714]` in the same `imul`/`idiv` scale computation - `[0x71A]` set from a byte read via far ptr `[0x1DB4]` in `SUB_F09C6` | Usage confirmed; not confirmed |
| `[0x718]`, `[0x71C]` | Y-axis counterparts of `[0x712]`/`[0x716]` - `[0x718]` compared against `[0x71C]` to pick which of 2 `imul`/`idiv` branches computes `[0x6BE]` (plot Y position) | Usage confirmed; not confirmed |
| `[0x71E]` | Read and `shl`'d by 1 in `SUB_F09C6`, indexes far ptr `[0x1DB4]` to set `[0x716]` | Usage confirmed; not confirmed |
| `[0x722]`, `[0x724]` | Sign-extended (bit `0x400` checked, `or 0xFC00` if set - classic 10-bit-to-16-bit sign extension) from 2 words read via far ptr `[0x1DB4]`+offsets `3`/`7`; added into the X/Y scale results above | Usage confirmed (sign-extension pattern is a strong, well-understood x86 idiom); source data's real-world meaning not confirmed |
| `[0x1DB4]` | A far pointer read repeatedly through this whole preamble as a small lookup table (byte reads at `+0`/`+1`, word reads at `+3`/`+7`) - feeds the scale-factor variables above. **Not related to the stroke-font pointer `[0x1DB0]`** (see below) despite being nearby | Usage confirmed; what table it points to and who sets it not confirmed (same open question as `[0x1DB0]`/`[0x1DB8]`/`[0x1DBC]` - nothing writes any of these 4 pointers in proven or heuristic code) |
| `[0x1DB8]`, `[0x1DBC]` | A pair of small per-record **byte-value caches** (NOT font/glyph data - ruled out this session, see `TODO.md`'s stroke-font item): indexed by `[bp-0x10]` (a channel/record index), storing/comparing an 8-bit value derived by shifting the current plot Y position `[0x6BE]` right 6 bits (`>>6`), used to clamp `[0x6BE]` against a previous cached value before `draw_pending_line_segment` actually plots - almost certainly 2 more instances of the already-documented `reset_all_channel_plot_caches` per-channel plot-scale-cache family | Mechanism confirmed via full byte trace; exact real-world unit of the cached value (a coarse Y bucket for redraw-skip decisions is the leading guess) not confirmed |
| `[0x1DB0]` | A far pointer to a **128-entry table of far pointers** (indexed `char_code*4`, masked to 7 bits), read by `draw_readout_char` (`0xE3854`) - each entry is itself a far pointer, presumably to that character's stroke-vector glyph data. This is the best remaining candidate for the long-sought stroke-font glyph table (see `TODO.md`) - structurally distinct from `[0x1DB4]`/`[0x1DB8]`/`[0x1DBC]`'s simple byte-array shape, so it is NOT just a 4th instance of the same small-cache family | Usage/structure confirmed; the actual stored far-pointer value (where the table itself lives in ROM) still not found - nothing writes `[0x1DB0]` in proven or heuristic code |

## Comm ROM (RS-232/GPIB option) variables

| Address | Role | Confidence |
|---|---|---|
| `[0x6E2]` (comm ROM) | A far pointer, initialized by the comm ROM's own `init_far_pointer_table` to `0x406F:0x0008` = physical `0x406F8` - the service manual's **Option Interrupt Mask Latch** (`U1236`). **Traced 2026-09-13** (see `docs/comm-rom/rs232-early-investigation.md`'s "Traced the interrupt mask latch's real outputs"): `es:[di]` (output `0D`, physical `0x406F8`) = RX-ready flag = the UART's `DR` interrupt enable/mask bit; `es:[di+1]` (output `1D`, `0x406F9`) = TX-ready flag = `TBRE` interrupt enable/mask bit; `es:[di+3]` (output `3D`, `0x406FB`) = a diagnostic/strobe bit used by `selftest_comm_readback` and `poll_dip_switch_change`, not an interrupt mask despite living in the same latch. Output `2D` (`0x406FA`) has no code reference found anywhere | Base pointer and outputs `0D`/`1D`/`3D` confirmed via direct trace; `2D`'s role (possibly `RLSD`/`DCD` generation, per the manual) not found |
| `[0x732]`/`[0x734]` (comm ROM) | A far pointer, read via `les di,[0x732]` at **89 sites** and `les bx,[0x732]` at **18 more** (107 total, found 2026-09-14 via a raw byte-level scan for the exact instruction encodings) - by far the most pervasively-referenced comm-ROM pointer found so far, clearly a central "current command/device context record." Many fields of the structure it points to are written elsewhere in the ROM (offsets `+0x8A`,`+0x8C`,`+0x8D`,`+0x93`-`+0x97` seen as `mov es:[di+N], imm8/imm16` stores). Field `+0x1F` specifically holds a numeric ID matching this session's extracted command-keyword dispatch table's alphabetical-order IDs exactly (`0x14`=`LONg`, `0x15`=`MESsage`, `0x1E`=`REFStat`, confirmed via `compute_response_format_flags`). Field `+0x94` is cleared to `0` by two small near-duplicate functions (physical `0x8532D`/`0x85367`) sitting right next to the already-confirmed `process_gpib_command_byte`/`parse_next_gpib_command_byte`/`set_comm_critical_flag` byte-processing cluster - both call `set_comm_config_flag(0, 0x1D)` immediately before the clear, suggesting `+0x94` is a per-command "processing in progress"/error-pending style flag reset once a command finishes or aborts | Pointer's centrality and read-site count confirmed via exhaustive byte scan; the struct's full field layout is not mapped, and whether `+0x1F` is genuinely "current parsed command's ID" throughout, or coincidentally reused for an unrelated "device type" value at boot time (see `init_comm_device_type_and_defaults`, which checks the same field before any command has been parsed), is not resolved. **The write site for `+0x1F` itself was searched for exhaustively (every byte/word write form via both confirmed base registers, immediate and register-sourced, plus a nearby bulk-copy) and not found** - see `docs/comm-rom/rs232-live-session-2026-09-14.md` |

## Memory regions (not individual variables, but the pools they live in)

| Address | Role | Confidence |
|---|---|---|
| `0x00000-0x003FF` | Interrupt vector table | Confirmed (see `MEMORY_MAP.md`) |
| `DS=0x0041` (physical `0x00410+`) | Main global/static variable pool, starts immediately after the IVT | Confirmed region; individual variables mostly unidentified beyond the ones listed above |
| `SS:SP` in segment `0x4000` (physical `0x40000+`) | Stack + scratch buffers | Confirmed (see `MEMORY_MAP.md`) |

## Not yet identified but seen referenced

These addresses came up during disassembly (e.g. as operands of `mov`/
`cmp` against fixed offsets) but haven't been traced to a specific
role yet. Listed here as known targets for future investigation rather
than left buried in `.lst` files:

- `[0x780]` — referenced early in boot-adjacent code, role unknown.
- `[0x61A]`, `[0x61B]` — referenced in `SUB_E004F` (one of the first
  functions in the main ROM), a small getter/setter-looking pair.
- `[0x1C94]`, `[0x1DDC]` — found 2026-09-22 while manually tracing
  `160-3633` physical `0xEFF99-0xEFFED` (a byte range `UNKNOWN_DATA.md`
  had flagged as unidentified data - see `docs/decode-anomalies/
  unknown-data-deep-dive-2026-09-15.md`'s "Follow-up, 2026-09-22"
  section for the full trace; that range turned out to be real code,
  not data). Both are loaded via `les`/`lds`-style far-pointer reads
  (`c4 1e 94 1c` / `c4 1e dc 1d`); the original static read found one
  call site each with `di` scaled by a small multiplier (`*8` for
  `[0x1c94]`, `*9`/`*10` for `[0x1ddc]`), which looked like the classic
  shape of a RAM pointer to a fixed-stride record table, same pattern
  as the comm ROM's `[0x732]` above.

  **Revised same day after a live emulator check.** A full grep of both
  ROMs' readable disassembly found `les ..., [0x1ddc]` at **64 call
  sites** (`di`/`bx`/`dx` as the destination register) - not a rare,
  single-purpose pointer at all. Watching both cells live
  (`AccessCounter.install_watch_range`, a narrow 4-byte range each,
  over a 5M-instruction boot trace that hits this ROM's "POWER UP
  FAILURES"/RAM-NMI-error path) showed 24 of those 64 sites actually
  execute, but the far-pointer value read back at `[0x1ddc]` varies
  wildly call to call - `0000:0000` most of the time, but also
  `3020:2020` (9 hits, all from the one recurring site at physical
  `0xE126C`, apparently called periodically - roughly every 10,000
  instructions), `FF00:0400`, `4100:4107`, and `746E:6320` (the last is
  literally ASCII bytes, i.e. this cell was holding leftover string
  data at that moment, not a pointer at all). That inconsistency is
  the opposite of what a dedicated, stable "record table pointer"
  variable should look like - **`[0x1ddc]` is far more likely
  general-purpose/reused scratch RAM that many unrelated routines
  borrow for their own temporary far pointer, not a single owned
  table**. The `*9`/`*10` stride seen at one call site may still be
  real for *that* caller, but it doesn't generalize to the variable as
  a whole. `[0x1c94]`'s one known call site (`160-3532` physical
  `0xFB94A`) never executed in this same run, so it's still unverified
  either way. Neither cell's actual table contents/purpose are
  confirmed; treat the "record table" framing above as superseded, not
  as fact. (Separately: this check also caught a mistake in how the
  live run's own access log was read the first time - `AccessCounter`
  doesn't count memory *reads* unless a `watch` range is explicitly
  installed for that run, per `io_stubs.AccessCounter`'s documented
  Unicorn 2.1.4 stack-corruption bug; an earlier read of a pre-existing
  dump that had no watch installed showed `read=0` for both cells, and
  that was wrongly reported as "nothing ever reads these" rather than
  "this dump never watched reads.")
