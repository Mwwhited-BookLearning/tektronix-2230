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
| `[0x758]`, `[0x759]` | Hardware status bytes, read every tick from fixed physical addresses `0x403FFA`/`0x403FFB` by `scheduler_tick_service`. `[0x758]` also checked (masked `&0x63`) around `read_channel1_status`/`read_channel2_status` calls | Source confirmed; which peripheral (front-panel key/encoder is the leading candidate) not confirmed |
| `[0x7B4]` | Previous snapshot of `[0x758]`, XOR'd against the new read each tick for edge/change detection | Confirmed |

## Menu navigation

| Address | Role | Confidence |
|---|---|---|
| `[0x1B50]` | Current menu/parameter cursor position, updated and clamped between caller-given min/max by `update_menu_position` (`0xE06B6`) | Confirmed |
| `[0x1B51]` | A flag `update_menu_position` sets/checks alongside `[0x1B50]` - looks like a "just wrapped" or "pending redraw" indicator for the cursor | Usage confirmed; exact meaning not confirmed |
| `[0x4E7]`, `[0x4E8]` | Front-panel button/control state bytes - bit 7 of each checked by `update_menu_position` for a simultaneous-both-held "accelerate" case | Usage confirmed; which physical controls these represent not confirmed |

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
