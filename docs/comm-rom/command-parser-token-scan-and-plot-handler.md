# Comm ROM: the command-line token scanner, and a candidate (unconfirmed) keyword-table walker

Found 2026-10-09 while working the "rename every identifiable routine"
`TODO.md` item - started from three previously-undocumented unnamed
proven functions (`SUB_850A2`, `FUNC_2998_E7895`, `FUNC_2998_EA2D6`;
only the first was investigated this session) and ended up tracing a
cluster of functions physically interleaved between `comm_call_main_
rom`'s label and `process_gpib_command_byte`'s label in `disasm/160-
2998-14.lst`, each independently `push bp`/`retf`-bounded (i.e.
separate functions, not part of `comm_call_main_rom`'s own straight-
line body).

## What's confirmed

`comm_call_main_rom`'s own body (physical `0x839F5`-`~0x844xx`) is a
much more substantial command-line **lexer/parser** than its existing
`FUNCTIONS.md` entry suggests - not just a thin dispatcher around the
`[0x72A]` state-jump table, but the actual byte-by-byte token-
recognition logic for the RS-232/GPIB command grammar. Evidence:

- `comm_call_main_rom` directly (`lcall`, confirmed call sites, not
  just the `[0x72A]` table) calls several of the previously-unnamed
  sibling functions in the `0x8509x`-`0x8519x` range:
  - `FUNC_2998_50C2` (`0x850C2`), twice (`0x84363`, `0x84449E`... see
    `160-2998-14.lst` lines 6665, 6775)
  - `FUNC_2998_50E4` (`0x850E4`), twice (lines 6272, 6308)
  - `FUNC_2998_5115` (`0x85115`), once (line 5996, physical `0x83C70`)
  - `FUNC_2998_519A` (`0x8519A`), once (line 6727, physical `0x8440C`)
- Right after the call to `FUNC_2998_5115` (line 5996), the code
  checks the current input byte (`[4]`) against `?` (`0x3F`, a query
  suffix), `;` (`0x3B`, a command separator), and space/comma/CR/LF
  (terminators) - the textbook shape of recognizing a command keyword
  and then checking what follows it. This is strong direct evidence
  `[4]` is "the current input character" and `FUNC_2998_5115` is
  invoked right at the point a keyword needs to be matched.
- `reset_comm_token_length` (`0x850A2`, renamed this session - see
  `FUNCTIONS.md`) and a byte-identical sibling `FUNC_2998_50B2`
  (`0x850B2`, left unnamed - see below) both zero `word ptr [0x5F4]`.
  `reset_comm_token_length` is called directly from `reinit_comm_
  channel`'s (`0x8FB01`, renamed this session) `[0x5A4]==0x16` branch.
- `FUNC_2998_50C2`/`FUNC_2998_50E4` both bounds-check-and-increment
  `[0x5F4]` (against `0x64`=100 and `0x47`=71 respectively), setting
  error code `[0x623]=0xB` and resetting the parser to idle
  (`[0x5A4]=1`) if exceeded. `FUNC_2998_50E4` additionally writes the
  current input byte through far pointer `[0x5F0]` and increments
  that pointer too - a genuine character-into-buffer accumulator.
  `FUNC_2998_50C2` does **not** write a buffer, only counts - so
  these two most likely serve two different token *kinds* (e.g. a
  numeric literal's digit count vs. a keyword/string's character
  accumulation), not confirmed which is which.

See `VARIABLES.md`'s new `[0x5A4]`/`[0x5F4]` entries for the per-
variable summary.

## A candidate (NOT confirmed) explanation for `command-keyword-table.md`'s open question

`docs/comm-rom/command-keyword-table.md`'s "Open questions" section
ends with: *"The code that actually walks either table has still not
been found in the disassembly."* `FUNC_2998_5115`/`FUNC_2998_519A`
are a strong candidate for (part of) that code, but this is **not
confirmed** - see the blocking problem below.

Both functions:

1. Call `SUB_96CE9` then `FUNC_2998_2FD7`.
2. Validate the current input byte (`[4]`) is `0x41`-`0x5A` (`A`-`Z`);
   if not, set a context-specific error code (`[0x623]=2` in `5115`,
   `=4` in `519A`) and return.
3. Compute `index = ([4] - 0x41) * 6` - a 26-letter x 6-byte-record
   lookup. This **exactly matches** this project's already-confirmed
   "6-byte record" shape for the comm ROM's dispatch/index table
   (table 2 in `command-keyword-table.md`: `[id_byte][0xFF marker]
   [2-byte offset][2-byte segment]`), though `5115`/`519A`'s table is
   indexed by **first letter**, not by numeric command ID like table
   2 is - so if related, it's a *different*, letter-bucketed table,
   not table 2 itself directly.
4. `FUNC_2998_5115` indexes via far-pointer table base `[0x6EA]`;
   `FUNC_2998_519A` via a **different** base, `[0x6F6]` - i.e. two
   separate 26-entry letter-indexed tables, plausibly one for top-
   level command keywords and one for argument/value keywords (the
   two string pools `command-keyword-table.md` already documents at
   comm-ROM file offsets `0x8A58`-`0x8F1E`).
5. Reads a far pointer from the indexed 6-byte record into a scratch
   area, calls `SUB_96CE9` again then `FUNC_2998_3048` (hypothesized
   string/keyword compare), and checks a match-success flag
   (`[0x607]`): on success, copies a resolved value byte (`[0x608]`)
   into `[0x604]`; on failure, sets an error code.

`[0x604]` **is confirmed** to be the resolved top-level command's
numeric ID: it's set from `FUNC_2998_5115`'s lookup (line 8030), and
later compared against `0x17` (line 6807, inside `FUNC_2998_44D0`) -
**`0x17` is `command-keyword-table.md`'s already-documented, hardware-
confirmed ID for `PLOt`**. This is solid, independent confirmation
that `FUNC_2998_5115` really is matching top-level command keywords
and resolving them to the same numeric IDs already extracted from the
ROM's string tables.

## The blocking problem, resolved 2026-10-09: `[0x6EA]`/`[0x6F6]` are plot-subsystem addresses, not a comm-ROM keyword-table base

Re-checked by searching every proven *and* heuristic listing for both
ROMs for any reference (read or write) to `[0x6EA]`, `[0x6EC]`,
`[0x6F6]`, `[0x6F8]` - not just proven code as before. **No write to
either pair exists anywhere in the comm ROM** (`160-2998-14.lst`,
which already includes its own heuristic layer per `gen_disasm_2998.
py` - only the 4 `les` *reads* cited above). Both pairs' only write
sites are in the **main ROM's heuristic-only layer**, and both are
unambiguously plot/HPGL-subsystem code, not comm-ROM initialization:

- `[0x6EA]`/`[0x6EC]`: written by `FUNC_3633_7EBF` (physical `0xE7EBF`,
  heuristic-only, sole confirmed caller `0xF140E` in `160-3532`) as a
  **literal constant far pointer, `0x8000:0x4000`** (physical
  `0x84000`), immediately followed by a loop zeroing up to `0x800`
  words (4096 bytes) through that same pointer - a fixed-address
  buffer-clear, not a table-base assignment computed from any comm-ROM
  state. The call site and the whole surrounding block gate on
  `[0x6CA]` - the already-documented **HPGL pen-up/pen-down state**
  variable (`MEMORY_MAP.md` line 1035) - and sit among calls to
  `FUNC_3633_7C27`/`FUNC_3633_7AA7`/`SUB_E8179`/`SUB_E8363`, all in the
  same plot-handling neighborhood. This is plotter/HPGL code clearing
  a fixed-location RAM buffer, unrelated to command parsing.
- `[0x6F6]`/`[0x6F8]`: as already noted, written by `reset_all_
  channel_plot_caches` alongside other acquisition/plot-channel cache
  pointers, with the paired "invalidate" sentinel `0x0800` - also
  unambiguously plot/acquisition code.

This confirms **possibility 2** from the original two candidates:
`[0x6EA]`/`[0x6F6]` are flat-RAM offsets reused by entirely unrelated
subsystems (HPGL plot-buffer management, not comm-ROM keyword
lookup) - the same already-documented reuse pattern as `[0x712]`
(`VARIABLES.md`'s "Acquisition/plot scaling" section). Worse than
"unconfirmed": since plotting and comm-command processing are both
foreground-loop activities that can interleave, a real keyword-table
base pointer sharing these exact addresses would get clobbered by
ordinary plot activity - strong evidence `FUNC_2998_5115`/
`FUNC_2998_519A`'s letter-indexed-table-walker hypothesis, while still
plausible in outline (the `[0x604]`/`0x17`=`PLOt` match is solid), is
**reading from the wrong base pointers**, or is simply wrong about
what `[0x6EA]`/`[0x6F6]` mean. Their *real* table base (if the
hypothesis survives at all) is still unknown and would need a fresh
candidate - not a reason to keep treating `[0x6EA]`/`[0x6F6]`
themselves as live leads.

## `FUNC_2998_44D0`: a generic argument-type validator, NOT a PLOt-specific handler

Initially mis-identified (and briefly, incorrectly, named
`validate_plot_command_args` before being caught and reverted - see
`changes/2026-10-09.md`). Full read of its body (`160-2998-14.lst`
lines 6793-7064) shows:

- It's called for **multiple** commands' argument lists, not just
  `PLOt` - the `cmp byte ptr [0x604], 0x17` check (line 6807) is one
  internal special case among its logic, not how the function itself
  gets reached.
- **No direct call site to `FUNC_2998_44D0` was found anywhere** in
  either ROM's disassembly (same for `reinit_comm_channel`,
  `0x8FB01`) - both are `kind="entry"` in the `.symbols.json`, meaning
  they're reached by some mechanism the current reachability trace
  doesn't model (most likely an indirect/computed call, analogous to
  `comm_call_main_rom`'s own `[0x72A]` table).
- Walks a parsed argument list via 12-byte records off a pointer table
  at `[0x702]`, validating each argument's type through calls to
  `FUNC_2998_477F`/`FUNC_2998_4835`/`FUNC_2998_4BE6`/`FUNC_2998_49EC`
  (none analyzed in depth this session).
- Confirmed by full-body read: it **never writes any PLOt-effect
  variable** (in particular, never touches `[0x461]`, the still-
  missing `FORmat`/output-port override byte - see `STILL_PENDING_
  DECODE.md` and `TODO.md`). It only sets an error code (`[0x623]`)
  on a syntax mismatch and always resets the parser to idle
  (`[0x5A4]=1`) on exit, success or failure alike.

Left unnamed (`FUNC_2998_44D0`) pending a correct, non-PLOt-specific
characterization of what it actually validates.

## Still open

- `FUNC_2998_5115`/`FUNC_2998_519A`'s *real* letter-indexed table base
  (now that `[0x6EA]`/`[0x6F6]` are confirmed address-reuse red
  herrings, not live leads - see above) - unknown, no fresh candidate
  found yet.
- `FUNC_2998_50B2` (byte-identical to `reset_comm_token_length` but no
  confirmed caller found), `FUNC_2998_50C2`, `FUNC_2998_50E4`,
  `FUNC_2998_5115`, `FUNC_2998_519A`, `SUB_96CE9`, `FUNC_2998_2FD7`,
  `FUNC_2998_3048`, `FUNC_2998_477F`, `FUNC_2998_4835`,
  `FUNC_2998_4BE6`, `FUNC_2998_49EC` - all plausibly characterized
  above but not confidently enough to rename per this project's "don't
  guess real hardware content" standard.
- The printer/plotter `FORmat` override write site (`[0x461]`) is
  **still not found** - this session's deep dive reached the actual
  `PLOt`-argument-validation code (`FUNC_2998_44D0`) and confirmed it
  isn't the site, narrowing the search further into the comm ROM's
  still-undisassembled tail, but didn't resolve it.
- **Correction**: the other two never-before-documented unnamed proven
  functions originally flagged alongside `SUB_850A2` were mislabeled
  above as comm-ROM functions (`FUNC_2998_...`) - they're actually
  **main-ROM** (`160-3633`) addresses in `sysrom_3532_3633.symbols.
  json`: `SUB_E7895` and `SUB_EA2D6`, physical `0xE7895`/`0xEA2D6`.
  **Both were investigated and resolved later the same session**:
  `SUB_E7895` is `umod32_core`, the unsigned-remainder sibling of
  `udiv32_core`'s shared restoring-division loop (see `FUNCTIONS.md`'s
  `smod32`/`umod32`/`umod32_core` entries and `changes/2026-10-09.md`
  for the full trace). `SUB_EA2D6` turned out to be a disassembly
  anomaly, not real code (it and its call target `SUB_EA13B` land
  inside known string-table text) - correctly left unnamed, see
  `docs/decode-anomalies/unknown-data-deep-dive-2026-09-15.md`'s
  2026-10-09 section. Both unrelated to this doc's comm-ROM command-
  parser cluster, just flagged from the same starting list.
