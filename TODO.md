# Active work

**Environment note**: NASM isn't on `PATH` in this environment, but
is installed at `C:\Users\mwwhi\AppData\Local\bin\NASM\nasm.exe`
(confirmed 2026-09-14, version 3.02). Pass it explicitly as an
argument where a `gen_source*.py` script accepts one (e.g. `python
gen_source_2998.py "C:\Users\mwwhi\AppData\Local\bin\NASM\nasm.exe"`)
instead of assuming bare `nasm` resolves.

**Environment note**: Ghidra is installed at `C:\repo\tools\ghidra`
(confirmed 2026-10-09 - moved here from a prior `Downloads\
ghidra_12.1.3_PUBLIC_20260817\...` location some earlier session's
notes still referenced, which no longer exists). `decompile/
apply_names.py`/`export_decompiled_c.py` call `pyghidra.start()` with
no explicit path, so it must be given via the `GHIDRA_INSTALL_DIR`
env var: `GHIDRA_INSTALL_DIR="C:\repo\tools\ghidra" python
apply_names.py`. See `docs/architecture/ghidra-project.md`'s
"Environment" note for the same.

## Next up

- [ ] **User report 2026-09-18: SELECT C1/C2 (momentary front-panel
      button) held at power-on produces zero diagnostic/UART output in
      the emulator, when real hardware and the service manual
      (`docs/maintenance.md`) both say holding it should invoke
      *extended* diagnostics with *more* output (an RS-232 ASCII error
      dump). Root cause of the *emulator's* zero-output behavior fully
      found and confirmed 2026-09-18 - see `JUMP_MAP.md`'s "boot-time
      diagnostics gating" section and `FUNCTIONS.md`'s `print_string_
      far` entry: that function itself checks `[0x1B48]==0` (set
      exactly when SELECT C1/C2 is held alone) and returns immediately,
      printing nothing - confirmed via an identical self-test call
      sequence in both held/unheld runs, differing only in whether each
      print call does anything. This is real, disassembled ROM logic,
      not an emulator stub gap or a bug in the emulator's button model.
      Along the way, also found and fixed a genuine, separate emulator
      bug (missing `hlt`-instruction detection, which had briefly
      misdiagnosed this as a crash into unmapped memory before the real
      mechanism was found) - see `emulator/docs/design.md`'s "Found and
      fixed a real bug: no hlt detection" section.
      **Hypothesis (1) checked and disproved**: searched every
      reference to `0x6F0`-`0x6F7` across the main ROM's disassembly -
      `write_readout_port_byte`'s target *is* the exact same physical
      address (`0x406F0`) already confirmed as the real UART's own data
      register (`io_stubs.InteractiveUartMock`'s mapping), not a
      separate CRT-only mirror with a different real channel elsewhere.
      So the suppressed channel really is the genuine UART - this
      doesn't resolve `MEMORY_MAP.md`'s "Puzzle" section in favor of
      "mirror" after all.
      **Also tried**: simulating "the operator presses a MENU key" to
      continue past the final idle halt, hoping that would reveal a
      real extended-diagnostics continuation. Built a genuine interrupt
      -injection wake for a halted CPU (reusing `timer.fire_interrupt`)
      to test this properly - it does wake the CPU correctly, but the
      instruction immediately after this specific halt (`halt_cpu`,
      `0xF1611`) crashes with an unmapped write every time, regardless
      of how execution reaches it. This confirms `halt_cpu` is a
      genuine one-way trap in this compiled ROM (matching its other
      caller `assert_and_halt`'s already-documented "hlt never returns"
      shape), not a resumable wait state - "PRESS MENU KEYS TO
      CONTINUE" most likely means a genuine hardware reset of the
      microprocessor (compare the `P9104` reset-jumper finding below),
      not a resumption of the halted code. See `emulator/docs/design.md`'s
      "Found and fixed a real bug: no hlt detection" section for the
      full writeup of both dead ends.
      **Confirmed on real hardware, 2026-09-18**: user directly tested
      this - holding SELECT C1/C2 through power-on produces genuine
      9600-baud diagnostic text over the RS-232 port on **both `-13`
      and `-14` ROM revisions**; otherwise the boot process looks
      visually identical either way. This is not a documentation
      quirk or a ROM-revision difference - it's real, reproducible
      behavior on the actual instrument that this emulator currently
      gets backwards (shows output unheld, none held).
      **Searched exhaustively for the mechanism and found nothing**:
      every reference to `[0x758]` (the front-panel byte) and
      `[0x1B48]` across the main ROM's proven listing, its heuristic
      listing, and the comm ROM's listing - all already accounted for
      by what's already documented (the boot-time flag setup and
      `print_string_far`'s gate). `[0x1B7A]` *is* written (by
      `selftest_sequence_enter`/`selftest_sequence_exit`,
      `0xF7BA5`/`0xF7D99` - heuristic-reachability only, correcting an
      earlier same-day claim that it was never written at all) but has
      no connection to `[0x758]` or SELECT C1/C2 at all, so it isn't
      the missing mechanism either.
      **Correction, same day**: this entry previously claimed the comm
      ROM (`160-2998`) "has no heuristic disassembly pass built for it
      at all" - false. `disasm/gen_disasm_2998.py` already built one in
      an earlier session (same push-bp-prologue technique as the main
      ROM's, in fact the main ROM's script's docstring says it borrowed
      the idea from this one) - it's already what produces the
      committed `160-2998-14.lst`/`.symbols.json` (400 heuristic entries
      layered on 24 proven ones, 20446 instructions, 91.69% byte
      coverage - see `disasm/compute_coverage.py` - on par with the
      main ROM's own 90-91%). The `[0x758]`/`[0x1B48]` search above
      already covered this combined listing, not just a proven-only
      subset. Checked further: the handful of remaining unreached comm-
      ROM byte ranges (documented in `UNKNOWN_DATA.md`'s "Chip 2998"
      section, largest two are a contiguous ~2KB span at phys
      `0x08824C-0x088A57`) don't contain the literal `58 07` (`0x758`
      LE) or `48 1B` (`0x1B48` LE) immediate-operand byte pattern
      either - not conclusive (a different addressing form or an
      entirely different comm-ROM-local variable could still reference
      the same hardware register) but one more lead closed off.
      **Read that remaining span by hand, same day**: the ~2KB block
      (`UNKNOWN_DATA.md` "Chip 2998" blocks 6/7, phys `0x08824C-
      0x088A57`) is a second far-pointer/dispatch table, structurally
      distinct from the confirmed command-keyword dispatch table it
      sits immediately adjacent to (ends exactly one byte before that
      table starts) - see "6." in `docs/decode-anomalies/unknown-data-
      deep-dive-2026-09-15.md` for the full evidence (record-fitting
      stats, 41% of plausible entries landing exactly on already-known
      function starts). Working hypothesis: a command-ID-to-handler-
      address table complementary to the confirmed command-ID-to-
      display-string one. Every target it resolves to is comm-ROM/
      main-ROM *code*, none of it front-panel-switch-related - this
      lead is now closed, not just deferred.
      **One remaining possibility**: a pure hardware-level effect - the
      switch wired directly to something like the UART's chip-select or
      baud-rate-clock enable, which would never appear in any
      disassembly at all no matter how much more code gets covered; the
      Diagrams survey (`docs/diagrams-index.md`) or a fresh schematic
      trace of the front-panel-to-comm-board wiring would be the only
      way to check that. The user's own interrupt hypothesis was
      narrowed but not confirmed: the held/unheld self-test call
      sequences are identical, so if a missing interrupt is the real
      cause, its install site must be in code outside the path already
      traced - most likely wired to the same hardware effect above,
      since every code-based lead is now exhausted.
      **Reviewed 2026-09-22, no code issue found**: re-checked this
      whole thread (prompted by the user recalling "issues" from the
      end of the previous session) against `emulator/captured-notes/
      SelectC1C2-Set/README.md` - the user's own note from just before
      this investigation started (`aca0921`, 2026-09-17 23:59),
      floating a "the registers might be inverted" hypothesis based on
      the checked/unchecked runs (`4305063`=checked=0 diag lines,
      `5391063`=unchecked=16 lines). That hypothesis is independently
      disproven, not just assumed away: `VARIABLES.md`'s `[0x758]`
      entry documents a 2026-09-13 live-hardware test where physically
      pressing SELECT C1/C2 flipped `dig=` octet 3 `0x08`->`0x88`
      (bit7 going high on press) - exactly matching `io_stubs.
      InteractiveFrontPanel`'s active-high model for this bit
      (`("SWB2", 7, False)`). So the emulator's button polarity is
      correct, not inverted; the checked-run's zero output is the real,
      already-documented `[0x1B48]`-gate behavior, not a register-
      mapping bug. Also re-ran `emu.py --show-diag-text` for a full
      30M-instruction boot (unheld baseline) and confirmed it still
      completes cleanly (17 diag lines, 0 unmapped faults) - the
      `hlt`-detection fix and the reverted wake-experiment from
      `41de556` left no leftover code (diff was comment-only) and
      nothing is broken. This closes the loop on the README note; the
      only open item remains the hardware-wiring possibility above.
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
      `[0x1AF2]`/`[0x1AEE]` so the self-test passes; (3) ~~build a
      write-then-readback coupling stub for the `ACQ_AB` address-line
      walking test~~ **done 2026-10-09** - `io_stubs.AcqAbAddrWalkStub`,
      see `changes/2026-10-09.md` for the full derivation (including a
      real bug found and fixed verifying it against a live run). The
      self-test sequence now progresses past `ACQ_AB` entirely, into 2
      new, previously-unreached failures - **next self-test failures
      blocking a fully-clean power-up sequence**: `HS_ACQ` (`acq_mem
      cntr 0800 <> 00AB`, then several `fill @` byte mismatches) and
      `TBD hs/2` (same shape, different counter/fill values).
      **Fully traced 2026-10-09** (see `docs/self-test/hardware-
      probes.md` and `changes/2026-10-09.md`): both bottom out in
      `run_adc_selftest`/`verify_pattern_with_report`, and the far
      ptr `[0x322]` they poll turns out to be `0x4377E` - the *exact
      same* Acquisition Memory Address Buffer register (U3427) the
      already-stubbed `ACQ_AB` test reads, not a separate ADC. So the
      register identity is no longer the blocker; what's still
      missing is (1) ~~the real acquisition hardware's write-then-
      readback behavior for *this* read pattern (plain 12-bit value +
      busy flag, not an address walk like `ACQ_AB`'s)~~ **done
      2026-10-09** - `io_stubs.AdcSelftestReadbackStub` hooks
      `run_adc_selftest`'s exact read instruction (`0xE137A`) and
      writes back `[bp+0xc]+[bp+0x16]`, read live off the caller's own
      stack frame - since the comparison code is identical for every
      caller, this one stub fixes both `HS_ACQ` and `TBD hs/2` without
      needing `TBD hs/2`'s own `[0x1DCC]` table decoded. Verified live
      (both `"acq_mem cntr"` mismatches gone; A/B'd via `git stash`
      that the run's later `halt_cpu` panic stop is unchanged either
      way, so not a regression). See `docs/self-test/hardware-
      probes.md` and `changes/2026-10-09.md`. Still open: (2) what
      fills the incrementing-ramp pattern `verify_pattern_with_report`
      expects at far ptr `[0x31E]` (physical `0x48000`, confirmed
      genuine Acquisition RAM U3418/U3419 - `configure_measurement_hw`
      never writes it, so something else must) - both `HS_ACQ`/`TBD
      hs/2`'s `"fill @"` mismatches persist, and the run now continues
      one self-test further into a third, still-unnamed `"TBD ps/2"`
      failure with the same `"fill @"` shape. Next step: figure out
      (2) well enough to extend or add an `io_stubs.py` coupling stub,
      following the same derivation approach as `AcqAbAddrWalkStub`.
- [ ] **User request 2026-09-15**: deep dive into `UNKNOWN_DATA.md`'s
      exported blocks (see `disasm/find_unknown_data.py`). Full history
      and findings in `docs/decode-anomalies/unknown-data-deep-dive-
      2026-09-15.md` and `STILL_PENDING_DECODE.md`'s "Decode anomalies"
      section; resolved-item detail moved to `changes/YYYY-MM-DD.md`
      per this project's TODO-hygiene convention rather than kept here.
      **The "sandwiched unknown code" sub-thread (findings 7-11) is now
      fully closed** - every block `find_unknown_data.py` ever flagged
      has either a confirmed explanation or an explicitly-tracked open
      question. **Still open**: the vector-icon-vs-font sub-thread
      (finding 3) - render more of the surrounding `160-3633` ROM with
      `disasm/decode_vector_icons.py` to check for a fuller alphabet or
      more icon variants nearby, and compare flipped/un-flipped Y-axis
      renders for every shape (only shapes 5-7 checked both ways so
      far).
- [ ] **User request**: decode the readout's stroke/vector font glyph
      table into SVG files + a catalog. **2026-09-16: closed for this
      project's real hardware** - `draw_readout_char` opens with
      `cmp byte [0x1b83], 0x14 / je <return>`, and `[0x1B83]` is
      `detect_comm_option_hw`'s comm-presence flag; both of this
      project's physical test units are RS-232/Option-12-equipped
      (flag=`0x14`), so this function returns immediately without
      drawing anything - the stroke-font path is dead code for the
      hardware actually being reverse-engineered here.
      `print_string_far`/`write_readout_port_byte` (the fixed
      `0x40000+0x6F0` port bank) is the confirmed real diagnostic-text
      channel, independent of comm-detection status. See
      `docs/display/vector-display-and-stroke-font.md`'s "2026-09-16"
      section for the full trace (via `io_stubs.py`'s
      `CommPresenceProbe` stub) and `STILL_PENDING_DECODE.md`. Left
      here only because the underlying mystery is still genuinely
      unresolved as a low-priority curiosity, not because it blocks
      anything: the `[0x1DB0]` table's runtime value (`E9A3:0000`,
      confirmed both statically and via live trace) doesn't behave
      like a working glyph table when walked, and no downstream
      renderer that could turn stroke-font style data into real HPGL/
      CRT output has been found.

      **2026-09-23: picked back up, no new progress, two loose ends
      closed.** Retried the "find a confirmed caller of `FUNC_3633_
      E60C`/`FUNC_3633_DF56`" next step - grepped every listing (all 3
      chips, proven+heuristic), zero call sites anywhere, confirming
      `ref_count: 0` isn't a scanner gap. A blind far-pointer byte scan
      across all ROMs (every possible segment) is not a sound technique
      and found nothing credible - recorded as a dead end, don't retry
      the same way. Also hit and ruled out a false lead: `SUB_ECEDA`
      (`0xECEDA`, ref=17) and `FUNC_3633_CF19` (`0xECF19`), in the same
      address neighborhood, turned out to be more callers of the
      *unrelated* `[0x1D10]` item-handler dispatch table (already
      documented under `dispatch_item_handler_if_enabled`), not
      stroke-font material - easy to re-conflate, so flagging again.
      Full detail: `docs/display/vector-display-and-stroke-font.md`'s
      "Follow-up, 2026-09-23" section, `STILL_PENDING_DECODE.md`.
      **Paused here, decision pending**: cheap static-analysis avenues
      are exhausted; the two remaining options are (a) a properly-
      targeted far-pointer search using segment bases already confirmed
      elsewhere in the ROM (not a blind sweep), or (b) a longer live-
      emulator trace exploring menus/modes beyond the boot self-test
      banner. Given it's a low-priority curiosity (not a practical
      blocker, per the 2026-09-16 closure above), next session should
      either commit to one of those two or switch to a different
      backlog item (comm-ROM incoming-byte path, hardware jumper hunt,
      or the rename-everything sweep are the other live candidates in
      this file) rather than defaulting back into this hunt by inertia.

      **2026-10-08: picked back up per explicit request to go deeper -
      found the family is much bigger than thought, still no caller.**
      A direct grep for every `[0x1DB0]` reference across both proven
      and heuristic listings for *both* main-ROM chips at once (never
      done before this exact way) turned up 3 more readers: a third
      `160-3633` sibling (now named `draw_readout_char_dup2` - this is
      the renamed `FUNC_3633_E60C`/`FUNC_3633_DF56` referenced above -
      plus a newly found `draw_readout_char_dup1`), and, more
      significantly, **3 entirely new readers in `160-3532`** (the
      *other* main-ROM half - this doc had only ever examined
      `160-3633`'s copy before), now named `draw_readout_char_3532_1`/
      `_2`/`_3`. All identical mechanism, all `ref_count: 0` in both
      the heuristic scanner and an independent Ghidra cross-check (for
      the first of the three, Ghidra's own auto-analysis didn't even
      create a function boundary - consistent with code that thorough
      that even a second, independent tool never bothered to mark it).
      A 4th related `160-3532` function, `draw_tick_marks_3532`, draws
      into the same shared buffer but doesn't read `[0x1DB0]` itself.
      Total known footprint is now **6 functions across both chips**
      with this mechanism; still only `draw_readout_char` itself is
      confirmed reachable (and dead on this project's real hardware,
      per 2026-09-16 above). Named all 6 in `FUNCTIONAL_NAMES`/
      `FUNCTIONS.md` and applied the names to the Ghidra project too
      (`decompile/apply_names.py`, then regenerated `decompile/
      exports/160-3633-14.c`/`160-3532-14.c`) since the mechanism is
      now understood confidently even though reachability isn't. New
      variables `[0x6AE]`/`[0x6AA]` (160-3532's counterparts of
      `[0x45E]`/`[0x46E]`) and `[0x1C02]` (confirmed **shared** between
      both chips' families) added to `VARIABLES.md`. Full detail:
      `docs/display/vector-display-and-stroke-font.md`'s "Follow-up,
      2026-10-08" section. **Note**: `binary/aligned/`'s `_readable.asm`
      regeneration step needs `nasm`, which isn't on `PATH` in this
      environment - the new names are live in `.lst`/`.symbols.json`/
      Ghidra, but `_readable.asm`/`binary/aligned/*.bin` are now stale
      with respect to them until that's rerun somewhere `nasm` exists.

      Below is the original investigation history, preserved as-is.

      Mechanism is fully understood
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
      self-test function (`selftest_front_panel_switch_a`,
      `selftest_acq_ab_addr_walk` (renamed 2026-10-09 from
      `selftest_front_panel_switch_b` - it's the real `ACQ_AB` test,
      see `docs/self-test/dispatcher-and-siblings.md`),
      `run_adc_selftest_range`, `selftest_tb_divider`, `step_tb_
      divider_test`, `step_acq_ab_addr_walk` (renamed from
      `step_front_panel_switch_b_test`), `wait_stable_
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
      entry, then regenerate everything: `gen_disasm_x86.py`, `gen_
      disasm_mainrom_heuristic.py`, `gen_source.py <nasm>` (bare CLI
      default - proven-only entry points, correct for 160-3633/160-3532
      *and* for 160-2998's own `_readable.asm`), **then
      `gen_source_2998.py <nasm>` separately** to regenerate plain
      `160-2998-14.asm` with its own heuristic layer included (`gen_
      disasm_2998.py`'s prologue-scan entries) - skipping this step
      silently reverts the comm ROM's `.asm` to proven-only coverage,
      discarding ~20000 previously-recovered heuristic instructions
      (discovered the hard way 2026-10-09, see `changes/2026-10-09.md`;
      `gen_source_readable.py` needs no separate comm-ROM step - its
      2998 output is intentionally proven-only, unlike the plain
      `.asm`). Don't substitute `validate_all.py`'s "all three heuristic
      sources combined" entry-point formula for `gen_source_2998.py`'s
      narrower one - it round-trips byte-identical too, but corrupts the
      diagnostic comment field on ~150 instructions via a `visited`-dict
      overwrite when the main-ROM heuristic layer's entries alias into
      comm-ROM physical addresses. Re-verify byte-identical/length-
      matching (and a near-zero `git diff --stat` on files your rename
      shouldn't have touched) before committing.
- [ ] **RS-232 comm thread**: the original "live command silence"
      blocker is **resolved** (2026-09-14 - baud-rate reliability, not
      firmware; dropping to 1200 baud fixed it. See `docs/comm-rom/
      rs232-breakthrough.md` and `changes/2026-09-14.md`). Still
      genuinely open, lower priority now:
      - Which `[0x1B83]` value (`0x1E` vs `0x14`) means "comm option
        installed" - write-probe address confirmed as general-purpose
        Time Base Mode Register U4119, not comm-specific; exact bit
        semantics still unresolved. See `detect_comm_option_hw` in
        `FUNCTIONS.md`, `docs/comm-rom/option-detection.md`.
      - The genuine UART-receive entry point (where an incoming byte
        first lands in `[6]`/`[0x580]`) is still unfound. The command-
        keyword table and its 6-byte-per-entry dispatch index ARE found
        (`docs/comm-rom/command-keyword-table.md`), but the code that
        walks it / assigns numeric command IDs from incoming bytes is
        not - a grep for the dispatch table's literal segment value
        found zero hits in already-disassembled code.
      - `STAtus?` returning `STATUS 128;` once was never reproduced
        (10/10 clean `STATUS 0;` on retry) and Table 7-34 hardcodes bit
        7 to `0` in every documented category - likely a one-off
        transient serial glitch, not a firmware defect. See
        `docs/comm-rom/rs232-live-session-2026-09-14.md`.
      - Whether `FUNC_2998_39F5` or `poll_comm_status_tick` relates to
        the real receive path is unresolved but moot now that RS-232
        works in practice.
      - Diffing/disassembling comm ROM revision `-13` remains a
        legitimate documentation gap, not a motivated investigation
        anymore.
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
