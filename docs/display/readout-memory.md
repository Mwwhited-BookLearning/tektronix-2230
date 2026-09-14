# CRT readout display memory

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## The readout/CRT display memory (physical 0x40000-0x4FFFF)

Found while tracing `SUB_E07B4` (`160-3633`, the report-printing
routine that wraps `print_selftest_banner`/`self_test_dispatcher` -
see above) down through its print-primitive call chain:
`print_string_far` (`0xE0AF5`) loops over a far-pointer nul-terminated
string byte-by-byte, calling `print_char` (`0xE0B2A`) per byte, which
calls `write_readout_port_byte` (`0xE0B50`). That last routine does
exactly one thing: writes the passed byte to the **fixed** physical
address `0x40000+0x6F0` (`es=0x4000, di=0x6F0`, never incremented). A
fixed destination address for every character of a string being
"printed" strongly suggests a hardware port (a character-generator
chip with its own internal cursor/position state), not plain RAM -
this is very likely the front-panel/CRT **readout character
generator**, matching this scope's known "readout" text overlay
system (channel/volts/time labels drawn on the CRT alongside the
analog trace).

A related routine, `SUB_E0B6C`/`SUB_E0B41`, writes a short run of
*different* literal bytes to *consecutive* offsets from that same
base (`0x40000+0x6F1`, `+0x6F2`, `+0x6F3` = `0x29`, `0x23`, `0x06`) -
consistent with a small fixed command/parameter block (position,
character code, attribute) latched together rather than a text
stream.

Separately, `append_readout_char` (`0xE39F0`) - called from a
different context (a running-output-line buffer at `[0x1AF4]`, not
this fixed port) - appends a byte at the buffer's current write
pointer, then **also** writes a second byte (from `AL`, computed
differently by its caller than the first byte in `DL`) to the *same*
pointer offset by a fixed `+0x8000`. Given `[0x1AF4]`'s segment traces
back to `[0x1CC4]` (also `0x4000`-segment in every call site checked
so far), this looks like a **dual-plane buffer inside the same 64KB
window**: a "character" plane at `0x40000-0x47FFF` and a second plane
(attribute? inverse-video? a shadow copy?) at `0x48000-0x4FFFF`,
`0x8000` higher. `SUB_E0BA3` separately reads raw bytes back out of a pointer
initialized to exactly `0x48000` (`[0x31E:0x320]`) and passes each one
directly to `append_readout_char` (`0xE39F0`) - i.e. it's re-printing
bytes it just read from that second plane, consistent with echoing
raw diagnostic/calibration values that were previously stashed there.

**Major re-examination, 2026-09-13, from the real service manual -
genuinely conflicting evidence, not resolved either way.** The service
manual's Table 3-1 ("Memory Space Allocation") labels physical
`0x406F0`-`0x406F7` (exactly `write_readout_port_byte`'s target and
`init_readout_port_config`'s 3-byte config block, `+1`/`+2`/`+3`
matching `0x406F1`-`0x406F3`) as **"Option UART/GPIB chips (I/O)" -
8 consecutive addresses**, not a readout/CRT register at all. This is
suspicious for a second reason: the service manual's GPIB theory-of-
operation section separately states the comm option's GPIB controller
(a TMS9914A) **"has eight internal registers"** - an exact count match
to Table 3-1's 8-address block. Taken together, this is a real,
specific case for `write_readout_port_byte` actually addressing the
**comm-option daughter board's UART/GPIB chip directly**, not any
CRT/readout hardware - which would mean this whole cluster
(`print_string_far`/`print_char`/`write_readout_port_byte`/`init_
readout_port_config`/`reset_readout_port`) was misnamed from an
earlier session, before this address was independently confirmed.

**Counter-evidence keeping this from being a confident rename**:
`print_string_far` calls `wait_readout_tick` before every character
it prints - `wait_readout_tick` busy-waits on `[0x752]`, the *general*
per-`INT2`-tick scheduler heartbeat (not something readout-specific
despite its own name), so this doesn't cleanly rule out either
interpretation: pacing serial/UART output by polling a generic system
tick is a perfectly ordinary crude flow-control technique in a small
embedded system with no dedicated baud-rate timer interrupt, so the
"wait" behavior is *consistent with* but doesn't *prove* either the
UART or the CRT-readout story.

**All 17 call sites of `print_string_far` are exclusively in the
`self_test_dispatcher`/`print_selftest_banner` self-test-report region**
(`0xE0400`-`0xE4300`) - none are used by the confirmed, separate
vector-stroke readout display list (`print_readout_string`/`draw_
readout_char`/`plot_readout_point`, which is unambiguously CRT-based
and uses a completely different mechanism, see "The readout vector
display list" above). This is consistent with either story too: a
self-test banner is exactly the kind of text that might legitimately
go out over an optional serial port for factory/field diagnostics
*or* be drawn once on the CRT via a simpler fixed-position hardware
character generator that the normal vector-font readout path doesn't
need for a one-time full-screen report.

**Follow-up, same day: checked the real TMS9914A datasheet (Texas
Instruments, via bitsavers.org) and this specifically rules out the
"GPIB Data Out register" theory.** The TMS9914A's own write-register
map (`RS2 RS1 RS0` register-select address lines) puts its **Data Out
register at address `111` (offset 7, the *last* of the 8 registers)**,
not offset 0. Register offset **0** on write is **Interrupt Mask
Register 0** - a mask/configuration register, not a byte-stream data
register. Since `write_readout_port_byte` writes to `0x406F0`
specifically - **offset 0** of the 8-address block, not offset 7 -
repeatedly writing arbitrary printable ASCII text bytes there
one-per-character makes no sense for a real TMS9914A: you would not
stream GPIB message bytes through the interrupt-mask register.

**This meaningfully weakens (does not fully rule out) the "comm-
option chip" re-interpretation above.** It specifically kills the
"streaming data out via the GPIB controller's own Data Out register"
version of that theory. It does *not* rule out the RS-232 variant of
the option - and checking the RS-232 side of the service manual
confirms it's the right track: **the RS-232 option board has a real
UART, "UART U1251"**, explicitly described as providing "serial-to-
parallel conversion," an internal baud-rate generator, and the classic
`TBRE`/`DR`/`INTR` interrupt lines (Transmitter-Buffer-Register-Empty,
Data-Ready, and a combined interrupt request) - textbook USART-style
signal names for a chip of this era. The same theory-of-operation
section confirms U1251 is enabled by one of the *same 8 address
strobes* that also enable the already-confirmed parameter buffer
(`U1222`) and status buffer (`U1223`) - i.e. **U1251 genuinely is one
of the chips occupying this exact 8-address "Option UART/GPIB chips"
block**, not a coincidental naming overlap.

**Conclusion: on RS-232-equipped units, `write_readout_port_byte`'s
write to `0x406F0` is very likely a genuine UART transmit-data write**
(a classic-era USART commonly puts its data register at the first
address of its register pair/block, distinguished from status/control
registers by a low-order address bit) - meaning the self-test banner
text really is being sent out over the RS-232 port when that option is
installed, harmlessly going nowhere when it isn't. On GPIB-equipped
units the exact behavior at offset 0 is still unclear (confirmed NOT
the TMS9914A's Data Out register, so possibly its Interrupt Mask
Register 0, meaning the "print" there might just be silently harmless
noise into a mask register). **Still short of a confident rename**:
haven't confirmed the exact chip part number for U1251 or its precise
register-offset layout, and the "CRT readout hardware" story from the
original naming was never definitively disproven either - but this is
now a well-evidenced, credible alternate explanation, not idle
speculation. Worth a rename once (or if) the remaining gap closes.

**`0x41000`/`0x42000` note superseded** - see `MEMORY_MAP.md`'s
"Confirmed regions" table: these are now confirmed via the service
manual as "Display chip interrupt reset"/"Display chip next frame",
and `SUB_E4429`/`SUB_E440A` have been renamed to `read_display_chip_
int_reset`/`read_display_chip_frame_trigger` accordingly.

**Not yet confirmed, worth revisiting**: whether `0x40000-0x4FFFF` is
read *as RAM* anywhere from the CPU's normal address space, or whether
it's exclusively accessed through this handful of fixed-offset
read/write primitives (which would make it much more clearly a
dedicated peripheral window rather than general video RAM the CPU
treats as memory). Also unconfirmed: the exact meaning of the `0x6F0`
port's "position" - nothing observed yet writes an X/Y coordinate
before writing a character, so either the coordinate is set elsewhere
(not yet found) or the hardware auto-advances a cursor per write
(more likely, given how `print_string_far`'s loop uses it - just a
tight byte-at-a-time write loop with no addressing logic at all).
