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
      - **Next step done, 2026-10-09** (re-read page 65 at 2x crop
        zoom - see `docs/diagrams-index.md`'s updated "Jumper
        findings" section for full detail): the prior bullet's own
        "`P9104` is NOT a jumper" conclusion was **wrong** - a closer
        crop shows a real 2-position jumper explicitly printed
        `P9104` with `RESET`/`NORM` labels, sitting on the same RC
        network, exactly matching `docs/theory-of-operation.md`'s
        text word for word. `U9104` (the clock IC) and `P9104` (this
        jumper) are two different, co-located parts - the survey just
        missed the small label the first time. Also found that
        "`P9105`" is actually **two** separate connectors: `P9105A`
        (3-pin "REMOVE FOR TEST," feeds `NMI`, with an earliest-board-
        revision pin-1/3-swap note) and `P9105C` (the TEST/NORM one,
        feeds the `EN` pin of decoder `U9106`, which drives
        `BLK0`-`BLK3`/`RAM SEG`/`TO SEG`/`COM SEG`). And `P9107`'s
        wiper now has a traced destination: it feeds one input of OR
        gate `U9102C`, ORed with `U9104`'s own `RESET` output.
        **Corrected, same day**: the OR gate's output does NOT go to
        "RESET TO U9208-9" - that text is a different net on the same
        page (`U9104`'s raw `RESET` fanning out directly off-page to
        Diagram 15/Digital Display). `U9102C`'s real output continues
        on-page into the CPU/latch cluster, traced directly into
        `U9111` - whose schematic block is explicitly labeled "8088, 8
        BIT MICROPROCESSOR" (first direct schematic-text confirmation
        of the CPU part number, see `docs/architecture/cpu-and-
        language.md`). So `P9107` gates the CPU's own local reset
        line, not a cross-board signal. **Still unresolved**: which of
        `P9107`'s two positions (ON/OFF) drives its OR-gate input high
        vs. low, and in turn whether that's the comm-detect mechanism
        originally suspected - the schematic crop doesn't show a rail
        label on either position pin, so this remains open rather than
        guessed. `P9105C`'s direction is now partly resolved (same day,
        via manual cross-reference, not further schematic tracing):
        `docs/theory-of-operation.md`'s "Decoder" section states "In
        normal operation, address block decoder U9106 is always
        enabled" - matching the jumper's own **NORM** label, so **TEST**
        is by elimination the position that takes U9106 out of its
        normal always-enabled state (consistent with the manual's
        documented power-up "hardware kernel test" mode). Exact pin
        polarity and what TEST substitutes for normal decoding are
        still not traced from the schematic - see `docs/diagrams-
        index.md`'s updated `P9105C` entry. Real next step, if ever
        revisited, is tracing `P9107`'s position pins to their
        rail/signal source on an adjacent schematic page, or checking
        the physical unit.
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
      probes.md` and `changes/2026-10-09.md`. (2) ~~what fills the
      incrementing-ramp pattern `verify_pattern_with_report` expects at
      far ptr `[0x31E]` (physical `0x48000`)~~ **done 2026-10-09** -
      `io_stubs.AdcRampFillStub` hooks the exact per-byte comparison
      read (`0xE113A`) and mirrors `verify_pattern_with_report`'s own
      already-computed "expected" local back into the buffer, since
      the self-test is defined to pass on real hardware - fixes
      `HS_ACQ`, `TBD hs/2`, *and* the previously-unreached `TBD ps/2`
      with one stub. Both now fully gone from a clean boot trace. See
      `docs/self-test/hardware-probes.md` for the full derivation
      (including 2 previously-undocumented mechanics found while
      re-reading the function: the expected-value accumulator wraps
      `&0xFF`, and mismatch *printing* silently stops past loop index
      `6` even though the comparison itself keeps going). **New open
      item found by clearing these three**: `selftest_mm_acq`
      (`MM_ACQ`) is now reachable too, and fails its own *different*
      "fill @"-shaped check (not `verify_pattern_with_report` - an
      inline loop comparing adjacent scratch-buffer byte deltas
      against 2 fixed allowed values, `0xFF`/`0xC7`). Deliberately
      *not* stubbed yet: there's no single computed "expected value"
      local to mirror here, so fixing it would mean guessing actual
      buffer content rather than deriving it - see `FUNCTIONS.md`'s
      `selftest_mm_acq` entry for the full mechanism as traced so far.
      Separately, traced the next-reached failure after `MM_ACQ`:
      `CDT`'s `"PRE-DETRIG"` (`wait_stable_measurement`, called from
      `measure_cursor_delta_time`/`selftest_cursor_delta_time` and
      `selftest_display_result_mode`). **Done 2026-10-09** -
      `io_stubs.AcqMemReadyBitStub` ORs bit `0x2000` into `[0x4377E]`
      at the exact check instruction (`0xE2EC8`), directly implied by
      the branch structure ("bit clear -> fail"), leaving bit `0x4000`
      untouched since it would corrupt a downstream numeric check.
      Verified live and A/B'd via `git stash` (same `halt_cpu` stop).
      **New open item found by clearing it**: `CDT` now fails a
      *different* way - `"uncaled : min = 0"` / `"uncaled : delta =
      0"` - because `measure_cursor_delta_time` range-checks the raw
      stabilized value against `[0x55,0x73]`/`[0xc8,0xd2]`, and
      `[0x32A]`'s plain-RAM default (`0`) falls outside both. This
      looks like it needs a genuine calibration constant, not a logic
      fix, so it's left unstubbed - see `docs/self-test/hardware-
      probes.md` and `STILL_PENDING_DECODE.md`.
      Traced the next-reached failure past `CDT` too: `FP_a2d`
      (`selftest_front_panel_adc`/`selftest_init_channel_hw`, a
      *different*, interrupt-driven `[0x1D20]`-based register cluster,
      unrelated to the `[0x322]`/`0x4377E` one above). Its busy-wait
      "ready" bit (`es:[di+5]&4`) is just as derivable as `CDT`'s was,
      but fixing only that doesn't make it pass: the real return value
      is built from a dual read of a separate data register
      (`es:[di+4]`), and that register's unstubbed-RAM default (`0`)
      still fails the self-test's `[0x100,0x700]` range check either
      way - just with a different message (`"gnd"` instead of
      `"TIME-OUT"`). **Deliberately not stubbed** - same "don't guess
      real hardware content" reasoning as `MM_ACQ` - see
      `STILL_PENDING_DECODE.md`'s new "Front-panel A/D converter
      self-test" section and `FUNCTIONS.md`'s `selftest_init_channel_
      hw` entry.
      Also traced `ROMS`'s `"MISMATCH,14,4C,14"` failure fully, and
      **this one's genuinely resolved, not a stub candidate at all** -
      `selftest_rom_checksum` isn't a computed checksum, it's a
      revision-byte cross-check between two ROM-embedded headers at
      physical `0xE0000`/`0xE8000` (the low/high 32KB of `160-3633`'s
      own 64KB image). Both bytes come straight from real ROM content
      the emulator already maps correctly (`0x14`/`0x4C`), and the two
      far pointers it reads through (`[0x1DD4]`/`[0x1DD8]`) *are*
      initialized by the already-documented `init_far_pointer_table_
      sysrom` table after all - an earlier same-day pass wrongly ruled
      that table out by comparing dest-offsets against the wrong base
      segment. One genuinely open question remains (whether `0xE8000`
      is really a second physical chip's header or just an incidental
      byte) - see `STILL_PENDING_DECODE.md`'s new "Main ROM revision
      cross-check" section and `FUNCTIONS.md`'s `selftest_rom_checksum`
      entry.
      Finally, traced `COMM_ROM`'s `"0C8F <> 2BA3"` checksum mismatch
      and **it's also fully resolved, genuinely not an emulator gap**:
      `selftest_comm_rom` checksums `160-2998` against its own
      embedded expected value (the big-endian word at its own first 2
      bytes, `0x2BA3`) via `compute_range_checksum` (a shift-and-add-
      with-carry running checksum) over physical `0x80002-0x87FFF`
      then `0x90000-0x97FFF`. Running the identical algorithm directly
      over `binary/160-2998-14.bin` in Python gives `0x0C8F`,
      reproducing the live trace exactly - a deterministic, ROM-only
      computation with zero RAM/stub dependency, same category of
      finding as `ROMS`. See `STILL_PENDING_DECODE.md`'s new "Comm ROM
      checksum" section.
      Last, traced `COMM_LB`'s `"FGET NOT SET"`/`"FGET NOT CLEAR"`
      failures (`selftest_comm_fget_flag`, `0xE1FBC`) - **this one's
      a genuine stub candidate, same category as `MM_ACQ`, not a
      resolved-ROM-content finding like `ROMS`/`COMM_ROM`**. Both
      subtests write a command byte to physical `0x406F3` (the 4th of
      the comm option's 8-register UART/GPIB bank) then check
      `comm_stat`/`0x4067C` bit `0x4` (`TBRE`) and `comm_param`/
      `0x406BC` bit `0x80` (the UART's live TX-line bit). Both fail
      deterministically because `0x406F3` isn't one of the two
      registers `io_stubs.InteractiveUartMock` models (`0x406F0`/
      `0x406F1`, the 8251 data/control pair), so the write never
      updates anything `comm_stat`'s `TBRE` mirror reads from, and
      `comm_param` bit `0x80` is a static, never-toggled
      `COMM_OPTION_STUBS` baseline. Left unstubbed on purpose:
      `MEMORY_MAP.md` already flags `0x406F1`-`0x406F3` as plausibly
      TMS9914A (GPIB chip) register space rather than confirmed UART
      registers, so there's no independently-confirmed real semantics
      to build a stub from without guessing. See `STILL_PENDING_
      DECODE.md`'s new "Comm-board loopback flag check" section and
      `FUNCTIONS.md`'s `selftest_comm_loopback_b` entry.
      **With this, every diagnostic line reached by a full boot trace
      has now been traced to a known cause**: 4 are genuine stub
      candidates deliberately left unstubbed pending real hardware
      content (`MM_ACQ`, `FP_a2d`'s `"gnd"`, `CDT`'s `"uncaled"`,
      `COMM_LB`), and 2 are genuine ROM-content inconsistencies with
      no further emulator-side work possible (`ROMS`, `COMM_ROM`).
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
- [ ] Find the comm ROM's actual **incoming**-data path - narrowed
      2026-10-09, not yet closed. The ring buffer at `[0x448]`/`[0x44C]`
      (base `0xAF`, size `0x384`) turned out to be a TX queue
      (`serial_tx_buffer_put` producer, `service_comm_tx_queue`
      consumer), not RX. **Now confirmed**: `comm_call_main_rom`/
      `process_gpib_command_byte` run via a dedicated scheduler task
      slot's default-handler table entry (`comm_task4_default_handler`,
      see `docs/interrupts/task-scheduler.md`'s 2026-10-09 section) -
      i.e. tick-driven cooperative polling, not a byte-level hardware
      interrupt. **Still missing**: where a real incoming byte first
      lands in `[6]`/`[0x580]`/`[0x590]` from actual hardware -
      `comm_call_main_rom` itself only ever writes `[6]` with 2 fixed
      literals, never from an I/O port read. **Both previously-flagged
      leads are now ruled out (2026-10-09)**: all 5 main-ROM
      `[0x590]` writes (including `0xF63CE`) are one unrelated
      buffer-bookkeeping code shape, and `FUNC_2998_56F8`/
      `FUNC_2998_5712` have zero callers anywhere in the comm ROM
      (confirmed by raw byte scan for far-pointer references, not just
      listing grep) - they're blind prologue-scan artifacts from
      `gen_disasm_2998.py`, not proven-reachable code. New lead found
      instead: a previously-undocumented far-pointer sub-table at
      `[0x1AD0]`-`[0x1ADE]` (written by `init_selftest_register_group`,
      pointing at the UART register bank `0x406F0` and its neighbors
      `0x4067C`/`0x406BC`/`0x406F8`) that's never read in any
      currently-disassembled code - probably consumed by a not-yet-
      found generic self-test/exerciser table-walk routine (matches
      the documented `/DIAGNOSTICS/EXERCISERS/IO/INPUT_PORTS` screen).
      Also confirmed the comm ROM has no genuine `in`/`out` instruction
      at all, consistent with the UART being memory-mapped only - see
      `docs/interrupts/task-scheduler.md`'s 2026-10-09 follow-up
      section for full detail.
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
      section for the full trace. **Checked 2026-10-09**: decoded
      `init_readout_port_config`'s literal bytes (`0x29`/`0x23`/`0x06`)
      against the 82C52's (U1251's confirmed part number) actual
      Mode/Command instruction bit layout - they don't fit. All three
      independently hit the chip's reserved/invalid stop-bit field
      when read as Mode bytes, and the write pattern itself (3
      different bytes to 3 different consecutive addresses) doesn't
      match how this chip's single-control-register, write-order-
      based Mode/Command interface actually works. Further evidence
      against the UART theory, still doesn't identify what the block
      actually is - remains open.
- [ ] **`PLOt FORmat [XY]` (HPGl/EPS7/EPS8/TJEt) software-override
      write site still not found.** The rest of this item (DIP-switch-
      vs-menu precedence for baud/parity/terminator/printer-plotter,
      `STOP BITS`/`FLOW` having no DIP-switch counterpart at all) is
      resolved - see `MEMORY_MAP.md`'s "RS-232 option board" section.
      `[0x461]`'s only confirmed writers in either ROM's disassembled
      bytes are the two DIP-switch readers themselves
      (`read_dip_switches_serial_config`/`_gpib_config`); a literal-
      displacement byte scan of the comm ROM's full undisassembled gap
      also came up empty (caveat: can't rule out an indirect/computed-
      pointer write). Most likely sitting in the ~8-9% of each ROM's
      byte range neither disassembly pass has reached yet. The
      adjacent `EPS7`/`EPS8`/`FORmat`/`HPGl`/`TJEt` strings and the
      comm-ROM "block 6" dispatch table near them turned out to be
      unrelated leads (now decoded/resolved - see
      `docs/comm-rom/command-keyword-table.md`'s "## 4." section). Pick
      up by targeting the remaining undisassembled gap directly if
      revisited.
- [ ] `COMM/DATA/ENCDG`'s ASCII/BINARY/HEX
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
      highest first - 262/282 named as of the last count; **+2 more
      named 2026-10-09** (`umod32`/`umod32_core`, `0xE7866`/`0xE7895` -
      previously-flagged, never-investigated unnamed proven functions;
      also corrected `sdiv32_unsigned_divisor` -> `smod32` in the same
      pass once tracing these two revealed its real operation is
      modulo, not division - see `FUNCTIONS.md` and `changes/2026-10-
      09.md`). A fresh `kind=="sub"` count now shows 19 unnamed (284
      total) - don't treat this as directly comparable to the 262/282
      figure above without re-deriving the same filter; re-derive a
      true like-for-like count before relying on either number. **Also
      investigated 2026-10-09**: `SUB_EA2D6` (the other previously-
      flagged, never-investigated unnamed proven function) and its
      sibling call target `SUB_EA13B` - both turned out to be a
      disassembly anomaly, not real code (their addresses land inside
      known string-table text; their shared caller `SUB_F173E` is a
      pre-existing manually-seeded entry point whose own body looks
      coherent but whose call targets don't hold up - see `docs/
      decode-anomalies/unknown-data-deep-dive-2026-09-15.md`'s
      2026-10-09 section). Left unnamed, correctly, not just not-yet-
      looked-at. **Also investigated 2026-10-09, same day**:
      `SUB_EAC86` (ref_count=4, the highest-ref-count unnamed proven
      function remaining) and its sole call target `SUB_EADA0` - same
      disassembly-anomaly pattern as `SUB_EA2D6`/`SUB_EA13B` (both
      decode as a structured-binary-record garbage pattern that
      exactly bridges two already-documented `UNKNOWN_DATA.md` gaps),
      but with a harder contradiction: `SUB_EAC86`'s 3 callers trace
      through `assemble_boot_splash_logo_chunks` (`0xF5898`, renamed
      later the same day - see below) back to the already-confirmed,
      boot-critical `draw_boot_splash_and_option_icon` - not another
      shaky entry point. Tried the emulator to settle it empirically;
      inconclusive (boot halts on unrelated, already-documented self-
      test failures before reaching it). Left unresolved and unnamed -
      see `docs/decode-anomalies/unknown-data-deep-dive-2026-09-15.md`'s
      "2026-10-09 follow-up #2" section. **`E97DC` resolved, same day**:
      turned out not to be a new anomaly at all - it's a legitimate
      secondary entry point into the already-confirmed `extract_
      strided_channel_samples` (`0xE9744`), already documented in
      `FUNCTIONS.md` but never given its own `FUNCTIONAL_NAMES` entry;
      renamed `copy_words_stride4` (see `FUNCTIONS.md`'s `0xE97DC`
      row). **`SUB_F5898` resolved, same day**: also not a new anomaly -
      its full body (always one `SUB_EAC86` call, 2 more gated by
      `[0x1bfb]` bit 0, then an unconditional tail call to `copy_words_
      stride4`) is completely coherent even though its own entry lacks
      a visible prologue (same "shares caller's open frame" class
      already documented for `sync_shift_register_output`/`apply_
      pending_position_delta`); renamed `assemble_boot_splash_logo_
      chunks` (see `FUNCTIONS.md`'s `0xF5898` row). **Correction to this
      note's own prior claim**: re-checking `docs/decode-anomalies/
      dual-entry-points.md` shows `E90A5`, `E92B0`, `F156E`, `F1581`,
      and `E99DF` were already investigated (by earlier sessions) and
      correctly left unnamed as landing-artifacts/garbage-data targets,
      not "not yet investigated" as this note previously claimed; `F5184`
      and `EF346`/`EF393`/`EFB64`/`EFBA5` are likewise already
      investigated and documented (see `docs/acquisition-and-plotting/
      mode-dispatcher-and-formatting.md`) as correctly-unnamed for
      their own separate reasons; and `E5D67` is not unnamed at all -
      it's already renamed `INT2_HANDLER_EARLY` (`FUNCTIONS.md` line
      315). **`E8E03`/`E8E29` resolved, same day**: unlike `SUB_F5898`,
      these did *not* turn out to be nameable "for what they do" -
      raw-byte-dumping `160-3633-14.bin` around both addresses showed
      each call target lands exactly 1 byte before a real, clean
      instruction (`jne L_E8E16` for `E8E03`'s case; `je L_E8E41` for
      `E8E29`'s), with the intervening garbage bytes (`adc`/`sub`/`and`/
      `cmp` nonsense, or a bogus `add`) merely an artifact of starting
      mid-displacement-byte. Confirmed by checking the raw bytes 1
      position earlier in each case and finding a complete, sensible
      instruction whose own target is the exact same reconvergence
      point the garbage decode eventually reaches anyway. This is 2
      more confirmed instances of the already-documented landing-1-
      byte-early artifact class (`SUB_E90A5`/`SUB_E92B0`/`L_EDA0A`),
      now 7 instances total - see `docs/decode-anomalies/dual-entry-
      points.md`'s new `SUB_E8E03`/`SUB_E8E29` subsection. Correctly
      left unnamed, not just not-yet-looked-at; this closes out the
      last 2 items from the original flagged list. **This means the
      originally-flagged "not yet investigated" list is now fully
      resolved** - everything on it has been either renamed
      (`E97DC`/`F5898`) or confirmed as a correctly-unnamed anomaly/
      landing-artifact (everything else). The next step for this
      standing goal is to re-derive a fresh ref-count-ordered list of
      remaining unnamed proven functions (the 19-vs-262/282 count
      mismatch noted above still needs a proper like-for-like re-
      derivation) rather than continuing to work off this list.
      **Re-derived, same day**: queried `sysrom_3532_3633.symbols.json`
      directly for `kind=="sub"` entries with no `functional_name` -
      **17 unnamed out of 284 proven subs** (not 19; that figure must
      have counted something else), sorted by `ref_count`:
      `EAC86`(4), `E90A5`/`F156E`/`F1581`(2 each), then 13 more at 1
      each (`E5D67`, `E8E03`, `E8E29`, `E92B0`, `E99DF`, `EA13B`,
      `EA2D6`, `EADA0`, `EF346`, `EF393`, `EFB64`, `EFBA5`, `F5184`).
      Cross-checked every single one against this session's work and
      earlier docs: **all 17 are already accounted for** - either a
      real rename that just lives in a different field (`E5D67` is
      named `INT2_HANDLER_EARLY` via the `ENTRY_POINTS` mechanism, not
      `FUNCTIONAL_NAMES`, so `functional_name` stays `None` by design,
      not by omission - see `gen_disasm_x86.py` line 65) or a
      documented landing-artifact/garbage-data anomaly (`EAC86`,
      `E90A5`, `F156E`, `F1581`, `E8E03`, `E8E29`, `E92B0`, `E99DF`,
      `EA13B`, `EA2D6`, `EADA0` - `docs/decode-anomalies/dual-entry-
      points.md` and `docs/decode-anomalies/unknown-data-deep-dive-
      2026-09-15.md`) or a separately-documented correctly-unnamed
      function (`EF346`/`EF393`/`EFB64`/`EFBA5`/`F5184` - `docs/
      acquisition-and-plotting/mode-dispatcher-and-formatting.md`).
      **The "rename every identifiable proven routine" goal is now
      substantively complete** for the proven-reachable set - nothing
      left in it is simply unlooked-at. Future renaming work on this
      codebase should look to the much larger heuristic-only layer
      instead (see below), or wait for a newly-confirmed proven entry
      point to surface (e.g. via the far-pointer-table technique in
      `docs/acquisition-and-plotting/ram-far-pointer-table.md`).
      See `docs/` (start at `docs/README.md`) for the
      dated session entries and `changes/` for the running list. The heuristic-only layer
      (tens of thousands more, across all 3 ROMs) is a much lower-
      confidence, much larger tail - the realistic goal is "every
      proven-reachable routine named," not literally every heuristic
      placeholder. Mechanically: add `{address: "name"}` to `gen_
      disasm_x86.FUNCTIONAL_NAMES`, add the matching `FUNCTIONS.md`
      entry, then regenerate everything: `gen_disasm_x86.py`, **if the
      renamed address is comm-ROM-local (`0x8xxxx`/`0x9xxxx`), also run
      `gen_disasm_2998.py`** (a separate script that builds `160-
      2998-14.symbols.json`/`.lst` on its own and consults `gen_
      disasm_x86.FUNCTIONAL_NAMES` independently - skipping it leaves
      `160-2998-14.symbols.json`'s `functional_name` field stale/`None`
      for the new name even though every other pipeline step reports
      success, since it isn't invoked by any of the other scripts below
      - discovered the hard way 2026-10-09, see `changes/2026-10-09.md`),
      `gen_disasm_mainrom_heuristic.py`, `gen_source.py <nasm>` (bare CLI
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
      **Progress 2026-10-09**: named `reinit_comm_channel` (`0x8FB01`,
      comm ROM; reachability caveat - no confirmed caller found, see
      `docs/comm-rom/command-parser-token-scan-and-plot-handler.md`)
      and `reset_comm_token_length` (`0x850A2`, comm ROM). Also traced
      (but left unnamed, pending further characterization)
      `FUNC_2998_50C2`/`50E4`/`5115`/`519A`/`44D0` and ruled out
      `FUNC_2998_44D0` as the still-missing `PLOt FORmat`/`[0x461]`
      write site - see that same doc. **Follow-up, same day**: resolved
      that doc's "blocking problem" (whether `FUNC_2998_5115`/`519A`'s
      hypothesized letter-indexed keyword-table base pointers,
      `[0x6EA]`/`[0x6F6]`, are ever legitimately initialized) by
      searching every write site in both ROMs' full proven+heuristic
      listings, not just proven code as the first pass had: both pairs
      are written only by unrelated main-ROM HPGL/plot-cache code
      (`FUNC_3633_7EBF`'s fixed `0x8000:0x4000` buffer-clear gated by
      the HPGL pen-state variable `[0x6CA]`; the already-known `reset_
      all_channel_plot_caches`), confirming genuine cross-subsystem
      address reuse rather than an as-yet-undiscovered comm-ROM
      initializer. This means the letter-indexed-table hypothesis is
      reading from addresses that would get clobbered by ordinary
      plot activity - evidence against it, not just "unconfirmed."
- [ ] **RS-232 comm thread, genuinely open parts** (the "live command
      silence" blocker itself is resolved - baud-rate reliability, not
      firmware; see `docs/comm-rom/rs232-breakthrough.md`):
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
