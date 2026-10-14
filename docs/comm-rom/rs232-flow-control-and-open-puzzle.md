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
(below, lightly edited) independently re-traced `poll_comm_status_tick`'s
*producer* side and its ring buffer without recognizing it as a
function already documented under its main-ROM alias address. Cross-
checked against a Ghidra decompile of the main ROM, which immediately
surfaced the duplicate: the function at native `0x8EF54` **is**
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
==0` (the common case) it resolves to `consume_comm_rx_ring_buffer` (native
`0x8F12B`, raw `0x9712B`); when `[0x1BFA]!=0` it resolves instead to
`dispatch_comm_rx_status_bits` (native `0x8F5AC`, raw `0x975AC`) - this corrects the
earlier (wrong) assumption that slot 5's target was simply "`SUB_975AC`
/`SUB_9712B`" together, as if both were reached unconditionally. Also
genuinely new: the consumer/second-dispatcher *behavior* of
`consume_comm_rx_ring_buffer`/`dispatch_comm_rx_status_bits` themselves (`poll_comm_status_tick`'s
existing description only covered the producer side) - see below.

**The producer - `poll_comm_status_tick`** (native
`0x8EF54`, raw/alias `0x96F54`; see `FUNCTIONS.md` for the full
existing writeup): reads `[0x6DA]` (status/interrupt-latch byte) and
on bit `0x02` (data-ready) reads the received byte plus two more
register bytes via `[0x6D6]`, packaging all 4 (data, reg+1, reg+3,
status) into the already-documented RX ring buffer (`[0x464]`/`[0x466]`
write ptr, base `0x46C`, size `0x80`, 32 four-byte records) - the
RX-side counterpart of the already-documented TX queue (`[0x448]`/
`[0x44A]`/`[0x44C]`, base `0xAF`, 1-byte records).

**The consumer - `consume_comm_rx_ring_buffer` (native `0x8F12B`)** - task slot 5's
*usual* target per the correction above: drains the ring buffer by
comparing read pointer `[0x468]`/`[0x46A]` against the write pointer,
and for each record calls `translate_comm_rx_event_type` (`0x8F54C`) on the record's `+1`
status/class byte (masked `0x9F`) to get a translated event-type byte,
dispatches further on specific bits of that result (`0x80` ->
`SUB_97253`, `0x10` -> `SUB_97583`), advances the read pointer by 4
with wraparound at `0x46C+0x80`, and services the TX queue once
drained.

**The alternate dispatcher - `dispatch_comm_rx_status_bits` (native `0x8F5AC`)** -
task slot 5's target only when `[0x1BFA]!=0`: keyed on `[0x598]` (the
raw status byte saved during capture), calls bit-by-bit: `SUB_976C0`
(`0x20`), `SUB_9785B` (`0x10`), and - **bit `0x04`** -
`parse_next_gpib_command_byte` (`0x8F905`) directly, the function
already known to read the staged byte out of `ds:0x586`/`ds:0x590`
into `[0x580]` and feed it onward into `process_gpib_command_byte`'s
classification/dispatch path. Whether `consume_comm_rx_ring_buffer` ever reaches
this bit-`0x04` path too (e.g. via a shared callee) isn't confirmed -
not traced in this pass.

Full chain, hardware to parser: `[0x6D6]`/`[0x6DA]` (UART registers)
-> `poll_comm_status_tick` -> RX ring buffer
(`[0x464]`/`[0x468]`, base `0x46C`) -> task slot 5's `[0x73C]/[0x73E]`
(conditional on `[0x1BFA]`) -> `consume_comm_rx_ring_buffer` (usual) or
`dispatch_comm_rx_status_bits` (rare) -> `parse_next_gpib_command_byte` -> `[0x580]`/
`[6]` -> the command dispatch machinery documented elsewhere in this
directory.

**Done, same day, later pass**: `consume_comm_rx_ring_buffer`,
`dispatch_comm_rx_status_bits`, and `translate_comm_rx_event_type`
are now real `gen_disasm_x86.FUNCTIONAL_NAMES` entries (native
`0x8F12B`/`0x8F5AC`/`0x8F54C`, each with an alias-address entry too),
and `poll_comm_status_tick`'s rename - documented in prose for a
prior session but never actually applied to the generator, per the
exhaustive-grep finding below - has also finally been applied. The
consumer-chain detail has been merged into `FUNCTIONS.md`'s
`poll_comm_status_tick` row, with dedicated rows added for the other
three. `docs/interrupts/task-scheduler.md`'s slot-5 resolution was
already current. `[0x598]` was not separately named this pass - still
only referenced inline as "the raw captured status byte."

## Resolved, 2026-10-14, later pass: the genuine incoming-data ring buffer, found

The "Direction correction" section above explicitly flagged "no
genuine incoming-data ring buffer has been identified yet" as open.
It's now found, closing that question. **Tooling note first**:
`consume_comm_rx_ring_buffer`'s two dispatch targets (`0x80` ->
`SUB_97253`, `0x10` -> `SUB_97583`) had no disassembled body in any
committed listing - not `160-2998-14.lst` (filtered out by
`gen_disasm_2998.py`'s `render_comm_only()`, which only renders the
native `0x80000`-`0x8FFFF` range, while these are reached only
through the `0x90000`-alias window from the comm ROM's own heuristic
entry points) nor `sysrom_3532_3633.lst` (that listing's separate
`gen_disasm_x86.py` run never reaches them, since nothing in the main
ROM literally calls them). Worked around with a one-off direct
capstone disassembly of the raw bytes in `binary/160-2998-14.bin` at
native file offsets `0xF253`/`0xF583` (`native - 0x80000`).

**`process_comm_rx_data_byte`** (native `0x8F253`, raw/alias
`0x97253`) is the real RS-232 RX byte-processing pipeline:
- strips parity per the already-documented `[0x4ED]` mode byte (same
  field `enqueue_comm_char` reads on the TX side);
- detects XON (`0x11`)/XOFF (`0x13`) control bytes, clearing/setting a
  new RX-paused flag `[0x45D]` and, on XON, setting a new pending-
  resume flag `[0x45A]`;
- detects CR (`0x0D`)/LF (`0x0A`) line-boundary bytes per the line-
  terminator mode `[0x4EF]`, updating two new message-boundary
  pointer pairs `[0x440]`/`[0x442]` and `[0x444]`/`[0x446]`;
- enqueues the processed byte into a new RX **message** buffer
  (`[0x434]`/`[0x436]` write ptr, `[0x43C]` base/read ptr, wraps to
  offset `0xF` at bound `0xAF`), setting a new full flag `[0x451]`
  and logging error code `0x19` into `[0x68C]` on overflow.

This is a *different* buffer from `poll_comm_status_tick`'s raw
4-byte-record RX ring buffer (`[0x464]`/`[0x468]`, base `0x46C`) -
that one holds raw hardware captures (data byte + 2 register bytes +
status); this one holds the *processed* byte stream after parity
stripping and XON/XOFF filtering, i.e. the buffer an actual line-
oriented consumer (GPIB/RS-232 command parser) would read from. See
`VARIABLES.md`'s new rows for the full field-by-field writeup.

**`resume_comm_tx_queue_if_pending`** (native `0x8F583`, raw/alias
`0x97583`) is `process_comm_rx_data_byte`'s XON-handling counterpart:
if `[0x45A]` is set, it calls `service_comm_tx_queue` to resume
transmission - completing the RX side of the XON/XOFF software
flow-control loop whose TX side (`get_xon_xoff_byte`/
`service_comm_tx_queue`) was already documented above.

**Not yet resolved**: `dispatch_comm_rx_status_bits`'s own two
dispatch targets, `SUB_976C0` (`0x20`) and `SUB_9785B` (`0x10`), were
only partially traced the same way (raw-byte capstone dump, bodies
not fully captured) - lower confidence, not yet named. `SUB_976C0`
looks like a lazy-init/bus-reset-detection routine that also writes
`0x83` to the `[0x6D6]` UART bank's `+3` register when the new
`[0x451]` full flag is set (plausibly RTS-deassert, the hardware-
flow-control counterpart to this section's software XON/XOFF
mechanism, but not confirmed against a schematic); `SUB_9785B`
closely parallels `service_comm_tx_queue`'s own dequeue-and-transmit
logic behind an unexplained priority chain of flags `[0x455]`-
`[0x458]`. See `VARIABLES.md`'s new row for both and
`STILL_PENDING_DECODE.md` for this as a tracked open item.

## Resolved, 2026-10-14, even later pass: the RX message buffer's consumer chain, and proof it feeds the real parser

The previous section resolved the *write* side of the RX message
buffer but left its consumer unidentified. That consumer is now fully
traced, and - critically - it's now proven that the buffer feeds the
real SCPI/GPIB command parser for **ordinary** traffic, not just the
overflow-error case.

**`[0x438]`/`[0x43A]`** is the buffer's true **consumption cursor**,
distinct from the write ptr `[0x434]` and the read ptr `[0x43C]`
documented in the previous section. The earlier-flagged "odd non-zero
wrap origin of `0xF`" is also resolved here: `0xF` is simply the
buffer's literal base offset - `init_comm_rx_message_pipeline` (native
`0x8E597`) initializes all three pointers to `0xF` at reset, not an
anomalous wrap target.

Six new functions, all found via `[0x43C]`'s own cross-reference list
(same already-rendered native `0x80000`-`0x8FFFF` range as the
previous section - no tooling workaround needed this time):

- **`init_comm_rx_message_pipeline`** (native `0x8E597`) - the
  RX-pipeline reset: zeroes both the raw hardware ring buffer
  (`[0x464]`/`[0x468]`) and the message buffer's three pointers (all
  to `0xF`), clears the line-boundary markers, sentinels `[0]`-`[3]`,
  and the TX-queue-reinit flag `[0x454]`, and performs a GPIB UART
  pulse. No resolved static caller - a heuristic entry point, like
  several of its siblings below.
- **`fetch_next_comm_rx_char`** (native `0x8E99B`) - the real
  pull-style "get next received character" primitive. **This is the
  proof**: it's called directly from inside the character/argument
  lexer - `normalize_and_validate_arg_char` and roughly ten other
  sites spanning `FUNC_2998_1731`..`FUNC_2998_519A` (digit/alpha range
  checks, `hex_digit_to_nibble`, quote/comma/semicolon delimiter
  handling) - immediately before each of those reads `byte ptr [4]`.
  On the normal path it advances `[0x438]` (wrapping to base `0xF`),
  reads the byte at the old position into `[4]`, and clears the
  "more data" sentinel `[3]` once caught up to either the write ptr
  `[0x434]` or the CR-marker `[0x440]`. On the error path (full flag
  `[0x451]` set, nothing left to consume) it stages error `0x19` into
  `[0x623]`/`[6]` and forcibly calls `process_gpib_command_byte` - the
  mechanism already confirmed in the previous section, now shown to be
  one branch of the same function that serves ordinary reads.
- **`sync_comm_rx_read_ptr_and_release_hold`** (native `0x8E8A2`) -
  buffer-drain maintenance: resyncs the read ptr `[0x43C]` to the
  consume cursor `[0x438]` once room has freed, clears the full flag,
  does a GPIB UART pulse under certain line-boundary conditions, and -
  when RS-232 hold-reason-bit-`1` is active in the already-confirmed
  hold bitmask `[0x45C]` and >= `0x28` (40) bytes are now free - calls
  `release_comm_hold(1)` to request XON. This directly cross-validates
  today's buffer-fullness tracking against the separately-confirmed
  `engage_comm_hold`/`release_comm_hold` mechanism (`FUNCTIONS.md`
  lines documenting `0x97BE6`/`0x97C28`) - independent evidence the
  read of this subsystem is correct.
- **`rearm_comm_rx_consume_cursor`** (native `0x8ECE9`) - called
  standalone throughout the same lexer functions that call
  `fetch_next_comm_rx_char` (not always paired with it): if the
  consume cursor differs from the read ptr, or they match but the
  full flag is set, snaps the cursor to the read ptr and sets the
  "more data" sentinel; a no-op otherwise.
- **`finalize_comm_rx_line_boundary`** (native `0x8EAAF`) - called
  from the alpha-char handling functions near `FUNC_2998_5115`/
  `FUNC_2998_519A`: calls `sync_comm_rx_read_ptr_and_release_hold`
  first, then promotes the pending LF-marker pair `[0x444]`/`[0x446]`
  into the CR-marker slot `[0x440]`/`[0x442]`, recomputes the
  "more data pending" sentinels from whether the write and read
  pointers still differ, and releases the critical flag.
- **`set_comm_rx_busy_for_mode(engage)`** (native `0x8EA89`) - a tiny
  RS-232-vs-GPIB dispatcher forwarding its argument to
  `set_comm_flow_hold` or `set_comm_queue_busy`. No resolved static
  caller.

**`[4]`** is the global "current input character" scratch cell that
the entire argument/character lexer reads throughout
`FUNC_2998_1731`-`FUNC_2998_519A`. `fetch_next_comm_rx_char`'s normal
path is its *only* writer. This is the direct link: every ordinary
received byte that the pipeline hands to the parser passes through
`[4]`, written by the same function that's also the overflow-error
handler - so the "closes the incoming-data gap" claim from the
previous section is now true end-to-end, not just up to an
unconnected buffer.

**Not yet traced**: the hand-off from `[4]` into the command buffer
`[0x586]` that `parse_next_gpib_command_byte` ultimately reads from -
i.e. exactly how a fully-lexed/validated character makes it from this
per-character scratch cell into the real command-buffer text that
gets dispatched. `init_comm_rx_message_pipeline`, `set_comm_rx_busy_
for_mode`, `rearm_comm_rx_consume_cursor`, and
`sync_comm_rx_read_ptr_and_release_hold` (outside its call from
`finalize_comm_rx_line_boundary`) still have no resolved static
caller - likely reached only via an unresolved far-pointer table, the
same unsolved-caller pattern already seen elsewhere in this ROM.
