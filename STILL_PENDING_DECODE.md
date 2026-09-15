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

- **The genuine UART-receive entry point is still unfound.** Where an
  incoming byte first lands (expected somewhere touching `[6]`/
  `[0x580]`) has never been traced in the disassembly - live RS-232
  communication works fine now (see `hardware/manuals/2230_
  programming/PRACTICAL_GUIDE.md`), so this is purely a documentation
  gap, not a functional blocker.
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
- **Comm ROM revision `-13` has never been disassembled.** `-13` and
  `-14` differ in two real ~16KB-aligned regions (unlike the main
  ROMs, which only differ in a 4-byte header) - only `-14` has ever
  been read as x86. One of the two physical test units runs `-13`.
  No longer motivated by a suspected defect (both revisions behave
  identically over live RS-232), but still a real documentation gap.
- **`compute_parity_mode_code` (comm ROM) calls the main ROM's
  `scale_and_plot_point_default`** using the exact save/restore
  argument shape normally used for a DS-segment-switch helper, but the
  actual target is confirmed to be the plot-scaling function. Whether
  this is deliberate reuse of a shared math primitive or something
  else is unresolved. See `docs/comm-rom/rs232-flow-control-and-open-
  puzzle.md`.
- **`[0x629]` (GPIB/RS-232 mode select)** still isn't confirmed as
  switch-sourced.
- **The 10-position PARAMETERS DIP switch is only partially mapped.**
  Only the baud-rate nibble (switches 1-4) has a confirmed bit-to-
  meaning map (see `hardware/manuals/2230_programming/
  PRACTICAL_GUIDE.md`) - `read_dip_switches_serial_config`/
  `read_dip_switches_gpib_config` decode the other switch positions
  too, but which physical switch controls which decoded bit beyond
  baud rate isn't confirmed. Service manual confirms `0x406BC` is the
  right register but doesn't give a bit-by-bit switch map in the
  sections read so far.
- **`COMM/DATA/STOP_BITS`/`FLOW` (runtime menu) vs. the rear-panel DIP
  switch** - both configure overlapping RS-232 parameters; not clear
  which wins or whether the DIP switch only sets power-on defaults.
- Binary/hex `CURVe?` waveform transfer is now fully confirmed **as a
  wire protocol** (see the practical guide), but still isn't tied to
  specific disassembled routines beyond the known ASCII path
  (`print_signed_decimal_serial`/`print_param_list_response`).

## Stroke-font glyph table (the CRT readout's character shapes)

See `docs/display/vector-display-and-stroke-font.md` and `TODO.md`'s
top item.

- **`[0x1DB0]`'s value (the glyph pointer-array's actual address) has
  never been found.** The bit-packing mechanism is fully understood
  and has a working tool (`disasm/decode_stroke_font.py`), but nothing
  in proven or heuristic code writes `[0x1DB0]` itself - confirmed
  2026-09-14 across all 3 chips' proven+heuristic listings (every hit
  is a read, zero writes). `boot_init`'s own data-driven init loop was
  fully traced 2026-09-14 and confirmed to be an unrelated RAM
  march-test routine, closing off that specific lead.
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
  out. Most promising next step: solve for the true HPGL-to-native
  transform using more captured samples, or find the ROM's own
  plot-scale-for-readout-text constant directly.

## Front-panel switches

See `docs/self-test/front-panel-switches.md` and `VARIABLES.md`.

- **`[0x758]`/`SWB2`'s bit map is confirmed** (`MEM 1`/`2`/`3`, `MENU
  ADV`, `SELECT C1/C2`, `MENU`, `1K/4K`, `POS/SEL`) - but these are all
  menu/memory controls, not the analog VOLTS/DIV-style switches the 3
  `update_menu_position`-range-scan self-tests
  (`selftest_front_panel_switch_a`/`_b`, `selftest_comm_option_
  switch`) are believed to exercise. **Which physical control each of
  those 3 self-tests actually corresponds to is still unconfirmed.**
- **`[0x759]`/`SWB1`'s bit map is not yet confirmed** - the same
  code-structure-vs-named-bits technique that worked for `SWB2` hasn't
  been repeated here because no literal-address read site has been
  found yet.
- **`[0x4E7]`/`[0x4E8]`'s `&0x80` "accelerate" pattern** isn't tied to
  a specific named `SWB1`/`SWB2` bit.

## Display / CRT readout hardware

See `docs/display/readout-memory.md` and `TODO.md`.

- **The `write_readout_port_byte`/UART theory doesn't hold up, and
  there's no confident alternative yet.** Physical `0x406F0`-
  `0x406F3` (written exclusively by the self-test text banner path)
  sits inside the comm option's UART/GPIB register bank, and desk
  research built a real case for "genuine UART transmit register" -
  but a live self-test with an RS-232 listener attached at the correct
  baud rate produced **zero bytes**. Back to genuinely unresolved.
  Next concrete step if picked up: check whether
  `init_readout_port_config`'s literal bytes (`0x29`/`0x23`/`0x06`)
  match documented UART/GPIB mode-register constants for a chip of
  this era.
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
- `SUB_F6382`'s "capstone misreading opcode `0x0F`" theory is a
  reasonable explanation but not fully confirmed.

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
