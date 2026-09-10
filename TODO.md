# Active work

## Next up

- [ ] Widen code coverage beyond the current ~10% (13,510 of 131,072
      main-ROM bytes, now including the resolved comm-ROM alias). Jump
      tables don't appear to be the lever here (0 unresolved indirect
      jmp/call in reached code) - instead look for more entry points
      nothing in-graph calls directly, the way the 4 interrupt handlers
      were found (see disasm/NOTES.md "Interrupt vector table
      entries"). Candidates: read through `INT1_HANDLER`/
      `INT255_HANDLER_*`/`INT2_HANDLER_LATE` themselves for further
      vector installs, and look for other IVT-write patterns (different
      base registers than `bx`) the current tracer wouldn't catch.
      - data (string tables, bitmaps, constant tables) not yet
        identified within the reached regions is the other likely
        source of "unreached" bytes.
- [ ] **Identify the ~20 not-yet-named subsystem-test subroutines**
      called from the self-test dispatcher `SUB_E416F` (see
      `disasm/NOTES.md` "Found: the self-test dispatcher" for the full
      call list). This is probably the single best lever for matching
      the I/O ports in `MEMORY_MAP.md` to real peripherals (A/D
      converter, front panel, display, etc.) - each test routine likely
      exercises one specific piece of hardware. Start a `FUNCTIONS.md`
      entry for each as it's identified.
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
      (`SUB_E44F1`), not a generic memory-size probe. Checks a ROM-
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
- [ ] Start filling in `functional_name` fields in the `.symbols.json`
      files as routines are understood (e.g. `SUB_E06B6` looks like a
      small dispatch/switch function keyed on a low nibble, touching
      memory locations 0x1B50/0x1B51/0x1B18 — candidate for renaming
      once those locations' purpose is confirmed), and add an entry to
      `FUNCTIONS.md` for each so there's one human-readable index across
      all three ROMs instead of three separate JSON files.
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
- peripherals: the A/D converter, front-panel controls, GPIB/RS-232
  hardware, and how the firmware talks to each

When a diagram would help (state machines, memory maps, menu trees),
embed PlantUML directly in the relevant markdown file rather than
building a separate image asset.

## Done

See `changes/` for a dated log of completed work per session.
