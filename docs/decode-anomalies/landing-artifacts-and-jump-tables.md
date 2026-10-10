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
  `compute_and_format_sample_delta_readout` leaves in `di` at that call site.
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
and third place (`0xE9858` with 21, `0xF44C8` with 18) were traced
next, in the same session - see below.

Traced `0xF3EA3`: it's 1 byte into a real `mov word ptr [bp-0x10],ax`
instruction (the same landing-artifact shape as every other case here)
and, once correctly read starting at the real landing point
(`0xF3EA4`), turns out to be part of the **already-mapped cursor/
delta-readout subsystem** - it shares the `[0x570]`-indexed
`[x+0x18C]`/`[x+0x18F]` per-item flag tables that
`compute_and_format_sample_delta_readout` and `sync_shift_register_
output` already use (see `FUNCTIONS.md`), checks those flags, computes
a position delta via the already-confirmed `read_acq_sample_with_wrap`
(`0xF830E`, called 23x in total including this site - **correction**:
this was first written up as "not yet traced," but it turns out to
already be a fully-documented, confirmed function from an earlier
session - see `FUNCTIONS.md`), and heads toward printing into the
readout buffer far pointer `[0x1C80]`. Named
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

**Traced the #3 candidate too: `0xF44C8`, now named
`compute_and_print_cursor_position_readout`.** Lands 1 byte into a
real `cmp byte ptr [0x1b83],0x14` instruction. Takes no arguments -
works entirely off global state. Computes 2 Y-position-looking values
(`>>3`, `+0xEE`) from a 14-byte-stride table at `[0x570]*2*0xE +
0x576` for the current item and item+1 (plausibly the 2 on-screen
cursors), checks a validity/type field at `[0x570]*0x10+0x192`, then
prints via **the exact same `SUB_E97DC` helper** `compute_and_format_
sample_delta_readout` already calls, branching on `[0x1B83]==0x14`
(the already-confirmed device-type field from `detect_comm_option_
hw`) to pick which label source to use. Very likely a sibling of
`compute_and_format_sample_delta_readout` in the same cursor/readout
family - printing cursor *position* rather than delta values - but
not proven, and the internal branches weren't individually walked
(same honesty caveat as `compute_and_print_item_delta_readout`).

**This closes out the top 5 candidates by caller count** - between
`write_hw_shift_register`/`SUB_EAC86` (found in earlier sessions) and
the 3 found this session, every landing-artifact candidate with more
than ~15 independent callers has now been traced.

**Immediately followed up on the shared helper all 3 functions call:
`SUB_E97DC`, now named `extract_strided_channel_samples`.** Turned out
*not* to be a print primitive as guessed - it's a **strided/de-
interleaving copy utility**. Its real entry point is `0xE9744` (found
by backtracking from `0xE97DC` for the nearest `push bp; mov bp,sp`);
`0xE97CA` and `0xE97DC` are both legitimate secondary entry points
(real `lcall` targets, not byte-corruption artifacts) that skip the
full function's remainder-alignment preamble - exactly the "caller
already has the parameters computed, skip the setup" shape already
documented for `write_hw_shift_register`. It copies every Nth byte or
word from a source to a destination far pointer with a caller-selected
stride (2, 3, or 6 bytes seen across the different entry points),
gated by 2 flag bytes choosing element size and which interleaved
sub-stream to extract. Given every one of its 3 known callers is a
per-channel measurement/readout function, the strong working
hypothesis is that this pulls one channel's samples out of interleaved
dual-channel acquisition memory - plausible, not proven.

## Follow-up, 2026-10-09: traced the next-highest-caller-count untraced candidate (`0xE951A`, 6 callers) - ruled out the `write_hw_shift_register`-style explanation, real identity still unresolved

Re-ran `find_landing_artifacts.py` after the renames above and re-ranked
the remainder by caller count. Two stood out from the single-caller
majority: `0xE951A` (6 callers) and `0xF4CE8` (6 callers, not yet
looked at). Traced `0xE951A` first.

`0xE951A` lands 1 byte short of `0xE951B`, exactly the dominant
root-cause case from the statistics above: the byte at `0xE951A` is
`0x00` (the high byte of the real, enclosing `cmp word ptr [bp+6],
0xff` instruction's `0x00FF` immediate), which is also a valid `ADD
r/m8,r8` opcode. Read as a fresh instruction stream from there, it
decodes as `add byte ptr [si+7], bh` (3 bytes, `0xE951A`-`0xE951C`),
landing on `0xE951D` - which is where the real `cmp word ptr [bp+6],
0x300` instruction begins regardless of whether the real `jl` at
`0xE951B` (1 byte into the real stream) is taken. **This is a weaker
reconvergence than `write_hw_shift_register`'s**: the fake 3-byte path
only lands back on the shared stream for the *not-taken* side of the
real `jl` - if the real branch is taken (jump to `L_E9524`), the fake
path silently diverges and never rejoins. `write_hw_shift_register`'s
two readings reconverged byte-exactly on *every* path; this one only
reconverges on one of two.

The address `0xE951A` sits inside the byte range of a heuristically-
recognized function, `FUNC_3633_9500` (`0xE9500`-`0xE956B`, a single
`push bp`/`mov bp,sp` entry with exactly one exit, `retf 2` at
`0xE956B`). All 6 real `lcall SUB_E951A` sites (`160-3532` physical
`0xF19F7`, `0xF1A1E`, `0xF259D`, `0xF25CE`, `0xF2778`, `0xF4774`) use
an **identical argument-pushing idiom**: `push word ptr [bp+0xa]`;
`push ds`; `mov di,0x674` / `push di`; then a far pointer (`push
es`/`push <reg>`) sourced either from the caller's own `[bp+6]`
parameter or a fixed global (e.g. `[0x6ae]`) via `les`. That's **7
pushes (14 bytes)** before every call, and every site cleans up
identically afterward with `add sp, 0xa` (10 bytes) right after the
`lcall` returns.

**Stack-accounting mismatch found**: 14 bytes pushed, but only
`0xa`=10 (caller's `add sp`) + `2` (`FUNC_3633_9500`'s own `retf 2`)
= 12 bytes get cleaned between the two of them - 2 bytes (1 word)
unaccounted for. If `0xE951A` really were a `write_hw_shift_register`-
style secondary entry point into `FUNC_3633_9500` (skipping its
`push bp`/comparison preamble the way the hardware-shift-register
case skips its bit-building preamble), the *shared exit* should still
balance the stack exactly the way it does for every other caller of
that function - it doesn't. **This is concrete evidence against the
"secondary entry into a known function" explanation for this specific
candidate**, unlike `write_hw_shift_register` and `extract_strided_
channel_samples` above. Two readings remain open, not resolved:
either this really is an ordinary single-byte-filler landing artifact
(consistent with the root-cause statistics, and with the real logic
living somewhere else that correctly accounts for all 14 bytes - i.e.
my attribution of the call target to `FUNC_3633_9500` is the wrong
function to look at, not that the call target address itself is wrong),
or all 6 call sites share a single stale/off-by-one call target
inherited from one common source-level macro or inlined helper (which
would explain why an "accidental" bug shows up identically 6 times -
one bug in a shared expansion, not 6 independent ones).

**Not resolved, and not worth guessing further without a dedicated
session**: what function is actually meant to receive `(word
[bp+0xa]-equivalent scalar, far ptr DS:0x674, far ptr <dynamic
source>)` - `0x674` is a fixed buffer address worth checking against
`VARIABLES.md` (currently undocumented) if this thread gets picked up
again.

## Follow-up, 2026-10-09: traced the other 6-caller candidate (`0xF4CE8`) - clean byte-exact reconvergence (unlike `0xE951A`), uncovered a new self-test-step character-code variable cluster, but the write target itself needs the emulator to pin down

`0xF4CE8` lands 1 byte short of `0xF4CE9`, same dominant root-cause
case as every other candidate (the byte at `0xF4CE8` is the `0x00`
high byte of a real `mov word ptr [0x678], 0` instruction's
immediate). Read fresh, it decodes as a 4-byte `add byte ptr [bp + di
+ 0x5de5], cl` (`0xF4CE8`-`0xF4CEB`) landing on `0xF4CEC` - **and
unlike `0xE951A`, this reconverges byte-exactly with the real tail on
every path, not just one side of a branch**: the real stream at
`0xF4CE9` is `mov sp, bp` (2 bytes) / `pop bp` (1 byte) = exactly the
3 bytes (`8b e5 5d`) the fake instruction's own ModRM+disp16 bytes
reuse, landing both readings on the identical `retf` at `0xF4CEC`.
This is the same unconditional-reconvergence shape `write_hw_shift_
register` has, not `0xE951A`'s weaker branch-dependent one.

All 6 real `lcall SUB_F4CE8` sites (physical `0xF7BF2`, `0xF7D00`,
`0xF7D1B`, `0xF7D36`, `0xF7D51`, `0xF7D7F`, all in `160-3532`) push
**zero** arguments and clean up **zero** bytes afterward - consistent
with `SUB_F4CE8`'s own landing-artifact tail (`retf` with no
immediate, cleans 0 bytes). No stack-accounting mismatch this time,
unlike `0xE951A`.

**New lead, not what was being looked for**: every one of the 6 call
sites is immediately preceded by `mov byte ptr [0x3e2], <literal>` and
immediately followed by `lcall SUB_F5F56` (`0xF5F56` - itself 1 byte
into `FUNC_3532_5F50`'s own body, right after that function's `push
bp`/`mov bp,sp`/`sub sp,0xa` preamble, and `FUNC_3532_5F50` itself has
**zero** callers anywhere in the corpus - the same "only ever reached
1 byte past its own prologue" shape already seen in `extract_strided_
channel_samples`). The 6 literals written to `[0x3e2]` right before
each pair of calls: `0x73`('s'), `0x75`('u'), `0x64`('d'), `0x6c`('l'),
`0x72`('r'), `0x78`('x') - a previously-undocumented single-byte
variable, written only ever as one of these 6 ASCII characters, right
before this same `SUB_F4CE8`+`SUB_F5F56` pair. **Not claiming what the
letters mean** (no "up/down/left/right" story fits cleanly - see
below) - just recording the raw fact.

All 6 call sites sit in the heuristic-reachability-only region already
documented as `selftest_sequence_enter` (`0xF7BA5`)/`selftest_sequence_
exit` (`0xF7D99`) in `FUNCTIONS.md` - the `'s'` write is the last thing
`selftest_sequence_enter` does before its own `retf` at `0xF7BFF`; the
other 5 (`u`/`d`/`l`/`r`/`x`) are in a previously-unlabeled function in
between the two (`FUNC_3532_7C00`-`0xF7D98`, immediately before
`selftest_sequence_exit` starts at `0xF7D99` - also zero callers found,
same heuristic-only caveat as its neighbors). That function computes a
bit value into `[bp-8]` as `([0x4E8] & [0x4E7]) & 0x63` (after an
earlier, separate `0x63`-masked check of `[0x4E9]` against `[0x542]`
gates whether this logic runs at all), then tests individual bits of
it - `0x20`->`'u'`, `1`->`'d'`, `2`->`'l'`, `0x40`->`'r'` - with `'x'`
reached by a separate, unrelated condition (`[0x532]`/`[0x466]&0x20`/
`[0x468]`). **Possibly related to the already-confirmed `SWB2` mask**:
`0x63` is the exact bitmask `docs/self-test/front-panel-switches.md`
confirmed as `SWB2`'s 4 menu-navigation buttons (`MEM3`+`MEM1`+`MEM2`+
`MENU ADV`) - but applied here to a different variable cluster
(`[0x4E7]`/`[0x4E8]`/`[0x4E9]`/`[0x542]`, not `[0x758]`/`SWB2` itself),
and the bit-to-letter mapping (`0x20`->`u`, `1`->`d`, `2`->`l`,
`0x40`->`r`) doesn't line up with those buttons' initials in any
obvious way - flagging the mask coincidence, not claiming the
connection is proven.

**What `SUB_F4CE8` actually does with this is still not pinned down**:
at the `'d'`/`'l'`/`'r'` call sites, `di` is reloaded from `[bp-8]`
and then masked in place right before the call, so `di` equals the
matching bit value (`1`, `2`, or `0x40`) at the moment `add byte ptr
[bp+di+0x5de5], cl` executes; at the `'s'`/`'u'` sites `dx` (not `di`)
carries the relevant value, so `di` is unaccounted for there. `cl` is
never set anywhere in this code - fully inherited/unknown at every
site. `bp` is whichever enclosing function's own valid frame pointer
(`selftest_sequence_enter`'s for `'s'`, `FUNC_3532_7C00`'s for the
rest) - so `bp+di+0x5de5` is a real, computable-in-principle but
data-dependent far-from-the-frame address (0x5de5 is large enough that
mod-0x10000 stack-segment wraparound is almost certainly the intended
mechanism, the same kind of trick already flagged as unresolved for
`0x88729`'s `ljmp [bp+di]` in the first section of this file) - **not
resolvable further by static reading; would need the emulator to
observe the actual `SS`/`bp` value at one of these 6 call sites and
compute the real target address**. `SUB_F5F56`'s own body (reached the
same zero-argument way, 1 byte past its home function's prologue) uses
`[bp+6]`/`[bp+0xa]`/`[bp+0xc]` as if given 3 real pushed parameters,
which none of the 6 real callers provide - whatever it reads there is
either stale stack content or genuinely irrelevant to why it's called
this way, also unresolved.

**Net assessment**: structurally this is a second clean `write_hw_
shift_register`-class dual-entry point (unlike `0xE951A`, which wasn't)
, and along the way it surfaced a real, new, previously undocumented
variable cluster (`[0x3E2]`, `[0x4E7]`-`[0x4E9]`, `[0x542]`) worth
adding to `VARIABLES.md` - but the actual payload (what byte gets
written where, and why a 1-character mnemonic matters) needs dynamic
tracing, not more static reading, to go further.

### Correction, same day: `SUB_F5F56`'s "unexplained" parameter reads aren't unexplained - same shared-frame mechanism as `0x88729` above, not a separate mystery

Re-examined `SUB_F5F56` (`0xF5F56`) itself, not just its 6 call sites.
It is 6 bytes into `FUNC_3532_5F50` (`push bp`/`mov bp,sp`/`sub sp,
0xa` - exactly 6 bytes), i.e. it **skips that function's own prologue
entirely** - meaning it never establishes its own `bp`. Its body reads
`[bp+6]` (a far pointer, used as a struct with fields at `+0`/`+2`/`+4`
/`+0xc`), `[bp+0xa]`, and `[bp+0xc]` - which, with no `push bp; mov
bp,sp` of its own, resolve against *whatever `bp` the caller already
had*, exactly the same "inherits the caller's frame instead of reading
stale stack garbage" mechanism already documented for `0x88729`'s
`ljmp [bp+di]` at the top of this file. Confirmed directly: all 6 real
`lcall SUB_F5F56` sites push zero bytes (re-verified reading the raw
bytes immediately before each, `0xF7BF7`/`0xF7D05`/`0xF7D20`/`0xF7D3B`
/`0xF7D56`/`0xF7D84` - no `push` anywhere between the preceding
`SUB_F4CE8` call and each of these), so there is no missing-argument
mystery: `[bp+6]`/`[bp+0xa]`/`[bp+0xc]` are simply the *enclosing*
function's own incoming parameters (`selftest_sequence_enter`'s or
`FUNC_3532_7C00`'s, whichever is live at the call site), reused
directly. Both of those enclosing functions are themselves heuristic-
reachability-only with no confirmed real caller yet (same pre-existing
caveat `FUNCTIONS.md` already carries for `selftest_sequence_enter`/
`_exit`), so the actual parameter *values* remain unresolved - but the
mechanism itself is now understood, correcting the previous framing
("reads 3 parameters none of the 6 callers provide") as a false
mystery. The same logic resolves `SUB_F4CE8`'s inherited `bp` the same
way (it also has no prologue of its own); only its `cl` input stays
genuinely unresolved, and for the identical reason - it's whatever `cl`
held when the *enclosing* function was itself entered, which traces
back to the same unconfirmed top-level caller, not a separate puzzle.

## Follow-up, 2026-10-10: traced the 5-caller candidate (`0xECE82`) - a third instance of the `0x88729`/`SUB_F5F56` shared-frame mechanism, and a reminder that the simple push-vs-retf stack check doesn't apply once the exit uses `mov sp,bp`

`0xECE82` is the next entry down the caller-count ranking after the
`>=6`-caller tier closed out above. It sits inside `FUNC_3633_CE5B`
(real entry `0xECE5B`), 1 byte into a local `je L_ECE9C` instruction
(`0xECE81`-`0xECE82`, opcode `74 19`): read fresh from the displacement
byte `0x19` onward, it decodes as a 4-byte `sbb word ptr [bp+si+2], di`
(`0xECE82`-`0xECE85`), landing on `0xECE86` - **exactly the same
address the real "branch not taken" path reaches** via its own 3-byte
`mov dx, 2` at `0xECE83`. Since the fake decode is literally made of
the real `je`'s own displacement byte plus the 3 bytes after it, an
external call to `SUB_ECE82` never evaluates the branch at all and
always reconverges - the same unconditional-reconvergence shape as
`write_hw_shift_register`/`0xF4CE8`, not `0xE951A`'s weaker
branch-dependent one.

All 5 real `lcall SUB_ECE82` sites (`0xF3C1E`, `0xF4D44`, `0xF54C9`,
`0xF5581`, `0xF562E`, all in `160-3532`) push exactly two far pointers
(8 bytes: `push <seg>`/`push <reg>` twice) and clean up only 4 of them
afterward via `add sp, 4`. In 4 of the 5 sites the first far pointer is
`ds:bx` where `bx` is an offset into the `[0x638]` table and the second
is either the fixed `ds:0x65e` or `ds:[0x65e + <per-item offset>]`; the
remaining site (`0xF4D44`) instead pushes `es:[di+2]` (from `[bp-0xa]`)
and the caller's own incoming far-pointer parameter `es:[bp+6]`. Every
site stores the returned `ax` into a destination tied to that same
`[0x638]`-ish table (`[0x650]`, `[0x638]`, `[0x63e]`, `[0x644]`, or
`es:[di]`), consistent with a lookup/format operation parameterized by
a per-item record rather than a fixed pair of arguments.

**`FUNC_3633_CE5B`'s true, fully-prologued entry point (`0xECE5B`) has
zero confirmed real callers anywhere in the corpus** - every real
reference reaches this code through the `SUB_ECE82` landing-artifact
offset instead (confirmed via `grep -n "ECE5B\|CE5B" disasm/
sysrom_3532_3633_heuristic.lst`, which returns only the function's own
definition line). The same "only ever reached past its own prologue"
shape already seen for `extract_strided_channel_samples` and
`FUNC_3532_5F50`/`SUB_F5F56` above.

**Why the simple stack-accounting check doesn't resolve this one the
way it did for `0xE951A`/`0xF4CE8`**: `SUB_ECE82` never executes its
own `push bp`/`mov bp,sp` (that's the 2 bytes of `FUNC_3633_CE5B`'s
real prologue it skips), and the function's single shared exit
(`L_ECF15`: `mov sp,bp` / `pop bp` / `retf`, no immediate) resets `SP`
from `BP` before popping and returning. Because `SUB_ECE82` never set
`BP` itself, that `mov sp,bp` resyncs the stack against whatever `BP`
the *enclosing* function already had - the same inherited-frame
mechanism documented for `0x88729` and `SUB_F5F56` above, not a fixed,
statically-computable `retf`-immediate. That means the straightforward
"sum pushed bytes vs. sum cleaned bytes" arithmetic that cleanly
confirmed `write_hw_shift_register`/`0xF4CE8` and cleanly ruled out
`0xE951A` **doesn't mechanically apply here**: the real `SP` delta
depends on the numeric relationship between the caller's `SP` and the
inherited `BP` at the moment of the call, which is a runtime quantity,
not something this static reading can pin down further. The 5 callers'
uniform `add sp, 4` is consistent with - but not independently proof
of - this mechanism; confirming the exact arithmetic would need the
emulator to observe a real `BP`/`SP` pair at one of these 5 call sites.

No `[bp+N]` read appears anywhere in `SUB_ECE82`'s body, so the
inherited, stale `BP` is never actually dereferenced for data - only
used by the final `mov sp,bp` to collapse the frame. The 2 pushed far
pointers are likewise never read via `[bp+N]`; they just sit on the
stack until the `mov sp,bp` reset discards them.

**Bonus, not yet connected to anything conclusive**: right before the
indirect per-item dispatch (`les di,[0x1d10]` / `lcall es:[bx+di+6]`,
indexed by `[0x464]`), `SUB_ECE82` does `mov dx,2` / `push dx` - i.e.
it calls the exact same table-slot handler (`[+6]`, table `[0x1d10]`,
index `[0x464]`) that `dispatch_item_handler_if_enabled` (`0xEDFFD`)
already uses, but with a literal argument of `2` where `dispatch_item_
handler_if_enabled` uses `4`. That handler is reached as a true far
call right after `SUB_ECE82`'s only push, with no other pending stack
content from this function's own body - suggestive of a shared,
generic per-item-action entry point keyed by a small integer command
code, not of the 2 far pointers being forwarded to it. Not confirmed
further; flagging the parallel for whoever picks up `[0x1d10]`'s
handler table next.

**Net assessment**: a third confirmed instance of the `0x88729`-style
shared-frame secondary entry mechanism (clean, unconditional
reconvergence; true entry unused; no own prologue) - not a repeat of
`0xE951A`'s genuine negative. The useful general lesson for the rest of
this candidate list: **check whether the shared exit does `mov sp,bp`
before trusting a push-vs-retf-immediate stack-accounting comparison**
- `write_hw_shift_register`/`0xF4CE8` both have exits that don't rely
on an inherited `BP` this way, which is why the simple arithmetic
worked cleanly for them.
