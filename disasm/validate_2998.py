#!/usr/bin/env python
"""
NASM round-trip validation for the comm ROM (160-2998-14.bin), reusing
validate_nasm.py's per-instruction conversion/comparison logic against
the virtual per-page chip set from gen_disasm_2998.py.

This validates instruction DECODE correctness the same way the main
ROM was validated. It does NOT validate whether the ~398 heuristically-
seeded entry points are real reachable code - that's a separate,
inherently weaker claim (see gen_disasm_2998.py's module docstring).
"""
import sys
import gen_disasm_2998 as c2998
import gen_disasm_x86 as g
import validate_nasm as v


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

    total = exact = alt_encoding = unconverted = 0
    real_mismatch = []

    for name in ("2998p0", "2998p1", "2998p2", "2998p3"):
        info = chips[name]
        src, slots = v.build_batch(name, info["buf"], visited, info["base"])
        src_path = f"_validate_{name}.asm"
        out_path = f"_validate_{name}.bin"
        open(src_path, "w").write(src)
        r = v.assemble(nasm_exe, src_path, out_path)
        if r.returncode != 0:
            print(f"=== {name} === NASM FAILED:\n{r.stderr[:3000]}")
            continue
        assembled = open(out_path, "rb").read()

        page_exact = page_alt = page_unconv = 0
        for i, (addr, size, mnem, op, orig, nasm_line) in enumerate(slots):
            total += 1
            slot_off = i * v.SLOT
            got = assembled[slot_off:slot_off + size]
            if nasm_line is None:
                unconverted += 1
                page_unconv += 1
                continue
            if got == orig:
                exact += 1
                page_exact += 1
                continue
            alt = v.alt_direction_encoding(got[:size])
            if alt is not None and alt == orig:
                alt_encoding += 1
                page_alt += 1
                continue
            alt2 = v.alt_displacement_encoding(orig)
            if alt2 is not None and got[:len(alt2)] == alt2:
                alt_encoding += 1
                page_alt += 1
                continue
            real_mismatch.append((name, addr, mnem, op, nasm_line, orig, got))

        print(f"=== {name} === ({len(slots)} instructions) exact={page_exact} "
              f"alt={page_alt} unconv={page_unconv} "
              f"mismatches={sum(1 for m in real_mismatch if m[0] == name)}")

    print()
    print(f"TOTAL comm-rom instructions: {total}")
    print(f"  exact byte match:   {exact}")
    print(f"  alt-encoding match: {alt_encoding}")
    print(f"  not converted:      {unconverted}")
    print(f"  real mismatches:    {len(real_mismatch)}")
    for m in real_mismatch[:30]:
        name, addr, mnem, op, nasm_line, orig, got = m
        print(f"  {name}:{addr:04x}  {mnem} {op!r} -> {nasm_line!r}  "
              f"orig={orig.hex()} got={got.hex()}")


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "nasm")
