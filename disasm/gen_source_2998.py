#!/usr/bin/env python
"""
Emit a buildable NASM source reconstruction of the comm/GPIB-RS232
option ROM (160-2998-14.bin), the same way gen_source.py does for the
main ROM pair - one real, assembleable file that reassembles byte-
identical to the original .bin (verified on every run), with decoded
instructions proven byte-exact by validate_2998.py written as real
code and everything else (alt-encoding-only, unconverted, unreached)
falling back to raw `db` bytes.

Caveat (see gen_disasm_2998.py's docstring and disasm/NOTES.md): the
entry points behind this ROM's code are heuristic (push-bp signature
scan), not proven reachable like the main ROM's are. Byte-fidelity of
the *reconstruction* doesn't depend on that - the db fallback covers
anything not confirmed - but treat the labeled function boundaries
themselves as lower-confidence than the main ROM's.
"""
import re

import gen_disasm_2998 as c2998
import gen_disasm_x86 as g
import gen_source as gs
import validate_nasm as v

STRING_RUN_RE = re.compile(rb"[ -~]{5,}")

# classify_instructions() resolves near-branch targets to PAGE-RELATIVE
# addresses (0x0000-0x3FFF), correct for validating one page in
# isolation. The final reconstruction concatenates all four pages under
# a single ORG 0 spanning the full 64KB file, so a page-relative target
# must be shifted to the matching GLOBAL file offset (page*0x4000 +
# page-relative value) before it's embedded - otherwise e.g. a real
# "jmp 0x188" inside page 1 gets misassembled as a jump to global
# offset 0x188 (which is inside PAGE 0), silently producing wrong bytes
# despite classify_instructions having verified the *page-relative*
# encoding was byte-exact.
NEAR_BRANCH_MNEMONICS = {
    "jmp", "call", "loop",
    "je", "jne", "jg", "jge", "jl", "jle", "ja", "jae", "jb", "jbe",
}
TRAILING_HEX_RE = re.compile(r"(0x[0-9a-fA-F]+)$")


def globalize_branch_target(mnem, nasm_line, page):
    if mnem not in NEAR_BRANCH_MNEMONICS:
        return nasm_line
    m = TRAILING_HEX_RE.search(nasm_line)
    if not m:
        return nasm_line
    page_relative = int(m.group(1), 16)
    global_addr = page * c2998.PAGE_SIZE + page_relative
    return nasm_line[:m.start()] + f"0x{global_addr:04x}"


def main(nasm_exe):
    buf, page_defs = c2998.build_chip_defs()
    chip_defs = dict(g.CHIPS)
    chip_defs.update(page_defs)

    def load_chips_patched(defs):
        data = {}
        for name, info in defs.items():
            if "_buf" in info:
                data[name] = {"buf": info["_buf"], "base": info["phys_base"],
                               "size": len(info["_buf"])}
            else:
                b = open(info["path"], "rb").read()
                data[name] = {"buf": b, "base": info["phys_base"], "size": len(b)}
        return data

    g.load_chips = load_chips_patched
    entry_points = c2998.build_entry_points(buf, chip_defs) + g.ENTRY_POINTS
    chips, visited, labels = g.main(chip_defs, entry_points)

    labels_by_full_offset = {}
    for phys, lab in labels.items():
        name = lab.get("fixed_name") or (
            ("SUB_%05X" % phys) if lab["kind"] == "sub" else ("L_%05X" % phys))
        chip_name, chip_off = g.phys_to_chip_offset(chips, phys)
        if chip_name and chip_name.startswith("2998p"):
            page = int(chip_name[len("2998p"):])
            full_offset = page * c2998.PAGE_SIZE + chip_off
            labels_by_full_offset[full_offset] = name

    # per-page exact-match sets, keyed by page-relative offset
    exact_by_page = {}
    for page in range(4):
        name = f"2998p{page}"
        page_buf = chip_defs[name]["_buf"]
        exact_by_page[page] = gs.classify_instructions(
            name, page_buf, visited, chip_defs[name]["phys_base"], nasm_exe)

    string_regions = gs.find_string_regions(buf)
    string_start = {lo: hi for lo, hi in string_regions}

    lines = [
        "; Tektronix 2230 comm/GPIB-RS232 option ROM - reconstructed NASM source",
        "; Source file: 160-2998-14.bin",
        "; Assembling this file (nasm -f bin) MUST reproduce the original",
        "; .bin byte-for-byte (verified by this generator on every run).",
        ";",
        "; UNCONFIRMED address mapping (see disasm/NOTES.md): this ROM's",
        "; bank-switch window into the CPU's real address space isn't",
        "; known, so instruction labels here are page-relative, not real",
        "; addresses. Entry points were found heuristically (push-bp",
        "; signature scan), not proven reachable like the main ROM's -",
        "; treat labeled function boundaries as lower-confidence than",
        "; 160-3633-14.asm/160-3532-14.asm.",
        "",
        "BITS 16",
        "ORG 0",
        "",
    ]

    addr = 0
    n = len(buf)
    filler_start = None
    filler_bytes = bytearray()

    def flush_pending():
        nonlocal filler_start, filler_bytes
        if filler_bytes:
            for i in range(0, len(filler_bytes), 16):
                row = filler_bytes[i:i + 16]
                raw = ", ".join(f"0x{b:02x}" for b in row)
                lines.append(f"    db {raw}  ; {filler_start + i:04X}")
            filler_bytes = bytearray()
            filler_start = None

    while addr < n:
        page = addr // c2998.PAGE_SIZE
        page_off = addr % c2998.PAGE_SIZE
        lab = labels_by_full_offset.get(addr)
        exact = exact_by_page[page]

        if page_off in exact:
            flush_pending()
            if lab:
                lines.append(f"{lab}:")
            size, nasm_line, mnem, op = exact[page_off]
            nasm_line = globalize_branch_target(mnem, nasm_line, page)
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

        if lab:
            flush_pending()
            lines.append(f"{lab}:")
        if filler_start is None:
            filler_start = addr
        filler_bytes.append(buf[addr])
        addr += 1

    flush_pending()
    src = "\n".join(lines) + "\n"

    out_path = "160-2998-14.asm"
    open(out_path, "w").write(src)
    print(f"wrote {out_path} ({len(src)} bytes)")

    check_bin = "_final_check_2998.bin"
    r = v.assemble(nasm_exe, out_path, check_bin)
    if r.returncode != 0:
        print(f"  NASM FAILED: {r.stderr[:2000]}")
        return
    assembled = open(check_bin, "rb").read()
    if assembled == buf:
        print("  VERIFIED byte-identical to 160-2998-14.bin")
    else:
        print(f"  MISMATCH: assembled {len(assembled)} vs original {len(buf)}")
        for i in range(min(len(assembled), len(buf))):
            if assembled[i] != buf[i]:
                print(f"    first diff at 0x{i:04x}: orig={buf[i]:02x} got={assembled[i]:02x}")
                break


if __name__ == "__main__":
    import sys
    main(sys.argv[1] if len(sys.argv) > 1 else "nasm")
