#!/usr/bin/env python
"""
NASM round-trip validation of the FULLY combined pass: official entry
points + the comm ROM's heuristic layer (gen_disasm_2998.py) + the
main ROM's heuristic layer (gen_disasm_mainrom_heuristic.py), across
all four mapped chips. Reuses validate_nasm.py's per-instruction
conversion/comparison logic (same as validate_nasm.py/validate_2998.py
do individually) but in one pass covering everything at once.
"""
import sys

import gen_disasm_x86 as g
import gen_disasm_2998 as c2998
import gen_disasm_mainrom_heuristic as mh
import validate_nasm as v


def main(nasm_exe):
    all_heuristic = c2998.build_entry_points()[1] + mh.build_entry_points()
    entry_points = g.ENTRY_POINTS + all_heuristic
    chips, visited, labels = g.main(None, entry_points)

    total = exact = alt_encoding = unconverted = 0
    real_mismatch = []

    for name, info in chips.items():
        src, slots = v.build_batch(name, info["buf"], visited, info["base"])
        src_path = f"_validate_{name}.asm"
        out_path = f"_validate_{name}.bin"
        open(src_path, "w").write(src)
        r = v.assemble(nasm_exe, src_path, out_path)
        if r.returncode != 0:
            print(f"=== {name} === NASM FAILED:\n{r.stderr[:3000]}")
            continue
        assembled = open(out_path, "rb").read()

        chip_exact = chip_alt = chip_unconv = 0
        chip_mismatch = 0
        for i, (addr, size, mnem, op, orig, nasm_line) in enumerate(slots):
            total += 1
            slot_off = i * v.SLOT
            got = assembled[slot_off:slot_off + size]
            if nasm_line is None:
                unconverted += 1
                chip_unconv += 1
                continue
            if got == orig:
                exact += 1
                chip_exact += 1
                continue
            alt = v.alt_direction_encoding(got[:size])
            if alt is not None and alt == orig:
                alt_encoding += 1
                chip_alt += 1
                continue
            alt2 = v.alt_displacement_encoding(orig)
            if alt2 is not None and got[:len(alt2)] == alt2:
                alt_encoding += 1
                chip_alt += 1
                continue
            alt3 = v.alt_duplicate_opcode(orig)
            if alt3 is not None and got[:len(alt3)] == alt3:
                alt_encoding += 1
                chip_alt += 1
                continue
            alt4 = v.alt_zero_displacement_encoding(orig)
            if alt4 is not None and got[:len(alt4)] == alt4:
                alt_encoding += 1
                chip_alt += 1
                continue
            alt5 = v.alt_xchg_encoding(got[:size])
            if alt5 is not None and alt5 == orig:
                alt_encoding += 1
                chip_alt += 1
                continue
            chip_mismatch += 1
            real_mismatch.append((name, addr, mnem, op, nasm_line, orig, got))

        print(f"=== {name} === ({len(slots)} instructions) exact={chip_exact} "
              f"alt={chip_alt} unconv={chip_unconv} mismatches={chip_mismatch}")

    print()
    print(f"TOTAL: {total} instructions")
    print(f"  exact byte match:   {exact}")
    print(f"  alt-encoding match: {alt_encoding}")
    print(f"  not converted:      {unconverted}")
    print(f"  real mismatches:    {len(real_mismatch)}")
    for name, addr, mnem, op, nasm_line, orig, got in real_mismatch[:40]:
        print(f"  {name}:{addr:04x}  {mnem} {op!r} -> {nasm_line!r}  "
              f"orig={orig.hex()} got={got.hex()}")


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "nasm")
