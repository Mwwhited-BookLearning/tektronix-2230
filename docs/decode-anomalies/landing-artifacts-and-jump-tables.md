# Decode anomalies: landing artifacts and jump tables

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Found: indirect jumps/calls through computed pointers - some resolved, some not, some a decoy

Asked directly: does this codebase use jump tables (computed `jmp`/
`call` through a pointer, the classic compiled-`switch` pattern) rather
than fixed addresses? A full sweep of both `.lst` files for any
`jmp`/`ljmp`/`call`/`lcall` whose operand is a register or memory
reference (not a literal address) found:

**Already-understood, resolved dispatch** (found in earlier passes,
not new): `lcall es:[bx]` in `switch_to_next_task` - the task
scheduler's per-task entry-point table at segment `0xE628`+`0xE`+
`task_idx*4`; and the many `lcall es:[bx+di+6]` sites - the per-item
handler table at `[0x1D10]` that `dispatch_item_handler_if_enabled`
and several self-test/menu-item handlers use. Both have a fully
enumerable target set (a real table with a small number of live
entries), so "resolved" here means genuinely understood, not just
"found."

**Two more turned up this sweep** (`ljmp [bp+di]` at `0x88729`, and
`jmp word ptr [bx+si]` in `SUB_F6F4A`) - but neither is a clean jump
table. Both are **the same landing-artifact anomaly documented
elsewhere in this file, just manifesting as a coincidentally-valid
jump/call opcode instead of a coincidentally-valid data-looking one**:

- `0x88729`: this is the *exact* target of `SUB_E99DF`'s `ljmp`
  (`0xE99E2`, one of the confirmed `SUB_EAC86`-class garbage openers,
  `int1`/`add cl,dl`/`ljmp 0x830A:0x5689`) - previously assumed to be
  a meaningless garbage target, never actually checked. It isn't
  garbage: `0x88729` holds a real, isolated `ljmp [bp+di]` instruction
  with no other code around it (a huge unreached gap on both sides).
  Since `SUB_E99DF` never sets up its own `bp` (no prologue), `[bp+di]`
  at that point resolves against whatever the *caller*
  (`compute_and_format_sample_delta_readout`) had in `bp`/`di` at the
  time of the call - i.e. this is a real indirect jump, but indexed
  off the caller's own stack frame rather than a fixed table in ROM,
  and its behavior can't be statically resolved without knowing what
  `SUB_F4150` leaves in `di` at that call site.
- `SUB_F6F4A`: a **third/fourth instance** of the `SUB_E90A5`/
  `SUB_E92B0`-class "lands 1 byte into a legitimate instruction"
  anomaly. The byte right before it (`0xF6F49`) is the last byte of a
  `cmp di,0x20` instruction belonging to the *real* surrounding
  function (an `update_display_mode_flags`-family bit-check chain,
  setting `[0x1B79]` etc.) - reading from `0xF6F4A` instead decodes
  the trailing `FF 20` bytes of that same `cmp` as `jmp word ptr
  [bx+si]`. Genuinely called via a real, unambiguous `lcall` from 2
  places (`0xEB322`/`0xEB518`), landing exactly 1 byte before where the
  real code continues. `SUB_F1254` (`call word ptr [di-0x75]`) is a
  **fifth instance** of the same thing - it's 1 byte before a genuine
  `55 8B EC` prologue (`FUNC_3532_1255`), called via a real `lcall`
  from `0xE8EEF` (inside the `convert_sample_value`/`assert_and_halt`
  tag-dispatch neighborhood, the same family `SUB_E90A5`/`SUB_E92B0`
  came from).

**This raises the "landing artifact" count to 5+ independent
instances across 2 unrelated code neighborhoods** (the `convert_
sample_value` tag-dispatch area, and the `update_display_mode_flags`
bit-check area), each a **real, unambiguous, compiled `CALL`/`LCALL`/
`JMP`** landing exactly 1-4 bytes before where the surrounding,
coherent code clearly intends execution to resume. This is now the
single strongest piece of evidence in the whole project for the
"stale call site left over from a slightly different build" theory
over "decode tooling bug" - a tooling bug could explain one address
being misread, not five independent, unambiguous encoded call/jump
targets all landing a few bytes short of their real destination.
**Not resolved**: whether this is a genuine off-by-N linker/relocation
defect in this specific ROM revision (worth checking against the
`-13` revision if it's ever dumped), or some other systematic cause.

## Systematic landing-artifact sweep (`find_landing_artifacts.py`) - root cause confirmed, one real dual-entry-point find

Built a detector to find every instance of this class at once rather
than one at a time by hand: for every genuine "sub"-kind call target
`T`, check whether `T`'s own decoded instruction overlaps (1-4 bytes,
strictly less than `T`'s own instruction size) with any other
independently-reached instruction start anywhere in the combined
proven+heuristic corpus. Found **49 candidates** across both ROMs.

**Root cause confirmed statistically**: read the raw opcode byte at
each of the 49 candidate addresses. 24/49 (49%) are an `ADD`-family
opcode (`0x00`=`ADD r/m8,r8` alone accounts for 18/49, `0x04`=`ADD
AL,imm8` for 4, `0x02`=`ADD r8,r/m8` for 2) - far more than chance
would predict for 3 opcodes out of 256 possible. This matches the
existing hypothesis exactly: landing 1 byte short overwhelmingly lands
on a byte that's serving as a displacement/immediate/high-byte filler
in the *real* preceding instruction, and `0x00` is by far the most
common such filler byte in compiled x86 (a zero displacement, a
`mov byte[x],0`, the high byte of a small `jmp rel16`) - which also
happens to be a valid single-byte `ADD` opcode. The remaining ~half
are a long tail of one-off single-byte opcode coincidences (`0xF6`
group/1-op forms, `0xE7`/`0xE8` near-misses on `out`/`call`, etc.),
consistent with "any byte can occasionally coincide with a valid
opcode," not a second distinct mechanism.

**One candidate turned out to be a genuinely new, fully-traced finding
- not a landing artifact at all, but a real, deliberate dual-entry
byte-sharing trick, the same class as the already-documented `SUB_
E8E29`/`SUB_E8E03` "two valid divergent decodes" case:**

`write_hw_shift_register` (`0xEE13B`) is **not a standalone function**
- it's a secondary entry point into the middle of `dispatch_item_
handler_if_enabled` (`0xEDFFD`-`0xEE35B`), reached by 8 real, separate
`lcall`s from both ROMs (`0xF7553`, `0xE8309`, `0xE834F`, `0xE8395`,
`0x87B3B` [comm ROM], `0xF75CD`, `0xE8499`, `0xE843B`). Byte-verified
both readings of the shared region (raw bytes from `0xE139`: `8b 7e f6
d1 e7 d1 e7 d1 e7 d1 e7 c4 1e 10 1d ...`):

- **Entered via fallthrough** from `L_EE139` (a backward `jl` inside
  `dispatch_item_handler_if_enabled`'s own loop): `mov di,[bp-0xa]`;
  `shl di,1` ×4; `les bx,[0x1d10]`; ... - builds up a bit pattern in
  `di` by repeated left-shift, then indexes the `[0x1D10]` item table.
- **Entered via the 8 external `lcall`s at `0xEE13B`** (1 byte later):
  `not cl`; `out 0xd1,ax` ×3; `out 0xd1,ax`→`out 0xc4,ax` (last one
  targets `0xC4` instead of `0xD1`) - the exact shift-register hardware
  write sequence already documented in `FUNCTIONAL_NAMES`'s comment.

**Both readings reconverge byte-exactly** at `0xEE148` (`mov dl, byte
ptr es:[bx+di+5]`) and continue as one shared tail from there. This is
not a near-miss or an off-by-one defect - it's a precisely-engineered
overlap: the compiler (or a hand-tuned fragment) encoded "shift a bit
pattern into `di`" and "flush `ax` out to the hardware shift register"
as two different readings of the *same* physical bytes, entered at two
different byte offsets, that land on the identical continuation
address afterward. Likely explanation: callers that already have the
bit pattern pre-computed in `ax` jump straight to the `OUT`-sequence
entry, skipping the shift-building preamble that `dispatch_item_
handler_if_enabled`'s own loop needs when building the pattern from
scratch. **Correction to make**: `FUNCTIONS.md`'s `write_hw_shift_
register` entry should note it's a secondary entry point sharing bytes
with (not an independent sibling of) `dispatch_item_handler_if_
enabled`, not a fully standalone routine.

**The other ~47 candidates were not all individually traced** (that
would need one dedicated session per non-trivial one, matching the
effort this single case took) - most match the already-documented
"lands 1-4 bytes into an ordinary neighboring instruction, silently
reconverges" shape with no further story to tell, per the root-cause
statistics above. Worth revisiting individually only if a specific one
looks suspicious on inspection (e.g. unusually large reconvergence
distance, or - like this one - reached by many independent call
sites, which is the strongest tell that a "candidate" is actually a
deliberate second entry point rather than an accidental near-miss).

## Follow-up, 2026-09-14: ranked every candidate by caller count, traced the top one - `compute_and_print_item_delta_readout`

Added caller-count ranking on top of `find_landing_artifacts.py`'s
existing output (it tracks each candidate's `refs` list already, just
never printed a ranked view) to systematically apply the "many
independent call sites" tell from the note above, instead of eyeballing
the raw list. Top of the list by a wide margin: physical `0xF3EA3`
(in `160-3532`) with **33 independent callers** - more than 4x the
previous record holder (`write_hw_shift_register`, 8 callers). Second
and third place (`0xE9858` with 21, `0xF44C8` with 18) are noted but
not yet individually traced - worth doing next if this is picked up
again, using the same ranking approach.

**Traced the #2 candidate too, same session: `0xE9858`, now named
`decimate_peakdet_samples`.** Lands 1 byte into a real `cmp word ptr
[bp+0x12],0` instruction. Full signature recovered (`far ptr src, far
ptr dst, count, byte/word mode flag`): for every group of 8 input
samples it scans for the min and max, then emits **2 output samples
per group** - whichever extremum changed most recently during the
scan, followed by either the other extremum or an averaged boundary
value depending on how the next sample compares - the standard
peak-detect min/max envelope-compression algorithm. This is a clean,
confident, complete-enough trace (unlike `compute_and_print_item_
delta_readout` above, which only got a partial first-pass ID) because
the algorithm's loop body is small and self-contained. Directly
confirms and *corrects* an existing cross-reference: `handle_gpib_
device_clear`'s `FUNCTIONS.md` entry already named this address
(as `SUB_E9858`) with the vague description "reset/clear a display
region" - now updated to reflect what it actually does. Called from
both ROMs (21 sites total, cross-ROM from the comm ROM in at least
one case) - strong independent confirmation this project's `PEAKDET`
acquisition mode (seen throughout this session's live RS-232/HPGL
testing) has a real, now-identified software implementation.

Traced `0xF3EA3`: it's 1 byte into a real `mov word ptr [bp-0x10],ax`
instruction (the same landing-artifact shape as every other case here)
and, once correctly read starting at the real landing point
(`0xF3EA4`), turns out to be part of the **already-mapped cursor/
delta-readout subsystem** - it shares the `[0x570]`-indexed
`[x+0x18C]`/`[x+0x18F]` per-item flag tables that
`compute_and_format_sample_delta_readout` and `sync_shift_register_
output` already use (see `FUNCTIONS.md`), checks those flags, computes
a position delta via a shared helper at `0xF830E` (itself called 23x
elsewhere - also not yet individually traced), and heads toward
printing into the readout buffer far pointer `[0x1C80]`. Named
`compute_and_print_item_delta_readout` and registered in `gen_disasm_
x86.FUNCTIONAL_NAMES`, regenerated, and verified byte-identical - see
`FUNCTIONS.md`.

**Honest scope of what's confirmed**: the high-level shape (checks
per-item enable flags, computes a delta, heads toward printing) and
its architectural home (the cursor/delta-readout subsystem) are solid.
The function is large (spans well past `0xF40C9` based on internal
`loc`-kind branch targets found during the trace) and **most of its
internal branches were not individually walked** - this is a
first-pass identification, not a full pseudocode-level trace. Given
it's called 33 times with different argument records (2-word structs
read from `[bp-0x18]`/`[bp-0x14]`/`[bp-0x10]` at offsets `+6`/`+8`),
the most likely explanation is that it's a shared engine invoked once
per distinct delta-measurement readout (`ΔV`, `ΔT`, frequency, etc. -
the same labels seen live in this project's own HPGL capture testing,
e.g. `ΔV1=0.00V`/`ΔT=0.000ms`) - plausible but not proven.
