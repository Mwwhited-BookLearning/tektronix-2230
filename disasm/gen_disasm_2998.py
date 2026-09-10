#!/usr/bin/env python
"""
Heuristic-entry-point supplement for the comm/GPIB-RS232 option ROM
(160-2998-14.bin).

The comm ROM's real address mapping is now CONFIRMED (see NOTES.md
"The comm ROM is NOT bank-switched"): it's a plain 64KB device at a
fixed physical base, 0x80000-0x8FFFF - the same simple pattern as the
two main-ROM halves, registered as chip "2998" in gen_disasm_x86.CHIPS.
Verified by checking every observed far-call target landing in that
range against this file's own C-compiler prologue signatures: 82/84
land exactly on one.

Running gen_disasm_x86.py alone now already reaches some of this ROM
through PROVEN control flow (real far calls from already-confirmed
main-ROM code). This script adds a second, lower-confidence layer on
top: every occurrence of the `55 8B EC` (`push bp; mov bp,sp`)
prologue signature (398 of them) plus this ROM's own two internal
boot-stub far jumps, seeded as ADDITIONAL entry points in the same
real address space. Since the mapping is now flat and confirmed, no
special virtual-addressing bookkeeping is needed any more - this is
just more entries in the same combined recursive descent.
"""
import re

import gen_disasm_x86 as g

COMM_ROM_PATH = "../binary/160-2998-14.bin"
COMM_ROM_BASE = 0x80000
PAGE_SIZE = 0x4000  # for locating the 2 internal boot-stub jumps only

PROLOGUE = b"\x55\x8b\xec"


def find_function_starts(buf):
    return [m.start() for m in re.finditer(re.escape(PROLOGUE), buf)]


def build_entry_points():
    buf = open(COMM_ROM_PATH, "rb").read()
    seg = COMM_ROM_BASE >> 4  # 0x8000, and COMM_ROM_BASE & 0xF == 0
    entries = []
    for off in find_function_starts(buf):
        entries.append((seg, off, f"FUNC_2998_{off:04X}"))
    for page in (2, 3):  # internal far-jump-out stubs, not push-bp prologues
        off = page * PAGE_SIZE
        entries.append((seg, off, f"BOOTSTUB_2998_page{page}"))
    return buf, entries


def render_comm_only(chips, visited, labels, out_path, sym_path):
    import json
    for phys, lab in labels.items():
        if phys in g.FUNCTIONAL_NAMES:
            lab["name"] = g.FUNCTIONAL_NAMES[phys]
        elif "fixed_name" in lab:
            lab["name"] = lab["fixed_name"]
        else:
            lab["name"] = ("SUB_%05X" % phys) if lab["kind"] == "sub" else ("L_%05X" % phys)

    comm_entries = sorted((e for e in visited.values() if e["chip"] == "2998"),
                           key=lambda e: e["phys"])

    with open(out_path, "w") as f:
        f.write("; Tektronix 2230 comm/GPIB-RS232 option ROM (160-2998-14.bin)\n")
        f.write("; Intel 8088/8086 real mode. CONFIRMED mapping: flat 64KB\n")
        f.write("; device at physical 0x80000-0x8FFFF (see NOTES.md 'The comm\n")
        f.write("; ROM is NOT bank-switched'). Entries reached only via the\n")
        f.write("; heuristic push-bp signature scan (not a proven far call from\n")
        f.write("; already-confirmed code) are lower-confidence - cross-check\n")
        f.write("; against sysrom_3532_3633.lst before trusting reachability.\n\n")

        last_phys = None
        for e in comm_entries:
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
                    if resolved is None and 0xE0000 <= tphys < 0x100000:
                        resolved = f"(main ROM 0x{tphys:05X} - see sysrom_3532_3633.lst)"
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
        if 0x80000 <= phys < 0x90000
    }
    with open(sym_path, "w") as f:
        json.dump(sym_out, f, indent=2)


def main():
    heuristic_entries = build_entry_points()[1]
    entry_points = g.ENTRY_POINTS + heuristic_entries
    chips, visited, labels = g.main(None, entry_points)

    comm_visited = {p: e for p, e in visited.items() if e["chip"] == "2998"}
    print(f"comm ROM: {len(comm_visited)} instructions reached "
          f"({len(heuristic_entries)} heuristic entries added on top of "
          f"the {len(g.ENTRY_POINTS)} official/proven ones)")

    render_comm_only(chips, visited, labels, "160-2998-14.lst", "160-2998-14.symbols.json")


if __name__ == "__main__":
    main()
