# Tek 2230 firmware emulator

A headless Unicorn-Engine-based tracer for running the real firmware
from reset and observing what it actually does at runtime - built to
answer questions static disassembly alone couldn't settle (see
`docs/design.md` for the motivating case: the stroke-font glyph-table
hunt in `docs/display/vector-display-and-stroke-font.md`).

**Status: design phase.** See `docs/design.md` for scope, the memory
map wiring plan, I/O stub strategy, and milestones. No implementation
yet.

Not to be confused with `decompile/`, the Ghidra static-analysis
project (see `docs/architecture/ghidra-project.md`) - that's a second
disassembler/cross-check tool; this is dynamic execution.
