# RS-232: live session, waveform formats and the command-keyword table

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Live session, 2026-09-14 (continued): binary/hex CURVe? formats confirmed live; found the real command-keyword table; STATUS 128 not reproduced

With the scope left connected at 4800 baud (confirmed reliable, see
above), ran a broad live RS-232 exploration using two new small tools,
`disasm/scope_rs232.py` (send one command or a batch file, log
responses) and reusing `disasm/plot_hpgl_to_svg.py`.

**Confirmed live: all three `DATa ENCdg` waveform-transfer formats,
byte-exact against the manual's documented format AND checksum
algorithm.** This closes the "only the ASCII path
(`print_signed_decimal_serial`/`print_param_list_response`) has been
identified in code so far - find the binary/hex one(s)" open item from
the *protocol* side (still not tied to a specific disassembled routine,
but no longer a documentation gap about what the wire format actually
is):

- **ASCII**: `CURVE 157,156,156,...;` - 1024 comma-separated `<NR1>`
  values, range 64-191 (consistent with `WFMPre?`'s `YMULT:80.0E-3`,
  `YOFF:0`, an 8-bit unsigned point format `PT.FMT:Y`).
- **BINARY**: `CURVE %<2-byte count><1024 data bytes><1-byte
  checksum>;` - byte count `0x0401`=1025 (**big-endian**, = 1024 data
  bytes + 1 checksum byte, exactly per the manual's "`<Binary Count
  MSB/LSB>` ... plus the 1-byte checksum" wording).
- **HEX**: `CURVE #H<4 hex chars byte-count><2048 hex chars
  data><2 hex chars checksum>;` - same byte count field, ASCII-hex
  instead of raw binary, exactly per the manual.
- **Checksum formula confirmed exactly, live, across 4 independent
  captures (different sweeps, different sums)**: two's-complement
  modulo-256 sum of the byte-count field bytes + all data bytes
  (`hardware/manuals/2230_programming/README.md` line 346's `WFMpre
  CRVchk:CHKsm0` description) - `(256 - (sum(byte_count_bytes +
  payload) % 256)) % 256`, verified to match the actual trailing
  checksum byte on all 4 binary captures and the 1 hex capture taken
  this session. Naive sum-mod-256, XOR, and one's-complement-with-carry
  candidates were all tried first and **do not** match - the two's
  complement of the *plain* sum (not carry-folded) is the right
  formula.
- `WFMPre?`'s default (`DATa?`'s `ENCDG:BINARY`) confirms **BINARY, not
  ASCII, is the actual power-on default** - matches the manual's `DATa
  ENCdg` row exactly (`Power-on default BINary`).

**Investigated the `STATUS 128` anomaly (open item from the 09-14
"RESOLVED" session) - not reproduced, and no ROM code path found that
could produce it.** 10 consecutive live `STAtus?` calls this session
all returned a clean `STATUS 0;`. Table 7-34's own bit-layout notation
(`0R1X ????` / `010X 0001` / `000X 0000`) hardcodes bit 7 to `0` in
*every* documented category - there is architecturally no row that
produces a value with bit 7 set. Without a repro, there's no specific
instruction to point to; **best available explanation remains a
one-off transient serial-line glitch** (a single flipped top bit),
consistent with this project's own baud/cable-reliability findings
elsewhere in this file - not a new firmware mystery, but also not
something the ROM's documented status-composition logic could have
produced deliberately. Also noticed and worth separately noting: some
value fields (`DELAy?`, `DELTAV?`, `REFOrmat?`, `SGLswp?`) substitute
an inline `STATUS <code>;` string for a specific field's value when
that field can't currently be computed (e.g. `DELAY VALUE:STATUS 98;`
when no valid delta-time measurement is available), rather than
failing the whole response - a real, previously-undocumented "partial
failure" formatting behavior worth keeping in mind when parsing any
response programmatically (a field may not be the type the query
header implies).

**Found the real command-keyword table** (comm ROM `160-2998-14.bin`,
file offsets `0x8A59`-`0x8F1D` for the string data, `0x8D4C`-`0x8DE6`+
for an index/dispatch table pointing into it) - this is very plausibly
the "downstream keyword-matching function" TODO.md has listed as
unfound since early sessions, or at least its backing data table. **The
sampled entries below are illustrative only - see
[`command-keyword-table.md`](command-keyword-table.md) for the complete
extracted contents of all three structures (110 argument keywords, 26
dispatch records, 45 header entries), including 4 header entries that
turned out not to match any live `HELp?` response and are still an
open question:**

- **Two adjacent length-prefixed ASCII string tables** (`[len_byte]
  [ASCII text]`, confirmed against `disasm/gen_strings.py`'s own
  length-prefix detection): one holds every **top-level command
  header** (`ACQuisition`, `ATRigger`, `CH1`, `CH2`, `CURSor`, `CURVe`,
  `DATa`, `DELAy`, ..., `WFMpre` - file offsets `0x8DE7`-`0x8F16`),
  **byte-for-byte identical to the live `HELp?` response** captured
  this session. The other (file offsets `0x8A59`-`0x8CB1`ish) holds
  **argument/value keywords** used *inside* command bodies - `ABOrt`,
  `ASCii`, `BINary`, `BN.Fmt`, `CHKsm0`, `ENCdg`, `GRAt`, `HPGl`,
  `PEAkdet`, `SAMple`, `TARget`, `VOLts`, `XMUlt`, `YMUlt`, etc. - every
  one matching a real argument name from the programming manual/live
  responses.
- **A 6-byte-per-entry index table immediately before the header-string
  table** (`0x8D4C` onward: `[id_byte][0xFF marker][2-byte
  little-endian offset][2-byte segment, fixed 0x90DE]`) whose far
  pointers, decoded exactly (`segment*16 + offset`, then resolved
  through the already-confirmed `0x90000-0x97FFF` address-decode alias
  back to the real ROM, see "The 0x90000+ region: fully resolved"
  above), **land exactly on the `[len_byte]` position of a real command
  keyword's own table entry** for every non-zero id - verified for
  `ACQuisition`, `BYTe`, `CH1`, `DATa`, `ERRor`, `FLOw`, `HELp`, `ID`,
  `JMP`, `LONg`, `MESsage`, `OPC`, `PLOt`, `REAdout`, `SAVeref`,
  `TRIggerd`, `VMOde`, `WAVfrm`. `id=0x00` entries (recurring
  periodically) all point to one shared `0x00` byte (file offset
  `0x8F1C`) - a **NULL/empty-string sentinel**, not a real command; a
  literal `ZZZZ` entry (`id=0x2C`) closes the table out, confirming its
  end boundary. Net effect: **given a numeric command ID, this table
  yields a far pointer straight to that command's canonical uppercase
  keyword text** - exactly what's needed to build the `ACQUISITION
  ...`/`ATRIGGER ...`/`VMODE CH1;`-style canonical headers seen in
  every live query response this session (the response header is
  always the *full* keyword in caps, regardless of how much was
  abbreviated in the request - this table is the natural mechanism for
  that).
- **Not yet found**: the code that actually *walks* this index table
  (assigns/receives the numeric command ID and dereferences the record).
  A grep for the literal segment value (`0x90DE`/`0x88DE`, both
  spellings) across the already-disassembled `.asm` found zero hits -
  the far pointer's 4 bytes only ever appear as *data* in this table,
  never as an instruction immediate, so whatever code reads it must
  either compute the segment dynamically (e.g. loading it from a
  variable rather than a literal) or live in a not-yet-disassembled/
  unreached region of the comm ROM. Tracing forward from wherever the
  as-yet-unfound UART-receive entry point lands (see the still-open
  TODO item) to find what computes the numeric command ID in the first
  place is the natural next step - this table strongly implies that ID
  exists as a real intermediate value somewhere in the parse path, it's
  just not been caught yet.
- Tooling used: `disasm/gen_strings.py`'s existing length-prefix
  detection got the string list; the index-table structure was found
  by brute-forcing byte alignment against the known-fixed `0xDE,0x90`
  segment suffix (6 candidate offsets tried, one gave 26/28 clean
  6-byte-aligned matches) - the same "brute-force every candidate
  offset/alignment and keep whichever maximizes exact structural
  matches" technique already noted elsewhere in this file for the
  `0x90000` alias discovery.

## `STATUS 128` reproduced a second time - now correlated with a DTR/RTS line-state transition, not just "transient noise"

After adding explicit hardware-flow-control support to both RS-232
tools (`--rts`/`--dtr`/`--rtscts`/`--dsrdtr`/`--show-lines` - the user
reported needing DTR/RTS asserted in hterm on some cable/adapter
combinations), the very first live command sent with `--rts on --dtr
on` explicitly forced for the first time this session produced
`STATUS 128;` again - the second independent sighting of this
previously-"not reproduced" anomaly (see above).

**Follow-up testing narrows the correlate, but doesn't fully explain
it:**
- 5 immediately-following `STAtus?` calls (separate process/connection
  each time, same `--rts on --dtr on` flags) all came back clean
  `STATUS 0;`.
- A further 10 rapid reconnect-and-query cycles (same flags, back to
  back) were **also all clean** - so "fast reconnect cadence" alone
  doesn't reproduce it either.

The one thing that *was* different about the triggering call: it was
the first command in this session sent with `--rts on --dtr on`
explicitly asserted, immediately following an `--show-lines` call that
read the port's modem-status lines right after opening (an extra
driver-level query touching the port at connect time), which itself
followed a `SET?` call in yet another fresh connection. **Working
theory, still unconfirmed**: a real voltage transition on the DTR/RTS
control lines (going from whatever they'd been left at to explicitly
asserted high, for the first time) briefly coupled noise onto the RX
line on this cable, corrupting one incoming byte right around that
transition - not a firmware defect, and not reproducible by simply
reconnecting or querying rapidly once the lines are already stable.
Still short of proof (would need to deliberately toggle DTR/RTS
low-then-high mid-session and see if it reproduces on demand), but this
is a more specific, physically-grounded theory than plain "transient
glitch," and rules out pure reconnect-timing as the cause.

## Tried tracing `MESsage <NR1>:"..."` to the drawing code (and from there, backward to the stroke-font table) - dead end, but a useful one

The idea (a good one, worth recording why it didn't pan out): `MESsage`
is a real, documented RS-232 command
(`hardware/manuals/2230_programming/README.md` line 236) that writes
arbitrary text directly to a readout row. If its handler could be
found and it calls straight into the already-confirmed readout-drawing
primitives (`print_readout_string`/`draw_readout_char`, the same ones
that reference the still-unlocated stroke-font pointer `[0x1DB0]`),
that would both nail down the handler *and* hand the stroke-font hunt
a second, independent way to make progress (e.g. tracing forward from
a known caller instead of scanning ROM bytes blindly).

**Exhaustively checked, and it's not a direct call.** Wrote a
byte-level scan (not a text grep, which would miss equivalent
segment:offset spellings of the same physical target) that finds every
literal far-`CALL` instruction (opcode `0x9A`, 5-byte encoding) across
**all three ROM chips' raw bytes** and resolves its target physical
address directly, independent of what's proven-reached or not. Checked
against `print_readout_string` (`0xE3821`), `draw_readout_char`
(`0xE3854`), `print_char` (`0xE0B2A`), `write_readout_port_byte`
(`0xE0B50`), `append_readout_char` (`0xE39F0`), and `print_string_far`
(`0xE0AF5`):

- **40 real call sites found, all in `160-3633`, zero in `160-3532` or
  the comm ROM (`160-2998`) in either its direct or aliased mapping.**
- **Every single one of the 40 resolves into an already-identified,
  self-test/boot-banner-related function**: `print_selftest_report_
  line`, `print_string_far` itself, `print_char`, `print_scratch_
  buffer_range`, `print_readout_string` itself (recursion), `run_
  selftest_sequence`, `print_boot_rom_id_banner`, `print_selftest_
  banner`, `print_banner_line`, `step_readout_window_pattern`, `report_
  screen_mode` - none of these are a generic "print a caller-supplied
  string at a caller-supplied row" utility; they're all fixed
  self-test/startup text.

**Conclusion: the comm ROM never directly calls any readout-drawing
primitive anywhere in its own code.** Whatever handles `MESsage` (and,
by extension, every other RS-232 command that changes instrument
state - `ACQuisition`, `CH1 VOLTS`, etc.) must go through some
intermediary that isn't a literal `lcall` to a fixed address - most
plausibly the same still-unfound mechanism as "who walks the numeric-
command-ID dispatch table" from earlier this session
(`docs/comm-rom/rs232-live-session-2026-09-14.md`'s command-keyword-
table section) or `docs/comm-rom/rs232-flow-control-and-open-puzzle.md`'s
cross-ROM call puzzle - a shared-memory mailbox/flag polled by the main
ROM's task scheduler is the leading architectural guess (matching how
decoupled the comm ROM and main ROM otherwise are throughout this
project's findings), but genuinely unconfirmed. **This didn't find the
stroke-font table**, but it does rule out the one specific path this
idea proposed, and strengthens the case that solving "who walks the
command-ID table" would likely unlock *both* mysteries at once, not
just the RS-232 one - worth prioritizing for that reason.
