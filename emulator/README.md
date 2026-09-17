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
crashing under the default "comm absent" stub).

**Second real result, same day**: fed this session's new hardware
knowledge (the service manual's Table 3-1, the RS-232 board's
schematic-traced chip identities, and real captured register values
from the exerciser-screen photos) back into `io_stubs.py` as 8 new
`FixedByteRead` stubs, and made the diagnostic-text capture
(`io_stubs.DiagnosticTextCapture`) permanent instead of one-off scratch
code. With `--comm-installed --stub-registers --show-diag-text`, the
**entire power-up self-test sequence now completes** for the first
time (`'2230/2220 Power up tests complete.'` actually prints). The 2
remaining failures ("Display controller : TIMEOUT" and the `ACQ_AB`
address-walk test) are now properly diagnosed rather than just
observed - see `docs/design.md`'s 2026-09-16 section for what each one
actually needs (real interrupt tracing for the first, a write-then-
readback coupling stub for the second) - neither is a simple fixed-
value gap, which is itself a useful distinction.

**Interactive mode, 2026-09-16**: `python interactive.py` is a REPL
debugger built on the same memory map and stubs - shows live register/
flag state and the current instruction after every stop, and adds
`step`/`run`/`continue`/`break` commands plus `trace on`/`trace off`
(streams one line per instruction, registers + the live value of any
memory operand it touches - e.g. `lodsb` shows exactly what byte it
just read). Simulate front-panel button presses (`press MENU`,
`release MENU`, ...) and inject incoming serial bytes (`serial <text>`,
supports `\n`/`\r`/`\t` escapes - via an explicitly experimental UART
mock, see `io_stubs.InteractiveUartMock`'s docstring for what it does
and doesn't model); `uart`/`outgoing` show what's been received/sent.
Both `emu.py` and `interactive.py` default to `--comm-installed`
(matching this project's real physical test units) - pass
`--no-comm-installed` for the "not installed" behavior instead.
`emu.py` stays the tool for long, unattended batch runs (much faster,
no per-step Python round-trip); `interactive.py` is for poking at a
specific point in execution. Type `help` at its prompt for the full
command list.

Not to be confused with `decompile/`, the Ghidra static-analysis
project (see `docs/architecture/ghidra-project.md`) - that's a second
disassembler/cross-check tool; this is dynamic execution.
