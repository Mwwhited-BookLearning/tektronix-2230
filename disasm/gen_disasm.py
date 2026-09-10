#!/usr/bin/env python
"""
First-pass disassembly generator for Tektronix 2230 firmware ROMs.

Working hypothesis (unconfirmed, see NOTES.md):
  - CPU: Motorola 6809 (8-bit), based on opcode-frequency analysis
    (RTS=0x39 and PSHS=0x34 dominate; 68000/6502/Z80 signatures absent)
    and board silkscreen labels embedded as strings (A10 = digital/
    acquisition board, matches known 2230 hardware).
  - Address mapping: flat, file offset == CPU address (0x0000-0xFFFF).
    This is NOT yet validated - JMP/JSR target self-consistency checks
    were inconclusive. Treat all code-side labels as provisional.

This script:
  1. Finds printable ASCII string runs (>=4 chars) and marks them as
     data so we don't try to disassemble text as instructions.
  2. Linearly disassembles everything else with capstone's M680X/6809
     decoder.
  3. Collects branch/jump/subroutine-call targets and emits address-
     based labels (L%04X for branch targets, SUB_%04X for JSR/BSR
     targets) so they can be renamed later once their purpose is
     understood.
  4. Writes a combined listing (disasm/<name>.lst) and a symbol table
     (disasm/<name>.symbols.json) for iterative refinement.
"""
import re
import json
import sys
import capstone as cs

BRANCH_MNEMONICS = {
    "bra","lbra","bhi","lbhi","bls","lbls","bcc","lbcc","bhs","lbhs",
    "bcs","lbcs","blo","lblo","bne","lbne","beq","lbeq","bvc","lbvc",
    "bvs","lbvs","bpl","lbpl","bmi","lbmi","bge","lbge","blt","lblt",
    "bgt","lbgt","ble","lble",
}
CALL_MNEMONICS = {"bsr","lbsr","jsr"}
JUMP_MNEMONICS = {"jmp"}

ADDR_OPERAND_RE = re.compile(r"\$([0-9a-fA-F]{4})\b")


def find_string_regions(data, min_len=4):
    regions = []
    i = 0
    n = len(data)
    while i < n:
        if 0x20 <= data[i] < 0x7F:
            j = i
            while j < n and 0x20 <= data[j] < 0x7F:
                j += 1
            if j - i >= min_len:
                regions.append((i, j))
            i = j
        else:
            i += 1
    return regions


def build_data_mask(n, regions):
    mask = bytearray(n)  # 0 = code candidate, 1 = data
    for lo, hi in regions:
        for i in range(lo, hi):
            mask[i] = 1
    return mask


def disassemble(data, mask):
    md = cs.Cs(cs.CS_ARCH_M680X, cs.CS_MODE_M680X_6809)
    md.detail = False
    listing = []  # list of dict entries: kind, addr, length, text, raw
    labels = {}   # addr -> {"name":..., "kind": "loc"/"sub", "refs": [addr,...]}
    n = len(data)
    addr = 0

    def add_ref(target, kind):
        if 0 <= target < n:
            lab = labels.setdefault(target, {"kind": kind, "refs": []})
            # a location can be referenced both as branch target and call
            # target across the file; keep the "stronger" classification
            if kind == "sub":
                lab["kind"] = "sub"
            lab["refs"].append(addr)

    while addr < n:
        if mask[addr]:
            start = addr
            while addr < n and mask[addr]:
                addr += 1
            text = data[start:addr]
            try:
                s = text.decode("ascii")
            except UnicodeDecodeError:
                s = None
            listing.append({
                "kind": "str" if s else "byte",
                "addr": start,
                "len": addr - start,
                "text": s,
                "raw": text.hex(),
            })
            continue

        chunk = data[addr:addr + 8]  # max 6809 instr length is small; 8 is safe
        insns = list(md.disasm(chunk, addr))
        if not insns:
            listing.append({"kind": "byte", "addr": addr, "len": 1,
                             "text": None, "raw": data[addr:addr+1].hex()})
            addr += 1
            continue

        insn = insns[0]
        mnem = insn.mnemonic
        opstr = insn.op_str
        ilen = insn.size

        if mnem == "fcb":
            listing.append({"kind": "byte", "addr": addr, "len": 1,
                             "text": None, "raw": data[addr:addr+1].hex()})
            addr += 1
            continue

        listing.append({
            "kind": "insn", "addr": addr, "len": ilen,
            "mnem": mnem, "op": opstr,
            "raw": data[addr:addr+ilen].hex(),
        })

        m = ADDR_OPERAND_RE.search(opstr)
        if m:
            target = int(m.group(1), 16)
            if mnem in CALL_MNEMONICS:
                add_ref(target, "sub")
            elif mnem in BRANCH_MNEMONICS or mnem in JUMP_MNEMONICS:
                add_ref(target, "loc")

        addr += ilen

    return listing, labels


def render(listing, labels, out_path, sym_path):
    for addr, lab in labels.items():
        lab["name"] = ("SUB_%04X" % addr) if lab["kind"] == "sub" else ("L%04X" % addr)

    with open(out_path, "w") as f:
        f.write("; Tektronix 2230 firmware - provisional 6809 disassembly\n")
        f.write("; Flat address mapping (file offset == CPU address) - UNVALIDATED\n")
        f.write("; Labels are address-based placeholders (L%04X / SUB_%04X);\n")
        f.write("; rename to functional names as their purpose is understood.\n\n")

        for entry in listing:
            addr = entry["addr"]
            lab = labels.get(addr)
            if lab:
                f.write(f"\n{lab['name']}:\n")

            if entry["kind"] == "str":
                f.write(f"    {addr:04X}  {entry['raw']:<20s}  .ascii \"{entry['text']}\"\n")
            elif entry["kind"] == "byte":
                f.write(f"    {addr:04X}  {entry['raw']:<20s}  .byte ${entry['raw']}\n")
            else:
                op = entry["op"]
                m = ADDR_OPERAND_RE.search(op)
                if m:
                    t = int(m.group(1), 16)
                    if t in labels:
                        op = ADDR_OPERAND_RE.sub(labels[t]["name"], op, count=1)
                f.write(f"    {addr:04X}  {entry['raw']:<20s}  {entry['mnem']:<6s} {op}\n")

    sym_out = {
        ("%04X" % addr): {
            "name": lab["name"],
            "kind": lab["kind"],
            "ref_count": len(lab["refs"]),
            "functional_name": None,
            "notes": None,
        }
        for addr, lab in sorted(labels.items())
    }
    with open(sym_path, "w") as f:
        json.dump(sym_out, f, indent=2)


def main():
    in_path = sys.argv[1]
    out_base = sys.argv[2]
    data = open(in_path, "rb").read()
    regions = find_string_regions(data)
    mask = build_data_mask(len(data), regions)
    listing, labels = disassemble(data, mask)
    render(listing, labels, out_base + ".lst", out_base + ".symbols.json")
    n_insn = sum(1 for e in listing if e["kind"] == "insn")
    n_str = sum(1 for e in listing if e["kind"] == "str")
    n_byte = sum(1 for e in listing if e["kind"] == "byte")
    print(f"{in_path}: {n_insn} instructions, {n_str} strings, {n_byte} raw-byte entries, {len(labels)} labels")


if __name__ == "__main__":
    main()
