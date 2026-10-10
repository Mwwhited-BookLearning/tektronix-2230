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

- **The genuine UART-receive entry point is still unfound, though the
  surrounding scheduling mechanism is now resolved (2026-10-09).**
  Where an incoming byte first lands (expected somewhere touching
  `[6]`/`[0x580]`/`[0x590]`) has never been traced in the disassembly -
  live RS-232 communication works fine now (see `hardware/manuals/
  2230_programming/PRACTICAL_GUIDE.md`), so this is purely a
  documentation gap, not a functional blocker. **Now confirmed**: the
  loop that reads `[6]` (`comm_call_main_rom`) is reached via a
  dedicated per-task scheduler default-handler table entry
  (`comm_task4_default_handler`) - genuine tick-driven cooperative
  polling, not a byte-level hardware interrupt; see
  `docs/interrupts/task-scheduler.md`'s 2026-10-09 section for the full
  trace and table dump. **Both of the previously-flagged leads are now
  ruled out (2026-10-09)**: all 5 main-ROM writes to `[0x590]`
  (including `0xF63CE`) belong to one recurring buffer/record-
  management code shape (`es:[di+2]`/`es:[di+4]` struct fields, gated
  on `[0x1b83]==0x14`, setting overflow flags `[0x560]`/`[0x55c]`) that
  has nothing to do with comm/serial - confirmed unrelated, not a lead.
  `FUNC_2998_56F8`/`FUNC_2998_5712` were confirmed to have **zero**
  callers anywhere: no far-pointer table entry in the whole comm ROM
  binary points at either address (checked by raw byte scan, not just
  listing grep), and `disasm/gen_disasm_2998.py` shows they're seeded
  purely from a blind `55 8B EC` prologue-signature scan, not reached
  via any proven call graph - demoted to "probably unreached/dead
  heuristic matches," not a real lead. See
  `docs/interrupts/task-scheduler.md`'s 2026-10-09 follow-up section
  for the new candidate found while ruling these out: a
  previously-undocumented 4-entry far-pointer sub-table at
  `[0x1AD0]`-`[0x1ADE]` (written by `init_selftest_register_group`,
  pointing at the confirmed UART register bank `0x406F0` plus
  `0x4067C`/`0x406BC`/`0x406F8`) that is never read anywhere in the
  currently-disassembled code - likely consumed by a generic
  self-test/exerciser table-walk routine (matching the documented
  `/DIAGNOSTICS/EXERCISERS/IO/INPUT_PORTS` screen in `MEMORY_MAP.md`)
  that hasn't been found yet. Also confirmed: the comm ROM's proven
  listing has no genuine `in`/`out` port instruction at all (the one
  apparent match at `0x84092` is a disassembler desync artifact in a
  non-code byte region, not real code) - consistent with
  `MEMORY_MAP.md`'s existing finding that the UART is memory-mapped,
  not port-mapped, so the real receive read (if it exists in code
  reached so far) must be a plain `mov`/`cmp` on `0x406F0`, not an
  `in` instruction.
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
- Binary/hex `CURVe?` waveform transfer is now fully confirmed **as a
  wire protocol** (see the practical guide), but still isn't tied to
  specific disassembled routines beyond the known ASCII path
  (`print_signed_decimal_serial`/`print_param_list_response`).

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
  is embedded in a giant, currently-unnamed function (`FUNC_3633_DF56`,
  `0xEDF56`). **Neither has a confirmed caller** - both are
  `ref_count: 0` in the heuristic symbol table, so this doesn't prove
  reachability, but it's real code worth someone naming and tracing
  forward from. Don't confuse this with the nearby, already-documented
  `dispatch_item_handler_if_enabled`/`[0x1D10]` item-table mechanism
  (a landing artifact of the same neighborhood, unrelated to the
  stroke font) - full detail and the exact disassembly in
  `docs/display/vector-display-and-stroke-font.md`'s "Found a second,
  independent reader of `[0x1DB0]`" section. **Correction, checked
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
  found yet.
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
  What draws these or where they're used on screen is not found. See
  `docs/decode-anomalies/unknown-data-deep-dive-2026-09-15.md`
  finding 3.

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
