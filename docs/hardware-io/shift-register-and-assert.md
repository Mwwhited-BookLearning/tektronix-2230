# Hardware I/O: shift register and the assert/panic mechanism

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Found: the hardware shift-register write (I/O ports 0xD1/0xC4)

Renamed `write_hw_shift_register` (`0xEE13B`), resolving I/O ports
`0xD1`/`0xC4` from `MEMORY_MAP.md`'s long-standing "no documentation
yet on which physical device" list. It writes `ax` to port `0xD1`
three times in a row - each write preceded by `shl di,1` - then once
to port `0xC4`. This is the textbook shape of clocking a value out to
a **serial shift-register-based hardware latch**: `0xD1` looks like
the data/clock port (written repeatedly as the value shifts) and
`0xC4` the strobe/latch port (written once, after the shift sequence
completes) - though which is which isn't confirmed.

Found it by reading the larger function around it (starting near
`0xEE004`): that function reads a table entry at `[0x1D10 + idx*16]`
(`idx` from `[0x464]`), and either makes an *indirect* far call through
the table (`lcall es:[bx+di+6]`) or falls through to
`write_hw_shift_register` as a default action. This shape - "look up a
setting's handler, call it, or do a generic hardware push if none" -
is consistent with a **front-panel setting (attenuator/gain/offset
calibration value) being written out to analog hardware whenever it
changes**. Not confirmed against a schematic, but the mechanism (a
serial shift-register write) now is, closing a mystery that's been
open since the very first coverage pass found these two ports.

## Found: the firmware's assert()/panic mechanism

`halt_cpu` (`0xF1611`) is a single `hlt` instruction - but tracing its
callers turned up something more interesting than a generic halt.
Callers (e.g. `assert_and_halt`, `0xE9255`) call it with 2 words
pushed as arguments (a computed value plus a small constant tag like
`0xB`/`0xC`/`2`) and then have unreachable stack-cleanup code
immediately after the call - because `hlt` never returns, that cleanup
never executes. This is exactly what compiled C looks like when a
function is declared to never return (an `abort()`/panic-style call):
the compiler still generates the normal call-then-cleanup sequence
(it doesn't know the callee halts forever), but the cleanup is dead
code in practice. `assert_and_halt` computes a value via
`convert_sample_value` (`0xF1001`) and halts with it plus a tag if
some bound is exceeded - a genuine embedded assertion-failure trap,
presumably only ever tripped during development/debugging (or as a
last-resort safety stop on real hardware fault). `convert_sample_value`
itself opens with a real x87 `fmul` instruction (entered via
fallthrough, no prologue of its own) followed by `mul32`/`sdiv32` -
further, more concrete evidence toward `TODO.md`'s open question of
whether a real 8087 coprocessor is present (this is a genuine,
non-drifted floating-point instruction in the middle of otherwise
completely ordinary compiled-C integer code, not part of any known
decode-drift cluster).

**Update 2026-09-13**: user directly inspected the main digital/
acquisition board (A10) and found **no 8087 chip present** - see
`HARDWARE.md`'s "Main system board interior" section. Since `fmul`
here only runs as part of computing `assert_and_halt`'s panic argument
(a path that, by definition, is meant to almost never trigger in
normal operation), this fits neatly into the same "real call path that
never actually executes on shipped hardware" pattern found repeatedly
elsewhere this session (`SUB_EAC86`, `SUB_F173E`→`0xEA13B`, etc.) -
the instruction is real and would execute as a harmless no-op on bare
8086/8088 silicon if ever reached (no 8087-absent fault exists for
`ESC` opcodes), it just most likely never actually gets reached on
real hardware. Not fully closed - worth checking whether any other
board in the unit (e.g. an acquisition daughter-board) carries an
8087 before concluding there's none anywhere in the instrument.
