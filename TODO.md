# Active work

## Next up

- [ ] **REVIEW LATER**: `binary/aligned/*.bin` (NOP-padded, fully-
      readable reconstructions - see `binary/aligned/README.md` and
      `disasm/NOTES.md` "NOP-aligned readable reconstruction") were
      adopted as the reference binary for future checks. Revisit this
      choice once the rest of the analysis (self-test subroutine ID,
      menu tree, I/O port mapping) is further along, to confirm
      nothing was missed by not using the true original byte-for-byte.
- [ ] Cross-reference the front-panel menu/parameter tree names found
      in `160-3633` (`ACQ_MODE_SETUP_TABLE`, `SELECT_MODE`, etc. - see
      `STRINGS.md`) against the code that reads them, to build the
      actual menu structure for `JUMP_MAP.md`/the "user-facing flows"
      documentation goal. Also cross-reference the self-test
      diagnostic message strings found in `160-3532` (`STRINGS.md`)
      against the still-unidentified subsystem-test subroutines below
      - the printed text next to each test is a strong hint at what it
      actually checks.
- [x] ~~Widen code coverage beyond the current ~10%~~ — **big jump: now
      85.6% combined (68,776 instructions) once a heuristic push-bp
      signature layer was applied to the main ROM too** (same technique
      already working well for the comm ROM). `3633`/`3532` both went
      from ~14-41% to 90.6% each. See `disasm/NOTES.md` "The main ROM
      has a heuristic layer too". Proven-only coverage is still ~8% and
      that number's lever is exhausted for now (checked: IVT-tracing
      with broadened register patterns found only false positives, 0
      unresolved indirect jmp/call anywhere, 0 unmapped far-call
      targets left) - growing the *proven* number further needs a new
      kind of independent entry point, not more of the same technique.
- [ ] Handle the ~39-instruction decode-drift cluster in `3633` around
      physical `0xEA1A0-0xEA615` (386-only features that can't be real
      on this 8086/8088 - see `disasm/NOTES.md`). Doesn't threaten the
      buildable `.asm`'s correctness (safety net already excludes it),
      but reaching it via pure fallthrough with no owning label means
      there's a real function boundary nearby the recursive descent
      doesn't know about - worth finding for a cleaner listing.
- [ ] Investigate `SUB_EAC86` (`160-3633`, proven set) - decodes as
      unambiguous garbage (including an impossible SSE instruction)
      despite being a **clean, unambiguous far-call target** reached
      identically from 3 separate places across 2 ROMs (`160-3532` x2,
      comm ROM x1). Different and more puzzling than the
      `0xEA1A0-0xEA615` fallthrough cluster above - no nearby byte
      shift produces a clean prologue either. Left unrenamed rather
      than guess. See `disasm/NOTES.md` "A second, more puzzling
      decode anomaly: SUB_EAC86" for what's been ruled out and
      candidate explanations (address-decode alias? dead code?).
- [x] ~~Follow `SUB_E094B` to find where self-test results get
      displayed~~ — traced it, and found something more valuable: it's
      called from `print_selftest_banner` (`0xE416F` - renamed;
      **CORRECTED, this was wrongly named `self_test_dispatcher`
      before** - it contains zero test calls, only banner-printing).
      The real OR-fold dispatcher is a different function, `0xE4244`
      (now carries the `self_test_dispatcher` name). Tracing
      `print_selftest_banner`'s print-primitive call chain
      (`print_string_far` → `print_char` → `write_readout_port_byte`)
      also found a new confirmed hardware region: physical
      `0x40000+0x6F0` is a fixed-address readout/CRT character-
      generator write port, plus two more candidate read ports at
      `0x41000`/`0x42000` and a dual-plane display buffer at
      `0x40000`/`0x48000`. See `disasm/NOTES.md` "self_test_dispatcher
      was misnamed" and "The readout/CRT display memory",
      `MEMORY_MAP.md`, `FUNCTIONS.md`.
- [x] ~~Identify the ~14 not-yet-named subsystem-test subroutines
      called from `self_test_dispatcher`~~ — **done, all identified**.
      The lever that worked: search each subroutine's body for a load
      of the fixed string-table segment (`0xFF7B`) + offset, then read
      the actual bytes at that physical address - nearly every one
      references a diagnostic label already in `STRINGS.md` (`HS_ACQ`,
      `MM_ACQ`, `XY_ACQ`, `ACQ_RAM`, `ROMS`/`MISMATCH`, `COMM_ROM`,
      `COMM_RAM`, `CMOS`, `COMM_LB`, `CDT`). Also fixed a documentation
      error: a full re-read of `self_test_dispatcher` found it actually
      calls `SUB_E28FE`/`SUB_E227E`/`SUB_E26D6`/`SUB_E286C`/`SUB_E2CEC`
      directly (earlier notes had wrongly placed them in the
      surrounding caller instead). See `disasm/NOTES.md` "Identified
      self_test_dispatcher's sibling subroutines" and `FUNCTIONS.md`.
      Three tests (2 front-panel-switch scans + 1 comm-option-switch
      scan) don't reference a string - identified by their distinctive
      `update_menu_position`-range-scan shape instead; which physical
      control each corresponds to is still open (see next item).
- [ ] Identify what peripheral `0x41000`/`0x42000` (single-byte read
      ports, found this session near the confirmed readout/CRT write
      port) actually are - front-panel switch/encoder status and CRT
      controller status are both plausible; check the service manual's
      I/O map when available. See `MEMORY_MAP.md`.
- [ ] Confirm whether the `0x48000-0x4FFFF` "second plane" found this
      session (written by `append_readout_char`, read back by
      `SUB_E0BA3`) is an attribute/inverse-video plane, a shadow copy,
      or per-channel diagnostic scratch space - see `disasm/NOTES.md`
      "The readout/CRT display memory".
- [ ] Identify and mark data regions (ASCII strings, tables) inside the
      already-reached code so the listing stops trying to disassemble
      them as instructions.
- [x] ~~Figure out the comm/GPIB ROM's (`160-2998`) exact bank-switching
      scheme/window~~ — **there is no bank-switching.** Confirmed it's a
      plain fixed-address 64KB device at physical `0x80000-0x8FFFF`,
      the same simple pattern as the two main-ROM halves (82/84 far-call
      targets land exactly on its own known function starts; the main
      ROM's proven-reachable code calls directly into it too). See
      `disasm/NOTES.md` "The comm ROM is NOT bank-switched". Disassembly
      redone against the corrected mapping (`160-2998-14.asm`, 20,446
      instructions with the heuristic layer included, NASM-validated).
- [x] ~~Figure out what's really at `0x90000` and up~~ — **fully
      resolved: it's an address-decode alias, not a separate region.**
      `0x90000-0x97FFF` is the same comm-ROM bytes as `0x88000-0x8FFFF`
      (file offset `0x8000-0xFFFF`), confirmed 82/82 by brute-forcing
      every possible base offset against observed far-call targets.
      Wired into `gen_disasm_x86.CHIPS` as `"2998_alias_90000"` (a
      `slice` of the same file) so the recursive descent follows it
      automatically - this alone grew main-ROM coverage from 10,488 to
      13,510 instructions. See `disasm/NOTES.md` "The 0x90000-0x97FFF
      region is fully resolved".
- [x] ~~Figure out how code gets INTO the mystery region~~ — moot, see
      above; there's no separate region to populate.
- [x] ~~Figure out what the `0xAA55` pattern really tests~~ — **it's an
      option-board presence + RAM/IO detection routine**
      (`check_comm_option_installed`), not a generic memory-size probe. Checks a ROM-
      header-style checksum first (byte + one's-complement = `0xFF`,
      same pattern as every ROM's self-ID header), then tests
      writability only if that passes. Prompted by the user asking
      whether it might be an install-check and/or RAM/IO on the comm
      board - both correct, as two stages of one probe. See
      `disasm/NOTES.md` "Found: the option-board presence/RAM-detection
      routine".
- [x] ~~Handle `insb`/`insw`/`outsb`/`outsw`~~ — done. Confirmed each
      assembles to the plain expected single opcode byte with no
      unwanted prefix; `insb`/`insw` treated like `stosb`/`stosw` (no
      possible segment override), `outsb`/`outsw` like `movsb`/`movsw`.
      Also fixed `xchg`'s reg/rm-swap ambiguity (needed its own
      `alt_xchg_encoding()` - same opcode, swapped ModRM subfields,
      not the direction-bit-flip pattern used for ALU ops/MOV). See
      `disasm/NOTES.md` "Validation status". Still excluded (and
      likely to stay that way - see NOTES.md for why): x87 FPU
      instructions (`fdiv` etc. - capstone's `st(N)` operand syntax
      needs translating for NASM), and `insd`/`outsd`/`bound` (require
      CPU features - a `0x66` prefix, or 80186+ - that can't be real
      on this 8086/8088, so any occurrence is far more likely decode
      drift than genuine code worth handling).
- [ ] Investigate the 2 `push` instructions with a stray `0x67` prefix
      that `validate_2998.py` flagged as real mismatches, landing right
      at the start of page 1's header/copyright text - almost certainly
      decode drift into data at the deepest heuristic reach, but worth
      a quick look to confirm.
- [ ] Narrow down what peripheral the I/O ports actually seen in code
      (`0x83`, `0xC4`, `0xD1`, and a DX-indexed range) correspond to —
      see `MEMORY_MAP.md` "I/O ports actually seen in code".
- [ ] **In progress: renaming EVERY identifiable routine, not just
      opportunistically** (explicit user request: "keep going, don't
      stop until everything is renamed"). Working through the
      proven-only set (`sysrom_3532_3633.symbols.json`) ordered by
      reference count, highest first - 32 renamed as of the last
      session, ~234 left in the proven set alone (see
      `disasm/NOTES.md`'s dated session entries for the running list
      and confidence notes). The heuristic-only layer (tens of
      thousands more, across all 3 ROMs) is a much lower-confidence,
      much larger tail - realistic goal is "every proven-reachable
      routine named," not literally every heuristic placeholder.
      Mechanically: add `{address: "name"}` to
      `gen_disasm_x86.FUNCTIONAL_NAMES` (this is what actually makes
      the name show up in the `.lst`/`.asm`/`.symbols.json` outputs -
      editing `.symbols.json` directly gets overwritten on the next
      regenerate), add the matching `FUNCTIONS.md` entry, then
      regenerate everything (`gen_disasm_x86.py`,
      `gen_disasm_mainrom_heuristic.py`, `gen_source.py`,
      `gen_source_readable.py`) and re-verify byte-identical/length-
      matching before committing.
      ~~`SUB_E06B6`~~ - **done**: it's `update_menu_position`, a
      bounded index/cursor tracker for menu navigation that reads two
      front-panel button-state bytes (`[0x4E7]`/`[0x4E8]`) - see
      `FUNCTIONS.md`.
- [ ] Confirm whether the x87 (`fdiv` etc.) instructions mean there's a
      real 8087 math coprocessor in the design (plausible for a scope
      doing voltage/time calculations) — check against the service
      manual's parts list. Stronger evidence found: `convert_sample_
      value` (`0xF1001`) opens with a genuine `fmul` in the middle of
      otherwise ordinary compiled-C integer code (mixed with `mul32`/
      `sdiv32`), not part of any known decode-drift cluster - see
      `disasm/NOTES.md` "Found: the firmware's assert()/panic
      mechanism".
- [x] ~~Revisit the 3 backward loop/jmp branches flagged by
      validate_nasm.py as landing outside the mapped ROM window~~ —
      **done, it was a validator bug, not a real 8086 quirk**. An
      earlier session's "genuine IP-wraparound" conclusion was wrong;
      the actual cause was `near_target_addr()` masking the raw target
      offset to 16 bits before adding it to the segment base, which
      only matters when capstone represents a negative target as a
      huge sign-extended hex string. Fixed by removing that premature
      mask (matching `gen_disasm_x86.py`'s own approach, which had
      already resolved 2 of these 3 targets to real labels). All 3 now
      validate and convert cleanly - only 1 raw-`db` instruction (the
      x87 `fmul`) remains anywhere in the readable/aligned pipeline.
      See `disasm/NOTES.md` "IP-wraparound branches was a validator
      bug".

## Ongoing documentation goal

As routines are understood, build up (not just individual function
names but the bigger picture):
- user-facing flows and menu structure (what the front panel/CRT UI
  actually lets someone do, and how the code implements it)
- configuration options and where they're stored
- the full memory map (RAM regions, I/O port assignments, peripheral
  registers) — started in `MEMORY_MAP.md` (IVT, RAM segments, the comm
  ROM's now-confirmed address, the narrower `0x90000+` mystery, and the
  4 I/O ports seen so far); keep it updated as more segments/ports are
  identified
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

See `changes/` for a dated log of completed work per session.
