"""
Byte-level coverage report: how much of each ROM chip's actual bytes
are accounted for by disassembled instructions, combining:
  - the proven (official) recursive-descent entry points
  - the main-ROM (3633/3532) push-bp heuristic supplement
  - the comm-ROM (2998) push-bp heuristic supplement
run together in one pass, so overlaps between layers aren't double
counted. Reports bytes covered / total bytes / percent per chip, and
the aggregate across all three real chips (2998_alias_90000 is a
slice alias of 2998's own bytes, not separate physical storage, so
its hits are folded back into 2998's own coverage, not counted twice).
"""
import gen_disasm_x86 as g
import gen_disasm_mainrom_heuristic as mh
import gen_disasm_2998 as g2998

def main():
    mainrom_entries = mh.build_entry_points()
    _, comm_entries = g2998.build_entry_points()
    all_entries = g.ENTRY_POINTS + mainrom_entries + comm_entries

    chips, visited, labels = g.main(None, all_entries)

    # bytes covered per real chip, folding the 0x90000 alias back into 2998
    covered_bytes = {"3633": set(), "3532": set(), "2998": set()}
    for e in visited.values():
        chip = e["chip"]
        off = e["chip_off"]
        size = e["size"]
        if chip == "2998_alias_90000":
            real_chip = "2998"
            real_off = off + 0x8000  # slice starts at file offset 0x8000
        elif chip in covered_bytes:
            real_chip = chip
            real_off = off
        else:
            continue
        for b in range(real_off, real_off + size):
            covered_bytes[real_chip].add(b)

    total = 0
    total_covered = 0
    print(f"{'chip':10s} {'covered':>8s} {'total':>8s} {'pct':>7s}")
    for name in ("3633", "3532", "2998"):
        chip_size = len(chips[name]["buf"])
        cov = len(covered_bytes[name])
        pct = 100.0 * cov / chip_size
        total += chip_size
        total_covered += cov
        print(f"{name:10s} {cov:8d} {chip_size:8d} {pct:6.2f}%")

    print(f"{'TOTAL':10s} {total_covered:8d} {total:8d} {100.0*total_covered/total:6.2f}%")

    # also report instruction counts for cross-check against prior sessions
    by_chip_insn = {}
    for e in visited.values():
        by_chip_insn[e["chip"]] = by_chip_insn.get(e["chip"], 0) + 1
    print()
    print("instruction counts (raw visited dict, includes alias separately):")
    for k, v in sorted(by_chip_insn.items()):
        print(f"  {k}: {v}")
    print(f"  TOTAL: {len(visited)}")

if __name__ == "__main__":
    main()
