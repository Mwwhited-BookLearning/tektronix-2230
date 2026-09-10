# Active work

## Next up

- [ ] Widen code coverage beyond the current ~8% (10,360 of 131,072
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
- [ ] Figure out the comm/GPIB ROM's (`160-2998`) actual mapping or
      bank-switching scheme, and confirm whether it's driven by the
      same 8088 or a separate coprocessor on the option board.
- [ ] Investigate the 12 call targets that resolve into 0x80000-0x97000
      — below the mapped ROM window (0xE0000+) — likely RAM-resident
      overlay code we don't have a dump of.
- [ ] Start filling in `functional_name` fields in
      `disasm/sysrom_3532_3633.symbols.json` as routines are understood
      (e.g. `SUB_E06B6` looks like a small dispatch/switch function
      keyed on a low nibble, touching memory locations 0x1B50/0x1B51/
      0x1B18 — candidate for renaming once those locations' purpose is
      confirmed).
- [ ] Investigate the `fmul`/x87 instructions capstone reported in a
      few decoded spots — could indicate a real 8087 math coprocessor
      in the design (plausible for a scope doing voltage/time
      calculations), or a misdecode. Worth checking against the service
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
  registers) as it's discovered, not just the ROM address map already
  in `disasm/NOTES.md`
- peripherals: the A/D converter, front-panel controls, GPIB/RS-232
  hardware, and how the firmware talks to each

When a diagram would help (state machines, memory maps, menu trees),
embed PlantUML directly in the relevant markdown file rather than
building a separate image asset.

## Done

See `changes/` for a dated log of completed work per session.
