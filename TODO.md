# Active work

## Next up

- [ ] **User request**: decode the readout's stroke/vector font glyph
      table into SVG files + a catalog. Mechanism is fully understood
      (`draw_readout_char`'s pen/coarse/fine bit-packing, see
      `disasm/NOTES.md` "Attempted: locating the stroke-font glyph
      table"), but the table's physical address (`[0x1DB0]`'s value)
      hasn't been found - nothing writes it in proven or heuristic
      code. `[0x1DB8]`/`[0x1DBC]` (nearby) are ruled out as simple
      plot-scale byte caches, not font data - see `VARIABLES.md`
      "Acquisition/plot scaling". The `0xEA5E6`-`0xEB131` candidate
      region is ruled out (generic repetitive hook shapes, no
      letterforms). **Tool built 2026-09-13**: `disasm/decode_stroke_
      font.py` implements the confirmed bit-packing formula and
      renders any byte range to an SVG glyph catalog - reusable for any
      future candidate, and re-confirmed the `0xEA5E6` ruling with a
      proper tool (`disasm/stroke_font_candidates/0xEA5E6-0xEB131.svg`).
      **Realized the earlier search approach was structurally wrong**:
      `[0x1DB0]` points to a 128-entry far-pointer *array*, not glyph
      data directly - each entry points to that character's own
      (possibly non-contiguous) stroke bytes elsewhere. Added `scan_
      pointer_table()` to search for the pointer array's shape instead
      of a contiguous glyph run, but it found **zero candidates** in
      either main-ROM chip even with relaxed thresholds - genuinely
      inconclusive (scoring heuristic may be too strict, or the table
      lives in the comm ROM/an unmodeled region). Worth revisiting with
      different heuristics, or tracing `boot_init`'s data-driven init
      loops to find what sets `[0x1DB0]`/`[0x1CC4]` directly instead of
      searching blind.
- [ ] Find the comm ROM's actual **incoming**-data path. The ring
      buffer at `[0x448]`/`[0x44C]` (base `0xAF`, size `0x384`) turned
      out to be a TX queue (`serial_tx_buffer_put` producer,
      `service_comm_tx_queue` consumer), not an RX buffer as first
      assumed. No genuine incoming-byte ring buffer/interrupt handler
      has been identified yet - worth tracing if a comm-module board
      photo turns up a UART chip whose interrupt line can be followed
      back into the IVT.
- [ ] **REVIEW LATER**: `binary/aligned/*.bin` (NOP-padded, fully-
      readable reconstructions - see `binary/aligned/README.md`) were
      adopted as the reference binary for future checks. Revisit this
      choice once the rest of the analysis (self-test subroutine ID,
      menu tree, I/O port mapping) is further along, to confirm
      nothing was missed by not using the true original byte-for-byte.
- [ ] The full `ACQ_MODE_SETUP_TABLE` menu tree and the `DIAGNOSTICS`
      self-test menu are photographed and documented end-to-end in
      `HARDWARE.md`/`hardware/photos/INVENTORY.md`. Still open: trace
      the actual menu-rendering code that reads/draws these screens (an
      `update_menu_position`-driven state machine is the leading
      candidate) to tie each menu string to its backing code. Also
      identify the specific functions behind self-test leaves
      `ACQ_ACCESS`/`PRC_READBACK` and `CAL_AIDS`'s `BOX`/`CAL_V_POS` and
      `EXERCISERS`'s `CONFIGURATION`/`IO` - `TB_DIVIDER`/`CLK_DELAY`'s
      registers and the A/D converter identity are now confirmed (see
      `MEMORY_MAP.md`/`CONTEXT.md`).
- [ ] **New 2026-09-13**: `160-2998-13.bin` vs `-14.bin` (the comm ROM)
      genuinely differ in two ~16KB-aligned regions (unlike the main
      ROMs, which are byte-identical between revisions except a 4-byte
      header) - not yet diffed/analyzed in x86 terms at all. This
      project's comm-ROM disassembly has only ever covered `-14`. Now
      concretely relevant: the two physical test units used for live
      hardware sessions run different revisions (Scope 1 = `-13`,
      Scope 2 = `-14` - confirmed via the `/DIAGNOSTICS/EXERCISERS/
      CONFIGURATION` screen, see `HARDWARE.md`), so Scope 1's actual
      running comm-ROM code has never been disassembled. Diffing the
      two binaries (byte-for-byte, flag the changed regions) and
      disassembling `-13`'s changed regions would let today's comm-
      detection findings be checked against both revisions instead of
      just the one this project has analyzed.
- [ ] `write_readout_port_byte`/`init_readout_port_config`/`print_char`/
      `print_string_far` (all used exclusively for the self-test text
      banner) write to physical `0x406F0`-`0x406F3`, inside the comm-
      option's "Option UART/GPIB chips" 8-register bank. Desk research
      alone (TMS9914A datasheet ruling out its Data Out register at
      this offset; the RS-232 side having a real UART, "UART U1251",
      sharing this address block) had built a reasonably strong case
      for "genuine UART transmit register." **Tested directly on real
      hardware 2026-09-13 and it doesn't hold up**: the user has RS-232
      installed, confirmed the DIP-switch baud rate (600) by reading
      the switches directly, and ran an actual self-test from the
      `DIAGNOSTICS/TESTS` menu while a listener captured the port at
      the correct baud rate - **zero bytes came through**. If this
      cluster reached a real UART transmit register, the self-test
      should have produced something. Back to genuinely unresolved,
      now with real experimental data (see `disasm/NOTES.md`'s "Live
      hardware test" section for the full writeup and caveats on what
      else could still explain a null result). **Still short of a
      confident rename** either way
      (exact U1251 part number/register layout not confirmed) - see
      `disasm/NOTES.md`'s expanded "The readout/CRT display memory"
      section for the full trace. Next step if picked up again: check whether
      `init_readout_port_config`'s literal bytes (`0x29`/`0x23`/`0x06`)
      match documented UART/GPIB mode-register constants for a chip of
      this era.
- [ ] Reconcile `COMM/DATA/STOP_BITS`/`FLOW` (a runtime menu) against
      the rear-panel PARAMETERS DIP switch (`read_dip_switches_serial_
      config`) - both seem to configure overlapping RS-232 parameters;
      not yet clear which wins or whether the DIP switch only sets
      power-on defaults. Also: `COMM/DATA/ENCDG` offers ASCII/BINARY/
      HEX waveform-data coding but only the ASCII path
      (`print_signed_decimal_serial`/`print_param_list_response`) has
      been identified in code so far - find the binary/hex one(s).
- [ ] **Landing-artifact phenomenon** (real compiled `CALL`/`LCALL`/
      `JMP`/`LJMP` targets landing 1-4 bytes before where coherent code
      actually resumes): root cause confirmed statistically (`disasm/
      find_landing_artifacts.py` found 49 candidates; the majority land
      on an `ADD`-family opcode, far above chance - a common filler
      byte coincidentally decoding as a valid opcode). Only 2 of the 49
      were individually traced in depth (`write_hw_shift_register`,
      `SUB_EAC86`/`SUB_EAD08`), both turning into real findings rather
      than near-misses. The remaining ~47 weren't individually chased -
      worth a look only if one stands out (many independent call sites
      is the best tell). See `disasm/NOTES.md`'s "Systematic landing-
      artifact sweep" section.
- [ ] `init_far_pointer_table_sysrom`'s embedded RAM-init table (`ES=
      0x209`, physical `0x2090-0x21F0`) led to 15 new proven entry
      points, mostly a plot-scale-computation preamble immediately
      before `draw_pending_line_segment` (see `VARIABLES.md`
      "Acquisition/plot scaling"). Still open: **no code anywhere in
      the corpus loads `ES`/`DS`=`0x209` via a literal immediate** - how
      (or whether) these functions actually get invoked in practice
      isn't proven. See `disasm/NOTES.md` "Found: a whole family of
      never-reached functions via the RAM far-pointer init table".
- [ ] Identify and mark data regions (ASCII strings, tables) inside the
      already-reached code so the listing stops trying to disassemble
      them as instructions.
- [ ] Narrow down what peripheral the I/O ports actually seen in code
      (`0x83`, `0xC4`, `0xD1`, and a DX-indexed range) correspond to -
      see `MEMORY_MAP.md` "I/O ports actually seen in code". These are
      true 8086 port-space (`in`/`out`) accesses, a different address
      space from the memory-mapped `0x40000+` window the service manual
      documents, so that manual's Table 3-1 doesn't cover them
      directly. The AUX-connector pen-lift relay doesn't cleanly fit
      `write_hw_shift_register`'s multi-bit shift-and-strobe shape (Pen-
      Down is a single digital line, per the service manual) - the X/Y
      analog plot-output DACs are a better structural fit but the
      specific register/DAC feeding them wasn't named in the manual
      excerpts read so far.
- [ ] **In progress: renaming EVERY identifiable routine, not just
      opportunistically** (standing request: "keep going, don't stop
      until everything is renamed"). Working through the proven-only
      set (`sysrom_3532_3633.symbols.json`) ordered by reference count,
      highest first - 262/282 named. The remaining 20 have each been
      individually investigated and have documented reasons they can't
      be safely named (see `disasm/NOTES.md`'s dated session entries
      and `changes/` for the running list). The heuristic-only layer
      (tens of thousands more, across all 3 ROMs) is a much lower-
      confidence, much larger tail - the realistic goal is "every
      proven-reachable routine named," not literally every heuristic
      placeholder. Mechanically: add `{address: "name"}` to `gen_
      disasm_x86.FUNCTIONAL_NAMES`, add the matching `FUNCTIONS.md`
      entry, then regenerate everything (`gen_disasm_x86.py`, `gen_
      disasm_mainrom_heuristic.py`, `gen_source.py`, `gen_source_
      readable.py`) and re-verify byte-identical/length-matching before
      committing.
- [ ] Which `[0x1B83]` value (`0x1E` vs `0x14`) specifically means
      "comm option installed" isn't resolved - see `detect_comm_
      option_hw` in `FUNCTIONS.md` and `disasm/NOTES.md` "Found the
      actual source of [0x1B83]". The write-probe address is confirmed
      as the general-purpose "Time Base Mode Register U4119" (not a
      comm-specific latch), consistent with the two-branch ambiguity,
      but doesn't pin down the exact bit semantics.

      **BREAKTHROUGH 2026-09-14**: the scope finally responded over
      RS-232 for the first time this entire investigation, once a DIP-
      switch bit-order error (see `HARDWARE.md`) and a bad DB9-to-DB25
      adapter (not yet characterized) were both fixed. `ID?` got back
      `STATUS 98;READY;` - real data, but **every other command tried
      (`SET?`, `STAtus?`, `HELp?`, even deliberate garbage) produced
      the identical reply**, while a passive 10s listen produced
      nothing. This means the whole hardware-level investigation below
      (comm-detection, interrupt masking, the `[0x712]` dispatch table)
      was tracing real mechanisms, but **none of them were actually the
      blocker** - it was baud rate and cabling all along. The live
      result now points squarely at the **keyword-matching function**
      (still not found in the disassembly) as the real remaining gap:
      the low-level byte pipeline clearly works end-to-end, but nothing
      differentiates *which* command was sent. See `disasm/NOTES.md`'s
      "BREAKTHROUGH, 2026-09-14" section for the full transcript and
      analysis. **This is now the top priority lead** - everything
      below this point is retained as accurate background on how the
      comm hardware/firmware works, but is no longer the live blocker.

      An extended live-hardware session on two real units (2026-09-13) initially
      found `COMM_LOOPBACK` reporting `UNTESTED` and `ID?` getting zero
      bytes back, and first suspected a `[0x1B83]` comm-detection
      failure as a unifying cause - **but `COMM_LOOPBACK`'s `UNTESTED`
      result has since been fully traced instruction-by-instruction and
      does NOT involve `[0x1B83]` at all** (see `disasm/NOTES.md`'s
      "Correction: `COMM_LOOPBACK`'s `UNTESTED` result fully traced" -
      it's `selftest_comm_readback`'s register readback landing on a
      "pass but not `0xD0`" value combined with `[0x1B7A]` being set
      during the full self-test sequence). The `[0x1B83]` question
      itself thus remains exactly as open as before - the live session
      just didn't end up being new evidence for it after all. The
      separate `ID?`-gets-nothing puzzle is still unexplained and
      *could* still involve `[0x1B83]`/comm-detection, or the `P9107`
      jumper (`hardware/070-6090-00.pdf`'s F10/F12 install instructions
      - moving it is required when installing either comm option), or
      something else entirely in the comm ROM's own RS-232 command
      parser. **Partially traced 2026-09-13** (see `disasm/NOTES.md`'s
      "Traced the comm ROM's byte-dispatch/parser core"): found a real,
      `[0x629]`-gated (RS-232-vs-other) byte-classification/dispatch
      core (`process_gpib_command_byte` at `0x8526B`, plus a companion
      `0x97905` in the comm-ROM alias range) built around a 4-byte-
      per-entry table at far pointer `[0x712]` - confirms RS-232
      command handling is real, active code, not a stub. **`[0x712]`'s
      contents found and dumped** (the earlier "not found" was a wrong
      DS-segment assumption - it's under `DS=0x8F80`, not `0x41`; see
      `disasm/NOTES.md`'s "Follow-up: found [0x712]'s actual contents"):
      a 67-entry ASCII-indexed byte-classification table, grouping
      characters into 7 handler IDs. Best-fit reading: this is
      low-level lexical classification (digit vs. control vs. ordinary
      character), not keyword dispatch - individual letters `C`
      onward all collapse to one handler, so actual keyword matching
      (`ID`/`SET`/`CURVE`/etc. from `STRINGS.md`) must happen in a
      **different, still-unfound function**. **Still not found**: (1)
      that downstream keyword-matching function; (2) the genuine
      UART-receive entry point that first puts an incoming byte into
      `[6]`/`[0x580]` (still nobody's found this - not an interrupt
      handler, not a polling loop, despite several look-arounds this
      session). Either one, found, would be the most direct route left
      to explaining the `ID?` silence. **Note**: both of the user's
      physical units are confirmed Option 12 (RS-232) only, no Option
      10 (GPIB) hardware available to cross-test against - so any
      further tracing should stay focused on the `[0x629]`-clear
      (RS-232) branches specifically, not the GPIB-specific code paths
      this same shared ROM also contains. **Promising live-hardware
      shortcut found 2026-09-13**: the service manual describes a
      `COM-OPTION/DEBUG` exerciser ("used in debugging the
      communications option... outputs a test message and displays any
      incoming messages (data) [on] the crt") - see `HARDWARE.md`'s
      menu tree. If findable on a real menu, this could show incoming
      RS-232 bytes directly on the scope's own screen, without needing
      the PC/adapter/cable at all - worth trying before more code
      tracing.
      The interrupt-mask-latch lead was **followed up and mostly
      resolved 2026-09-13** (see `disasm/NOTES.md`'s "Traced the
      interrupt mask latch's real outputs"): found the indirection
      (`[0x6E2]` far pointer, comm ROM), and mapped 3 of its 4 outputs
      - `0D`=RX-ready/`DR`-interrupt-mask, `1D`=TX-ready/`TBRE`-mask
      (both via `set_comm_queue_busy`/`update_comm_tx_ready_flag`),
      `3D`=a diagnostic strobe (not an interrupt mask despite the
      shared latch). **The "never unmasked" theory doesn't hold up**:
      `set_comm_queue_busy`'s disengage path does restore `0D`=1
      (unmask), and is called during normal comm-channel init - the
      masking mechanism looks correctly implemented. Output `2D`
      (`0x406FA`) still has no reference found anywhere (possibly
      `RLSD`/`DCD` generation, set once rather than dynamically).
      **Remaining open questions, narrower now**: (1) whether the
      init sequence containing the unmask call is *actually reached*
      on real hardware (ties back to `[0x1B83]`/`[0x1BF9]` comm
      detection - `COMM_LOOPBACK`'s `UNTESTED` result is *some*
      evidence it is, but not a direct confirmation of this specific
      call); (2) whether the CPU's own interrupt plumbing (IVT entry,
      `IF` flag) is correctly set up post-boot - not checked; (3) the
      keyword-matching function from the `[0x712]` work above - still
      not found, and still needed even if the interrupt fires
      correctly.
      **Question (2) is now resolved, and it's a big deal**: the
      service manual's prose (not a table) states outright *"the
      Maskable Interrupt (INTR) is vectored to 03FC"* - confirming
      `INT 255` (this project's existing `INT255_HANDLER_LATE`) **is**
      the real hardware interrupt line, not a software-only vector as
      previously assumed. Traced the chain all the way through:
      `INT255_HANDLER_LATE` → `run_continuous_selftest_tick` → the
      `[0x740]` hook → (when comm installed) physical `0x96F54`, a
      real `[0x629]`-aware per-tick comm status poller (tentatively
      named `poll_comm_status_tick`) - reads the DIP-switch-config
      byte, logs comm-channel-status snapshots to a circular buffer,
      and calls out to `0x839D1`/`0x8006:0x9C` on specific status
      bits. See `disasm/NOTES.md`'s "MAJOR CORRECTION: INT 255 is
      NOT..." for the full writeup. **This is now the best candidate
      for "how does incoming RS-232 activity actually get serviced"**
      - a tick-driven poller reached via a confirmed hardware
      interrupt, not the never-confirmed-reachable `FUNC_2998_39F5`
      loop this session spent time on earlier. **Traced `0x839D1`/
      `0x8006:0x9C` immediately after - both are already-documented
      housekeeping** (icon redraw, TX-ready flag update), not the
      keyword-matching function. So `poll_comm_status_tick` is a real,
      confirmed-reachable status-*sync* routine, but not where bytes
      actually get received/parsed - that path is still unfound.
      Genuinely next steps now: (a) find what, if anything, actually
      calls `process_gpib_command_byte`'s chain (is `FUNC_2998_39F5`
      reachable some other way, or is there a second, different entry
      point into that dispatcher this project hasn't found yet); (b)
      the still-missing keyword-matching function from item (3) above.

      **Live testing 2026-09-14, now with `PROGRAMMING_MANUAL.md`'s
      transcribed status/event tables to decode the reply exactly**:
      `STATUS 98` = "Execution Error, RQS On, Not Busy" (Table 7-34) -
      command recognized but not executable, not "command not
      understood". Ran the decisive test: `REMote ON`, then drained
      `EVEnt?` up to 8x specifically to read the real 3-digit event
      codes - **the queue never drains**, every `EVEnt?` call (which
      should always succeed per the manual) comes back wrapped in the
      same non-informative `STATUS 98;READY;`/`STATUS 97;` template
      regardless of what's sent. This is conclusive that command
      content isn't being differentiated at all - not just "the
      keyword-matching function is unreached," but that a separate,
      generic status-reporting path is intercepting every message
      before real command dispatch would occur, and never clears.
      **Every settings-based theory now eliminated (2026-09-14)**: a
      genuine cold power-cycle produced the identical result (not a
      stuck state); the DIP switches (photographed live) confirm
      parity genuinely disabled, ruling out an intermittent-corruption
      theory that would explain the 97-vs-98 inconsistency; the `COMM`
      menu (`FLOW`/`STOP_BITS`/`DATA SOURCE`/`CHANNEL`/`ENCDG`, all
      photographed live) are all at normal, expected values and were
      never the problem. **Top remaining hypothesis**: something
      specific to comm-ROM revision `-13` (Scope 1's revision, never
      disassembled by this project - Scope 2 runs `-14`, the revision
      actually traced this session). **Single most informative next
      step**: run this identical `ID?`/`EVEnt?` test sequence against
      Scope 2. Matching behavior implicates something common to both
      revisions still hiding in the already-read `-14` code; different
      behavior would justify disassembling `-13` specifically. See
      `disasm/NOTES.md`'s "BREAKTHROUGH, 2026-09-14" section for the
      full transcript and reasoning trail.
      **Cross-check run**: Scope 2 (`-14`) tested the same way -
      `SET?` also returned `STATUS 98;READY;`, the same dominant
      pattern as Scope 1. **This weakens the `-13`-specific-defect
      theory** - getting the same non-answer on both ROM revisions
      points toward something common to both (not yet found in the
      already-read `-14` disassembly), or a gap in this project's own
      test methodology vs. how a period-correct terminal/controller
      would actually talk to the instrument. Also raised live: a
      *second* oscilloscope may be probing the RX/TX lines on Scope 1
      for monitoring purposes, which could itself be loading/
      corrupting the signal - worth removing before trusting any
      further "corruption" observations on Scope 1 specifically.
- [ ] Which physical front-panel control each of the 3 `update_menu_
      position`-range-scan self-tests (`selftest_front_panel_switch_a`/
      `_b`, `selftest_comm_option_switch`) corresponds to isn't
      confirmed. **`[0x758]`/`SWB2`'s exact bit map is now confirmed
      2026-09-13**, not just address-matched - see `VARIABLES.md`. Its
      bits (`MEM 1`/`2`/`3`, `MENU ADV`, `SELECT C1/C2`, `MENU`, `1K/
      4K`, `POS/SEL`) are all *menu/memory* controls, not the specific
      analog VOLTS/DIV-style switches these 3 self-tests are believed
      to exercise - so this confirmed byte doesn't directly answer
      *this* item, but the technique (compare a bit mask's code
      structure to the manual's named bits, not just match addresses)
      is proven and worth repeating for `[0x4E7]`/`[0x4E8]` and
      `[0x759]`/`SWB1` once a literal-address read site for either is
      found. `[0x4E7]`/`[0x4E8]`'s `&0x80` "accelerate" pattern still
      isn't tied to a specific named `SWB1`/`SWB2` bit - remains open.
- [ ] Found the comm option board's DIP-switch reader (`read_dip_
      switches_serial_config`/`read_dip_switches_gpib_config`, see
      `HARDWARE.md`) - still open: map each of the 10 physical switch
      positions to which specific decoded bit(s) it controls. `[0x629]`
      (GPIB/RS-232 mode) still isn't confirmed as switch-sourced. The
      service manual confirms `0x406BC` is the right register ("Option
      Parameters Latch (in)") but doesn't give a bit-by-bit switch map
      in the sections read so far.

## Ongoing documentation goal

As routines are understood, build up (not just individual function
names but the bigger picture):
- user-facing flows and menu structure (what the front panel/CRT UI
  actually lets someone do, and how the code implements it)
- configuration options and where they're stored
- the full memory map (RAM regions, I/O port assignments, peripheral
  registers) — started in `MEMORY_MAP.md`; keep it updated as more
  segments/ports are identified
- a "jump map" of major control-flow relationships as activity
  diagrams — started in `JUMP_MAP.md`, high-level first, drilling into
  more detail as more routines are understood
- a function list — started in `FUNCTIONS.md`, one entry per
  identified routine (address, current label, what it does, evidence)
  across all three ROMs
- a variable list — started in `VARIABLES.md`, same idea for memory
  locations
- a strings/constants catalog — started in `STRINGS.md` (curated) with
  a machine-readable companion in `disasm/strings_<rom>.json`
  (regenerate via `disasm/gen_strings.py`)
- clean pseudo-C reconstructions of well-understood routines — started
  in `PSEUDOCODE.md`; add one whenever a `FUNCTIONS.md` entry reaches
  "Confirmed" confidence
- peripherals: the A/D converter, front-panel controls, GPIB/RS-232
  hardware, and how the firmware talks to each

When a diagram would help (state machines, memory maps, menu trees),
embed PlantUML directly in the relevant markdown file rather than
building a separate image asset.

## Done

See `changes/` for a dated log of completed work per session. Keep
this file trimmed to active/pending items only — when something gets
resolved, move its detail into the current day's `changes/YYYY-MM-DD.md`
entry instead of leaving a long `[x]`-marked writeup here.
