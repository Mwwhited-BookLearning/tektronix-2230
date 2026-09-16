# Tek 2230 firmware emulator

A headless Unicorn-Engine-based tracer for running the real firmware
from reset and observing what it actually does at runtime - built to
answer questions static disassembly alone couldn't settle (see
`docs/design.md` for the motivating case: the stroke-font glyph-table
hunt in `docs/display/vector-display-and-stroke-font.md`).

**Status: working, and it already paid off, 2026-09-16.** Boots the
real firmware from reset (`python emu.py`), a working synthetic
scheduler tick (INT 2/NMI) unblocks `wait_readout_tick`, and milestones
1 and 3 from `docs/design.md` are functioning. 4 real emulation gotchas
found and fixed along the way (RAM-vs-ROM mapping, NMI-vs-maskable-
interrupt semantics, the 8086's 1MB address wraparound Unicorn doesn't
model on its own, and a false-positive "vector installed" check) - see
`docs/design.md`'s "Findings and gotchas" section before touching this
code.

**First real result**: instrumenting `draw_readout_char` and stubbing
the comm-option hardware-presence probe (`io_stubs.CommPresenceProbe`,
`--comm-installed`) resolved the long-standing stroke-font glyph-table
hunt for this project's actual hardware - see `changes/2026-09-16.md`
and `docs/display/vector-display-and-stroke-font.md`'s "Live emulation
confirms..." section. With `--comm-installed`, the boot trace now runs
cleanly to 60M+ instructions with zero crashes (versus reliably
crashing under the default "comm absent" stub). Next possible uses:
the still-open HPGL native-coordinate transform puzzle, or adding more
I/O stubs to get further past self-test (currently "Display controller
: TIMEOUT" and "ACQ_AB read-back 0 <> 2" are genuine self-test
failures from unstubbed hardware, not firmware bugs).

Not to be confused with `decompile/`, the Ghidra static-analysis
project (see `docs/architecture/ghidra-project.md`) - that's a second
disassembler/cross-check tool; this is dynamic execution.
