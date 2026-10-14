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

**Checked for the `SUB_F6382` (now `draw_marker_box_and_update_position`)/`SUB_E90A5`-style "landing 1-byte-early
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

Found while investigating `serial_tx_buffer_put` and `SUB_96872` (now `spawn_task_with_tag`): the
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

## Found, 2026-10-14: the genuine incoming RX path, end to end

Direct resolution of the long-open "where does an incoming byte first
land" question (`TODO.md`, `STILL_PENDING_DECODE.md`'s "Comm ROM /
RS-232 command parsing" section). Found by following the already-
confirmed-but-unresolved far pointer `[0x6D6]` (into the UART/GPIB
register bank at physical `0x406F0`-`0x406F7`) for *reads*, not just
the previously-documented baud/parity-config *writes*.

**The producer - `FUNC_2998_EF54` (physical `0x8EF54`, comm ROM)**:
reads a second far pointer, `[0x6DA]`, which resolves into the same
register bank (physical `0x406F4`-ish neighborhood) and functions as
the UART's status/interrupt-latch byte. It tests bit `0x80` (toggles
the comm-status icon via `[0x732+0x97]` + `redraw_option_status_icon`)
and bit `0x02`. **When bit `0x02` (data-ready) is set**, it reads the
actual received byte straight from `es:[di]` where `di` comes from
`[0x6D6]` - i.e. a plain `mov dl, byte ptr es:[di]` on the UART's data
register, exactly the "no `in`/`out` instruction, must be a memory-
mapped `mov`" shape `STILL_PENDING_DECODE.md` already predicted - plus
two more register bytes at `[0x6D6]+1` and `[0x6D6]+3`. It packages all
4 bytes (data, reg+1, reg+3, the original status byte) into one record
and writes it into a **second, previously-unidentified ring buffer**:
write pointer `[0x464]`/`[0x466]` (far), base `0x46C`, size `0x80` (32
four-byte records), with the same wraparound shape as the already-
documented TX queue - this is that queue's RX-side counterpart
(`[0x448]`/`[0x44A]`/`[0x44C]`, base `0xAF`, 1-byte records; RX needs
4 bytes/record since it also carries per-byte status).

**Wired into the scheduler exactly like the two already-documented
task slots** (`docs/interrupts/task-scheduler.md`'s 2026-10-09
section): `init_comm_dispatch_table` (`0xE6D4D`, main ROM) sets up
*three* far-pointer pairs, not two - `[0x738]/[0x73A]` (slot 4 ->
`comm_call_main_rom`, already known) and `[0x73C]/[0x73E]` (slot 5 ->
`SUB_975AC`/`SUB_9712B`, already known) plus a third, **`[0x740]/
[0x742]`, set to offset `4` segment `0x96F5` - physical `0x96F54`,
i.e. `FUNC_2998_EF54` itself**. Whichever task slot's default-handler
entry does `lcall [0x740]` (not yet matched to one of the 12 dumped
slot entries - slots 0 and 10 are still "not traced this session" in
the task-scheduler doc and are the likeliest candidates) is therefore
the real tick-driven UART poll.

**The consumer - `FUNC_2998_F12B` (physical `0x8F12B`)**: drains the
ring buffer by comparing read pointer `[0x468]`/`[0x46A]` against the
write pointer, and for each record calls `SUB_9754C` (`0x8F54C`) on
the record's `+1` status/class byte (masked `0x9F`) to get a
translated event-type byte, then dispatches further on specific bits
of that result (`0x80` -> `SUB_97253`, `0x10` -> `SUB_97583`), advances
the read pointer by 4 with wraparound at `0x46C+0x80`, and services the
TX queue once drained.

**Closes the loop into the already-documented command-byte parser**:
a second, closely-related dispatcher, `FUNC_2998_F5AC` (`0x8F5AC`),
keyed on `[0x598]` (the raw status byte saved during capture), calls
bit-by-bit: `SUB_976C0` (`0x20`), `SUB_9785B` (`0x10`), and -
**bit `0x04`** - `parse_next_gpib_command_byte` (`0x8F905`) directly.
That is the function already known to read the staged byte out of
`ds:0x586`/`ds:0x590` into `[0x580]` and feed it onward into
`process_gpib_command_byte`'s classification/dispatch path. So the
full chain, producer to parser, is now traced: hardware register
(`[0x6D6]`/`[0x6DA]`) -> `FUNC_2998_EF54` -> RX ring buffer (`[0x464]`/
`[0x468]`, base `0x46C`) -> `FUNC_2998_F12B` / `FUNC_2998_F5AC` ->
`parse_next_gpib_command_byte` -> `[0x580]`/`[6]` -> the command
dispatch machinery documented elsewhere in this directory.

Not yet done: naming `FUNC_2998_EF54`/`FUNC_2998_F12B`/`FUNC_2998_F5AC`
and the new ring-buffer variables via `gen_disasm_x86.py`'s
`FUNCTIONAL_NAMES`, and identifying which scheduler task slot owns
`[0x740]`. Left for a follow-up pass rather than guessed at here.
