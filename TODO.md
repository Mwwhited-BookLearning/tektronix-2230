# Active work

## Next up

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
- [ ] **Identify the ~20 not-yet-named subsystem-test subroutines**
      called from `self_test_dispatcher` (see `disasm/NOTES.md` "Found:
      the self-test dispatcher" for the full call list). Already tried
      matching them directly to the diagnostic message strings in
      `STRINGS.md` (`HS_ACQ`, `COMM_ROM`, `SYS_RAM`, etc.) by searching
      for code references - found none; they likely return a status
      code that `SUB_E094B` logs rather than printing a message
      directly (see `FUNCTIONS.md`). Next: find where `SUB_E094B`'s
      logged results actually get displayed/printed - that routine is
      more likely to hold the string-to-test mapping. This is probably
      the single best lever for matching the I/O ports in
      `MEMORY_MAP.md` to real peripherals. Add a `FUNCTIONS.md` entry
      (and wire a name into `gen_disasm_x86.FUNCTIONAL_NAMES`) for each
      as it's identified.
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
- [ ] Handle the instruction kinds currently excluded from NASM
      conversion (safe raw-`db` fallback, not a correctness problem,
      just lower fidelity in the buildable `.asm` sources): x87 FPU
      instructions (`fdiv` etc. - capstone's `st(N)` operand syntax
      needs translating for NASM) and the rare `insw`/`outsw`/`outsd`/
      `bound` instructions. See `disasm/NOTES.md` "Comm ROM
      disassembly" for where these showed up.
- [ ] Investigate the 2 `push` instructions with a stray `0x67` prefix
      that `validate_2998.py` flagged as real mismatches, landing right
      at the start of page 1's header/copyright text - almost certainly
      decode drift into data at the deepest heuristic reach, but worth
      a quick look to confirm.
- [ ] Narrow down what peripheral the I/O ports actually seen in code
      (`0x83`, `0xC4`, `0xD1`, and a DX-indexed range) correspond to —
      see `MEMORY_MAP.md` "I/O ports actually seen in code".
- [ ] Keep renaming routines as they're understood: add the address to
      `gen_disasm_x86.FUNCTIONAL_NAMES` (this is what actually makes
      the name show up in the `.lst`/`.asm`/`.symbols.json` outputs -
      editing `.symbols.json` directly gets overwritten on the next
      regenerate) and add the matching entry to `FUNCTIONS.md`. Next
      candidate: `SUB_E06B6` looks like a small dispatch/switch
      function keyed on a low nibble, touching memory locations
      `0x1B50`/`0x1B51`/`0x1B18` — needs those locations' purpose
      confirmed first.
- [ ] Confirm whether the x87 (`fdiv` etc.) instructions mean there's a
      real 8087 math coprocessor in the design (plausible for a scope
      doing voltage/time calculations) — check against the service
      manual's parts list.
- [ ] Revisit the 4 backward loop/jmp branches flagged by
      `validate_nasm.py` as landing outside the mapped ROM window when
      resolved through their segment — likely a sign this specific
      code area is misaligned/misdecoded (walked into data).

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
