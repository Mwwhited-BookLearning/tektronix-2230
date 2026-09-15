# The tick-driven task scheduler

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## A small task scheduler, driven by INT2_HANDLER_LATE

Read `INT2_HANDLER_LATE` closely this session (while renaming
functions in its immediate neighborhood) - it's more than a generic
NMI handler, it implements a **small task/context switcher**:

1. Saves the interrupted context's `SP`/`SS` into a per-task table at
   `[0x1A9D + idx*4]`, where `idx` is the current task index at
   `[0x1ACD]`.
2. Checks the *new* `SP` against a stack-limit table (indexed the same
   way, at a fixed table based at `ES=0xE628`) or a constant `0x7C4`
   for task 0 - if the stack has grown past its limit, jumps to
   `L_E5E99` (in the boot-adjacent `0xE5D1` segment) - **a per-task
   stack-overflow guard**.
3. Sets a "ready" flag bit (`0x40`) for the current task in a flags
   table at `[0x1A91 + idx]`, calls `scheduler_tick_service`
   (`0xE6524` - see "the per-tick heartbeat" below, NOT the task
   picker as originally guessed here), then falls into
   `switch_to_next_task` (`0xE6166`).
4. `switch_to_next_task` calls **`select_next_ready_task`** (`0xE5C65`
   - the actual "pick the next task to run" logic guessed at above):
   it walks BACKWARD through the same 12-entry per-task ready-state
   table at `[idx + 0x1A91]`, looking for a task whose ready bits
   (low nibble, after masking off bits `0xB0`) are nonzero, and writes
   the found index into `[0x1ACD]` (also setting `[0x1A8F]` to `0` if
   the found index is `8` - very plausibly a distinguished idle/
   background task slot - or `0x200` otherwise). `switch_to_next_task`
   then re-reads `[0x1ACD]` (now updated), checks that task's ready
   flag - if set, loads *that* task's saved `SP`/`SS` from the same
   `[0x1A9D + idx*4]` table and resumes it (the standard 9-register
   pop + `iret` epilogue, now running on the new task's stack); if not
   ready, falls through to a different dispatch via a table at
   `ES=0xE628` indexed the same way (likely a default/idle handler per
   task slot).

This is genuinely a **small preemptive (interrupt-driven) multitasking
kernel** with independent per-task stacks and overflow protection, not
just a generic interrupt handler - a significant piece of this
firmware's overall architecture. Confirms the earlier speculation
above ("the firmware switches between distinct operating states").

**Found the other half: task creation.** `create_task` (`0xE6224`)
saves a brand-new register context (`pushf`/`cli`/push all 9 regs) onto
a fresh stack, stores that stack's `SP`/`SS` into the *same*
`[0x1A9D+idx*4]` table `switch_to_next_task` reads from, saves an
entry-point far pointer to `[0x7C0]`/`[0x7C2]`, sets the new task's
ready flag, and jumps directly into `switch_to_next_task` to start it
running. Companions: `restart_current_task` (`0xE693C`, re-spawns the
current task via `create_task`) and `mark_task_ready` (`0xE6A8E`,
clears a task's "restarting" flag and bumps its ready-flags byte).

`create_task` is called from all three ROM address spaces (`160-3633`,
`160-3532`, and the comm ROM at `2998_alias_90000`) - confirming it's
a shared kernel primitive callable from anywhere via a far call, not
something private to one subsystem.

**Found the per-tick heartbeat.** `scheduler_tick_service` (`0xE6524`)
is called *unconditionally* from both paths inside `INT2_HANDLER_LATE`
- whether or not a task switch actually happens on this particular
tick, this runs every time. It:
- Reads two hardware status bytes from **fixed physical addresses
  `0x403FFA`/`0x403FFB`** into `[0x758]`/`[0x759]` - notably, this is
  memory-mapped I/O sitting just *above* the boot-time stack's initial
  top (`SS:SP = 4000:3FFA` at reset, see `boot_init`) - a classic
  embedded design where the stack is placed immediately below a fixed
  I/O window to use RAM efficiently. This is the real source of
  `[0x758]`, previously only known as "some status/mode byte checked
  around `read_display_chip_int_reset`/`read_display_chip_frame_trigger`" - now
  confirmed to be a genuine hardware status register, polled once per
  tick.
- XORs `[0x758]` against a previous snapshot (`[0x7B4]`) - classic
  edge/change detection on that hardware status.
- Increments `[0x752]` - **the exact counter `wait_readout_tick`
  busy-waits on** - confirming `wait_readout_tick`'s delay really is
  paced by this same timer tick, not an unrelated counter.
- Cycles through a few debounce/periodic-maintenance steps gated by
  `[0x1A94]`/`[0x1A95]`/`[0x756]` vs. a threshold at `[0x764]`, and
  (via the `[0x7B2]` 0/1/2 rotation in the code just before it's
  called) rotates between `scan_low_ram_chunk0`/`chunk1`/`chunk2`
  (`0xE5D3D`/`0xE5D49`/`0xE5D58`) - **a genuine background low-memory
  watchdog scan, not a timing delay** as first guessed for the
  similar-shaped `delay_read_128w`. Each reads through a different,
  non-overlapping slice of physical `0000:0000+` via `rep lodsw`
  (`0x00-0x5F`, `0x60-0xBF`, `0xC0-0xFF` respectively) - together they
  cover exactly the **first 256 bytes**, a quarter of the interrupt
  vector table, split across 3 ticks so no single tick blocks for the
  whole scan. This reframes `delay_read_128w` too: it's likely the
  SAME kind of integrity check (of the first 256 bytes in one shot)
  rather than a deliberate delay, though it's called once at startup
  rather than tick-rotated, so the "delay" name was kept for it
  specifically.

This resolves "what triggers INT2" indirectly: INT2 itself is still
presumed to be a periodic hardware timer (not confirmed which), but
now it's clear *why* a timer interrupt exists at all - it's the
heartbeat for both task switching AND this hardware status/debounce
polling, unified in one place.

**Still not confirmed**: how many tasks exist and what each one does
(the self-test/UI/acquisition-refresh loop are plausible candidates),
and exactly which hardware `0x403FFA`/`0x403FFB` belong to (front-panel
key/encoder status is the leading candidate, matching where `[0x758]`
gets used elsewhere).

`SUB_E61E3` (now `create_task_b`) turned out to be a near-identical
duplicate of `create_task`'s own body - same save-context, same
per-task table write, same ready-flag set - called specifically from
`mark_task_ready` rather than sharing the existing `create_task`. Not
confirmed why the compiler/source duplicated this rather than calling
`create_task` directly - possibly an artifact of how the original C
source structured task creation vs. task wake-up as textually separate
functions despite near-identical bodies.

## Follow-up, 2026-09-15: `create_task` is a real `fork()`-style trampoline, and the answer to "how many tasks exist"

Went looking for "how many tasks exist and what each does" by finding
every caller of `create_task` and `create_task_b` directly - **35 and
1 respectively**. Before trying to name what each one does, checked
`create_task`'s actual calling convention by looking at 3 sample call
sites - **none of them push anything before the `lcall`**. That's a
real puzzle, since the function's own body clearly saves *something*
as "the new task's entry point" (`[0x7C0]`/`[0x7C2]`).

The answer: `create_task` doesn't take an explicit entry-point
argument at all - it's a **`fork()`-style trampoline**. Its prologue
(`pushf; cli; push ax,bx,cx,dx,bp,si,di,ds,es; mov bp,sp`) is a normal
context-save, but then it does something unusual to 3 specific stack
slots:

```
mov ax, [bp+0x12]      ; ax = FLAGS (from the pushf)
xchg [bp+0x16], ax      ; ax <-> return CS
xchg [bp+0x14], ax      ; ax <-> return IP
mov [bp+0x12], ax       ; write back
```

Before this runs, `[bp+0x12]`/`[bp+0x14]`/`[bp+0x16]` hold (in order)
`FLAGS`/return-`IP`/return-`CS` - the values the *ordinary far `call`
into `create_task`* pushed automatically, not anything the caller
pushed on purpose. This 3-step rotation reorders them (net effect:
`[bp+0x12]`=old `IP`, `[bp+0x14]`=old `CS`, `[bp+0x16]`=old `FLAGS`),
and *that* rearranged pair is what gets saved to `[0x7C0]`/`[0x7C2]` as
the "entry point." **In plain terms: calling `create_task()` turns the
call site's own return address into a brand-new, independently-
scheduled task.** The code physically following each `create_task`
call in the source is never executed by the calling function itself -
it becomes that new task's entire body, run later (on its own stack)
whenever the scheduler picks it, exactly like Unix `fork()` returning
into the child at the same program point the parent would have
continued from.

**Verified on 2 independent call sites**, not just theorized:
- `0xE72D8` (inside an unnamed function starting `0xE7298`): the code
  right after the call (`0xE72DD: jmp 0xE7300`) is unreachable any
  other way from this function's own control flow - it can only run as
  the new task resuming later.
- `0xE6F8C` (inside another unnamed function starting `0xE6F5C`): the
  post-call code is a **self-perpetuating loop** - checks `[0x1A99]`/
  `[0x1A93]`'s high nibble (setting to `0x41` if clear) and `[0x1B76]`
  bit 7, and if that bit is still set, jumps *back* before the
  `create_task` call to fork itself again. A task that keeps re-
  spawning its own continuation each time it runs, rather than looping
  internally - a genuinely different concurrency idiom than a normal
  `while` loop, only possible because of the fork-style mechanism.

**Correction, same session, before this got written up further: it
does NOT create a new, independent task identity.** Re-reading
`create_task`'s body line by line: `bx = [0x1ACD]` (the index of the
task that is *currently running*) is read once, near the top, and
**never modified anywhere in the function** - the SP/SS save and the
ready-flag update both write to that same, current task's own slot.
There is no allocation of a new/free index anywhere in `create_task`.
So this is really a **"yield the currently-running task, saving a new
resume point for it" primitive** operating on an existing task slot,
not a spawn primitive - "create_task" is a misleading name inherited
from an earlier, less thorough session, on par with other renamed-
then-corrected cases in this project. The actual number of distinct
task *slots* is bounded by whatever `select_next_ready_task` iterates
over (documented above as a **12-entry** table) - genuinely new task
identities, if this system ever creates any beyond a fixed roster set
up at boot, must be established some other way not yet found.

**So "how many tasks exist" is answered differently than first
thought**: not 36 independent tasks, but **at most 12 task slots**
(per the already-documented 12-entry ready-state table), each of which
can yield-and-resume from any of the 35 `create_task` call sites (plus
1 `create_task_b` site) found this session - 35 different *potential
resume points in code*, not 35 different task identities. In practice
each call site is presumably only ever reached from within whichever
one task's own code path normally executes it, so there's likely still
a natural association between "this call site" and "this one task,"
just not a *mechanical* 1:1 the way originally claimed. Some call
sites are already inside named functions (`spawn_task_with_tag`,
`restart_current_task`, `comm_call_main_rom`, `mark_task_ready` x4);
most of the containing functions are still unnamed. **What each task
slot actually spends its time doing** requires tracing individual
call sites' post-call continuations - only 3 have been looked at so
far (see below). Full call-site list (physical addresses, containing
function name where known) - still useful as "every place in the code
a yield can happen," even under the corrected understanding:

```
0x09688F  spawn_task_with_tag        0x0FD16A  (unnamed, 0xFD006)
0x0F0DFA  (unnamed, 0xF0D64)         0x0FD0FF  (unnamed, 0xFD006)
0x0E6952  restart_current_task       0x0FCBA3  (unnamed, 0xFCAF6)
0x08E88F  (unnamed, 0x87F44)         0x0FD3D1  (unnamed, 0xFD2E8)
0x083B05  comm_call_main_rom         0x0FD31E  (unnamed, 0xFD2E8)
0x0FCFEC  (unnamed, 0xFCFD2)         0x0FD56D  (unnamed, 0xFD471)
0x0FF14F  (unnamed, 0xFF067)         0x0FD6BB  (unnamed, 0xFD471)
0x0FF39F  (unnamed, 0xFF067)         0x0FD73D  (unnamed, 0xFD471)
0x0FF3B9  (unnamed, 0xFF067)         0x0FEA82  (unnamed, 0xFEA2E)
0x0FED4E  (unnamed, 0xFED1D)         0x0FDF8E  (unnamed, 0xFDF88)
0x0FEBC7  (unnamed, 0xFEB40)         0x0E6F8C  (unnamed, 0xE6F5C) - traced above
0x0FF19E  (unnamed, 0xFF067)         0x0F074E  (unnamed, 0xF04A2)
0x0FCDBE  (unnamed, 0xFCCEC)         0x0F07A8  (unnamed, 0xF04A2)
0x0E72D8  (unnamed, 0xE7298) - traced above
0x0E72F3  (unnamed, 0xE7298)         0x0E9013  (unnamed, 0xE8E29)
                                     0x0E8FF5  (unnamed, 0xE8E29)
                                     0x0E7030  (unnamed, 0xE6F9E)
0x0E6C3A/0x0E6C1C/0x0E6C24/0x0E6BBE  all inside mark_task_ready
0x0E6AA0  create_task_b's sole call site, also inside mark_task_ready
```

Note several containing functions call `create_task` **more than
once** (`0xFF067` x4, `0xFD2E8` x2, `0xFD471` x3, `0xE7298` x2,
`0xE8E29` x2, `mark_task_ready` x4 across both `create_task`/
`create_task_b`) - each occurrence is a *separate* potential resume
point with its *own* distinct continuation code (per the correction
above, these are yield points a task can use, not 36 separate task
identities - the real task count is bounded by the 12-entry ready-
state table instead).

**Traced a 3rd example - `comm_call_main_rom`'s call site - and it
makes the real, common usage pattern unambiguous: `create_task` is
mostly used as a cooperative *yield point*, not a one-shot spawn.**
`comm_call_main_rom` (`0x8839D1`, comm ROM) is a real state-machine
loop: it switches to the main ROM's `DS` (`0x41`), calls `process_
gpib_command_byte` a couple of times, does an **indexed call through a
computed pointer table at `[0x72A]`** (`les di,[0x72A]; ...; lcall
es:[bx+di]`, indexed by `[0x5A4]*4` - a genuine state-dispatch jump
table, distinct from anything documented in `docs/decode-anomalies/
landing-artifacts-and-jump-tables.md` so far and worth its own look
later), checks whether the dispatched state changed, and **loops back
to the top (`jmp 0x83A04`) if not done**. Right before the bottom of
this loop, it calls `create_task` unconditionally, and the code
immediately after that call (which, in a normal function, would be
dead/unreachable, since the loop already jumps back earlier) is
exactly: switch `DS` back to the comm ROM's own segment (`0x8F80`),
then `jmp 0x83A04` - **back to the top of the very same loop**. In
other words: this function periodically forks a continuation of
*itself*, letting the scheduler run other tasks in between, then
picks its own state machine back up exactly where it left off once
rescheduled - a cooperative multitasking yield, implemented entirely
via the fork mechanism rather than a dedicated "yield" primitive.

This reframes the earlier 2 examples too, and matches the correction
above precisely: the self-perpetuating loop found at `0xE6F5C` fits
the same "yield and resume the same loop" shape. **Given `create_task`
never allocates a new task index (see the correction above), every one
of the 36 call sites is structurally the same kind of thing - the
currently-running task yielding and marking where to resume - not a
mix of "yield" vs. "spawn" cases.** What differs from call site to
call site is only what the *post-call continuation* does: sometimes
it's a tight loop back into the same function (`comm_call_main_rom`,
`0xE6F5C`), sometimes it's a short one-shot "restore DS and return
normally to my own caller" (`spawn_task_with_tag`, see below) - so the
practical *effect* ranges from "yield once, then continue exactly as
before" to "yield repeatedly forever as this task's entire purpose,"
but the underlying mechanism is identical in all cases.

**Corrected `spawn_task_with_tag` too** (`FUNCTIONS.md`'s entry
previously said it "launches a background comm-tx-servicing task" -
no longer accurate under this corrected understanding). Its actual
shape: tag the *current* task's own scratch byte, yield once via
`create_task`, and when resumed, immediately restore `DS` and return
normally to its own caller (`serial_tx_buffer_put`). So it doesn't
launch anything independent - it inserts a single deliberate
scheduling yield (presumably to let a higher-priority tick, like the
actual TX-servicing one, run) before continuing on with the same call
chain, tagged with a marker for whatever reads `[idx+0x744]` later.
