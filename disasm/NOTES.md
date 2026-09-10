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
- `validate_nasm.py` — round-trip validator. Converts every decoded
  instruction to NASM syntax, assembles each in its own `section ...
  vstart=<real address>` (packed sequentially in the output so one
  instruction's encoded length can't shift/cascade into the next one's
  comparison), and diffs against the original ROM bytes. Requires a
  NASM executable path as its one argument (NASM isn't vendored in the
  repo - see "Validation status" below for how to get one).
- `gen_source.py` — emits the actual committed disassembly:
  `160-3633-14.asm` and `160-3532-14.asm`, one real buildable NASM
  source file per chip. Decoded instructions proven byte-exact by
  `validate_nasm.py` are written as real instructions; everything else
  (alt-encoding-only matches, unconverted instructions, and every
  byte the recursive descent hasn't reached yet) is emitted as raw
  `db` bytes grouped 16-per-line, with the original mnemonic in a
  comment where known. This means **the .asm files always reassemble
  byte-identical to the original .bin** regardless of how much is
  understood yet - `gen_source.py` verifies this itself on every run
  (assembles its own output and diffs against the source .bin). ASCII
  string runs get a comment showing the decoded text. Also takes a
  NASM executable path as its one argument. These .asm files are the
  ones to open when reading or editing "the disassembly" - `.lst` is
  a secondary diagnostic view with extra formatting `.asm` doesn't
  have (physical addresses, chip:offset pairs).

## Is the comm ROM its own CPU?

Own ROM chips, own physical board (A23/A24 vs. the main board A10) —
but **not its own CPU**. Evidence:

- Both 16KB pages at file offset `0x8000` and `0xC000` in `160-2998`
  open with byte `0xEA` (x86 far-JMP), decoding cleanly as
  `ljmp 0xE64C:0000` → physical `0xE64C0` — which falls *inside* the
  already-confirmed main-ROM window (`0xE0000-0xEFFFF`). A genuinely
  separate CPU with its own private address space would have no reason
  to reference an address inside the other board's ROM.
- The classic C-compiler prologue `55 8B EC` (`push bp; mov bp,sp`)
  appears **398 times** across all four 16KB pages (121/102/84/91 per
  page) — real compiled functions, same style (BP-based frames, `retf`/
  `retf N` far returns) as the main ROM, consistent with the same
  toolchain having built both.
- Comm-ROM code makes far calls like `lcall 0x9470, 0xE` (physical
  `0x9470E`) — landing in the *same* unidentified `~0x80000-0x97000`
  region that the main ROM's code also calls into (see `MEMORY_MAP.md`).
  Two independently-addressed CPUs coincidentally referencing the same
  absolute address for what looks like a shared service-call table
  would be a bizarre coincidence; far more likely this is one shared
  bus/address space with a documented low-level API both the main
  firmware and any plug-in option board call through.
- Reset vector check: confirmed absent. Both `160-2998-13.bin` and
  `-14.bin`'s last 16 bytes (physical `0xFFF0-0xFFFF`, where an 8088
  always starts fetching after reset) are unprogrammed `0xFF` — this
  ROM was never a candidate to be the boot ROM, consistent with it
  being a bank-switched overlay that only gets paged in once the main
  firmware's boot code decides to.

Still unknown: the exact bank-switch mechanism (which port/register
selects this ROM into the address space, and what window size).

**Update: there is no bank-switch mechanism - see the next section.**

## The comm ROM is NOT bank-switched

Corrected after the user pointed out the obvious question: with a 1MB
(20-bit) address space and only ~192KB of ROM plus modest RAM in use,
why would anyone bank-switch a 64KB device when there's plenty of free
address space to just give it a fixed home? There wasn't a good
answer - the "bank-switching" framing above came from over-reading the
repeated per-page headers as evidence of paging, when it wasn't.

**Confirmed: `160-2998` is a plain 64KB device at a fixed physical
address, `0x80000-0x8FFFF`** - the exact same simple pattern as the
two main-ROM halves at `0xE0000`/`0xF0000`. Verified two ways:

1. Every far-call target (from both the main ROM's already-proven code
   and the comm ROM's own code) landing in `0x80000-0x8FFFF` was
   checked against this file's own `55 8B EC` push-bp signatures at
   `file_offset = phys - 0x80000`: **82 of 84 distinct targets land
   exactly on a known function start** (the other 2 likely just hit a
   leaf function using a different prologue).
2. The main ROM's own already-proven-reachable code makes several
   direct far calls straight into that range (e.g. `160-3633:0x1f03`
   calls `8013:0003` → physical `0x80133`) - this isn't a heuristic
   result, it's PROVEN reachability from the confirmed boot path.

This also resolved what had been called "the mystery
`~0x80000-0x97000` region" - the lower half of it (`0x80000-0x8FFFF`)
*is* the comm ROM we already have a full dump of; it was only ever
mysterious because of the wrong (arbitrary bookkeeping) addresses this
project had been using for it. The real, still-unresolved mystery is
narrower now: `0x90000` and up (see `MEMORY_MAP.md`).

`gen_disasm_x86.CHIPS` now includes `"2998"` (`phys_base=0x80000`)
directly, exactly like `"3633"`/`"3532"` - no more special-cased
virtual-page-addressing module needed. `gen_disasm_2998.py` is now
just a thin heuristic-entry-point supplement (the 398 push-bp
signatures + the 2 internal boot-stub jumps) layered on top of the
same real address space, and `gen_source_2998.py` regenerates
`160-2998-14.asm` with those included (while leaving the main ROM's
`.asm` files as proven-only, via `gen_source.main()`'s new
`only_chips` parameter, so heuristic confidence doesn't leak into
files that are supposed to be proven-reachable-only).

Reaching further into the main ROM through these newly-resolved comm-
ROM call sites surfaced a few instruction kinds the NASM converter
hadn't seen yet: x87 FPU instructions (`fdiv`, etc. - capstone's
`st(N)` operand syntax isn't NASM-compatible as written) and some rare
string-I/O/bounds-check instructions (`insw`/`outsw`/`outsd`/`bound`).
Rather than get their exact NASM syntax right immediately, these are
conservatively excluded from conversion (fall back to raw `db`, same
safe-by-default principle as everywhere else) - see `TODO.md`. Also
found and fixed one more real encoding ambiguity while re-validating:
opcode `0x82` is an undocumented exact duplicate of `0x80` for byte-
sized group-1 immediate ops (the sign-extend bit that distinguishes
them doesn't mean anything for an 8-bit destination) - NASM always
emits `0x80`, so this needed the same kind of alternate-encoding
detection as the direction-bit and displacement-width ambiguities.

## Comm ROM disassembly (160-2998-14)

`160-2998` (`phys_base=0x80000`) is now registered directly in
`gen_disasm_x86.CHIPS`, exactly like `"3633"`/`"3532"` - see "The comm
ROM is NOT bank-switched" above. Running `gen_disasm_x86.py` alone
already reaches some of it (99 instructions) through PROVEN control
flow: real far calls from already-confirmed main-ROM code (this is
included in the official `sysrom_3532_3633.lst`/`.symbols.json`, no
separate confidence tier needed for these 99).

`gen_disasm_2998.py` adds a second, lower-confidence layer on top:
every occurrence of the `55 8B EC` (`push bp; mov bp,sp`) C-compiler
prologue (398 of them) plus this ROM's own two internal boot-stub far
jumps, seeded as additional entry points in the *same* real address
space (no per-page virtual addressing needed any more - that
machinery is gone now that the mapping is confirmed flat). This is a
strong signal but not proof of reachability the way the main ROM's
recursive descent is - one still-open thread: 2 far calls found landing
in `0x80000-0x8FFFF` didn't hit an exact function start under this
scan (see the "82/84" figure above), meaning either a different
prologue style or a genuine decode drift into data at that spot.

One high-confidence discovery from the earlier (pre-correction) pass
remains valid: the boot-stub far-jump target (`0xE64C:0000`) is
directly observed (both comm-ROM pages 2 and 3 independently encode
the identical jump) and is promoted into the official
`gen_disasm_x86.py` `ENTRY_POINTS` as `COMM_ROM_BOOTSTUB_TARGET`.

Result with the heuristic layer included: 20,446 instructions reached,
NASM-validated (`validate_2998.py`): 18,290 exact + 2,151 alt-encoding,
2 real mismatches (both `push` instructions with a stray `0x67`
address-size-override prefix, landing right where page 1's header/
copyright text starts - almost certainly decode drift into data at
the deepest heuristic reach, not a validator problem), 3 not
converted. A buildable NASM source (`160-2998-14.asm`, via
`gen_source_2998.py`) reassembles byte-identical to the original .bin
regardless, same guarantee as the main ROM's `.asm` files - real
mismatches/unconverted instructions just fall back to raw `db`.

Chasing validation failures across both correction passes surfaced
several more real bugs/gaps in `validate_nasm.py`, all now fixed:
- a segment-override prefix byte (`0x26`/`0x2E`/`0x36`/`0x3E`) was
  being read as if it were the instruction's opcode, breaking the
  immediate-width-widening check for segment-overridden ALU ops with
  a memory destination (e.g. `or word [es:di+0xA], imm`).
- capstone omits the `0x` prefix on some near-branch targets too (not
  just far ones) - an unprefixed target like a bare `"9"` was falling
  through to a bogus literal-address conversion instead of being
  resolved or safely rejected.
- x87 FPU instructions (`fdiv`, etc.) and a few rare instructions
  (`insw`/`outsw`/`outsd`/`bound`) surfaced once coverage reached
  further via the corrected comm-ROM mapping; conservatively excluded
  from conversion (raw `db` fallback) rather than risk getting their
  NASM syntax subtly wrong - `TODO.md` has this as a follow-up.
- opcode `0x82` is an undocumented exact duplicate of `0x80` (byte-
  sized group-1 immediate ops - the sign-extend bit that distinguishes
  them at 16/32-bit doesn't mean anything for an 8-bit destination);
  NASM always emits `0x80`, so this needed the same kind of alternate-
  encoding detection as the direction-bit/displacement-width cases.

`gen_source.py`'s `main()` gained an `only_chips` parameter for this
work: the comm ROM's heuristic entries can open up new PROVEN-reachable
code in the main ROM chips too (real code, discovered via a real call
path), but writing that into `160-3633-14.asm`/`160-3532-14.asm` would
silently mix heuristic-derived confidence into files that are supposed
to be proven-reachable-only. `gen_source_2998.py` now passes
`only_chips=["2998"]` so only the comm ROM's file picks up the wider
heuristic-assisted view; the main ROM's `.asm` files stay proven-only,
regenerated separately via `gen_source.py`'s default entry set.

## The 0x90000-0x97FFF region is fully resolved: an address-decode alias

**Not RAM, not a missing chip - it's the comm ROM's own upper half
(`0x88000-0x8FFFF`, file offset `0x8000-0xFFFF`, pages 2-3) appearing a
second time at a different physical address.** The user asked "would
the pointers into `0x90000-0x97FFF` make sense if they were a shadow
of `0x80000-0x8FFFF`?" - a plain `+0x10000` shift didn't fit (1/82
targets matched), but brute-forcing every possible base offset against
all 82 observed far-call targets found `base=0x88000` gives **82/82
exact matches**. So: `phys - 0x88000 == file_offset` for everything in
this range, meaning physical address `P` (for `0x90000 <= P <
0x98000`) is electrically the same location as physical address
`P - 0x8000` (which already falls inside the confirmed comm-ROM
window). This is a classic partial/incomplete address-decode artifact
- the comm ROM's chip-select logic most likely only compares a subset
of the high address lines, so it responds (aliases) across a wider
range than its "official" 64KB window. Nothing to reverse-engineer
further here; these are bytes we already have a complete dump of.

**General technique worth reusing for any future "why does this
address not resolve" puzzle**: don't assume a single fixed offset -
brute-force every candidate base against the full set of observed
targets and take whichever one maximizes exact matches against known
function-start (or other) signatures. A single spot-check can miss a
non-obvious offset; scoring all of them at once found this instantly.

## Found: the option-board presence/RAM-detection routine

Tracing why the `0xAA55` pattern gets tested (originally read as a
generic RAM-size probe) led to a much more specific and useful answer,
prompted by two user questions: "might this be a check for whether the
comm option card is installed?" and "what if it's RAM or memory-mapped
I/O on the comm board?" **Both are confirmed correct, and it's one
function doing both:**

`check_comm_option_installed` (`160-3633`) hardcodes checking `ES:DI = 0x8000:0x0004` -
physical `0x80004`, which is exactly bytes 4-5 of the comm ROM's own
10-byte self-ID header (the BCD-revision byte + its one's-complement
byte, from the header format decoded on day one of this project). The
function:
1. Reads that word, splits it into high/low bytes, and checks whether
   they sum to exactly `0xFF` - i.e., re-derives the same
   value+complement checksum every one of these ROMs' headers uses.
   This is a **"is a genuine Tektronix ROM header present here"**
   check, not a generic memory probe.
2. Only if that passes does it run the save/write-`0xAA55`/verify/
   restore sequence - **not against the same address**, against
   `ES:[DI+0xA000]` (`DI` is unchanged from the checksum read, still
   `4`), i.e. physical `0x8A004` - a *different* word 0xA000 bytes
   further into the same 64KB comm-board window. Testing whether
   *that* location is writable, i.e., whether the option board
   provides RAM/memory-mapped I/O somewhere in its address space in
   addition to its ROM. Why that specific offset was chosen isn't
   confirmed - one plausible explanation is a small dedicated
   presence-detect scratch register on the option board, decoded by
   the board's own sub-decode logic to occupy just that one word
   while everything else in the window reads as ROM (this would also
   explain why our raw EPROM dump shows ordinary code/data there: a
   dump reads the physical EPROM chip directly, not "as seen by the
   CPU with the board installed and its sub-decode active").
3. Packs the result into a status byte (`[0x1BF9]`): one bit for "valid
   header found," a second bit (only set if the RAM-write also stuck
   *and* another config byte at `[0x1B83]` reads `0x1E`) for "and it
   has working RAM/IO too."

So the comm/GPIB option board's presence detection and its RAM/IO
presence detection are the same probe, just gated in sequence - exactly
matching the "Function requires options not installed in this
instrument" message string found in `160-3532` back at the very start
of this project.

## Found: the self-test dispatcher

`check_comm_option_installed` (above) isn't called on its own - it's one of a long chain
of calls inside `self_test_dispatcher` (`160-3633`), which is a **self-test
dispatcher**: roughly 25+ calls to individual subsystem-test
subroutines in a row, most immediately followed by `or word [bp-0xA],
ax` (folding that test's return code into an accumulating result
word) and `mov word [0x1B18], 1` (a status/progress flag, exact
meaning tbd). `check_comm_option_installed`'s option-detection call is one link in this
chain but notably does NOT get OR'd into the same accumulator the way
its neighbors do - consistent with "is an option installed" being
informational rather than a pass/fail test that could error out.

`self_test_dispatcher` is itself gated: called from `160-3633:0x07F8`, guarded by
`cmp word [0x1B10], 0 / jne <skip>` - so it only runs when some flag at
`0x1B10` is zero (candidate meanings: "self-test not yet run this
power-cycle," or "not in some other mode" - not yet confirmed).

The sibling test subroutines called from `self_test_dispatcher` (in call order,
not yet individually identified - good next targets, since matching
each to a real peripheral would meaningfully advance the "what
peripheral do these I/O ports belong to" question in `MEMORY_MAP.md`):
`SUB_E374E`, `SUB_E3821`, `SUB_E0AF5` (called twice), `SUB_E3F2C`,
`SUB_E3F99`, `SUB_E2FC8`, `SUB_E1B16`, `SUB_E252A`, `SUB_E0ADD`,
`SUB_E0DCC`, `SUB_E0E56`, `SUB_E28FE`, `SUB_E227E`, `SUB_E26D6`,
`SUB_E286C`, `SUB_E2CEC`, `SUB_E0FD0`, **`check_comm_option_installed`** (comm/GPIB
option detect, now identified), `SUB_E16EA`, `SUB_E1E3E`, `SUB_E1D28`,
`SUB_E1DB3`, `SUB_E1E90`, `SUB_E1F18`, then a few calls to `SUB_E553B`
and `SUB_E6D2F`/`SUB_E4429` that look like they might be outside the
main per-subsystem-test loop (end-of-sequence cleanup/reporting?).

Variables seen so far associated with this self-test machinery (roles
inferred from usage, not confirmed):
- `[0x1B10]` - gates whether `self_test_dispatcher` (the whole dispatcher) runs at
  all this call.
- `[0x1B18]` - written `1` after nearly every individual test call;
  exact role (progress indicator? "last test index"? always the
  literal `1`, so maybe not an index) not yet confirmed.
- `[bp-0xA]` (a caller-local, not a fixed address) - accumulates OR'd
  return codes from each test into an overall self-test result.
- `[0x1BF9]` - the option-board-presence/RAM status byte set by
  `check_comm_option_installed` specifically (bit 1 = valid header found, bit 2 = also
  RAM/IO-backed).
- `[0x1B83]` - a config byte `check_comm_option_installed` checks equals `0x1E` as part
  of confirming the RAM/IO result; role otherwise unconfirmed.

## The 0x90000+ region: fully resolved (see above)

This used to be a substantial open question ("`~0x80000-0x97000`,
likely RAM, contents unknown"). It's now completely closed - see "The
0x90000-0x97FFF region is fully resolved: an address-decode alias"
above. Short version: it's not a separate region at all, it's the same
comm-ROM bytes we already have, visible at a second physical address
because of how the chip-select logic decodes address lines. Nothing
further to chase here. (The `0xAA55` pattern that originally suggested
"RAM test" turned out to be part of the option-presence/RAM-detection
routine documented above too, not a generic memory-size probe.)

## Interrupt vector table entries (real code entry points)

The reset routine and two later routines (`SUB_E5E53`, inside what's
now `INT255_HANDLER_EARLY`'s neighborhood) install real 8086 interrupt
vectors by writing directly into low memory with `es=0` (or `es=0x3F`,
whose paragraph base `0x3F0` still lands inside the 1KB IVT that spans
physical `0x000-0x3FF`). Found by tracing `mov <reg>, imm` / `mov
es,<reg>` / `mov word [es:bx(+disp)], <reg>` triples through the
already-decoded code:

| IVT slot (int #) | Handler installed | When |
|---|---|---|
| 0x008 (INT 2, NMI) | `E5D1:0057` (`SUB_E5D67`) | at reset |
| 0x004 (INT 1, trap) | `E5D1:019D` (`INT1_HANDLER`) | at reset |
| 0x3FC (INT 255) | `E5D1:0090` (`INT255_HANDLER_EARLY`) | at reset |
| 0x3FC (INT 255) | `E60B:0005` (`INT255_HANDLER_LATE`) | later, overwrites the above |
| 0x008 (INT 2, NMI) | `E60B:003A` (`INT2_HANDLER_LATE`) | later, overwrites the reset-time NMI handler |

These 4 non-`SUB_E5D67` handlers are entry points nothing in the
program's direct call graph would ever reach (only the corresponding
interrupt firing calls them), so they were added to
`gen_disasm_x86.py`'s `ENTRY_POINTS` list and seeded directly. This
raised coverage from 9,369 to 10,360 instructions. Re-running the same
vector-tracing search against the newly-reached code found no further
handlers - this specific lead is exhausted for now, though other
interrupt-installing sites may still exist in code we haven't reached
by other means.

INT 1 (trap/single-step) being explicitly handled is a little unusual
for shipped production firmware and might be diagnostic/self-test
infrastructure left in; INT 2 (NMI) and INT 255 (a software-only
vector, not a CPU exception) getting reprogrammed mid-run suggests the
firmware switches between distinct operating states (e.g. a
diagnostic/POST mode vs. normal-run mode) with different NMI/software-
interrupt handling per state - worth confirming once
`INT255_HANDLER_EARLY`/`_LATE` and `INT2_HANDLER_LATE` are read closely.

## Validation status

Ran the recursive-descent output (10,488 decoded instructions - main
ROM plus the 99 comm-ROM instructions now proven reachable once
`160-2998` was registered at its confirmed real address, `0x80000`)
through `validate_nasm.py`: **9,480 byte-exact, 1,003 provably-
equivalent alternate encodings, 0 real mismatches, 5 not independently
checked.**
This is strong evidence the x86 decode itself (mnemonic, operands,
instruction length) is correct throughout what's been reached so far —
it does NOT validate the code-vs-data classification (whether a given
address *should* be treated as code at all), only that wherever we did
treat something as an instruction, the decode is right.

The "alternate encoding" bucket is large (~10% of instructions) because
x86 has several genuinely ambiguous encodings where two different byte
sequences produce identical CPU behavior, and NASM's default encoder
doesn't always pick the same one this ROM's original assembler did:
- register-to-register ALU ops/MOV: the ModRM "direction" bit can go
  either way (e.g. `mov bp,sp` as `8B EC` or as `89 E5`) for the same
  effect - detected by flipping the direction bit and reg/rm subfields
  in Python and re-comparing, rather than trusting NASM to reproduce it.
- immediate width: `cmp ax, 1` can be the 3-byte AX-specific opcode
  (`3D 01 00`, always 16-bit immediate) or the generic group-1 form
  sign-extended from a byte (`83 F8 01`) - NASM defaults to the
  shorter sign-extended form, so the converter checks the *original*
  opcode byte and adds a `strict word` qualifier only when the ROM
  actually used the wider encoding.
- displacement width: `[bx+0x68]` can be encoded with an 8-bit or
  16-bit displacement when the value fits in a signed byte; the ROM
  consistently used the wider disp16 form where NASM would default to
  disp8, so this is detected the same way (recompute the alternate
  encoding in Python, compare against NASM's actual output).
- capstone names the 16-bit `CBW`/`CWD` opcodes after their 32-bit
  (386+) identities (`cwde`/`cdq`) regardless of mode; taken literally
  under `BITS 16` that makes NASM add a spurious `0x66` operand-size
  prefix, so the converter renames them back to the 16-bit mnemonics.

The 4 not-independently-checked instructions are backward `loop`/`jmp`
branches whose capstone-reported target, once resolved through the
current segment, lands outside either chip's mapped 64K window - a
signal that these specific spots may be misaligned/misdecoded (walked
into data) rather than a validator limitation, so they're deliberately
left unverified rather than force-converted. Worth revisiting if code
coverage expands into that area.

## Current coverage

10,488 instruction-start bytes reached out of 131,072 total main-ROM
bytes (~8%), from 8 seed entry points (reset vector, 2 discovered
while tracing the reset path, 4 interrupt handlers found via IVT-write
tracing, and the comm-ROM boot-stub target - see above). This is
expected to still be a small fraction —
most unreached bytes are either (a) code only reachable via computed/
indirect jumps we can't resolve statically (e.g. jump tables - though
notably **zero unresolved indirect jmp/call instructions exist in the
code reached so far**, suggesting this codebase may prefer cmp+je
dispatch chains over jump tables, at least in what's been seen), or
(b) data (string tables, bitmaps, constant tables) that hasn't been
identified yet.

12 call targets resolve to addresses in the 0x80000-0x97000 range, well
below the mapped ROM window (0xE0000+) — likely calls into RAM-resident
overlay code we don't have a dump of, or the comm-board's address space.
Not yet investigated.

## Open questions / next steps

1. Widen code coverage further. Jump-table dispatch doesn't appear to
   be in use in what's been reached (0 unresolved indirect jmp/call
   sites), so the next lever isn't jump tables - it's finding more
   entry points nothing in-graph calls directly (interrupt handlers,
   as above, are one source; worth checking for others e.g. timer
   ISRs, or simply reading through `INT1_HANDLER`/`INT255_HANDLER_*`/
   `INT2_HANDLER_LATE` themselves, which may install further vectors).
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
