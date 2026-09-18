# Active work

**Environment note**: NASM isn't on `PATH` in this environment, but
is installed at `C:\Users\mwwhi\AppData\Local\bin\NASM\nasm.exe`
(confirmed 2026-09-14, version 3.02). Pass it explicitly as an
argument where a `gen_source*.py` script accepts one (e.g. `python
gen_source_2998.py "C:\Users\mwwhi\AppData\Local\bin\NASM\nasm.exe"`)
instead of assuming bare `nasm` resolves.

## Next up

- [ ] **User request 2026-09-17: hunt down every hardware jumper** on
      the main boards - they may explain debugging/configuration
      behavior (comm detection, reset) the firmware/emulator can't
      account for from register state alone. Found so far, both from
      OCR'd service-manual sections:
      - **`P9104`** (Storage/Clock-Generator board area, near `U9104`):
        a manual-reset jumper - moving it to the RESET position forces
        a reset of the Microprocessor and Display Controller (`docs/
        theory-of-operation.md`'s Microprocessor/Clock Generator
        section). Straightforward, already understood.
      - **`P9107`** (Storage circuit board, near the ROM sockets):
        moved "one pin over toward the center" specifically when
        installing the F10 (GPIB)/F12 (RS-232) comm option (`docs/
        f10-f12-option-installation.md`, step 22, Figure 3) - **not
        yet understood electrically**. Already flagged in `HARDWARE.md`
        as "a strong candidate for why live firmware comm-detection
        might fail on a unit where this wasn't done correctly," but
        that lead was never resolved (the real RS-232 blocker turned
        out to be baud-rate reliability, not this - see `docs/comm-rom/
        rs232-breakthrough.md`). Given today's session found a real,
        previously-unexplained comm self-test failure mode
        (`selftest_comm_readback` requiring a hardware loopback this
        project just modeled with `io_stubs.DiagCommLatchLoopback` -
        see `changes/2026-09-17.md`), this jumper is worth a fresh
        look: does it gate memory decoding for the 2230's Option
        Memory circuit board, or something closer to the comm-detect/
        self-test logic? Next step: check Section 9 (Diagrams) once
        OCR'd for the actual schematic, or ask the user to check which
        position their own physical unit(s) currently have it in.
      - Also noted: `docs/maintenance.md`'s Power Distribution section
        mentions generic "service jumper connections" (Diagrams 10,
        11, 21) used to isolate power-supply loading during
        troubleshooting - lower priority, not memory/config-related.
      - **Checked 2026-09-18**: `docs/options.md` (Service Manual
        Section 7, now OCR'd in full, including its own "OPTION 10/12
        THEORY OF OPERATION") **does not mention `P9107` at all** -
        ruled out as the source, still unresolved. That same document
        did resolve a different, related open question though: **Table
        7-36 ("RS-232-C Status Buffer Functions")** gives `comm_stat`'s
        full authoritative bit map (see `MEMORY_MAP.md`'s updated
        Interrupt Mask Latch section) - and in doing so, exposed a
        genuine **conflict** with this session's own disassembly-
        derived model: the table says bit `0x40` is `DIAG`/Interrupt-
        Mask-Latch-output-`3D` and bit `0x80` is the unrelated `/DCD2`
        (modem carrier-detect), but `selftest_comm_readback`'s own
        instructions require bit `0x80` (not `0x40`) to be the one that
        toggles with `3D` for its check to reach the required `0xD0`
        result - confirmed by testing bit `0x40` instead, which
        produces `0x40` and genuinely fails. `io_stubs.
        DiagCommLatchLoopback`'s docstring documents this conflict in
        full rather than silently picking a side. A real schematic
        trace of `3D`'s actual wiring (Section 9, Diagrams) would
        settle it - a survey of specific pages from that section is in
        progress as of this writing (see below); check there next.
      - **Found, not yet electrically understood, 2026-09-18**: the
        34-page Diagrams survey (`docs/diagrams-index.md`) found
        `P9107` on **page 65** (Diagram 14, "Microprocessor and Store
        Panel Controls," board A10), printed with **ON/OFF labels**, right
        next to the `U9104` reset/clock RC network (`R9107`/`C9107`) -
        consistent with `docs/theory-of-operation.md`'s reset-timing
        description, but still doesn't say what ON vs OFF *does*
        beyond "move it when installing a comm option." Also found a
        **second, previously-unknown jumper `P9105`** right next to it,
        printed with **TEST/NORM labels** - very plausibly a genuine
        firmware self-test/diagnostic-mode strap, not yet connected to
        anything in this project's own findings. **`P9104` is NOT a
        jumper** - it's the clock/oscillator IC itself; every reference
        to "jumper P9104" (including this project's own, sourced from
        the Theory of Operation text) most likely means the nearby
        `P9107`/`P9105` jumpers, not a literal `P9104` jumper - worth
        re-reading `docs/theory-of-operation.md`'s exact wording again
        with this in mind. No jumpers found anywhere else across the 34
        pages, including the comm-option boards (22-25) - only fixed
        connectors there. **Next step**: get page 65 read again
        specifically for the ON/OFF and TEST/NORM position labels'
        actual meaning (what circuit each position connects to), since
        the survey only confirmed the jumpers exist and their labels,
        not their electrical effect.
      - **Same survey, on the bit-6-vs-bit-7 conflict above**: page 114
        (Diagram 23, RS-232 Option Board) confirms `DIAG` and `RLSD`
        (the real schematic name for what `/DCD2` refers to) are two
        distinct, separately-labeled signal nets - supporting that
        they're genuinely different signals, not the same one under two
        names. Does **not** resolve which one actually toggles during
        `selftest_comm_readback`'s sequence - that needs real wire-
        level netlist tracing, out of scope for an index survey. Also
        surfaced a possible chip-designator slip worth double-checking:
        the schematic prints "U1235" for what `docs/options.md`'s own
        prose calls "U1236" (Interrupt Mask Latch) in 6+ separate
        places with matching pin/behavior detail - the text is strong
        enough evidence that `U1236` stays the confirmed designator in
        `MEMORY_MAP.md`, but flag if a sharper look at the original
        page 114 scan ever contradicts that.
- [ ] **Reminder (user, 2026-09-16)**: review service manual **page
      415** - the acquisition memory logic (RAM chips + decode logic).
      User's own preview while noting this down: 2x 2048x8 static RAM,
      **`U3418`/`U3419`**, that look fed by the DAC. These 2 chips are
      already independently confirmed in this project as the physical
      backing for `MEMORY_MAP.md`'s `0x48000-0x4BFFF` "Acquisition
      Memory - 4 images of Acquisition RAM U3418/U3419" range - a
      component-level schematic trace here (matching the RS-232 option
      board reviews already integrated, see `changes/2026-09-16.md`)
      would do for the acquisition/DAC path what that review did for
      the comm option: name the actual decode/support chips around the
      already-known address range, not just the RAM itself.

      **Unconfirmed hypothesis, added while previewing the page**:
      `U3418`/`U3419`'s own address lines are `AA1`-`AA11` (11 lines,
      matching each chip's own 2048-byte capacity exactly -
      `2^11=2048`). User's guess, explicitly not yet verified against
      the actual logic: **`AA0`** (one bit lower than the chips' own
      address bus) selects between `CH1`/`CH2` - i.e. the 2 RAM chips
      might be a per-channel pair rather than an interleaved/depth-
      doubling pair, with `AA0` as the bank-select bit sitting outside
      each chip's own address pins. No existing finding in this
      project to cross-check against (checked `MEMORY_MAP.md`/
      `VARIABLES.md` for any prior channel-select-bit note for the
      acquisition RAM specifically - none found, this is new
      territory). Confirm or refute directly from the page 415 logic
      when reviewed.
- [ ] **`emulator/` next steps** (built 2026-09-16, see `emulator/
      README.md`/`emulator/docs/design.md`): resolved the stroke-font
      glyph-table hunt for this project's real hardware (full story in
      `changes/2026-09-16.md`). **Updated same day**: fed this
      session's hardware findings into `io_stubs.py` (8 new register
      stubs, 6 backed by real captured values) - the full power-up
      self-test sequence now completes for the first time. Remaining
      open threads: (1) the still-unsolved HPGL native-coordinate
      transform puzzle (`docs/display/vector-display-and-stroke-
      font.md`'s "Follow-up, 2026-09-14/15" sections) could potentially
      be chased dynamically the same way; (2) ~~trace what actually
      sets `[0x1AEE]`~~ **done 2026-09-18** - it wasn't
      `selftest_display_irq_idle` at all (that function was proven to
      pass); the real TIMEOUT source is the separate `selftest_display_
      irq_active` (`0xE3F99`) busy-polling `[0x1AEE]` after reading the
      Display Chip Interrupt Reset register (`0x41000`) - see
      `FUNCTIONS.md`'s corrected entries for both functions and
      `io_stubs.DisplayChipIrqStub`, which now couples that read into
      `[0x1AF2]`/`[0x1AEE]` so the self-test passes; (3) build a
      write-then-readback coupling stub for the `ACQ_AB` address-line
      walking test against the Acquisition Memory Address Buffer (see
      `MEMORY_MAP.md`'s `0x4377E`/`0x4377F` entry) the way
      `CommPresenceProbe` already couples a different register pair -
      **still open**, this is the next self-test failure blocking a
      fully-clean power-up sequence.
- [ ] **User request 2026-09-15**: deep dive into `UNKNOWN_DATA.md`'s
      exported blocks (see `disasm/find_unknown_data.py`). Found 5
      things worth following up, ranked by confidence in `docs/decode-
      anomalies/unknown-data-deep-dive-2026-09-15.md`: (1) a real
      ~100-entry jump table in `160-3532` with 2 real callers landing
      1 byte inside it (landing artifact) from a channel-mode
      dispatcher that itself has zero confirmed callers; (2) a probable
      per-item Y-position/width table in `160-3633` next to a
      2-byte function whose body the heuristic decoder over-runs into
      the table; (3) **the encoding is fully decoded and actually
      rendered** - built `disasm/decode_vector_icons.py` (pen-bit
      convention, same as `draw_readout_char`'s stroke font, wider
      coordinate) and rendered the `160-3633` region to SVG/PNG.
      **Rendering it changed the conclusion**: a 40-point circle
      (radius≈14, appears twice as a cyclic point-list rotation) and a
      small circle sharing its center are real, but 3 medium shapes
      that looked letter-like at one render scale don't hold up as a
      clean, orientation-independent alphabet either - genuinely
      unresolved whether this is a small icon set or a rough font, see
      the doc for the full before/after reasoning; (4) **revised** -
      only the first 44 bytes of a 3532 record table are near (not
      exactly on) an already-known self-test string cluster, the other
      ~850 bytes are unrelated; also surfaced a methodological gap (a
      `160-3633` region heuristically "decodes" as including an
      impossible SSE instruction - `UNKNOWN_DATA.md` can't see
      "covered but garbage" regions, only uncovered gaps); (5) a
      grouped incrementing-ID table in the comm ROM, tentatively but
      not confirmedly related to the manual's Table 7-34 status
      categories. None reached the confidence bar for a
      `FUNCTIONS.md` rename yet - see `STILL_PENDING_DECODE.md`'s
      "Decode anomalies"/"Menu/UI rendering"/"Comm ROM" sections for
      each as a tracked open item. Next step if picked up: render more
      of the surrounding ROM with `decode_vector_icons.py` to check for
      a fuller alphabet or more icon variants nearby, and compare
      flipped/un-flipped Y-axis renders for every shape (only shapes
      5-7 were checked both ways so far).
- [ ] **User request**: decode the readout's stroke/vector font glyph
      table into SVG files + a catalog. Mechanism is fully understood
      (`draw_readout_char`'s pen/coarse/fine bit-packing, see
      `docs/display/vector-display-and-stroke-font.md`), but the
      table's physical address (`[0x1DB0]`'s value) hasn't been found -
      nothing writes it in proven or heuristic code, confirmed again
      2026-09-14 by grepping every disassembly listing across all 3
      chips for any touch of offset `0x1db0` - every hit is a `les`
      read inside `draw_readout_char`, zero writes anywhere. `[0x1DB8]`/
      `[0x1DBC]` (nearby) are ruled out as simple plot-scale byte
      caches, not font data - see `VARIABLES.md` "Acquisition/plot
      scaling". The `0xEA5E6`-`0xEB131` candidate region is ruled out
      (generic repetitive hook shapes, no letterforms). `boot_init`'s
      own data-driven init loop (physical `0xE0155`/`0xE017D`/
      `0xE01AF`) was traced fully 2026-09-14 - it's a RAM-sizing/march-
      test routine (bit patterns like `0xFD`/`0xF0F0`/`0xFFFF` written
      and read back), confirmed unrelated to font data, not just
      "not fully traced" as before.

      **Tool built 2026-09-13**: `disasm/decode_stroke_font.py`
      implements the confirmed bit-packing formula and renders any byte
      range to an SVG glyph catalog - reusable for any future
      candidate, and re-confirmed the `0xEA5E6` ruling with a proper
      tool. **Realized the earlier search approach was structurally
      wrong**: `[0x1DB0]` points to a 128-entry far-pointer *array*, not
      glyph data directly - each entry points to that character's own
      (possibly non-contiguous) stroke bytes elsewhere. Added `scan_
      pointer_table()` to search for the pointer array's shape instead
      of a contiguous glyph run.

      **2026-09-14: found and fixed a real bug in that scanner**
      (`_stroke_run_score` penalized bit `0x08` as an "invalid coarse
      bit," but it's actually part of the valid *fine* nibble - every
      byte `0x01`-`0xFF` is syntactically legal under this encoding,
      so there's no invalid-bit-pattern signal to filter on at all).
      Reran across all three chips including the comm ROM (never tried
      as the table's *location* before) - still zero candidates at
      strict thresholds; loosened thresholds surface candidates but
      resolving them shows segments jumping randomly across all three
      chips per adjacent character - confirmed false positives, not a
      real table. **Blind byte-pattern scanning may be fundamentally
      unable to find this table** (there's no statistically-invalid
      byte pattern to search for). **Tried a new angle instead**: used
      a real live HPGL capture of an isolated "2V" readout label
      (`scratchpad/plot1.hpgl`, gitignored - re-capture with
      `disasm/plot_hpgl_to_svg.py` if needed) to derive the expected
      native coordinate sequence from actual hardware output and search
      for a content match. Hit a concrete, reproducible obstacle:
      both captured characters need 9 distinct native Y levels
      (`0,1,3,4,5,6,7,8`) to fit their observed HPGL Y-coordinates
      under a step-4 assumption, one more than the 3-bit coarse field's
      8-value capacity - confirmed on 2 different glyphs, not a fluke,
      and not resolved by trying alternate baselines or a signed/two's-
      complement reinterpretation (every variant tried still overflows
      by one slot, since the *span* forces it regardless of where the
      baseline sits). See `docs/display/vector-display-and-stroke-
      font.md`'s "Follow-up, 2026-09-14" section for the full numbers -
      likely next step is solving for the true HPGL-to-native transform
      using more captured samples, or finding the ROM's own plot-scale-
      for-readout-text constant directly instead of reverse-solving
      from output.
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
      the actual menu-rendering code that reads/draws these screens.
      **Checked 2026-09-14: `update_menu_position` is NOT this
      candidate after all** - enumerated all 9 of its callers
      exhaustively (proven+heuristic) and every single one is a
      self-test function (`selftest_front_panel_switch_a`/`_b`,
      `run_adc_selftest_range`, `selftest_tb_divider`, `step_tb_
      divider_test`, `step_front_panel_switch_b_test`, `wait_stable_
      measurement`, `run_indexed_adc_selftest`, `selftest_comm_
      readback`) - it's scoped entirely to diagnostic test-position
      scanning, not general menu navigation. Its own backing variable
      `[0x1B50]` is likewise only ever touched from inside its own
      body (9 read/write sites, all clustered at its own address) -
      nothing outside it re-reads the position. **The real
      `ADVANCED_FUNCTIONS`/`DIAGNOSTICS`/etc. menu-navigation state
      machine is a genuinely different, still entirely unfound
      mechanism** - this was a real, useful correction (it redirects
      away from a dead end), not just re-confirmation. Also identify
      the specific functions behind self-test leaves `ACQ_ACCESS`/
      `PRC_READBACK` and `CAL_AIDS`'s `BOX`/`CAL_V_POS` and
      `EXERCISERS`'s `CONFIGURATION`/`IO` - `TB_DIVIDER`/`CLK_DELAY`'s
      registers and the A/D converter identity are now confirmed (see
      `MEMORY_MAP.md`/`CONTEXT.md`).

      **Checked 2026-09-14, genuine dead end for now**: tried the same
      string-table cross-reference technique that found `self_test_
      dispatcher`'s 18 siblings (find the fixed-segment string load,
      compute the physical address, identify the caller) on `ACQ_
      ACCESS`/`PRC_READBACK`/`BOX`/`CAL_V_POS`/`CONFIGURATION`/`IO`
      specifically. Found all 6 strings (in `160-3633`, offsets
      `0x9fcb`-`0xa08x`) but they're just plain concatenated NUL-
      terminated text (`RAM\0SYSTEM\0ACQ_ACCESS\0PRC_READBACK\0FP...`)
      with **zero individual code references anywhere in either main
      ROM chip** - checked every immediate-load encoding for each
      string's exact offset, none found. Unlike the 18 already-found
      self-test siblings (each individually hardcoded into a call
      site), these look like they're walked by a generic menu-string-
      table mechanism instead - i.e. this is blocked on the same
      still-unfound general menu-navigation code above, not
      independently solvable by this technique. Worth retrying once
      (if ever) that general mechanism is found.
- [x] **RESOLVED 2026-09-15**: diffed `160-2998-13.bin` vs `-14.bin`
      byte-for-byte - only 133 bytes differ total, in exactly 3 runs (a
      6-byte header, and two runs starting at the 16KB-page boundaries
      `0x4000`/`0xC000`, ~63-67 bytes each - "two ~16KB-aligned
      regions" meant page-*aligned start*, not 16KB-*long*). At
      `0x84000`: `-13` has a small, previously undocumented config-check
      routine (tests `[0x0623]`/`[0x0002]`/`[0x05FA]`/`[0x05F7]`); `-14`
      has a truncated, non-terminated leftover copy of the ROM's own
      copyright string in the same slot (the real, complete copy lives
      at file offset `0x000a` in both revisions) - most likely a build
      artifact, not functional. At `0xC000`: corrects an existing claim
      that both comm-ROM pages 2 and 3 carry the boot-stub far-jump -
      true only for `-14`; `-13` has no page-3 boot stub at all (that
      offset falls mid-function in `-13`, patched over in `-14`). Full
      writeup, hex dumps, and hand-disassembly in
      `docs/comm-rom/revision-13-vs-14-diff.md`. **Not done**: actually
      disassembling `-13`'s changed regions into the project's tooling
      (the two `-13`-only routines found are hand-decoded only) - low
      priority since `-13` isn't this project's baseline chip.
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
      now with real experimental data (see `docs/comm-rom/rs232-early-
      investigation.md`'s "Live hardware test" section for the full writeup and caveats on what
      else could still explain a null result). **Still short of a
      confident rename** either way
      (exact U1251 part number/register layout not confirmed) - see
      `docs/display/readout-memory.md`'s expanded "The readout/CRT display memory"
      section for the full trace. Next step if picked up again: check whether
      `init_readout_port_config`'s literal bytes (`0x29`/`0x23`/`0x06`)
      match documented UART/GPIB mode-register constants for a chip of
      this era.
- [ ] Reconcile `COMM/DATA/STOP_BITS`/`FLOW` (a runtime menu) against
      the rear-panel PARAMETERS DIP switch (`read_dip_switches_serial_
      config`) - both seem to configure overlapping RS-232 parameters;
      not yet clear which wins or whether the DIP switch only sets
      power-on defaults. `COMM/DATA/ENCDG`'s ASCII/BINARY/HEX
      waveform-data formats and the binary checksum algorithm are now
      all confirmed live byte-exact against the manual (see
      `docs/comm-rom/rs232-live-session-2026-09-14.md`'s "Live session, 2026-09-14 (continued)") -
      still not tied to specific disassembled routines beyond the
      known ASCII path (`print_signed_decimal_serial`/`print_param_
      list_response`), just no longer a protocol/format unknown.
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
      is the best tell). See `docs/decode-anomalies/landing-artifacts-and-jump-tables.md`'s "Systematic landing-
      artifact sweep" section.
- [ ] `init_far_pointer_table_sysrom`'s embedded RAM-init table (`ES=
      0x209`, physical `0x2090-0x21F0`) led to 15 new proven entry
      points, mostly a plot-scale-computation preamble immediately
      before `draw_pending_line_segment` (see `VARIABLES.md`
      "Acquisition/plot scaling"). Still open: **no code anywhere in
      the corpus loads `ES`/`DS`=`0x209` via a literal immediate** - how
      (or whether) these functions actually get invoked in practice
      isn't proven. See `docs/acquisition-and-plotting/ram-far-pointer-table.md` "Found: a whole family of
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
      be safely named (see `docs/` (start at `docs/README.md`) for the
      dated session entries and `changes/` for the running list). The heuristic-only layer
      (tens of thousands more, across all 3 ROMs) is a much lower-
      confidence, much larger tail - the realistic goal is "every
      proven-reachable routine named," not literally every heuristic
      placeholder. Mechanically: add `{address: "name"}` to `gen_
      disasm_x86.FUNCTIONAL_NAMES`, add the matching `FUNCTIONS.md`
      entry, then regenerate everything (`gen_disasm_x86.py`, `gen_
      disasm_mainrom_heuristic.py`, `gen_source.py`, `gen_source_
      readable.py`) and re-verify byte-identical/length-matching before
      committing.
- [x] **RESOLVED 2026-09-14 - the live RS-232 command silence was a
      baud-rate reliability problem, not firmware.** A day-long live
      investigation (interrupt mask latch tracing, the `[0x712]`
      dispatch table, `poll_comm_status_tick`, exhaustive settings
      elimination, a two-ROM-revision cross-check) chased what turned
      out to be a red herring: at 9600 baud, both scopes reliably
      returned clean-looking but content-blind `STATUS 98;READY;`
      responses to every command. Dropping to **1200 baud** made every
      command work correctly and immediately (`ID?`, `EVEnt?`, `SET?`,
      `HELp?` all returned exactly the documented format). All of that
      day's disassembly findings remain accurate documentation of how
      the comm ROM actually works (interrupt masking, the tick-driven
      status poller, the byte-classification table, etc.) - they just
      weren't the blocker. Full transcript and reasoning trail in
      `docs/comm-rom/rs232-breakthrough.md`'s "RESOLVED, 2026-09-14: it was baud rate
      reliability all along, not firmware".

      **Still genuinely open, lower priority now**:
      - Which `[0x1B83]` value (`0x1E` vs `0x14`) means "comm option
        installed" - see `detect_comm_option_hw` in `FUNCTIONS.md` and
        `docs/comm-rom/option-detection.md` "Found the actual source of [0x1B83]". Write-
        probe address confirmed as general-purpose "Time Base Mode
        Register U4119", not comm-specific, but exact bit semantics
        still unresolved.
      - The genuine UART-receive entry point (where an incoming byte
        first lands in `[6]`/`[0x580]`) is still unfound in the
        disassembly. **Progress 2026-09-14**: found the likely backing
        *data* for the keyword matcher itself - a real command-keyword
        table in the comm ROM (file offsets `0x8A59`-`0x8F1D`) whose
        entries match the live `HELp?` list byte-for-byte, plus a
        6-byte-per-entry index/dispatch table immediately before it
        that resolves numeric command IDs to far pointers landing
        exactly on each keyword's table entry - see `docs/comm-rom/rs232-live-session-2026-09-14.md`'s
        "Found the real command-keyword table" and `docs/comm-rom/command-keyword-table.md` for the full extracted contents. **Still not found**:
        the code that actually walks this index table / assigns the
        numeric command ID from incoming bytes - a grep for the far
        pointers' literal segment value found zero hits in the
        already-disassembled code, so it's either computed dynamically
        or lives in an unreached region.
      - `STAtus?` returned `STATUS 128;` at 1200 baud once, and never
        again - **investigated further 2026-09-14, not reproduced**:
        10 consecutive live `STAtus?` calls at 4800 baud all returned a
        clean `STATUS 0;`, and Table 7-34's bit layout hardcodes bit 7
        to `0` in every documented category, so no ROM code path can
        produce it under the documented status scheme. Best remaining
        explanation is a one-off transient serial glitch, not a
        firmware defect - see `docs/comm-rom/rs232-live-session-2026-09-14.md`'s "Live session,
        2026-09-14 (continued)" for the full writeup. Also newly found
        in the same session: some query responses substitute an inline
        `STATUS <code>;` for a single field's value (e.g. `DELAY
        VALUE:STATUS 98;`) rather than failing the whole response -
        worth remembering when parsing any response programmatically.
      - Whether `FUNC_2998_39F5` (the originally-suspected polling
        loop, never confirmed reachable) or `poll_comm_status_tick`
        (confirmed reachable via the real hardware interrupt, but only
        does status housekeeping) relates to the real receive path is
        still unresolved - moot for practical use now that RS-232
        communication works, but open for anyone continuing the
        disassembly.
      - Diffing/disassembling comm ROM revision `-13` (Scope 1, vs. the
        `-14` this project has actually read) is no longer motivated by
        a suspected defect, but remains a legitimate documentation gap.
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
