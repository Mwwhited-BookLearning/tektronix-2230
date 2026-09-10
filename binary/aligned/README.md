# Aligned binaries - NOT ORIGINAL FIRMWARE

Everything in this directory is a **reconstructed** binary, generated
by `disasm/gen_source_readable.py` assembling
`disasm/160-*-14_readable.asm` with NASM. It is **not** a dump from
real hardware and must never be treated as one.

## What "aligned" means

For every decoded instruction, if NASM's own preferred encoding is
*shorter* than the byte sequence actually found in the true original
ROM (the immediate-width / displacement-width / zero-displacement
encoding ambiguities documented in `disasm/NOTES.md`), the shorter
form is padded back out to the original length with explicit `NOP`
(`0x90`) bytes. That keeps every address *after* the substitution
identical to the true original, so every jump/call target and data
reference elsewhere in the ROM stays correct - at the cost of the
substituted bytes themselves no longer matching.

Net effect per chip (measured against `binary/160-*-14.bin`):

| Chip | Bytes differing | Total size |
|------|-----------------|------------|
| 160-3633-14 | 1,432 | 65,536 |
| 160-3532-14 | 324 | 65,536 |
| 160-2998-14 | 108 | 65,536 |

Lengths always match the true original exactly; only those specific
substituted bytes differ, and only in ways that are semantically
equivalent (NASM assembling its own preferred encoding of the same
instruction, or a NOP standing in for nothing).

## Why this exists

Requested directly: produce a version that's easier to read (no raw
`db` fallback for anything actually decoded) while staying address-
compatible with the true original, using NOPs to absorb the length
difference where NASM's canonical encoding is shorter. See
`disasm/gen_source_readable.py`'s module docstring for the mechanics.

## Status: provisional, review later

This is being adopted as the reference binary for *future*
disassembly/validation checks (easier to keep in sync with the
symbol table than re-deriving encoding equivalence each time), but
that choice has not been fully validated yet. **Review again once the
rest of the analysis is further along** (self-test subroutine ID,
menu tree, I/O port mapping) to confirm nothing was missed - in
particular whether any self-checksum routine in the firmware (at
least two are known to exist, see `disasm/NOTES.md`) would behave
differently against these bytes. That question is moot for our
purposes (nothing here ever runs on real hardware), but it's the kind
of thing worth double-checking before leaning on this file for
anything beyond readability.

`binary/160-*-14.bin` (the parent directory, not this one) remains the
one and only ground truth for byte-fidelity purposes.

## Regenerating

```
cd disasm
python gen_source_readable.py nasm
```
