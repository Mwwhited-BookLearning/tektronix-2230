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

## Comm ROM disassembly (160-2998-14)

Built with `gen_disasm_2998.py`, reusing `gen_disasm_x86.py`'s engine
with a combined chip set: the comm ROM's four 16KB pages get arbitrary
non-overlapping virtual physical bases (`0xA0000`, `0xB0000`, `0xC0000`,
`0xD0000` - a full 64KB apart per page, not just 16KB, so a near-branch
offset overflowing past `0x3FFF` lands in genuinely unmapped space
instead of bleeding into the next page's window) alongside the real
main-ROM chips, so a far call/jmp landing in `0xE0000-0xFFFFF`
correctly continues into the already-disassembled main ROM.

**Confidence is lower than the main ROM's disassembly.** The main ROM
is seeded only from proven entry points (the real reset vector, and
things reachable from it). The comm ROM has no known reset vector and
nothing we've disassembled calls into it directly, so it's seeded
heuristically: every occurrence of the `55 8B EC` (`push bp; mov
bp,sp`) prologue (398 of them) plus the two boot-stub far jumps. This
is a strong signal (C-compiler prologues are distinctive) but not
proof of reachability the way the main ROM's recursive descent is.

One genuinely high-confidence discovery came out of this, though: the
boot-stub far jump target (`0xE64C:0000`) is directly observed (both
comm-ROM pages 2 and 3 independently encode the identical jump), so it
was promoted into the *official* `gen_disasm_x86.py` `ENTRY_POINTS` as
`COMM_ROM_BOOTSTUB_TARGET` - this grew the main ROM's own confirmed
coverage by 29 instructions.

Result: 20,180 instructions reached, NASM-validated the same way as
the main ROM (`validate_2998.py`): 18,039 exact + 2,141 alt-encoding,
0 real mismatches. A buildable NASM source (`160-2998-14.asm`, via
`gen_source_2998.py`) reassembles byte-identical to the original .bin,
same guarantee as the main ROM's `.asm` files. Chasing validation
failures here surfaced two more real bugs in `validate_nasm.py`,
fixed and now benefiting both ROMs' validation:
- a segment-override prefix byte (`0x26`/`0x2E`/`0x36`/`0x3E`) was
  being read as if it were the instruction's opcode, which broke the
  immediate-width-widening check for any segment-overridden `add`/
  `or`/`cmp`/etc. with a memory destination (e.g. `or word
  [es:di+0xA], imm`).
- capstone omits the `0x` prefix on some near-branch targets too (not
  just far ones, which were already handled) - an unprefixed numeric
  target like a bare `"9"` was silently falling through to a bogus
  literal-address conversion instead of being resolved or safely
  rejected.

Because the comm ROM's page-relative addressing differs from the main
ROM's real absolute addressing, `gen_source_2998.py` needed one more
fix beyond reusing `gen_source.py`'s logic: `classify_instructions()`
resolves near-branch targets to page-relative addresses (correct for
validating one 16KB page in isolation via its own `vstart` section),
but the reconstruction concatenates all four pages under a single
`ORG 0` spanning the full 64KB file - so a page-relative target must
be shifted to the matching global file offset (`page*0x4000 +
page-relative value`) before being embedded, or it silently points at
the wrong page.

## The 0x80000-0x97FFF region: likely RAM, contents unknown

With the comm ROM fully disassembled, its far calls into the
previously-flagged "mystery" region turned out to be much larger than
first thought: **150+ distinct call targets** across dozens of
separate segments (`8006`, `802c`, `81ae`, `82c9`, `839f`, `8511`,
`85eb`, `911e`, `92cf`, `941f`, `9470`, `9628`, `9687`, `96f5`, `97c6`,
...), spanning `0x80000` to `0x97C95` - roughly 96KB. None of it
overlaps the confirmed main-ROM window, and TekWiki explicitly
confirms there is no third ROM chip for the 2230 (just the two 27512s
on A10 plus the comm option ROM), ruling out "it's a ROM we simply
don't have a dump of."

Direct evidence it's real, tested RAM rather than something exotic:
the main ROM contains a classic non-destructive memory-test sequence
targeting `es=0x8000` (`160-3633` offsets around `0x4544`/`0x4566`) -
read the current value, write the test pattern `0xAA55`, then (per the
disassembly around there) restore it - the standard way period BIOS/
POST code detects how much RAM is actually installed by probing
upward until the pattern stops sticking.

**Still open: how code gets into that RAM for the comm ROM to call.**
Went looking for a block-copy (`rep movsw`/`movsb`) moving bytes from
ROM into `0x80000+`; found a generic memcpy-style utility
(`SUB_FBC09`, appears in both `160-3532` and `160-3633`, called from 5
places) but the 2 call sites checked so far both copy within the
normal low-RAM globals area, not into the mystery region. Remaining
possibilities: the load happens via a call site not yet checked, via
a different mechanism entirely (downloaded over GPIB/RS-232 at
runtime, generated programmatically, or something else), or the RAM
simply isn't populated with anything meaningful unless a specific
hardware option is installed. **Whatever the mechanism, it's likely
invisible to static analysis of these three ROM dumps alone** - worth
factoring into any estimate of how much of this system can ultimately
be reverse-engineered from what we have.

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

Ran the recursive-descent output (10,360 decoded instructions, after
seeding the interrupt-handler entry points above) through
`validate_nasm.py`: **9,368 byte-exact, 988 provably-equivalent
alternate encodings, 0 real mismatches, 4 not independently checked.**
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

10,360 instruction-start bytes reached out of 131,072 total ROM bytes
(~8%), from 7 seed entry points (reset vector, 2 discovered while
tracing the reset path, 4 interrupt handlers found via IVT-write
tracing - see above). This is expected to still be a small fraction —
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
