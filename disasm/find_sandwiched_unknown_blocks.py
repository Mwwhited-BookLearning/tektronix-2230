"""
Systematic follow-up to the 2026-09-22 deep-dive finding that
UNKNOWN_DATA.md's `160-3633` block 20 (file 0xFF99-0xFFED) wasn't data
at all - it was real, reachable code the coverage tooling missed
because nothing in the proven/heuristic entry-point corpus ever led
into it, sitting sandwiched directly between two regions of
already-covered code (a jmp at 0xEFF96 on one side, code resuming at
0xEFFEF on the other). See docs/decode-anomalies/unknown-data-deep-
dive-2026-09-15.md's "Follow-up, 2026-09-22" section 7, and TODO.md's
"Next step if picked up" for that finding.

This does NOT re-decode every remaining unknown block by hand (that's
what the 2026-09-15/2026-09-22 deep-dive passes already did for a
hand-picked subset) - it ranks every block find_unknown_data.py would
report by one mechanical signal (covered code immediately before AND
after the gap, as opposed to chip-end filler or a block genuinely
isolated on both sides) and shows the covered instruction bracketing
each side, so a human can see at a glance which of the ~40 remaining
blocks are worth tracing by hand next instead of picking at random.

Also flags, for the covered instruction immediately before a gap, if
it's a near jmp/call/conditional-jump whose *unmasked* Capstone-
resolved target offset is >= 0x10000 - the same "target crosses this
chip's own 64KB half" shape the 2026-09-22 finding's two jumps had -
in case that specific shape recurs elsewhere too.

Usage: python find_sandwiched_unknown_blocks.py [min_block_size]
(default 32, same threshold UNKNOWN_DATA.md uses)
"""
import sys

import find_unknown_data as fud
import gen_disasm_x86 as g
import gen_disasm_mainrom_heuristic as mh
import gen_disasm_2998 as g2998


def build_visited():
    mainrom_entries = mh.build_entry_points()
    _, comm_entries = g2998.build_entry_points()
    all_entries = g.ENTRY_POINTS + mainrom_entries + comm_entries
    _, visited, _ = g.main(None, all_entries)
    return visited


def index_by_chip_offset(visited):
    """chip -> {chip_off: entry}, folding the 2998_alias_90000 slice
    back onto real 2998 file offsets so it lines up with the same
    gap coordinates find_unknown_data.py reports."""
    idx = {}
    for e in visited.values():
        chip = e["chip"]
        off = e["chip_off"]
        if chip == "2998_alias_90000":
            chip = "2998"
            off = off + 0x8000
        idx.setdefault(chip, {})[off] = dict(e, chip=chip, chip_off=off)
    return idx


def find_instruction_ending_at(by_off, target_end):
    """The covered instruction whose [chip_off, chip_off+size) ends
    exactly at target_end - i.e. the real instruction immediately
    before a gap, found by scanning a small backward window (max x86
    instruction length on this ISA is well under 16 bytes)."""
    for start in range(target_end - 1, max(target_end - 16, -1), -1):
        e = by_off.get(start)
        if e and start + e["size"] == target_end:
            return e
    return None


def describe(e):
    if e is None:
        return "(none - not itself a decoded instruction boundary)"
    return f"{e['chip']}:{e['chip_off']:04X}  {e['mnem']} {e['op']}  (raw {e['raw']})"


def raw_target_note(e):
    """For jmp/call/conditional-jump entries, flag when the *unmasked*
    Capstone-resolved near-target offset is >= 0x10000 - this specific
    instruction's target crosses this chip's own 64KB half, the exact
    shape of the two jumps found in the 2026-09-22 block-20 finding."""
    if e is None:
        return ""
    mnem = e["mnem"]
    op = e["op"]
    is_jmp_family = (mnem in ("jmp", "call")) or (mnem.startswith("j") and mnem != "jmp")
    if not is_jmp_family or not op.startswith("0x"):
        return ""
    toff = int(op, 16)
    if toff >= 0x10000:
        return f"  <-- target 0x{toff:X} crosses this chip's 64KB half"
    return ""


def main():
    min_size = int(sys.argv[1]) if len(sys.argv) > 1 else 32

    visited = build_visited()
    by_off = index_by_chip_offset(visited)
    covered = fud.compute_covered_bytes()

    candidates = []

    for chip in ("3633", "3532", "2998"):
        info = fud.ROMS[chip]
        data = open(info["bin"], "rb").read()
        str_ranges = fud.load_string_ranges(info["strings"])
        known_ranges = set()
        for s, e_ in fud.KNOWN_DATA_RANGES.get(chip, []):
            known_ranges.update(range(s, e_))
        identified = covered[chip] | str_ranges | known_ranges
        gaps = fud.find_gaps(identified, len(data), min_size)
        chip_by_off = by_off.get(chip, {})

        print(f"\n{'='*78}")
        print(f"Chip {chip}: {len(gaps)} unknown blocks >= {min_size} bytes")
        print(f"{'='*78}")

        for i, (start, end) in enumerate(gaps, 1):
            length = end - start
            before = find_instruction_ending_at(chip_by_off, start)
            after = chip_by_off.get(end)
            before_covered = (start - 1) in covered[chip]
            after_covered = end < len(data) and end in covered[chip]
            sandwiched = before_covered and after_covered
            all_ff = all(b == 0xFF for b in data[start:end])

            if all_ff:
                flag = "filler (all 0xFF) - not a candidate"
            elif sandwiched:
                flag = "*** SANDWICHED - candidate, same shape as block 20 ***"
                candidates.append((chip, i, start, end))
            elif before_covered or after_covered:
                flag = "one side only covered - weaker candidate"
            else:
                flag = "isolated on both sides - not this pattern"

            print(f"\nblock {i}: file 0x{start:05X}-0x{end-1:05X}  "
                  f"phys 0x{info['phys_base']+start:06X}-0x{info['phys_base']+end-1:06X}  "
                  f"({length} bytes)  [{flag}]")
            if not all_ff:
                print(f"  before: {describe(before)}{raw_target_note(before)}")
                print(f"  after : {describe(after)}")

    print(f"\n{'='*78}")
    print(f"{len(candidates)} sandwiched candidate(s) worth tracing by hand next:")
    for chip, i, start, end in candidates:
        phys_base = fud.ROMS[chip]["phys_base"]
        print(f"  {chip} block {i}: phys 0x{phys_base+start:06X}-0x{phys_base+end-1:06X}")


if __name__ == "__main__":
    main()
