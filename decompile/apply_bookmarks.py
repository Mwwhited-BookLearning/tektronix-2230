"""Leave Ghidra Bookmarks at addresses worth flagging for deep
analysis later - a lightweight, git-trackable "here's where to look"
mechanism (Window > Bookmarks in the GUI; also shown as markers in the
address gutter) so a future session (or a fresh machine, rebuilding
this project from `apply_names.py`/`apply_parameters.py`/this script)
doesn't have to rediscover where the interesting unresolved spots are.

BOOKMARKS below is the single git-tracked source of truth - add an
entry here (with a comment explaining why) any time this project
finds something worth flagging but not yet fully resolved, rather than
only noting it in a doc.

Usage: run from anywhere - all paths are absolute.
    python apply_bookmarks.py
"""
import sys
import pyghidra

REPO_ROOT = r"C:\repo\_learning\tektronix-2230"
sys.path.insert(0, REPO_ROOT + r"\disasm")

pyghidra.start()

from ghidra.base.project import GhidraProject
from ghidra.program.model.listing import BookmarkType

PROJECT_DIR = REPO_ROOT + r"\decompile"

PROGRAMS = [
    ("160-3633-14.bin", 0xE0000, 0xEFFFF, 0x000E),
    ("160-3532-14.bin", 0xF0000, 0xFFFFF, 0x000F),
    ("160-2998-14.bin", 0x80000, 0x8FFFF, 0x0008),
    ("160-2998-14-shadow.bin", 0x90000, 0x97FFF, 0x0009),
]


def resolve(phys):
    for name, lo, hi, seg in PROGRAMS:
        if lo <= phys <= hi:
            return name, seg, phys - lo
    return None, None, None


# (physical_addr, category, note) - category groups related bookmarks
# in the Bookmarks window; keep them short and specific.
BOOKMARKS = [
    # The 5 addresses that fail CreateFunctionCmd (see apply_names.py's
    # own run output and docs/architecture/ghidra-project.md's
    # "Decompiled-C export" section) - Ghidra's own function-boundary
    # heuristics independently confirm these 5 are NOT clean
    # instruction boundaries, matching this project's own static-
    # analysis "landing artifact" findings for the same addresses.
    # Resolve via docs/decode-anomalies/dual-entry-points.md's process
    # (find the real entry point some bytes away) before trying to
    # force a Function here again.
    (0xEE13B, "Decode anomaly", "write_hw_shift_register's own label "
     "address fails CreateFunctionCmd - real entry point is offset "
     "some bytes away, not resolved yet"),
    (0xE80E4, "Decode anomaly", "handle_acq_mode_change's own label "
     "address decodes as garbage ('push cs; cmp al,byte[di]...') and "
     "fails CreateFunctionCmd - real entry point not resolved yet"),
    (0xED9BC, "Decode anomaly", "compute_and_draw_scale_marker's own "
     "label address lands mid-instruction ('add si,ax') and fails "
     "CreateFunctionCmd - real entry point not resolved yet"),
    (0xEEA58, "Decode anomaly", "clear_readout_attrs_for_item's own "
     "label address fails CreateFunctionCmd - body decodes as raw db "
     "bytes in the readable.asm too, real entry point not resolved"),
    (0xF0CCC, "Decode anomaly", "start_plot_output_task's own label "
     "address fails CreateFunctionCmd - real entry point not resolved"),

    # The candidate vector shape table - mathematically confirmed
    # 40-point circle + small circle + several open shapes, genuinely
    # unresolved whether this is a UI icon set or a rough font. See
    # docs/display/vector-icons/README.md and docs/decode-anomalies/
    # unknown-data-deep-dive-2026-09-15.md finding 3.
    (0xEAE64, "Unresolved data",
     "candidate vector shape table (icon set or rough font, unresolved "
     "which) - see docs/display/vector-icons/README.md"),
]


def main():
    project = GhidraProject.openProject(PROJECT_DIR, "Tek2230", False)
    by_program = {}
    for phys, category, note in BOOKMARKS:
        prog_name, seg, off = resolve(phys)
        if prog_name is None:
            print(f"SKIP {hex(phys)}: no program covers this address")
            continue
        by_program.setdefault(prog_name, []).append((seg, off, category, note))

    total = 0
    for prog_name, entries in by_program.items():
        program = project.openProgram("/", prog_name, False)
        try:
            af = program.getAddressFactory()
            bm = program.getBookmarkManager()
            tx = program.startTransaction("apply bookmarks")
            ok = False
            try:
                for seg, off, category, note in entries:
                    addr = af.getAddress("%04x:%04x" % (seg, off))
                    bm.setBookmark(addr, BookmarkType.NOTE, category, note)
                    total += 1
                ok = True
            finally:
                program.endTransaction(tx, ok)
            project.save(program)
            print(f"{prog_name}: {len(entries)} bookmarks applied")
        finally:
            project.close(program)
    project.close()
    print(f"\nDone: {total} bookmarks applied.")


if __name__ == "__main__":
    main()
