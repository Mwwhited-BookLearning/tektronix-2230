# Comm ROM: option-board presence detection

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Found: the option-board presence/RAM-detection routine

Tracing why the `0xAA55` pattern gets tested (originally read as a
generic RAM-size probe) led to a much more specific and useful answer,
prompted by two user questions: "might this be a check for whether the
comm option card is installed?" and "what if it's RAM or memory-mapped
I/O on the comm board?" **Both are confirmed correct, and it's one
function doing both:**

`check_comm_option_installed` (`160-3633`) hardcodes checking `ES:DI = 0x8000:0x0004` -
physical `0x80004`, which is exactly bytes 4-5 of the comm ROM's own
10-byte self-ID header (the BCD-revision byte + its one's-complement
byte, from the header format decoded on day one of this project). The
function:
1. Reads that word, splits it into high/low bytes, and checks whether
   they sum to exactly `0xFF` - i.e., re-derives the same
   value+complement checksum every one of these ROMs' headers uses.
   This is a **"is a genuine Tektronix ROM header present here"**
   check, not a generic memory probe.
2. Only if that passes does it run the save/write-`0xAA55`/verify/
   restore sequence - **not against the same address**, against
   `ES:[DI+0xA000]` (`DI` is unchanged from the checksum read, still
   `4`), i.e. physical `0x8A004` - a *different* word 0xA000 bytes
   further into the same 64KB comm-board window. Testing whether
   *that* location is writable, i.e., whether the option board
   provides RAM/memory-mapped I/O somewhere in its address space in
   addition to its ROM. Why that specific offset was chosen isn't
   confirmed - one plausible explanation is a small dedicated
   presence-detect scratch register on the option board, decoded by
   the board's own sub-decode logic to occupy just that one word
   while everything else in the window reads as ROM (this would also
   explain why our raw EPROM dump shows ordinary code/data there: a
   dump reads the physical EPROM chip directly, not "as seen by the
   CPU with the board installed and its sub-decode active").
3. Packs the result into a status byte (`[0x1BF9]`): one bit for "valid
   header found," a second bit (only set if the RAM-write also stuck
   *and* another config byte at `[0x1B83]` reads `0x1E`) for "and it
   has working RAM/IO too."

So the comm/GPIB option board's presence detection and its RAM/IO
presence detection are the same probe, just gated in sequence - exactly
matching the "Function requires options not installed in this
instrument" message string found in `160-3532` back at the very start
of this project.

## Found: the actual source of [0x1B83] (detect_comm_option_hw)

`[0x1B83]` has been checked (`==0x1E`) by `check_comm_option_installed`
and `self_test_dispatcher` since early sessions, but where it got its
value was never traced - until `detect_comm_option_hw` (`0xE75C0`)
turned up, called from `run_selftest_sequence`'s setup. It's a
**second, independent hardware probe** for the comm option board,
separate from `check_comm_option_installed`'s ROM-header-checksum
check:

1. Writes `0` to physical `0x40000+0x7DE`.
2. Reads bit `0x1000` of a word at physical `0x40000+0x377E`. If
   already set, `[0x1B83] = 0x1E` and done.
3. Otherwise, writes `1` to `0x40000+0x7DE` (a write-then-readback
   probe - the same idea as `check_comm_option_installed`'s `0xAA55`
   test, just at different addresses) and re-reads `0x40000+0x377E`'s
   bit `0x1000`. If it's now set, `[0x1B83] = 0x14`. If still clear,
   `[0x1B83] = 0x1E`.

**Not fully resolved**: which value specifically means "installed".
`0x1E` results from *two* branches with opposite-looking conditions -
bit already set (no write needed) **or** bit still clear even after
the write probe - while `0x14` is the one case where the write
visibly changed the read-back bit. This doesn't fit a simple binary
"present/absent" flag cleanly. Plausible readings: `0x1E` might be a
safe default used whenever the probe *doesn't* need the write-trigger
to succeed (either because the board was already responding, or
because nothing is there to respond even after trying), while `0x14`
marks a specific "board present, but only detected via the write
probe" case - possibly distinguishing a hardware revision or a board
that needs an explicit initialization poke before it reports itself.
Worth revisiting once more of what reads `[0x1B83]` (beyond just
`==0x1E`) is understood - if some code also checks `==0x14`
specifically, that would clarify the distinction.

**Update 2026-09-13, from the real service manual**: the write-probe
address (`0x40000+0x7DE` = `0x407DE`) is confirmed to be the **"Time
Base Mode Register U4119"** (service manual Table 3-1) - a general
timebase-mode register, not a comm-specific latch. This means `detect_
comm_option_hw`'s probe works by toggling a bit in the general Time
Base Mode Register and checking whether a *different* nearby register
reflects that change - consistent with the comm option's presence
being wired to gate or reflect onto part of this register's behavior,
rather than having its own dedicated "are you there" bit.

**Resolved 2026-09-13, from the manual's full Table 3-1 page image**:
the readback address (`0x40000+0x377E` = `0x4377E`) is an **exact
match** for **"Acquisition Memory Address Buffer Low bits U3427"** -
the earlier "closest is `0x437BE`, `0x40` away, not confirmed" note was
comparing against the wrong neighboring row in a garbled OCR pass of
this table; the real page image resolves it cleanly. So `detect_comm_
option_hw`'s probe toggles a bit in the general Time Base Mode
Register (`U4119`) and checks whether it's reflected in the
acquisition address buffer (`U3427`) - two ordinary, non-comm-specific
registers, not dedicated comm-detection hardware. Still doesn't
resolve the `0x1E` vs `0x14` meaning question above, but the
*mechanism* (which physical registers are involved) is now fully
grounded rather than partially inferred.

## The `[0x4E0]`-`[0x4FC]` cluster - a whole group of front-panel-adjacent bytes gated on `[0x1B83]`

Tracing `update_menu_position`'s `[0x4E7]`/`[0x4E8]` bit tests (looking
for a clean cross-reference to the service manual's `SWB1`/`SWB2`
signal names - see `HARDWARE.md`) led to a heuristic-only function,
`FUNC_3633_92D1` (`0xE92D1` - **no call site to it found yet, proven or
heuristic; reachability not confirmed**), that initializes a whole
cluster of neighboring bytes together, gated on the same `[0x1B83]==
0x14` dispatch already documented above:

- **`[0x1B83]==0x14` branch**: `[0x4E7]=0x10`, `[0x4EB]=0xBF`,
  `[0x1B72]=1`, `[0x4F8]=0xFF`, `[0x4FC]=0xFF`, `[0x4F4]=0xFF`,
  `[0x4F0]=0xFF`, `[0x4FB] |= 0x10`, plus two flag fields (`+0x46`,
  `+0x4C`) of index-0's record in the `[0x1D1C]` per-item table (the
  same table `update_indexed_value_if_changed`/`SUB_E8E29` use) are set
  to `1`, then a 4-iteration loop writes into two *other* far-pointer
  tables (`[bp+6]`'s far ptr at `+0x574`/`+0x576`, stride `0xE`) and
  sets `[0x4E0]=1` at the end.
- **`[0x1B83]!=0x14` branch** (`L_E937B`): `[0x4F7]=0`, `[0x4FB]=0xFF`,
  `[0x4E8]=0xFF`, `[0x4EC]=0xFF`, `[0x4F4]=0xFF`, `[0x4F0]=0xFF`, then a
  5-iteration loop (stride `6`, same `[0x1D1C]` table base) continues
  (not fully traced).

**What this confirms**: `[0x4E0]`, `[0x4E7]`, `[0x4E8]`, `[0x4EB]`,
`[0x4EC]`, `[0x4F0]`, `[0x4F4]`, `[0x4F7]`, `[0x4F8]`, `[0x4FB]`,
`[0x4FC]` really are one related group of variables (not just
coincidentally adjacent addresses) - both branches touch nearly the
same set, just with different specific values, confirming a genuine
per-hardware-configuration default-initialization routine. **Not
confirmed**: whether this function ever actually executes (no caller
found), and what physical significance the specific bit patterns
(`0x10`, `0xBF`, `0xFF`, `|=0x10`) have. Left as an open, documented
thread rather than guessed at - see `VARIABLES.md`'s `[0x4E7]`/`[0x4E8]`
entry for the cross-reference.
