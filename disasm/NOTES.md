# Tektronix 2230 firmware reverse-engineering notes

## Confirmed facts

- **CPU: Intel 8088/8086, 16-bit real mode.** (Earlier guess of a 6809 was
  wrong — opcode-frequency heuristics on 6800-family were misleading.
  Confirmed by TekWiki (w140.com/tekwiki/wiki/2230) and by getting fully
  coherent, self-consistent x86 disassembly at the real reset vector.)
- Three ROMs, each shipped in (at least) two firmware revisions (`-13`,
  `-14`; TekWiki also references `-01`/`-15` revisions we don't have):
  - `160-2998-13/14.bin` — comm/GPIB-RS232 option board ROM (silkscreen
    "comm_rom_0", sockets U1243/U1343 on boards A23/A24). Not yet mapped
    into the main address space — appears to be paged/bank-switched in
    16KB windows (see below). Not handled by the x86 disassembler yet.
  - `160-3633-13/14.bin` — main system ROM, low half. Silkscreen
    "sys_rom_0", socket **U9109**, board A10. **Mapped at physical
    0xE0000-0xEFFFF.**
  - `160-3532-13/14.bin` — main system ROM, high half. Silkscreen
    "sys_rom_1", socket **U9110**, board A10. **Mapped at physical
    0xF0000-0xFFFFF.** Contains the CPU reset vector at 0xFFFF0
    (`ljmp 0xE00B:1` → physical 0xE00B1, inside the 3633 half).
  - Between `-13` and `-14`, `3532` and `3633` differ **only in their
    4-byte ID header** (byte-identical code otherwise). `2998` differs
    in two ~16KB-aligned regions — a real functional change to the comm
    ROM between revisions, not yet analyzed in x86 terms (still assumed
    6809/8-bit for that board — unconfirmed; the comm board may have its
    own separate, smaller CPU rather than sharing the 8088).
- Each ROM starts with a 10-byte self-ID header before the "Copyright
  (C) Tektronix..." banner: `[u16 chip-specific value][u16 BCD part
  number][u8 BCD revision][u8 one's-complement of revision][u8 0x00][u16
  size/load-address code]`.
- "Latest version" of the main system ROM pair = `160-3532-14.bin` +
  `160-3633-14.bin` (revision 14 is the higher BCD value of the two we
  have).

## Tooling

- `gen_disasm_x86.py` — recursive-descent x86-16 disassembler, seeded
  from three known entry points (reset vector, plus two far-jump targets
  discovered while tracing the reset path: `E5D1:B4`/`E5D1:C7`, an
  interrupt-vector-table setup + memory-clear routine). Only follows
  *resolved* control flow (direct near/far jmp, jcc, call/lcall);
  indirect (register/memory) jumps stop that walk rather than guessing.
  Outputs:
  - `sysrom_3532_3633.lst` — the disassembly listing.
  - `sysrom_3532_3633.symbols.json` — address → placeholder name, with
    empty `functional_name`/`notes` fields to fill in as we understand
    what each routine/label does. This is the file to edit when renaming
    things by intent instead of address.
- `gen_disasm.py` — the earlier (obsolete) naive-linear-sweep 6809
  attempt. Kept for history; not useful now that we know the real CPU.
  Safe to ignore/delete once the x86 tooling supersedes it.

## Current coverage

As of the first working recursive-descent pass: ~9,400 instruction-start
bytes reached out of 131,072 total ROM bytes (~7%), from just 3 seed
entry points. This is expected — most unreached bytes are either (a)
code only reachable via computed/indirect jumps we can't resolve
statically (e.g. jump tables), or (b) data (string tables, bitmaps,
constant tables) that hasn't been identified yet.

12 call targets resolve to addresses in the 0x80000-0x97000 range, well
below the mapped ROM window (0xE0000+) — likely calls into RAM-resident
overlay code we don't have a dump of, or the comm-board's address space.
Not yet investigated.

## Open questions / next steps

1. Widen code coverage: many routines are probably only reachable via
   jump tables (`SUB_E06B6`-style dispatch functions suggest the
   convention exists) — need to find and manually seed those table
   entries as additional entry points.
2. Identify data regions (ASCII string tables like the command-name
   list found in the comm ROM, the acquisition-mode-setup strings in
   3633) within the *reached* code so the listing doesn't try to
   disassemble them as instructions.
3. Figure out the comm ROM (2998)'s actual mapping/bank-switching
   scheme and confirm whether it's driven by the same 8088 or a separate
   coprocessor.
4. Start filling in `functional_name` in the `.symbols.json` files as
   routines are understood (e.g. `SUB_E06B6` looks like a small
   dispatch/switch function keyed on a low nibble — candidate name once
   confirmed: something like `set_mode_from_param` or similar, pending
   more context on the 0x1B50/0x1B51/0x1B18 memory locations it touches).
