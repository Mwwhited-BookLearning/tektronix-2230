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

**TUI mode, 2026-09-16**: `tui.bat` (or `python tui.py` once
`requirements.txt` is installed) is a Textual-based full-screen
dashboard alternative to `interactive.py`'s REPL - a live registers
panel plus a scrolling trace/diagnostic/serial log, same command set
(`step`/`run`/`continue`/`press`/`serial`/... - see `debugger_core.
HELP`, bound to F1) typed into an input bar, with F2-F5 shortcuts for
step/run/continue/trace. Built on `debugger_core.py`, the engine now
shared by both front ends so they can't drift apart - `interactive.py`
got noticeably shorter as a result, since it's just the REPL loop now.
Genuinely cross-platform (Textual has its own native Windows Terminal/
PowerShell driver, unlike plain `curses` which needs a `windows-
curses` shim on Windows and still has rough edges there); verified
headless via Textual's own test harness (`App.run_test()`) before ever
touching a real terminal - stepping, the `serial` interrupt-masking
diagnostic, and `incoming` all behaved identically to the REPL.

**Both `interactive.bat` and `tui.bat` now set up their own venv** in
`emulator\.venv` (created on first run) and install `requirements.txt`
into it automatically - no manual `pip install` step needed on a fresh
machine. `emu.py`/`interactive.py`/`tui.py` can still be run directly
with `python` if you're managing your own environment.

**Real i8251 UART core, 2026-09-16**: `i8251.py` is a byte-oriented
port of MAME's actual `i8251.cpp`/`.h` (fetched from source, BSD-3-
Clause) - the Mode/Command register state machine and RxRDY/TxRDY/
status-bit semantics are faithful to the real chip, not approximated.
`InteractiveUartMock` now drives this core: `0x406F0`=Data,
`0x406F1`=Control/Status (matching the confirmed `BA0`->`A0` UART
wiring), with RxRDY/TxRDY mirrored into the Option Status Latch and
`INT255` firing on RxRDY going high (gated on the Interrupt Mask
Latch). Unit-tested standalone before integration; re-verified the
full self-test sequence still completes identically through the new
TX path. See `docs/design.md`'s 2026-09-16 section for what's ported
faithfully vs. deliberately simplified (no bit-level serial-line
clocking, since this emulator only ever injects/observes whole bytes).

**Paced serial delivery**: `serial <text>` no longer dumps bytes into
the UART instantly - `serial-rate <n>` controls how many instructions
elapse between queued-byte deliveries, and overrun now happens for
real if a byte isn't serviced in time (matching real no-FIFO 8251-
family hardware) - built specifically to explore whether a too-fast
delivery rate explains this project's own live-hardware finding that
9600 baud was unreliable while 1200 baud worked perfectly (`docs/
comm-rom/rs232-breakthrough.md`). Also fixed a serious TUI bug found
while testing this: disabling the input during a run stripped its
focus and nothing restored it, so **every command after the first one
silently did nothing** - now fixed, see `docs/design.md`.

Not to be confused with `decompile/`, the Ghidra static-analysis
project (see `docs/architecture/ghidra-project.md`) - that's a second
disassembler/cross-check tool; this is dynamic execution.
