#!/usr/bin/env python
"""
Emit a real, buildable NASM source reconstruction of each main-ROM chip.

Unlike sysrom_3532_3633.lst (an annotated diagnostic listing) and
validate_nasm.py's per-instruction test harness (isolated padded slots
for independent comparison), this produces one clean, contiguous
source file per chip that:
  - uses the validated NASM conversion for every instruction proven
    BYTE-EXACT by validate_nasm.py (see its "exact" bucket),
  - falls back to raw `db` bytes (with a comment showing the decoded
    mnemonic) for anything only proven equivalent via an alternate
    encoding, anything not converted at all, and every byte not
    reached by the recursive descent (unclassified code or data) -
    guaranteeing the assembled output is byte-identical to the
    original ROM regardless of any encoding-choice ambiguity,
  - labels ASCII string runs with a comment for readability.

Run, then verify with NASM before trusting the output.
"""
import re
import gen_disasm_x86 as g
import validate_nasm as v

STRING_RUN_RE = re.compile(rb"[ -~]{5,}")


def find_string_regions(buf):
    return [(m.start(), m.end()) for m in STRING_RUN_RE.finditer(buf)]


def classify_instructions(chip_name, buf, visited, chip_base, nasm_exe):
    """Return {chip_off: (size, nasm_line_or_None, mnem, op)} for
    instructions that assemble to their ORIGINAL bytes exactly. Anything
    else (alt-encoding only, unconverted, or a real mismatch) is left
    out so the caller falls back to raw db bytes for it."""
    entries = [e for e in visited.values() if e["chip"] == chip_name]
    src, slots = v.build_batch(chip_name, buf, visited, chip_base)
    src_path = f"_src_check_{chip_name}.asm"
    out_path = f"_src_check_{chip_name}.bin"
    open(src_path, "w").write(src)
    r = v.assemble(nasm_exe, src_path, out_path)
    if r.returncode != 0:
        raise RuntimeError(f"NASM failed for {chip_name}:\n{r.stderr}")
    assembled = open(out_path, "rb").read()

    exact = {}
    for i, (addr, size, mnem, op, orig, nasm_line) in enumerate(slots):
        if nasm_line is None:
            continue
        slot_off = i * v.SLOT
        got = assembled[slot_off:slot_off + size]
        if got == orig:
            exact[addr] = (size, nasm_line, mnem, op)
    return exact


def build_source(chip_name, buf, visited, chip_base, nasm_exe):
    exact = classify_instructions(chip_name, buf, visited, chip_base, nasm_exe)
    string_regions = find_string_regions(buf)
    string_start = {lo: hi for lo, hi in string_regions}
    by_chip_off = {e["chip_off"]: e for e in visited.values() if e["chip"] == chip_name}

    lines = [
        "; Tektronix 2230 main system ROM - reconstructed NASM source",
        f"; Source file: 160-{chip_name}-14.bin, physical base "
        f"0x{chip_base:05X}",
        "; Assembling this file (nasm -f bin) MUST reproduce the",
        "; original .bin byte-for-byte - see disasm/NOTES.md for how",
        "; this was validated (disasm/validate_nasm.py).",
        ";",
        "; Instructions proven byte-exact by validate_nasm.py are",
        "; written as real NASM source; everything else (alternate-",
        "; encoding-only matches, unconverted instructions, and",
        "; unreached bytes/data) is emitted as raw `db` so the",
        "; rebuild is guaranteed to be byte-identical regardless of",
        "; any x86 encoding-choice ambiguity.",
        "",
        "BITS 16",
        "ORG 0",
        "",
    ]

    def flush_filler(lines, start, chunk):
        """Emit a run of unclassified bytes, 16 per line, so the file
        stays readable instead of one `db` per byte."""
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
        lab = labels_by_addr.get((chip_name, addr))

        if addr in exact:
            flush_pending()
            if lab:
                lines.append(f"{lab}:")
            size, nasm_line, mnem, op = exact[addr]
            lines.append(f"    {nasm_line:<40s} ; {addr:04X}: {mnem} {op}")
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
                            f"(not byte-exact via NASM - see NOTES.md)")
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
    """`only_chips`, if given, restricts which chips' .asm files get
    written - useful when `entry_points` includes lower-confidence
    (e.g. heuristic) seeds that should expand what's explored WITHOUT
    mixing that lower confidence into another chip's committed .asm
    (e.g. the comm ROM's heuristic entries can open up new reachable
    code in the main ROM chips too, but that shouldn't silently change
    what 160-3633-14.asm/160-3532-14.asm claim to be proven-reachable)."""
    global labels_by_addr
    chips, visited, labels = g.main(None, entry_points)

    labels_by_addr = {}
    for phys, lab in labels.items():
        name = g.FUNCTIONAL_NAMES.get(phys) or lab.get("fixed_name") or (
            ("SUB_%05X" % phys) if lab["kind"] == "sub" else ("L_%05X" % phys))
        chip_name, chip_off = v.g.phys_to_chip_offset(chips, phys)
        if chip_name:
            labels_by_addr[(chip_name, chip_off)] = name

    for name, info in chips.items():
        if only_chips is not None and name not in only_chips:
            continue
        if "alias" in name:
            # a view into bytes another chip already owns a committed
            # .asm for (e.g. the 0x90000 comm-ROM alias) - nothing new
            # to write out here.
            continue
        src = build_source(name, info["buf"], visited, info["base"], nasm_exe)
        out_path = f"160-{name}-14.asm"
        open(out_path, "w").write(src)
        print(f"wrote {out_path} ({len(src)} bytes)")

        # final verification: does this file alone reassemble to the
        # exact original binary?
        check_bin = f"_final_check_{name}.bin"
        r = v.assemble(nasm_exe, out_path, check_bin)
        if r.returncode != 0:
            print(f"  NASM FAILED on final source: {r.stderr[:2000]}")
            continue
        assembled = open(check_bin, "rb").read()
        if assembled == info["buf"]:
            print(f"  VERIFIED byte-identical to 160-{name}-14.bin")
        else:
            print(f"  MISMATCH: assembled {len(assembled)} bytes vs "
                  f"original {len(info['buf'])} bytes")
            for i in range(min(len(assembled), len(info["buf"]))):
                if assembled[i] != info["buf"][i]:
                    print(f"    first diff at 0x{i:04x}: "
                          f"orig={info['buf'][i]:02x} got={assembled[i]:02x}")
                    break


if __name__ == "__main__":
    import sys
    main(sys.argv[1] if len(sys.argv) > 1 else "nasm")
