#!/usr/bin/env python
"""
One-off scan: for every still-unnamed SUB_ in the proven symbol table,
look through its instruction block in sysrom_3532_3633.lst for a
0xFF7B segment load, collect nearby 4-hex-digit immediates as offset
candidates, and print the string found at each resulting physical
address (0xFF7B0 + offset) in 160-3532-14.bin. Manual review required -
many candidates will be false positives (an immediate that isn't
actually an offset into the string table).
"""
import json
import re

SYMS = json.load(open("sysrom_3532_3633.symbols.json"))
LST = open("sysrom_3532_3633.lst").readlines()

# Build a map from address (hex string, e.g. "E28FE") to line index
label_line = {}
for i, line in enumerate(LST):
    m = re.match(r"^([A-Za-z_][A-Za-z_0-9]*):$", line.rstrip("\n"))
    if m:
        label_line[m.group(1)] = i

# Reverse: for a given SUB_XXXXX address, find its block of lines up to
# the next top-level (non-"L_") label.
def block_for(addr_hex):
    name = f"SUB_{addr_hex}"
    if name not in label_line:
        return None
    start = label_line[name]
    end = len(LST)
    for i in range(start + 1, len(LST)):
        m = re.match(r"^([A-Za-z_][A-Za-z_0-9]*):$", LST[i].rstrip("\n"))
        if m and not m.group(1).startswith("L_"):
            end = i
            break
    return LST[start:end]

data3532 = open("../binary/160-3532-14.bin", "rb").read()
BASE3532 = 0xF0000

def read_string(off):
    phys = 0xFF7B0 + off
    fo = phys - BASE3532
    if fo < 0 or fo >= len(data3532):
        return None
    chunk = data3532[fo:fo + 40]
    return chunk

unnamed = [(v["ref_count"], k) for k, v in SYMS.items()
           if v["kind"] == "sub" and v["name"].startswith("SUB_")]
unnamed.sort(key=lambda x: -x[0])

IMM_RE = re.compile(r",\s*(?:0x)?([0-9a-fA-F]{1,4})\b")

for rc, addr in unnamed:
    block = block_for(addr)
    if block is None:
        continue
    text = "".join(block)
    if "0xff7b" not in text.lower():
        continue
    # Only trust immediates on a "mov REG, 0xNNNN" line within 2 lines
    # of an actual 0xff7b load - real string-table refs always come in
    # tight (segment, offset) pairs like the confirmed examples.
    MOV_IMM_RE = re.compile(r"\bmov\s+\w+,\s*(?:0x)?([0-9a-fA-F]{1,4})\b")
    candidates = set()
    for i, line in enumerate(block):
        if "0xff7b" not in line.lower():
            continue
        for j in range(max(0, i - 2), min(len(block), i + 3)):
            m = MOV_IMM_RE.search(block[j])
            if not m:
                continue
            try:
                v = int(m.group(1), 16)
            except ValueError:
                continue
            if v != 0xff7b and 0 < v < 0x1000:
                candidates.add(v)
    hits = []
    for off in sorted(candidates):
        s = read_string(off)
        if s is None:
            continue
        # only report if it looks like printable text (heuristic)
        printable = sum(1 for b in s[:8] if 32 <= b < 127)
        if printable >= 4:
            hits.append((off, s))
    if hits:
        print(f"=== SUB_{addr} (refs={rc}) ===")
        for off, s in hits[:6]:
            print(f"  0x{off:04x} -> {s!r}")
