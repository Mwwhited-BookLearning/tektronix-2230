# RS-232: flow control and an open cross-ROM puzzle

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Open puzzle: comm ROM's compute_parity_mode_code calls the main ROM's scale_and_plot_point_default

`compute_parity_mode_code` (`0x96800`, comm ROM) is a straightforward
DIP-switch-to-parity-code dispatcher, but it brackets that dispatch
with two calls to `scale_and_plot_point_default` (physical `0xF0078`,
confirmed by direct disassembly to multiply an argument by the global
plot-scale reciprocal at `[0x6D2]` and fall through into real plotting
code) - once at entry with the literal argument `0x8F80`, saving its
`AX` result, and again at exit passing that saved result back in. This
is the exact argument/save/restore shape used everywhere else for the
`set_ds_return_old` DS-segment-switch helper (`push value; lcall;
save AX; ...; push saved AX; lcall` again) - but the target here is
confirmed to be the plot-scaling function's real address, not
`set_ds_return_old`'s. Not resolved: whether the comm ROM is
deliberately (ab)using this shared math primitive for value round-
tripping unrelated to plotting (and the "plot a point" side effect is
simply harmless/unobserved in this codepath), or whether this points
at some other addressing subtlety not yet understood. Left as an open
question rather than guessed at; the parity-code logic itself doesn't
depend on the answer.

**Second instance found**: `plot_point_with_flag_update` (`0xE004F`,
main ROM `160-3633`) uses the *exact same* bracketing shape (call
`scale_and_plot_point_default(0x8F80)`, save `AX`, do unrelated work,
call it again with the saved value) - but this one is called from
within the plot-output task area (`0xE6E2B`), where actually invoking
the real plot-scaling code is completely plausible, unlike the comm
ROM's parity-code context. This tips the balance slightly toward "the
`0x8F80`/save/restore bracketing is a real, intentional pattern with
some purpose beyond DS-switching" rather than a decode error - but
what that purpose is (is `0x8F80` a real point coordinate? is the
saved/restored value meaningful, or is only the side effect of the
call wanted?) still isn't understood.

**Checked for the `SUB_F6382`/`SUB_E90A5`-style "landing 1-byte-early
into a neighboring instruction" explanation** and it does NOT apply
here: the bytes immediately before `0xF0078` (`0xF0070-0xF0077`) are
never reached by recursive descent from any other confirmed path, so
there's no second, contradicting decode to compare against - unlike
those two cases, where a real fallthrough path disagreed with the
call-target path. `scale_and_plot_point_default`'s own decode (`inc
sp; add byte [bp+si-0x19], dh; ...`) is the only interpretation
available for those bytes, however unusual it looks.
## Found: RS-232 software flow control (XON/XOFF) in the comm ROM

`get_xon_xoff_byte` (`0x9751A`, `160-2998`) is a small, unambiguous
match for standard RS-232 software flow control: it checks two request
bits in `[0x460]` and returns the ASCII control code for XOFF (`0x13`,
DC3) or XON (`0x11`, DC1), clearing the corresponding request bit as
it does. It's called from `service_comm_tx_queue` (`0x97431` -
**renamed from an earlier wrong `service_comm_rx_queue`**: this drains
the OUTGOING tx ring buffer that `serial_tx_buffer_put` produces into,
not an incoming rx buffer - see "Direction correction" below), which
services a wrap-around ring buffer (`[0x448]`/`[0x44A]` read pointer,
`[0x44C]` write pointer, base `0xAF` size `0x384`) and forwards either
a pending flow-control byte or the next queued outgoing data byte to
`enqueue_comm_char` (`0x974E1`), which also implements space/mark
parity handling based on a `[0x4ED]` mode byte (0 = no parity
adjustment, else strip bit 7, and for mode `3` specifically force bit
7 back on). This is a solid, concrete confirmation of the RS-232 (not
just GPIB) personality of the comm ROM's serial path.

### Direction correction: the ring buffer is a TX queue, not RX

Found while investigating `serial_tx_buffer_put` and `SUB_96872`: the
ring buffer at `[0x448]`(read)/`[0x44C]`(write), base `0xAF` size
`0x384`, has `serial_tx_buffer_put` as its PRODUCER (writes the next
byte to transmit and advances `[0x44C]`) and what was named
`service_comm_rx_queue` as its CONSUMER (reads via `[0x448]` and
forwards each byte on toward the real hardware tx path via
`enqueue_comm_char`). A "service_comm_**rx**_queue" name for the
consumer of a queue that `serial_tx_**buffer_put**` feeds was
backwards - renamed to `service_comm_tx_queue`, and its paired
initializer `init_comm_rx_queue_and_ready_flags` to `init_comm_tx_
queue_and_ready_flags`. No genuine incoming-data ring buffer has been
identified yet - worth keeping an eye out for one when tracing however
incoming RS-232/GPIB bytes actually reach the firmware (an interrupt
handler is the likely mechanism, not yet traced).
