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
