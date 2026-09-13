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
| `[0x1B83]` | The comm-option-board detection result. Set by `detect_comm_option_hw` (`0xE75C0`) to `0x1E` or `0x14` based on a write-then-readback hardware probe (bit `0x1000` of a word at physical `0x40000+0x377E`, poked via a write to `0x40000+0x7DE`); checked `==0x1E` by `check_comm_option_installed`, `self_test_dispatcher` (gates one test), and elsewhere | Source function confirmed; which specific value definitively means "installed" vs "not installed" not fully resolved - `0x1E` results from two different branches with opposite-seeming conditions, see `disasm/NOTES.md` |

## Task scheduler / hardware polling

| Address | Role | Confidence |
|---|---|---|
| `[0x752]` | Tick counter, incremented once per `INT2` timer interrupt by `scheduler_tick_service`; `wait_readout_tick` busy-waits for this to change | Confirmed |
| `[0x758]` | **Confirmed 2026-09-13: this is `SWB2` (service manual Table 6-16/6-17), the front-panel "Switch Bank 2" byte, physical `0x40000+0x3FFA`=`0x43FFA` = Front Panel Buffer `U9301`** (Table 3-1). Read every tick by `scheduler_tick_service`. Exact bit map (bit0=LSB): bit0=`MEM 3`, bit1=`MEM 1`, bit2=`POS/SEL`, bit3=`1K/4K`, bit4=`MENU`, bit5=`MEM 2`, bit6=`MENU ADV`, bit7=`SELECT C1/C2`. **Validated by comparing code structure to these exact bits**: the self-test report loop (`0xE3B6A` and 7 other sites near it) masks `[0x758] & 0x63` (binary `01100011` = bits 0,1,5,6 = `MEM 3`+`MEM 1`+`MEM 2`+`MENU ADV`) as one abort/branch condition and separately checks `& 0x80` (`SELECT C1/C2`) as another - this exactly matches the service manual's own documented behavior ("If the SELECT C1/C2 button is held in while the test is running, the test loops on the first error") plus the complete set of menu-navigation buttons (`MEM 1`/`2`/`3` = the repurposed `Menu Select` buttons, `MENU ADV` = `SAVE REF/►` - see `HARDWARE.md`'s "Menu navigation control scheme"). A precise, independently-confirmed match between the raw bit mask and the named switches | Confirmed |
| `[0x759]` | By the same address derivation, this should be `SWB1` (`U9302`, physical `0x43FFB` = Front Panel Buffer `U9302`): bit0=`A ONLY`(active-low), bit1=`PRE/POST`, bit2=`HOR CAL`, bit3=`HOR MAG`, bit4=`ROLL`, bit5=`HOLD`, bit6=`B ONLY`(active-low), bit7=`STORE ON`. **Bit0/bit6 now independently confirmed by live hardware test 2026-09-13**: running `FP_VALUES` and cycling `HORIZONTAL MODE` through `A`/`BOTH`/`B` changed the exerciser's digital-switch byte exactly `0x40`→`0x41`→`0x01`. In binary that's `A`=`0100 0000` (bit0=`0`=asserted/active-low, bit6=`1`=inactive), `BOTH`=`0100 0001` (bit0=`1`, bit6=`1`, both inactive - neither "only"), `B`=`0000 0001` (bit0=`1`=inactive, bit6=`0`=asserted) - an exact match to the predicted `A ONLY`/`B ONLY` active-low bit map, by real physical control manipulation rather than address pairing alone | **Bit0 (`A ONLY`) and bit6 (`B ONLY`) confirmed by live test**; remaining bits still only inferred from address pairing |
| `[0x7B4]` | Previous snapshot of `[0x758]`, XOR'd against the new read each tick for edge/change detection | Confirmed |

## Menu navigation

| Address | Role | Confidence |
|---|---|---|
| `[0x1B50]` | Current menu/parameter cursor position, updated and clamped between caller-given min/max by `update_menu_position` (`0xE06B6`) | Confirmed |
| `[0x1B51]` | A flag `update_menu_position` sets/checks alongside `[0x1B50]` - looks like a "just wrapped" or "pending redraw" indicator for the cursor | Usage confirmed; exact meaning not confirmed |
| `[0x4E7]`, `[0x4E8]` | Front-panel button/control state bytes - bit 7 of each checked by `update_menu_position` for a simultaneous-both-held "accelerate" case (`al=[0x4E7]&0x80` then `al=[0x4E8]&0x80`, both must be set) | Usage confirmed; which physical controls these represent not confirmed. **Complication found 2026-09-13**: `[0x4E8]` is also the base of a small 4-entry byte array (indexed `[di+0x4E8]`, `di`=`index*4` for `index` 0-3, i.e. `[0x4E8]`/`[0x4EC]`/`[0x4F0]`/`[0x4F4]`, cleared in a loop at `0xE8E7E`) and is separately read with mask `0x73` (not `0x80`) at `0xE8E33` gating `handle_gpib_device_clear` - so `[0x4E8]` is overloaded: sometimes a per-control state byte (`update_menu_position`'s use), sometimes the head of an unrelated 4-entry array, sometimes read for other bits entirely. The "front-panel control" interpretation is solid for `update_menu_position`'s specific `&0x80` check; don't assume it holds for every reference to this address. Cross-reference attempt against the service manual's `SWB1`/`SWB2` bit names (see `HARDWARE.md` "Menu navigation control scheme") didn't reach a confirmed mapping - `SWB1`/`SWB2` are multi-bit hardware registers holding several distinct named switches, while `[0x4E7]`/`[0x4E8]`'s `&0x80` test looks more like a single "this control is currently active" flag per byte (e.g. a rotary encoder's own momentary-activity bit) - plausible these are a *derived*, software-side per-control byte pair fed by reading `SWB1`/`SWB2` elsewhere, not the same register directly, but that intermediate step hasn't been found. **New context found the same day**: a heuristic-only function (`FUNC_3633_92D1`, `0xE92D1` - not confirmed reachable, no call site to it found yet) initializes a whole cluster of neighboring bytes together - `[0x4E0]`, `[0x4E7]`, `[0x4E8]`, `[0x4EB]`, `[0x4EC]`, `[0x4F0]`, `[0x4F4]`, `[0x4F7]`, `[0x4F8]`, `[0x4FB]`, `[0x4FC]` - to different specific default patterns depending on which branch of the `[0x1B83]==0x14` comm-detection dispatch is taken, confirming these ~11 bytes really are one related group (not coincidentally adjacent), and tying their defaults to the same still-unresolved `[0x1B83]` hardware-variant question. See `disasm/NOTES.md` "The `[0x4E0]`-`[0x4FC]` cluster" for the full byte-by-byte breakdown. Still open. |

## Front-panel A/D converter (analog controls)

The service manual's `FP_VALUES` exerciser (Table 6-16, "Display
Format") shows the front-panel ADC's raw digitized readings under the
`AD DATA` column, with 2 bytes listed for each channel row: `E114`/
`E115` for `CH 1`, `E164`/`E165` for `CH 2` (`E`-prefixed designators -
likely schematic test points feeding an analog multiplexer ahead of
the front-panel ADC, `U6105`, per `MEMORY_MAP.md`'s "Two separate ADCs"
note).

**Confirmed 2026-09-13 by direct hardware manipulation** (the user ran
`FP_VALUES` and moved individual front-panel controls while watching
the displayed hex change): turning the **CH1 POSITION** knob changes
exactly the *first* of `CH 1`'s two `AD DATA` bytes (`E114`); turning
**CH2 POSITION** changes the *first* of `CH 2`'s two bytes (`E164`).
This is a live, empirical confirmation, not inference from code or
documentation alone - the strongest kind of evidence this project has
for a front-panel control mapping.

**Confirmed 2026-09-13, second finding**: the *second* byte of each
pair (`E115`/`E165`) is **`VOLTS/DIV`** - turning **CH1 VOLTS/DIV**
changes the second byte of `CH 1`'s `AD DATA` pair, and the user
confirmed **CH2 VOLTS/DIV** does the same for `CH 2`'s pair. This
directly confirms the `selftest_front_panel_switch_a`/`_b` hypothesis
(both confirmed to verify their swept control via ADC readback -
`VOLTS/DIV` for CH1/CH2 was the leading guess for those two self-tests
specifically; see `disasm/NOTES.md`'s "Also found" paragraph) by live
manipulation rather than code inference alone.

Also reported the same session, from the CH1 **VAR/CAL** switch:
toggling out of calibrated detent changes the third nibble of a third
octet (position within the exerciser's full display line not yet
pinned down precisely - the `AD DATA` field only lists 2 bytes per
channel in Table 6-16, so this third octet may belong to a different
column, e.g. `ISTAT`). Raw finding recorded here for future
correlation; not yet mapped to a specific bit/register.

**Digital switch-bank findings from the same live session** (most
likely reflected in `SWB1`/`SWB2` - see `[0x759]` above for the
`HORIZONTAL MODE` A/BOTH/B confirmation, the cleanest of this batch):
- `VERT MODE` (CH1/BOTH/CH2 selector) and toggling `XY` mode both
  changed bytes in the exerciser's `dig=` digital-switch field (e.g.
  `XY` toggled one byte between `0x73`/`0x63`); exact bit(s) not yet
  isolated - `SWB1`/`SWB2`'s currently-documented bit maps don't have
  an obvious single-bit match for these, so either another byte is
  involved or one of the "not yet independently bit-validated" bits in
  `[0x759]` corresponds to these. Needs a slower, one-control-at-a-time
  re-test to isolate cleanly.
- Input coupling switch (`AC`/`GND`/`DC`) changed a third byte's value
  (reported as `0x12`-ish/`0x2AA`-ish/`0x3xE`-ish across the three
  positions - transcription uncertain, values not confidently hex-clean
  enough to bit-map yet). Needs a repeat test with the exact displayed
  hex read back carefully.

## Acquisition/plot scaling

Found via `draw_pending_line_segment`'s (`0xF0C2A`) own previously-
unreached preamble (`0xF09C0`-`0xF0C29`), only made reachable this
session by decoding `init_far_pointer_table_sysrom`'s embedded RAM-init
table and adding 15 new `ENTRY_POINTS` - see `disasm/NOTES.md` "Found:
a whole family of never-reached functions via the RAM far-pointer init
table" and "Systematic landing-artifact sweep".

| Address | Role | Confidence |
|---|---|---|
| `[0x712]`, `[0x714]` | X-axis scale inputs - `[0x712]&0x3FF` used as the `imul` multiplicand against `[0x716]`, result `idiv`'d by `[0x71A]`, feeding `[0x6BC]` (plot X position) | Usage confirmed; physical meaning (a sample index? a time value?) not confirmed |
| `[0x716]`, `[0x71A]` | Paired with `[0x712]`/`[0x714]` in the same `imul`/`idiv` scale computation - `[0x71A]` set from a byte read via far ptr `[0x1DB4]` in `SUB_F09C6` | Usage confirmed; not confirmed |
| `[0x718]`, `[0x71C]` | Y-axis counterparts of `[0x712]`/`[0x716]` - `[0x718]` compared against `[0x71C]` to pick which of 2 `imul`/`idiv` branches computes `[0x6BE]` (plot Y position) | Usage confirmed; not confirmed |
| `[0x71E]` | Read and `shl`'d by 1 in `SUB_F09C6`, indexes far ptr `[0x1DB4]` to set `[0x716]` | Usage confirmed; not confirmed |
| `[0x722]`, `[0x724]` | Sign-extended (bit `0x400` checked, `or 0xFC00` if set - classic 10-bit-to-16-bit sign extension) from 2 words read via far ptr `[0x1DB4]`+offsets `3`/`7`; added into the X/Y scale results above | Usage confirmed (sign-extension pattern is a strong, well-understood x86 idiom); source data's real-world meaning not confirmed |
| `[0x1DB4]` | A far pointer read repeatedly through this whole preamble as a small lookup table (byte reads at `+0`/`+1`, word reads at `+3`/`+7`) - feeds the scale-factor variables above. **Not related to the stroke-font pointer `[0x1DB0]`** (see below) despite being nearby | Usage confirmed; what table it points to and who sets it not confirmed (same open question as `[0x1DB0]`/`[0x1DB8]`/`[0x1DBC]` - nothing writes any of these 4 pointers in proven or heuristic code) |
| `[0x1DB8]`, `[0x1DBC]` | A pair of small per-record **byte-value caches** (NOT font/glyph data - ruled out this session, see `TODO.md`'s stroke-font item): indexed by `[bp-0x10]` (a channel/record index), storing/comparing an 8-bit value derived by shifting the current plot Y position `[0x6BE]` right 6 bits (`>>6`), used to clamp `[0x6BE]` against a previous cached value before `draw_pending_line_segment` actually plots - almost certainly 2 more instances of the already-documented `reset_all_channel_plot_caches` per-channel plot-scale-cache family | Mechanism confirmed via full byte trace; exact real-world unit of the cached value (a coarse Y bucket for redraw-skip decisions is the leading guess) not confirmed |
| `[0x1DB0]` | A far pointer to a **128-entry table of far pointers** (indexed `char_code*4`, masked to 7 bits), read by `draw_readout_char` (`0xE3854`) - each entry is itself a far pointer, presumably to that character's stroke-vector glyph data. This is the best remaining candidate for the long-sought stroke-font glyph table (see `TODO.md`) - structurally distinct from `[0x1DB4]`/`[0x1DB8]`/`[0x1DBC]`'s simple byte-array shape, so it is NOT just a 4th instance of the same small-cache family | Usage/structure confirmed; the actual stored far-pointer value (where the table itself lives in ROM) still not found - nothing writes `[0x1DB0]` in proven or heuristic code |

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
