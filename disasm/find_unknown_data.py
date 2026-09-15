"""
Find large contiguous byte ranges in each ROM chip that are neither:
  - reached by the disassembler (proven entry points + the main-ROM/
    comm-ROM push-bp heuristic supplements - same coverage computed by
    compute_coverage.py), nor
  - an already-identified printable string (per strings_<rom>.json,
    the machine-readable companion to STRINGS.md)

These are the genuine "we don't know what this is" regions: not code,
not text. Could be a real data table (a font, a lookup table, packed
BCD constants, etc.) or just filler/padding - that's exactly what this
report exists to let a human eyeball and classify.

Usage: python find_unknown_data.py [min_block_size]
(default min_block_size = 32 bytes - smaller gaps are almost always
just instruction-decode misalignment noise, not real unidentified data)

Writes nothing itself - prints a report to stdout. Redirect to a file
or pipe into whatever assembles UNKNOWN_DATA.md.
"""
import json
import struct
import sys

import gen_disasm_x86 as g
import gen_disasm_mainrom_heuristic as mh
import gen_disasm_2998 as g2998

ROMS = {
    "3633": {"bin": "../binary/160-3633-14.bin", "strings": "strings_160-3633.json", "phys_base": 0xE0000},
    "3532": {"bin": "../binary/160-3532-14.bin", "strings": "strings_160-3532.json", "phys_base": 0xF0000},
    "2998": {"bin": "../binary/160-2998-14.bin", "strings": "strings_160-2998.json", "phys_base": 0x80000},
}

# Non-string, non-code data tables that are already fully identified
# elsewhere in this project's docs, with confirmed exact byte extents -
# excluded here so this report only surfaces genuinely unknown data.
# (file-offset ranges, end-exclusive)
KNOWN_DATA_RANGES = {
    "2998": [
        (0x8A59, 0x8D15),  # argument/value keyword table - docs/comm-rom/command-keyword-table.md #1
        (0x8D3E, 0x8DE7),  # dispatch/index table - same doc #2
        (0x8DE7, 0x8F18),  # top-level command header table - same doc #3
    ],
}


def compute_covered_bytes():
    """Same logic as compute_coverage.py: union of proven + heuristic
    disassembly coverage per chip, folding the 0x90000 alias slice
    back onto its real underlying 2998 file-offset range."""
    mainrom_entries = mh.build_entry_points()
    _, comm_entries = g2998.build_entry_points()
    all_entries = g.ENTRY_POINTS + mainrom_entries + comm_entries

    _, visited, _ = g.main(None, all_entries)

    covered = {"3633": set(), "3532": set(), "2998": set()}
    for e in visited.values():
        chip = e["chip"]
        off = e["chip_off"]
        size = e["size"]
        if chip == "2998_alias_90000":
            real_chip = "2998"
            real_off = off + 0x8000
        elif chip in covered:
            real_chip = chip
            real_off = off
        else:
            continue
        for b in range(real_off, real_off + size):
            covered[real_chip].add(b)
    return covered


def load_string_ranges(path):
    """Return a set of byte offsets covered by already-identified
    printable strings, per gen_strings.py's own offset convention."""
    entries = json.load(open(path, encoding="utf-8"))
    covered = set()
    for e in entries:
        start = int(e["offset"], 16)
        end = start + len(e["text"])
        if e["length_prefixed"]:
            start -= 1
        if e["null_terminated"]:
            end += 1
        for b in range(start, end):
            covered.add(b)
    return covered


def find_gaps(identified, total_size, min_size):
    gaps = []
    start = None
    for i in range(total_size):
        if i in identified:
            if start is not None:
                if i - start >= min_size:
                    gaps.append((start, i))
                start = None
        else:
            if start is None:
                start = i
    if start is not None and total_size - start >= min_size:
        gaps.append((start, total_size))
    return gaps


def hexdump_line(data):
    return " ".join(f"{b:02X}" for b in data)


def octdump_line(data):
    return " ".join(f"{b:03o}" for b in data)


def ascii_line(data):
    return "".join(chr(b) if 32 <= b < 127 else "." for b in data)


def decode_block(data):
    """~20 bytes decoded as every native 8088-relevant fixed-width
    type, plus IEEE-754 float for reference (the 2230 has no 8087 on
    the main board - see HARDWARE.md - so a "float" reading here is
    just a convenience decode, not evidence anything here IS a float)."""
    sample = data[:20]
    lines = []
    lines.append(f"hex   : {hexdump_line(sample)}")
    lines.append(f"oct   : {octdump_line(sample)}")
    lines.append(f"ascii : {ascii_line(sample)}")

    u8 = list(sample)
    i8 = [b - 256 if b >= 128 else b for b in u8]
    lines.append(f"uint8 : {u8}")
    lines.append(f"int8  : {i8}")

    n16 = len(sample) // 2
    if n16:
        u16 = struct.unpack_from(f"<{n16}H", sample)
        i16 = struct.unpack_from(f"<{n16}h", sample)
        lines.append(f"uint16le: {list(u16)}")
        lines.append(f"int16le : {list(i16)}")

    n32 = len(sample) // 4
    if n32:
        u32 = struct.unpack_from(f"<{n32}I", sample)
        i32 = struct.unpack_from(f"<{n32}i", sample)
        f32 = struct.unpack_from(f"<{n32}f", sample)
        lines.append(f"uint32le: {list(u32)}")
        lines.append(f"int32le : {list(i32)}")
        lines.append(f"float32le: {[round(x, 6) for x in f32]}")

    # far pointer reading (seg:off, little-endian offset then segment -
    # the natural 8088 far-pointer in-memory layout)
    if n16 >= 2:
        pairs = []
        for j in range(0, (n16 // 2) * 2, 2):
            off, seg = u16[j], u16[j + 1]
            pairs.append(f"{seg:04X}:{off:04X} (phys 0x{seg*16+off:06X})")
        lines.append(f"as far ptrs (off,seg): {pairs}")

    return "\n".join(lines)


def decode_block_md(data):
    sample = data[:20]
    lines = []
    lines.append(f"hex     : `{hexdump_line(sample)}`")
    lines.append(f"oct     : `{octdump_line(sample)}`")
    lines.append(f"ascii   : `{ascii_line(sample)}`")

    u8 = list(sample)
    i8 = [b - 256 if b >= 128 else b for b in u8]
    lines.append(f"uint8   : `{u8}`")
    lines.append(f"int8    : `{i8}`")

    n16 = len(sample) // 2
    u16 = ()
    if n16:
        u16 = struct.unpack_from(f"<{n16}H", sample)
        i16 = struct.unpack_from(f"<{n16}h", sample)
        lines.append(f"uint16le: `{list(u16)}`")
        lines.append(f"int16le : `{list(i16)}`")

    n32 = len(sample) // 4
    if n32:
        u32 = struct.unpack_from(f"<{n32}I", sample)
        i32 = struct.unpack_from(f"<{n32}i", sample)
        f32 = struct.unpack_from(f"<{n32}f", sample)
        lines.append(f"uint32le: `{list(u32)}`")
        lines.append(f"int32le : `{list(i32)}`")
        lines.append(f"float32le: `{[round(x, 6) for x in f32]}`")

    if n16 >= 2:
        pairs = []
        for j in range(0, (n16 // 2) * 2, 2):
            off, seg = u16[j], u16[j + 1]
            pairs.append(f"{seg:04X}:{off:04X} (phys 0x{seg*16+off:06X})")
        lines.append(f"as far ptrs (off,seg): `{pairs}`")

    return "\n".join(lines)


def run(min_size, markdown=False):
    covered = compute_covered_bytes()
    all_gaps = {}

    for chip, info in ROMS.items():
        data = open(info["bin"], "rb").read()
        str_ranges = load_string_ranges(info["strings"])
        known_ranges = set()
        for s, e in KNOWN_DATA_RANGES.get(chip, []):
            known_ranges.update(range(s, e))
        identified = covered[chip] | str_ranges | known_ranges
        gaps = find_gaps(identified, len(data), min_size)
        all_gaps[chip] = (data, gaps)

    if not markdown:
        for chip, (data, gaps) in all_gaps.items():
            info = ROMS[chip]
            total_gap_bytes = sum(e - s for s, e in gaps)
            print(f"\n{'='*70}")
            print(f"Chip {chip} ({info['bin']}): {len(gaps)} unknown blocks "
                  f">= {min_size} bytes, {total_gap_bytes} bytes total "
                  f"({100.0*total_gap_bytes/len(data):.1f}% of {len(data)})")
            print(f"{'='*70}")
            phys_base = info["phys_base"]
            for start, end in gaps:
                length = end - start
                print(f"\n--- file 0x{start:05X}-0x{end-1:05X}  "
                      f"phys 0x{phys_base+start:06X}-0x{phys_base+end-1:06X}  "
                      f"(length 0x{length:X} = {length}) ---")
                print(decode_block(data[start:end]))
        return

    # Markdown mode: summary table, then one subsection per block.
    print("## Summary\n")
    print(f"Minimum block size shown: {min_size} bytes.\n")
    print("| Chip | ROM file | Unknown blocks | Unknown bytes | % of chip |")
    print("|---|---|---|---|---|")
    grand_blocks = 0
    grand_bytes = 0
    grand_total = 0
    for chip, (data, gaps) in all_gaps.items():
        info = ROMS[chip]
        total_gap_bytes = sum(e - s for s, e in gaps)
        grand_blocks += len(gaps)
        grand_bytes += total_gap_bytes
        grand_total += len(data)
        print(f"| {chip} | `{info['bin'].split('/')[-1]}` | {len(gaps)} | "
              f"{total_gap_bytes} | {100.0*total_gap_bytes/len(data):.1f}% |")
    print(f"| **TOTAL** | | {grand_blocks} | {grand_bytes} | "
          f"{100.0*grand_bytes/grand_total:.1f}% |")

    for chip, (data, gaps) in all_gaps.items():
        info = ROMS[chip]
        print(f"\n## Chip {chip} (`{info['bin'].split('/')[-1]}`)\n")
        print("| # | File offset | Physical | Length |")
        print("|---|---|---|---|")
        phys_base = info["phys_base"]
        for i, (start, end) in enumerate(gaps, 1):
            length = end - start
            print(f"| {i} | `0x{start:05X}-0x{end-1:05X}` | "
                  f"`0x{phys_base+start:06X}-0x{phys_base+end-1:06X}` | "
                  f"{length} |")

        for i, (start, end) in enumerate(gaps, 1):
            length = end - start
            print(f"\n### {chip} block {i}: file `0x{start:05X}-0x{end-1:05X}`, "
                  f"phys `0x{phys_base+start:06X}-0x{phys_base+end-1:06X}` "
                  f"(length {length})\n")
            print(decode_block_md(data[start:end]))
            print()


def main():
    min_size = 32
    markdown = False
    for arg in sys.argv[1:]:
        if arg == "--markdown":
            markdown = True
        else:
            min_size = int(arg)
    run(min_size, markdown=markdown)


if __name__ == "__main__":
    main()
