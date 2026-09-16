# Emulator design phase

Started 2026-09-16, prompted by the still-open stroke-font glyph-table
hunt (`docs/display/vector-display-and-stroke-font.md`) hitting a wall
that static disassembly can't cleanly resolve: two candidate writers of
`[0x1DB0]` were found, neither with a confirmed caller, and the
downstream HPGL-rendering transform (the "9 native Y-levels for an
8-value 3-bit field" puzzle) still doesn't match anything found by
static shape-matching. The hypothesis this project is testing: dynamic
execution (actually running the firmware) can answer these questions
directly instead of by inference - single-stepping shows every real
write to a variable, not just the ones a grep for literal operands
happens to find, and shows the actual runtime pointer value used to
index the glyph table instead of guessing between candidates.

This is a design-phase document, not implementation yet. See `TODO.md`
for whether/when this has moved to active work.

## Scope: headless tracer, not a full-system emulator

**In scope**: emulate the 8088/8086 CPU accurately enough to execute
real firmware code from reset, with every ROM/RAM access and I/O port
access observable and loggable. The deliverable is a debugging tool -
"run the firmware and tell me what it actually does" - not a usable
recreation of the oscilloscope.

**Explicitly out of scope** (for now - could be revisited later if the
in-scope tool proves valuable enough to extend):
- Real CRT/vector-display rendering. The vector-list writes get logged
  and can be fed through `disasm/decode_vector_icons.py`/
  `disasm/plot_hpgl_to_svg.py` after the fact, same as live-hardware
  captures already are - no need to build a renderer into the emulator
  itself.
- Real front-panel input (knobs, buttons, ADC values reacting to a
  simulated user). Front-panel registers get stubbed with fixed or
  scripted values, not live interaction.
- Cycle-accurate timing. Only enough timer/interrupt fidelity to keep
  the tick-driven scheduler (`docs/interrupts/task-scheduler.md`)
  making forward progress, not real wall-clock accuracy.
- Any UART/GPIB *protocol* fidelity beyond what's needed to stop the
  firmware from hanging on a read - no real serial peer.

## Why Unicorn Engine

Confirmed available: `pip install unicorn` resolves to `unicorn-2.1.4`
(win_amd64 wheel), not currently installed in this environment.
Supports `UC_ARCH_X86` / `UC_MODE_16`, which is exactly this project's
already-confirmed CPU model (real-mode 8088/8086 - see
`docs/architecture/cpu-and-language.md`). Gives per-instruction and
per-memory-access hooks needed for tracing without writing a CPU core
from scratch.

Alternative considered and rejected for now: writing a from-scratch
8086 interpreter. More control, but this project's actual need is
observability into *existing, already-understood* firmware, not CPU
research - Unicorn's hook API already provides exactly the
observability needed (`UC_HOOK_MEM_READ`/`WRITE`, `UC_HOOK_INSN` for
`IN`/`OUT`, `UC_HOOK_CODE` for single-stepping) with a mature, tested
CPU core underneath. Revisit only if Unicorn's real-mode segmentation
model turns out to not match this firmware's addressing (not expected
- real mode segment:offset is well-trodden ground for Unicorn).

## Memory map wiring

Pull directly from `MEMORY_MAP.md` - do not re-derive. Planned regions:

| Region | Backing | Access |
|---|---|---|
| `0xE0000-0xEFFFF` | `binary/160-3633-14.bin` | read-only |
| `0xF0000-0xFFFFF` | `binary/160-3532-14.bin` | read-only |
| `0x80000-0x8FFFF` | `binary/160-2998-14.bin` | read-only |
| `0x90000-0x97FFF` | same bytes as `0x88000-0x8FFFF` (confirmed genuine second ROM half per `MEMORY_MAP.md`'s 2026-09-13 correction, not an alias - map the same file's second-half bytes here directly, not a memory alias) | read-only |
| `0x00000-0x07FFF` | RAM (IVT + flat variable pool + waveform display RAM) | read-write |
| `0x08000-0x0FFFF` | RAM (waveform attribute plane) | read-write |
| `0x40000-0x4FFFF` | RAM (stack, I/O window, acquisition memory) | read-write, see I/O stub note below - most of this range is real RAM but specific addresses within it are registers, not memory (see `MEMORY_MAP.md`'s "Confirmed regions" table) |
| everything else touched at runtime | RAM, allocate on demand as unmapped-access faults surface real gaps in the map | read-write |

**Known open question to resolve during build, not before**: whether
`0x02090-0x021F0`'s far-pointer table (`ES=0x209` base,
`docs/acquisition-and-plotting/ram-far-pointer-table.md`) needs
special handling - static analysis found no code loading `ES`/`DS` =
`0x209` via literal immediate, so it's unclear whether real execution
ever reaches it this way. Emulation will settle this immediately (an
unmapped-memory fault would fire if something tries and the region
isn't backed).

## I/O port and MMIO stub strategy

Every register `MEMORY_MAP.md` already documents gets a hook that (a)
logs the access (address, direction, value, current CS:IP) and (b) on
read, returns a scripted/fixed stub value. Planned initial stub table
(all subject to revision once real hang points are observed):

| Address(es) | Register | Initial stub behavior |
|---|---|---|
| `0x406F8-FB` | Option Interrupt Mask Latch | reads return last written value (simple loopback register) |
| `0x4067C` | Option Status Latch (in) | fixed value, revise if self-test logic loops on it |
| `0x406BC` | Option Parameters Latch (in) | fixed DIP-switch pattern (can mirror a real captured value from `HARDWARE.md`) |
| `0x437F6`/`0x437FA`/`0x437FB` | Front-panel A/D control/data, main FP input | fixed "all buttons up, pots centered" pattern |
| `0x41000`/`0x42000` | Display chip int-reset/frame-trigger | reads return 0 (never "still busy") so any poll loop exits immediately |
| I/O ports `0x83`/`0xC4`/`0xD1` | plotter/shift-register writes | log-only, no read side needed (write-only in all confirmed code) |

The general rule: **start every stub as "always return the value that
lets the firmware move on," and only add real behavior (state,
sequencing, toggling) when a specific hang is observed and diagnosed**
- don't pre-build state machines for registers that turn out not to
matter for reaching the code of interest.

## Interrupt/timer strategy

The tick-driven scheduler (`docs/interrupts/task-scheduler.md`) needs
its timer interrupt to actually fire periodically or nothing beyond
the boot/init sequence ever runs. Plan: after N emulated instructions
(tunable, not wall-clock - determinism matters more than realism
here), pause emulation and synthetically invoke the installed `INT2`
handler via Unicorn's `uc.emu_start` re-entry at the handler's IVT
address, matching real INTR/NMI delivery mechanics documented in
`docs/interrupts/ivt-and-int255.md`. `INT255`'s real hardware trigger
(UART DR/TBRE/INTR lines) stays unfired unless/until a comm-ROM
investigation specifically needs it - the main-ROM boot/self-test path
doesn't depend on it.

## Milestones (design-phase estimate, revise once building starts)

1. CPU boots from `0xFFFF0`, executes without faulting on unmapped
   memory through the earliest boot code (stack setup, IVT install) -
   proves the memory map above is complete enough for the very start.
2. Reaches and executes `init_far_pointer_table_sysrom` - directly
   observe (not infer) what it writes to `[0x1DB0]`, settling the
   "does anything really write this" question with certainty instead
   of the current "found by tracing a `movsw` loop, not fully
   confirmed live" status.
3. Scheduler tick fires at least once (proves the synthetic timer
   approach works) and self-test or normal boot flow continues past
   the point any current static trace has confirmed reachable.
4. Reaches `draw_readout_char` or `FUNC_3633_E60C` (the two `[0x1DB0]`
   readers) with a real character code as input - log the actual
   physical address dereferenced. This is the concrete target that
   directly answers the open question - once reached, cross-check the
   dereferenced address's *contents* against the pen-bit vector
   encoding already confirmed in `decode_vector_icons.py` to see if it
   decodes into a plausible glyph shape.

Milestone 4 is the actual goal; 1-3 are checkpoints proving the
infrastructure is trustworthy enough to believe milestone 4's answer
once reached.

## Non-goals reminder

If this tool successfully answers the stroke-font question, resist the
temptation to grow it into a full-system emulator/simulator project
scope-creep - update `docs/display/vector-display-and-stroke-font.md`
and `STILL_PENDING_DECODE.md` with the finding and return to static
analysis for whatever's next, unless a *specific* new question makes
"run more of the firmware dynamically" the right tool again.
