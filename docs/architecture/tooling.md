# Disassembly tooling

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Tooling

- `gen_disasm_x86.py` — recursive-descent x86-16 disassembler, seeded
  from three known entry points (reset vector, plus two far-jump targets
  discovered while tracing the reset path: `E5D1:B4`/`E5D1:C7`, an
  interrupt-vector-table setup + memory-clear routine). Only follows
  *resolved* control flow (direct near/far jmp, jcc, call/lcall);
  indirect (register/memory) jumps stop that walk rather than guessing.
  Outputs:
  - `sysrom_3532_3633.lst` — the disassembly listing.
  - `sysrom_3532_3633.symbols.json` — address → placeholder name, with
    empty `functional_name`/`notes` fields to fill in as we understand
    what each routine/label does. This is the file to edit when renaming
    things by intent instead of address.
- `gen_disasm.py` — the earlier (obsolete) naive-linear-sweep 6809
  attempt. Kept for history; not useful now that we know the real CPU.
  Safe to ignore/delete once the x86 tooling supersedes it.
- `validate_nasm.py` — round-trip validator. Converts every decoded
  instruction to NASM syntax, assembles each in its own `section ...
  vstart=<real address>` (packed sequentially in the output so one
  instruction's encoded length can't shift/cascade into the next one's
  comparison), and diffs against the original ROM bytes. Requires a
  NASM executable path as its one argument (NASM isn't vendored in the
  repo - see "Validation status" below for how to get one).
- `gen_source.py` — emits the actual committed disassembly:
  `160-3633-14.asm` and `160-3532-14.asm`, one real buildable NASM
  source file per chip. Decoded instructions proven byte-exact by
  `validate_nasm.py` are written as real instructions; everything else
  (alt-encoding-only matches, unconverted instructions, and every
  byte the recursive descent hasn't reached yet) is emitted as raw
  `db` bytes grouped 16-per-line, with the original mnemonic in a
  comment where known. This means **the .asm files always reassemble
  byte-identical to the original .bin** regardless of how much is
  understood yet - `gen_source.py` verifies this itself on every run
  (assembles its own output and diffs against the source .bin). ASCII
  string runs get a comment showing the decoded text. Also takes a
  NASM executable path as its one argument. These .asm files are the
  ones to open when reading or editing "the disassembly" - `.lst` is
  a secondary diagnostic view with extra formatting `.asm` doesn't
  have (physical addresses, chip:offset pairs).
## New tool: `analyze_loops_vs_functions.py` - separating local control flow from shared code

Built in response to a direct question: of the many `L_XXXXX` branch
targets in the disassembly, which are genuinely local loop/goto
constructs (a `for`/`while`/`do-while` or an `if`/`switch`/early-exit
inside one function) versus branch targets that are actually reached
from a *different* function's address range - i.e. code that's really
shared/independent but only ever got a bare `L_` label because nothing
reaches it via `CALL`/`LCALL`.

**Method**: walk the `.lst` top to bottom, tracking "current function"
as the most recently seen `sub`/`entry`-kind label (functions occupy
contiguous address ranges in this address-sorted listing, so this is a
reasonable proxy for lexical scope). For every branch/jump instruction,
resolve its target's physical address and record `(source, source's
owning function)`. For every `loc`-kind (bare `L_`) target with
incoming branches, classify:
- `loop` - only backward edges (source addr > target addr), all from
  its own owning function - classic `for`/`while`/`do-while`.
- `goto` - only forward edges, all local - `if`/`switch`/early exit.
- `mixed` - both, still local.
- `cross-fn` - reached by a `jmp`/`Jcc` from a **different** function's
  range than the one that lexically owns the target address.

Also computes raw jump **distance** (`|target - source|`) independent
of the owning-function heuristic, per the user's suggestion that a
large displacement is itself a strong, assumption-free signal that a
target isn't ordinary local control flow.

**Results** (proven set, 13,510 instructions, 1,032 classified `loc`
targets): 127 `loop`, 885 `goto`, 5 `mixed`, **15 `cross-fn`**. Of
those 15, most match the "un-prologued entry" cluster already
documented above (`SUB_EF346`/`SUB_EF393`, `SUB_EFB64`/`SUB_EFBA5`,
`SUB_F156E`/`SUB_F1581`, `SUB_F4150`/`SUB_F5184`, `SUB_F6382`/
`SUB_F635E`) plus the big multi-entry plot/acq region around
`draw_pending_line_segment`/`reset_plot_home_or_acq`/
`reset_all_channel_plot_caches` - good independent confirmation that
those groupings are real, not cherry-picked.

**The distance lens found something the cross-fn lens alone wouldn't
have prioritized as clearly**: `L_EDA0A` is reached by a `jmp` from
`SUB_F1581` at a displacement of **0x3B7A (15,226 bytes)** - by far the
largest in the whole proven set (the next-largest is `L_F08E4` at
0x3A8=936, and everything past that is well under 1,000, matching
ordinary big-function loop/if spans). Follow-up (see the "A third
instance" paragraph in the `SUB_E90A5`/`SUB_E92B0` section above):
**resolved, not a real function** - `L_EDA0A` is `SUB_F1581` landing 2
bytes into a `mov` instruction belonging to `compute_and_draw_scale_
marker` (renamed on the strength of the other ~90% of its body), the
same "landing mid-instruction, clean nearby reconvergence" class as
`SUB_E90A5`/`SUB_E92B0`. The huge displacement was a real, useful
signal (it correctly flagged this as worth a close look over anything
else in the list) - it just pointed at a decode artifact rather than
an undiscovered function this time.

**Two `cross-fn` hits turned out to be heuristic false positives when
manually verified** (worth knowing before trusting this tool's list
blindly): `L_EE139` (flagged as "owned by `SUB_EDFFD`, crossed from
`write_hw_shift_register`") is really just `write_hw_shift_register`'s
own loop-top, 2 bytes before where the recursive descent happened to
plant that function's `SUB_` label - the "owning function" bisection
mis-assigns anything in that 2-byte gap to the *previous* function
purely because there's no label exactly at the boundary. Likewise
`L_F0E50` (flagged as "owned by `start_plot_output_task`, crossed from
`convert_sample_value`") is really `convert_sample_value` looping back
on itself across an address gap (documented in `FUNCTIONS.md`'s
`convert_sample_value` entry) - `start_plot_output_task` has nothing
to do with it, it's just the nearest preceding label. **Small
distances in the `cross-fn` list need manual confirmation**; large
ones (`L_EDA0A`, and to a lesser extent `L_F08E4`/`L_F0678`/`L_F0A70`)
are much more trustworthy since a real address gap or genuinely
distant tail-jump is much less likely to be a boundary artifact.

**One genuine new find from manually checking the list**: `SUB_F750A`
(`jmp`s backward into `update_display_mode_flags`'s `L_F7504` scan
loop, sharing its `[bp-8]` local) turned out to be a real secondary
entry point into that function - confirmed and renamed to
`sync_shift_register_output` (see `FUNCTIONS.md`). This also surfaced
that `update_display_mode_flags` is the one that *sets* the
`[0x1BEC]` scale-clamp bound (`0x400`/`0x1000`) that the `SUB_ED9BC`/
`L_EDA0A` region reads - connecting two previously-separate
investigation threads.
