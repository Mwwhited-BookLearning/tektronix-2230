#!/usr/bin/env python
"""
Regenerate all three ROMs' buildable .asm sources using the combined
entry-point set (official/proven + the comm ROM's heuristic push-bp
signature scan - see gen_disasm_2998.py), so 160-2998-14.asm reflects
the fuller heuristic-assisted coverage rather than just the ~99
instructions reachable from proven entry points alone.

Now that the comm ROM's mapping is confirmed flat (chip "2998" in
gen_disasm_x86.CHIPS, physical 0x80000-0x8FFFF - see NOTES.md "The
comm ROM is NOT bank-switched"), this is just gen_source.py with a
bigger entry_points list; no separate page-relative-address handling
is needed any more, unlike before that mapping was confirmed.
"""
import sys

import gen_disasm_2998 as c2998
import gen_disasm_x86 as g
import gen_source as gs

if __name__ == "__main__":
    nasm_exe = sys.argv[1] if len(sys.argv) > 1 else "nasm"
    heuristic_entries = c2998.build_entry_points()[1]
    entry_points = g.ENTRY_POINTS + heuristic_entries
    # only_chips=["2998"]: the heuristic entries can open up new reachable
    # code in 3633/3532 too (that's fine, real code), but writing THOSE
    # chips' .asm here would silently mix heuristic-derived confidence
    # into files that are supposed to be proven-reachable-only. Those get
    # regenerated separately via gen_source.py's default (proven-only)
    # entry set.
    gs.main(nasm_exe, entry_points, only_chips=["2998"])
