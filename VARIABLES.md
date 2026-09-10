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
| `[0x1B10]` | Gates whether the self-test dispatcher (`SUB_E416F`) runs at all (`cmp word [0x1B10],0 / jne skip`) | Usage confirmed; exact meaning (mode flag? "self-test already ran this power-cycle"?) not confirmed |
| `[0x1B18]` | Written literal `1` after nearly every individual self-test call inside `SUB_E416F` | Usage confirmed; meaning (progress marker? always-1 flag rather than an index) guessed |
| `[bp-0xA]` (local, not fixed) | Accumulates OR'd return codes from each self-test call into an overall result word, inside `SUB_E416F`'s caller | Confirmed |
| `[0x1BF9]` | Option-board presence/RAM status byte, set by `SUB_E44F1`: bit 1 = valid ROM header checksum found at the probed address, bit 2 = that address is also RAM/IO-backed | Confirmed |
| `[0x1B83]` | Config byte `SUB_E44F1` checks equals `0x1E` as part of confirming the RAM/IO result | Usage confirmed; meaning of the specific value `0x1E` not confirmed |

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

- `[0x758]`, `[0x780]`, `[0x1B50]` — referenced early in boot-adjacent
  code; `SUB_E06B6` (see `disasm/sysrom_3532_3633.lst`) is a small
  dispatch/switch function keyed on a low nibble that touches these.
- `[0x61A]`, `[0x61B]` — referenced in `SUB_E004F` (one of the first
  functions in the main ROM), a small getter/setter-looking pair.
