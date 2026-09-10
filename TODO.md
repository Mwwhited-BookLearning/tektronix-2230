# Active work

## Next up

- [ ] Widen code coverage beyond the current ~8% (10,488 of 131,072
      bytes) of the main ROM pair. Jump tables don't appear to be the
      lever here (0 unresolved indirect jmp/call in reached code) -
      instead look for more entry points nothing in-graph calls
      directly, the way the 4 interrupt handlers were found (see
      disasm/NOTES.md "Interrupt vector table entries"). Candidates:
      read through `INT1_HANDLER`/`INT255_HANDLER_*`/`INT2_HANDLER_LATE`
      themselves for further vector installs, and look for other
      IVT-write patterns (different base registers than `bx`) the
      current tracer wouldn't catch.
      - data (string tables, bitmaps, constant tables) not yet
        identified within the reached regions is the other likely
        source of "unreached" bytes.
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
- [ ] Figure out what's really at `0x90000` and up (see `MEMORY_MAP.md`
      "Unidentified / open" and `disasm/NOTES.md` "The 0x90000+ region").
      What used to look like a `0x80000-0x97FFF` mystery region turned
      out to be mostly the comm ROM itself (see above); the genuinely
      open part is narrower now but still real - dozens of far-call
      targets from the comm ROM's own code land in `0x90000-0x97C95`
      and aren't covered by any ROM dump we have. Checked 2 of 5 call
      sites of a generic memcpy utility (`SUB_FBC09`) looking for a
      load-into-that-region copy; found nothing yet — check the other
      3, and look for non-memcpy loading mechanisms (GPIB/RS-232
      download, runtime code generation). May be a hard ceiling on
      what's recoverable from static analysis alone if it's RAM
      populated only at runtime.
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
- [ ] Start filling in `functional_name` fields in
      `disasm/sysrom_3532_3633.symbols.json` as routines are understood
      (e.g. `SUB_E06B6` looks like a small dispatch/switch function
      keyed on a low nibble, touching memory locations 0x1B50/0x1B51/
      0x1B18 — candidate for renaming once those locations' purpose is
      confirmed).
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
- peripherals: the A/D converter, front-panel controls, GPIB/RS-232
  hardware, and how the firmware talks to each

When a diagram would help (state machines, memory maps, menu trees),
embed PlantUML directly in the relevant markdown file rather than
building a separate image asset.

## Done

See `changes/` for a dated log of completed work per session.
