#!/usr/bin/env python
"""
Heuristic-entry-point supplement for the main ROM pair (160-3633 /
160-3532), the same technique that worked well for the comm ROM
(gen_disasm_2998.py): scan for the `55 8B EC` (`push bp; mov bp,sp`)
C-compiler prologue signature and seed every occurrence not already
proven-reachable as an additional entry point.

Unlike the comm ROM, the main ROM already HAS a confirmed, proven
entry point (the real reset vector) and ~40% of it is already reached
that way. This is purely a coverage-widening supplement for whatever
the proven recursive descent hasn't reached yet - same lower-confidence
caveat as the comm ROM's heuristic pass applies: a `push bp;mov bp,sp`
match is a strong signal but not proof the code is actually reachable
in practice (dead code, alternate build config, etc. could explain a
false match, though empirically these have been very reliable so far).

Produces sysrom_3532_3633_heuristic.lst - a SEPARATE file from the
official sysrom_3532_3633.lst (which stays proven-only). Does not
touch the committed 160-3633-14.asm/160-3532-14.asm either - those
stay proven-only too, via gen_source.py's only_chips parameter.
"""
import re

import gen_disasm_x86 as g

PROLOGUE = b"\x55\x8b\xec"


def build_entry_points():
    """Return heuristic entries for every push-bp signature in 3633/3532
    that isn't already proven-reachable, so this can be layered on top
    of (not instead of) the official entry points."""
    chips = g.load_chips()
    _, proven_visited, _ = g.main()
    entries = []
    for name in ("3633", "3532"):
        buf = chips[name]["buf"]
        base = chips[name]["base"]
        seg = base >> 4  # both chips are paragraph-aligned (base & 0xF == 0)
        for off in re.finditer(re.escape(PROLOGUE), buf):
            start = off.start()
            phys = base + start
            if phys in proven_visited:
                continue  # already covered by the proven descent
            entries.append((seg, start, f"FUNC_{name}_{start:04X}"))
    return entries


def render_mainrom_only(chips, visited, labels, out_path, sym_path):
    import json
    for phys, lab in labels.items():
        if phys in g.FUNCTIONAL_NAMES:
            lab["name"] = g.FUNCTIONAL_NAMES[phys]
        elif "fixed_name" in lab:
            lab["name"] = lab["fixed_name"]
        else:
            lab["name"] = ("SUB_%05X" % phys) if lab["kind"] == "sub" else ("L_%05X" % phys)

    entries = sorted((e for e in visited.values() if e["chip"] in ("3633", "3532")),
                      key=lambda e: e["phys"])

    with open(out_path, "w") as f:
        f.write("; Tektronix 2230 main system ROM - WITH HEURISTIC LAYER\n")
        f.write("; This is NOT the official listing (that's sysrom_3532_3633.lst,\n")
        f.write("; proven-reachable only). This file ALSO includes every push-bp/\n")
        f.write("; mov-bp,sp signature match not otherwise proven reachable - a\n")
        f.write("; strong signal (real compiled functions), NOT proof of actual\n")
        f.write("; reachability, same caveat as 160-2998-14.lst's heuristic layer.\n")
        f.write("; Entries named FUNC_<chip>_<offset> are heuristic; everything\n")
        f.write("; else (SUB_/L_/named entries) is proven, same as the official\n")
        f.write("; listing.\n\n")

        last_phys = None
        for e in entries:
            phys = e["phys"]
            if last_phys is not None and phys != last_phys:
                f.write("\n")
            lab = labels.get(phys)
            if lab:
                f.write(f"{lab['name']}:\n")

            op = e["op"]
            mnem = e["mnem"]
            resolved = None
            if mnem in ("jmp", "call") and op.startswith("0x"):
                toff = int(op, 16)
                tphys = ((e["seg"] << 4) + toff) & 0xFFFFF
                resolved = labels.get(tphys, {}).get("name")
            elif mnem in ("ljmp", "lcall"):
                m = g.FAR_TARGET_RE.match(op)
                if m:
                    tseg, toff = int(m.group(1), 16), int(m.group(2), 16)
                    tphys = ((tseg << 4) + toff) & 0xFFFFF
                    resolved = labels.get(tphys, {}).get("name")
                    if resolved is None and 0x80000 <= tphys < 0x98000:
                        resolved = f"(comm ROM 0x{tphys:05X} - see 160-2998-14.lst)"
            elif mnem.startswith("j") and mnem != "jmp" and op.startswith("0x"):
                toff = int(op, 16)
                tphys = ((e["seg"] << 4) + toff) & 0xFFFFF
                resolved = labels.get(tphys, {}).get("name")

            if resolved:
                op = f"{resolved}  ; {op}"

            f.write(f"    {e['chip']}:{e['chip_off']:04X}  [{phys:05X}]  "
                     f"{e['raw']:<14s}  {mnem:<7s} {op}\n")
            last_phys = phys + e["size"]

    sym_out = {
        ("%05X" % phys): {
            "name": lab["name"], "kind": lab["kind"],
            "ref_count": len(lab["refs"]),
            "functional_name": g.FUNCTIONAL_NAMES.get(phys), "notes": None,
        }
        for phys, lab in sorted(labels.items())
        if 0xE0000 <= phys < 0x100000
    }
    with open(sym_path, "w") as f:
        json.dump(sym_out, f, indent=2)


def main():
    heuristic_entries = build_entry_points()
    entry_points = g.ENTRY_POINTS + heuristic_entries
    chips, visited, labels = g.main(None, entry_points)

    for name in ("3633", "3532"):
        count = sum(1 for e in visited.values() if e["chip"] == name)
        heuristic_count = sum(1 for s, o, n in heuristic_entries if n.startswith(f"FUNC_{name}"))
        print(f"{name}: {count} instructions reached with heuristic layer "
              f"({heuristic_count} heuristic entries added)")

    render_mainrom_only(chips, visited, labels,
                         "sysrom_3532_3633_heuristic.lst",
                         "sysrom_3532_3633_heuristic.symbols.json")


if __name__ == "__main__":
    main()
