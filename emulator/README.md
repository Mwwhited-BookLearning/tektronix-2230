# Tek 2230 firmware emulator

A headless Unicorn-Engine-based tracer for running the real firmware
from reset and observing what it actually does at runtime - built to
answer questions static disassembly alone couldn't settle (see
`docs/design.md` for the motivating case: the stroke-font glyph-table
hunt in `docs/display/vector-display-and-stroke-font.md`).

**Status: first implementation pass, 2026-09-16.** Boots the real
firmware from reset (`python emu.py`), a working synthetic scheduler
tick (INT 2/NMI) unblocks `wait_readout_tick`, and milestones 1 and 3
from `docs/design.md` are functioning. 4 real emulation gotchas found
and fixed along the way (RAM-vs-ROM mapping, NMI-vs-maskable-interrupt
semantics, the 8086's 1MB address wraparound Unicorn doesn't model on
its own, and a false-positive "vector installed" check) - see
`docs/design.md`'s "Findings and gotchas" section before touching this
code. Currently blocked on a deterministic (tick-count-linked, not
timing-noise) unmapped read at physical `0x0C69B9` after exactly 33
ticks - next session's starting point.

Not to be confused with `decompile/`, the Ghidra static-analysis
project (see `docs/architecture/ghidra-project.md`) - that's a second
disassembler/cross-check tool; this is dynamic execution.
