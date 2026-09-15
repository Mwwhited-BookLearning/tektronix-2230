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

**This concretely answers "how many tasks exist"**: at least the 35
distinct `create_task` call sites (plus 1 `create_task_b` site) found
this session, each a genuine, independent point where a new task gets
spun up. Some are already inside named functions (`spawn_task_with_
tag`, `restart_current_task`, `comm_call_main_rom`, `mark_task_ready`
x4); most of the containing functions are still unnamed. **What each
one specifically does** requires tracing its own post-call
continuation individually - only the 2 above have been looked at.
Full caller list (physical addresses, containing function name where
known):

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
`create_task_b`) - each occurrence is a *separate* fork point with its
*own* distinct continuation code, so the true task count (by distinct
entry point) is exactly 36, not "36 calls to a handful of tasks."
