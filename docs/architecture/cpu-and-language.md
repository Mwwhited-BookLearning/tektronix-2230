# CPU identity and source language

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

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
    **Confirmed same CPU/address space, not a separate coprocessor** —
    see "Is the comm ROM its own CPU?" below.
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

## What language was this written in?

**Not hand-written assembly - this is a compiled higher-level
language, and the evidence points to C specifically.**

Evidence it's compiled, not hand-assembled:
- Every function uses the *exact same* prologue/epilogue boilerplate
  (`push bp; mov bp,sp; sub sp,N` / `mov sp,bp; pop bp; retf [N]`)
  regardless of whether that specific function is complex enough to
  need it - a human writing assembly directly would skip unnecessary
  frame setup for simple functions; a compiler emits it mechanically
  for every function using the same code-generation template.
- Repeated, near-identical function pairs differing only in one
  immediate value (an enable/disable-style pair found in the comm ROM)
  - the signature of two instantiations of the same source template,
    not two independently hand-written routines.
- `les`/`lds far-pointer` loads from static storage throughout - this
  idiom (and the very concept of "far pointer" as a first-class thing
  distinct from a plain offset) is how segmented-memory C compilers of
  this era (Microsoft C, Turbo C/Borland, and various embedded 8086
  cross-compilers) represented the `far` pointer qualifier, not
  something someone would introduce by hand without a reason.

Evidence pointing at **C** specifically, over Pascal or something else
(see `STRINGS.md` for the full string catalog this is drawn from):
- **Printf-style format strings.** The main ROM's HPGL plotter output
  code uses literal `%c`/`%d` format specifiers embedded in string
  literals (e.g. `"PU%d,%d;"`, `"%cC%c%c"`) feeding what's clearly a
  `sprintf`-family call. This is C's standard-library convention
  specifically - Pascal's formatted output (`Write(x:5)`-style) doesn't
  work this way at all. This is the single strongest piece of evidence.
- **Null-terminated string literals** throughout both main-ROM images
  (147/169 and 149/171 of all strings found end in a `0x00` byte - see
  `STRINGS.md`). C's defining string representation; Pascal's native
  string type doesn't naturally produce a trailing null unless the
  programmer deliberately uses a zero-terminated array type instead of
  the language's normal string type.
- The comm ROM's GPIB command-keyword table uses **length-prefixed**
  strings instead (131/185 of its strings have a length byte
  immediately before them, matching exactly - classic Pascal
  `ShortString` style). This initially looked like it might argue for
  Pascal, but it's much better explained as a deliberately hand-built
  lookup-table format for fast command parsing (read length byte,
  compare that many characters) - a technique any language can use for
  a specific data structure, independent of the compiler's own native
  string type. It doesn't contradict the C conclusion; it's a
  different, purpose-built encoding for one specific table.
- The consistent `retf N` (callee cleans up N bytes of stack) calling
  convention seen throughout is sometimes called "the Pascal calling
  convention" precisely because Pascal defaults to it - this initially
  looked like Pascal evidence too. But several 1980s C compilers
  targeting the 8086 supported (and some projects deliberately chose)
  callee-cleanup calling conventions as a code-size optimization for
  embedded work, where every byte of ROM mattered and the caller-side
  cleanup `add sp,N` that C's default `cdecl` convention requires at
  *every* call site adds up. Given the much stronger, harder-to-fake
  evidence above (printf-style formatting, null-terminated general
  strings), this is more likely explained as a deliberate size
  optimization in a C codebase than as genuine Pascal source.

Net assessment: **compiled C**, cross-compiled for the 8088/8086 with
a `far`-pointer-aware, segmented memory model, using a non-default
(callee-cleanup) calling convention throughout - plausibly a deliberate
code-size choice for an embedded instrument. Not certain (no compiler
banner or debug info survives in these ROMs to confirm a specific
toolchain), but the printf-style format strings in particular are hard
to explain any other way.
