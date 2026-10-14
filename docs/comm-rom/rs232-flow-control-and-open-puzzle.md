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
  proof**: it's called from 9 sites across 8 enclosing functions -
  `FUNC_2998_1909`, `normalize_and_validate_arg_char`, `FUNC_2998_3B52`,
  `FUNC_2998_3B86`, `FUNC_2998_3D2C` (2 sites), `FUNC_2998_3F22`,
  `FUNC_2998_4F3B` (2 sites), and `FUNC_2998_5022` - immediately before
  each of those reads `byte ptr [4]` (digit/alpha range checks,
  `hex_digit_to_nibble`, quote/comma/semicolon delimiter handling).
  **Correction (same day, later re-check)**: an earlier pass in this
  section described the callers loosely as "spanning
  `FUNC_2998_1731`..`FUNC_2998_519A`" - that was wrong. Checking each
  call site's actual enclosing function (not just address proximity)
  turned up the 8 functions listed above; `FUNC_2998_1731` and
  `FUNC_2998_519A` are themselves *not* among them. `FUNC_2998_1731` in
  particular reads `[4]` on entry with no preceding call to
  `fetch_next_comm_rx_char` in its own body, and its caller
  (`FUNC_2998_1AE1`) is deep in plot/waveform-output code
  (`draw_pending_line_segment`, `reset_plot_home_or_acq`,
  `redraw_option_status_icon`) - see `command-parser-token-scan-and-
  plot-handler.md` for that cluster. `[4]` is a generic single-byte
  scratch cell, not uniquely owned by the RX pipeline, so a bare read
  of it elsewhere in the ROM doesn't by itself imply RX-buffer
  consumption; only the 8 functions above are confirmed to pull `[4]`
  from this pipeline. This doesn't weaken the proof - it narrows and
  sharpens it to the sites that actually call the function.
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
- **`rearm_comm_rx_consume_cursor`** (native `0x8ECE9`) - called from
  12 sites across 9 functions: `FUNC_2998_1731`, `FUNC_2998_3048` (2),
  `FUNC_2998_3298`, `FUNC_2998_3B86`, `FUNC_2998_3E79`,
  `FUNC_2998_40A9`, `FUNC_2998_4240`, `FUNC_2998_5115` (2), and
  `FUNC_2998_519A` (2) - genuinely spanning the `FUNC_2998_1731`..
  `FUNC_2998_519A` range (only `FUNC_2998_3B86` overlaps with the
  `fetch_next_comm_rx_char` caller list above; this function's own
  caller set is mostly a *different* group of siblings). If the
  consume cursor differs from the read ptr, or they match but the
  full flag is set, snaps the cursor to the read ptr and sets the
  "more data" sentinel; a no-op otherwise. **This is also the function
  that resolves `SUB_96CE9`**, left unnamed in `command-parser-token-
  scan-and-plot-handler.md`'s "Still open" list - see that doc for the
  `FUNC_2998_5115`/`FUNC_2998_519A` keyword-table-walker context two
  of these call sites sit in.
- **`finalize_comm_rx_line_boundary`** (native `0x8EAAF`) - called
  from 2 sites, both inside `FUNC_2998_5022` (one of the 8 confirmed
  `fetch_next_comm_rx_char` callers above, not from `FUNC_2998_5115`/
  `FUNC_2998_519A` as an earlier pass in this section mistakenly said):
  calls `sync_comm_rx_read_ptr_and_release_hold` first, then promotes
  the pending LF-marker pair `[0x444]`/`[0x446]` into the CR-marker
  slot `[0x440]`/`[0x442]`, recomputes the "more data pending"
  sentinels from whether the write and read pointers still differ, and
  releases the critical flag.
- **`set_comm_rx_busy_for_mode(engage)`** (native `0x8EA89`) - a tiny
  RS-232-vs-GPIB dispatcher forwarding its argument to
  `set_comm_flow_hold` or `set_comm_queue_busy`. No resolved static
  caller.

**`[4]`** is the global "current input character" scratch cell that
the 8 lexer functions listed above read right after calling
`fetch_next_comm_rx_char`, which is its *only* confirmed writer.
This is the direct link: every ordinary received byte that the
pipeline hands to the parser passes through `[4]`, written by the
same function that's also the overflow-error handler - so the
"closes the incoming-data gap" claim from the previous section is
now true end-to-end, not just up to an unconnected buffer. (`[4]` is
also read elsewhere in the ROM - e.g. `FUNC_2998_1731`, see the
correction above - but those other readers aren't shown to get their
value from this pipeline, so they're out of scope for this proof.)

**Resolved, 2026-10-14, even later pass: the `[4]` hand-off, and a
correction - the destination is `[0x5A9]`, not `[0x586]`**

The question above ("where does a fetched character in `[4]` actually
end up?") is answered, but the premise was wrong: it never reaches
`[0x586]` (the GPIB command buffer `parse_next_gpib_command_byte`
reads from) at all. Found via `docs/comm-rom/command-parser-token-
scan-and-plot-handler.md`'s lead on `[0x5F4]`/`[0x5F0]` (that doc's
"Still open" list had flagged `FUNC_2998_50C2`/`FUNC_2998_50E4` as a
plausible token-length/accumulator mechanism, from a different,
earlier investigation thread into the same general address range):

- **`append_char_to_comm_token_buffer`** (native `0x850E4`) - if the
  token-length counter `[0x5F4]` is `< 0x47` (71), writes `[4]`
  through the far pointer at `[0x5F0]`:`[0x5F2]` - initialized
  elsewhere (`FUNC_2998_3E79`) to the literal far pointer
  `DS:0x5A9`, a dedicated scratch buffer, not `[0x586]` - advancing
  both the pointer and `[0x5F4]`; past the bound, sets error `0xB`
  into `[0x623]` and aborts via `[0x5A4]=1`. Called twice, both from
  `scan_quoted_arg_char`, immediately after `fetch_next_comm_rx_char`
  fills `[4]`. **This is the actual `[4]` -> token-buffer hand-off.**
- **`bump_comm_token_length_bounded`** (native `0x850C2`) - a sibling
  with the same error-on-overflow shape but a different bound
  (`0x64`=100) and no buffer write at all - purely tracks token/path
  length, called from two different functions (`FUNC_2998_42F4`,
  `FUNC_2998_43F0`) right after a non-`:` character is confirmed -
  i.e. counting overall command-path length, separate from the
  quoted-string buffer above.
- **`scan_quoted_arg_char`** (native `0x83F22`) - the caller of both
  `fetch_next_comm_rx_char` and `append_char_to_comm_token_buffer`: a
  per-character state machine (state in `[0x5FA]`) that scans a
  quoted SCPI string argument, handling the closing/doubled `"`
  delimiter and `^`-prefixed control-character escapes (`^` followed
  by `0x40`-`0x5F` maps to the control code `0x00`-`0x1F` at the same
  offset - classic caret-notation for embedding control characters in
  a string literal, e.g. `^M` for CR). No resolved static caller.

So `[0x5A9]` (not `[0x586]`) is where a validated character from a
*quoted string argument* lands - a separate scratch buffer from the
GPIB command-byte buffer. `[0x586]`'s own hand-off (how a resolved
GPIB command byte gets there) remains untraced, but that's now a
clearly separate question from this one.

**Resolved, same day, next pass**: `scan_quoted_arg_char` is no longer
callerless - it's entry 9 of `comm_call_main_rom`'s 24-entry
parser-state dispatch table at native `0x88060` (reached via `[0x5A4]`'s
indirect `lcall`, not a static call site at all). See
`docs/comm-rom/command-keyword-table.md` section 7 for the full
24-entry dump; most of this ROM's other "no resolved static caller"
functions turn out to be explained the same way. `init_comm_rx_
message_pipeline` (native `0x8E597`) and `set_comm_rx_busy_for_mode`
(native `0x8EA89`) remain genuinely unexplained - both fall outside
that table's address range (`0x83B1A`-`0x85098`). (`rearm_comm_rx_
consume_cursor` is **not** in this list either - see the correction
above, it has 12 confirmed callers.)
