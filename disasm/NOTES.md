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

## Found: the actual source of [0x1B83] (detect_comm_option_hw)

`[0x1B83]` has been checked (`==0x1E`) by `check_comm_option_installed`
and `self_test_dispatcher` since early sessions, but where it got its
value was never traced - until `detect_comm_option_hw` (`0xE75C0`)
turned up, called from `run_selftest_sequence`'s setup. It's a
**second, independent hardware probe** for the comm option board,
separate from `check_comm_option_installed`'s ROM-header-checksum
check:

1. Writes `0` to physical `0x40000+0x7DE`.
2. Reads bit `0x1000` of a word at physical `0x40000+0x377E`. If
   already set, `[0x1B83] = 0x1E` and done.
3. Otherwise, writes `1` to `0x40000+0x7DE` (a write-then-readback
   probe - the same idea as `check_comm_option_installed`'s `0xAA55`
   test, just at different addresses) and re-reads `0x40000+0x377E`'s
   bit `0x1000`. If it's now set, `[0x1B83] = 0x14`. If still clear,
   `[0x1B83] = 0x1E`.

**Not fully resolved**: which value specifically means "installed".
`0x1E` results from *two* branches with opposite-looking conditions -
bit already set (no write needed) **or** bit still clear even after
the write probe - while `0x14` is the one case where the write
visibly changed the read-back bit. This doesn't fit a simple binary
"present/absent" flag cleanly. Plausible readings: `0x1E` might be a
safe default used whenever the probe *doesn't* need the write-trigger
to succeed (either because the board was already responding, or
because nothing is there to respond even after trying), while `0x14`
marks a specific "board present, but only detected via the write
probe" case - possibly distinguishing a hardware revision or a board
that needs an explicit initialization poke before it reports itself.
Worth revisiting once more of what reads `[0x1B83]` (beyond just
`==0x1E`) is understood - if some code also checks `==0x14`
specifically, that would clarify the distinction.

## Found: the self-test dispatcher

**CORRECTION (this session): `self_test_dispatcher` was misnamed.**
Tracing what actually calls `SUB_E094B` (see next section) led to
reading `0xE416F` (`160-3633`) end-to-end for the first time instead
of just skimming its call list, and it turned out to contain **zero**
OR-fold test calls - it's entirely print/banner setup: `SUB_E3567`
(clear/init a line buffer region), `SUB_E3930`, `SUB_E3854`, a helper
that prints two far-pointer strings (`SUB_E4217`, itself calling
`SUB_E3821` then `print_string_far` twice), then the mirror-image
sequence again, ending with `mov word [0x1B10], 3` and `retf`. This
routine is now renamed `print_selftest_banner`.

The *actual* self-test dispatcher - the one with the ~14-call
OR-fold-into-`[bp-0xA]` pattern originally described here - is the
**next function**, `0xE4244`, called from a completely different
place (`160-3633:0x3DEE`, not from `print_selftest_banner` at all).
It's now renamed `self_test_dispatcher` (taking over the name from the
routine above). `check_comm_option_installed`'s option-detection call
is one link in this chain but notably does NOT get OR'd into the same
accumulator the way its neighbors do - consistent with "is an option
installed" being informational rather than a pass/fail test that
could error out. One test (`SUB_E252A`) is conditionally skipped based
on `[0x1B83]==0x1E` - the same "comm option RAM/IO confirmed" config
byte `check_comm_option_installed` checks, so this dispatcher already
adapts its own test list based on what `check_comm_option_installed`
found.

`self_test_dispatcher` (`0xE4244`) is called unconditionally from
`0xE3DEE`; there's no `[0x1B10]==0` gate directly on it (that gate is
on the OLD, differently-purposed `print_selftest_banner` call site at
`0x07F8`, which is a sibling/neighbor call in the same outer routine,
not this dispatcher's caller).

**Update: all ~19 sibling test calls now individually identified** (a
later session re-read `self_test_dispatcher`'s body end-to-end more
carefully than the first pass here, which had actually missed a chunk
- see "Identified self_test_dispatcher's sibling subroutines" below
for the full corrected list and how each was found). The earlier text
in this section claiming `SUB_E28FE`/`SUB_E227E`/`SUB_E26D6`/
`SUB_E286C`/`SUB_E2CEC` were *not* test calls, but part of the
surrounding caller instead, **was wrong** - they are genuine direct
test calls inside `self_test_dispatcher`, confirmed by a full re-read.

`wait_readout_tick` (`0xE0ADD`) throttles `print_string_far`'s
character-output loop to the readout hardware's actual pace, and
`clear_selftest_status_flags` (`0xE0E56`) resets a small group of
status bytes tied to the far-pointer table `SUB_E4443` sets up.

`SUB_E374E`, `SUB_E3821`, and `SUB_E0AF5` (now `print_string_far`) -
previously listed here as unidentified sibling test subroutines - are
**not tests at all**. They're display/print primitives, found by
tracing the actual bytes each is called with (far pointers into a
fixed string-table segment, `0xFF7B:xxxx`, landing in `160-3532`) - see
"The readout/CRT display memory" below. This was the actual
correction that triggered re-examining `self_test_dispatcher` in the
first place.

Variables seen so far associated with this self-test machinery (roles
inferred from usage, not confirmed):
- `[0x1B10]` - gates whether `print_selftest_banner` runs at all this
  call (0 → run + print pre-test banner; >0xF → skip both banner and
  everything through label `L_E0894`); also used as a countdown/index
  in a `imul 0x32` calculation in the surrounding caller, role there
  still unclear.
- `[0x1B18]` - written `1` immediately before nearly every individual
  test call inside `self_test_dispatcher`; exact role (progress
  indicator? "last test index"? always the literal `1`, so maybe not
  an index) not yet confirmed.
- `[bp-0xA]` (a caller-local, not a fixed address, inside
  `self_test_dispatcher`) - accumulates OR'd return codes from each
  test into an overall self-test result, returned in `ax`.
- `[0x1BF9]` - the option-board-presence/RAM status byte set by
  `check_comm_option_installed` specifically (bit 1 = valid header found, bit 2 = also
  RAM/IO-backed).
- `[0x1B83]` - a config byte both `check_comm_option_installed` and
  `self_test_dispatcher` check against `0x1E`/`0x14` respectively as
  part of confirming/gating the comm-option RAM/IO result; role
  otherwise unconfirmed.

## The readout/CRT display memory (physical 0x40000-0x4FFFF)

Found while tracing `SUB_E07B4` (`160-3633`, the report-printing
routine that wraps `print_selftest_banner`/`self_test_dispatcher` -
see above) down through its print-primitive call chain:
`print_string_far` (`0xE0AF5`) loops over a far-pointer nul-terminated
string byte-by-byte, calling `print_char` (`0xE0B2A`) per byte, which
calls `write_readout_port_byte` (`0xE0B50`). That last routine does
exactly one thing: writes the passed byte to the **fixed** physical
address `0x40000+0x6F0` (`es=0x4000, di=0x6F0`, never incremented). A
fixed destination address for every character of a string being
"printed" strongly suggests a hardware port (a character-generator
chip with its own internal cursor/position state), not plain RAM -
this is very likely the front-panel/CRT **readout character
generator**, matching this scope's known "readout" text overlay
system (channel/volts/time labels drawn on the CRT alongside the
analog trace).

A related routine, `SUB_E0B6C`/`SUB_E0B41`, writes a short run of
*different* literal bytes to *consecutive* offsets from that same
base (`0x40000+0x6F1`, `+0x6F2`, `+0x6F3` = `0x29`, `0x23`, `0x06`) -
consistent with a small fixed command/parameter block (position,
character code, attribute) latched together rather than a text
stream.

Separately, `append_readout_char` (`0xE39F0`) - called from a
different context (a running-output-line buffer at `[0x1AF4]`, not
this fixed port) - appends a byte at the buffer's current write
pointer, then **also** writes a second byte (from `AL`, computed
differently by its caller than the first byte in `DL`) to the *same*
pointer offset by a fixed `+0x8000`. Given `[0x1AF4]`'s segment traces
back to `[0x1CC4]` (also `0x4000`-segment in every call site checked
so far), this looks like a **dual-plane buffer inside the same 64KB
window**: a "character" plane at `0x40000-0x47FFF` and a second plane
(attribute? inverse-video? a shadow copy?) at `0x48000-0x4FFFF`,
`0x8000` higher. `SUB_E0BA3` separately reads raw bytes back out of a pointer
initialized to exactly `0x48000` (`[0x31E:0x320]`) and passes each one
directly to `append_readout_char` (`0xE39F0`) - i.e. it's re-printing
bytes it just read from that second plane, consistent with echoing
raw diagnostic/calibration values that were previously stashed there.

Also found in this neighborhood: two single-byte **read**-only fixed
addresses, `0x41000` (`SUB_E4429`) and `0x42000` (`SUB_E440A`) - not
yet renamed (purpose unconfirmed - candidates: front-panel
switch/encoder status, or CRT controller status), but very likely
more of the same memory-mapped I/O window given their proximity to
the confirmed readout port. Added to `MEMORY_MAP.md` as candidate I/O.

**Not yet confirmed, worth revisiting**: whether `0x40000-0x4FFFF` is
read *as RAM* anywhere from the CPU's normal address space, or whether
it's exclusively accessed through this handful of fixed-offset
read/write primitives (which would make it much more clearly a
dedicated peripheral window rather than general video RAM the CPU
treats as memory). Also unconfirmed: the exact meaning of the `0x6F0`
port's "position" - nothing observed yet writes an X/Y coordinate
before writing a character, so either the coordinate is set elsewhere
(not yet found) or the hardware auto-advances a cursor per write
(more likely, given how `print_string_far`'s loop uses it - just a
tight byte-at-a-time write loop with no addressing logic at all).

## The readout vector display list (a second, separate print mechanism)

Found while working through the "rename everything" pass, tracing the
print-primitive cluster around `160-3633:0x3200-0x3A00`. There are
**two entirely separate mechanisms** for putting text on the readout,
not one:

1. **The hardware-port path** (documented above): `print_string_far`
   -> `print_char` -> `write_readout_port_byte`, writing straight to
   the fixed physical port `0x40000+0x6F0`.
2. **The vector display-list path** (new this session):
   `print_readout_string` (`0xE3821`) -> `draw_readout_char` (`0xE3854`)
   -> `plot_readout_point_relative` (`0xE3900`) ->
   `plot_readout_point` (`0xE3930`), which **appends a `(y, x)`
   coordinate pair plus a duplicated attribute byte** to a buffer at
   `[0x1CC4]` (tracked via a running write-pointer at `[0x1AF4]`/
   `[0x1AF6]`, with a *second plane* offset stored in `[0x1C02]` - the
   same dual-plane pattern `append_readout_char` uses). This is a
   **vector-stroke display list**, not a bitmap: `draw_readout_char`
   looks a character up in a stroke-font table at `[0x1DB0]` (indexed
   by `char & 0x7F`, 4 bytes/char - almost certainly a far-pointer
   table into a per-character stroke-list), then walks each stroke
   byte extracting a pen-up/down bit (`0x80`), a coarse component
   (bits 4-6, `>>4`), and a fine component (bits 0-3), calling
   `plot_readout_point_relative` once per stroke. This is exactly the
   classic Tektronix "readout" character-generator format used on
   other scopes of this era (each character is a short list of vector
   strokes, not a bitmap) - strong independent corroboration that this
   scope's CRT readout (channel/volts/time labels drawn alongside the
   analog trace) really is stroke/vector-based, matching `CONTEXT.md`'s
   hardware overview.

   `plot_readout_point` also handles **circular-buffer wraparound**:
   if the write pointer advances past `[0x1CC4]+0x1400` (5120 bytes),
   it wraps back to `[0x1CC4]+0x13FF` and writes a marker byte
   (attribute `2`) - the *same* marker value `mark_readout_delimiter`
   (`0xE3766`) uses, suggesting `2` is a reserved "delimiter/marker"
   attribute code throughout this buffer format, not a real
   character attribute.

   Print **records** in this system (`init_print_record`/
   `build_print_record`/`init_print_region` .. `close_print_record`)
   bracket a line: `init_print_region` opens one (position + default
   16x16 cell size), the line's text is drawn into the display list,
   and `close_print_record` tags the record's first byte with a
   completion code (`0x11` for the "normal" close, `0x39` for a second
   variant, `close_print_record_b` - exact meaning of the two codes
   not confirmed). **Correction**: earlier pseudocode wrongly assumed
   `close_print_record` (then still `SUB_E374E`) printed a string via
   its far-pointer argument - it doesn't traverse that pointer at all,
   it only tags the record. Fixed in `PSEUDOCODE.md`.

`[0x1CC4]`'s own segment component isn't directly confirmed as `0x4000`
in the proven-reachable code (no write to it was found there - it must
be initialized somewhere not yet reached), but every *use* of the
pointer it feeds (`[0x1AF4]`/`[0x1AF6]`, `[0x1C02]`) lines up with the
same `0x4000`-segment readout memory documented above, so it's the
working assumption pending that write site being found.

## Attempted: locating the stroke-font glyph table for SVG extraction

Per the user's request (they noticed vector graphics/icons on the CRT
in the hardware photos and asked whether the ROM's raw-data segments
might be this kind of vector data, wanting SVGs + a catalog if it can
be decoded): the mechanism is fully understood and already reflected
in the naming above (`draw_readout_char` reads a per-character far
pointer from a table at `[0x1DB0]`, indexed `(char & 0x7F) * 4`; each
stroke byte packs pen-up/down (`0x80`), a coarse component (`(byte &
0x70) >> 4`, 0-7), and a fine component (`byte & 0xF`, 0-15), consumed
one byte at a time until a `0x00` terminator - confirmed by directly
tracing `plot_readout_point`'s parameter order: `[bp+6]` = Y
(coarse + a baseline captured once per character from `[0x1AF8]`),
`[bp+8]` = X (the raw fine nibble), `[bp+0xa]` = a pen attribute
derived from both the stroke's own bit 7 and the character code's own
bit 7).

**What's missing**: `[0x1DB0]`'s actual value (i.e. where the glyph
table physically lives) - no write to it was found in the
proven-reachable code, so it's presumably set during an early boot
step not yet traced (`boot_init` does have a generic table-driven
initialization loop around physical `0xE0155`/`0xE017D`/`0xE01AF` in
`160-3633`, but it turned out to be a RAM-sizing/fill routine keyed by
a flag byte, not a simple pointer-copy table - not fully traced
further).

**Tried and ruled out**: wrote a script to scan both ROMs for a
128-entry far-pointer-array signature (segment mostly constant,
offsets mostly ascending, each pointing to a short byte run terminated
by `0x00`). It found a strong-looking candidate at physical `0xF1A30`
in `160-3532` (pointing into `160-3633` at segment `0xE9C3`) - but
directly reading that target region as raw bytes shows it's real
compiled code immediately followed by the plain-ASCII menu string
table (`HEAD\0ACQ_MODE_SETUP_TABLE\0SELECT_MODE\0SWP_...`), not glyph
data. The "table" was a coincidental byte-pattern match, not real
pointers - confirmed false positive, not pursued further as a lead.

**Next steps if resuming this**: either (a) trace `boot_init`'s
data-driven initialization loop(s) fully to find whatever sets
`[0x1DB0]` (and `[0x1CC4]`, also never found being written - likely
set by the same or a nearby step), or (b) tighten the heuristic search
to also validate that decoded strokes, when actually rendered,
produce a coherent connected shape for known letters (e.g. render
candidate glyphs for `'A'`/`'H'`/`'O'` and check visually/
programmatically for a plausible letterform) rather than just
checking byte-run length. No SVGs or catalog were produced this pass -
don't claim otherwise if this note is read out of context.

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

## A small task scheduler, driven by INT2_HANDLER_LATE

Read `INT2_HANDLER_LATE` closely this session (while renaming
functions in its immediate neighborhood) - it's more than a generic
NMI handler, it implements a **small task/context switcher**:

1. Saves the interrupted context's `SP`/`SS` into a per-task table at
   `[0x1A9D + idx*4]`, where `idx` is the current task index at
   `[0x1ACD]`.
2. Checks the *new* `SP` against a stack-limit table (indexed the same
   way, at a fixed table based at `ES=0xE628`) or a constant `0x7C4`
   for task 0 - if the stack has grown past its limit, jumps to
   `L_E5E99` (in the boot-adjacent `0xE5D1` segment) - **a per-task
   stack-overflow guard**.
3. Sets a "ready" flag bit (`0x40`) for the current task in a flags
   table at `[0x1A91 + idx]`, calls `SUB_E6524` (not yet examined -
   very likely "pick the next task to run" or "signal work
   available"), then falls into `switch_to_next_task` (`0xE6166`).
4. `switch_to_next_task` re-reads `[0x1ACD]` (now presumably updated by
   `SUB_E6524` to the *next* task's index), checks that task's ready
   flag - if set, loads *that* task's saved `SP`/`SS` from the same
   `[0x1A9D + idx*4]` table and resumes it (the standard 9-register
   pop + `iret` epilogue, now running on the new task's stack); if not
   ready, falls through to a different dispatch via a table at
   `ES=0xE628` indexed the same way (likely a default/idle handler per
   task slot).

This is genuinely a **small preemptive (interrupt-driven) multitasking
kernel** with independent per-task stacks and overflow protection, not
just a generic interrupt handler - a significant piece of this
firmware's overall architecture. Confirms the earlier speculation
above ("the firmware switches between distinct operating states").

**Found the other half: task creation.** `create_task` (`0xE6224`)
saves a brand-new register context (`pushf`/`cli`/push all 9 regs) onto
a fresh stack, stores that stack's `SP`/`SS` into the *same*
`[0x1A9D+idx*4]` table `switch_to_next_task` reads from, saves an
entry-point far pointer to `[0x7C0]`/`[0x7C2]`, sets the new task's
ready flag, and jumps directly into `switch_to_next_task` to start it
running. Companions: `restart_current_task` (`0xE693C`, re-spawns the
current task via `create_task`) and `mark_task_ready` (`0xE6A8E`,
clears a task's "restarting" flag and bumps its ready-flags byte).

`create_task` is called from all three ROM address spaces (`160-3633`,
`160-3532`, and the comm ROM at `2998_alias_90000`) - confirming it's
a shared kernel primitive callable from anywhere via a far call, not
something private to one subsystem.

**Found the per-tick heartbeat.** `scheduler_tick_service` (`0xE6524`)
is called *unconditionally* from both paths inside `INT2_HANDLER_LATE`
- whether or not a task switch actually happens on this particular
tick, this runs every time. It:
- Reads two hardware status bytes from **fixed physical addresses
  `0x403FFA`/`0x403FFB`** into `[0x758]`/`[0x759]` - notably, this is
  memory-mapped I/O sitting just *above* the boot-time stack's initial
  top (`SS:SP = 4000:3FFA` at reset, see `boot_init`) - a classic
  embedded design where the stack is placed immediately below a fixed
  I/O window to use RAM efficiently. This is the real source of
  `[0x758]`, previously only known as "some status/mode byte checked
  around `read_channel1_status`/`read_channel2_status`" - now
  confirmed to be a genuine hardware status register, polled once per
  tick.
- XORs `[0x758]` against a previous snapshot (`[0x7B4]`) - classic
  edge/change detection on that hardware status.
- Increments `[0x752]` - **the exact counter `wait_readout_tick`
  busy-waits on** - confirming `wait_readout_tick`'s delay really is
  paced by this same timer tick, not an unrelated counter.
- Cycles through a few debounce/periodic-maintenance steps gated by
  `[0x1A94]`/`[0x1A95]`/`[0x756]` vs. a threshold at `[0x764]`, and
  (via the `[0x7B2]` 0/1/2 rotation in the code just before it's
  called) rotates between `scan_low_ram_chunk0`/`chunk1`/`chunk2`
  (`0xE5D3D`/`0xE5D49`/`0xE5D58`) - **a genuine background low-memory
  watchdog scan, not a timing delay** as first guessed for the
  similar-shaped `delay_read_128w`. Each reads through a different,
  non-overlapping slice of physical `0000:0000+` via `rep lodsw`
  (`0x00-0x5F`, `0x60-0xBF`, `0xC0-0xFF` respectively) - together they
  cover exactly the **first 256 bytes**, a quarter of the interrupt
  vector table, split across 3 ticks so no single tick blocks for the
  whole scan. This reframes `delay_read_128w` too: it's likely the
  SAME kind of integrity check (of the first 256 bytes in one shot)
  rather than a deliberate delay, though it's called once at startup
  rather than tick-rotated, so the "delay" name was kept for it
  specifically.

This resolves "what triggers INT2" indirectly: INT2 itself is still
presumed to be a periodic hardware timer (not confirmed which), but
now it's clear *why* a timer interrupt exists at all - it's the
heartbeat for both task switching AND this hardware status/debounce
polling, unified in one place.

**Still not confirmed**: how many tasks exist and what each one does
(the self-test/UI/acquisition-refresh loop are plausible candidates),
and exactly which hardware `0x403FFA`/`0x403FFB` belong to (front-panel
key/encoder status is the leading candidate, matching where `[0x758]`
gets used elsewhere).

`SUB_E61E3` (now `create_task_b`) turned out to be a near-identical
duplicate of `create_task`'s own body - same save-context, same
per-task table write, same ready-flag set - called specifically from
`mark_task_ready` rather than sharing the existing `create_task`. Not
confirmed why the compiler/source duplicated this rather than calling
`create_task` directly - possibly an artifact of how the original C
source structured task creation vs. task wake-up as textually separate
functions despite near-identical bodies.

## Validation status

Ran the recursive-descent output (13,510 decoded instructions - main
ROM plus the comm-ROM instructions proven reachable, including the
`0x90000` alias) through `validate_nasm.py`: **12,133 byte-exact,
1,375 provably-equivalent alternate encodings, 0 real mismatches, 2
not independently checked** (down from a larger unconverted count
across two sessions - `xchg`'s reg/rm-swap ambiguity, the `outsb`/
`outsw`/`insb`/`insw` string-I/O mnemonics, and a target-computation
bug affecting 3 backward branches are all now fixed, see below and
"IP-wraparound branches was a validator bug" above).
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
- `xchg` between two registers: no separate direction bit (unlike the
  ALU ops/MOV above) - the *same* opcode (`0x86`/`0x87`) with reg/rm
  simply swapped in the ModRM byte expresses the identical operation,
  since xchg is symmetric (e.g. `xchg di,si` as `87 F7` or `xchg si,di`
  as `87 FE`). Needed its own `alt_xchg_encoding()` (found via one
  instance in `160-3633` at chip offset `0114`) rather than reusing the
  direction-bit flip, since XOR-ing the opcode by `0x02` (what the ALU/
  MOV case does) turns `0x87` into `0x85` (`test`), a different
  instruction entirely.
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

`outsb`/`outsw`/`insb`/`insw` (the 8-/16-bit string I/O instructions)
were previously excluded pending verification; confirmed this session
that NASM encodes each as the plain expected single opcode byte
(`6E`/`6F`/`6C`/`6D`) with no unwanted prefix, and that `outsb`/`outsw`
follow the same "source(SI) is the *second* op_str operand" shape as
`movsb`/`movsw` for segment-override purposes while `insb`/`insw`'s
sole operand is the fixed, unoverridable `ES:DI` side, like `stosb`/
`stosw`. `insd`/`outsd`/`bound` remain excluded - `insd`/`outsd`
require a `0x66` operand-size prefix that never legitimately appears
on this 8086/8088, and `bound` is an 80186+ instruction that can't be
real on this CPU either, so any occurrence of either is far more
likely decode drift than genuine code (none appear in the proven-only
listing at all, only in the heuristic layer's known-noisy tail).

**The 2 remaining not-independently-checked instructions**: 1 x87 FPU
instruction (`fmul`, `160-3532`) - capstone's `st(N)` operand syntax
isn't translated to NASM syntax yet, see `TODO.md`; and `insd`/`outsd`/
`bound` collectively (which never occur in the proven-only set - see
"Validation status" above for why these stay excluded). The 3 backward
branches that used to be excluded here (1 `loop` at `160-3633:FB66`, 2
`jmp` at `160-3532:0C79`/`0x1044`) are now fixed - see "IP-wraparound
branches was a validator bug" below.

## IP-wraparound branches was a validator bug, not a real quirk (RESOLVED)

**Correction**: an earlier session concluded the 3 backward `loop`/
`jmp` instructions below were a genuine 8086 "IP register wraps mod
`0x10000` while CS stays fixed" hardware behavior, landing their
targets outside the current chip's 64KB window. That conclusion was
**wrong** - it was a bug in `validate_nasm.py`'s own target-computation
code, not real 8086 behavior.

| Instruction | seg | off | disp |
|---|---|---|---|
| `160-3633:FB66 loop` | `0xEFB6` | `0x0006` | `-0x78` |
| `160-3532:0C79 jmp` | `0xF0C2` | `0x0059` | `-0x20C` |
| `160-3532:0x1044 jmp` | `0xF0EB` | `0x0194` | `-0x1F7` |

What actually happened: `validate_nasm.py`'s `near_target_addr()`
masked the raw target offset to 16 bits (`int(off_str, 16) & 0xFFFF`)
*before* adding it to `seg<<4`. Capstone represents a backward
branch's target as a huge sign-extended hex string when the raw
`ip+disp` computation goes negative (e.g. `"0xfffffe50"` for a small
negative result) - masking that to 16 bits *first* effectively models
a literal "IP register wraps, CS stays put" reinterpretation, which
lands the result a full `0x10000` away from the intended target
whenever `seg` isn't 16-aligned.

The fix: don't mask the raw offset at all - just add the (possibly
huge) raw integer to `seg<<4` and mask the *final sum* to 20 bits, the
same way `gen_disasm_x86.py`'s own `seg_off_to_phys()` already did.
This works because `2**32` (capstone's effective width for the
sign-extended value) is an exact multiple of `2**20` (the physical
address space), so the excess high bits cancel out under the final
mask regardless of `seg`'s alignment - recovering the plain
"target_physical - current_physical" arithmetic the original compiler
almost certainly used when back-patching the branch displacement.
Confirmed independently: `gen_disasm_x86.py`'s recursive descent had
*already* followed 2 of these 3 exact jumps successfully and left real
labels at their targets (`L_F0A70`, `L_F0E50`, both cleanly inside
`160-3532`'s own range) - proof the "naive" interpretation was correct
all along and the wraparound theory was never needed.

After the fix, all 3 instructions validate and convert cleanly (0
unconverted, 0 mismatches) - see "Validation status" below. This also
means `binary/aligned/`'s readable reconstruction now has only **1**
remaining raw-`db` instruction in the entire 128KB main ROM pair (the
one x87 `fmul`, still unhandled - see `TODO.md`).

## Current coverage

**Proven-only** (official, what `sysrom_3532_3633.lst` shows): 13,510
instruction-start bytes (~10% of the 128KB main-ROM pair, including
the comm ROM's `0x90000` alias region), from 8 seed
entry points (reset vector, 2 discovered while tracing the reset path,
4 interrupt handlers found via IVT-write tracing, and the comm-ROM
boot-stub target). Zero unresolved indirect jmp/call instructions
exist anywhere in the reached code - this codebase appears to prefer
cmp+je dispatch chains over jump tables, at least in what's been seen,
so that's confirmed *not* to be the lever for growing this further;
finding more independent entry points (the way the interrupt handlers
were found) is.

**With the heuristic layer** (both ROMs' `55 8B EC` push-bp/mov-bp,sp
signature scan, plus the confirmed real address-decode alias -
combining `gen_disasm_2998.py` and the new `gen_disasm_mainrom_
heuristic.py`, see below): **68,776 instructions, 196,372 of 229,376
total mapped bytes (85.6%)** across all four mapped regions
(`3633`/`3532`/`2998`/`2998_alias_90000`). Individually: `3633` and
`3532` both 90.6%, `2998` 91.8%, the `0x90000` alias 53.3% (lower
because it only reflects the *upper half* of the comm ROM, and only
what's reachable from entries seeded in that same half). NASM-validated
end to end (`validate_all.py`): 61,870 exact + 6,799 alt-encoding + 53
not-converted + 54 real mismatches, all traced to genuine decode
drift into non-code bytes (see "The main ROM has a heuristic layer
too" below) - none of them affect the buildable `.asm`'s correctness,
since the generator only ever trusts exact-match bytes for real
conversion. (The mismatch count rose from 41 to 54 this session after
`insb`/`outsb`/`outsw` were newly handled - inside the drift cluster
below, these now get *attempted* and correctly flagged as mismatches
against garbage bytes, instead of being silently skipped as
"unconverted" the way they were before; this is the validator working
as intended, not a new problem.)

The former "12 call targets resolve to addresses in the 0x80000-0x97000
range" open item is resolved - see "The comm ROM is NOT bank-switched"
and "The 0x90000-0x97FFF region is fully resolved" above.

## The main ROM has a heuristic layer too

Applied the same technique that worked well for the comm ROM to the
main ROM pair: scanned `160-3633`/`160-3532` for the `55 8B EC`
prologue not already proven-reachable. Found **216** in `3633` and
**209** in `3532` - a lot of real compiled functions the proven-only
recursive descent simply had no path to yet. Seeding them
(`gen_disasm_mainrom_heuristic.py`, output in
`sysrom_3532_3633_heuristic.lst` - a SEPARATE file, the official
`sysrom_3532_3633.lst` stays proven-only) took both halves' coverage
from ~14-41% to **90.6% each**.

Same lower-confidence caveat as the comm ROM's heuristic layer applies
(a prologue match is a strong signal, not proof of reachability). NASM
validation found a real cluster of ~39 genuinely bad decodes around
physical `0xEA1A0-0xEA615` in `3633` - 386-only features (`fs`/`gs`
segment prefixes, `popal`/`popaw`, the `esi` register) that literally
cannot be real instructions on this confirmed 8086/8088 system, so
that narrow range is decode drift into non-code bytes. Traced its
origin: these addresses have **no label at all** (not a jump/call
target, not a heuristic entry) - they were reached by pure sequential
fallthrough from whatever real instruction happened to precede them,
walking straight across a code/data boundary with no jump in between.
This is a real, if narrow, limitation of linear-descent-by-default:
recursive descent has no way to know a data region starts at a given
byte unless something tells it to stop there. Left as-is rather than
patched around, since the validator's exact-match-only trust model
already prevents this from corrupting the buildable `.asm`.

Chasing these mismatches also surfaced (and fixed) several more real
`validate_nasm.py` bugs, now benefiting every ROM's validation:
- `jcxz` doesn't accept NASM's `short` qualifier the way other `jcc`
  mnemonics do (it only ever had one encoding) - was crashing the
  whole batch assembly for any file containing one.
- `cmpsb`/`cmpsw` have their source/dest operand order reversed from
  `movsb`/`movsw` in capstone's output (source first, not second) -
  the segment-override detection was checking the wrong side.
- A whole class of alternate-encoding helpers (`alt_displacement_
  encoding`, `alt_duplicate_opcode`, the new `alt_zero_displacement_
  encoding`) didn't account for a leading segment-override prefix byte
  (like `cs:`), so they misread the prefix byte itself as the opcode
  whenever one was present - added a shared `split_seg_prefix()` used
  by all three now.
- New alternate-encoding case: a `mod=01` (disp8) or `mod=10` (disp16)
  addressing form with a displacement of exactly `0` can also be
  encoded as `mod=00` (no displacement byte at all) for the identical
  effective address - NASM prefers the shorter form (except when
  `rm==0b110`, reserved at `mod=00` for direct addressing, so `[bp+0]`
  has no 2-byte form).
- A stray `0xF2`/`0xF3` (REPNE/REP) prefix byte in front of a
  non-string instruction - legal on real hardware (the CPU just
  ignores it there) but there's no NASM syntax for it, so it's
  excluded from conversion rather than silently dropped.

## NOP-aligned readable reconstruction (`binary/aligned/`)

Tested whether NASM's `CPU` directive would resolve the encoding-
choice ambiguities documented above (register-direction bit,
immediate/displacement width, zero-displacement collapse, duplicate
`0x80`/`0x82` opcode) so the buildable `.asm` wouldn't need a raw `db`
fallback for them. Confirmed directly (`CPU 8086` tested against `mov
bp,sp` and `cmp ax,1`): it doesn't - `CPU` only restricts which
instruction-set level is *available*, it has no effect on which legal
encoding NASM picks when more than one exists. Also tested `CPU 8086`
as a guard against 386-only decode drift: mixed results (correctly
rejects `esi`, but not `gs:` and only warns rather than errors on
`popal`) - not reliable enough to lean on.

Landed on NOP-padding instead, directly from the idea of substituting
NASM's preferred (shorter) encoding and padding the difference with
`NOP` (`0x90`) bytes so every following address is unchanged from the
original. `disasm/gen_source_readable.py` implements this: every
decoded instruction is emitted as real NASM text (never a raw `db`),
and any instruction whose NASM-preferred encoding is shorter than the
original gets `times N nop` appended to absorb the gap.

Output: `160-XXXX-14_readable.asm` (disasm/) reassembles, per chip, to
the same length as the true original with only a handful of bytes
differing (all NASM's own equivalent-encoding choice, verified
instruction-by-instruction, not decode error):

| Chip | Bytes differing (of 65,536) |
|------|------|
| 160-3633-14 | 1,431 |
| 160-3532-14 | 324 |
| 160-2998-14 | 108 |

The assembled result is saved permanently at `binary/aligned/160-XXXX-
14_aligned.bin` - **not original firmware**, see `binary/aligned/
README.md`. Adopted as the reference binary for *future* checks
(easier to keep address-aligned with the growing symbol table than
re-deriving encoding equivalence every time this comes up), but this
is a methodology choice, not a fully-vetted one - flagged in `TODO.md`
to review again once the rest of the analysis is further along.

## Found the top-level self-test orchestrator

`run_selftest_sequence` (`0xE3B12`) ties together nearly everything
this project has learned about the self-test subsystem across
multiple sessions, in one function: it writes a marker byte at
physical `0x00000` (plausibly an `iret`/iret-like opcode planted at
address 0 to make an accidental null-pointer call harmlessly return,
a common embedded-firmware defensive trick), initializes the readout
vector display-list buffer (`[0x1AF4]`/`[0x1AF6]` from `[0x1CC4]`,
`[0x1C02]=0x8000` - the dual-plane offset), runs setup via `SUB_E4443`/
`SUB_E75C0`/`SUB_E128D`, picks a test-mode byte `[0x1B48]` based on
`[0x758]`, calls `init_selftest_report_screen`, and then calls
`print_selftest_report_line` exactly once immediately followed by
`self_test_dispatcher` itself. This is the actual entry point that
starts a self-test run - found by tracing print_selftest_report_line's
callers, which had been an open thread since it was first identified.

Also found this session: the HPGL plotter output driver
(`update_plot_position`/`plot_line_to`, emitting `PU%d,%d;`/
`PD%d,%d;` via `format_string_va`, plus `SP1;SC0,1023,0,1023;` and
`ESC*rB`-style HP-GL2 escape sequences seen nearby) sits directly
around the already-documented I/O port `0x83` write in `160-3532` -
a strong candidate for that port being the GPIB/plotter output,
though not confirmed against a schematic. And `verify_adc_calibration`
(`0xE30B5`) is a calibration-check sibling to `run_adc_selftest`,
comparing the `[0x322]` status register's 12-bit value field against
an expected reference and reporting an "uncaled" error in hex if it
doesn't match.

## Identified self_test_dispatcher's sibling subroutines

**The single biggest lever this project has had for matching self-test
subroutines to real peripherals**: rather than guessing from call
order, went back through each of `self_test_dispatcher`'s ~19 sibling
calls and searched its body for a `mov <reg>, 0xFF7B` (the fixed
string-table segment) paired with an offset, then read the actual
bytes at that physical address (`0xFF7B0 + offset` in `160-3532`).
Nearly every one directly references one of the diagnostic labels
already catalogued in `STRINGS.md` - a much stronger identification
than call-order proximity ever gave. Also corrected: the earlier read
of `self_test_dispatcher` (documented just above) had missed a whole
chunk of its body, wrongly attributing 5 of its real test calls to the
surrounding caller instead - a full re-read fixed this.

Full corrected call list, all now renamed in `gen_disasm_x86.
FUNCTIONAL_NAMES`:

| Call | String found | Test |
|---|---|---|
| `selftest_hs_acq` (`0xE28FE`) | `HS_ACQ` | High-speed acquisition mode |
| `selftest_front_panel_switch_a` (`0xE227E`) | (none - see below) | Front-panel control, range 0-8 |
| `selftest_mm_acq` (`0xE26D6`) | `MM_ACQ` | Min-max acquisition mode |
| `selftest_xy_acq` (`0xE286C`) | `XY_ACQ` | X-Y acquisition mode |
| `selftest_cursor_delta_time` (`0xE2CEC`) | `CDT` / `PRE-DETRIG` / `TIME-OUT` | Cursor delta-time measurement |
| `selftest_front_panel_switch_b` (`0xE2FC8`) | (none) | Front-panel control, range 0-0x15 |
| an **inline block** (no separate sub) | (none) | Runs `configure_measurement_hw`+poll+`clear_selftest_status_flags` directly in `self_test_dispatcher`'s own body, gated on `[0x1B7A]!=1` - result NOT OR-folded (informational, like `check_comm_option_installed`) |
| `selftest_tb_divider` (`0xE252A`, conditional) | `TB_DIVIDER` (via `HARDWARE.md` photo, not a code string) | Timebase divider, range 0-0x18, only tested if comm option's RAM/IO confirmed |
| `selftest_measure_and_report` (`0xE0FD0`) | (none) | Enable/run/disable measurement (identified previous session) |
| `check_comm_option_installed` (`0xE44F1`) | (n/a, not OR-folded) | Comm/GPIB option detect |
| `selftest_rom_checksum` (`0xE16EA`) | `ROMS` / `MISMATCH` | Main ROM checksum |
| `selftest_comm_rom` (`0xE1E3E`) | `COMM_ROM` | Comm ROM checksum (both its `0x80000` real address and `0x90000` alias) |
| `selftest_comm_loopback_a` (`0xE1D28`) | `COMM_LB` (via `SUB_E20B0`) | Comm-board loopback, phase A |
| `selftest_comm_loopback_b` (`0xE1DB3`) | `COMM_LB` / `FGET NOT SET` / `FGET NOT CLEAR` (via `SUB_E1FBC`) | Comm-board loopback, phase B |
| `selftest_comm_ram` (`0xE1E90`) | `COMM_RAM` / `CMOS NOT SUPPORTED` | Comm-board RAM |
| `selftest_cmos` (`0xE1F18`) | `CMOS` / `reformated` / `recovered` | CMOS/NVRAM (with recovery) |

(Table order above is by identification method, not call order - see
the raw disassembly for the exact sequence, documented in
`gen_disasm_x86.FUNCTIONAL_NAMES`'s comments.)

The three front-panel-control tests (`selftest_front_panel_switch_a`/
`_b`, and the third one) don't reference a diagnostic string
directly - identified instead by their distinctive shape: each scans
`update_menu_position` across a fixed range (0-8, 0-0x15, 0-0x18
respectively) via a small step-helper, exactly the same mechanism the
real menu-navigation cursor uses (see "Menu navigation" in
`VARIABLES.md`). This means they're testing actual front-panel
controls (knobs/switches) by sweeping them through their full range,
not reading a fixed diagnostic ID.

**Update - the third one is `selftest_tb_divider`, not a comm-option
switch.** Tracing `0xE252A`'s step helper (`step_tb_divider_test`,
`0xE255E`) further shows it calls `verify_timebase_prc` with a
shifted `0xFFF` threshold mask - matching the real on-screen name
`TB_DIVIDER` confirmed in the `HARDWARE.md` hardware photos (under
`DIAGNOSTICS/TESTS/ACQUISITION`). Renamed from the earlier
`selftest_comm_option_switch` guess. It's still gated on
`[0x1B83]==0x1E` (comm option confirmed) - possibly the timebase
divider circuit under test needs the comm board's `EXT CLK` line
(see the AUX connector in `HARDWARE.md`), but this isn't confirmed;
the gate doesn't mean the *control* being swept is on the comm board.

**Also found**: `selftest_front_panel_switch_a`'s own step helper
(`run_adc_selftest_range`, `0xE22AF`) is the *same* function later
also confirmed as part of the dedicated `A_TO_D_TESTS` A/D exerciser
(calling `run_indexed_adc_selftest`) - meaning `selftest_front_panel_
switch_a` verifies its swept control via **ADC readback**, not just a
digital position read. This is a real clue for "which control": it's
very plausibly an **analog/potentiometer-based** front-panel control
(read through the A/D converter) rather than a purely digital rotary
switch - VOLTS/DIV is the leading candidate given its 9-ish detent
positions matching the 0-8 sweep range. **`selftest_front_panel_
switch_b`'s step helper (`step_front_panel_switch_b_test`, 0xE2FFC)
turns out to use the exact same shape** - it also calls an ADC
verification routine (`verify_adc_calibration`) rather than a digital
read, over its 0-0x15 (21-position) range. So both of the still-open
front-panel-control tests are ADC-verified analog controls, not
digital switches - VOLTS/DIV (CH1) and VOLTS/DIV (CH2) is now a more
likely pairing than VOLTS/DIV + TIME/DIV, though not confirmed.

The `selftest_display_irq_idle`/`selftest_display_irq_active`
(`0xE3F2C`/`0xE3F99`) pair - referenced from a *different* part of the
self-test flow, not `self_test_dispatcher`'s main OR-fold list, but
found during the same string cross-reference sweep - test the readout/
CRT display controller's interrupt line (see "The readout vector
display list" above): idle-state check (`line stuck high`) and an
active check after drawing a shape and expecting an interrupt within a
timeout (`TIMEOUT`/`unable to reset`).

**Follow-up sweep found 4 more, closing most of the remaining
questions**: wrote a one-off scanner (`disasm/scan_string_refs.py`,
not part of the regular pipeline, kept for reuse) that automates the
same technique across every still-unnamed function at once - for each
one, find any `mov reg, 0xFF7B` within 2 lines of another `mov reg,
0xNNNN`, and read the string at that computed address. Found:
- `format_selftest_result_string` (`0xE0C3D`) - **this is the missing
  link** noted in earlier sessions ("the actual message printing
  happening elsewhere, not yet found"). Builds `PASSED`/`FAILED`/
  `UNTESTED`/`Not installed` text from a status-bits argument:
  `0x20`=not installed, `0x02`=failed, `0x01`=passed, else untested -
  this is the bit encoding used throughout `self_test_dispatcher`'s
  OR-folded `[bp-0xA]` accumulator.
- `selftest_front_panel_adc` (`0xE296E`) - references `FP_a2d`,
  identifying the peripheral behind the `configure_measurement_hw`/
  `run_adc_selftest`/`selftest_measure_and_report` cluster: **the
  front-panel A/D converter**, not an acquisition-channel ADC as
  originally guessed.
- `ram_pattern_test` (`0xE1B89`) - a generic memory test engine
  (alternating `0xAA`/`0x55` write, then masked read-back verify) -
  the likely shared implementation behind the `SYS_RAM`/`NIB_RAM`/
  `ACQ_RAM`/`COMM_RAM` self-tests. Identified from its code shape
  alone, no string needed.
- `selftest_comm_readback` (`0xE20B0`) - the 2nd phase called by
  `selftest_comm_loopback_a`, references `COMM_RB`/`rb(1)=`/`rb(0)=`.
  **Surprising and unreconciled**: it reads/writes physical
  `0x40000+0x67C`/`0x6F8` - the readout/CRT memory window documented
  above, nowhere near the comm ROM's actual `0x80000` address. Either
  the comm board's registers are somehow also mapped into part of the
  `0x40000` RAM window (a form of shared/dual-ported memory not
  otherwise evidenced), or "COMM" in `COMM_RB` doesn't mean the GPIB/
  RS-232 option board in this specific string (could be a different
  "communication" - e.g. between the CRT controller and main CPU).
  Left unresolved - worth another look once more of this address
  range's other uses are mapped.

## Possible ADC/measurement self-test hardware

Found while renaming the `SUB_E12F4`/`SUB_E2DC9`/`SUB_E0DCC` cluster
(`160-3633`) - flagged as a lead at the end of the previous renaming
session. These three, plus `selftest_init_channel_hw` (`0xE2AB0`) and
`clear_selftest_status_flags` (`0xE0E56`) from earlier, all read/write
a shared set of "hardware register" variables via far pointers:
`[0x322]`, `[0x326]`, `[0x32A]`, `[0x32E]`, `[0x336]`, `[0x33A]`, plus
the `[0x31E]`-based (physical `0x48000`) scratch buffer used as a
lookup table.

`configure_measurement_hw` (`0xE0DCC`) writes 5 caller-given parameters
into this register cluster (including two *reverse-indexed* lookups -
`0x1000 - param` and `0xFF0 - param` - into the `[0x31E]`/`[0x32E]`
buffers, suggesting a calibration/reference table addressed from its
end). `run_adc_selftest` (`0xE12F4`) then polls `[0x322]` for a busy
bit (`0x8000`) with a timeout, reads a **12-bit result** (mask `0xFFF`)
once the busy bit clears, and compares it against a reference value -
classic ADC status/data register shape (busy flag + N-bit result in
the same word). `wait_stable_measurement` (`0xE2DC9`) is a sibling that
instead waits for a byte at `[0x32A]` to stop changing across
consecutive reads (a debounce/settle pattern), then checks two more
status bits (`0x2000`, `0x4000`) in `[0x322]`.

**Working hypothesis**: this is the firmware's generic **ADC-based
self-test primitive**, reused across several of `self_test_dispatcher`'s
still-unidentified subroutines (`SUB_E3F2C`, `SUB_E3F99`, etc. are good
candidates to check next - if any of them call `configure_measurement_
hw`/`run_adc_selftest` directly, that would both identify them and
narrow down which physical measurement each one performs). **Not
confirmed**: which physical ADC/ADC-mux this addresses, or whether
`[0x322]`'s bits 13/14 (checked by `wait_stable_measurement`) mean
"calibration valid"/"lock detected" (plausible for a timebase PLL) or
something else entirely.

## Possible waveform acquisition buffer init (updated: likely a plot-scale cache, not a buffer)

Found while renaming `160-3532:0x03F4`/`0x0414`/`0x0446` (all three
converge, via a tiny stub or a short print-then-fall-through, on the
same shared tail block at `L_F0678`). That block initializes **8
separate values to the identical value `0x800`
(2048 decimal)**: `[0x6F8]`, `[0x6F6]`, `[0x6F4]`, `[0x6F2]`, `[0x6E8]`,
`[0x6E6]`, `[0x6B4]`, `[0x6B2]`, plus a handful of other fixed values
(`[0x6FA]`-based byte `=0xD5`, `[0x700]`/`[0x702]=0`, `[0x704]=0x14`,
`[0x70E]=0`) and a call to `update_plot_position(0x200, 0x200)`.

**Updated interpretation** (a later session correctly identified the
neighboring `scale_and_plot_point`/`scale_and_plot_point_default`
pair, previously miscalled `divide_scale`/`divide_scale_default` -
they do fixed-point multiply-by-reciprocal-then-shift scaling, not
divide): `[0x6E6]`/`[0x6E8]` are exactly the two variables
`scale_and_plot_point` **caches its scaled point into** for drawing
the next line segment. Given that, this init block is much more
likely a **plot-position/scale cache reset to a midpoint default**
than a literal "waveform buffer" - `0x800` = 2048 is exactly the
midpoint of a 12-bit range (`0-4095`), consistent with `[0x322]`'s
confirmed 12-bit ADC value field (see `run_adc_selftest`) and a
sensible "no data yet, assume centered" starting value for a scaled
plot point. The other two pairs (`[0x6F8]`/`[0x6F6]`/`[0x6F4]`/
`[0x6F2]` and `[0x6B4]`/`[0x6B2]`) plausibly reset the *other* cached
points this rendering pipeline tracks (e.g. per-channel or per-axis
"last plotted point" caches), all to the same centered default.

**Still not confirmed**: the exact number/purpose of all 8 values
individually, or whether this is really scoped to acquisition/plot
rendering at all rather than something else (a GPIB/plot output buffer
set, for instance - the comm ROM code is
physically adjacent in the address space story but this is the MAIN
ROM, so that's less likely). Revisit if the self-test subroutines or
menu-string cross-referencing work ever turns up a direct link to
"record length" or a channel-buffer concept.

**Update:** the shared tail itself (`L_F0678`, reached via `SUB_F0446`'s
un-prologued "push es; jmp" stub) is now renamed
`reset_all_channel_plot_caches`. Its entry style (no `push bp`/`mov bp,
sp` of its own, relying on a frame already established by whatever
reaches it) is the same shared-tail pattern already confirmed harmless
for `convert_sample_value`.

## Found: the hardware shift-register write (I/O ports 0xD1/0xC4)

Renamed `write_hw_shift_register` (`0xEE13B`), resolving I/O ports
`0xD1`/`0xC4` from `MEMORY_MAP.md`'s long-standing "no documentation
yet on which physical device" list. It writes `ax` to port `0xD1`
three times in a row - each write preceded by `shl di,1` - then once
to port `0xC4`. This is the textbook shape of clocking a value out to
a **serial shift-register-based hardware latch**: `0xD1` looks like
the data/clock port (written repeatedly as the value shifts) and
`0xC4` the strobe/latch port (written once, after the shift sequence
completes) - though which is which isn't confirmed.

Found it by reading the larger function around it (starting near
`0xEE004`): that function reads a table entry at `[0x1D10 + idx*16]`
(`idx` from `[0x464]`), and either makes an *indirect* far call through
the table (`lcall es:[bx+di+6]`) or falls through to
`write_hw_shift_register` as a default action. This shape - "look up a
setting's handler, call it, or do a generic hardware push if none" -
is consistent with a **front-panel setting (attenuator/gain/offset
calibration value) being written out to analog hardware whenever it
changes**. Not confirmed against a schematic, but the mechanism (a
serial shift-register write) now is, closing a mystery that's been
open since the very first coverage pass found these two ports.

## Found: the firmware's assert()/panic mechanism

`halt_cpu` (`0xF1611`) is a single `hlt` instruction - but tracing its
callers turned up something more interesting than a generic halt.
Callers (e.g. `assert_and_halt`, `0xE9255`) call it with 2 words
pushed as arguments (a computed value plus a small constant tag like
`0xB`/`0xC`/`2`) and then have unreachable stack-cleanup code
immediately after the call - because `hlt` never returns, that cleanup
never executes. This is exactly what compiled C looks like when a
function is declared to never return (an `abort()`/panic-style call):
the compiler still generates the normal call-then-cleanup sequence
(it doesn't know the callee halts forever), but the cleanup is dead
code in practice. `assert_and_halt` computes a value via
`convert_sample_value` (`0xF1001`) and halts with it plus a tag if
some bound is exceeded - a genuine embedded assertion-failure trap,
presumably only ever tripped during development/debugging (or as a
last-resort safety stop on real hardware fault). `convert_sample_value`
itself opens with a real x87 `fmul` instruction (entered via
fallthrough, no prologue of its own) followed by `mul32`/`sdiv32` -
further, more concrete evidence toward `TODO.md`'s open question of
whether a real 8087 coprocessor is present (this is a genuine,
non-drifted floating-point instruction in the middle of otherwise
completely ordinary compiled-C integer code, not part of any known
decode-drift cluster).

## Open puzzle: comm ROM's compute_parity_mode_code calls the main ROM's scale_and_plot_point_default

`compute_parity_mode_code` (`0x96800`, comm ROM) is a straightforward
DIP-switch-to-parity-code dispatcher, but it brackets that dispatch
with two calls to `scale_and_plot_point_default` (physical `0xF0078`,
confirmed by direct disassembly to multiply an argument by the global
plot-scale reciprocal at `[0x6D2]` and fall through into real plotting
code) - once at entry with the literal argument `0x8F80`, saving its
`AX` result, and again at exit passing that saved result back in. This
is the exact argument/save/restore shape used everywhere else for the
`set_ds_return_old` DS-segment-switch helper (`push value; lcall;
save AX; ...; push saved AX; lcall` again) - but the target here is
confirmed to be the plot-scaling function's real address, not
`set_ds_return_old`'s. Not resolved: whether the comm ROM is
deliberately (ab)using this shared math primitive for value round-
tripping unrelated to plotting (and the "plot a point" side effect is
simply harmless/unobserved in this codepath), or whether this points
at some other addressing subtlety not yet understood. Left as an open
question rather than guessed at; the parity-code logic itself doesn't
depend on the answer.

**Second instance found**: `plot_point_with_flag_update` (`0xE004F`,
main ROM `160-3633`) uses the *exact same* bracketing shape (call
`scale_and_plot_point_default(0x8F80)`, save `AX`, do unrelated work,
call it again with the saved value) - but this one is called from
within the plot-output task area (`0xE6E2B`), where actually invoking
the real plot-scaling code is completely plausible, unlike the comm
ROM's parity-code context. This tips the balance slightly toward "the
`0x8F80`/save/restore bracketing is a real, intentional pattern with
some purpose beyond DS-switching" rather than a decode error - but
what that purpose is (is `0x8F80` a real point coordinate? is the
saved/restored value meaningful, or is only the side effect of the
call wanted?) still isn't understood.

**Checked for the `SUB_F6382`/`SUB_E90A5`-style "landing 1-byte-early
into a neighboring instruction" explanation** and it does NOT apply
here: the bytes immediately before `0xF0078` (`0xF0070-0xF0077`) are
never reached by recursive descent from any other confirmed path, so
there's no second, contradicting decode to compare against - unlike
those two cases, where a real fallthrough path disagreed with the
call-target path. `scale_and_plot_point_default`'s own decode (`inc
sp; add byte [bp+si-0x19], dh; ...`) is the only interpretation
available for those bytes, however unusual it looks.

## Resolved (partially): SUB_E90A5/SUB_E92B0 are call targets landing 1 byte into a "mov di, tag" instruction

An earlier session's investigation of `SUB_E90A5` and `SUB_E92B0` (both
called via genuine `lcall`, both opening with the same
capstone-decoded-as-garbage bytes `1f 00 57 9a ...` / `00 57 9a ...`)
is now explained, at least mechanically. In both cases, the call
target address is exactly **one byte past** the start of a `mov di,
TAG` instruction (`BF <tag_lo> 00`, 3 bytes) that a separate,
already-understood fallthrough/jump path executes normally as part of
the same repeating pattern used throughout `assert_and_halt`: `mov di,
TAG; push di; lcall convert_sample_value; push di; push ax; lcall
halt_cpu`. Landing 1 byte in means the CPU instead starts decoding
from the immediate's low byte (which, for tag values `< 0x100`, equals
the tag itself) and the always-zero high byte, producing exactly the
"garbage" opcodes seen (e.g. `1f` = `pop ds`, `00 57 9a` = `add byte
[bx-0x66], dl`, etc). For `SUB_E92B0` specifically, tracing both
byte streams forward shows they **reconverge exactly at `0xE92B7`**
with identical subsequent bytes (`mov word [bp-0xa], ax`, the start of
the next `convert_sample_value` result store) - strong confirmation
this is a real, reproducible byte-level coincidence and not a
transcription error.

**Not fully resolved:** *why* something calls directly into what looks
like the middle of another check's tag-setup instruction, rather than
its intended start. Possibly these call sites intentionally reuse a
"skip the redundant tag load, the value we want is already what's
sitting in that immediate's low byte" trick, or possibly this is
itself a symptom of the same `0x0F`-style capstone-vs-real-8086
opcode-table mismatch documented for `SUB_F6382` interacting badly
with an unrelated coincidence here - not pursued further. Left
`SUB_E90A5`/`SUB_E92B0` unrenamed; they are not independent functions
worth naming, just artifacts of where a real call happens to land
inside `assert_and_halt`'s/`plot_scaled_point`'s tag-dispatch code.

## Found: RS-232 software flow control (XON/XOFF) in the comm ROM

`get_xon_xoff_byte` (`0x9751A`, `160-2998`) is a small, unambiguous
match for standard RS-232 software flow control: it checks two request
bits in `[0x460]` and returns the ASCII control code for XOFF (`0x13`,
DC3) or XON (`0x11`, DC1), clearing the corresponding request bit as
it does. It's called from `service_comm_tx_queue` (`0x97431` -
**renamed from an earlier wrong `service_comm_rx_queue`**: this drains
the OUTGOING tx ring buffer that `serial_tx_buffer_put` produces into,
not an incoming rx buffer - see "Direction correction" below), which
services a wrap-around ring buffer (`[0x448]`/`[0x44A]` read pointer,
`[0x44C]` write pointer, base `0xAF` size `0x384`) and forwards either
a pending flow-control byte or the next queued outgoing data byte to
`enqueue_comm_char` (`0x974E1`), which also implements space/mark
parity handling based on a `[0x4ED]` mode byte (0 = no parity
adjustment, else strip bit 7, and for mode `3` specifically force bit
7 back on). This is a solid, concrete confirmation of the RS-232 (not
just GPIB) personality of the comm ROM's serial path.

### Direction correction: the ring buffer is a TX queue, not RX

Found while investigating `serial_tx_buffer_put` and `SUB_96872`: the
ring buffer at `[0x448]`(read)/`[0x44C]`(write), base `0xAF` size
`0x384`, has `serial_tx_buffer_put` as its PRODUCER (writes the next
byte to transmit and advances `[0x44C]`) and what was named
`service_comm_rx_queue` as its CONSUMER (reads via `[0x448]` and
forwards each byte on toward the real hardware tx path via
`enqueue_comm_char`). A "service_comm_**rx**_queue" name for the
consumer of a queue that `serial_tx_**buffer_put**` feeds was
backwards - renamed to `service_comm_tx_queue`, and its paired
initializer `init_comm_rx_queue_and_ready_flags` to `init_comm_tx_
queue_and_ready_flags`. No genuine incoming-data ring buffer has been
identified yet - worth keeping an eye out for one when tracing however
incoming RS-232/GPIB bytes actually reach the firmware (an interrupt
handler is the likely mechanism, not yet traced).

## A third decode anomaly: SUB_F6382, likely capstone misreading opcode 0x0F

`SUB_F6382` (`160-3532`, 3 far-call sites from `160-3633`) opens with
`0f 7e 05` -> capstone reads this as the SSE2/MMX instruction `movd
dword ptr [di], mm0`, impossible on an 8086/8088. Unlike `SUB_EAC86`
(pure garbage for many bytes with no coherent reconvergence - see
below), this one is a single misdecoded instruction: everything from
the byte right after it (`0xF6385` onward: `mov word [bp-0xa],
0x0f9a`) is completely ordinary, bp-relative compiled-C code, and the
function's tail (checking `[0x1B83]` against `0x14` - the same comm-
option-installed value from `detect_comm_option_hw`!) is entirely
coherent.

The likely explanation: on a real 8086/8088, opcode byte `0x0F` alone
is the **undocumented 1-byte `POP CS`** instruction, not a two-byte
SSE/MMX escape prefix (that reuse of `0x0F` didn't happen until the
80286). Capstone's 16-bit mode still decodes `0x0F` as a multi-byte
escape, so it's very likely misreading a real (if unusual/undocumented)
`POP CS` as `movd`. Support for this: decoding `0x0F` as a 1-byte
`pop cs` at `0xF6382`, the very next byte `0x7E` naturally starts a
fresh `jle` instruction at `0xF6383`, landing exactly on the same
`cmp byte [0x1B83], 0x14` block (`0xF638A`) that a separate fallthrough
path into this same code reaches via its own, differently-encoded
comparison - i.e. under the `pop cs` reading, both the far-call entry
and the fallthrough entry cleanly reconverge, which doesn't happen
under capstone's `movd` reading. (Whether a real `POP CS` immediately
after a far call is itself sound - it would clobber the just-pushed
return address - isn't resolved; possibly the routine's own `retf 4`
stack cleanup account for it, or this entry point is never actually
exercised on real hardware. Not pursued further.)

**Not renamed** pending a firmer resolution (either fix the generator
to treat bare `0x0F` as `pop cs` on this specific address and see if
the surrounding disassembly cleans up, or find corroborating evidence
this instruction is genuinely reached). Distinct from `SUB_EAC86`:
that one decodes as incoherent garbage for many consecutive
instructions with no clean reconvergence at any nearby byte shift,
while this one is a single misdecoded opcode immediately followed by
clean code.

## Found: the acquisition mode-change dispatcher (handle_acq_mode_change)

`SUB_E80E4` takes a single "what changed" flags word (arg at `[bp-8]`)
and dispatches on individual bits, each corresponding to one aspect of
acquisition/display state that just changed:

- bit `0x40` - acquisition timeout handling. If a countdown at
  `[0x54A]` has already expired (`<= 0`), calls
  `reset_acq_buffers_stub`; otherwise arms a fresh timeout by
  snapshotting the current scheduler tick (`scheduler_tick_service`'s
  `[0x752]`) into `[0x544]`, computing a deadline `[0x546] =
  [0x544] + 0x783` (0x783 = 1923 ticks), and setting bit 3 of
  `[0x1B76]` (the same flags byte `run_selftest_sequence`/
  `reinit_system_state` touch).
- bits `0x23`, `2`, `0x20` (and further ones past what's transcribed
  here) each gate their own small block of calls to
  `reset_acq_buffers_stub`, `print_and_reset_acq_buffers`,
  `update_display_mode_flags`, and `reset_plot_home_or_acq`, in
  different combinations depending on which bit(s) fired and the
  state of `[0x54C]`.

Renamed to `handle_acq_mode_change`. This is a good caller-side
confirmation that `[0x752]` (scheduler tick count) doubles as a
lightweight timebase for non-hardware timeouts elsewhere in the
firmware, not just the busy-wait use in `wait_readout_tick`.

**Left unresolved: `SUB_E804F`.** One of the two call sites for this
function's sibling (also reached from the plot-position-cache code
around `update_plot_position`/`plot_line_to`) is `SUB_E804F`, whose
first bytes (`1c 1d` = `sbb al, 0x1d`) don't form a recognizable
prologue, yet the function later executes `pop si` / `mov sp, bp` /
`pop bp` / `retf 2` - implying a stack frame that was never visibly
set up. Unlike the `write_hw_shift_register`/`convert_sample_value`
"shared tail via fallthrough" pattern, this one **can't** be a
fallthrough-entry function: there's a ~172-byte gap between the
previous function's `retf` (at `0xE7FA3`) and `SUB_E804F`'s start
(`0xE804F`) with nothing in the recursive-descent graph reaching
through it, and both known callers (`160-3633` and `160-3532`) target
`0xE804F` directly via `lcall`. The body between the odd opening and
the `retf` is otherwise coherent (looks up an entry in a table
pointed to by the far pointer at `[0x1D1C]`, indexed by a value
derived from the caller's argument, and conditionally writes a
"changed" flag + new value into it, setting global dirty flag
`[0x532]` if so) - functionally plausible as "update a cached
coordinate/id table entry and flag it dirty if the value changed,"
but the un-prologued opening means the register-level details (what's
really in `ax`/`bx` on entry) aren't trustworthy. Left unrenamed
pending a closer look; flagged here rather than guessed. Not
classified as a `SUB_EAC86`-style anomaly since the code past the
odd opening is coherent, not garbage.

## A second, more puzzling decode anomaly: SUB_EAC86

Found while renaming: `SUB_EAC86` (`160-3633`, in the *proven*, not
heuristic, set) decodes as unambiguous garbage from its very first
byte - `add bl,al` / `int1` / more nonsense, eventually including
`minps xmm4,xmm4` (an SSE instruction, decades newer than anything
this hardware could execute). This is a **different, more serious**
case than the already-documented `0xEA1A0-0xEA615` fallthrough cluster
above:

- It's **outside** that cluster's address range entirely.
- It's reached via a **clean, unambiguous far call** (`lcall 0xEA34:
  0x946`, computing to physical `0xEAC86` with no ambiguity), not
  fallthrough - and the *same* literal `(0xEA34, 0x946)` target is
  called from **4 separate places**: 3 times in `160-3532` (all inside
  one small routine near `0xF58B1`) and once from the comm ROM
  (`2998_alias_90000`, inside `SUB_924D2`/`build_comm_status_message`).
  A shared target called this consistently from two different ROMs
  strongly implies it's supposed to be a real, working function.
- **New: the calling convention is now fully understood**, even though
  the bytes at the target still aren't. All 4 call sites push the
  identical shape of arguments - two far pointers, `(dest_far_ptr,
  src_far_ptr)` (`push es; push offset` twice) - consistent with a
  small "append/copy a chunk from src to dest" utility. In `160-3532`,
  `src` is always the fixed string-table segment `0xFF7B` at offsets
  `0x20F`/`0x221`/`0x233`, each exactly `0x12` (18) bytes apart; reading
  those bytes shows **not printable text** but a repeating pattern of
  small negative-looking bytes (`0xF8`/`0xFB`/`0xFA`) terminated by
  `0xFF`, then pairs starting with `0x07` - the same shape as the
  already-documented vector/stroke-font draw-command data used by the
  readout display list (see "a second, separate print mechanism"
  below) - immediately followed in the table by the plain ASCII text
  `" TEKTRONIX  2220"`. This strongly suggests the caller is assembling
  a **boot-splash "TEKTRONIX" logo** by appending 3 stroke-data chunks
  in sequence (each call advances the destination offset by `0xE`=14
  bytes), i.e. `SUB_EAC86`'s *intended* role is something like
  `append_stroke_or_string_chunk(dest, src)`. This makes the "it's
  supposed to be a real function" case even stronger, but doesn't
  explain why its actual bytes are garbage - still not resolved.
- Checked for an off-by-a-few-bytes misalignment (the classic
  "recursive descent walked into the middle of an instruction"
  failure mode) by dumping the raw bytes a few positions before and
  after `0xEAC86` - no nearby shift produces a clean `55 8B EC`-style
  prologue either. The bytes genuinely look like non-code data at
  every alignment checked.

**Not resolved.** Left `SUB_EAC86` unrenamed rather than guess a
purpose for what might not be reachable code in practice - naming it
would imply confidence that isn't there. Candidate explanations, none
confirmed: (a) another address-decode alias like the confirmed
`0x90000` one, where the real hardware serves different bytes at this
physical address than our flat EPROM dump does; (b) genuinely dead
code (a stale far-call target left over from an earlier build) that
happens to still be called 3x but never actually executed in practice
(e.g. gated by a runtime condition that's always false); (c) some
other decode subtlety not yet identified. Worth revisiting if the
`0x90000`-alias-style brute-force technique (documented in
`CLAUDE.md`) ever gets pointed at this specific address.

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
