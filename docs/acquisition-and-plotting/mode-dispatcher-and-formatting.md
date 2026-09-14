# Acquisition mode dispatch and print formatting

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Found: the acquisition mode-change dispatcher (handle_acq_mode_change)

`SUB_E80E4` takes a single "what changed" flags word (arg at `[bp-8]`)
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

**Left unresolved: `SUB_E804F`.** One of the two call sites for this
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
pending a closer look; flagged here rather than guessed. Not
classified as a `SUB_EAC86`-style anomaly since the code past the
odd opening is coherent, not garbage.
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

## Found: the print-record character-cell-copy engine (SUB_EF346/SUB_EF393/SUB_EF440)

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
