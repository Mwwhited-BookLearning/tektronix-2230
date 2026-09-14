# Disassembly validation and coverage

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

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

**Proven-only** (official, what `sysrom_3532_3633.lst` shows): now
14,799 instruction-start bytes, from 23 seed entry points (the
original 8, plus 15 found this session via the RAM far-pointer init
table - see below).

**Correction to an old claim**: earlier sessions asserted "zero
unresolved indirect jmp/call instructions exist anywhere in the
reached code" as evidence this codebase prefers `cmp`+`je` dispatch
chains over jump tables. **That's no longer accurate** - see "Found:
indirect jumps/calls through computed pointers" below for the full,
current picture (several *are* resolved dispatch tables; a few are
genuinely unresolved; and a cluster of them turned out to be the same
landing-artifact anomaly already documented elsewhere, not real jump
tables at all).

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

**Corrected/deduplicated re-measurement** (`disasm/compute_coverage.py`,
new): the 85.6% figure above double-counts the comm ROM, since
`2998_alias_90000` is the *same underlying bytes* as the upper half of
`2998`, not separate physical storage - it treats what's really
3×64KB=196,608 total bytes as if it were 229,376. Folding the alias
hits back onto `2998`'s own bytes gives a cleaner picture:

| | proven-only | proven + heuristic |
|---|---|---|
| `3633` | 37.4% (24,495 B) | 89.9% (58,896 B) |
| `3532` | 7.6% (4,967 B) | 90.6% (59,372 B) |
| `2998` | 13.0% (8,525 B) | 91.7% (60,092 B) |
| **all 3 chips** | **19.3% (37,987 / 196,608 B)** | **90.7% (178,360 / 196,608 B)** |

So: the 278-function *proven* set this session's renaming pass has been
working through is a call-graph-verified ~19% of the total ROM bytes.
Layering the lower-confidence heuristic push-bp scan on top reaches
~91% of all bytes. The remaining **~9.3% (~18,250 bytes)** across the
3 chips isn't reached by either method - almost certainly a mix of
data tables (string tables, the stroke-font/print-record templates,
menu-string blobs already catalogued in `STRINGS.md`) and genuinely
unreached code (dead code, or code behind an entry point not yet
found). Not further broken down byte-by-byte; `compute_coverage.py`
re-derives these numbers on demand rather than needing to be kept
in sync by hand.

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
