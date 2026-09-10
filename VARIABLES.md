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
| `[0x1B83]` | Config byte `SUB_E44F1` checks equals `0x1E` as part of confirming the RAM/IO result | Usage confirmed; meaning of the specific value `0x1E` not confirmed |

## Menu navigation

| Address | Role | Confidence |
|---|---|---|
| `[0x1B50]` | Current menu/parameter cursor position, updated and clamped between caller-given min/max by `update_menu_position` (`0xE06B6`) | Confirmed |
| `[0x1B51]` | A flag `update_menu_position` sets/checks alongside `[0x1B50]` - looks like a "just wrapped" or "pending redraw" indicator for the cursor | Usage confirmed; exact meaning not confirmed |
| `[0x4E7]`, `[0x4E8]` | Front-panel button/control state bytes - bit 7 of each checked by `update_menu_position` for a simultaneous-both-held "accelerate" case | Usage confirmed; which physical controls these represent not confirmed |

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

- `[0x758]` — checked (masked `&0x63`) around calls to
  `read_channel1_status`/`read_channel2_status`; role beyond "some
  status/mode byte" not yet confirmed.
  `[0x780]` — referenced early in boot-adjacent code, role unknown.
- `[0x61A]`, `[0x61B]` — referenced in `SUB_E004F` (one of the first
  functions in the main ROM), a small getter/setter-looking pair.
