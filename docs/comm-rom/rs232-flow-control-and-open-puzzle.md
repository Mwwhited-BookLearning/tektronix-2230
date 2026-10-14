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

**Correction, same day, later pass**: the first version of this section
(below, lightly edited) independently re-traced `FUNC_2998_EF54`'s
*producer* side and its ring buffer without recognizing it as a
function already documented under its main-ROM alias address. Cross-
checked against a Ghidra decompile of the main ROM, which immediately
surfaced the duplicate: `FUNC_2998_EF54` (native `0x8EF54`) **is**
`FUNCTIONS.md`'s existing `poll_comm_status_tick` (raw/alias
`0x96F54`), which already documented the same `[0x6DA]`/`[0x6D6]`
reads and the same `[0x464]`/`0x46C`-`0x46C+0x80` ring buffer. The
"third default-handler pointer pair, not yet matched to a scheduler
slot" claim below is also **wrong**: `FUNCTIONS.md` already resolved
`[0x740]` as `run_continuous_selftest_tick`'s per-tick hook (called via
the hardware `INT255`/`INTR` chain, confirmed 2026-09-13), not a
13th entry in the 12-slot default-handler table - that mechanism is
unrelated to the `switch_to_next_task` task-slot table this section
originally pointed at. **What genuinely is new** (confirmed directly
against `init_comm_dispatch_table`'s raw bytes,
`disasm/sysrom_3633-14.lst`/`sysrom_3532_3633.lst` lines 10745-10780):
task-scheduler.md's long-open "task slot 5's target, not yet traced"
question. `[0x73C]/[0x73E]` is set *conditionally* on `[0x1BFA]`
(`VARIABLES.md`'s already-documented COMM_LOOPBACK self-test readback
flag, set only when that probe reads exactly `0xD0`): when `[0x1BFA]
==0` (the common case) it resolves to `FUNC_2998_F12B` (native
`0x8F12B`, raw `0x9712B`); when `[0x1BFA]!=0` it resolves instead to
`FUNC_2998_F5AC` (native `0x8F5AC`, raw `0x975AC`) - this corrects the
earlier (wrong) assumption that slot 5's target was simply "`SUB_975AC`
/`SUB_9712B`" together, as if both were reached unconditionally. Also
genuinely new: the consumer/second-dispatcher *behavior* of
`FUNC_2998_F12B`/`FUNC_2998_F5AC` themselves (`poll_comm_status_tick`'s
existing description only covered the producer side) - see below.

**The producer - `poll_comm_status_tick` / `FUNC_2998_EF54`** (native
`0x8EF54`, raw/alias `0x96F54`; see `FUNCTIONS.md` for the full
existing writeup): reads `[0x6DA]` (status/interrupt-latch byte) and
on bit `0x02` (data-ready) reads the received byte plus two more
register bytes via `[0x6D6]`, packaging all 4 (data, reg+1, reg+3,
status) into the already-documented RX ring buffer (`[0x464]`/`[0x466]`
write ptr, base `0x46C`, size `0x80`, 32 four-byte records) - the
RX-side counterpart of the already-documented TX queue (`[0x448]`/
`[0x44A]`/`[0x44C]`, base `0xAF`, 1-byte records).

**The consumer - `FUNC_2998_F12B` (native `0x8F12B`)** - task slot 5's
*usual* target per the correction above: drains the ring buffer by
comparing read pointer `[0x468]`/`[0x46A]` against the write pointer,
and for each record calls `SUB_9754C` (`0x8F54C`) on the record's `+1`
status/class byte (masked `0x9F`) to get a translated event-type byte,
dispatches further on specific bits of that result (`0x80` ->
`SUB_97253`, `0x10` -> `SUB_97583`), advances the read pointer by 4
with wraparound at `0x46C+0x80`, and services the TX queue once
drained.

**The alternate dispatcher - `FUNC_2998_F5AC` (native `0x8F5AC`)** -
task slot 5's target only when `[0x1BFA]!=0`: keyed on `[0x598]` (the
raw status byte saved during capture), calls bit-by-bit: `SUB_976C0`
(`0x20`), `SUB_9785B` (`0x10`), and - **bit `0x04`** -
`parse_next_gpib_command_byte` (`0x8F905`) directly, the function
already known to read the staged byte out of `ds:0x586`/`ds:0x590`
into `[0x580]` and feed it onward into `process_gpib_command_byte`'s
classification/dispatch path. Whether `FUNC_2998_F12B` ever reaches
this bit-`0x04` path too (e.g. via a shared callee) isn't confirmed -
not traced in this pass.

Full chain, hardware to parser: `[0x6D6]`/`[0x6DA]` (UART registers)
-> `poll_comm_status_tick`/`FUNC_2998_EF54` -> RX ring buffer
(`[0x464]`/`[0x468]`, base `0x46C`) -> task slot 5's `[0x73C]/[0x73E]`
(conditional on `[0x1BFA]`) -> `FUNC_2998_F12B` (usual) or
`FUNC_2998_F5AC` (rare) -> `parse_next_gpib_command_byte` -> `[0x580]`/
`[6]` -> the command dispatch machinery documented elsewhere in this
directory.

Not yet done: naming `FUNC_2998_F12B`/`FUNC_2998_F5AC` and the RX
ring-buffer's consumer-side variables (`SUB_9754C`, `[0x598]`) via
`gen_disasm_x86.py`'s `FUNCTIONAL_NAMES`; reconciling `poll_comm_
status_tick`'s `FUNCTIONS.md` row with this section's extra consumer-
chain detail (should likely be merged into that row directly); and
adding `docs/interrupts/task-scheduler.md`'s slot-5 resolution. Left
for a follow-up pass rather than done here to avoid further duplicate
rediscovery without re-reading that file first.
