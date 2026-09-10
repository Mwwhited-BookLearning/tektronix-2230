#!/usr/bin/env python
"""
Emit a fully human-readable NASM reconstruction: every decoded
instruction appears as real assembly text (never a raw `db` fallback,
unlike gen_source.py), at the cost of NOT being byte-identical to the
original ROM.

For any instruction where NASM's canonical encoding is SHORTER than
the original (the immediate-width, displacement-width, and zero-
displacement-collapse ambiguities documented in disasm/NOTES.md), the
shorter encoding is padded out to the original length with explicit
NOP (0x90) bytes. NOP is semantically transparent, so:
  - every instruction after it still lands at the exact same address
    it did in the original ROM (nothing shifts), so every jump/call
    target and data reference throughout BOTH chips stays valid, and
  - the CPU executes the padded version identically to the original,
    give or take a few no-op cycles.

What this does NOT preserve: the original bytes themselves. Anything
that checksums its own code (this firmware demonstrably does, at
least twice - see disasm/NOTES.md) would detect this reconstruction
as "different" even though it runs the same. Use gen_source.py's
output (160-3633-14.asm/160-3532-14.asm) when byte-fidelity matters;
use this one when you want to read something with zero db-encoded
instructions in it.

Same underlying analysis as gen_source.py + validate_nasm.py; only the
choice of what to do with a non-exact-but-understood instruction
differs.

PROVISIONAL: the assembled output of this script (binary/aligned/) is
being adopted as the reference binary for future disassembly/
validation checks, on the theory that a NOP-aligned, fully-decoded
image is easier to keep in sync with the growing symbol table than
re-deriving alternate-encoding equivalence every time. This is a
methodology choice, not a proven-safe one yet - REVIEW LATER, once the
rest of the analysis (self-test subroutine ID, menu tree, I/O port
mapping) is further along, to confirm nothing downstream actually
depended on byte-for-byte originality (e.g. a self-checksum routine
choking on the substituted bytes if this were ever run on real
hardware - it never should be, this is an analysis artifact only).
See binary/aligned/README.md.
"""
import os
import gen_disasm_x86 as g
import gen_source as gs
import validate_nasm as v

def alt_encoding_bytes(mnem, op, orig, got, size):
    """Return the NASM-preferred encoding bytes for this instruction if
    it matches one of the known alternate-encoding classes, else None.
    Mirrors validate_nasm.py's own mismatch-classification logic but
    returns the winning alternate's bytes instead of just a yes/no."""
    alt = v.alt_direction_encoding(got[:size])
    if alt is not None and alt == orig:
        return alt
    alt2 = v.alt_displacement_encoding(orig)
    if alt2 is not None and got[:len(alt2)] == alt2:
        return alt2
    alt3 = v.alt_duplicate_opcode(orig)
    if alt3 is not None and got[:len(alt3)] == alt3:
        return alt3
    alt4 = v.alt_zero_displacement_encoding(orig)
    if alt4 is not None and got[:len(alt4)] == alt4:
        return alt4
    return None


def classify_readable(chip_name, buf, visited, chip_base, nasm_exe):
    """Return {chip_off: (orig_size, nasm_line, mnem, op, pad_bytes)}.
    pad_bytes is how many trailing NOPs to emit after the instruction
    to make up the original length - 0 for exact matches and same-
    length alt-encodings, >0 for shorter alt-encodings, and the entry
    is omitted entirely (caller falls back to raw db) for anything not
    converted or a genuine unexplained mismatch."""
    src, slots = v.build_batch(chip_name, buf, visited, chip_base)
    src_path = f"_src_check_{chip_name}.asm"
    out_path = f"_src_check_{chip_name}.bin"
    open(src_path, "w").write(src)
    r = v.assemble(nasm_exe, src_path, out_path)
    if r.returncode != 0:
        raise RuntimeError(f"NASM failed for {chip_name}:\n{r.stderr}")
    assembled = open(out_path, "rb").read()

    result = {}
    for i, (addr, size, mnem, op, orig, nasm_line) in enumerate(slots):
        if nasm_line is None:
            continue
        slot_off = i * v.SLOT
        got = assembled[slot_off:slot_off + size]
        if got == orig:
            result[addr] = (size, nasm_line, mnem, op, 0)
            continue
        alt_bytes = alt_encoding_bytes(mnem, op, orig, got, size)
        if alt_bytes is not None:
            pad = size - len(alt_bytes)
            if pad >= 0:
                result[addr] = (size, nasm_line, mnem, op, pad)
        # else: genuine mismatch or unexplained - omitted, falls back to db
    return result


def build_source(chip_name, buf, visited, chip_base, nasm_exe):
    readable = classify_readable(chip_name, buf, visited, chip_base, nasm_exe)
    string_regions = gs.find_string_regions(buf)
    string_start = {lo: hi for lo, hi in string_regions}
    by_chip_off = {e["chip_off"]: e for e in visited.values() if e["chip"] == chip_name}

    lines = [
        f"; Tektronix 2230 ROM chip {chip_name} - READABLE reconstruction",
        "; NOT byte-identical to the original .bin - see this file's",
        "; generator (disasm/gen_source_readable.py) for why. Every",
        "; decoded instruction appears as real assembly (no raw `db`",
        "; fallback for anything actually understood); a handful of",
        "; instructions are followed by explicit NOP padding to keep",
        "; every address after them unchanged from the original.",
        ";",
        "; For a byte-identical reconstruction, see 160-<chip>-14.asm",
        "; (gen_source.py) instead.",
        "",
        "BITS 16",
        "ORG 0",
        "",
    ]

    def flush_filler(lines, start, chunk):
        for i in range(0, len(chunk), 16):
            row = chunk[i:i + 16]
            raw = ", ".join(f"0x{b:02x}" for b in row)
            lines.append(f"    db {raw}  ; {start + i:04X}")

    addr = 0
    n = len(buf)
    filler_start = None
    filler_bytes = bytearray()

    def flush_pending():
        nonlocal filler_start, filler_bytes
        if filler_bytes:
            flush_filler(lines, filler_start, filler_bytes)
            filler_bytes = bytearray()
            filler_start = None

    while addr < n:
        lab = gs.labels_by_addr.get((chip_name, addr))

        if addr in readable:
            flush_pending()
            if lab:
                lines.append(f"{lab}:")
            size, nasm_line, mnem, op, pad = readable[addr]
            lines.append(f"    {nasm_line:<40s} ; {addr:04X}: {mnem} {op}")
            if pad:
                lines.append(f"    times {pad} nop"
                              f"  ; padding to preserve address alignment")
            addr += size
            continue

        if addr in string_start:
            flush_pending()
            if lab:
                lines.append(f"{lab}:")
            hi = string_start[addr]
            text = buf[addr:hi].decode("ascii")
            raw = ", ".join(f"0x{b:02x}" for b in buf[addr:hi])
            lines.append(f"    db {raw}  ; {addr:04X}: \"{text}\"")
            addr = hi
            continue

        e = by_chip_off.get(addr)
        if e is not None:
            flush_pending()
            if lab:
                lines.append(f"{lab}:")
            size = e["size"]
            raw = buf[addr:addr + size]
            lines.append(f"    db " + ", ".join(f"0x{b:02x}" for b in raw)
                          + f"  ; {addr:04X}: {e['mnem']} {e['op']} "
                            f"(not converted or a genuine mismatch)")
            addr += size
            continue

        if lab:
            flush_pending()
            lines.append(f"{lab}:")
        if filler_start is None:
            filler_start = addr
        filler_bytes.append(buf[addr])
        addr += 1

    flush_pending()
    return "\n".join(lines) + "\n"


def main(nasm_exe, entry_points=None, only_chips=None):
    chips, visited, labels = g.main(None, entry_points)

    gs.labels_by_addr = {}
    for phys, lab in labels.items():
        name = lab.get("fixed_name") or g.FUNCTIONAL_NAMES.get(phys) or (
            ("SUB_%05X" % phys) if lab["kind"] == "sub" else ("L_%05X" % phys))
        chip_name, chip_off = v.g.phys_to_chip_offset(chips, phys)
        if chip_name:
            gs.labels_by_addr[(chip_name, chip_off)] = name

    for name, info in chips.items():
        if only_chips is not None and name not in only_chips:
            continue
        if "alias" in name:
            continue
        src = build_source(name, info["buf"], visited, info["base"], nasm_exe)
        out_path = f"160-{name}-14_readable.asm"
        open(out_path, "w").write(src)
        print(f"wrote {out_path} ({len(src)} bytes)")

        os.makedirs("../binary/aligned", exist_ok=True)
        aligned_bin = f"../binary/aligned/160-{name}-14_aligned.bin"
        r = v.assemble(nasm_exe, out_path, aligned_bin)
        if r.returncode != 0:
            print(f"  NASM FAILED: {r.stderr[:2000]}")
            continue
        assembled = open(aligned_bin, "rb").read()
        same_length = len(assembled) == len(info["buf"])
        diffs = sum(1 for a, b in zip(assembled, info["buf"]) if a != b)
        print(f"  wrote {aligned_bin} - length {'MATCHES' if same_length else 'MISMATCH'} "
              f"original ({len(assembled)} vs {len(info['buf'])} bytes); "
              f"{diffs} bytes differ from the true original (expected - "
              f"NOT byte-identical by design, see binary/aligned/README.md)")


if __name__ == "__main__":
    import sys
    main(sys.argv[1] if len(sys.argv) > 1 else "nasm")
