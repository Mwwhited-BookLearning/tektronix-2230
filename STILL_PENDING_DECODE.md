# What's still undecoded

A status board for the reverse-engineering effort: everything that's
still an open unknown in the firmware, organized by area, with enough
context to pick any one of them up cold. This is a *technical
inventory of gaps*, not a task list - see `TODO.md` for prioritized
action items (many of these map 1:1 to a `TODO.md` entry; some don't
because they're lower-priority background gaps rather than active
work). Update this whenever an item here gets resolved or a new one is
found - it should track reality, not just grow.

## The big picture: how much of the ROM is actually understood

| | proven (call-graph verified) | + heuristic (prologue-scan, lower confidence) |
|---|---|---|
| `160-3633` (main ROM low half) | 37.4% | 89.9% |
| `160-3532` (main ROM high half) | 7.6% | 90.6% |
| `160-2998` (comm ROM) | 13.0% | 91.7% |
| **all 3 chips** | **19.3%** | **90.7%** |

Full methodology in `docs/architecture/validation-and-coverage.md`.
Three things to take from this:

1. **The ~19% "proven" slice is the only part anyone should fully
   trust.** It's reached by actual recursive descent from confirmed
   entry points (reset vector, interrupt vectors, proven call sites) -
   every instruction in it is genuinely known to execute.
2. **The ~71% "heuristic-only" slice (90.7% minus 19.3%) is a strong
   guess, not a fact.** It's found by scanning for the `55 8B EC`
   (`push bp; mov bp,sp`) function-prologue signature and assuming
   anything that looks like a function start is one - true almost
   always, but not proven reachable, and essentially all of the
   "renaming" work so far has deliberately stayed within the proven
   slice for that reason.
3. **~9.3% (~18,250 bytes across all 3 chips) is genuinely unreached by
   either method** - almost certainly a mix of data tables (further
   string/glyph/menu tables like the ones already found) and truly
   dead or unreachable code. Nothing has systematically catalogued
   this remainder byte-by-byte yet.

## Comm ROM / RS-232 command parsing

The single biggest cluster of open items - see
`docs/comm-rom/rs232-early-investigation.md`,
`docs/comm-rom/rs232-breakthrough.md`, and
`docs/comm-rom/rs232-live-session-2026-09-14.md`.

- **The genuine UART-receive entry point is resolved (2026-10-14,
  corrected same day)**, end to end. The producer is the comm ROM's
  already-documented `poll_comm_status_tick` (`FUNCTIONS.md`, raw/
  alias `0x96F54` / native `0x8EF54`) - a same-day pass independently
  re-found it under its native address before recognizing the
  duplicate; it reads the UART status/interrupt-latch
  byte via far pointer `[0x6DA]` and, on its data-ready bit, reads the
  actual received byte (plus two more chip registers) via `[0x6D6]` -
  a plain `mov` on the memory-mapped register (the comm ROM has no
  genuine `in`/`out` instruction at all; the UART is memory-mapped,
  not port-mapped) - into the already-documented RX ring buffer
  (`[0x464]`/`[0x468]` pointers, base `0x46C`, size `0x80`, 4-byte
  records: data + 2 registers + status). What's genuinely new: the
  consumer, `consume_comm_rx_ring_buffer` (native `0x8F12B`), drains the buffer and
  dispatches on translated event-type bits; a sibling dispatcher
  `dispatch_comm_rx_status_bits` (native `0x8F5AC`), keyed on the captured status
  byte, hands off directly into the already-documented `parse_next_
  gpib_command_byte` - closing the loop into `[6]`/`[0x580]`'s
  classification/dispatch path. Also newly resolved: scheduler task
  slot 5's target (`[0x73C]/[0x73E]`, `docs/interrupts/task-
  scheduler.md`) is conditional on `[0x1BFA]` (the COMM_LOOPBACK
  self-test readback flag) - `consume_comm_rx_ring_buffer` when zero (the common
  case), `dispatch_comm_rx_status_bits` when nonzero. (`[0x740]`/`[0x742]`, also set
  by `init_comm_dispatch_table`, was initially miscast as a third,
  unmatched scheduler-slot pointer - it's actually the already-
  resolved `run_continuous_selftest_tick` `INT255` hook, unrelated to
  the 12-slot task table.) Full trace, including the correction, in
  `docs/comm-rom/rs232-flow-control-and-open-puzzle.md`'s "Found,
  2026-10-14" section and `changes/2026-10-14.md`. **Functional
  naming completed, same day, later pass**: `consume_comm_rx_ring_
  buffer`, `dispatch_comm_rx_status_bits`, and the event-type-
  translation helper `translate_comm_rx_event_type` (native
  `0x8F54C`) are now real `FUNCTIONAL_NAMES` entries, not just prose -
  see `FUNCTIONS.md`.
- **Resolved, 2026-10-14, later pass**: the genuine incoming-data
  message buffer (the gap flagged in this same file's earlier entry
  and in `docs/comm-rom/rs232-flow-control-and-open-puzzle.md`'s
  "Direction correction" section) is found. `consume_comm_rx_ring_
  buffer`'s two dispatch targets, previously unnamed `SUB_97253`/
  `SUB_97583` (no disassembled body in any listing - reached only via
  the comm ROM's `0x90000`-alias window, which `gen_disasm_2998.py`'s
  `render_comm_only()` filters out of its rendered output; worked
  around with a one-off raw-byte capstone disassembly), are now
  `process_comm_rx_data_byte` (native `0x8F253`) and `resume_comm_tx_
  queue_if_pending` (native `0x8F583`) - real `FUNCTIONAL_NAMES`
  entries. `process_comm_rx_data_byte` is the full RS-232 RX pipeline
  (parity strip, XON/XOFF flow control, CR/LF line framing, enqueue
  into a new RX message buffer `[0x434]`/`[0x436]`/`[0x43C]`);
  `resume_comm_tx_queue_if_pending` is its XON-handling counterpart.
  Full writeup in `docs/comm-rom/rs232-flow-control-and-open-puzzle.
  md`'s "Resolved, 2026-10-14" section and `VARIABLES.md`'s new rows.
  **Still open**: `dispatch_comm_rx_status_bits`'s own two targets,
  `SUB_976C0` (`0x20`) and `SUB_9785B` (`0x10`), were only partially
  traced the same way - lower confidence, not yet named. `SUB_976C0`
  looks like a lazy-init/bus-reset-detection routine that also signals
  a UART register on RX-buffer-full (plausibly RTS-deassert hardware
  flow control); `SUB_9785B` closely parallels `service_comm_tx_
  queue`'s dequeue-and-transmit logic behind an unexplained flag
  chain `[0x455]`-`[0x458]`. Needs either further tracing or
  continued documentation as a lead - see `VARIABLES.md`'s
  `[0x452]`-`[0x458]` row.
- **Resolved, 2026-10-14, even later pass**: the RX message buffer's
  consumer chain (left open by the entry above) is now fully traced,
  and it's proven the buffer feeds the real SCPI/GPIB parser for
  ordinary traffic, not just the overflow-error path. Six new
  `FUNCTIONAL_NAMES` entries: `init_comm_rx_message_pipeline` (native
  `0x8E597`), `fetch_next_comm_rx_char` (`0x8E99B` - the pull-style
  "get next character" primitive, confirmed called directly from
  inside the argument lexer immediately before it reads the shared
  scratch cell `[4]`), `sync_comm_rx_read_ptr_and_release_hold`
  (`0x8E8A2`), `rearm_comm_rx_consume_cursor` (`0x8ECE9`),
  `finalize_comm_rx_line_boundary` (`0x8EAAF`), and `set_comm_rx_
  busy_for_mode` (`0x8EA89`). Also resolves the earlier "odd non-zero
  wrap origin of `0xF`" puzzle - it's just the buffer's base offset.
  Full writeup in `docs/comm-rom/rs232-flow-control-and-open-puzzle.
  md`'s "even later pass" section and `VARIABLES.md`'s `[0x438]`/
  `[4]`/`[0]`-`[3]` rows. **Correction, same day, later re-check**: the
  call-site claims above were re-verified by enclosing function rather
  than address proximity. `fetch_next_comm_rx_char`'s real callers are
  8 functions spanning `FUNC_2998_1909`..`FUNC_2998_5022` (not
  `FUNC_2998_1731`/`FUNC_2998_5115`/`FUNC_2998_519A`, which turned out
  to be unrelated siblings); `finalize_comm_rx_line_boundary`'s only
  callers are 2 sites inside `FUNC_2998_5022`; and
  `rearm_comm_rx_consume_cursor` is **not** an uncalled entry point -
  it has 12 confirmed call sites across 9 functions (including
  `FUNC_2998_5115`/`FUNC_2998_519A`, which is also how it resolves that
  doc's previously-unnamed `SUB_96CE9`). See `docs/comm-rom/rs232-
  flow-control-and-open-puzzle.md`'s corrected call-site listings for
  the full detail. **Resolved, same day, even later pass**: the `[4]`
  hand-off is traced - but not into `[0x586]`. `scan_quoted_arg_char`
  (native `0x83F22`) calls `fetch_next_comm_rx_char` then forwards the
  validated `[4]` into `append_char_to_comm_token_buffer` (native
  `0x850E4`), which writes it through far pointer `[0x5F0]`/`[0x5F2]`
  into scratch buffer `DS:0x5A9` - a dedicated quoted-string-argument
  token buffer, separate from the GPIB command buffer `[0x586]`.
  Sibling `bump_comm_token_length_bounded` (native `0x850C2`) tracks a
  different, buffer-less length bound for the overall command path.
  This also resolved two items from `command-parser-token-scan-and-
  plot-handler.md`'s "Still open" list (`FUNC_2998_50C2`/
  `FUNC_2998_50E4`). Full writeup in `docs/comm-rom/rs232-flow-
  control-and-open-puzzle.md`'s matching section and `VARIABLES.md`'s
  `[0x5F4]`/`[0x5F0]`/`[4]` rows. **Still open**: `[0x5A9]`'s own
  reader (no code anywhere reads back through `[0x5F0]`/`[0x5F2]`);
  `[0x586]`'s hand-off, now confirmed as a separate question from this
  one. **Resolved, same day, next pass**: `scan_quoted_arg_char`'s "no
  resolved static caller" status is explained - it's entry 9 of the
  `comm_call_main_rom` 24-entry parser-state dispatch table at native
  `0x88060` (full dump in `docs/comm-rom/command-keyword-table.md`
  section 7), reached only via `[0x5A4]`'s indirect `lcall`. Still
  genuinely unexplained by any known mechanism: `init_comm_rx_message_
  pipeline` (native `0x8E597`) and `set_comm_rx_busy_for_mode` (native
  `0x8EA89`) - both fall outside that table's address range
  (`0x83B1A`-`0x85098`).
- **The code that walks the command-keyword dispatch table is
  unfound - now confirmed to block a second investigation too.** A
  real command-keyword table was found live on 2026-09-14 (comm ROM
  file offsets `0x8A59`-`0x8F1D`, matching the live `HELp?` list
  byte-for-byte) plus a 6-byte-per-entry index table resolving numeric
  command IDs to far pointers into it - fully extracted in
  `docs/comm-rom/command-keyword-table.md` (110 argument keywords, 26
  dispatch records, 45 header entries). A grep for the far pointers'
  literal segment value found **zero** hits in the disassembled code -
  whatever assigns the numeric command ID and walks this table either
  computes the segment dynamically or lives in an unreached region.
  This is very likely the last missing piece of "how does the parser
  turn `STA?` into a recognized command." **Also confirmed 2026-09-14**:
  an exhaustive byte-level scan proved the comm ROM never directly
  calls any readout-drawing primitive (`draw_readout_char`/`print_
  readout_string`/etc.) anywhere - so the same unfound dispatch
  mechanism is also what's blocking "trace the RS-232 `MESsage`
  command to the drawing code" as a way into the stroke-font hunt (see
  `docs/comm-rom/rs232-live-session-2026-09-14.md`'s "Tried tracing
  `MESsage`" section). Solving this one item would likely unlock both.
  Also unresolved: only 26 of the header table's 44 real entries have a
  dispatch record in the slice found so far (a second/wider table may
  exist), and 4 header-table entries (`ADDress`, `BYTe`, `JMP`,
  `SEGment`) don't match any live `HELp?` response at all.
  **Progress 2026-09-14**: found where the numeric command ID actually
  gets *consumed* at runtime - `[0x732+0x1F]` (a byte field on the far
  pointer `[0x732]`, itself read via `les di,[0x732]` at 89 separate
  sites across the comm ROM, by far the most-referenced comm-ROM
  variable found so far - see `VARIABLES.md`). Confirmed via a new
  function, `compute_response_format_flags` (`0x85C9D`), which matches
  this field against `0x14`/`0x15`/`0x1E` (`LONg`/`MESsage`/`REFStat`,
  confirmed against the keyword table's own ID order) to flag commands
  needing quote-aware response formatting. **Still not found**: who
  *writes* `[0x732+0x1F]` in the first place (i.e. who assigns the ID
  after parsing an incoming command) - that would be the actual
  dispatcher entry point. Searched exhaustively for it (every byte/word
  write form via the two confirmed base registers, immediate and
  register sourced, plus a nearby `rep movsb`/`movsw` bulk-copy, plus
  every alternate way of loading `[0x732]` itself) - genuinely not
  found by any of these; likely lives in the unreached ~8% of the comm
  ROM, or is set via an operand this project hasn't thought to search
  for yet. Also **ruled out a promising-looking false lead**: a bigger
  `cmp ax,<id>` cluster at `0x8752A` (12 consecutive alphabetical IDs)
  turned out to read a *different* variable (`[0x3DA]`) and is actually
  an error-code-to-message dispatcher, not the command dispatcher - see
  `docs/comm-rom/rs232-live-session-2026-09-14.md`'s follow-up sections
  for both the full negative-result writeup and the reusable technique
  (search `cmp ax, <id>` against each of the other ~37 known command
  IDs) for whoever picks this up next.
  **Resolved 2026-10-11 (most likely explanation)**: decoding the rest
  of `init_far_pointer_table` (comm ROM, `0x80133`) found that `[0x732]`
  itself is initialized at boot to raw far pointer `0041:1b64` - under
  this project's own flat-`DS=0x41` variable-naming convention, that
  *is* `[0x1B64]`, the already-documented `find_item_table_indices`
  index (completely unrelated subsystem, main ROM). Consequently
  `[0x732]+0x1F` resolves to physical `0x410+0x1B64+0x1F = 0x1F93`,
  which is **exactly** `[0x1B83]` under the same convention - the
  already-fully-documented comm-option-hardware-detection-result byte,
  written by `detect_comm_option_hw` to `0x14` or `0x1E` (never `0x15`).
  This explains the exhaustive-search failure above: the real write
  site is in the main ROM, under a completely different name and
  instruction form (`mov byte ptr [0x1b83], imm8`), not anything in the
  comm ROM. Confirmed by direct arithmetic/byte-match, not speculative.
  **Still open**: `compute_response_format_flags` also checks for
  `0x15` (`MESsage`) on this same field, but no write of `0x15` to
  `[0x1B83]` has been found anywhere - either that branch is dead code,
  or a second, undiscovered write path exists. See `docs/comm-rom/
  command-keyword-table.md` section 7 and `VARIABLES.md`'s `[0x732]`/
  `[0x1B83]` rows for the full writeup.
  **Candidate found 2026-10-09, still not confirmed**: `FUNC_2998_5115`/
  `FUNC_2998_519A` (`0x85115`/`0x8519A`, both directly `lcall`'d from
  `comm_call_main_rom`'s own body) walk a 26-entry, first-letter-
  bucketed, 6-byte-record table and resolve a match into `[0x604]` -
  confirmed to land on this project's own `id=0x17`→`PLOt` value via a
  later comparison inside `FUNC_2998_44D0`. Blocked on the same pattern
  as `[0x732+0x1F]` above: neither candidate's table-base far pointer
  (`[0x6EA]` for `5115`, `[0x6F6]` for `519A`) has a confirmed write
  site - `[0x6EA]` has none found at all, and `[0x6F6]`'s only writes
  are inside an apparently unrelated main-ROM routine
  (`reset_all_channel_plot_caches`) setting it to a sentinel value,
  which may be coincidental address reuse rather than this table's real
  initialization. See `docs/comm-rom/command-parser-token-scan-and-
  plot-handler.md` for the full writeup. Also ruled out this session:
  `FUNC_2998_44D0` (walked looking for the `PLOt FORmat` write site
  below) is a generic, multi-command argument-type validator, not a
  PLOt-specific handler - it never writes `[0x461]` on any path.
  **Resolved 2026-10-11**: both write sites exist after all, found by
  decoding `init_far_pointer_table` (comm ROM, `0x80133`) in full - it
  writes `[0x6EA]`/`[0x6F6]` via a register-indirect `ES:DI` copy loop,
  invisible to the literal-displacement-instruction grep used above.
  `[0x6EA]` resolves to `command-keyword-table.md` section 2's own
  dispatch/index table (whose documented start was itself off by 2
  phantom records, corrected `0x8D3E`→`0x8D4A` as part of this fix) -
  not a second table. `[0x6F6]` resolves to a genuinely new,
  previously-undocumented second 26-entry letter-bucket table
  (physical `0x889BC`) indexing into table 1 instead of table 3. Both
  confirmed via full 26-slot decode: non-empty slot `N`'s resolved text
  starts with `chr(0x41+N)` in every case. See `command-keyword-
  table.md` section 6 and `VARIABLES.md`'s `[0x6EA]`/`[0x6F6]` (comm
  ROM) row for the full writeup.
- **`[0x1B83]`'s exact bit semantics.** Confirmed as the general-
  purpose "Time Base Mode Register U4119" (not comm-specific), and
  `detect_comm_option_hw` treats `0x1E` vs `0x14` as the "comm
  installed" test, but *why* those two specific values mean what they
  mean isn't confirmed. See `docs/comm-rom/option-detection.md`.
- **`STATUS 128` (seen twice via `STAtus?`, both times right after a
  DTR/RTS line-state transition).** Table 7-34's status-byte bit
  layout hardcodes bit 7 to 0 in every documented category, so no known
  code path can produce it. The second sighting narrowed the likely
  cause to a real voltage transition on the DTR/RTS control lines
  briefly corrupting one incoming byte on that cable, rather than pure
  random noise - 10 rapid reconnect-and-query cycles with the lines
  already stable reproduced nothing. Still not proven (would need a
  deliberate mid-session DTR/RTS toggle-and-observe test), but not a
  ROM code path either way. See `docs/comm-rom/rs232-live-session-2026-
  09-14.md`.
- **Comm ROM revision `-13` has never been formally disassembled into
  this project's tooling.** `-13` and `-14` differ in exactly 3 small
  byte runs (133 bytes total: the expected header, plus 2 runs each
  starting at a 16KB-page boundary), now hand-decoded and documented
  in `docs/comm-rom/revision-13-vs-14-diff.md` - `-13` has a small
  unnamed config-check routine and lacks `-14`'s page-3 boot-stub
  jump, but neither has been added to `gen_disasm_2998.py`. One of the
  two physical test units runs `-13`. No longer motivated by a
  suspected defect (both revisions behave identically over live
  RS-232), but still a real documentation gap.
- **New lead found 2026-09-15, unconfirmed**: `160-2998` file offset
  `0x80EC`-`0x8211` (physical `0x880EC`-`0x88211`) contains a clean
  record table - `(group, subgroup, incrementing 16-bit id)` - where
  `group` takes only 5 distinct values (`2,3,4,5,8`). Tentatively
  suspected of being related to the manual's Table 7-34 status/event
  categories (comm ROM, small handful of category values, same
  general shape) but the numbers don't line up cleanly enough to
  claim a match. See `docs/decode-anomalies/unknown-data-deep-dive-
  2026-09-15.md` finding 5.
- **`compute_parity_mode_code` (comm ROM) calls the main ROM's
  `scale_and_plot_point_default`** using the exact save/restore
  argument shape normally used for a DS-segment-switch helper, but the
  actual target is confirmed to be the plot-scaling function. Whether
  this is deliberate reuse of a shared math primitive or something
  else is unresolved. See `docs/comm-rom/rs232-flow-control-and-open-
  puzzle.md`.
- **`[0x629]` (GPIB/RS-232 mode select)** - **resolved 2026-10-09**:
  confirmed NOT switch-sourced at all. `poll_dip_switch_change` derives
  it from a before/after toggle-and-XOR test of the GPIB-config State
  buffer's bits `0x40`/`0x80` (toggling the Option Interrupt Mask
  Latch's `3D` output, `[0x6E2]+3`, and re-reading `[0x6DA]`) - a
  hardware option-board-type auto-detection, not a switch read. See
  `MEMORY_MAP.md`'s new "GPIB option board" section.
- **The 10-position PARAMETERS DIP switch is only partially mapped.**
  Only the baud-rate nibble (switches 1-4) has a confirmed bit-to-
  meaning map (see `hardware/manuals/2230_programming/
  PRACTICAL_GUIDE.md`) - `read_dip_switches_serial_config`/
  `read_dip_switches_gpib_config` decode the other switch positions
  too, but which physical switch controls which decoded bit beyond
  baud rate isn't confirmed. **Narrowed 2026-09-16** (user's schematic
  trace, see `MEMORY_MAP.md`'s "RS-232 option board" section): the
  physical *register layout* is now known - switches 1-7 land in the
  Parameter Buffer (`0x406BC`, `BD0`-`BD6`), switches 8-10 in the State
  Buffer (`0x4067C`, `BD3`-`BD5`) - but the still-open part is
  unchanged: which *specific* switch number (within each group)
  controls which *specific* decoded firmware setting (parity, stop
  bits, etc. beyond the already-confirmed baud-rate nibble). **Polarity
  resolved 2026-09-17** (see `MEMORY_MAP.md`'s update to this section):
  the switches are active-LOW (ON pulls the bit to `0`), confirmed
  against a live exerciser-screen photo - only the switch-to-setting
  mapping beyond baud rate remains open, not the bit-level polarity.
  **Fully resolved 2026-10-09**, by combining a fresh line-by-line
  disassembly of `read_dip_switches_serial_config`
  (`0x966E7`-`0x96780`) with `docs/options.md`'s OCR'd Tables 7-7, 7-8,
  7-9 (RS-232-C PARAMETERS switch), and the already-confirmed Table
  7-36 bit map for the State Buffer. See `MEMORY_MAP.md`'s "RS-232
  option board" section for the full switch-by-switch bit formulas;
  summary: switches 1-4 = baud nibble (unchanged); switch 5 = parity
  enable/disable (gates whether switches 6-7 are even consulted);
  switches 6-7 = parity type (ODD/MARK/EVEN/SPACE, in the firmware's
  own 1-4 internal order, which is *not* the same order as Table 7-9's
  ODD/EVEN/MARK/SPACE listing - switch 7, not switch 6, turned out to
  be the higher-weight bit of the firmware's internal code); switch 8
  = CR vs CR-LF line terminator; switches 9-10 = printer/plotter
  device select (HP-GL/ThinkJet/Epson), consistent with - and now
  fully explained by - the switch-9/10 bit swap Table 7-36 had already
  surfaced. No remaining open sub-question here.
- **`COMM/DATA/STOP_BITS`/`FLOW` (runtime menu) vs. the rear-panel DIP
  switch** - **mostly resolved 2026-10-09** (see `MEMORY_MAP.md`'s
  "RS-232 option board" section): `STOP BITS`/`FLOW` turned out to have
  no DIP switch counterpart at all, so there's no real conflict for
  those two. For the settings the switch genuinely decodes (baud/
  parity/terminator/printer-plotter), `read_dip_switches_serial_config`
  is the only writer found anywhere in either ROM, consistent with the
  manual's own wording (a software/MENU override is explicitly
  promised only for printer/plotter, not baud/parity/terminator) - but
  the actual printer/plotter override code was **not found**, still a
  genuine open thread. **Re-checked 2026-10-09**: confirmed the manual's
  `PLOt FORmat [XY], HPGl, EPS7, EPS8, TJEt` command really is a
  software SET (not read-only), and re-grepped every reference to
  `[0x461]` across both ROMs' full proven+heuristic listings - still
  only the two switch-decode writers, nothing else. The handler most
  likely lives in the ~8-9% of ROM bytes neither disassembly pass has
  reached yet, rather than somewhere already-disassembled but missed.
  **Further narrowed 2026-10-09**: traced the actual `PLOt` command's
  argument-validation code (`FUNC_2998_44D0`, reached via `[0x604]==
  0x17`) end-to-end and confirmed by full-body read that it never
  writes `[0x461]` on any path (only sets an error code and resets
  parser state) - ruling out this one remaining already-disassembled
  candidate. See `docs/comm-rom/command-parser-token-scan-and-plot-
  handler.md`.
  **Further checked 2026-10-09**: scanned the comm ROM's entire
  undisassembled byte range (8 gaps, 5424 bytes total, computed via a
  one-off extension of `disasm/compute_coverage.py`) for the literal
  little-endian displacement bytes `61 04` (i.e. any direct or
  base+disp16 instruction writing `[0x461]`) - **zero hits anywhere**.
  Caveat: this cannot rule out a write via a computed/indirect pointer
  that doesn't embed the offset as an immediate. Also checked where
  the `EPS7`/`EPS8`/`FORmat`/`HPGl`/`TJEt` strings themselves live
  (`disasm/strings_160-2998.json`): they're already part of the fully-
  documented table-1 argument/value keyword table in
  `docs/comm-rom/command-keyword-table.md` (`0x88a58`-range) - not a
  new lead, and not near any hidden code. However, immediately
  *before* that documented table (file offsets `0x08005`-`0x08A57`,
  physical `0x88005`-`0x88A57`, already flagged as unidentified in
  `UNKNOWN_DATA.md` blocks 4-7) sits a distinct, previously-
  uncharacterized binary structure: block 6 (`0x8824C`-`0x088728`).
  **Decoded (partially) same day**: its first 6 records are a genuine
  dispatch table whose far pointers resolve (via the confirmed
  `0x90000`-`0x97FFF` alias) to 6 real, already-disassembled functions
  (`FUNC_2998_C411`/`C424`/`C437`/`C44A`/`C45D`/`C470`), each calling a
  shared helper that reads `get_comm_config_flag` and returns a
  response-string pointer - most likely a query-response stringifier
  for boolean settings like `SMOoth`/`VECtors`/`GRAticule`/`AUTo`/
  `FLOw`, **not** related to `FORmat`/`[0x461]`'s write site. Records
  past #6 don't keep a fixed stride and aren't fully decoded. An
  apparent `[0x73A]`-address conflict surfaced while tracing this
  (between `get_comm_config_flag`/`set_comm_config_flag` and
  `init_comm_dispatch_table`) was found and resolved the same day: the
  comm ROM standardly runs under `DS=0x8f80`, so its `[0x73A]` is
  physical `0x8FF3A`, not the sysrom's `DS=0` `[0x73A]` - cross-
  subsystem address reuse via a `DS` swap, not a real conflict. Full
  writeup in `docs/comm-rom/command-keyword-table.md`'s new "## 4."
  section.
  **Correction, 2026-10-11**: the shared helper above (renamed
  `flag_value_to_keyword_ptr`, `0x944D5`) is not a generic boolean
  `OFf`/`ON` stringifier - it reads one of exactly 4 fixed offsets off
  a single base far pointer (`[0x6F2]`/`[0x6F4]`, itself confirmed this
  session to be the base pointer to table 1) based on a config flag's
  raw value, landing on `ACCpeak`/`AVErage`/`PEAkdet`/`SAMple` -
  textbook Tek acquisition-mode keywords, not `SMOoth`/`VECtors`/
  `GRAticule`/`AUTo`/`FLOw`-style booleans. Which of the 6 callers'
  flag-index constants (`0x3a`/`0x41`/`0x48`/`0x50`/`0x58`, plus one
  computed) maps to which real oscilloscope setting is still open. See
  `docs/comm-rom/command-keyword-table.md` section 4's 2026-10-11
  follow-up, `VARIABLES.md`'s `[0x6F2]`/`[0x6F4]` entry, and
  `FUNCTIONS.md`'s `flag_value_to_keyword_ptr` entry.
  **Follow-up, 2026-10-11**: found the SET-side counterpart of this
  same 5-flag-index family (8 functions at `0x8C1FA`-`0x8C356`), and in
  the process decoded "block 7" (the region the above section's far
  pointer A pointed into, previously undecoded) as a large shared
  function-reference table - at least 35 distinct real function
  targets confirmed so far (up from an initial 18), all with
  `ref_count: 0`/`functional_name: null`, i.e. this table appears to be
  the actual reference mechanism for a meaningful chunk (~11%) of the
  comm ROM's 310 total orphan functions. One of the 35
  (`SUB_945CF`/`0x8C5CF`) ties the flag family to the real `LONg`
  command (`[0x732]+0x1F == 0x14`) - narrowing, but not yet resolving,
  which real setting each flag index represents. Full writeup in
  `docs/comm-rom/command-keyword-table.md`'s new "## 5." section;
  `VARIABLES.md`'s `[0x732]`/`[0x734]` and new `[0x5fe]` entries. Still
  open: the table's true start/end boundaries, the `id`/`marker` byte
  semantics, what code actually reads a record's `id`/`marker` to
  select it at runtime (not yet found), and which of the 5 flag indices
  maps to which real setting.
  **Follow-up, 2026-10-11**: decoded `init_far_pointer_table`
  (`0x80133`, 31 RAM-pointer entries) while still hunting block 7's
  reader. Found `[0x6FA]` (block 7's own base pointer, confirmed to
  point exactly at `0x887C4`) but no static reader for it anywhere -
  genuinely still open. Also found, as a side effect, that this same
  initializer resolves the long-standing `[0x6EA]`/`[0x6F6]` "zero
  comm-ROM writes" mystery from the `[0x732+0x1F]` writer-search
  section above - see that section's own 2026-10-11 resolution note.
  **Further follow-up, 2026-10-11 (same day, later pass)**: finished
  decoding all 31 of `init_far_pointer_table`'s entries. New lead on
  the flag-index question: `[0x73A]` (the `get_comm_config_flag`/
  `set_comm_config_flag` base) is initialized at boot to far pointer
  `0041:03e3` - under the flat-`DS=0x41` convention, that's the
  already-documented `[0x3E3]` per-item alt-value step/position byte
  array (indexed by front-panel item index). So "comm config flags"
  are literally the main item table's per-item step bytes, and the 5
  flag-index constants (`0x3a`/`0x41`/`0x48`/`0x50`/`0x58`) are most
  likely item indices 58/65/72/80/88 into that table - not yet looked
  up against a name, but a much more concrete lead than before. Also
  resolved in the same pass: `[0x732]`'s own boot value (`0041:1b64`,
  i.e. `[0x1B64]`) proves the identity with `[0x1B83]` described in the
  `[0x732+0x1F]` write-site section above; `[0x736]` similarly forwards
  to `[0x548]` (display-dirty flag); `[0x72A]`'s own write site and
  24-entry jump-table contents were found (see `VARIABLES.md`'s
  `[0x5A4]` row); and table 3's base pointer (`[0x6E6]`), the per-
  command minimum-abbreviation-length array (`[0x6EE]`), block 6's two
  pointers (`[0x706]`/`[0x6FE]`), and a new `ID?`-response candidate
  string (`[0x70A]`, `"TEK/2230 SYS:0.0,COMM:0.0"`) were all resolved.
  Full writeup in `docs/comm-rom/command-keyword-table.md` section 7;
  `VARIABLES.md`'s comm-ROM section has the per-address rows.
  **Resolved, 2026-10-11 (same day, later pass)**: the flag-index
  question itself. `docs/programming-manual.md` table 7-27 documents
  exactly 6 `ACQuisition` sub-arguments (`CURRent`/`HSRec`/`LSRec`/
  `REPetitive`/`ROLl`/`SCAn`) drawn from the same 4-keyword set
  `flag_value_to_keyword_ptr` dispatches onto, a 1:1 count match with
  the 5 fixed flag indices plus the one computed-index path. Each flag
  index's SET-side value-domain (2/3/3/4/4 legal values for `0x58`/
  `0x3a`/`0x41`/`0x48`/`0x50`) exactly fingerprints each sub-argument's
  manual-documented legal-keyword restriction, and block 6's own
  GET-side push order (`0x3a`,`0x41`,`0x48`,`0x50`,`0x58`, confirmed by
  direct read) matches the manual's own `ACQuisition?` example response
  field order (`REP`,`HSR`,`LSR`,`SCA`,`ROL`) position-for-position:
  **`0x3a`=`REPetitive`, `0x41`=`HSRec`, `0x48`=`LSRec`, `0x50`=`SCAn`,
  `0x58`=`ROLl`**, computed-index=`CURRent`. The `0x3a`/`0x41` pairing
  and `0x48`/`0x50` pairing each rest on the response-field-order
  argument alone (their handler bodies are structurally identical
  within each pair), so treat those two orderings as well-supported but
  not independently proven the way `0x58`=`ROLl` is. Full writeup in
  `docs/comm-rom/command-keyword-table.md` section 4's 2026-10-11 later
  pass. Still open: which specific front-panel item numbers 58/65/72/
  80/88 name in the main item table (orthogonal to the comm-command
  identity now resolved above); the dense uncharacterized cluster
  (`[0x722]`/`[0x716]`/`[0x70E]`/`[0x712]`/`[0x71E]`/`[0x71A]`/
  `[0x726]`/`[0x72E]`, physical `0x88006`-`0x880F4`) still isn't decoded
  past initial hypotheses.
  **Follow-up, 2026-10-12**: found `[0x5fe]`'s write site -
  `FUNC_2998_43F0` (`0x8444C`), fed by `FUNC_2998_5115`/
  `FUNC_2998_519A`'s letter-bucket walk plus `FUNC_2998_3048`'s
  per-candidate scan increment (the same mechanism already confirmed
  for command-ID resolution). Tested whether `[0x5fe]`'s raw
  comparison literals (`1`/`7`/`0x43`/`0x35`) equal table 1's global
  alphabetical index of the matched keyword: exact match for
  `ACCpeak`(1)/`AVErage`(7), but off by `+4`/`+2` for `SAMple`(real 63,
  literal 67)/`PEAkdet`(real 51, literal 53) - independently
  re-verified by a manual byte-level table walk, so not a counting
  bug. **Still open**: the exact numeric encoding `[0x5fe]` actually
  uses remains unresolved; full writeup (including a side finding
  about table 1's 3-byte metadata-gap entries encoding the next
  entry's first letter) in `docs/comm-rom/command-keyword-table.md`
  section 8.
  **Follow-up, 2026-10-12**: checked whether the main item table
  (`[0x1D10]`) itself names item indices 58/65/72/80/88, separately
  from their already-resolved comm-command identity. Two avenues ruled
  out: the table's own record fields have no string/label field at
  all (see `VARIABLES.md`'s `[0x1D10]` row), and grepping every
  `[0x464]` (current item index) write in the main ROM found none that
  assign a literal matching these 5 values - all go through a register
  loaded from an as-yet-unidentified switch-ID-to-index lookup. Would
  need that lookup table or a live RAM dump (emulator) to go further;
  not attempted. See `VARIABLES.md`'s `[0x73A]` row for the full
  writeup. Low priority given the real-world identity is already
  solid via the comm-command cross-reference.
  **Follow-up, 2026-10-12**: the separately-listed "dense uncharacterized
  cluster" (`[0x722]`/`[0x716]`/`[0x70E]`/`[0x712]`/`[0x71E]`/`[0x71A]`/
  `[0x726]`/`[0x72E]`) is resolved for 6 of its 8 offsets - traced each
  one's actual runtime *reader* (not just its boot-init pointer-table
  entry) and found 5 unrelated structures: `[0x716]`/`[0x71A]` are a
  baud-rate/parity table pair feeding the RS-232 UART register bank
  (indexed by the already-known DIP-switch decode `[0x4EC]`/`[0x4ED]`);
  `[0x71E]` is a command-ID membership list gated by a device-variant
  byte; `[0x722]` is a literal `"#H"` ASCII string printed as a numeric
  prefix; `[0x726]` is a GET-response field-index list reusing
  `ACQuisition`'s block-6 mechanism for a different, still-unidentified
  command; `[0x72E]` is a 10-byte-stride range/limit-check-shaped record
  array. `[0x70E]`/`[0x712]` remain as previously characterized (baud-rate
  text tokens / 2-letter mnemonic strings, both still unconfirmed). Full
  writeup in `docs/comm-rom/command-keyword-table.md` section 9;
  `VARIABLES.md` now has rows for all 6 newly-resolved offsets.
  **Second follow-up, 2026-10-12**: `[0x702]` (section 7's "comm ROM's
  own lower half, not yet characterized") is now resolved too - it's
  the master command-ID -> block-6-record-slice descriptor table,
  indexed by `[0x604]*2` (the resolved command ID), giving each
  command's record count and starting-record offset into the shared
  12-byte block-6 array based at `[0x6FE]`. Full writeup in
  `command-keyword-table.md` section 10; `VARIABLES.md` has the new
  row.
  **Third follow-up, 2026-10-13**: the large structure surfaced while
  tracing `[0x702]` (`[0x73E]`/`[0x746]`/`[0x742]`/`[0x74A]`, indexed by
  `[0x5F8]`) is now substantially resolved: it's the **lettered
  reference-memory (`REFA`-`REFZ`) record array**. `[0x73E]` (12-byte
  stride, 26 records) holds a write-protection flag byte at `+0xA`
  (`"UNLock"`/`"PERM"`/`"LOCk"`, via keyword-table-1); `[0x742]`
  (78-byte stride, same 26-record indexing) holds the actual saved
  waveform data pointers/lengths; `[0x746]`/`[0x74A]` are a small
  pool-header struct managing `[0x742]`'s waveform-data heap.
  `[0x5F8]` selects `1..4` = numbered refs `REF1`-`REF4` or `5..30` =
  lettered refs `REFA`-`REFZ`, resolved into the array index via
  `SUB_93B1C`/`SUB_93B7E`/`SUB_96126` - confirmed independently by
  `FUNC_2998_C85B`, a sibling entry point that derives the same index
  directly from a raw letter (`letter - 'a'`). Strong candidate
  real-world identity: `REFProt`/`REFStat? PROTect` (keyword states and
  the 4-numbered + 26-lettered = 30-total count both match the manual
  exactly). Full writeup in `command-keyword-table.md` section 11;
  `VARIABLES.md` has rows for all 6 variables.
  **Fourth follow-up, 2026-10-13, later pass**: found the `REFProt`
  SET-side siblings - `FUNC_2998_B252` (clears `+0xA` bits `0x04`+`0x40`,
  candidate `LOCked`) and `FUNC_2998_B2BC` (clears `0x40`, sets `0x04`,
  candidate `UNLocked`), both rejecting `1..4`; no SET-side function
  setting bit `0x40` alone (`PERM`) was found. `FUNC_2998_B339`
  (previously tagged "unlock") was **re-identified away from `REFProt`**
  toward `REFDisp REF{A-Z}:EMPTY`, a better structural fit (same
  lettered-only validation; "erases the named reference if it is not
  protected" per the manual). Also found a genuine, unresolved
  **inconsistency**: the GET-side `FUNC_2998_B3A3` (candidate `REFProt?`)
  and `REFStat? PROTect`'s own string builder `FUNC_2998_B568` (via
  `FUNC_2998_D067`, the real target of `SUB_95067`) map the same raw
  `+0xA` byte **oppositely** for the clear-byte and bit-`0x04`-only
  cases (`B3A3`: clear->`LOCk`, `0x04`->`UNLock`; `B568`: clear->`'U'`,
  `0x04`->`'L'`); both agree only on bit-`0x40`->PERM. Left open rather
  than guessed. Found `FUNC_2998_B600` (KB-free readout over
  `[0x746]+6`).
  **Fifth follow-up, 2026-10-13, Ghidra decompiled-C re-trace**:
  `FUNC_2998_B622` (`decompile/exports/160-2998-14.c` line 9471) turned
  out to be far more than a simple cross-check - it's the actual
  validating SET-side/copy-executor for `DATa SOUrce`/`TARget`.
  Resolves **both** `[0x4F6]` (source) and `[0x5F8]` (target) through
  the same `SUB_93B1C` call confirmed for `REFProt`, proving `[0x4F6]`
  holds the full `0`(ACQ)/`1..4`(numbered)/`5..30`(lettered) selector
  (not just a 2-state boolean, which is merely what `FUNC_2998_B227`'s
  narrower report path collapses it to). Validates `[0x5F8]` against
  the device-variant byte `[0x1B83]` and `[0x5F7]` (at the time
  characterized as a **lettered-reference feature-gate** - corrected
  by the thirty-second follow-up below, it's really an ephemeral
  per-command argument-token classification byte: `4` + variant `0x1E`
  enables the full `1..30` range; otherwise, in range or not,
  force-defaults to `[0x5F8]=4`/`[0x4F6]=0` - an exact match for the
  manual's "REF4 is the only available reference memory for the 2220
  and 2221"). Rejects source/target pairing `{REF1,REF4}` in either
  order. Performs a real copy: `ACQ`->numbered-ref via
  `draw_pending_line_segment`/`SUB_93925`/`reset_plot_home_or_acq`;
  ref->ref via `SUB_9471C`/`SUB_94D9C` (lettered source) or a
  capability-bit check (numbered source) then `FUNC_2998_7609`, gated
  by a `1`/`4` category match when either endpoint is `REF1`/`REF4`.
  This reinforces the `DATa SOUrce`/`TARget` candidate identity for
  `[0x4F6]`/`[0x5F8]` from weak corroboration to strong (though still
  not 100%-proven by a direct string/ID trace).
  Remaining open within this subsystem: `[0x742]`'s individual fields
  beyond the far-pointer/length pairs, `[0x746]`/`[0x74A]`'s exact pool
  semantics, the `B3A3`-vs-`B568` bit-mapping inconsistency, `PERM`'s
  SET-side (not found), `[0x5F7]`'s precise real-world meaning (beyond
  "lettered-reference feature-gate"), the `{REF1,REF4}`-pairing
  rejection and `1`/`4` category-match's rationale, and the newly
  surfaced, not-yet-traced callees `SUB_93925`/`SUB_9471C`/
  `SUB_94D9C`/`FUNC_2998_7609`.
  **Sixth follow-up, 2026-10-13, ground-truth re-trace of the resolver
  chain**: direct reads of `SUB_93B1C`, `SUB_93B7E`, and `SUB_96126`
  (rather than inferring their combined behavior from input/output)
  found the earlier "arithmetic consistent with `[0x5F8]-5`" framing
  was an imprecise blend, and that `SUB_96126` isn't part of the index
  resolution at all: `SUB_93B1C` (physical `0x8BB1C`) is a switch that
  converts the numeric selector to an ASCII **letter** (`0..3`
  pass through; `4`/`REF4` aliases to `1`/`REF1` unconditionally,
  independent of device variant - **explained, twenty-eighth follow-up
  below, via the programming manual's "REF4... occupies the REF1-REF3
  memory locations in the 2230" note**; `5..30` -> `(input+0x5C)&0xFF`
  = `'a'..'z'`); `SUB_93B7E`
  (physical `0x8BB7E`) is the separate step converting that letter to
  the `0..25` record index (`byte-0x61`, else sentinel `0xFFFF`);
  `SUB_96126` (physical `0x8E126`, = `FUNC_2998_E126`) takes no
  arguments and is a test-and-set **interlock/busy flag** over the
  structure `[0x746]` points to (checks/sets that structure's `+0`/
  `+2`/`+4` words with interrupts disabled, writing a new status-code
  variable `[0x690]` to `0x6D9`/`0x6E4`) - every observed caller
  (`FUNC_2998_B252`/`B2BC`/`B339`) gates its `[0x73E]` record update on
  this interlock, and releases it (resets `+0` to `0`) itself on
  success. Also confirmed `[0x746]` is a plain far-pointer *variable*
  (not itself the struct) to a 14-byte pool-header structure
  (`FUNC_2998_E1AF`, physical `0x8E1AF`, `memcpy_far`s all 14 bytes out
  to a caller buffer), and that `FUNC_2998_B252`/`B2BC`/`B339` share a
  previously-undocumented `[0x603]` bit-`0x01` dirty/redraw flag
  (distinct from `FUNC_2998_B622`'s `[0x736]` bit `0x200`). Full
  writeup in `command-keyword-table.md` section 11; `VARIABLES.md`'s
  `[0x5F8]`/`[0x746]` rows updated and a new `[0x690]` row added.
  `SUB_93925`/`SUB_9471C`/`SUB_94D9C`/`FUNC_2998_7609`/`[0x74A]`'s exact
  role/`[0x690]`'s real-world text remain open.
  **Seventh follow-up, 2026-10-13**: found `FUNC_2998_E1E9` (physical
  `0x8E1E9`, same no-static-caller shape as its neighbors) - given a
  `0..25` record index, it zeroes `[0x73E]`'s record words `+0`/`+2`/
  `+4`/`+6`/`+8`, sets `+0xA` to the literal `1` (a fourth value
  distinct from the `0x04`/`0x40`/`0x44` protection masks), and
  `memset_far`s the entire parallel 78-byte `[0x742]` record to zero -
  a full per-letter record wipe. This is a **stronger-fitting
  candidate for `REFDisp REF{A-Z}:EMPTY`** than `FUNC_2998_B339`
  (which only ever sets protection bits, never clears `[0x742]`),
  reopening `B339`'s own identity rather than settling it. Also pins
  `[0x73E]`'s record as having at least 5 more word fields beyond
  `+0xA`, not yet characterized in their normal (non-wiped) use.
  **Eighth follow-up, 2026-10-13**: traced `SUB_93925` (physical
  `0x8B925`, one of `FUNC_2998_B622`'s not-yet-traced callees) - it's
  a post-validation display-mode/acquisition-state update dispatcher,
  branching on the `(source,target)` `1`-or-`4` category pair and
  calling combinations of `update_display_mode_flags`,
  `print_and_reset_acq_buffers`, `disable_item_pair`, and a
  `restart_current_task` poll loop, always `or`-setting `[0x736]` bit
  `0x200` on success (confirming that's the shared redraw-pending
  flag). Each branch's real-world meaning (which UI state transition
  it implements) is not pinned down. `SUB_9471C`/`SUB_94D9C`/
  `FUNC_2998_7609`'s `func_0x00086b4b` branch remain untraced.
  **Ninth follow-up, 2026-10-13**: traced `SUB_9471C`/`SUB_94D9C`
  (physical `0x8C71C`/`0x8CD9C`) - a matched acquire/release pair for
  a lettered reference's waveform data. `SUB_9471C` checks `[0x73E]`
  record `+0xA` bit `0x02` ("has waveform data" capability) then bit
  `0x10`/`[0x746]+4`/`+0` ("busy") before granting access: on success
  it sets bit `0x08`, increments the record's own `+8` field, and
  increments `[0x746]+2`, returning a far pointer to the letter's
  `[0x742]` record. `SUB_94D9C` reverses this (decrements `+8`/`+2`,
  clearing bit `0x08` only once `+8` hits `0`) via one branch, or
  clears bit `0x10`/decrements `[0x746]+4` via a second branch for a
  handle type `SUB_9471C` itself never sets. This resolves `[0x746]+2`/
  `+4` as two independent pool-wide outstanding-handle counters (not a
  dirty-flag pair as previously guessed) and explains `SUB_96126`'s
  "`+0`/`+2`/`+4` all zero" idle check as "no reference checked out for
  reading, none exclusively locked, and no other caller already holds
  the interlock." Open: no setter for bit `0x10`/`[0x746]+4` (the
  presumed "exclusive lock acquire" counterpart to `SUB_9471C`) has
  been found in this cluster.
  **Tenth follow-up, 2026-10-13**: traced `FUNC_2998_7609` (physical
  `0x87609`) and its `func_0x00086b4b`/`FUNC_2998_6B4B` branch (both
  now confirmed). `FUNC_2998_7609` is the actual reference-to-
  reference copy executor: dispatches numbered sources (`REF1`-`REF3`,
  and transitively `REF4` via its `SUB_93B1C` alias) to
  `FUNC_2998_6B4B` to translate into a `[0x742]`-record-shaped
  descriptor, or lettered sources directly to `SUB_9471C`; both paths
  converge on a shared format check, `disable_item_pair`/
  `print_and_reset_acq_buffers`/poll-loop sequence, and a payload
  `memcpy_far` whose length is recomputed by scanning the source bytes'
  low 2 bits for values `1`/`3` - a previously-undocumented structural
  fact that waveform-point bytes carry a 2-bit status tag, meaning not
  yet confirmed. Also resolved (correcting an earlier, imprecise
  description) that `FUNC_2998_B622` computes **two distinct** values
  around this call, not one shared category: a source/target format
  class (`0..3`, lettered/`ACQ` collapsing to `0`) and a separate
  "either endpoint is `REF1`-or-`REF4`" priority value, the latter
  gating a mandatory format-class match only when `REF1`/`REF4` is
  actually involved - narrowing the long-open `{REF1,REF4}`-pairing
  rejection rationale to "fixed, format-specific storage slots," though
  which of `REF1`/`REF4` is the "short" vs. "long" format class is
  still open. See `docs/comm-rom/command-keyword-table.md` section 11.
  **Eleventh follow-up, 2026-10-13**: traced `FUNC_2998_6B4B` to its
  `retf 2` epilogue (confirming the same word-arg-in/far-ptr-out/
  callee-cleans-2-bytes convention as `SUB_9471C`/`SUB_94D9C`) and
  worked out the exact point-tag-scan mechanics left open above. The
  scan's byte-vs-word stride is selected by bits `0x30` of the same
  `[0x18C]`+class*16 table byte `FUNC_2998_B622` already reads bit
  `0x01` from; within either stride it's a sequential "skip until
  marker" walk that stores the distance to the first byte tagged `3`
  into `+0x32`, then the distance from there to the first byte tagged
  `1` into `+0x34` - distances-to-first-occurrence, not independent
  tallies. This is now a solid mechanical answer; the real-world
  meaning of what `3`/`1` specifically denote (and why that order)
  remains unconfirmed. See `docs/comm-rom/command-keyword-table.md`
  section 11.
  **Twelfth follow-up, 2026-10-13**: identified `FUNC_2998_6F6F`
  (physical `0x86F6F`), adjacent to `FUNC_2998_6B4B` and its exact
  inverse - a scatter/"install" counterpart to `6B4B`'s gather/
  "translate." Its only caller is `FUNC_2998_7609` itself, which uses
  it to write a locally-assembled destination record's fields back out
  to the same scattered per-reference tables `6B4B` reads from, when a
  reference-copy's destination is a numbered reference. Confirms the
  same `retf 2` stack shape as `6B4B` but the opposite data-flow
  direction through the far-pointer argument. Also surfaces a new,
  unexplained detail: each numbered reference occupies **two adjacent
  table-row slots** (`class*2`/`class*2+1`) in several of these tables,
  not one row per reference. See `docs/comm-rom/command-keyword-table.md`
  section 11.
  **Fifteenth follow-up, 2026-10-13**: resolved the two-adjacent-slots
  question above - not a duplicated copy. `FUNC_2998_6F6F`'s own field
  reads show record offset `+0x26`'s capability bit going to slot
  `class*2` while the very next byte, `+0x27`, goes to slot
  `class*2+1`; likewise record bytes `+0x42`-`+0x46` (5 bytes) fill
  slot `class*2`'s `b0`-`b4` row while the next 5 bytes, `+0x47`-
  `+0x4b`, fill slot `class*2+1`'s row. Each reference's record bundles
  two disjoint field groups, not one group duplicated - plausibly the
  two markers `FUNC_2998_6B4B`'s point-tag scan produces (`+0x32`/
  `+0x34`, the tag-`3` and tag-`1` distances), though that specific
  mapping isn't proven. See `docs/comm-rom/command-keyword-table.md`
  section 11.
  **Sixteenth follow-up, 2026-10-13**: identified `FUNC_2998_B051` as
  **`REFDisp?`**, confirmed by a direct, byte-exact string match (not
  a guess) - its three possible keyword-table-1 responses, read
  straight from the ROM binary at the offsets it computes, are
  `"EMPty"`, `"OFF"`, `"ON"`, exactly the manual's documented `REFDisp?`
  response set. Also confirms the documented 2230-vs-2220/2221
  default-argument behavior byte-for-byte (`REF1` vs `REF4`). For
  numbered references it reads a 2-bit status from `es:[0x732+0x7E]`
  at both `class*2` and `class*2+1` and ORs them - updates the
  two-adjacent-slots hypothesis above: ORing two slots to decide
  on-screen display status fits "the two stored channels, `REFOrmat
  CHAnnel:{CH1,CH2}`" better than the two point-tag markers originally
  guessed, though neither is proven. See `docs/comm-rom/command-
  keyword-table.md` section 11.
  **Seventeenth follow-up, 2026-10-13**: found the numbered-`REFDisp`
  SET-side's two best-fitting candidates, `FUNC_2998_AD17` and
  `FUNC_2998_AF8A` (neither has a disassembler-resolved caller, so
  behavioral fit stands in for a confirmed call site) - and in doing so
  explains why the whole-ROM write-site search (12th follow-up below)
  found nothing: neither function writes `es:[0x732+0x7E]`/`[0x1BE2]`
  directly. Both far-call into shared **main-ROM** menu-item code
  (`set_item_pair_active`/`disable_item_pair`, already confirmed in
  `FUNCTIONS.md`) that performs the actual bit toggle inside
  `set_item_active_flag`, on the same `[item+0x1BE2]` array independently
  proven (15th follow-up) to be `[0x732]+0x7E`. `FUNC_2998_AF8A`
  (clears both status bits via `disable_item_pair`) matches `EMPty`
  better than `OFF`; `FUNC_2998_AD17`'s `[bp+6]!=0` branch (sets only
  the "selected" bit via `set_item_pair_active`) fits `ON`. The `[bp+6]
  ==0` branch (calls only `print_and_reset_acq_buffers`, no flag-bit
  touch) is not yet matched to `OFF` or anything else. **Checked
  further, 2026-10-13**: confirmed by direct read that
  `print_and_reset_acq_buffers` really doesn't touch `[0x1BE2]`
  anywhere in its own body (ruling out the "maybe it clears the bit
  internally" guess), and that the class-`2`/`3` branch exhibits the
  identical `[bp+6]==0` -> "`print_and_reset_acq_buffers` only, no
  flag touch" behavior as the class-`1`/`4` branch - so this is
  consistent across both independently-gated paths, not a one-off
  oversight, which only deepens the puzzle rather than resolving it.
  Still open. See `docs/comm-rom/command-keyword-table.md` section 11.
  **Twenty-eighth follow-up, 2026-10-13, later pass**: traced the
  class-`1`/`4` gate inside `FUNC_2998_AD17`'s shared `L_8AD5B` path in
  full. It calls `FUNC_2998_6B4B` with its class argument **hardcoded
  to `1`** (never the real selector) and then checks bit `0x01` of the
  resulting record's offset-`+0` byte with **opposite required
  polarity** per class: class `1` needs it clear, class `4` needs it
  set - a real, mechanically confirmed asymmetry (both branches
  converge on the same downstream `[bp+6]` check), not a copy-paste
  artifact. Plausible tie-in to the manual's note that 2230's REF4
  "occupies the REF1-REF3 memory locations" (i.e. isn't a distinct
  physical store on this model) - but `FUNC_2998_6B4B`'s offset-`+0`
  byte has no established meaning yet, so the specific bit-sense story
  is a lead, not a resolution. **Thirty-first follow-up, 2026-10-13,
  later pass - the broader premise is now corroborated**: `SUB_93B1C`
  (resolver-chain writeup above) maps selector `4` to the same output
  as `1` **unconditionally, regardless of the `[0x1B83]` device-variant
  byte** - i.e. not a 2220/2221-only compatibility shim, but a
  universal rule on this firmware. That matches the manual's "REF4...
  occupies the REF1-REF3 memory locations in the 2230" read as a
  general 2230 hardware fact rather than a 2220/2221-specific
  restriction, and gives two independent code sites (this gate and
  `SUB_93B1C`'s selector normalization) converging on the same
  explanation - resolving the long-open "REF4->1 alias root cause"
  question. Does **not** resolve the `[bp+6]==0`-isn't-`:OFF` puzzle
  from the seventeenth follow-up, which remains open. See `docs/
  comm-rom/command-keyword-table.md` section 11.
  **Eighteenth follow-up, 2026-10-13**: resolved the `B3A3`-vs-`B568`
  bit-mapping "inconsistency" from the fourth follow-up below - it was
  largely a transcription error in how this doc described `B3A3`'s own
  branch order (the *clear* case was tested first, not the *set* case
  as originally written). Corrected, `B3A3` and `B568`/`D067` agree on
  3 of 4 raw-byte states (all-clear->`UNLock`/`U`; bit `0x04` alone->
  `LOCk`/`L`; both bits->`PERM`/`P`) and differ only on the fourth
  (bit `0x40` alone, `0x04` clear): `B3A3` would call it `UNLock`,
  `B568` would call it `PERM`. None of the three known `+0xA`-byte
  SET-side functions (`B252`/`B2BC`/`B339`) ever produce that raw byte
  (all three touch bits `0x04` and `0x40` together), and a whole-ROM
  grep for an immediate `or ..., 0x40` found zero matches - ruling out
  the simplest form `PERM`'s still-missing SET-side could take and
  reinforcing the existing guess that `PERM` isn't a simple
  immediate-OR flag toggle in this ROM. See `docs/comm-rom/command-
  keyword-table.md` section 11.
  **Nineteenth follow-up, 2026-10-13**: looked up `[0x690]`'s
  "real-world text" per the sixth follow-up's open item, and found the
  premise wrong - it isn't a 2-value status/message pair at all.
  Grepping every `[0x690]` access in both ROM listings found **41
  distinct write sites, each storing its own unique literal word
  value** (`0xF4` through `0x6E4`, strictly increasing with address),
  spread across only **six** functions (`FUNC_2998_C71C`,
  `FUNC_2998_C85B`, `FUNC_2998_D1AA`, `FUNC_2998_D476`,
  `FUNC_2998_DB69`, and `SUB_96126`/`FUNC_2998_E126` itself) - i.e. each
  of these functions sets a different fine-grained reason code at
  nearly every one of its internal exit paths. **No read site for
  `[0x690]` exists anywhere in either ROM's disassembly** (confirmed by
  grep for every non-immediate-write access pattern), and no write site
  loads the `0xFF7B`-segment string-table selector first - so the
  "real-world text" question has a clean answer: there isn't any
  firmware-internal text lookup for these codes at all. `FUNC_2998_C85B`
  (already identified as a per-lettered-reference query-building entry
  point, section 11 above) is exactly the kind of function the
  already-documented "`STATUS <code>;` inline substitution for an
  unavailable query field" behavior (`rs232-live-session-2026-09-14.md`)
  would need such codes for, making that the leading hypothesis for
  what consumes `[0x690]` - most plausibly read by the *caller* of these
  functions (not found yet) rather than by any code within them.
  Corrected `VARIABLES.md`'s `[0x690]` row (previously said it was
  written only by `SUB_96126` with exactly two values - both wrong).
  See `docs/comm-rom/command-keyword-table.md` section 11.
  **Twentieth follow-up, 2026-10-13**: re-examined why `FUNC_2998_B339`
  had been moved off `REFProt`'s `PERM` SET-side (second follow-up
  below) onto `REFDisp REF{A-Z}:EMPTY` instead, and found the original
  reasoning doesn't hold up. The stated reason was that `B339` would be
  "redundant with `B2BC`" if left as a protection-SET function - but
  `B2BC` sets bit `0x04` *alone* (`LOCk`), while `B339` sets *both*
  `0x04` and `0x40` unconditionally; that's never redundant, and both
  bits set is exactly `PERM` under every version of the `B3A3`/`B568`
  mapping, corrected or not (the one state they'd always agreed on).
  Direct reread of `B339`'s body (physical `0x8B339`-`0x8B3A2`)
  confirms it does nothing but the bit-OR, the same `5..30` range
  check, and the same `SUB_93B1C`->`SUB_93B7E`->`SUB_96126` resolver/
  interlock chain as `B252`/`B2BC` - structurally identical to them,
  not to an "erase" operation. Since `FUNC_2998_E1E9` (seventh
  follow-up above) is a far better `EMPTY` fit anyway (a real record
  wipe), `B339` no longer needs the `EMPTY` role it was reassigned to
  cover. **`PERM`'s "still not found" SET-side is resolved**: `B252`
  (clears both bits) = `UNLocked`, `B2BC` (`0x04` alone) = `LOCk`,
  `B339` (both bits set) = `PERM` - a clean three-function family that
  was sitting in this cluster the whole time. See `docs/comm-rom/
  command-keyword-table.md` section 11.
  **Twenty-first follow-up, 2026-10-13**: re-read `FUNC_2998_C85B`'s
  full body (one of the nineteenth follow-up's six `[0x690]`-writing
  orphan functions) and found the "query-building entry point" framing
  used above is wrong - it's `SAVeref REF{A-Z}`'s SET-side **store**
  handler, not a GET-side query-response builder. It acquires the
  `SUB_96126` pool interlock, checks the destination letter's `0x04`
  protection bit first (refusing `SAVeref` into a locked/permanent
  reference, error `0x165`), compares existing-vs-source record
  lengths, `memcpy_far`s the source's waveform data into the
  destination `[0x742]` record on a length match, and sets the
  destination's `0x10` "exclusively-locked" bit and increments
  `[0x746]+4` - the exact bit/counter pair `SUB_9471C`/`SUB_94D9C`
  (eleventh follow-up... see `docs/comm-rom/command-keyword-table.md`
  section 11) already resolved, here acquired inline and left for a
  later release rather than released within this function. This also
  means the "`C85B` is a query-response builder" premise behind the
  `STATUS <code>;` hypothesis for `[0x690]` needs revising (the
  hypothesis itself isn't ruled out, but its caller - still not
  found - would have to be reachable from a command-processing path,
  not a live `?`-query path). The exhaustive search for that caller
  continued this follow-up: checked every record of section 5's "block
  7" shared function-reference table (walked in full, using the
  corrected `native = segment*16+offset-0x8000` alias resolution,
  catching a `-0x80000` off-by-one-hex-digit bug in the first attempt)
  and section 4's "block 6" flag-dispatch table (scanned for every
  occurrence of far-pointer-B's fixed segment `0x941f`) for a reference
  to any of the six functions' physical addresses - none found in
  either. The caller remains genuinely unresolved, not just unsearched.
  See `docs/comm-rom/command-keyword-table.md` section 11.
  **Twenty-second follow-up, 2026-10-13, later pass**: closed four of
  the nineteenth follow-up's six `[0x690]`-writing orphan functions by
  the same `0x9000`-segment call-target alias rule used throughout this
  cluster - each is byte-for-byte identical to an already-named
  function, just missing a `FUNCTIONAL_NAMES` entry at its own native
  comm-ROM address: `FUNC_2998_D1AA` = `validate_gpib_record_
  consistency` (native `0x8D1AA`), `FUNC_2998_D476` = `scan_and_repair_
  gpib_records` (native `0x8D476`), `FUNC_2998_D0C2` (immediately
  preceding `D1AA` in the listing) = `build_gpib_message_checksum`
  (native `0x8D0C2`), and `FUNC_2998_E1E9` = `init_gpib_record` (native
  `0x8E1E9`, a simple confirmation - already traced in a prior session
  as the full-record-wipe function). Reading all four native bodies in
  full also completed the `[0x746]` pool-header struct's last two
  fields (`+0xA` cumulative scan-diagnostic word, `+0xC` cumulative
  repair counter) and two new `[0x73E]+0xA` flag bits (`0x20`
  "checksums stale," `0x80` "state inconsistency"), plus two new
  checksum-byte fields (`[0x73E]+0xB` payload checksum, `[0x742]+0x4C`
  header checksum) computed by `build_gpib_message_checksum` and
  verified by `validate_gpib_record_consistency`. Added the four
  `FUNCTIONAL_NAMES` entries and regenerated the comm ROM's `.lst`/
  `.asm`/aligned-binary outputs - all four now appear under their real
  names. **This reframes the still-unresolved `[0x690]`-caller question
  from the twenty-first follow-up**: four of the six writers (`D1AA`/
  `D476`/`D0C2`-via-`C85B`'s-own-acquire/`E126`) are now understood as
  internal integrity-check/repair/interlock plumbing, not command
  handlers - making a boot-time or self-test integrity scrub, or purely
  internal calls from `SUB_9471C`/`SUB_94D9C`'s own acquire/release
  logic, a more plausible home for the still-missing caller than a live
  GPIB/RS-232 command dispatcher.
  **Twenty-third follow-up, 2026-10-13, later pass**: closed the
  sixth and last `[0x690]`-writing orphan function the same way -
  `FUNC_2998_DB69` is byte-for-byte `validate_gpib_device_records`
  (native `0x8DB69` = aliased `0x95B69 - 0x8000`), already named but
  under a stale high-level description ("searches for a matching/
  available entry," estimated ~1000 lines). The real ~192-line body
  is a **pool defragmentation/compaction pass** over the `[0x73E]`/
  `[0x742]` lettered-reference waveform heap: given a caller-supplied
  plan array of (position, record-index, delete-flag) triples, it
  discards flagged records, leaves already-compacted ones in place,
  and physically slides the rest down to close gaps, rewriting
  `[0x73E]+0`/`+4` and `[0x742]`'s far pointers and recomputing
  checksums as it goes. On success it writes the new compaction
  cursor into `[0x746]+8` and the remaining space into `[0x746]+6` -
  **correcting `+8`'s role** from a static "pool base pointer" to a
  live watermark (offset just past the last used byte), with `+6`
  confirmed as `limit - watermark`. No static caller found (same
  shape as its siblings). See `docs/comm-rom/command-keyword-table.md`
  section 11.
  **Twenty-fourth follow-up, 2026-10-13, later pass**: checked the
  twenty-third follow-up's "leading candidate" hypothesis by reading
  `FUNC_2998_C85B` (`SAVeref`)'s entire body (all ~445 lines,
  `0x8C85B`-`0x8CD99`) end to end - **ruled out**. Its only `lcall`
  targets are `set_ds_return_old`, `SUB_96126`, `memcpy_far`, and a
  previously-unidentified function `SUB_95D81` (called once, on the
  length-mismatch/insufficient-room branch); no call to
  `validate_gpib_device_records` (native or aliased form) appears
  anywhere. The other insufficient-room branch just fails with an
  error code instead of compacting. This also completes a
  byte-for-byte trace of `SAVeref` itself (previously "strong
  confidence, not byte-for-byte complete"). The missing caller for
  `validate_gpib_device_records` remains unresolved; `SUB_95D81`
  (physical `0x8D581`) is a new, untraced lead on the same path. See
  `docs/comm-rom/command-keyword-table.md` section 11.
  **Twenty-fifth follow-up, 2026-10-13, later pass**: traced
  `SUB_95D81` to completion, correcting the twenty-fourth follow-up's
  guessed native address (`0x8D581`) to the real one, `0x8DD81`. It's
  a genuine function, now named `vacate_and_compact_gpib_record` - the
  single-record counterpart to `validate_gpib_device_records`: given
  one record index, it vacates that record's slot and compacts every
  subsequent record down to close the gap (walking forward by
  position, not a caller-supplied plan array), then fully reinitializes
  the vacated record via `init_gpib_record`. **This positively confirms
  `SAVeref` DOES call a pool-compaction function** on its insufficient-
  room path - just this one, not `validate_gpib_device_records`/`DB69`
  (whose own real caller is still unresolved; the two findings are
  complementary, not contradictory). While regenerating the listing, a
  second call site turned up inside a previously-unidentified function,
  `FUNC_2998_CEF5` (native `0x8CEF5`), now named `erase_gpib_record` -
  tracing its body resolved it as the **real `REFDisp REF{A-Z}:EMPTY`
  handler**, closing `VARIABLES.md`'s `[0x73E]` row's long-open
  question of whether `B339` or `E1E9` implements `EMPTY` (neither
  does). A further sibling, `FUNC_2998_CFDA`, was noticed toggling the
  `0x04` LOCK bit but not yet traced - flagged as a new lead. See
  `docs/comm-rom/command-keyword-table.md` section 11.
  **Twenty-sixth follow-up, 2026-10-13, later pass**: traced
  `FUNC_2998_CFDA` (physical `0x8CFDA`-`0x8D061`). Given a letter,
  range-checked and interlocked exactly like `erase_gpib_record`, it
  checks `[0x73E]+0xA` bit `0x40` (PERM) - fails outright (`0x400`) if
  set - and otherwise **unconditionally XORs (toggles) bit `0x04`**
  (LOCK), returning success. Toggle semantics don't match the already-
  confirmed explicit-SET `B252`(clears both)/`B2BC`(sets LOCK alone)
  pair, which already cover `REFProt`'s `UNLocked`/`LOCk` SET-sides -
  a third toggle-based function on the same bit would be redundant as
  a GPIB command (no parameter lets a caller pick a direction). Not
  named; leading guess is a **front-panel LOCK-button handler**
  (toggle fits a physical pushbutton better than a keyword), but no
  caller has been traced either way. Also corrected a stale doc-sync
  bug found along the way: `VARIABLES.md`'s `[0x73E]` row had `B252`/
  `B2BC`'s candidate labels swapped (said `B252`=`LOCked`,
  `B2BC`=`UNLocked` - backwards from what their own bit operations do)
  even though `docs/comm-rom/command-keyword-table.md` section 11 had
  already corrected this in an earlier pass; brought back in sync. See
  `docs/comm-rom/command-keyword-table.md` section 11.
  **Twenty-seventh follow-up, 2026-10-13, later pass**: resolved the
  "real caller still unresolved" question left open by the twenty-
  fourth/twenty-fifth follow-ups above. `scan_and_repair_gpib_records`
  (`0x8D476`/`0x95476`) is `validate_gpib_device_records`'s genuine
  static caller, at physical `0x8DB0A` - confirmed by a label-boundary
  check showing every label between that function's entry point and
  the call site is a local jump target, not a separate function. This
  also fills in the second half of `scan_and_repair_gpib_records`'s own
  behavior (previously only its first-half per-record consistency
  loop was traced): it builds a 6-byte-stride plan array on its own
  stack frame, sorts it by pool offset, flags overlaps/gaps/watermark
  mismatches into `[0x746]+0xA`/`+0xC`, and - only if those flags came
  up nonzero and the repair count didn't exceed `6` - calls `validate_
  gpib_device_records` with that plan array to actually compact the
  pool. The plan-array layout it builds matches exactly what `validate_
  gpib_device_records` was already documented to expect, confirming
  this is the designed caller rather than a coincidental hit. See
  `docs/comm-rom/command-keyword-table.md` section 11.
  **Thirteenth follow-up, 2026-10-13**: identified `FUNC_2998_718D`
  (physical `0x8718D`) while chasing the two-adjacent-slots question
  above (didn't resolve it, but found this instead) - a "step to next/
  previous reference satisfying capability bit `0x02`" scanner with a
  flag selecting combined numbered+lettered wraparound vs. lettered-
  only. Its six callers all share a debounced-accumulator shape: a
  signed delta at `[0x52A]` accumulates into `[0x4C6]`, and crossing
  +-250 resets it and steps the per-context current-selection byte at
  `[0x4CA]`+context. Open: whether `[0x52A]`'s delta comes from a
  physical front-panel knob or something else has not been traced to
  its source. See `docs/comm-rom/command-keyword-table.md` section 11.
  **Fourteenth follow-up, 2026-10-13**: searched for `[0x52A]`'s write
  site across the whole comm ROM via three independent techniques
  (direct-displacement stores, immediate-loaded-register forms via a
  raw byte-pattern grep for `0x052a`, and the ROM's only two port I/O
  instructions) - all three come up empty; every occurrence of
  `[0x52A]` in the disassembly is one of the 6 known reads, and
  neither port instruction is positioned near them. Leading hypothesis
  (unconfirmed, no schematic to check against): `[0x52A]` is a
  memory-mapped hardware register - e.g. a front-panel rotary-encoder
  up/down counter latch - that external hardware updates directly,
  with no CPU store instruction involved at all. See `docs/comm-rom/
  command-keyword-table.md` section 11.
  **Twenty-ninth follow-up, 2026-10-13, later pass - supersedes the
  fourteenth follow-up's hardware-register hypothesis**: that search
  was scoped only to the comm ROM; `[0x52A]` is a low-RAM address both
  ROMs' code can reference. A raw byte-pattern grep of all six ROM
  binaries found 3 genuine write sites in the **main ROM**, inside a
  function (`FUNC_3633_9500`, physical `0xE9500`) that exists only in
  the main ROM's secondary heuristic disassembly listing
  (`sysrom_3532_3633_heuristic.lst`), not its primary one. The writes
  store **the difference between a remembered previous value
  (`[0x526]`/`[0x528]`) and the function's current incoming
  parameter** - a software-computed delta, not a hardware latch; the
  "no CPU store instruction involved" hypothesis is now ruled out, not
  just unconfirmed. The function's one known caller is a magnitude/
  decade numeric-formatter dispatcher (`FUNC_3532_51D9`); combined with
  the already-known `compute_and_draw_scale_marker` consumer (reads
  `[0x52A]` as an incremental screen-position shift) and the comm ROM's
  debounced accumulators, a plausible (unproven) unifying story is
  "how much a formatted/plotted value changed since the last update."
  Also re-surfaced, but already resolved before this session
  (2026-10-11, see the matching entry further down this file): a
  mid-function, no-prologue entry point (`SUB_E951A`) inside the same
  function, reached by six separate far-call sites - a real inherited-
  `bp`/`mov sp,bp` frame-collapse bug, not a new oddity. See `docs/
  comm-rom/command-keyword-table.md` section 11.
  **Thirtieth follow-up, 2026-10-13, later pass - confirms the
  twenty-sixth follow-up's "front-panel LOCK-button handler" guess**:
  `FUNC_2998_CFDA` (toggle-LOCK) has exactly one call site in the comm
  ROM, inside `FUNC_2998_63EA` - one of the six debounced-accumulator
  callers of `FUNC_2998_718D`, specifically the lettered-reference
  (`[0x4CB]`, `'a'..'z'`) selector. The call is gated on `[0x4E8]&0x80`
  and `[0x4E7]&0x80` both being set - the identical two-byte, same-bit,
  "both held" condition `VARIABLES.md` already documents as `update_
  menu_position`'s front-panel "accelerate" chord. Confirms by direct
  trace (not just shape-matching) that toggling a lettered reference's
  LOCK state happens via a front-panel control combination while
  stepping through lettered references with the selection knob, not
  via any GPIB command path. Which two physical controls `[0x4E7]`/
  `[0x4E8]` are remains unconfirmed. See `docs/comm-rom/command-
  keyword-table.md` section 11.
  **Thirty-second follow-up, 2026-10-13, later pass - resolves
  `[0x5F7]`'s write site, overturning the fifth follow-up's "lettered-
  reference feature-gate" characterization above**: grepped
  `disasm/160-2998-14.lst` directly for every `[0x5f7]` access (~20
  found, none reached by a resolvable static `lcall` - same dispatch-
  table situation as `[0x5fe]`). It is reset to `0` at the start of
  each command's argument scan (`FUNC_2998_3C35`'s `L_83CF0`, physical
  `0x83CF0`; mirrored in `FUNC_2998_4E99`, `0x84E9F`) and is `[0x5fe]`'s
  direct sibling for operand slot 1 of the generic two-operand argument
  tokenizer (`[0x5fe]`/`[0x5ff]` are slot 2's byte pair) - a per-command
  classification code, not a persistent model/option flag. `4` comes
  from `FUNC_2998_43F0` (`0x84427`), the exact handler already traced
  in `docs/comm-rom/command-keyword-table.md` section 8 as the source
  of `[0x5fe]`/`[0x606]`'s copied value from the letter-bucket keyword-
  matcher's `[0x608]` - so `[0x5F7]==4` mechanistically means "slot 1
  was recognized via the keyword/letter-table matching path," the only
  form that could ever spell a lettered designator like `REFA`. Other
  observed codes: `2` (`FUNC_2998_4240`, `0x84268`, numeric-literal-
  shaped), `5` (`FUNC_2998_42F4`, `0x84318`, colon-subfield-shaped), `6`
  (two untraced sites, `0x83EDE`/`0x83F04`). The external behavior
  documented above (variant-gated validation, `ACQ`/`REF4` collapse)
  stands unchanged; only the "persistent feature-gate" framing was
  wrong. See `docs/comm-rom/command-keyword-table.md` section 12.
  **Thirty-third follow-up, 2026-10-13, later pass - resolves code `6`
  as a parse-error classification and corrects code `2`'s shape**:
  traced the two previously-untraced `0x83EDE`/`0x83F04` sites (inside
  `FUNC_2998_3E79`, parser-state table entry 8) and found both set
  `[0x5F7]=6` together with `[0x623]=0xF` ("unrecognized/invalid
  argument character," independently corroborated this pass by
  `FUNC_2998_1A5B`'s bitmask-to-error-code priority encoder) and a
  transition to the generic cleanup state (`[0x5A4]=1`) - so code `6`
  is "slot 1's scan ended in a parse error," not a fifth successful
  match shape. Along the way, found and named a new generic flag,
  `[0x616]` ("this character is invalid for an argument token," 3 write
  sites, ~14 read sites), and three new helper functions:
  `normalize_and_validate_arg_char` (`0x82FD7`), `hex_digit_to_nibble`
  (`0x82F97`), `decode_hex_byte_pair` (`0x82F3A`). Tracing the last of
  these into `FUNC_2998_4240` (the code-`2` writer) also corrects code
  `2`'s shape from generic "numeric-literal-shaped" to the more
  specific IEEE-488.2 `#H`-style binary-block hex-digit-pair
  accumulator (assembles decoded hex-pair bytes into `[0x612]` via
  `shl`-by-8 + add). Code `5` (colon-subfield-shaped) was not traced
  further this pass. Also noticed but explicitly not pursued: the
  `FUNC_2998_0A8C`/13-small-wrapper-function cluster near `0x804F6`-
  `0x80A70`, shaped like a top-level per-keyword command-dispatch
  mechanism - flagged as a fresh, unexplored lead for a future pass.
  See `docs/comm-rom/command-keyword-table.md` section 13 and
  `VARIABLES.md`'s `[0x5F7]`/`[0x616]` rows.
- Binary/hex `CURVe?` waveform transfer is now fully confirmed **as a
  wire protocol** (see the practical guide), but still isn't tied to
  specific disassembled routines beyond the known ASCII path
  (`print_signed_decimal_serial`/`print_param_list_response`).

  **Thirty-fourth follow-up, 2026-10-13, later pass - the `FUNC_2998_
  0A8C` dispatch cluster**: picked up the lead flagged at the end of
  the thirty-third follow-up. Named and characterized the cluster's
  shared prolog, `FUNC_2998_0A8C` -> `validate_source_and_select_
  response_header` (validates a new selector `[0x4F4]`, sets up
  far-pointer response-formatting fields, derives a header code into
  `[0x556]`) and its own validator, `FUNC_2998_1344` -> `validate_
  source_selector_has_data` (range-checks `[0x4F4]` 0..4, checks a
  capability bit via the point-tag-scan function). One of the 13
  sibling wrapper functions, `FUNC_2998_04F6` -> `build_wfid_
  descriptor_string`, was identified with string-pool evidence
  (`"ACQ"`/`"REF"`/`"SAMPLE"`/`"PEAKDET"`/etc. - matches the manual's
  `WFMpre:WFID` field). **The other 12 siblings remain unidentified**:
  they don't embed direct string-pool references the way this one
  does, so they likely delegate through a shared helper via an index -
  a fresh lead for a future pass, not a dead end. See `docs/comm-rom/
  command-keyword-table.md` section 14 and `VARIABLES.md`'s `[0x4F4]`/
  `[0x556]` rows.

## Stroke-font glyph table (the CRT readout's character shapes)

See `docs/display/vector-display-and-stroke-font.md` and `TODO.md`'s
top item.

- **`[0x1DB0]`'s value probably isn't right yet, but it IS written -
  revised 2026-09-15.** The 2026-09-14 claim "nothing writes `[0x1DB0]`"
  was based on a text search for literal `mov [0x1DB0], ...`-style
  instructions, which misses a real write that exists: `init_far_
  pointer_table_sysrom` (`0xE5EAE`, proven-reachable at boot) copies a
  far pointer into it via a data-driven `movsw` loop, whose destination
  only appears as a raw word value inside an embedded table, never as
  an instruction operand - a literal grep was never going to find that.
  The written value, `E9A3:0000`, was previously argued against because
  it "lands on real compiled code" - rechecked directly and that
  argument was flawed (the real code starts 4 bytes later, a landing
  artifact; a data table has no reason to avoid looking code-adjacent
  either way). The *actually valid* reason to doubt this value stands
  on its own: sampled `char*4`-indexed lookups against it produce
  incoherent far pointers, not a real per-character pattern. Also
  checked whether an aliased write could be hiding under a different
  `DS` segment convention - cataloged every fixed-immediate `DS` load
  in the entire corpus, found only two (`0x41`, the standard convention;
  `0xE5D1`, this same table's own transient source read), no hidden
  third path. The one gap that can't be closed by static text search:
  a few functions load `DS` from a caller-supplied far-pointer argument
  rather than a fixed value - not checked for aliasing. `boot_init`'s
  own separate data-driven init loop was fully traced 2026-09-14 and
  confirmed to be an unrelated RAM march-test routine, closing off that
  specific (different) lead. Full detail in `docs/display/vector-
  display-and-stroke-font.md`'s "Follow-up, 2026-09-15" section right
  after "The readout vector display list".
- **2026-09-16: emulator confirms `E9A3:0000` produces incoherent
  per-character pointers live, and surfaces a bigger, previously-
  unconsidered question.** Built `emulator/` (a headless Unicorn-Engine
  tracer, see `emulator/docs/design.md`), booted the real firmware, and
  instrumented `draw_readout_char` directly. Watched it render the
  power-up self-test banner text (`" POWE"`, confirmed against
  `STRINGS.md`'s "POWER UP FAILURES") - real, meaningful execution, not
  an artifact - and confirmed `[0x1DB0]` really is `E9A3:0000` live in
  RAM, with the 5 resulting per-character far pointers scattered across
  completely unrelated segments (one landing outside any mapped device
  entirely). This is the first *dynamic* confirmation of the "incoherent
  pointers" finding, not just a hand-computed static one. **Bigger
  finding**: `draw_readout_char` starts with `cmp byte[0x1b83],0x14 /
  je <return-immediately>`, and `[0x1B83]` is `detect_comm_option_hw`'s
  own comm-option-presence flag. This emulator run reads `0x1E` (comm
  absent, because the I/O stub for the presence probe is plain
  unbacked RAM) - but **both of this project's real physical test
  units have the RS-232 option installed**, meaning on the actual
  hardware `[0x1B83]` would be `0x14` and this function would return
  immediately, drawing nothing. If that holds up, this function may be
  dead code for readout text on the units actually being tested, and
  the real text-rendering path on comm-equipped units might be
  `write_readout_port_byte`'s still-unresolved `0x40000+0x6F0`
  UART-register-bank overlap (`MEMORY_MAP.md`'s "Puzzle" section) -
  potentially the same mystery as this one, not two separate open
  questions. **Confirmed true, same day, immediately after**: built
  `emulator/io_stubs.py`'s `CommPresenceProbe` to stub the hardware
  probe as "installed" (catching a real pre-existing typo in
  `MEMORY_MAP.md`'s own address table along the way - `0x4007DE`
  instead of the correct `0x407DE`, an extra digit). With `[0x1B83]`
  correctly reading `0x14`, `draw_readout_char` returns immediately on
  every single one of ~48 calls made while rendering the full self-test
  banner, and the emulator runs cleanly to 60M instructions with no
  crash (versus reliably crashing under the default stub). Separately
  confirmed `print_string_far`/`write_readout_port_byte` fire
  **unconditionally** regardless of comm-detection status, carrying the
  complete real diagnostic text live (`'POWER UP FAILURES'`, `'Display
  controller : TIMEOUT'`, etc., captured byte-for-byte). **This closes
  the loop**: on this project's real physical hardware (both units
  confirmed RS-232-equipped), the vector-stroke-font path is dead code
  for this text, and `write_readout_port_byte` is the only channel that
  ever carries it - `MEMORY_MAP.md`'s "Puzzle" section is now marked
  resolved for the practical question, though the port's exact
  schematic device identity is still not proven. Full detail in
  `docs/display/vector-display-and-stroke-font.md`'s "Live emulation
  confirms..." section.
- A structural search for the 128-entry far-pointer array's *shape*
  found zero real candidates in any of the 3 chips (main ROMs and comm
  ROM all tried as of 2026-09-14) - a real scoring bug was found and
  fixed along the way (bit `0x08` was wrongly treated as an invalid
  coarse-nibble bit; it's actually part of the valid fine nibble), but
  loosening thresholds after the fix just lets random noise back in
  rather than surfacing a real table. Blind byte-pattern scanning may
  be fundamentally unable to distinguish real stroke data from
  coincidence here, since (per the bug fix) every non-zero byte is
  syntactically a legal stroke byte.
- **New 2026-09-14**: tried matching against real, live-captured
  ground truth (an isolated "2V" HPGL label) instead of scanning ROM
  bytes blindly. Hit a concrete, reproducible quantization puzzle -
  two different captured characters both need 9 distinct native Y
  levels to fit their plotted coordinates under the most natural
  step-size assumption, one more than the 3-bit coarse field can hold.
  Not resolved; see `docs/display/vector-display-and-stroke-font.md`'s
  "Follow-up, 2026-09-14" for the exact numbers and what's been ruled
  out.
- **2026-09-15: found why the sample-fitting approach was stuck, but
  still didn't resolve it.** Re-read `draw_readout_char`/`plot_readout_
  point` line-by-line and pinned down the *exact* arithmetic: `Y =
  baseline([0x1AF8], captured once at function entry) + coarse` is a
  **raw add** (not `coarse*4`), and `X = fine` is raw/unscaled with no
  per-character offset - `print_readout_string` itself doesn't advance
  any X position between characters either. The confirmed HPGL step of
  4 units, and the character-cell X positions seen in captures (10, 38,
  ...), can't come from this arithmetic - there must be a **separate,
  unfound downstream stage** that walks the `[0x1CC4]` display list and
  converts these internal coordinates into real output. The HPGL-
  sample-fitting approach was implicitly modeling a single-stage
  transform when there are actually (at least) two - explains the
  earlier "overflows by one slot no matter what" result, but checking
  per-character (not pooled) still shows `V`'s own points alone
  overflowing by one slot, so this doesn't fully resolve the puzzle
  either. **Also corrected a real error found along the way**:
  `FUNCTIONS.md` said `draw_readout_char` calls `plot_readout_point_
  relative` - it actually calls `plot_readout_point` directly, verified
  against the real `lcall` target.
- **Found a second, independent reader of `[0x1DB0]`** while chasing
  the above: `FUNC_3633_E60C` (physical `0xEE60C`, now `draw_readout_char_dup2`) uses the exact same
  `char*4` indexing and `[0x1DB0]` far-pointer read as `draw_readout_
  char`, and its own stroke loop extracts `fine`/`coarse` with the
  identical mask/shift formula - a real, structurally-verified second
  consumer, not a landing-artifact coincidence. It writes into a
  different target (`[0x45E]`, with `coarse` added to a running
  accumulator `[0x46E]` instead of a fixed per-character baseline) and
  was originally described as embedded in one giant function starting
  at `0xEDF56`. **2026-10-10 correction: that's not one giant
  function** - `0xEDF56`-`0xEE705` is actually a sequence of several
  separately-prologued, cleanly-`retf`-bounded functions (now
  `dispatch_item_change_notification`/`reset_current_item_to_table_
  default`/`render_item_list_row` plus 2 more still-unnamed ones
  (`FUNC_3633_E35E`/`FUNC_3633_E382`)) that merely share an
  unreached, heuristic-only ROM neighborhood - see `docs/display/
  vector-display-and-stroke-font.md`'s "Correction, 2026-10-10". **Still
  neither has a confirmed caller** - both are `ref_count: 0` in the
  heuristic symbol table, so this doesn't prove reachability, but it's
  real code worth someone naming and tracing forward from. Don't
  confuse this with the nearby, already-documented `dispatch_item_
  handler_if_enabled`/`[0x1D10]` item-table mechanism (a landing
  artifact of the same neighborhood, unrelated to the stroke font) -
  full detail and the exact disassembly in `docs/display/vector-
  display-and-stroke-font.md`'s "Found a second, independent reader of
  `[0x1DB0]`" section. **Correction, checked
  right after finding this**: `[0x46E]`'s accumulator is fed by a call
  to `0xF6510`, which was first guessed to be a character-width lookup
  - it isn't; its actual body is a bounding-box min/max clamp against
  a `0xFFF` limit over an unrelated `×14`-stride record table. What
  `[0x46E]` really represents is unresolved again. **Most promising
  next step now**: find and trace whatever produces real CRT/plotter
  output from either `[0x1CC4]` or `[0x45E]`'s buffer - still
  not found, many candidates, none traced.
- **Tried shape-matching instead of structural scanning - a better
  technique, still no hit.** Rather than searching for a plausible
  pointer-table *structure* (weak, since every byte is a syntactically
  legal stroke byte per the earlier bug-fix finding), converted the
  captured "2" glyph's own HPGL points into a native `(fine, coarse)`
  delta sequence and searched all three ROMs for that exact geometry -
  no valid pointer table needed, since this searches for the glyph
  *data* directly. Zero exact matches (any orientation), zero even for
  a fully permissive sign-only match, and a scale sweep of the Y axis
  found nothing either - match counts only appear once the window
  shrinks to 3-4 deltas, the expected shape of coincidental noise, not
  a near-miss. **The technique is sound and worth reusing once the
  native-to-HPGL transform is fixed** (see the previous bullet's
  "downstream renderer" lead) - it doesn't depend on finding `[0x1DB0]`'s
  pointer table at all, so it's a more direct path to the real glyph
  data than continuing to chase the pointer. Full detail in `docs/
  display/vector-display-and-stroke-font.md`'s "Follow-up, 2026-09-15:
  tried shape-matching" section.
- **2026-09-23: retried the "find a confirmed caller" next step for
  `FUNC_3633_E60C`/`FUNC_3633_DF56` - still nothing.** Grepped every
  `.lst` listing (proven and heuristic, all three chips) for any
  reference to either address - the only hits are the two functions'
  own `push bp` lines, confirming `ref_count: 0` isn't a heuristic-
  scanner gap. A blind byte-scan of all three ROMs for a far pointer
  resolving to either address (trying every segment `0xC000`-`0xFFFF`)
  is not a sound technique (too many trial segments, no selective
  hit) and is recorded as a dead end, not a lead. Also hit, and ruled
  out, a false trail worth flagging again: two heavily-referenced
  nearby functions (`SUB_ECEDA` `0xECEDA` ref=17, `FUNC_3633_CF19`
  `0xECF19`) turned out to be more callers of the *other*, already-
  documented `[0x1D10]` item-handler table (unrelated to the stroke
  font, see the bullet above) - easy to re-conflate mid-search since
  they share this address neighborhood. Net: practical conclusion
  from 2026-09-16 stands unchanged; no new static-analysis avenue
  found. Full detail in `docs/display/vector-display-and-stroke-
  font.md`'s "Follow-up, 2026-09-23" section.
- **2026-10-08: the `[0x1DB0]`-reader family is bigger than thought -
  6 functions across both main-ROM chips now, still no caller.** A
  direct grep for every `[0x1DB0]` reference across both chips' proven
  and heuristic listings at once (not done this way before) found a
  third `160-3633` sibling and, for the first time, **3 siblings in
  `160-3532`** - the other main-ROM half, never previously checked for
  this table. All 6 (`draw_readout_char`, `draw_readout_char_dup1`/
  `_dup2`, `draw_readout_char_3532_1`/`_2`/`_3`) share the identical
  `char*4 -> [0x1DB0] -> far-pointer stroke list` mechanism; only
  `draw_readout_char` is confirmed reachable (and dead on this
  project's real hardware). A 4th `160-3532` function,
  `draw_tick_marks_3532`, shares the family's output buffer
  (`[0x6AE]`/`[0x1C02]`, confirmed shared with `160-3633`'s `[0x45E]`/
  `[0x1C02]`) without reading `[0x1DB0]` itself. No caller found for
  any of the 5 unreached siblings via listing grep or an independent
  Ghidra cross-check (for `draw_readout_char_3532_1`, Ghidra's own
  auto-analysis didn't even create a function there). All 6 named in
  `FUNCTIONAL_NAMES`/`FUNCTIONS.md` and in the Ghidra project itself
  (`decompile/apply_names.py` + re-exported `decompile/exports/*.c`)
  since the mechanism is confidently understood even though
  reachability still isn't. Full detail: `docs/display/vector-display-
  and-stroke-font.md`'s "Follow-up, 2026-10-08" section.
- **2026-10-10, same session: resolved the `0xEE35E`/`0xEE382`
  "unclear which subsystem" open item - 3 unrelated functions, none of
  them item-dispatch.** `0xEE35E` (renamed `scale_and_plot_point_
  chained`) chains 2 calls to the acquisition/scale-and-plot pipeline
  `scale_and_plot_point_default` - belongs to that subsystem, not
  stroke-font. `0xEE382` (left unnamed) computes a confirmed in-bounds
  offset into `plot_readout_point`'s own `[0x1CC4]` display-list buffer
  but its only other action is a call to `SUB_F63BC`, a **second**
  confirmed instance of the `SUB_ECEDA`-style landing-artifact pattern
  (lands inside `draw_marker_box_and_update_position`'s shared tail,
  frame-mismatched, probable noise). Its ROM neighbor `0xEE3BC`
  (renamed `plot_readout_point_list`) is a genuine, confirmed bulk
  point-list appender into that same `[0x1CC4]` display-list buffer,
  reusing `plot_readout_point`'s exact second-plane-offset/delimiter-
  attribute/bounds-check mechanism - plausibly a whole-glyph-outline
  appender for the stroke font, though no caller found yet. See
  `docs/display/vector-display-and-stroke-font.md`'s matching
  2026-10-10 follow-up and `docs/decode-anomalies/landing-artifacts-
  and-jump-tables.md`'s "second confirmed instance" follow-up for the
  `SUB_F63BC` landing-artifact detail.

## Front-panel switches

See `docs/self-test/front-panel-switches.md` and `VARIABLES.md`.

- **`[0x758]`/`SWB2`'s bit map is confirmed** (`MEM 1`/`2`/`3`, `MENU
  ADV`, `SELECT C1/C2`, `MENU`, `1K/4K`, `POS/SEL`) - but these are all
  menu/memory controls, not the analog VOLTS/DIV-style switch the
  `update_menu_position`-range-scan self-test `selftest_front_panel_
  switch_a` is believed to exercise (leading candidate: VOLTS/DIV).
  **Which physical control it actually corresponds to is still
  unconfirmed.** Of the other 2 functions originally grouped with it
  by shape alone: `selftest_tb_divider` was resolved (it's `TB_DIVIDER`,
  confirmed via `HARDWARE.md`), and `selftest_front_panel_switch_b` was
  **renamed 2026-10-09 to `selftest_acq_ab_addr_walk`** - it's actually
  the `ACQ_AB` acquisition-memory address-bus self-test, not a
  front-panel switch at all, despite reusing the same scan shape (see
  `docs/self-test/dispatcher-and-siblings.md`'s correction). So only
  `selftest_front_panel_switch_a` remains an actual open "which
  physical control" question in this group.
- **`[0x759]`/`SWB1`'s bit map is not yet confirmed** - the same
  code-structure-vs-named-bits technique that worked for `SWB2` hasn't
  been repeated here because no literal-address read site has been
  found yet. **Checked exhaustively 2026-10-11**: grepped every
  `[0x759]` reference in `disasm/sysrom_3532_3633_heuristic.lst` (both
  ROMs) - only 2 exist project-wide, and both are plain `mov byte ptr
  [0x759], al/dl` captures from hardware (`INT2_HANDLER_EARLY` at
  `0xE5D85` and `scheduler_tick_service` at `0xE65B1`, both already
  confirmed/named), never a mask/compare/bit-test. Unlike `SWB2`
  (whose bits were validated by matching a `& 0x63`/`& 0x80` code mask
  against named buttons), **no code anywhere in the disassembled
  corpus ever reads `[0x759]`'s bits back** - the technique genuinely
  can't be applied here with what's currently disassembled, not
  because no one looked. `SWB1`'s bit map remains hardware-test-only
  confirmed (`VARIABLES.md`'s `[0x759]` entry), not independently
  cross-validated by code structure the way `SWB2` is.
- **`[0x4E7]`/`[0x4E8]`'s `&0x80` "accelerate" pattern** isn't tied to
  a specific named `SWB1`/`SWB2` bit.

## Acquisition self-test hardware (HS_ACQ/TBD hs/2/TBD ps/2/MM_ACQ/CDT emulator blocker)

See `docs/self-test/hardware-probes.md`'s 2026-10-09 rewrite for the
full derivation.

- **Register identities now fully resolved** (2026-10-09): the
  `configure_measurement_hw`/`run_adc_selftest`/`verify_pattern_with_
  report`/`wait_stable_measurement` cluster's far pointers all resolve
  to already-CONFIRMED (`MEMORY_MAP.md`, Table 3-1) physical
  addresses - `[0x322]`=`0x4377E` (Acquisition Memory Address Buffer
  U3427, the *same register* `ACQ_AB` reads), `[0x31E]`=`0x48000`
  (Acquisition RAM U3418/U3419), `[0x32A]`=`0x437F7` (Clock Delay
  Timer U4231), `[0x32E]`=`0x437EE` (Record Counter), `[0x332]`=
  `0x437DE` (B Delay Timer), `[0x326]`/`[0x336]`/`[0x33A]` similarly
  resolved. No longer an open "which hardware" question.
- **Resolved in the emulator, 2026-10-09**: `io_stubs.
  AdcSelftestReadbackStub` couples `run_adc_selftest`'s plain
  busy-flag+12-bit read of `0x4377E` to `[bp+0xc]+[bp+0x16]`, computed
  generically off the caller's own stack frame at the exact read
  instruction (`0xE137A`) - fixes `HS_ACQ`, `TBD hs/2`, and `MM_ACQ`'s
  `"acq_mem cntr"` mismatch without needing any of their own call-site
  tables decoded. Verified live against a full boot trace. No longer
  open.
- **Also resolved in the emulator, same day**: `verify_pattern_with_
  report`'s "fill @" ramp-pattern mismatch - `io_stubs.AdcRampFillStub`
  mirrors that function's own already-computed "expected" local
  (`[bp-0xe]`) into the buffer byte about to be read, one instruction
  before its comparison (`0xE113A`), since the self-test is defined to
  pass on real working hardware. Fixes `HS_ACQ`, `TBD hs/2`, and the
  previously-unreached `TBD ps/2` - all three no longer appear at all
  in a full boot trace. No longer open; what real hardware mechanism
  *would* fill this buffer on an actual scope remains unknown, but is
  no longer blocking the emulator.
- **Newly found, still genuinely open**: clearing the above revealed a
  fourth, previously-unreached failure, `MM_ACQ` (`selftest_mm_acq`,
  `0xE26D6`) - fails its *own* distinct check, not a call to `verify_
  pattern_with_report`: an inline loop comparing the difference
  between two adjacent scratch-buffer bytes against two fixed allowed
  deltas (`0xFF` or `0xC7`). Genuinely open because there's no
  "already-computed expected value" local here to mirror - writing a
  stub would mean picking actual byte content that produces one of
  those two deltas, which is a guess about real acquisition hardware
  behavior (a DAC/ramp rollover pattern?), not a derivation from the
  self-test's own logic. See `FUNCTIONS.md`'s `selftest_mm_acq` entry.
- **Also resolved in the emulator, same day**: the next-reached
  failure after `MM_ACQ`, `CDT`'s `"PRE-DETRIG"` (`wait_stable_
  measurement`, `0xE2DC9`) - `io_stubs.AcqMemReadyBitStub` ORs bit
  `0x2000` into `[0x4377E]` at the exact check instruction (`0xE2EC8`),
  directly implied by the branch structure ("bit clear -> fail"), not
  guessed. Bit `0x4000` deliberately left untouched - setting it would
  corrupt the downstream numeric range check below. Verified live and
  A/B'd via `git stash` (same `halt_cpu` stop, no regression).
- **Newly found, still genuinely open**: clearing `PRE-DETRIG` revealed
  a *different* `CDT` failure - `"uncaled : min = 0"` / `"uncaled :
  delta = 0"`. `measure_cursor_delta_time` range-checks the raw
  stabilized value of `[0x32A]` (physical `0x437F7`) against
  `[0x55,0x73]` and the delta between two such reads against
  `[0xc8,0xd2]`; plain RAM's default (`0`) falls outside both. Unlike
  the ready-bit check, there's no branch-implied target value here -
  passing requires `[0x32A]` to hold a specific plausible calibration
  constant, which can't be derived from the self-test's own logic
  alone. See `FUNCTIONS.md`'s `measure_cursor_delta_time` entry.

## Front-panel A/D converter self-test (`FP_a2d`/`"[0x1D20]"` cluster, emulator blocker, genuinely open)

See `FUNCTIONS.md`'s `selftest_front_panel_adc`/`selftest_init_channel_hw`
entries for the full derivation (traced 2026-10-09, continuing past `CDT`
in the boot trace).

- **Mechanism fully traced, not yet resolvable**: `selftest_front_panel_
  adc` (`0xE296E`, `FP_a2d`) makes 3 calls to `selftest_init_channel_hw`
  (`0xE2AB0`, channels `0x22`/`0xE0`/`0x40`), sums the 2nd+3rd results,
  and range-checks the sum against `[0x100,0x700]` (`"gnd = <hex> <>
  5"` on failure). Each `selftest_init_channel_hw` call writes a command
  sequence to a far-pointer hardware register block at `[0x1D20]`, then
  busy-polls a countdown (seeded `0x800`) waiting for either an ISR-set
  flag (`[0x1AEE]&1` - no ISR for this chip located yet) or a status bit
  (`es:[di+5]&4`); if the countdown hits 0 first, it prints `"<label> :
  TIME-OUT"` and returns `0xFFFF`. Live-captured trace (`"FP_a2d :
  TIME-OUT"` then `"FP_a2d : gnd =  FFFF> 5"`) matches this exactly: the
  1st call times out, the 2nd call also times out (`0xFFFF` summed with
  the 3rd call's near-zero result lands outside `[0x100,0x700]`).
- **Why this is genuinely open, not just unstubbed yet**: the `0x4`
  status bit is directly derivable the same way `AcqMemReadyBitStub`'s
  bit was ("bit clear -> keep waiting/fail" implies "pass needs it
  set") - but fixing *only* that doesn't make `FP_a2d` pass. Once the
  busy-wait resolves, the function still builds its real return value
  from reading the chip's data register (`es:[di+4]`) *twice* and
  combining the bits (`(read1<<2) + (read2>>6)`) - a dual-read ADC
  sampling scheme. With that register's unstubbed default (`0`), the
  result is still `0`, which still falls outside `[0x100,0x700]` - just
  with a different printed message (`"gnd"` instead of `"TIME-OUT"`),
  not an actual pass. A real fix needs a plausible ADC sample value,
  which is content-guessing exactly like `selftest_mm_acq`'s delta
  check - so no stub was attempted for either half of this one.
- Hardware identity: `[0x1D20]`'s command sequence plus the dual-read
  data construction strongly suggest a real front-panel A/D converter
  chip (3 channels for `0x22`/`0xE0`/`0x40`, consistent with the
  self-test's own name) - but this isn't independently confirmed
  against a schematic/service manual.

## Main ROM revision cross-check (`ROMS`/`"MISMATCH"`, resolved mechanism, one open question remains)

See `FUNCTIONS.md`'s `selftest_rom_checksum` entry for the full derivation
(traced 2026-10-09, same session as `FP_a2d` above).

- **Fully resolved, not an emulator issue**: `selftest_rom_checksum`
  (`0xE16EA`, `ROMS`) is a revision-byte cross-check, not a computed
  checksum. It compares a one-byte "revision" field (offset `+4` of a
  small embedded header: id word, BCD part-number digits, revision
  byte, `0xEB` sentinel, then a `"Copyright..."` string) at physical
  `0xE0000` (start of `160-3633`'s low 32KB) against the same field at
  physical `0xE8000` (start of its high 32KB). The real ROM bytes give
  `0x14` and `0x4C` respectively - genuinely different - which
  reproduces the live `"ROMS : MISMATCH,14,4C,14"` trace exactly, with
  the 3rd value always the byte at literal physical `0x80004` (`160-
  2998`'s own header, `0x14`). Nothing here needs a stub: both bytes
  are plain ROM content the emulator already maps correctly, and
  `[0x1DD4]`/`[0x1DD8]` (the two far pointers used) are themselves
  correctly initialized by `init_far_pointer_table_sysrom`'s existing
  embedded table - an earlier same-day pass had wrongly ruled that
  table out as the source, by comparing its dest-offsets (`ES=0x209`-
  relative) directly against `[0x1DD4]`/`[0x1DD8]`'s flat-space
  offsets without converting through the base-segment difference.
- **One open question remains**: whether physical `0xE8000` is
  supposed to be a second physical EPROM chip's own header (i.e.
  `160-3633`'s logical 64KB mapped range is built from two separately
  revision-stamped 32KB chips, matching the service manual's Table 3-1
  "low/high half of U9109"/"low/high half of U9110" chip-pair
  language that `MEMORY_MAP.md` elsewhere set aside - but for a
  different, unrelated question, whether `160-3532`/`160-3633`
  interleave across *address space*, which stays correctly rejected
  and is orthogonal to this) - or whether `0xE8000` was never meant to
  be a header location at all and `0x4C` is just an incidental code/
  data byte that happens to sit there, making this self-test's check
  spurious. The actual bytes at `0xE8000` (`f7 ea c4 1e 4c 1d 8b f8 26
  c4 51 02 89 56 f4 8c`) don't contain a recognizable `"Copyri"` run
  the way the genuine header at `0xE0000` does, which leans toward the
  "incidental byte" explanation, but isn't conclusive either way
  without an independent source (e.g. a second physical unit's dump,
  or schematic-level chip-count confirmation) - left open rather than
  guessed.

## Comm ROM checksum (`COMM_ROM`/`"0C8F <> 2BA3"`, resolved - genuine ROM-content mismatch, not an emulator gap)

See `FUNCTIONS.md`'s `selftest_comm_rom`/`verify_rom_checksum_and_report`/
`compute_range_checksum` entries for the full derivation (traced
2026-10-09, same session as `ROMS` above).

- **Fully resolved**: `selftest_comm_rom` checksums the comm ROM
  (`160-2998`) against its own embedded expected value. The stored
  expected value is the big-endian word at the ROM's own first 2
  bytes (physical `0x80000`/`0x80001`, `0x2BA3`); the computed value
  chains `compute_range_checksum` (a shift-left-then-add-with-carry
  running checksum, 1 byte at a time, inclusive range) over physical
  `0x80002-0x87FFF` (the ROM's low 32KB minus its own 2-byte checksum
  header) then `0x90000-0x97FFF` (the upper 32KB, read via its real,
  independently-confirmed own address - not the "RAM alias" reading
  this project corrected away from in `MEMORY_MAP.md`). Running the
  exact same algorithm directly over `binary/160-2998-14.bin` in
  Python gives computed=`0x0C8F`, expected=`0x2BA3` - reproducing the
  live `"comm_rom_0 0C8F <> 2BA3"` trace exactly.
- Like `ROMS` just above, **this is conclusively not an emulator-
  fidelity question** - the whole computation is deterministic,
  ROM-only arithmetic with no RAM or stub dependency whatsoever. The
  dumped `160-2998-14.bin` file's content simply doesn't satisfy its
  own embedded checksum. No further investigation is possible from
  the emulator side; resolving *why* would need either a second,
  independently-dumped copy of this ROM to compare against, or giving
  up on finding an explanation beyond "this dump's checksum doesn't
  verify" (the same category of finding as `ROMS`, and plausibly
  related to it - both are consistency checks on ROM content that
  this specific set of dumps fails).

## Comm-board loopback flag check (`COMM_LB`/`"FGET NOT SET"`/`"FGET NOT CLEAR"`, failure mechanism fully traced - genuine stub candidate, deliberately left unstubbed)

See `FUNCTIONS.md`'s `selftest_comm_loopback_b` entry for the full
derivation (traced 2026-10-09, same session as `ROMS`/`COMM_ROM`
above - this was the last diagnostic line left from that session's
full-boot-trace sweep).

- **Mechanism fully traced**: `selftest_comm_fget_flag` (`0xE1FBC`)
  writes a literal command byte to physical `0x406F3` (the 4th
  register of the comm-option's 8-register UART/GPIB bank,
  `0x406F0`-`0x406F7`), then checks `comm_stat` (`0x4067C`) bit `0x4`
  (`TBRE` per `MEMORY_MAP.md`'s Table 7-36) and `comm_param`
  (`0x406BC`) bit `0x80` (the UART's own live serial-data-output line,
  per that file's 2026-09-16 schematic-trace finding). Subtest 1
  writes `0x86`, needs both bits SET; subtest 2 writes `6`, needs both
  CLEAR. Both fail in the live trace, and both failures are fully
  explained, deterministically, by the current emulator model: the
  write target (`0x406F3`) isn't one of the two registers
  `io_stubs.InteractiveUartMock` actually models (`0x406F0`
  data/`0x406F1` control, the standard 8251 pair), so it never updates
  the i8251 core's `command` register - `comm_stat` bit `0x4`
  (mirrored from `chip.txrdy_r()`) stays permanently `0`, failing
  subtest 1 unconditionally. `comm_param` bit `0x80` is a static
  `COMM_OPTION_STUBS` baseline (`0xF8`) with no dynamic coupling to
  anything, so it stays permanently `1`, failing subtest 2's CLEAR
  requirement unconditionally too.
- **Deliberately left unstubbed, not a bug to fix**: `MEMORY_MAP.md`
  already flags `0x406F1`-`0x406F3` as plausibly TMS9914A (GPIB chip)
  register space rather than confirmed UART registers (the
  `init_readout_port_config`/`write_readout_port_byte` puzzle, "eight
  internal registers" count match). There's no independently-confirmed
  real register semantics for `0x406F3` to build a stub from - doing
  so would mean guessing how a write there is really supposed to
  affect `comm_stat`/`comm_param`, which this project has no source
  for. Same category as `MM_ACQ`/`FP_a2d`'s `"gnd"`/`CDT`'s
  `"uncaled"`: real hardware content genuinely unknown, left open
  rather than guessed.
- **What would resolve this**: either a real-hardware exerciser-screen
  trace of `comm_stat`/`comm_param` while deliberately toggling
  whatever `0x406F3` really is (GPIB-chip register vs. a 3rd
  UART-adjacent register), or a schematic trace of the comm board's
  UART/GPIB chip-select decoding for that specific address - the same
  kind of evidence that already resolved `comm_param`'s/`comm_stat`'s
  other bit assignments earlier in `MEMORY_MAP.md`.

## Display / CRT readout hardware

See `docs/display/readout-memory.md` and `TODO.md`.

- **Reconciled 2026-09-16 - this item's own two halves had drifted out
  of sync with each other; see `MEMORY_MAP.md`'s "Puzzle" section for
  the up-to-date version.** The *live-hardware* half below (an RS-232
  listener at the correct baud rate seeing zero bytes during whatever
  was tested) is left as-is - genuinely not re-tested since. But the
  *mechanism* half is now settled by the 2026-09-16 emulator work:
  `write_readout_port_byte` (physical `0x40000+0x6F0`) demonstrably
  **is** the real, unconditional channel for self-test/POST banner
  text - confirmed dynamically by booting the real firmware and
  watching it carry the complete real diagnostic text (`"POWER UP
  FAILURES"`, etc.) live, regardless of comm-option-installed status.
  So the *role* (real text-output channel, not a dead/mistaken guess)
  is confirmed; what's still genuinely open is only whether that
  channel's *physical destination* is the RS-232 UART specifically (as
  the address's location inside the UART/GPIB register bank suggests)
  or something else the address happens to overlap with, given the
  live-listener test's zero-byte result was never explained away. If
  picked up again: check whether the zero-byte live test was run
  during the *same* kind of self-test/boot text this emulator finding
  covers, or a different code path (e.g. an `ID?`/command-response
  path) - a mismatch there would resolve this cleanly without any
  further hardware contradiction. `init_readout_port_config`'s literal
  bytes (`0x29`/`0x23`/`0x06`) still haven't been checked against
  documented UART/GPIB mode-register constants for a chip of this
  era - now more concretely checkable, since `MEMORY_MAP.md`'s
  "RS-232 option board" section (same day) identifies the UART part
  number directly as an **82C52**.
- **Whether `0x40000-0x4FFFF` is a single memory-mapped display or two
  separate chips (character + attribute/inverse-video planes) isn't
  confirmed** - the `+0x8000` dual-plane pattern is observed but not
  tied to a specific hardware document.
- **The mechanism that auto-advances a write position across readout
  writes hasn't been found** - not yet clear whether it's a
  software-maintained cursor or a hardware auto-increment feature.
- **Screen data paging is confirmed absent, 2026-10-14** - an
  exhaustive grep of `MEMORY_MAP.md`/`FUNCTIONS.md`/`VARIABLES.md`/
  every `docs/display/*` file found zero evidence of page-flipping,
  bank-switching, or double-buffering for either CRT buffer (readout
  text memory or the vector display list). Every "bank"/"page"/"flip"
  hit in current docs is unrelated (ACQ RAM banks, front-panel switch
  banks, service-manual page numbers, the vector-icon Y-axis-flip
  rendering option). Treat this as a real, confirmed gap - not a hint
  that paging logic is still waiting to be found. See
  `docs/architecture/subsystem-object-model.md` section 4.
- **The vector display list's hardware consumer has never been
  found** - the producer chain (`print_readout_string`->
  `draw_readout_char`->`plot_readout_point_relative`->
  `plot_readout_point`, writing to the stroke-font buffer at physical
  `0x02200`) is well understood, but nothing that reads that buffer
  back and actually paints it to the CRT has been traced. Doubly moot
  on this project's own physical test hardware: `draw_readout_char` is
  gated by `[0x1B83]` and returns immediately without drawing on both
  of this project's test units (both Option-12/RS-232 equipped). See
  `docs/display/vector-display-and-stroke-font.md` and
  `docs/architecture/subsystem-object-model.md` section 4.

## Configuration settings persistence (NVRAM/EEPROM)

**Confirmed absent, 2026-10-14.** No load-from or save-to a
non-volatile store has been found anywhere in either ROM.
`selftest_cmos` only *tests* the CMOS chip (a self-test diagnostic,
pass/fail), it never reads settings from it or writes settings to it.
The staged-settings-commit mechanism (`commit_pending_settings_and_
refresh_display`, `0xE8D64`, decoding raw bytes `[0x4e0]`-`[0x4fc]`
into flags `[0x1b6x]`-`[0x1b8x]`) only moves data between RAM
structures - nothing in its call chain reaches outside RAM. As far as
current documentation shows, every boot starts from code-level
defaults; whatever front-panel/menu settings a user configures appear
not to survive a power cycle. Worth keeping an eye out for a
not-yet-found candidate (an unidentified I/O port write, or a call
into a not-yet-traced function) rather than assuming this is settled -
but nothing has turned up despite looking. See
`docs/architecture/subsystem-object-model.md` section 6.

## I/O ports (true 8086 port-space `in`/`out`, not memory-mapped)

See `MEMORY_MAP.md`'s "I/O ports actually seen in code" and
`docs/hardware-io/shift-register-and-assert.md`.

- **`0x83`'s identity is a candidate, not confirmed** - sits in the
  middle of HPGL plotter command-generation code, suggesting a GPIB/
  plotter output port, but not verified against a schematic.
- **`0xC4`/`0xD1`'s specific peripheral is unconfirmed.** The
  *mechanism* is solid (`write_hw_shift_register` clocks a value out
  via 3×`0xD1` + 1×`0xC4`, the classic shift-register-to-DAC/latch
  shape), but which real hardware it drives isn't pinned down. Leading
  candidates: an attenuator/gain/offset calibration DAC, or the
  AUX-connector's pen-lift relay / X-Y plotter output DACs (per
  `HARDWARE.md`'s photos) - worth checking whether this write
  correlates with `update_plot_position`/`plot_line_to`'s pen-up/down
  state (`[0x6CA]`).
- No genuine *runtime-computed* port access has been found anywhere in
  the corpus - all three confirmed ports use literal, fixed numbers.

## Decode anomalies (landing artifacts, dual entry points)

See `docs/decode-anomalies/dual-entry-points.md` and
`docs/decode-anomalies/landing-artifacts-and-jump-tables.md`.

- **`find_landing_artifacts.py` found 49 candidate call targets that
  land 1-4 bytes short of coherent code; the top 5 by independent
  caller count have now all been traced** (all 5 turned into real
  findings - `write_hw_shift_register`'s dual entry point, the
  `SUB_EAC86` family landing on real data, and 3 found 2026-09-14 by
  ranking all 49 candidates by caller count, a technique now built
  into the tool itself: `compute_and_print_item_delta_readout` (33
  callers), `decimate_peakdet_samples` (21 callers - a clean, complete
  trace of the standard min/max peak-detect envelope-compression
  algorithm, confirming a real software implementation behind the
  `PEAKDET` acquisition mode seen throughout this project's live
  testing, and correcting an existing vague cross-reference in
  `handle_gpib_device_clear`'s entry), and `compute_and_print_cursor_
  position_readout` (18 callers - a likely sibling of `compute_and_
  format_sample_delta_readout`, sharing its `extract_strided_channel_
  samples` helper, printing cursor position rather than delta values).
  **The other ~44 weren't individually chased** - the caller-count
  ranking has now been exhausted down to single-digit counts, so
  further candidates are progressively less likely to be worth the
  effort per the project's own "many callers = deliberate" heuristic,
  though not ruled out. **Immediately followed up on the shared helper
  itself**: `SUB_E97DC` is now named `extract_strided_channel_samples`
  - turned out to be a strided/de-interleaving copy utility (not a
  print helper as guessed), with 2 more legitimate secondary entry
  points (`0xE97CA`, real entry `0xE9744`) skipping its remainder-
  alignment preamble, the same "caller already has params computed"
  shape as `write_hw_shift_register`. Strong but unproven hypothesis:
  pulls one channel's samples out of interleaved dual-channel
  acquisition memory, given all 3 known callers are per-channel
  measurement/readout functions. **Checked the other two leads and
  both turned out to already be resolved from earlier sessions**:
  `0xF830E` is already `read_acq_sample_with_wrap`, a fully-documented,
  confirmed function (earlier notes in this file mistakenly called it
  "unnamed" - corrected); `SUB_E99DF` (the other helper `compute_and_
  format_sample_delta_readout` calls) is the already-documented
  landing artifact whose `ljmp` target (`0x88729`) resolves against
  the *caller's* stack frame, not statically resolvable further per
  `docs/decode-anomalies/dual-entry-points.md`. Tracing the real,
  coherent function physically adjacent to it (`0xE999E`, a group-of-4
  byte/word decimation routine) turned out to be a heuristic-only
  orphan with zero confirmed callers - not worth naming without more
  context. This specific sub-thread is now exhausted.
- **2026-10-09: traced the next candidate down the caller-count
  ranking, `0xE951A` (6 callers) - this one did NOT turn into another
  real finding.** Unlike the 5 above, its fake 3-byte decode only
  reconverges with real code on one side of the real branch it lands
  short of (not byte-exact on every path), and a stack-accounting
  check across all 6 real call sites (each pushes 7 words/14 bytes in
  an identical `(word, far ptr DS:0x674, far ptr <dynamic>)` idiom,
  cleaned via `add sp,0xa` after return) found only 12 of those 14
  bytes get cleaned between the caller and the nearest enclosing
  function's own exit (`retf 2`) - concrete evidence this is *not* a
  `write_hw_shift_register`-style secondary entry point into that
  function. Real identity unresolved. See `docs/decode-anomalies/
  landing-artifacts-and-jump-tables.md`'s "Follow-up, 2026-10-09"
  section.
- **2026-10-09: traced the other 6-caller candidate, `0xF4CE8` - this
  one DID turn out structurally clean**, byte-exact reconvergence on
  every path (no branch dependency, unlike `0xE951A`) and all 6 real
  callers push/clean zero bytes, consistent with a genuine `write_hw_
  shift_register`-style secondary entry point. Surfaced a new,
  previously-undocumented variable cluster along the way: a
  single-byte self-test-step code at `[0x3E2]` (written only ever as
  `'s'/'u'/'d'/'l'/'r'/'x'`, always immediately paired with this same
  `SUB_F4CE8`+`SUB_F5F56` call pair) and a `0x63`-bitmask check on
  `[0x4E7]`/`[0x4E8]`/`[0x4E9]`/`[0x542]` feeding the `u`/`d`/`l`/`r`
  branch selection - `0x63` is the same mask already confirmed for
  `SWB2`'s 4 menu-navigation buttons in `docs/self-test/front-panel-
  switches.md`, but applied to a different variable and with no clean
  bit-to-letter correspondence, so the connection is flagged, not
  claimed. **Mechanism clarified same day**: both `SUB_F4CE8` and
  `SUB_F5F56` skip their own prologue, so `bp` (and thus `[bp+6]`/
  `[bp+0xa]`/`[bp+0xc]`) is inherited from the *enclosing* function's
  frame - the same shared-frame mechanism `0x88729`'s `ljmp [bp+di]`
  already established, not a missing-argument mystery. The remaining
  open piece is only that the enclosing functions (`selftest_sequence_
  enter`/`FUNC_3532_7C00`) have no confirmed real caller yet, so the
  actual parameter/`cl` *values* still need emulator tracing to pin
  down. See `docs/decode-anomalies/landing-artifacts-and-jump-
  tables.md`'s "Follow-up, 2026-10-09" section and its same-day
  correction subsection.
- **2026-10-10: traced the next candidate down, `0xECE82` (5 callers)
  - a third instance of the shared-frame mechanism, not a new
  mystery.** Byte-exact unconditional reconvergence (it's literally the
  displacement byte of a local `je`, so an external call never
  evaluates the branch). `FUNC_3633_CE5B`'s real, fully-prologued entry
  (`0xECE5B`) has zero confirmed callers anywhere - only the
  landing-artifact offset is ever reached, same shape as `extract_
  strided_channel_samples`/`SUB_F5F56`. All 5 real callers push 8 bytes
  (2 far pointers) and clean only 4 via `add sp,4`, but this is **not**
  a repeat of `0xE951A`'s genuine mismatch: the function's shared exit
  does `mov sp,bp` before `pop bp`/`retf`, and since `SUB_ECE82` never
  sets its own `bp`, that reset collapses the frame against the
  *inherited* `bp` from the enclosing function - a runtime quantity the
  simple push-vs-retf-immediate arithmetic can't account for statically
  (confirming the exact byte count would need the emulator to observe a
  real `bp`/`sp` pair at a call site). General lesson for the rest of
  this candidate list: check for `mov sp,bp` in the shared exit before
  trusting that stack-accounting check. See `docs/decode-anomalies/
  landing-artifacts-and-jump-tables.md`'s "Follow-up, 2026-10-10"
  section.
- **2026-10-10: resolved the 4 helpers flagged from `dispatch_item_
  change_notification`/`render_item_list_row` (`SUB_F43CC`/`SUB_F408E`/
  `SUB_F45A4`/`SUB_F47AB`) - all 4 are landing artifacts, none is a
  separate function.** `SUB_F408E` lands inside the already-named,
  highest-caller-count landing-artifact function `compute_and_print_
  item_delta_readout` (`0xF3EA3`) - simply one of its own previously-
  unwalked internal branches. The other 3 land inside 3 real,
  cleanly-prologued sibling functions (`FUNC_3532_42E6`/`FUNC_3532_
  4542`/`FUNC_3532_4629`), originally framed as one adjacent SREF
  reference-waveform readout cluster. **Correction, same day**: that
  framing was wrong for `FUNC_3532_4542`, renamed `select_channel_acq_
  preview_source` - it's an acquisition-mode preview-icon selector
  (SAMPLE/AVERAGE/PEAKDET/ACCPEAK, confirmed via the `0xFF7B` string
  table), not SREF. Also found the generic settings-commit mechanism
  behind the shared `[0x1b70]`-`[0x1b8c]`/`[0x53a]` flags: a settings-
  byte-translator family (`SUB_F6E7C`/`SUB_F7045`/`SUB_F7273`, reading
  raw settings bytes `[0x4e0]`-`[0x4fc]`, none yet identified) feeding
  a master commit dispatcher (`0xE8D64`, now `commit_pending_settings_
  and_refresh_display`) - a generic mechanism used well outside SREF
  (>100 grep hits across both ROMs). `FUNC_3532_42E6`/`FUNC_3532_4629`
  remain unnamed (only `4629`, via its `SUB_F47AB` string proximity,
  still plausibly SREF-specific). See `docs/decode-anomalies/landing-
  artifacts-and-jump-tables.md`'s "Follow-up, 2026-10-10" sections
  (second and third) and `docs/acquisition-and-plotting/mode-
  dispatcher-and-formatting.md`'s matching follow-up.
- **2026-10-10 (same session): resolved the 2 remaining ranked
  candidates, `0xEDDE2` (4 callers) and `0xEFD99` (3/1 callers) - both
  landing artifacts, plus a new unresolved third case, `SUB_EFDC2`.**
  `0xEDDE2` lands 2 bytes into the real `cmp ax, 0` inside the newly-
  named `format_word_decimal_and_copy_out` (`0xEDD89`) and always
  falls through to that function's copy-out phase, skipping digit
  extraction. `0xEFD99` lands 1 byte into the tail of `les di, ptr
  [0x1ddc]` inside an unnamed per-channel sample-processing function
  (`FUNC_3633_FC94`, `0xEFC94`). Both confirmed via direct raw-byte
  decode. Open/unresolved: why `format_word_decimal_and_copy_out`'s
  own space-padding fill is never copied by its output path; the
  inconsistent stack-parameter push counts across `0xEDDE2`'s 4
  callers; what `FUNC_3532_B529` (called twice by `FUNC_3633_FC94`)
  does **(resolved 2026-10-11, see below - now `compute_sample_
  position_value`)**. A third case, `SUB_EFDC2` (3 callers, all from a separate
  dispatcher `FUNC_3633_9611`), was flagged as an open third category.
  See `docs/decode-anomalies/landing-artifacts-and-jump-tables.md`'s
  matching 2026-10-10 follow-up and `FUNCTIONS.md`'s new
  `format_word_decimal_and_copy_out` entry.
- **2026-10-11: corrected the `SUB_EFDC2` framing above - it's neither
  a clean secondary entry nor plain garbage, it's a third mechanism
  (inherited-`bp` + a `mov sp,bp` exit collapses the *caller's* frame
  too).** Traced the exact stack arithmetic for `FUNC_3633_9611`'s 3
  call sites: `[bp-0xe]` at `SUB_EFDC2`'s entry is the just-pushed far
  return address's own IP half (not a channel index - confirmed
  garbage, as suspected), but more importantly, `SUB_EFDC2`'s shared
  exit (`mov sp,bp`/`pop bp`/`retf 2`) uses the *inherited* `bp` -
  `FUNC_3633_9611`'s own `bp`, never reanchored - so it unwinds
  `FUNC_3633_9611`'s entire frame and returns straight to
  *`FUNC_3633_9611`'s own caller*, never back to the `add sp,2`
  instruction after the `lcall`. Every instruction after each of the 3
  `lcall`s inside `FUNC_3633_9611` (the shift-register writes in
  `SUB_E962C`, the `[0x502]` table reads, the final `jmp L_E972C`) is
  dead code along this path - unreachable regardless of `SUB_EFDC2`'s
  internal branch. **Plausible, not confirmed**: `0xE951A`'s still-
  unresolved "14 pushed/12 cleaned" stack mismatch has the same 2-byte
  asymmetry signature this mechanism produces - worth re-examining with
  this specific check before re-deriving from scratch. Not resolved:
  `FUNC_3633_9611`'s `ax=5..14` path (via `SUB_EFFEE`, whose own
  landing point wasn't pinned down - it decodes into a confusing region
  that appears to cross into the `160-3532` ROM's address space); and
  whether `FUNC_3633_9611` (`ref_count: 0`, no confirmed callers of its
  own) is a real function at all. Left unnamed. See `docs/decode-
  anomalies/landing-artifacts-and-jump-tables.md`'s "Follow-up,
  2026-10-11" section.
- **2026-10-11 (same session, later): cross-checked `SUB_EFDC2`/
  `SUB_EFFEE`/`FUNC_3633_9611` against Ghidra's own independent
  analysis.** Used `CreateFunctionCmd` (the project's established
  authoritative boundary test, see `docs/architecture/ghidra-
  project.md`) plus a query of Ghidra's existing, already-committed
  auto-analysis function boundaries. Results converge with the manual
  finding above: Ghidra's own auto-analysis independently folds both
  `SUB_EFDC2` and `SUB_EFD99` into the *same* function as
  `FUNC_3633_FC94` (`FUN_000e_fc94`, body `0xEFC94`-`0xEFE29`) -
  ending at exactly the last byte of the `retf 2` traced by hand as
  `SUB_EFDC2`'s real shared exit. Second, independent confirmation
  that it's a clean mid-function landing point, not a byte-overlap
  artifact (doesn't speak to the frame-collapse/dead-code conclusion,
  which is a dataflow fact outside what boundary analysis checks).
  `FUNC_3633_9611` already has a genuine Ghidra function
  (`0xE9611`-`0xE9731`) - consistent with clean code, independent of
  whether anything calls it. New lead: forcing `CreateFunctionCmd` at
  `SUB_EFFEE` returns a **degenerate zero-length body**
  (`0xEFFEE`-`0xEFFEE`, start==end) - Ghidra's disassembler can't form
  a coherent instruction stream there at all, corroborating the
  "confusing region crossing into 160-3532" observation and pointing
  toward `SUB_EFFEE` being dead/incoherent garbage rather than a clean
  landing point. `SUB_EFFEE`'s real mechanics still not pinned down.
- **2026-10-11 (same session, later): resolved `SUB_EFFEE` by hand -
  the Ghidra-probe-based "dead garbage" guess directly above was
  wrong.** Manually decoding raw bytes from `0xEFF99` onward (well
  before the landing point) shows real, coherent code - a
  `[x+0x57c]`-table read matching the neighboring `FUNC_3633_FE2A`'s
  pattern, not noise. It's the untraced tail of `FUNC_3633_FF72`
  (`0xEFF72`): neither the heuristic scanner nor Ghidra's own
  auto-analysis ever disassembles anything past `FUNC_3633_FF72`'s own
  `jmp` at `0xEFF96` (confirmed directly against Ghidra's live
  function/instruction tables - both empty there), which is why
  `CreateFunctionCmd` returned a degenerate body rather than evidence
  of garbage. `SUB_EFFEE`'s landing point (`0xEFFEE`) sits exactly 2
  bytes into the real `jmp rel16 +0x27` instruction at `0xEFFEC`,
  reinterpreting its displacement high byte as a fresh opcode - the
  same mechanism as `SUB_EDDE2`/`SUB_EFD99`, not a new case. **New
  open question surfaced**: that `jmp`'s own target isn't settled -
  the displacement arithmetic overflows 16-bit IP, and neither
  resolution convention (carry into the next 64 KB, landing in
  `160-3532`; or true wraparound within the segment, landing inside
  `160-3633`'s own copyright-string header) produces a sensible
  target. Flagged as a possible systemic bug in the heuristic
  disassembler's cross-64 KB jump-target computation, worth checking
  against other `L_F0xxx`-labeled near-jump targets project-wide, and
  a good candidate for the emulator to settle by single-stepping real
  execution rather than static reasoning. See `docs/decode-anomalies/
  landing-artifacts-and-jump-tables.md`'s matching 2026-10-11
  follow-up. **Partially addressed, same day (later still)**: audited
  every project-wide near-jump crossing the `160-3633`/`160-3532` chip
  boundary - only 2 exist total (this one, plus a newly-found second
  instance, `SUB_F1581`/`0xF1584`), and both are confirmed landing
  artifacts decoding garbage operand bytes, not real compiled jumps.
  Narrows the "possible systemic bug" framing (no evidence found of a
  real compiled jump being mis-targeted by the scanner) but doesn't
  resolve the underlying curiosity about Capstone's own odd operand-
  string arithmetic for these misdecodes, since the bytes are garbage
  either way. See the matching 2026-10-11 (later still) follow-up in
  the same doc.
- **2026-10-11 (same session, later still): resolved what `FUNC_3532_
  B529` (called twice by `FUNC_3633_FC94`, flagged above) does -
  renamed `compute_sample_position_value`.** Packs an 11-bit value from
  2 bytes of a far source record, then either rescales it via a new
  sibling helper (`scale_offset_by_code`, `0xFAA08` - a selectable-
  fraction shift-add multiply, 5-bit code picks the shift chain) or
  adds a linear offset, and writes the result into a far destination
  record via the already-named `set_position_record_3532`. Confirmed
  the `sample` parameter is genuine acquisition data (not a coincidence
  of naming) by checking the one of its 10 call sites with a nearby
  self-describing prologue (`0xFB2EE`), where it's fed straight from
  `read_acq_sample_with_wrap`. Open/unresolved: the exact fraction each
  `scale_offset_by_code` code value (1/2/4/8/9/other) encodes, and the
  specific source/destination record identities at the other 9 call
  sites. See `changes/2026-10-11.md`'s matching entry and `FUNCTIONS.md`.
  **Also surfaced a separate environment gap while regenerating**:
  `nasm` is not installed/on `PATH` in this session, so `disasm/
  gen_source_readable.py` could not run and `disasm/*_readable.asm`/
  `binary/aligned/*_aligned.bin` were NOT updated with these two new
  names (confirmed via `git status` - those tracked files are
  unchanged, not partially regenerated) - needs a re-run once `nasm` is
  available again.
- **2026-10-11 (same session, later still): resolved `0xE951A`'s "14
  pushed/12 cleaned" stack mismatch from 2026-10-09 above - it's the
  same inherited-`bp`/`mov sp,bp` frame-collapse mechanism as
  `SUB_EFDC2`/`SUB_ECE82`, not a new/unexplained bug.** All 6 real
  callers `lcall` straight into `SUB_E951A` (`0xE951A`), well past
  `FUNC_3633_9500`'s real prologue, so its shared exit's `mov sp,bp`
  uses the *calling* function's own inherited `bp`, collapsing straight
  to that caller's frame and returning 2 bytes (1 word) short of what
  that caller's own `retf 4` would have cleaned (confirmed against 2 of
  the 6 call sites, `FUNC_3532_19DF`/`FUNC_3532_1A05`, both with the
  identical `lcall`/`add sp,0xa`/`mov sp,bp`/`pop bp`/`retf 4` shape).
  This is a real, silent stack-corruption bug in the original firmware,
  not a disassembly artifact - the 3rd confirmed instance of this exact
  mechanism. Still unresolved: what the intended call target actually
  was, if any (plausibly there wasn't a distinct one - all 6 sites may
  share one coincidentally-decodable byte offset from a common
  source-level expansion, per the 2026-10-09 entry's original theory,
  now better supported). See `docs/decode-anomalies/landing-artifacts-
  and-jump-tables.md`'s matching 2026-10-11 follow-up. Left
  `FUNC_3633_9500`/`SUB_E951A` unnamed.
- **2026-10-11 (same session, later still): exhausted the static-
  reference search for `FUNC_3633_9611` itself (`ref_count: 0`) -
  still no confirmed caller, now via 4 independent checks instead of
  1.** Beyond the already-documented Ghidra cross-check (no caller
  found by either tool) and the already-documented absence of a
  `[0x1D10]`-table boot-init entry at the needed `dest_offset=0x96`
  (2026-10-10, above), grepped the full heuristic disassembly for the
  literal value `9611` in any form (only hit: the label's own
  definition - no `lcall`/`call`/`mov`-immediate anywhere references
  it), then byte-pattern-searched both raw ROM binaries for every
  plausible stored far-pointer encoding (256 segment-word candidates)
  and for a bare 2-byte near-offset encoding with no segment
  constraint at all - zero hits in either ROM for any of these. No
  static reference to `0xE9611` exists anywhere in either ROM except
  its own definition, under every encoding this project knows how to
  search for. **Hedged conclusion, not certain**: can't rule out a
  dynamically-computed call target (built at runtime from a register
  plus computed index rather than a literal/table constant), which no
  static search can find - the emulator would be needed to settle that
  definitively. Working assessment: most likely genuinely unreferenced
  in this ROM revision. No renames; `SUB_E962C` remains untouched. See
  `docs/decode-anomalies/landing-artifacts-and-jump-tables.md`'s
  matching 2026-10-11 (later still) follow-up.
- **2026-10-11 (same session, later still): the settings-byte-
  translator family (`FUNC_3532_6E7C`/`FUNC_3532_7045`/`FUNC_3532_
  7273`, formerly informally called `SUB_F6E7C`/`SUB_F7045`/
  `SUB_F7273`) shares `FUNC_3633_9611`'s exact "zero confirmed caller"
  signature, and the same exhaustive static-reference search comes
  back negative for all 3 of them too, plus their consumer `commit_
  pending_settings_and_refresh_display`.** Re-traced all 3 translators'
  actual byte/field coverage (bigger than previously documented:
  `FUNC_3532_6E7C` alone decomposes 4 raw bytes - `[0x4e7]`, `[0x4e8]`,
  `[0x4eb]`, `[0x4ec]` - into 7 staged fields, not the smaller 2-3-byte
  description from 2026-10-10). All 3 show `ref_count: 0` in `disasm/
  sysrom_3532_3633_heuristic.symbols.json`, same as `commit_pending_
  settings_and_refresh_display` itself. Ran the literal-value-grep +
  bare-2-byte-near-offset-scan technique (developed for `FUNC_3633_
  9611` above) against all 4 addresses (`0x6E7C`/`0x7045`/`0x7273`/
  `0x8D64`): zero genuine hits (one false positive for `0x7045` - 2
  bytes of the ASCII string "...Epson..." in printer-config text,
  confirmed by reading surrounding bytes and discarded). Also tested
  and ruled out a specific lead: whether `commit_pending_settings_and_
  refresh_display`'s own `[0x1D58]` far-pointer variable (base of its
  internal 10-byte-stride handler-record table) is boot-initialized by
  `init_far_pointer_table_sysrom`'s 80-entry table - it does have an
  entry (`dest_offset=0xD8` -> far pointer `F07F:0004`, physical
  `0xF07F4`) but that address is already-traced real code (`L_F07F4:
  mov word ptr [0x6d8], 1`, 2 confirmed jump-target references), the
  same "incidental overlap, not real init" pattern already flagged for
  other entries in this exact table (`[0x1D10]`'s `0x90`/`0x94`,
  `[0x1D5C]`/`[0x1D60]`'s `0xDC`/`0xE0`). Net: this whole settings-
  commit subsystem (1 dispatcher + 3 translators) now carries the
  identical "most likely genuinely unreferenced in this ROM revision,
  can't rule out a dynamically-computed call target" hedge as
  `FUNC_3633_9611` - a second, independent instance of the same open
  pattern, not a one-off. See `docs/acquisition-and-plotting/mode-
  dispatcher-and-formatting.md`'s 2026-10-11 follow-up. No renames.
- **2026-10-11 (same session, later still): found and resolved a
  second cross-64 KB-chip-boundary landing artifact, `SUB_F1581`
  (`0xF1584`), with 2 confirmed real callers.** Lands 1 byte into the
  operand bytes of the real `lcall SUB_E9778` at `0xF1580`; its
  misdecoded body (`push`/`jmp`) is garbage, but 2 genuine external
  `lcall SUB_F1581` sites (`0xE6443`, `0xE65C3`) confirm it's a real,
  called landing point, not dead bytes. One caller (`0xE65C3`) sits
  inside the already-documented `scheduler_tick_service`-style poll
  routine that captures `SWB2`/`SWB1` into `[0x758]`/`[0x759]` -
  checked this against `VARIABLES.md`'s existing entries for both and
  confirmed it adds nothing new (the capture site and its `[0x7B4]`
  XOR companion are already fully documented/confirmed there by live
  hardware test). See `docs/decode-anomalies/landing-artifacts-and-
  jump-tables.md`'s matching 2026-10-11 (later still) follow-up. No
  renames - `SUB_F1581` isn't a real function. **Correction, same day
  immediately after**: this wasn't a new identification -
  `docs/decode-anomalies/dual-entry-points.md` already found and
  traced this exact address (lands 2 bytes into `compute_and_draw_
  scale_marker`, reconverges cleanly onto real code, already declined
  to rename). The actual new contribution this pass was tracing the 2
  confirmed real callers (not previously documented), and "garbage,
  not real code" was an overstatement - corrected to "mechanically the
  preceding `lcall`'s own operand bytes, but genuinely executed and
  cleanly reconverging," matching the earlier doc's own framing. See
  the matching 2026-10-11 correction in the same doc.
- **2026-10-11 (later still): traced the last untraced multi-caller
  landing-artifact candidate, `SUB_F6DC9` (4 callers) - resolved the
  identity, but left a genuine open question needing the emulator.**
  Re-ran `find_landing_artifacts.py`; every candidate with 3+ callers
  was already resolved in the docs except this one. It lands 1 byte
  into the real `mov word ptr [bp-0x20], di` tail inside the
  already-confirmed `find_item_table_indices` (`0xF6CEA`), the usual
  clean-reconvergence shape on the not-taken path - but the misdecoded
  tail byte (`0xe0`) happens to be `LOOPNE`, a genuinely conditional
  branch on `cx`/`ZF`, unlike every other instance of this class.
  None of the 4 real callers, nor their enclosing functions back to
  each one's own entry, ever write `cx` before the call, so whether
  the branch is ever actually taken at runtime - and therefore whether
  this is cosmetic-only like the rest of the class, or a latent
  control-flow hazard - can't be settled from static reading alone.
  See `docs/decode-anomalies/landing-artifacts-and-jump-tables.md`'s
  matching 2026-10-11 (later still) follow-up. No renames.
  **Checked with the emulator, same day**: ran a full 80M-instruction
  boot-to-self-test-complete trace with watchpoints on `SUB_F6DC9`'s 4
  callers and itself - zero hits on all 5. None of the 4 callers
  execute during boot/self-test; they're plausibly only reached from a
  specific on-screen readout mode selected via the front panel, which
  the emulator's current build doesn't simulate (front-panel input is
  stubbed, not live-interactive - see `emulator/docs/design.md`).
  Whether `loopne` is ever taken remains unresolved, but the reason is
  now a known capability gap (needs scripted front-panel/menu
  navigation), not an unlooked-at question.
- **Whether the whole landing-artifact phenomenon is a genuine
  off-by-N linker/relocation defect specific to this ROM revision, or
  some other systematic cause, is unresolved** - would be worth
  checking against the `-13` comm ROM revision if it's ever
  disassembled (see the comm-ROM section above).
- `SUB_F6382` (now `draw_marker_box_and_update_position`)'s "capstone
  misreading opcode `0x0F`" theory is a reasonable explanation but not
  fully confirmed.
- **New 2026-09-22: found a real coverage-tooling off-by-one, not just
  another landing artifact.** `160-3633` physical `0xEFF99-0xEFFED`
  was listed in `UNKNOWN_DATA.md` as unidentified data; manually
  decoding all 85 bytes found it's genuine reachable code (two
  branches converge cleanly back into already-disassembled code at
  `0xFFEF`) that a boundary-computation bug orphaned - the block's own
  labeled end (`0xFFED`) cuts a `jmp` instruction one byte short,
  leaving a stray byte at `0xFFEE` that the heuristic disassembler
  then misdecoded into a bogus instruction, which is exactly what had
  looked like an unexplained landing artifact. Two new candidate
  RAM-pointer variables came out of the trace, `[0x1C94]`/`[0x1DDC]`
  (see `VARIABLES.md`), purpose unknown.
  **Checked systematically (2026-09-22, second pass)**: wrote
  `disasm/find_sandwiched_unknown_blocks.py` to test whether the
  coverage/heuristic tooling generally fails to follow jumps whose
  target wraps past a chip's own 64KB half (both jumps in this
  function's tail resolve into `160-3532`'s own file-offset space per
  the combined 128KB main-ROM convention). **Negative** - exactly one
  instance exists project-wide (this same block). But the broader
  phenomenon - real code the tooling has no entry point into - does
  recur via a *different* root cause: `160-3633` physical
  `0xE956E-0xE95A0` is two more real `retf`-terminated leaf
  subroutines (writes to the confirmed front-panel A/D control latch,
  reads `fp_intstat`), not data, but no literal far-pointer anywhere in
  any of the three ROMs references either entry address, so the real
  caller is a still-unresolved computed/indirect far call. Also
  clarified that several other "sandwiched" `160-3532` blocks
  (`0x1A86-0x213A`) are just uncovered pieces of the jump table already
  documented just below (finding 1), not new mysteries. See
  `docs/decode-anomalies/unknown-data-deep-dive-2026-09-15.md`'s
  "Follow-up, 2026-09-22" section, findings 7 and 10 plus the
  second-pass subsection.
  **RESOLVED 2026-10-09**: the 6 remaining "unconfirmed lead" blocks
  from the second-pass subsection were each traced the same rigorous
  way - none were false leads. `160-3633` `0xE77F8-0xE783C` is a
  brand-new standalone function (named `sdiv32_unsigned_divisor`, a
  sibling of `sdiv32` that treats the divisor as magnitude-only;
  caller still unresolved, same category as the `0xE956E` pair above).
  **Renamed again, same day, after further tracing**: `smod32` - the
  two other previously-unnamed proven functions it's structurally
  adjacent to (`SUB_E7866`/`SUB_E7895`) turned out to be its own
  unsigned-remainder core (`umod32`/`umod32_core`), which the original
  name didn't yet know about. See `FUNCTIONS.md` for the corrected
  mechanism (it's the real signed 32-bit modulo, not a division
  variant).
  `160-3633` `0xE9180-0xE91EF` is the shared, no-own-frame tail of an
  existing switch/case dispatcher (falls through from 4 known case
  labels, converges into already-known `L_E924B`). `160-3633`
  `0xE97A2-0xE97C9` was never a real gap - it's inside the
  already-fully-documented `extract_strided_channel_samples`.
  `160-3633` `0xE9404-0xE9471` bridges an already-reached-but-unnamed
  function straight into the already-named
  `merge_record_flags_if_changed` via plain fallthrough (why coverage
  stopped there specifically is still unexplained, left open). The 3
  `160-3532` blocks (`0xF08E7-0xF09BF`, `0xF0A73-0xF0AA9`,
  `0xF6E23-0xF6E4B`) are the same "gap sandwiched inside an
  already-reached function" shape, each with an already-known
  entry/label picking back up at exactly the byte after the gap ends.
  See `docs/decode-anomalies/unknown-data-deep-dive-2026-09-15.md`'s
  "2026-10-09 follow-up" section (finding 11) for the full per-block
  evidence.
  **New anomaly found investigating a separate pair, same day**:
  `SUB_EA2D6` and its only caller's other far-call target `SUB_EA13B`
  (`160-3633`) are NOT real code - both addresses land squarely inside
  already-documented string-table text (`"...SELECT to Start a
  PLOT\0Enable plotting of graticule..."` and `"...Points before
  trigger, PRE or POST..."` respectively), confirmed by direct raw-
  byte read against `disasm/strings_160-3633.json`'s recorded offsets.
  Their shared caller, `SUB_F173E`, is one of the `gen_disasm_x86.
  ENTRY_POINTS` "found via `init_far_pointer_table_sysrom`'s RAM-init
  table" batch, whose comment claims "every one decodes as coherent,
  non-garbage x86" - true for `SUB_F173E`'s own body by eyeball, but
  this finding shows that claim doesn't extend to what it calls.
  Left genuinely unresolved (3 live, unconfirmed explanations) rather
  than guessed at - see `docs/decode-anomalies/unknown-data-deep-dive-
  2026-09-15.md`'s new 2026-10-09 section for the full writeup. Not
  nameable; not a hardware-content guess, just an open disassembly-
  confidence question.
  **A second, harder instance of the same pattern, same day**:
  `SUB_EAC86`/`SUB_EADA0` (`160-3633`, ref_count 4/1) decode as a
  repeating binary-record pattern (not text) that exactly bridges two
  already-documented `UNKNOWN_DATA.md` gaps (block 16 ends the byte
  before `SUB_EAC86` starts; block 17 starts the byte after its
  decoded body ends in an `iret`) - strong evidence it's one
  continuous, still-unexplained data table, not code. But unlike
  `SUB_F173E`, `SUB_EAC86`'s 3 callers are inside `assemble_boot_
  splash_logo_chunks` (renamed 2026-10-09 from `SUB_F5898`, its own
  mechanism/entry-point oddity unrelated to this one), which is
  called from the already-confirmed, already-named `draw_boot_splash_
  and_option_icon` (the real boot-splash/logo routine) - a confirmed,
  important, boot-critical caller pointing straight at what looks like
  inert data, using the project's own established `0xFF7B` string-
  table calling convention. Tried booting the real firmware in
  `emulator/interactive.py` with a breakpoint at `0xEAC86` to settle it
  empirically - inconclusive: the run halts on already-documented,
  unrelated self-test failures (`ROMS`/`COMM_ROM`/`COMM_LB`/`MM_ACQ`/
  `CDT`, all pre-existing known emulator gaps per `emulator/README.md`)
  before ever reaching the breakpoint. Left unresolved (3 live
  explanations, including the possibility this is a genuine EPROM dump
  read error rather than a disassembly artifact) - see `docs/decode-
  anomalies/unknown-data-deep-dive-2026-09-15.md`'s "2026-10-09
  follow-up #2" section. Not nameable.
- **New systematic instance found 2026-09-15**: a previously-unknown
  real ~100-entry jump table in `160-3532` (file offset `0x1A33`-
  `0x213A`) has 2 of its 4 real callers (from `FUNC_3633_E9FA`, a
  6-case acquisition-mode dispatcher) land exactly 1 byte into the
  jump table's own `jmp` instruction, decoding as nonsense (and, for
  one, a `ret`/far-call mismatch) if read literally - while the other
  2 callers land on clean, sensible code. `FUNC_3633_E9FA` itself has
  zero confirmed callers, so whether this specific instance is ever
  reached on real hardware is unconfirmed. A separate, consistent
  4-byte stack-argument/cleanup shortfall across all 3 traced call
  sites in that function is also unexplained. See `docs/decode-
  anomalies/unknown-data-deep-dive-2026-09-15.md` finding 1.
- **Revised 2026-09-15**: `160-3532` file offset `0xBEE6`-`0xC263`
  (894 bytes of clean `(small count, 16-bit value)` records) is
  **not one table** - only the first 11 records (44 bytes) have their
  16-bit value landing near (not exactly on) `160-3532`'s existing,
  already-named self-test string cluster near the end of the chip
  (`"ROM/RAM/NMI :"`, `"2230/2220 Power up tests complete."`, etc. -
  see `STRINGS.md`). The remaining ~850 bytes correlate with nothing
  and are a separate, still-fully-unknown structure. Also surfaced a
  methodological gap while checking this: `160-3633` physical
  `0xEAC90`-`0xEADA0` heuristically decodes as including an SSE
  instruction (`minps`), impossible on an 8088 - a "covered but
  garbage" region `UNKNOWN_DATA.md` can't see because it only flags
  *uncovered* gaps. See `docs/decode-anomalies/unknown-data-deep-dive-
  2026-09-15.md` finding 4.

## The RAM far-pointer init table family

See `docs/acquisition-and-plotting/ram-far-pointer-table.md`.

- **No code anywhere in the corpus loads `ES`/`DS`=`0x209` via a
  literal immediate.** `init_far_pointer_table_sysrom`'s embedded
  RAM-init table led to 15 new proven entry points (mostly a
  plot-scale-computation preamble), but whether these functions
  actually get invoked in practice at runtime is still unproven -
  they're only reachable in the disassembly by *assuming* that
  segment gets loaded somehow.
- **2026-10-10: decoded the full 80-entry table by destination
  address, resolving `[0x1D1C]`'s boot init but not the `[0x1D10]+6`
  handler.** `[0x1D1C]` (a separate per-item table's base pointer,
  read 46x via `les` but never written anywhere else in either `.lst`
  - see `VARIABLES.md`) gets boot value `0038:0008` (physical
  `0x00388`, inside the IVT/low-RAM region) from `dest_offset=0x9C`,
  proven-reachable. Checked whether the same table also initializes
  the long-open `[0x1D10]+6` item-dispatch handler far pointer - it
  doesn't (no entry at the needed `dest_offset=0x96`); 2 entries
  (`0x90`/`0x94`) do land inside `[0x1D10]`'s own record 0 but look
  like incidental destination-range overlap, not a targeted init (one
  of them is the same mid-instruction-landing far pointer already
  flagged in `docs/acquisition-and-plotting/ram-far-pointer-table.md`).
  See `docs/acquisition-and-plotting/mode-dispatcher-and-formatting
  .md`'s 2026-10-10 follow-up for the full trace.
- **2026-10-10, same session: 2 more entries (`dest_offset=0xDC`/
  `0xE0`) land on `[0x1D5C]`/`[0x1D60]`** - two new far-pointer table
  bases found while tracing `[0x1D1C]`'s further usage (via the new
  `find_item_table_indices`, `0xF6CEA` - see `FUNCTIONS.md`/
  `VARIABLES.md`). Both resulting far pointers (`F076:000A`/
  `F070:000E`) decode as plausible x86 *code*, not the flat data
  arrays the consuming function's scan loop expects - almost
  certainly the same incidental-overlap situation as `[0x1D10]`'s
  `0x90`/`0x94` entries above, not a real init. These 2 pointers'
  actual runtime values remain unconfirmed.
- **2026-10-10, same session: explained `SUB_ECEDA`'s `hlt` opening -
  not a new function, and its 14-17 external callers are mechanically
  dead on arrival.** `0xECEDA` is just where the heuristic scanner's
  external-call-target label landed inside the already-documented
  `step_item_subvalue_back_guarded`/`_back`'s own shared dispatch tail
  (reached for real via a genuine internal `jle L_ECEDE` 4 bytes
  later, pushing command code `3`). The 14-17 external `lcall` sites
  (checked at 2 sample sites, both clean/coherent surrounding code, not
  scanner noise) all target `0xECEDA` itself, not `0xECEDE` - and the
  real ROM byte there is `hlt` followed by an unconditional `jmp`
  straight to the function's exit, skipping the dispatch body
  entirely. Per 8086 `HLT` semantics this would freeze the CPU until
  the next interrupt, then return without using the dispatch table or
  the just-pushed arguments - closes the "possible undocumented 4th
  `[0x1D10]`-dispatch sibling" question (there isn't one) but leaves
  open *why* real compiled code would deliberately target an address
  that halts (deliberate tick-wait idiom vs. a path patched out in this
  ROM revision - no evidence yet to choose). See `docs/decode-
  anomalies/landing-artifacts-and-jump-tables.md`'s matching 2026-10-10
  follow-up section for the full byte-level trace.

## Menu/UI rendering

See `TODO.md` and `HARDWARE.md`/`hardware/photos/INVENTORY.md` for the
photographed menu tree this maps to.

- **The actual menu-rendering code hasn't been traced.** `update_menu_
  position` was the leading candidate but is now **ruled out** (checked
  2026-09-14): all 9 of its callers, exhaustively enumerated, are
  self-test functions - it's scoped entirely to diagnostic test-
  position scanning, and its backing variable `[0x1B50]` is never read
  from outside its own body. The real `ADVANCED_FUNCTIONS`/
  `DIAGNOSTICS`/etc. menu-navigation state machine is a genuinely
  different, still entirely unfound mechanism - this rules out a
  previously-assumed lead rather than just re-confirming it.
- Specific self-test leaf functions not yet identified: `ACQ_ACCESS`/
  `PRC_READBACK`, `CAL_AIDS`'s `BOX`/`CAL_V_POS`, `EXERCISERS`'s
  `CONFIGURATION`/`IO`.
- **New lead found and rendered 2026-09-15, purpose still unresolved**:
  `160-3633` physical `0xEAE64`-`0xEB061` contains real vector-graphics
  data - a confirmed circle (40 points, radius ≈14, center (17,17),
  appearing twice as an exact cyclic rotation of the same point list),
  a second, perfectly circular smaller shape (radius exactly 3.16,
  same center), a straight line, and 3 medium shapes. Built
  `disasm/decode_vector_icons.py` to actually render these to SVG/PNG
  and look, rather than judging from radius numbers alone - the
  rendering **doesn't settle** whether this is a small UI icon set
  (dial/knob indicator) or a rough font distinct from the confirmed
  stroke font: the 3 medium shapes look letter-like at one scale but
  don't confirm as a clean, orientation-independent alphabet. Distinct
  either way from the stroke font (different encoding, different
  address range) and from any previously-ruled-out candidate region.
  What draws these or where they're used on screen is not found.
  **2026-10-09**: confirmed no sibling table exists anywhere else in
  any of the 3 ROMs (`scan_chip_for_shape_clusters`), and rendered all
  14 shapes (not just the 3 letter-like ones) at both Y-axis
  orientations - no further letterform candidates found, ambiguity
  unchanged. See `docs/decode-anomalies/unknown-data-deep-dive-
  2026-09-15.md` finding 3.

## The tick-driven task scheduler

See `docs/interrupts/task-scheduler.md`.

- **`create_task` is a self-yield primitive, not a spawn primitive -
  corrected 2026-09-15 after first getting this wrong in the same
  session.** It's a real `fork()`-style trampoline (rotates its own
  saved return-address words rather than taking an explicit entry-
  point argument, so the code physically following each call site
  becomes a resume point run later by the scheduler) - but the task
  index it operates on (`[0x1ACD]`) is read once and never changed, so
  it always re-arms the *currently-running* task's own slot, never
  allocates a new one. **"How many tasks exist" is therefore bounded
  by the already-documented 12-entry ready-state table, not by the 35
  `create_task` + 1 `create_task_b` call sites found this session** -
  those are 36 different potential *resume points* a task can yield
  through, not 36 task identities. How genuinely new task identities
  ever get established (if this system creates any beyond a fixed
  boot-time roster) is still unfound. **What each task slot actually
  spends its time doing is still mostly open** - only 3 call sites
  have been individually traced so far; most of their containing
  functions are still unnamed. Full call-site list in `docs/
  interrupts/task-scheduler.md`.
- The scheduler tick's timer source is presumed to be a periodic
  hardware timer, but which one isn't confirmed.

## Renaming backlog (lower priority - a completeness gap, not a mystery)

See `FUNCTIONS.md` and `TODO.md`.

- 20 proven-reachable routines remain unrenamed, each individually
  investigated with a documented reason it can't yet be safely named
  (see `changes/` for the running list) - 262/282 of the proven set is
  named.
- The heuristic-only layer (tens of thousands of additional functions
  across all 3 chips, plus ~400 in the comm ROM specifically) is a
  much lower-confidence, much larger tail. The realistic goal is
  "every proven-reachable routine named," not literally every
  heuristic placeholder - these are candidates for naming once traced
  from a known caller, not an active backlog to clear exhaustively.

## Data regions not yet marked as data

Identified string/constant tables inside already-*reached* code are
still disassembled as if they were instructions in the listing output
(harmless to the buildable `.asm` - the generator only trusts
exact-match bytes - but makes the `.lst` listing noisier than it needs
to be around those regions). Not yet systematically marked.

**Concrete example found 2026-09-15**: `SUB_E88CB` (`160-3633`,
physical `0xE88CB`, proven-reachable, called with exactly one word
argument) heuristically decodes as `inc word [bx+si]` followed
immediately by garbage that's actually the start of an adjacent,
otherwise-unidentified 843-byte data table (`UNKNOWN_DATA.md`'s
`3633` block 10, file offset `0x88DE`-`0x8C29` - a probable per-item
Y-position/width record table, `(0xFFFF, 0, 0, Y, 0x38)` repeating).
See `docs/decode-anomalies/unknown-data-deep-dive-2026-09-15.md`
finding 2 - a good real test case if this data-region-marking work
gets picked up.
