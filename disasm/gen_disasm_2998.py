#!/usr/bin/env python
"""
Recursive-descent disassembler for the comm/GPIB-RS232 option ROM
(160-2998-14.bin).

Unlike the main ROM pair, this chip's real bank-switch mapping into the
CPU's physical address space is NOT confirmed (see disasm/NOTES.md "Is
the comm ROM its own CPU?"). What IS confirmed: it's 8088/8086 code
(same CPU as the main ROM), organized as four independent 16KB pages
(0x0000-0x3FFF, 0x4000-0x7FFF, 0x8000-0xBFFF, 0xC000-0xFFFF within the
file), each of which is presumably windowed into the CPU's address
space as a unit when the option board's ROM is selected.

Since we don't know the real segment each page maps to, this assigns
each page an arbitrary, non-overlapping VIRTUAL physical base in an
address range nothing else uses (0xA0000-0xD4000ish, spaced a full
64KB apart per page - well clear of the confirmed main-ROM window at
0xE0000-0xFFFFF and the low-memory/RAM regions in MEMORY_MAP.md). Near
jmp/call/jcc math is self-relative, so
it comes out correct regardless of which base we pick; only FAR
jmp/call targets are real absolute addresses, and those get resolved
against the ACTUAL main-ROM chips too (reusing gen_disasm_x86's engine
with a combined chip set), so a comm-ROM far call that lands in
0xE0000-0xFFFFF correctly continues into the already-disassembled main
ROM instead of dead-ending.

Lacking a confirmed entry point (no reset vector, no known caller),
entry points here are seeded from every occurrence of the `55 8B EC`
(`push bp; mov bp,sp`) C-compiler prologue signature - a strong but
NOT proof-of-reachability heuristic, unlike the main ROM's disassembly
which only follows PROVEN control flow. Treat this output as lower-
confidence accordingly.
"""
import re

import gen_disasm_x86 as g

COMM_ROM_PATH = "../binary/160-2998-14.bin"
PAGE_SIZE = 0x4000
VIRTUAL_BASE = 0xA0000  # arbitrary, unused in the real 20-bit address space
# Pages are spaced a full 64KB apart (not just PAGE_SIZE) even though each
# page is only 16KB of real content: a real x86 segment spans 64KB, so if
# any near-branch offset overflowed past 0x3FFF, tight PAGE_SIZE spacing
# could make it wrongly resolve into the *next* page's window instead of
# correctly landing in unmapped (None) space.
VIRTUAL_SPACING = 0x10000

PROLOGUE = b"\x55\x8b\xec"


def find_function_starts(buf):
    return [m.start() for m in re.finditer(re.escape(PROLOGUE), buf)]


def page_virtual_base(page):
    return VIRTUAL_BASE + page * VIRTUAL_SPACING


def build_chip_defs():
    buf = open(COMM_ROM_PATH, "rb").read()
    defs = {}
    for i in range(4):
        page_buf = buf[i * PAGE_SIZE:(i + 1) * PAGE_SIZE]
        defs[f"2998p{i}"] = {"_buf": page_buf, "phys_base": page_virtual_base(i)}
    return buf, defs


def build_entry_points(buf, chip_defs):
    entries = []
    starts = find_function_starts(buf)
    for off in starts:
        page = off // PAGE_SIZE
        page_off = off % PAGE_SIZE
        virt_phys = page_virtual_base(page) + page_off
        seg, iof = virt_phys >> 4, virt_phys & 0xF
        entries.append((seg, iof, f"FUNC_2998_{off:04X}"))
    # the two page-boundary boot stubs (far jmp out of this ROM) are not
    # push-bp prologues but are clearly real entry points too
    for page in (2, 3):
        virt_phys = page_virtual_base(page)
        seg, iof = virt_phys >> 4, virt_phys & 0xF
        entries.append((seg, iof, f"BOOTSTUB_2998_page{page}"))
    return entries


def main(nasm_exe=None):
    buf, page_defs = build_chip_defs()

    # merge the comm-rom virtual pages with the real main-ROM chips so a
    # far call/jmp landing in 0xE0000-0xFFFFF continues into that
    # already-disassembled code instead of dead-ending.
    chip_defs = dict(g.CHIPS)
    for name, info in page_defs.items():
        chip_defs[name] = info

    # gen_disasm_x86.load_chips() reads from a "path" key; patch it to
    # accept our in-memory page buffers instead of re-reading from disk.
    def load_chips_patched(defs):
        data = {}
        for name, info in defs.items():
            if "_buf" in info:
                data[name] = {"buf": info["_buf"], "base": info["phys_base"],
                               "size": len(info["_buf"])}
            else:
                data[name] = {"buf": open(info["path"], "rb").read(),
                               "base": info["phys_base"], "size": len(open(info["path"], "rb").read())}
        return data

    g.load_chips = load_chips_patched

    entry_points = build_entry_points(buf, chip_defs) + g.ENTRY_POINTS
    chips, visited, labels = g.main(chip_defs, entry_points)

    comm_visited = {p: e for p, e in visited.items() if e["chip"].startswith("2998")}
    comm_labels = {p: l for p, l in labels.items() if VIRTUAL_BASE <= p < VIRTUAL_BASE + 4 * VIRTUAL_SPACING}
    print(f"comm ROM: {len(comm_visited)} instructions reached, "
          f"{len(comm_labels)} comm-rom-local labels "
          f"(from {len(entry_points) - len(g.ENTRY_POINTS)} seeded function starts)")

    spillover = {p: e for p, e in visited.items() if not e["chip"].startswith("2998")}
    print(f"far calls/jumps that landed in the main ROM: "
          f"{len(spillover)} additional instructions reached there")

    render(chips, visited, labels, "160-2998-14.lst", "160-2998-14.symbols.json")


def render(chips, visited, labels, out_path, sym_path):
    import json
    for phys, lab in labels.items():
        if "fixed_name" in lab:
            lab["name"] = lab["fixed_name"]
        else:
            lab["name"] = ("SUB_%05X" % phys) if lab["kind"] == "sub" else ("L_%05X" % phys)

    comm_entries = sorted(
        (e for e in visited.values() if e["chip"].startswith("2998")),
        key=lambda e: e["phys"])

    with open(out_path, "w") as f:
        f.write("; Tektronix 2230 comm/GPIB-RS232 option ROM (160-2998-14.bin)\n")
        f.write("; Intel 8088/8086 real mode. UNCONFIRMED address mapping - each\n")
        f.write("; 16KB page is assigned an arbitrary virtual base (0xA0000 +\n")
        f.write("; page*0x10000) purely so near-branch math resolves; only FAR\n")
        f.write("; jmp/call targets are real absolute addresses. Entry points are\n")
        f.write("; heuristic (push-bp/mov-bp,sp signature scan), NOT proven\n")
        f.write("; reachable from a known boot path like the main ROM's are.\n")
        f.write("; A far call/jmp landing in 0xE0000-0xFFFFF continues into the\n")
        f.write("; already-disassembled main ROM (see sysrom_3532_3633.lst).\n\n")

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

            f.write(f"    {e['chip']}:{e['chip_off']:04X}  [virt {phys:05X}]  "
                     f"{e['raw']:<14s}  {mnem:<7s} {op}\n")
            last_phys = phys + e["size"]

    sym_out = {
        ("%05X" % phys): {
            "name": lab["name"], "kind": lab["kind"],
            "ref_count": len(lab["refs"]), "functional_name": None, "notes": None,
        }
        for phys, lab in sorted(labels.items())
        if VIRTUAL_BASE <= phys < VIRTUAL_BASE + 4 * VIRTUAL_SPACING
    }
    with open(sym_path, "w") as f:
        json.dump(sym_out, f, indent=2)


if __name__ == "__main__":
    main()
