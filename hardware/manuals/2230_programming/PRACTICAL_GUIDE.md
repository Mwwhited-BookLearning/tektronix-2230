# Tek 2230 RS-232 programming: a field-tested guide

`README.md` in this folder is a hand-transcription of the original
Tektronix programming manual (`hardware/Tektronix_2230_Programming_
Manual.pdf`) - it documents what Tektronix wrote in 1987. This file is
different: it's what actually works, confirmed against two real 2230
units over live RS-232 sessions, plus a few things the printed manual
doesn't cover at all (the real default encoding, the exact checksum
formula, response quirks, and what's now known about the comm ROM's
own command-parsing tables). Where this file and `README.md` disagree,
trust this one - it's backed by a transcript, not a scan.

Every claim below is tagged **[confirmed live]** (directly observed on
real hardware this session or a prior one), **[manual]** (documented
by Tektronix, not independently re-tested), or **[from ROM]** (derived
from disassembling `160-2998-14.bin`, not from either the manual or a
live response). See `disasm/NOTES.md` for the full ROM-side writeups
this guide summarizes.

## Getting a real connection working

**[confirmed live]** This is the part that actually blocked this
project for a full day, so it's worth stating plainly up front:

- **8N1, no hardware handshake needed for basic command/response
  use.** `FLOw OFF` (the power-on default) is required before any
  binary/hex `CURVe?` transfer - the manual is explicit that binary
  data and XON/XOFF flow control don't mix.
- **9600 baud is unreliable** - confirmed on **both** physical test
  units (different comm-ROM revisions, `-13` and `-14`), and unaffected
  by hardware flow control (still unreliable with DTR/RTS forced on,
  ruling that out as the cause). It produces a content-blind,
  misleadingly clean-looking `STATUS 98;READY;` for every single
  command, including ones that should always succeed (`ID?`,
  `EVEnt?`). This looks exactly like a real firmware problem and isn't
  one. **1200 and 4800 baud are both confirmed reliable on both units.**
  If you get `STATUS 98;READY;` for everything no matter what you send,
  try a slower baud rate before anything else - this is consistently a
  cable/UART-timing issue at the higher rate, not something DTR/RTS or
  the comm-ROM revision changes.
- **Rear-panel PARAMETERS DIP switch, baud rate nibble bit order:
  switch 4 = MSB (weight 8), switch 3 = weight 4, switch 2 = weight 2,
  switch 1 = LSB (weight 1).** It's easy to read this backwards (this
  project did, initially) since the switches are numbered 1-10 but the
  manual's own bit diagram labels them 4-3-2-1 for this nibble.
  | Code | Baud | Code | Baud | Code | Baud | Code | Baud |
  |---|---|---|---|---|---|---|---|
  | 0000 | 50 | 0100 | 150 | 1000 | 1800 | 1100 | 3600 |
  | 0001 | 75 | 0101 | 300 | 1001 | 2000 | 1101 | 7200 |
  | 0010 | 110 | 0110 | 600 | 1010 | 2400 | 1110 | 9600 |
  | 0011 | 134.5 | 0111 | **1200** | 1011 | 3600 | 1111 | Off-line |
  Confirmed 10-position switch strings (switches 1-10, `1`=up/on):
  `0111000000` = 1200 baud, `0011000000` = 4800 baud, `0110000000` =
  600 baud.
- Two physical test units in this project run **different comm-ROM
  revisions** (`160-2998-13` vs `-14`) but showed identical behavior
  for everything tested - the revision difference hasn't mattered in
  practice so far.

## Command syntax, the parts worth remembering

**[manual, confirmed live]**

- A command/query header's minimum abbreviation is exactly its
  capitalized letters (`STAtus?` → `STA?` works, `VMOde?` → `VMO?`
  works). Case doesn't matter at all - `id?`, `ID?`, `Id?` are
  identical.
- Every line is terminated with `<CR>` (`\r`), not `<LF>` and not just
  `;`. Semicolons separate/terminate *statements within* a response or
  a multi-argument command - they are not themselves the RS-232 line
  terminator.
- **Under Option 12 (RS-232), one command per line.** Don't try to
  chain `ID?;SET?` on one line the way you might over GPIB.
- **Query responses always echo the full, canonical keyword in caps**,
  not whatever abbreviation you sent - `STA?` gets back `STATUS 0;`,
  `VMO?` gets back `VMODE CH1;`. **[from ROM]** this is very likely
  backed by a real table in the comm ROM: a length-prefixed ASCII
  string for every top-level command header, plus a small index table
  that maps a numeric command ID straight to that string - see
  `disasm/NOTES.md`'s "Found the real command-keyword table" for the
  full byte-level evidence. The code that actually walks that table
  (i.e. the genuine keyword-matching function) hasn't been located in
  the disassembly yet.
- **A query response can substitute an inline `STATUS <code>;` for a
  single field's value** instead of failing the whole response, when
  that specific field can't currently be computed - e.g. `DELAY
  VALUE:STATUS 98;` when there's no valid delta-time measurement to
  report, or `SGLSWP STATUS 101;;`. **[confirmed live]** This isn't
  documented anywhere in the printed manual. If you're parsing
  responses programmatically, don't assume a field's text always
  matches the type its header implies - check for an embedded `STATUS`
  token first.

## Status bytes and event codes

**[manual, confirmed live]** `STAtus?`'s reply is a single decimal
number built from a category + 2 modifier bits (`R`=1 if `RQS ON`,
`X`=1 if busy). The ones actually seen on real hardware:

| Value | Meaning | Seen when |
|---|---|---|
| 0 | No status to report | Idle, nothing pending - the common case |
| 97 | Command Error (header not recognized), RQS on, not busy | Sent an invalid/misspelled header (`RS232?`, `LOCk?` as a top-level query, `ACQuire?` instead of `ACQuisition?`) |
| 98 | Execution Error, RQS on, not busy | Header recognized but couldn't execute (this is also what a whole session gets misleadingly stuck on at unreliable baud rates) |
| 101 | Execution Warning, RQS on, not busy | A field-level "can't compute this value right now" case (see above) |

**`STATUS 128` was seen exactly once, never reproduced.** 10
consecutive live `STAtus?` calls in a later session all came back
clean `0`. Every category in the manual's own status-byte bit layout
hardcodes bit 7 to 0 - there's no documented (or, as far as this
project has found, undocumented-but-real) code path that can set it.
Best explanation: a one-off transient serial-line glitch, not a real
instrument state. Don't be surprised if you never see it; don't panic
if you do once.

`EVEnt?` gives a more specific 3-digit code when `STAtus?` alone isn't
enough - `EVENT 0;` means nothing pending, anything else indexes into
the manual's Table 7-35 (`101`=command header error, `107`=invalid
message-unit delimiter, etc. - see `README.md` for the full table).

## Waveform transfer - all three formats confirmed live

**[confirmed live]** `DATa ENCdg:<ASCii|BINary|HEX>` picks the format
for `CURVe?`. **Contrary to what this project originally documented as
an open gap, BINARY (not ASCII) is the real power-on default** -
`DATa?` right after power-on reports `ENCDG:BINARY`. All three were
captured live from a real 1024-point acquisition and match the manual
exactly:

- **ASCII**: `CURVE 157,156,156,...,148;` - comma-separated `<NR1>`
  integers, one per point, terminated by `;`.
- **BINARY**: `CURVE %` + 2 raw bytes (byte count, **big-endian**) +
  1024 raw data bytes + 1 raw checksum byte + `;`. Byte count for a
  1024-point/1-byte-per-point acquisition is `0x0401` = 1025 (1024 data
  bytes + 1 checksum byte - the count includes the checksum, per the
  manual's wording).
- **HEX**: `CURVE #H` + 4 ASCII hex chars (byte count) + 2048 ASCII hex
  chars (2 per data byte) + 2 ASCII hex chars (checksum) + `;`. Same
  byte-count value, just hex-encoded.

**The checksum formula, confirmed against 4 independent live captures**
(different sweeps, different sums, all matched): two's-complement
modulo-256 sum of the byte-count field's bytes plus every data byte -
`(256 - (sum(count_bytes + data_bytes) % 256)) % 256`. This matches the
manual's one-line description (`WFMpre CRVchk:CHKsm0`) exactly, but
it's easy to get wrong by guessing (plain sum-mod-256, XOR, and
carry-folded one's-complement were all tried first here and all
**don't** match - it's specifically the two's complement of the
*uncarried* sum).

Data points are unsigned 8-bit in every format here (`min`/`max` of 64/
191 on a real captured 1kHz sine, consistent with `YOFF:0`,
`YMULT:80.0E-3`, an offset-binary-style unsigned range). `WFMPre?`
gives you `XINCR`/`YMULT`/`YOFF`/etc. to convert raw point values to
real volts/seconds - do that conversion yourself; the wire format
never sends physical units directly.

## Practical gotchas worth knowing before you script anything

- **Hardware flow control (RTS/CTS, DTR/DSR) isn't documented for this
  instrument, but some cables/adapters need it anyway.** The manual
  only documents *software* (XON/XOFF) flow control (`FLOw ON/OFF`) -
  no mention of RTS/CTS or DTR/DSR at all, so the 2230 itself isn't
  known to require or drive them. But some USB-serial adapters gate
  transmission on hardware CTS at the chip/driver level regardless of
  any software setting, and some DB9/DB25 adapters/cables jumper RTS
  straight to CTS (or DTR to DSR) so the adapter always reads "clear to
  send" without a real handshake partner. Both tools below leave these
  lines exactly as pyserial/the OS driver sets them by default (no
  explicit control) unless told otherwise - use `--show-lines` to see
  what a given cable/adapter actually presents (`CTS`/`DSR`/`CD`/`RI`
  readback), and `--rts on|off`/`--dtr on|off` to force a line, or
  `--rtscts`/`--dsrdtr` to turn on OS-level hardware flow-control
  gating, if a specific cable needs it. **If a command/plot silently
  gets nothing back on one adapter but works on another, this is worth
  checking before assuming it's another baud-rate issue.**
- **A cut-off `PLOt` leaves the scope "busy."** While a plot is in
  progress the instrument ignores every command except `PLOt ABOrt` -
  if your own tooling times out and gives up mid-plot, the *next*
  command you send (even something unrelated like `ID?`) will get
  `STATUS 97;READY;` until you send `PLOt ABOrt` first. Any script that
  talks to this scope should send an unconditional `PLOt ABOrt` before
  starting, every time - see `disasm/plot_hpgl_to_svg.py` and
  `disasm/scope_rs232.py`, both of which do this.
- **HPGL plots only include a real waveform trace when `STORe` is ON.**
  With `STORe OFF` (continuously-refreshing live trace), a `PLOt`
  request still transmits the graticule and all on-screen readout text
  (channel labels, cursor deltas, menu prompts - genuinely rendered as
  HPGL vector strokes, one command per line segment) but the trace
  itself is silently omitted. Turn `STORe` on (front panel or `STORe
  ON`) before plotting if you want the waveform in the output.
- **A big response takes real transmission time at low baud.** A full
  1024-point ASCII `CURVe?` reply is ~3.7KB of text; at 4800 baud
  that's genuinely ~8 seconds of transfer, and idle-timeout-based
  "wait until the line goes quiet" capture logic needs a generous
  timeout (a few seconds) or it'll cut the response short mid-transfer.
- **Not every guessable header exists.** `RS232?`, `LOCk?` (as a
  top-level query), `ACQuire?`, `DISplay?`, and bare `SAVE?` all return
  `STATUS 97;` (Command Error) - the real headers are (respectively)
  not a standalone query at all, an *argument* keyword used inside
  other commands (not its own query), `ACQuisition?`, no equivalent at
  all, and `SAVeref?`. `HELp?`'s response is the authoritative list of
  every real top-level header; when in doubt, check it live rather
  than guessing from the abbreviation shown in a command's own
  argument syntax.

## Tools in this repo

- `disasm/scope_rs232.py` - send one command (`--cmd`) or a whole batch
  file (`--batch-file`, one command per line) to a live scope over
  RS-232, with per-command idle-timeout-based response capture and
  optional transcript logging (`--log`). Defensively sends `PLOt ABOrt`
  first. Use `--raw-out` to save a single response's exact bytes (needed
  for binary/hex `CURVe?` data).
- `disasm/plot_hpgl_to_svg.py` - request an HPGL plot, save the raw
  HPGL, and render it to an inspectable SVG. Defaults to 1200 baud
  (the first rate confirmed reliable); pass `--baud 4800` if that's
  what the DIP switches are set to.

Both default to `COM3`; override with `--port` for a different setup.
Both also share the same `--rtscts`/`--dsrdtr`/`--rts`/`--dtr`/
`--show-lines` flags for hardware flow control - see the gotcha above.
