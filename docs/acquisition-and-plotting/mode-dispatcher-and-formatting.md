# Acquisition mode dispatch and print formatting

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Found: the acquisition mode-change dispatcher (handle_acq_mode_change)

`handle_acq_mode_change` takes a single "what changed" flags word (arg at `[bp-8]`)
and dispatches on individual bits, each corresponding to one aspect of
acquisition/display state that just changed:

- bit `0x40` - acquisition timeout handling. If a countdown at
  `[0x54A]` has already expired (`<= 0`), calls
  `reset_acq_buffers_stub`; otherwise arms a fresh timeout by
  snapshotting the current scheduler tick (`scheduler_tick_service`'s
  `[0x752]`) into `[0x544]`, computing a deadline `[0x546] =
  [0x544] + 0x783` (0x783 = 1923 ticks), and setting bit 3 of
  `[0x1B76]` (the same flags byte `run_selftest_sequence`/
  `reinit_system_state` touch).
- bits `0x23`, `2`, `0x20` (and further ones past what's transcribed
  here) each gate their own small block of calls to
  `reset_acq_buffers_stub`, `print_and_reset_acq_buffers`,
  `update_display_mode_flags`, and `reset_plot_home_or_acq`, in
  different combinations depending on which bit(s) fired and the
  state of `[0x54C]`.

Renamed to `handle_acq_mode_change`. This is a good caller-side
confirmation that `[0x752]` (scheduler tick count) doubles as a
lightweight timebase for non-hardware timeouts elsewhere in the
firmware, not just the busy-wait use in `wait_readout_tick`.

**Since renamed: `SUB_E804F` is `update_indexed_value_if_changed`.**
One of the two call sites for this
function's sibling (also reached from the plot-position-cache code
around `update_plot_position`/`plot_line_to`) is `SUB_E804F`, whose
first bytes (`1c 1d` = `sbb al, 0x1d`) don't form a recognizable
prologue, yet the function later executes `pop si` / `mov sp, bp` /
`pop bp` / `retf 2` - implying a stack frame that was never visibly
set up. Unlike the `write_hw_shift_register`/`convert_sample_value`
"shared tail via fallthrough" pattern, this one **can't** be a
fallthrough-entry function: there's a ~172-byte gap between the
previous function's `retf` (at `0xE7FA3`) and `SUB_E804F`'s start
(`0xE804F`) with nothing in the recursive-descent graph reaching
through it, and both known callers (`160-3633` and `160-3532`) target
`0xE804F` directly via `lcall`. The body between the odd opening and
the `retf` is otherwise coherent (looks up an entry in a table
pointed to by the far pointer at `[0x1D1C]`, indexed by a value
derived from the caller's argument, and conditionally writes a
"changed" flag + new value into it, setting global dirty flag
`[0x532]` if so) - functionally plausible as "update a cached
coordinate/id table entry and flag it dirty if the value changed,"
but the un-prologued opening means the register-level details (what's
really in `ax`/`bx` on entry) aren't trustworthy. Left unrenamed
the un-prologued-opening anomaly itself is unresolved, but the
mechanism was confident enough to name anyway - same reasoning as
`init_front_panel_cluster_defaults` and the `compute_and_draw_scale_
marker` cluster elsewhere in `docs/decode-anomalies/dual-entry-
points.md`. Not classified as a `SUB_EAC86`-style anomaly since the
code past the odd opening is coherent, not garbage.
## Found: a decimal-formatting engine tangled up with SUB_F5184/L_F50FA/SUB_ED0AE

While tracing `compute_and_format_sample_delta_readout` (`SUB_F4150`,
renamed) it calls `SUB_ED0AE`, which itself calls into `SUB_F5184` -
and `SUB_F5184`'s own body sits right in the middle of a clear
`itoa`-style digit-formatting loop that starts at `L_F50FA` (repeated
`idiv`-by-10, appending `'0'+digit` characters, with a `.` decimal-
point insertion at a per-item table position `[bx+0x63A]`).

**This also confirms the "outlined helper, no own frame" theory above
with a concrete, traceable example, not just circumstantial evidence.**
`SUB_ED0AE` genuinely never touches `bp` at all - its entire body is
`mov ax,[0x464]; mov [0x46C],ax; lcall SUB_F5184; jmp L_ED0C6` (then a
bare `retf`, caller-cleans-stack, matching `SUB_F750A`/`SUB_F7603`).
Its caller (`SUB_F4150`) pushes 3 words before calling it and cleans
them up itself afterward (`add sp,6`) - words `SUB_ED0AE` never reads.
Since `SUB_F5184` (called from inside `SUB_ED0AE`) *also* has no
prologue and reads `[bp-0x10]` etc., those locals **must** belong to
`SUB_F4150`'s own frame, 2 calls up the chain - there is no other
frame they could be. This is direct, traceable confirmation (not
speculation) that at least this chain shares one frame across 3
nested `lcall`s with zero prologues in between.

`SUB_F5184` itself opens with 2 bytes (`46 0AC7` = `inc si` / `or al,bh`)
that don't fit this loop's logic at all - likely another instance of
the un-prologued-entry/decode-drift family - immediately followed by
`mov word [bp-0x10], 0`, identical to a reset done a few bytes earlier
at `L_F515C`, strongly suggesting `SUB_F5184` is just another named
entry point into the *same* loop, reached both by explicit far call
(from `SUB_ED0AE`) and by fallthrough from `L_F5175`. **Not renamed**:
the exact function boundaries here are genuinely ambiguous (this is a
shared digit-formatting engine used from several `bp-relative-context`
callers per the "outlined helper" theory above), and forcing a name on
`SUB_F5184`/`L_F50FA` specifically would overclaim precision this
investigation doesn't have.

**Follow-up: the format table's full shape is now clear, even though
`SUB_F5184`/`L_F50FA` themselves stay unrenamed.** After the 5-digit
`idiv`-by-10 loop (`L_F518B`'s `cmp [bp-8],5`), the tail at `L_F5197`
indexes a **format-spec table** by `[bp+8]*6` and appends 2 more
characters to the output buffer (`[bp+0xA]`/`[bp+0xC]`, a far pointer,
incremented after each write, NUL-terminated at the end):
`[table+0x63D]` only if nonzero, then `[table+0x63C]` unconditionally.
Combined with the decimal-point position already found at
`[table+0x63A]` (compared against the digit-loop's position counter
`[bp-8]`), this is a complete **per-format-index number formatter**:
decimal-point position + a 1-or-2-character units suffix, all driven
by one 6-byte-stride table entry (`[0x63A]`=dp position, `[0x63C]`/
`[0x63D]`=suffix chars) selected by `[bp+8]`. This is almost certainly
how the firmware renders values like "1.23mV" or "500ns" with the
correct decimal placement and unit for whatever's being measured.
`SUB_ED0AE` was renamed to `snapshot_index_and_format_number` on the
strength of this (see `FUNCTIONS.md`) even though `SUB_F5184`/
`L_F50FA` remain unrenamed - their *behavior* is now fully understood,
just not their exact standalone entry-point boundaries, which is a
narrower and more honest thing to leave unresolved than the mechanism
itself.

## Found: the print-record character-cell-copy engine (SUB_EF346/SUB_EF393/copy_char_cell_template_and_sync, was SUB_EF440)

A second entangled-but-now-mostly-understood region, in the same style
as the decimal-formatting engine above. `SUB_EF346`/`SUB_EF393` are a
**position-wraparound-clamp preamble**: given a cell index `[bp+6]`
(compared against 8 to pick a shift amount) and an in/out position
`[bp+0xA]`, they compute a half-width around a reference tick `[0x72]`
(halved again if channel flag `[0x18C]` bit `4` is set) and wrap
`[bp+0xA]` into range against it - the exact same wraparound-clamp
shape as `read_acq_sample_with_wrap`'s buffer-position math, just for
a different (character-cell display) context. Optionally rounds the
result to an even address (`[0x20]` bit `4`, gated by `[0x1B8B]`).

That preamble feeds `SUB_EF440` (**renamed** to `copy_char_cell_
template_and_sync`): once the position settles, it copies a `[0x34]`-
byte (`0xA`=10, the same constant `init_default_print_cell_dimensions`
sets) template chunk between two slots of a shared char-cell table at
`[0x1C14]` via `memcpy_far`, gated by a per-channel config nibble at
`[dest_idx*16+0x18F]`, then calls `sync_shift_register_output`. This
is also reachable **directly**, bypassing the wraparound preamble
entirely (`SUB_E8E03` calls it with a fixed arg `0`) - the two halves
are only loosely coupled, which is why `SUB_EF346`/`SUB_EF393` stay
unrenamed even though `SUB_EF440` is now confidently named: the
preamble's *output* is well understood (a clamped position), but its
own entry-point boundaries have the same dual/overlapping-decode
ambiguity documented elsewhere in this cluster (`SUB_EF393`'s first
instruction is a `jle` that lands mid-loop inside `SUB_EF346`'s body,
matching the shared-tail shape, not a fresh call convention).

**Correction/deepening, 2026-10-09**: re-read `SUB_EF346`'s full body
while checking it as a naming candidate (per the established "name for
what it does despite entry-point ambiguity" precedent used for
`init_front_panel_cluster_defaults` etc.) and found "feeds the
position into `copy_char_cell_template_and_sync`" undersells it -
after the wraparound clamp, `SUB_EF346` itself does its **own**
unconditional `memcpy_far` (`0xEF403`-`0xEF438`) between two `[0x1C14]`
slots indexed directly by `[bp+6]`/`[bp+8]` (no `+1` offset), using a
separately-computed write position (`es:[bp-0xE] + [bp+0xA]`, stored to
`[bp-0x12]` but not obviously consumed again before the `memcpy_far`)
- then falls through into `copy_char_cell_template_and_sync`'s *own*,
separately-gated, `+1`-offset copy of the same table. So this isn't a
clamp-then-copy-once pipeline, it's **two stacked copies with
different indexing** into the same table, and what the first
`memcpy_far`'s destination/source actually represent (vs. the second)
isn't understood yet - **not confident enough to name even under the
relaxed precedent**; this needs the `[bp-0x12]`/first-`memcpy_far`
relationship traced further, not just the preamble's clamp math. Left
unrenamed; recording the deeper shape so the next pass doesn't have to
re-derive it from scratch.

## Found, 2026-10-10: a "previous/next alternate value" stepper pair for multi-value menu items (`step_item_subvalue_back`/`_fwd`)

While naming the `0xECE82` landing-artifact candidate (see `docs/
decode-anomalies/landing-artifacts-and-jump-tables.md`'s "Follow-up,
2026-10-10"), identified what its host function (`FUNC_3633_CE5B`,
real entry `0xECE5B`) actually does from its behavior, independent of
its 5 messy external callers - per the project's "name from what it
does, not from who calls it" convention.

`0xECE5B` checks the current item's alt-value count `[0x3D6]`: if
`<= 0`, jumps to a tail that dispatches the item's `[0x1D10]` handler
with arg `3` (if present) and exits - no formatting. Otherwise, it
dispatches the same handler with arg `2` (if present), then
**decrements** a per-item alt-value step byte (`[0x3E3 + item index]`,
the index being a cached copy of `[0x464]` at `[0x46C]`), wraps the
result modulo `[0x3D6]` with proper negative-wraparound handling (a
signed `idiv`, not a simple mask), writes it back, and reformats it via
the already-documented decimal formatter (`SUB_F5184`). **This is a
"show the previous alternate value" step** for a menu item that cycles
through several related readouts (plausibly something like a cursor
readout offering `ΔV`/`ΔT`/frequency variants, though no specific
on-screen item has been tied to it yet).

Immediately following it in ROM, `FUNC_3633_CF19` (real entry
`0xECF19`) is a near-exact structural twin - identical guard, same
`[0x1D10]` lookup, same arg-`2`/arg-`3` dispatch split - except it does
`inc` instead of `dec` on the step byte: the "next alternate value"
counterpart. No direct caller was found for `0xECF19` by its exact
`lcall` encoding (unlike `0xECE5B`, which at least has the landing-
artifact bypass `0xECE82` reached 5 times) - named from the shape/
pairing alone, per the same precedent already used for `init_front_
panel_cluster_defaults` and the `compute_and_draw_scale_marker`
cluster.

Renamed (see `FUNCTIONS.md` and `VARIABLES.md`'s new "Per-item handler
dispatch table" section for the full variable writeup):
`0xECE5B`→`step_item_subvalue_back_guarded`, `0xECE82`→`step_item_
subvalue_back`, `0xECF19`→`step_item_subvalue_fwd`. Also noteworthy:
the handler dispatched through `[0x1D10]+6` is now confirmed called
with at least 3 different literal command codes across different
callers (`2`, `3`, `4` - the last from the already-named `dispatch_
item_handler_if_enabled`) - a small integer message-dispatch
convention for a shared per-item handler, not type-specific argument
passing. Not pursued further: what the handler actually does with each
code, and which on-screen item(s) ever have a nonzero `[0x3D6]`.

## Follow-up, 2026-10-10: traced the dispatcher behind command code `4`, found 2 more functions in the cluster

Picked up the open thread above - what the `[0x1D10]+6` handler's
command codes (`2`/`3`/`4`) actually drive - by re-reading `dispatch_
item_handler_if_enabled`'s (`0xEDFFD`) full body instead of just its
previously-documented arg-`4`-dispatch-and-exit summary.

**`dispatch_item_handler_if_enabled` is itself a secondary entry
point.** The real, fully-prologued function is `0xEDF56`, now named
`dispatch_item_change_notification`: it branches on the current item's
behavior-flag byte `[0x466]` (bits `0x4`/`0x1`/`0x40`, each gated by a
`[0x3E2]==0x78`/`[0x468]` guard pair seen elsewhere in this ROM as a
"comm option installed" check, not confirmed here) to conditionally
call one of 3 not-yet-analyzed helpers (`SUB_F408E`, `SUB_F45A4`,
`SUB_F47AB`), and/or dispatch the current item's handler with code `4`.
Two different internal paths through this flag logic both funnel into
the same dispatch-arg-`4`-then-exit tail that `dispatch_item_handler_
if_enabled`'s own externally-confirmed entry (`tag_position_marker_
and_dispatch` calling it by exact far address `0xEDA2:0x5DD`) also
reaches - so the existing, narrower `dispatch_item_handler_if_enabled`
writeup was correct for that one call site, just incomplete about the
rest of the function it turned out to be embedded in. A second,
richer block in the same body (calls `clamp_position_counter_across_
records(1,1,0)`, then re-dispatches the handler a second time with
code `4`) is reachable only via `dispatch_item_change_notification`'s
own internal flow, never from the `tag_position_marker_and_dispatch`
call site - why the handler would need notifying twice around a
position-counter re-clamp is unresolved.

**Found and named a second function in the same ROM region,
`reset_current_item_to_table_default` (`0xEE0BB`)**: reads the
`[0x1D10]` table's own record-0 field at `+4` into `[0x464]` (sets the
current item index from the table's own designated default entry, not
a caller argument), zeros `[0x46C]`, sets `[0x462]=1`, then reloads
`[0x466]` from the new current item's `+0xE` field - a "jump back to
the table's first item and reload its flags" initializer. No caller
confirmed yet.

**Corrected `write_hw_shift_register`'s reachability** (see
`MEMORY_MAP.md` and `FUNCTIONS.md`): it is *not* reached as a fallback
tail of `dispatch_item_handler_if_enabled`/`dispatch_item_change_
notification` as previously written - that function `retf`s well
before `write_hw_shift_register`'s address. The real enclosing function
is a large, not-yet-named item-list loop (`0xEE0F7`) that calls
`compute_and_format_sample_delta_readout` and `clamp_position_counter_
across_records` repeatedly and uses `write_hw_shift_register` as a
shared-frame secondary entry inside that loop - the same mechanism
already documented for `0x88729`/`SUB_F5F56`/`step_item_subvalue_back`.
This function was only partially traced (its body runs well past
`0xEE358`) - a good next candidate for a full trace, since pinning it
down would likely explain why `write_hw_shift_register` has 8 real
external callers distinct from this internal one.

Net: the `[0x1D10]`-table cluster is bigger than previously mapped -
at least 5 distinct functions now (`dispatch_item_change_notification`/
`dispatch_item_handler_if_enabled`, `reset_current_item_to_table_
default`, the unnamed `0xEE0F7` loop containing `write_hw_shift_
register`, plus `step_item_subvalue_back`/`_fwd` from the prior
session) all reading or driving the same per-item record array. Command
code `4` is now tied to "item changed/position re-clamped" rather than
a specific stepping direction, consistent with codes `2`/`3` (stepping)
being distinct from `4` (general refresh notification) - a plausible
but not confirmed reading.
