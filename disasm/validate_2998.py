#!/usr/bin/env python
"""
NASM round-trip validation for the comm ROM (160-2998-14.bin) including
its heuristic entry points (see gen_disasm_2998.py), reusing
validate_nasm.py's per-instruction conversion/comparison logic.

This validates instruction DECODE correctness the same way the main
ROM was validated. It does NOT validate whether the heuristically-
seeded entry points are real reachable code - that's a separate,
inherently weaker claim (see gen_disasm_2998.py's module docstring).
"""
import sys

import gen_disasm_2998 as c2998
import gen_disasm_x86 as g
import validate_nasm as v


def main(nasm_exe):
    heuristic_entries = c2998.build_entry_points()[1]
    entry_points = g.ENTRY_POINTS + heuristic_entries
    chips, visited, labels = g.main(None, entry_points)

    info = chips["2998"]
    src, slots = v.build_batch("2998", info["buf"], visited, info["base"])
    src_path = "_validate_2998.asm"
    out_path = "_validate_2998.bin"
    open(src_path, "w").write(src)
    r = v.assemble(nasm_exe, src_path, out_path)
    if r.returncode != 0:
        print(f"NASM FAILED:\n{r.stderr[:4000]}")
        return
    assembled = open(out_path, "rb").read()

    total = exact = alt_encoding = unconverted = 0
    real_mismatch = []
    for i, (addr, size, mnem, op, orig, nasm_line) in enumerate(slots):
        total += 1
        slot_off = i * v.SLOT
        got = assembled[slot_off:slot_off + size]
        if nasm_line is None:
            unconverted += 1
            continue
        if got == orig:
            exact += 1
            continue
        alt = v.alt_direction_encoding(got[:size])
        if alt is not None and alt == orig:
            alt_encoding += 1
            continue
        alt2 = v.alt_displacement_encoding(orig)
        if alt2 is not None and got[:len(alt2)] == alt2:
            alt_encoding += 1
            continue
        alt3 = v.alt_duplicate_opcode(orig)
        if alt3 is not None and got[:len(alt3)] == alt3:
            alt_encoding += 1
            continue
        alt4 = v.alt_zero_displacement_encoding(orig)
        if alt4 is not None and got[:len(alt4)] == alt4:
            alt_encoding += 1
            continue
        real_mismatch.append((addr, mnem, op, nasm_line, orig, got))

    print(f"TOTAL comm-rom instructions: {total}")
    print(f"  exact byte match:   {exact}")
    print(f"  alt-encoding match: {alt_encoding}")
    print(f"  not converted:      {unconverted}")
    print(f"  real mismatches:    {len(real_mismatch)}")
    for addr, mnem, op, nasm_line, orig, got in real_mismatch[:30]:
        print(f"  2998:{addr:04x}  {mnem} {op!r} -> {nasm_line!r}  "
              f"orig={orig.hex()} got={got.hex()}")


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "nasm")
