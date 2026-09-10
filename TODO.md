# Active work

## Next up

- [ ] Widen code coverage beyond the current ~8% (10,389 of 131,072
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
- [ ] Figure out the comm/GPIB ROM's (`160-2998`) exact bank-switching
      scheme/window. CPU identity is now settled (same 8088, not a
      separate coprocessor — see `disasm/NOTES.md` "Is the comm ROM
      its own CPU?"), but which port/register selects it and how large
      the overlay window is are still unknown. Disassembly done
      (`160-2998-14.asm`, 20,180 instructions, NASM-validated) but
      confidence is lower than the main ROM's — entry points are
      heuristic (push-bp signature scan), not proven reachable. See
      `disasm/NOTES.md` "Comm ROM disassembly" for full detail.
- [ ] Identify the unidentified `~0x80000-0x97000` region referenced by
      far calls from BOTH the main ROM and the comm ROM (see
      `MEMORY_MAP.md`) — likely a shared service/API call table, either
      RAM-resident or a ROM we don't have a dump of.
- [ ] Narrow down what peripheral the I/O ports actually seen in code
      (`0x83`, `0xC4`, `0xD1`, and a DX-indexed range) correspond to —
      see `MEMORY_MAP.md` "I/O ports actually seen in code".
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
  registers) — started in `MEMORY_MAP.md` (IVT, RAM segments, the
  unidentified shared `~0x9470`-area region, and the 4 I/O ports seen
  so far); keep it updated as more segments/ports are identified
- peripherals: the A/D converter, front-panel controls, GPIB/RS-232
  hardware, and how the firmware talks to each

When a diagram would help (state machines, memory maps, menu trees),
embed PlantUML directly in the relevant markdown file rather than
building a separate image asset.

## Done

See `changes/` for a dated log of completed work per session.
