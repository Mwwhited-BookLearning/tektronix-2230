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
| `0x80000-0x87FFF` | `binary/160-2998-14.bin` file offset `0x0000-0x7FFF` (ROM lower half) | read-only |
| `0x88000-0x8FFFF` | **RAM**, not ROM - "Option nonvolatile RAM" per `MEMORY_MAP.md`'s confirmed table. Caught while writing this doc: an earlier draft of this table mapped the whole `0x80000-0x8FFFF` span as one flat ROM block copied from the file, exactly the mistake `MEMORY_MAP.md` itself already documents having made and corrected once (2026-09-13) - initial content is unknown real NVRAM state, zero-fill and revisit if a self-test depends on specific nonvolatile content | read-write |
| `0x90000-0x97FFF` | `binary/160-2998-14.bin` file offset `0x8000-0xFFFF` (ROM upper half - a genuine second ROM half at a different physical location, confirmed **not** an alias of `0x88000-0x8FFFF`, per `MEMORY_MAP.md`'s 2026-09-13 correction) | read-only |
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

**All 4 milestones done, 2026-09-16, with a real answer.** Milestone
4's dereferenced address turned out to be incoherent garbage (per
`docs/display/vector-display-and-stroke-font.md`'s "Live emulation
confirms..." section) - but chasing *why* led to the actual resolution:
`draw_readout_char` is conditionally dead code whenever the comm option
is detected as installed, which both of this project's real physical
test units are. `io_stubs.py` (a new module, see its docstring and
`design.md`'s "I/O port and MMIO stub strategy" above) stubs the
comm-presence hardware probe to confirm this directly - see
`changes/2026-09-16.md` for the full story. The emulator remains active
infrastructure for whatever's next (the still-open HPGL transform
puzzle is the leading candidate), not a one-shot tool that's now done.

## Findings and gotchas from the first implementation pass, 2026-09-16

Moved past design-only once Unicorn's install was confirmed working -
`memory_map.py`/`timer.py`/`emu.py` now exist and boot the real
firmware from `0xFFFF0`. Milestone 1 (boots, runs clean) and milestone
3 (synthetic scheduler tick) are both working; 3 real, non-obvious
problems were found and fixed along the way, each worth remembering
before touching this code again:

1. **`0x88000-0x8FFFF` is RAM, not a ROM continuation.** Caught while
   writing this very design doc's memory-map table, before any code
   was even run - an earlier draft mapped the whole `0x80000-0x8FFFF`
   span as one flat 64KB ROM block copied from the file, exactly the
   mistake `MEMORY_MAP.md` itself documents already having made and
   corrected once. Fixed before implementation, not found by testing -
   worth noting as a case where re-reading the source-of-truth doc one
   more time before coding caught it for free.

2. **INT 2 is NMI, not a normal maskable interrupt - it must never be
   gated on the `IF` flag.** `docs/interrupts/ivt-and-int255.md`
   already states this ("the NMI vector is at 00008" = physical `0x8`
   = IVT slot 2), but the first `timer.py` draft still checked `IF`
   before firing anyway. Result: 100% of ticks silently skipped for
   the entire run (confirmed: 10000/10000 skipped over 20M
   instructions) because the firmware hadn't executed `sti` yet at the
   point ticks were being attempted - `wait_readout_tick` never
   unblocked. Fixed by not respecting `IF` specifically for `int_num
   == 2`.

3. **A genuine 8086 wraps addresses above 1MB back to the bottom of
   memory (no A20 gate); Unicorn's x86 core does not do this
   automatically.** A segment register near `0xFFFF` combined with a
   large offset computes a physical address up to `0x10FFEF` - on real
   hardware this is silently the *same byte* as `physaddr & 0xFFFFF`
   (20 address lines only), but Unicorn will fault it as genuinely
   unmapped memory if nothing is mapped there. Confirmed by testing:
   hit `UC_ERR_READ_UNMAPPED` at physical `0x104A21`, whose
   low-20-bit-masked equivalent `0x4A21` sits cleanly inside the
   low-RAM region - not a real gap in the memory map, a missing CPU
   behavior. Fixed with a true zero-overhead alias: `mem_map_ptr()` the
   low-RAM region's own host buffer a second time at `0x100000` (size
   `0x10000`, covering every address the 16-bit real-mode segment:
   offset arithmetic can ever compute above 1MB) instead of a
   read/write hook pair.

4. **A naive "is the IVT slot nonzero yet" check for "has the real
   vector been installed" fires on transient garbage, not the real
   handler.** The boot-time RAM-clear/pattern-test loop (physical
   `0xE0161`, the very first thing milestone 1's boot trace found
   spinning on) writes non-zero test patterns across low memory - IVT
   included - before the real interrupt vectors get installed. Firing
   a synthetic tick during that window jumps to whatever pattern
   happened to be sitting in the vector slot at that instant (confirmed
   by testing: crashed at CS:IP `F2F3:F4F5`, an obvious fill pattern,
   not real code). Fixed by gating the first tick on an exact byte
   match against `INT2_HANDLER_EARLY`'s confirmed installed address
   (`E5D1:0057`, from `docs/interrupts/ivt-and-int255.md`) rather than
   "nonzero."

**Status after these 4 fixes**: boots clean, the RAM-clear/pattern-test
loop at `0xE0161` completes, `INT2_HANDLER_EARLY` gets installed and
confirmed, the synthetic tick fires (33 times before the next issue,
each one correctly incrementing `[0x752]` and unblocking
`wait_readout_tick`), and execution reaches new code past that point -
milestone 3 is genuinely working. **Next real blocker found**: an
unmapped read at physical `0x0C69B9` from `les di, [bp-0xc]` /
`mov dl, byte [es:di]` (physical `0xE38ED`) - a far pointer read from a
*stack-local variable*, not a static table, so the bad value must be
flowing in from whatever set up that stack frame. **Tested and revised**: tried tick intervals of 2000/20000/100000/
500000 instructions - the crash happens at the exact same address
(`0x0C69B9`) every time, and the tick counter reads exactly **33
fired** in every single run regardless of interval. That rules out
"the tick landed at an unlucky instruction-count moment" (a timing-
noise theory, which interval changes would have perturbed) - this is
tied to the tick *count*, not raw instruction timing. Strong new
hypothesis: real scheduler/task-dispatch logic keyed off `[0x752]`
(or a derived counter) selects a different code path once the tick
count crosses some threshold near 33, landing on a path this cold
boot never legitimately reaches because real firmware likely completes
more of its own initialization before interrupts are this far along -
i.e. this emulator is running the *tick-driven* logic correctly but
too early relative to some *other*, not-yet-modeled setup step. Next
session: instrument what's read from/written to `[0x752]` and any
tick-count-derived index (`% N`-style) rather than continuing to
adjust the tick interval, which testing now shows isn't the relevant
variable.

## Stub expansion and first full self-test pass, 2026-09-16

Prompted directly: "update the emulator with what you know now and see
if it gets further." A lot of new hardware knowledge had accumulated
since the emulator was built (the service manual's Table 3-1 clean
re-scan, the RS-232 option board's schematic-traced chip identities and
bit maps, live-hardware-captured register values from the exerciser
screen photos) that had never been fed back into `io_stubs.py` - only
`CommPresenceProbe` existed, and design.md's own original stub-strategy
table (display chip int-reset/frame-trigger, comm status/parameter
latches, front-panel registers) had been *planned* but never actually
built.

**Built**: `io_stubs.FixedByteRead`, a generic "force every read of
this address to a fixed value" hook (confirmed with a standalone test
first: writing inside a `UC_HOOK_MEM_READ` callback does override the
value the *current* instruction sees, not just future ones - necessary
since plain RAM's "read back whatever was last written" is wrong for a
register real hardware clears/drives independently of the CPU). Applied
it to 8 registers, 6 of them backed by a **real captured value** rather
than a guess: `comm_stat`=`0x7D`, `comm_param`=`0xF8`, `fp_ad_data`
=`0x62`, `fp_intstat`=`0x9B`, `SWB2`=`0x08`, `SWB1`=`0x44` (all from the
`/DIAGNOSTICS/EXERCISERS/IO/INPUT_PORTS` photos or `VARIABLES.md`'s
live-hardware baseline captures - see `MEMORY_MAP.md`), plus the 2
display-chip registers forced to `0` per the original design-doc plan.
Also made `io_stubs.DiagnosticTextCapture` (hooking `write_readout_
port_byte`'s actual byte-move instruction, physical `0xE0B63`) a
permanent, reusable module instead of the one-off scratch code used for
the earlier stroke-font investigation - this is what makes it possible
to actually *see* self-test progress during a long run instead of just
an instruction count.

**Result: real, new forward progress.** With `--comm-installed
--stub-registers --show-diag-text`, the emulator now runs the **entire
power-up self-test sequence to completion** for the first time -
`'2230/2220 Power up tests complete.'` actually prints, instead of the
run just continuing silently past whatever static analysis had already
confirmed reachable. Full captured sequence:

```
2230/2220 boot : 160-3633-14
POWER UP FAILURES
PRESS MENU KEYS TO CONTINUE
PU : ROM/RAM/NMI :  0003
MI : Display controller : TIMEOUT
ACQ_AB : read-back     0 <>     2
ACQ_AB : read-back     0 <>     6
ACQ_AB : read-back     0 <>     E
ACQ_AB : read-back     0 <>    1E
ACQ_AB : read-back     0 <>    3E
ACQ_AB : read-back     0 <>    7E
ACQ_AB : read-back     0 <>    FE
ACQ_AB : read-back     0 <>   1FE
ACQ_AB : read-back     0 <>   3FE
ACQ_AB : read-back     0 <>   7FE
ACQ_AB : read-back     0 <>   FFE
2230/2220 Power up tests complete.
```

**Two remaining failure classes, both newly diagnosed (not fixed)**:

1. **`Display controller : TIMEOUT`** (`selftest_display_irq_idle`,
   `0xE3F2C` - see `FUNCTIONS.md`): traced to a literal `sti`/`cli`
   pair (`enable_interrupts`/`disable_interrupts`, `0xE5D2D`/`0xE5D2F`)
   with nothing in between - a "briefly enable interrupts, see if a
   level-triggered line is already stuck asserted" hardware test.
   Fails if `[0x1AEE]` is nonzero afterward. **Tested and confirmed
   deterministic, not a synthetic-ticker timing artifact**: reran at 4
   very different tick intervals (500/2000/3000/7777 instructions) and
   got byte-identical output every time - if the emulator's own
   INT2/NMI ticker were coincidentally landing inside that 1-2
   instruction window, changing the interval this much should have
   shifted the result. It didn't, so something real is asserting an
   interrupt-pending condition at this exact point in boot that this
   emulator doesn't yet model. Next step: trace what actually sets
   `[0x1AEE]` (which interrupt vector, and why it's pending here) -
   this needs real investigation, not another stub guess.
2. **`ACQ_AB` read-back failures**: the sequence `2, 6, E, 1E, 3E, 7E,
   FE, 1FE, 3FE, 7FE, FFE` is a textbook **address-line walking test**
   (one more bit shifted in on each failure) - almost certainly
   exercising the Acquisition Memory Address Buffer (`0x4377E`/
   `0x4377F`, `U3427`/`U3428`) and/or the acquisition RAM's real
   address decode. See `MEMORY_MAP.md`'s updated entry for this
   register. Would need a real write-then-readback coupling stub (like
   `CommPresenceProbe`'s, but wired to the actual `0x48000-0x4BFFF`
   acquisition RAM or a dedicated readback path) to pass - not built
   yet, since the real coupling mechanism isn't traced.

Both are genuine "not simulatable with a flat value" cases, exactly the
kind design.md's own stub philosophy anticipates ("only add real
behavior when a specific hang is observed and diagnosed") - correctly
distinguishing "needs a fixed value" from "needs real modeled coupling"
is itself useful progress, not a dead end.

## Interactive REPL debugger, 2026-09-16

User request: "coudl I get an option to run it with an interactive
interface? somethign that shows the registers and current processing
and allows simulation of pushing buttons or sending a serial message?"
This directly reopens 2 things design.md's original scope explicitly
called out of scope ("Real front-panel input... stubbed with fixed or
scripted values, not live interaction" and no UART protocol fidelity) -
noted, but the user's call to make, not a mistake to correct.

Built `interactive.py`: a plain REPL (not a curses/rich TUI - user
confirmed "a repl would be good enough" when asked, avoiding a new
dependency and Windows-terminal-compatibility risk) on the same
`memory_map.py`/`io_stubs.py` setup as `emu.py`. Shows CS:IP (with the
physical address), all general registers, decoded flags, the current
instruction (via capstone), and the front-panel/UART mock status after
every stop. Commands: `step`/`run`/`continue`/`break`/`delete`/`mem`/
`regs`, plus the 2 new interactive pieces:

- **`press`/`release` + `io_stubs.InteractiveFrontPanel`**: a live,
  mutable version of the `SWB1`/`SWB2` registers (`FixedByteRead`'s
  constant stubs are for the headless tracer only now - split into
  `FRONT_PANEL_BUTTON_STUBS` vs. the always-fixed `FRONT_PANEL_STUBS`).
  Bit maps and polarity straight from `VARIABLES.md`'s live-hardware-
  confirmed `[0x758]`/`[0x759]` entries, starting at the same real
  captured idle baselines so an interactive run with nothing pressed
  behaves identically to the headless default.
- **`serial` + `io_stubs.InteractiveUartMock`**: explicitly labeled
  experimental in its own docstring - this project has never confirmed
  which of the 8 "Option UART/GPIB chips" registers is genuinely the
  receive-data register, nor exercised the comm ROM's real byte-
  reception path in any emulator run. Picked `0x406F0` (the same
  address `write_readout_port_byte` already writes diagnostic text to)
  as a plausible scaffold and Option Status Latch bit1 ("UART INTR+DR")
  as the ready flag - good enough to experiment with, not a validated
  register map. Installed *after* `COMM_OPTION_STUBS`'s fixed
  `comm_stat` hook so it only touches bit1 on top of that baseline
  (Unicorn calls same-address read hooks in installation order).

**Verified working**: stepping, breakpoints (hit exactly at the
already-known `0xE0161` RAM-clear loop on the first try), button
press/release correctly toggling `SWB2`/`SWB1`, serial injection
queuing bytes, and a full `run` reproducing the exact same power-up
self-test sequence `emu.py --stub-registers` already gets - same
underlying stubs, same result, just interactively inspectable now.

**Follow-up same day, 3 more real fixes from direct user testing**:

1. **Confirmed empirically, not assumed**: after the fixed power-up
   sequence completes and execution settles into the early scheduler-
   tick loop, injected `serial` bytes sit in the queue completely
   untouched even after 30M further instructions - nothing in the
   currently-reached code path reads the mock RX register at all. This
   matches the mock's own documented uncertainty (the comm ROM's real
   receive path has never been confirmed reached) - a genuine, useful
   negative result, not a bug.
2. **Added `trace on`/`trace off`** (user: "could I have a debug view
   that scrolls all the registers as well as the current data value?")
   - streams one compact line per executed instruction (all registers,
   flags, the decoded instruction, and - genuinely useful for anything
   touching a table via `[bp+N]`/`[si]`/`es:[di]` - the live value at
   the instruction's memory operand, resolved via capstone's detail
   mode). Caught and fixed a real bug immediately: capstone exposes an
   x86 memory operand's segment-override register as `op.mem.segment`,
   not `insn.segment` (`AttributeError: segment` on first real test -
   the difference matters because a segment override belongs to a
   specific *operand*, not the instruction as a whole, when more than
   one operand could theoretically have one).
3. **Fixed a real escape-handling bug** (user: "how do I append \n to
   the request so it receives a newline?"): the `serial` command was
   running every line through `shlex.split()` first, which treats
   backslash as its own escape character outside quotes - a literal
   `serial hello\n` typed at the prompt had its backslash silently
   eaten by `shlex` before `decode_escapes` ever saw it (confirmed via
   a real test: `serial hello\n` queued only 5 bytes, not 6). Fixed by
   special-casing `serial` to bypass `shlex` entirely and take the raw
   line remainder, then applying `decode_escapes` (`\n`/`\r`/`\t`/`\0`/
   `\\`, `printf -e`-style) directly. Also caught a second bug in the
   same area during testing: the raw-text extraction originally called
   `.strip()`, which silently eats a genuine trailing `\r`/whitespace
   the user actually wanted sent - fixed to rely only on `str.split`'s
   own leading-whitespace skip.
4. **Added `outgoing`/`uart` visibility** (user: "I would like to see
   the incoming messages and outgoing messages"): `InteractiveUartMock`
   now also hooks `UC_HOOK_MEM_WRITE` on the same data address (not
   just the existing read hook), independent of which specific calling
   instruction writes there - broader coverage than `DiagnosticText
   Capture`'s single-instruction hook, in case the comm ROM ever writes
   the same address through a different code path. Also prints
   `[SERIAL RX] ...` live whenever a queued byte is actually consumed,
   so a real RX event (once one ever happens) is immediately visible,
   not just inferable from the queue count changing.
5. **Flipped `--comm-installed`'s default to on** in both `emu.py` and
   `interactive.py` (user: "is it configured to assume the comm module
   is installed or do I always have to send in --comm-installed?") -
   both of this project's real physical test units genuinely are
   Option 12/RS-232 equipped, and the "not installed" stub is already
   confirmed to eventually crash, so defaulting to the *wrong* config
   was pure friction. `--stub-registers` flipped to on-by-default too,
   for the same reason (it's evidence-based, not speculative). Both
   still support an explicit opt-out (`--no-comm-installed`, `--no-
   stub-registers`, via `argparse.BooleanOptionalAction`) for
   comparison runs against the "worse" baseline.
6. **Made `continue`'s run length configurable** (user: "I would like
   to be able to set the continue length... and I'd like to start by
   cutting the default in half") - added `--continue-length` (default
   `25000000`, half the previous hardcoded `50000000`); `continue <n>`
   at the prompt also updates the stored length for the rest of the
   session, not just that one call.

## Non-goals reminder

If this tool successfully answers the stroke-font question, resist the
temptation to grow it into a full-system emulator/simulator project
scope-creep - update `docs/display/vector-display-and-stroke-font.md`
and `STILL_PENDING_DECODE.md` with the finding and return to static
analysis for whatever's next, unless a *specific* new question makes
"run more of the firmware dynamically" the right tool again.
