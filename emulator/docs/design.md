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
8. **Trace coloring, outgoing-buffer clear, real CRLF rendering**
   (left directly in `TODO.md` as "From the Architect" notes): added
   `io_stubs.ANSI_GRAY`/`ANSI_WHITE`/`ANSI_RESET`, applied gray to
   `trace`'s scrolling lines and white to `[DIAG TEXT]`/`[SERIAL RX]`
   so the two visually separate when both stream at once; added
   `outgoing clear`; fixed `InteractiveUartMock.outgoing_text()` to
   pass real `\r`/`\n`/tab bytes through as actual control characters
   instead of the `\xNN` escape form (only genuinely unprintable bytes
   still get escaped) - confirmed with `cat -v` that both the color
   codes and the real line breaks render as intended.
9. **Tested `serial` against the real, confirmed live-hardware answer,
   and made the RX path interrupt-driven** (user: "test the serial
   operations yourself... when ID?\n is sent in it should return the
   same messages you were getting from the real hardware tests
   before"). `docs/comm-rom/rs232-breakthrough.md` has the confirmed
   real answer: `ID?` -> `ID TEK/2230,V81.1,VERS:14;`. Injected
   `ID?\n` and ran 40M+ instructions past self-test completion -
   **the real response never appears**, confirmed by grepping the
   captured outgoing text for `ID TEK`/`VERS:14` (zero matches).

   User's follow-up diagnosis, and it was right: "serial should
   probably be setting the interrupt that is used on the UART" - a
   passive status-bit stub can never trigger code that's genuinely
   interrupt-driven rather than polled. `InteractiveUartMock.inject()`
   now also tries to fire `INT255` (via `timer.fire_interrupt`, the
   same mechanism `TickScheduler` already uses for `INT2`), gated on
   the Option Interrupt Mask Latch's `0D`/DR output (`0x406F8`) being
   nonzero. Rerunning with this in place gave a **precise diagnosis
   instead of a vague negative result**: `0x406F8` is still `0`
   (masked) even 40M instructions in - the firmware genuinely hasn't
   reached whatever point in its own init sequence unmasks RS-232
   receive interrupts, matching the documented real-hardware behavior
   ("forced LO by BRST... masked until firmware explicitly unmasks
   it"). **Caveat honestly documented in `InteractiveUartMock`'s own
   docstring**: the exact bit-level write convention for that mask
   output wasn't re-traced in this pass (`set_comm_queue_busy` writes
   it through a RAM-resident far pointer, `[0x6E2]`, whose own
   initialization wasn't tracked down here) - "nonzero byte" is a
   reasonable approximation, not a re-confirmed bit convention.

   Also added `incoming` (shows the RX queue's actual byte contents,
   matching `outgoing`'s rendering) per a follow-up request to see
   what's queued, not just how much.

   **Real next step this surfaces**: find what code path actually
   writes nonzero to `0x406F8`, and whether the emulator can be
   steered there directly (a specific self-test branch? a comm-menu
   action?) rather than just running the boot trace longer - a much
   more specific target than "run more instructions and hope."

## Textual TUI + shared debugger core + venv-aware launchers, 2026-09-16

User asked whether a Python TUI would work on both Windows and Linux
before requesting one, then confirmed wanting it as an additional
option alongside the REPL (not a replacement), plus venv-aware
launchers so packages don't need manual installation.

**Answered the portability question with a real distinction, not a
flat yes**: plain `curses` needs a separate `windows-curses` shim on
Windows and still has rough edges there; **Textual** has its own
native Windows Terminal/PowerShell driver, genuinely no
platform-specific code needed. Chose Textual over `prompt_toolkit` for
its purpose-built live-dashboard widgets (`RichLog`, reactive panels).

**Refactored first, before writing any Textual code**: extracted the
entire CPU/memory/stub setup, stepping, register/instruction
formatting, and command dispatch out of `interactive.py` into a new
`debugger_core.py` - the `Debugger` class, `dispatch_command()`
(the single source of truth for what every command does, returning
response lines rather than printing directly), and `HELP`. Both
`interactive.py` and `tui.py` now import from here; `interactive.py`
shrank to just its REPL loop as a result. This was worth doing *before*
the TUI, not after - writing the TUI against the same tested engine the
REPL already uses avoids two copies of the command logic drifting
apart the first time either one gets a bug fix.

**One real design problem solved**: `io_stubs.DiagnosticTextCapture`/
`InteractiveUartMock` used to call `print()` directly for their live
`[DIAG TEXT]`/`[SERIAL ...]` messages - fine for the REPL, but a
Textual app fully owns the terminal, so a stray direct `print()` during
a run would corrupt the display. Both classes now take a `sink=print`
callable instead, and `Debugger.__init__` forwards its own `output`
parameter into them - so the *same* stub classes work unmodified under
either front end, each just supplying a different sink (`print` for
the REPL, a widget-append function for the TUI).

**A second real problem, and its fix**: `dbg.step()`/`dbg.run()` are
synchronous and can run for tens of millions of instructions
(`continue`) - calling them directly from a Textual event handler would
freeze the whole UI. Every command (not just step/run - all of them,
for one uniform threading model) now runs inside `self.run_worker(...,
thread=True)`, and the `output` sink passed to `Debugger` always goes
through `self.call_from_thread(...)` to get back onto the app's own
thread safely.

**A pleasant surprise**: Rich's `Text.from_ansi()` parses the exact raw
ANSI escape codes `io_stubs.ANSI_GRAY`/`ANSI_WHITE` already produce
into proper styled `Text` objects - the TUI's log reuses the REPL's
existing gray/white coloring scheme verbatim, no parallel Rich-markup
color scheme needed.

**Verified working headless, before ever opening a real terminal**:
Textual ships `App.run_test()` specifically for this - drove the app
with a `Pilot` (typing `step 5`, `serial ID?\n`, `incoming` into the
input, pressing Enter/F1) and confirmed the register count advanced
correctly, the interrupt-masking diagnostic printed with correct white
styling (`Style(color=Color('color(15)'...`, confirming the ANSI
parsing worked), and `incoming` showed the queued text with its real
trailing newline intact - all identical to the REPL's already-verified
behavior for the same command sequence.

**venv-aware launchers**: `interactive.bat` and `tui.bat` now create
(if missing) and use a venv at `emulator\.venv`, installing
`requirements.txt` into it before running - `.venv/` added to the root
`.gitignore`. Both scripts share the same venv and requirements file,
so either one being run first sets it up for both.

## Ported a real i8251 USART core from MAME, 2026-09-16

User request: "port MAME's i8251.cpp so you can interface it with
unicorn and get this serial working correctly" - prompted directly by
`InteractiveUartMock`'s hand-approximated status-bit model being
exactly the kind of register-level detail worth getting from a real,
tested reference instead of reconstructing from memory.

**Fetched the actual source rather than recalling it**:
`raw.githubusercontent.com/mamedev/mame/master/src/devices/machine/
i8251.{h,cpp}` (BSD-3-Clause, copyright-holders smf/Robbbert). New
`emulator/i8251.py` ports the register-level state machine faithfully
- the Mode/Sync1/Sync2/Command write-sequencing (`control_w`'s
dispatch on `m_flags`), the exact status bits and what sets/clears
each one (`TX_READY`/`RX_READY`/`TX_EMPTY`/`OVERRUN_ERROR`/etc.), and
RxRDY/TxRDY's masking by the chip's own Receive/Transmit-Enable
command bits (`rxrdy_r()`/`txrdy_r()`) - verified line-by-line against
the fetched source, not reconstructed from general USART knowledge.

**Deliberately left out**: MAME's bit-by-bit `receive_clock`/
`transmit_clock`/`sync1_rxc`/`sync2_rxc` machinery, which simulates a
real serial line one bit at a time against TxC/RxC clock signals -
irrelevant here since this emulator only ever injects/observes whole
bytes (`serial <text>`), never individual RS-232 bits. `receive_byte()`
goes straight to what MAME's `receive_character()` does (a complete
byte has arrived); `data_w()` (TX) completes "instantly" rather than
being paced by a transmit clock, matching how `write_readout_port_
byte`'s diagnostic-text writes already pace themselves via the
firmware's own generic tick busy-wait, not chip timing.

**Unit-tested the port standalone before any Unicorn integration**:
mode/command programming, RxRDY going high on `receive_byte()` and
clearing on `data_r()`, overrun detection (second byte before the
first is read), and TX byte capture - all verified against expected
values matching the MAME source's documented behavior.

**Rewired `InteractiveUartMock` around the real chip**, with a more
precise register map than the old approximation: `0x406F0`=Data,
`0x406F1`=Control/Status (the standard 8251-family `BA0` convention,
matching this project's confirmed `BA0`->`A0` UART wiring) - making
`write_readout_port_byte`'s already-confirmed `0x406F0` diagnostic-text
writes a genuine, literal UART TX data write under this model, not a
coincidental address overlap. RxRDY/TxRDY are also mirrored into the
Option Status Latch's `BD1`/`BD2` bits (`"UART INTR+DR"`/`"UART TBRE"`
per the schematic-traced bit map), on top of the existing fixed
baseline. `INT255` still fires on RxRDY going high, still gated on the
Interrupt Mask Latch's `0D` output - same mechanism as before, now
driven by a real chip model instead of a hand-rolled approximation.

**Result, tested end to end**: the full power-up self-test sequence
still completes identically through the new TX path (590 bytes,
byte-for-byte the same diagnostic text). Re-injecting `ID?\n` after 40M
instructions past self-test now gives a *more complete* diagnosis than
before: not only is the board-level Interrupt Mask Latch still `0`
(masked), the chip's own Receive-Enable command bit has never been set
either - `rxrdy_r()` correctly stays `0` regardless of mask-latch
state, so no interrupt is even attempted. Two independent, real
hardware-level confirmations (chip-level and board-level) that
firmware genuinely hasn't reached RS-232 initialization in this trace,
rather than one approximate signal. Verified via both the REPL and a
headless Textual `App.run_test()` run of the TUI - identical behavior
in both front ends, as expected from the shared `debugger_core.py`.

## Fixed a real TUI crash, added a live outgoing-serial panel, 2026-09-16

User report: pressing F4 (`continue`) broke out of the TUI entirely,
leaving the terminal printing raw escape codes to the screen (a
screenshot showed `^[[<35;NN;NNM`-style sequences - SGR mouse-tracking
reports being echoed as literal text). **Root cause found, not
guessed**: every `run_worker(...)` call used `exclusive=True`, which
tells Textual to *cancel* the in-flight worker when a new one starts.
`dbg.run(25_000_000)` is a long synchronous Unicorn C call with no safe
way to be interrupted from another thread - forcibly cancelling a
thread stuck inside it can corrupt the interpreter badly enough to
skip Textual's own terminal-restoration cleanup (mouse tracking/
alternate-screen mode never get turned back off), exactly matching the
reported symptom. Almost certainly triggered by pressing F4 again (or
any other worker-launching key/Enter) before the first `continue`
finished.

**Fix**: replaced `exclusive=True` everywhere with a `_busy` flag and
a single `_launch()` choke point - a second command is refused (with a
visible message) rather than cancelling the first, since cancelling
isn't actually safe here. Also disables the input and shows a "running
..." indicator while busy, so a long `continue` looks like it's working
rather than looking frozen (which is likely what prompted the repeated
key presses in the first place). `quit` is refused the same way while
busy, since Python's thread-pool executor would otherwise just block
process exit waiting for the same uncancellable thread.

**Verified the exact crash scenario is now safe**: a headless test
pressed F4, then F4 five more times in rapid succession while the
first run was still active - confirmed refused every time (`_busy`
stayed `True`, no crash), then confirmed a new `continue` was correctly
allowed to start once the first one actually finished.

**Also added a dedicated live outgoing-serial panel** (user: "I'd also
like a dedicated section added to the TUI that will automatically
display the outbound serial data") - `io_stubs.InteractiveUartMock`
gained an `on_tx` callback (parallel to the existing `sink`, but for
raw per-byte TX events rather than log lines), threaded through
`Debugger`'s constructor. The TUI's new `#outgoing` panel buffers bytes
into complete lines before writing (the same convention `Diagnostic
TextCapture` already uses for the main log - writing per-byte would
put one character per line instead of readable text) and updates live
during any run, not just on request via the `outgoing` command.
Verified via a headless test: the panel populated with the real POST
banner text (`'2230/2220 boot : 160-3633-14'`, etc.) during a live run.

**Also clarified, from a user question**: "does that UART have a FIFO
built in?" - no. The 8251/82C52 family has exactly two 1-byte holding
registers (RX and TX), no FIFO (later chips like the 16550 added one
specifically to fix this) - confirmed directly from the ported MAME
source. What looked like "only one character got read" after
injecting `ID?\n` is exactly this: the mock's own software staging
queue (`InteractiveUartMock.queue`) feeds the chip's single RX holding
register one byte at a time as each prior byte is read out - the first
byte (`'I'`) was genuinely loaded into the chip (confirmed via direct
inspection: `chip.rx_data`, `RX_READY` set), the rest waits safely in
the queue, not lost. It stays there because nothing in the current
boot trace ever reads it (same masked-interrupt/unset-RxEN finding as
before) - accurate modeling of real hardware, not a bug.

## Paced serial delivery, and a serious TUI focus bug found while testing it

User hypothesis, directly following the FIFO explanation above: "this
is probably read in by interrupts which is what could cause the 9600
to not operate correctly... to make this work you need to take the
serial input and send it one character at a time into the interrupt
through the 8251/82C52." Exactly right, and it exposed a real gap:
before this, a freed RX holding register was refilled **instantly**
the moment the CPU read it - no time/instruction delay stood in for
the real per-byte serial arrival interval, so overrun could never
actually happen here regardless of how slowly firmware serviced bytes,
making the 9600-vs-1200-baud reliability difference this project's own
live-hardware testing found (`docs/comm-rom/rs232-breakthrough.md`)
impossible to explore.

**Fix**: `InteractiveUartMock.pump_paced(current_count)`, called once
per emulated instruction from `Debugger._on_code` (the same pattern
already used for `TickScheduler`/`trace`), delivers the next queued
byte once enough instructions have elapsed since the last delivery -
and does so **unconditionally**, not gated on the RX register being
free, since real hardware doesn't wait for the CPU's convenience
either. `self.chip.receive_byte` already implements the resulting
overrun exactly right (ported from MAME) if the previous byte wasn't
read in time. The pacing interval (`instructions_per_byte`, default
50000, deliberately not claimed to be baud-rate-calibrated since this
project hasn't confirmed the real CPU clock precisely enough for that)
is adjustable live via `serial-rate <n>` for exploring the hypothesis.
**Verified both directions standalone**: tight pacing with no read in
between correctly produces overrun (second byte overwrites the first,
`OVERRUN_ERROR` set); realistic pacing with a read between deliveries
produces none.

**While testing this through the TUI, found and fixed a much more
serious, unrelated bug**: after literally any single command
completed, every subsequent command silently did nothing at all - no
error, no log output, `_busy` never even flipped back to `True` for
the second command. Traced it (not guessed at it) via a headless test
printing `app.focused` after the first command: **`None`**. Disabling
the `Input` widget for the busy-guard (added earlier this session)
also strips its focus in Textual, and nothing was restoring it once
the widget was re-enabled - so the terminal's Enter key was reaching
no focused widget at all. **This affected every TUI session beyond its
very first command**, not just a `continue`-specific edge case - a far
more serious regression than the crash it was originally protecting
against. Fixed with one line (`inp.focus()` inside `_set_busy(False)`);
verified with a 5-command sequential session (`step`, `serial`,
`incoming`, `uart`, `regs`) all producing correct output in order.

## Live-updating registers, an interrupts/mask panel, and real hardware ground truth, 2026-09-17

User request: "I would like the registers and so on updated as the
system runs in the emulator so you can see the running values." Added
`Debugger.on_progress` - a callback fired periodically (wall-clock
throttled to 100ms, checked only every 1000 instructions so the check
itself doesn't add per-instruction overhead) during any step/run/
continue. `tui.py` wires this to `refresh_registers()`, so the panel
now updates live during a long `continue` instead of only once it
stops - verified via a headless test confirming the rendered widget
text (not just the internal counter) changes over time during a run.

**Then**: "add the interrupts and their masks to that area." Added
`Debugger.interrupts_status()`/`_interrupts_line()` - the CPU's IF
flag, the Option Interrupt Mask Latch's 4 outputs (`0D`-`3D`, read
live from `0x406F8`-`0x406FB`), the UART's own Rx/TxEN command bits
and resulting RxRDY/TxRDY pin signals, and the synthetic INT2 ticker's
fired/skipped counts - shown in both the REPL's `regs`/`interrupts`
commands and the TUI panel continuously. **Then**: "the TX queue
should be shows as well" - relabeled the UART status line for RX/TX
symmetry (`RX queue: N pending` / `TX queue: N sent`, the TX side
having no real backlog concept since `data_w()` completes "instantly"
- see `i8251.py`) and added explicit `RX_READY`/`TX_READY`/`TX_EMPTY`
status-bit display alongside the pin-level RxRDY/TxRDY signals.

**Real hardware ground truth, same day**: user connected a real scope
to COM3 and asked to see what the real device does. Probed with
`pyserial` across several baud rates to find the current DIP-switch
setting; user confirmed **4800 baud, 8N1, DTR+RTS**. Got a completely
clean, live-reconfirmed response set matching `docs/comm-rom/rs232-
breakthrough.md`'s protocol exactly: `ID?` -> `ID TEK/2230,V81.1,
VERS:14;`, `EVEnt?` -> `EVENT 0;`, `SET?` -> the full settings dump -
also newly confirming 4800 baud (not just the previously-documented
1200) works reliably with hardware flow control asserted. This is now
the live, freshly-verified target the emulator work is aiming to
reproduce, once the RS-232 interrupt path is actually reachable.

**User confirmed the plan going forward**: "yes, I want you to fix the
emulator... if you step the code and see an address or io that isn't
functioning that you can't figure out, let me know and I will check
the hardware from the service manual" - establishing the collaborative
loop for the NVRAM pre-seeding fix and whatever else surfaces next.

## The NVRAM pre-seeding fix, and verifying it the right way

Implemented the fix proposed above: `io_stubs.seed_comm_nvram_defaults()`
writes the plausible factory-calibrated far pointer (`0x4000:0x06F8`,
matching this project's own established `ES=0x4000` I/O-window
convention plus the already-confirmed `0x406F8` Interrupt Mask Latch
address) into both `[0x6D6]`/`[0x6E2]` (physical `0x8FED6`/`0x8FEE2`)
at startup, in both `Debugger._setup_memory()` and `emu.py`'s own
setup. **Flagged clearly in the function's own docstring as the single
least-confirmed assumption in this fix** - the value is inferred, not
read from a real captured NVRAM dump - per the user's own instruction
to flag anything uncertain rather than guess silently.

**Verifying this needed a real forced-call test, not just re-running
the boot trace** - the natural cold-boot path still doesn't reach
`set_comm_queue_busy` (confirmed separately, unrelated to this fix), so
proving the fix actually works meant manually constructing a far-call
stack frame and jumping straight to it. **Got the calling convention
wrong on the first attempt** - pushed the return CS/IP and the
parameter in the wrong order relative to what `[bp+6]` needs (the
parameter must be pushed *first*, ending up at the *highest* stack
offset, with the CALL FAR-pushed CS/IP landing at bp+4/bp+2 in that
order) - caught immediately because execution landed at a nonsense
`BEEF:0000` instead of the expected dummy return address, a clear
signal the stack frame was wrong rather than the fix. **Also forgot to
set `DS`** to the comm ROM's own flat-variable segment (`0x8F80`) on
the first pass, which would have made `[0x6E2]` resolve against the
wrong base entirely - fixed before drawing any conclusion from a
result that would have been meaningless otherwise.

**With both test-setup bugs fixed, the real result is clean and
decisive**: calling `set_comm_queue_busy(0)` (the disengage/unmask
path) writes `01` to *both* `0D` and `1D` (unmasking `DR` and `TBRE`)
at the real Interrupt Mask Latch address, executes cleanly through to
the expected far return, and the Interrupt Vector Table is
byte-for-byte unchanged before and after (`00000000` both times) -
exactly the behavior real hardware would show, and definitively not
the IVT-corrupting behavior the zero-initialized pointer would have
caused before this fix. The full self-test sequence was also re-
verified to still complete identically (590 TX bytes) with the fix in
place, confirming it doesn't disturb anything already working.

**Still open**: the natural boot trace itself still doesn't call this
function (or anything else that would unmask the RS-232 interrupt) -
this fix makes the *mechanism* correct once something does reach it,
it doesn't by itself make the emulator reach it. Finding what code
path would call `set_comm_queue_busy`/`update_comm_tx_ready_flag` (a
comm-menu action? a later self-test phase?) is the next real target.

## Interactive front-panel controls: checkboxes, a dropdown, headers, an incoming panel

User requests, in sequence: "for the buttons could I get toggle
buttons or checkboxes on the display? as well as dropdowns for any
other front panel display values" - then "I would like headers on
each of the boxes. and I would like a box right above the outgoing
serial that shows the incoming button... it only needs to be 2 lines."

**Checkboxes for the 14 genuine independent toggle buttons**
(`textual.widgets.Checkbox`, one per `InteractiveFrontPanel.BUTTONS`
entry except the 2 handled specially below), wired via `on_checkbox_
changed` straight to `dbg.front_panel.set_button()`. **A dropdown for
the one real mutually-exclusive group**: `A_ONLY`/`B_ONLY` are both
active-low bits in the same register, and on real hardware pressing
one physically releases the other - representing them as 2 independent
checkboxes would allow an impossible "both pressed" state a real front
panel can't produce. A single 3-option `Select` (`A ONLY`/`BOTH`/
`B ONLY`) makes the real hardware constraint structural instead of
just documented.

**Startup sync, not continuous**: `_sync_front_panel_widgets()` reads
`dbg.front_panel`'s actual idle-baseline state (the real captured
values already used elsewhere - `SWB2=0x08`, `SWB1=0x44`) and sets
every checkbox/the dropdown to match, so e.g. `1K_4K`'s checkbox starts
*checked* (matching the real baseline having that bit set) rather than
defaulting every control to unchecked and silently misrepresenting the
panel. Verified via a headless test: `1K_4K` starts `True`, others
`False`, dropdown starts `A_ONLY` - all matching the documented
baseline exactly. Checking `MENU` correctly flipped `SWB2` (`0x08` ->
`0x18`); selecting `B_ONLY` in the dropdown correctly flipped both
relevant bits at once (`SWB1` `0x44` -> `0x05` - `A_ONLY`'s bit set
back to inactive, `B_ONLY`'s cleared to active). One acknowledged
limitation: this sync is startup-only - a button pressed/released via
a typed `press`/`release` command won't retroactively move its
checkbox, since the checkboxes are the intended way to drive the panel
interactively, not a live mirror of every other input path.

**Headers**: every panel (`registers`, the new `incoming`, `front
panel`, `outgoing`, `log`) now sets `border_title`, so the dashboard is
self-labeling instead of relying on position/context to tell panels
apart.

**The incoming panel**: a small `Static` (fixed `height: 4` - a
border top/bottom plus the requested 2 content lines), showing `dbg.
uart.incoming_text()` - the RX queue's actual byte contents, the same
thing the REPL's `incoming` command already showed on request, now
visible continuously without asking. Folded its refresh into
`refresh_registers()` itself (called from the same places already:
after every command, and periodically via `on_progress` during a long
run) rather than adding a second, easy-to-forget call site.

## Front-panel controls moved below the log, full-width grid

User request: "put the front panel items under the log. it can be the
same width with multiple columns for the inputs." Restructured the
layout: everything that was previously inside the narrow (`width: 44`)
`#side` sidebar column now sits in a new `#root` Vertical wrapping
`#main` (the existing registers/incoming/outgoing + log row) and a new
`#front-panel` `Grid` sibling below it, so the front-panel section
spans the *combined* width of the sidebar and the log rather than
being squeezed into the sidebar's own narrow column. `grid-size: 5`
lays the 14 checkboxes out 5-per-row instead of one per row; the
`HORIZONTAL MODE` dropdown spans the full 5-column width above them
(`column-span: 5`) since it's a single, wider control.

Verified via a headless test: the front-panel `Grid`'s rendered width
now exactly equals `#main`'s width (both `160` in a 160-column test
terminal), and 5 consecutive checkboxes land on the same row (`y=35`)
at increasing `x` positions before the 6th wraps to the next row -
confirming the multi-column layout actually took effect, not just
that the CSS parsed without error.

## Non-goals reminder

If this tool successfully answers the stroke-font question, resist the
temptation to grow it into a full-system emulator/simulator project
scope-creep - update `docs/display/vector-display-and-stroke-font.md`
and `STILL_PENDING_DECODE.md` with the finding and return to static
analysis for whatever's next, unless a *specific* new question makes
"run more of the firmware dynamically" the right tool again.
