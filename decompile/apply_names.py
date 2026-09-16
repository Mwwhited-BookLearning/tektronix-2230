"""Apply this project's confirmed function names and descriptions to
the Ghidra project - the "recipe" that reconstructs Ghidra's naming
state from pure git-tracked text (disasm/gen_disasm_x86.py's
FUNCTIONAL_NAMES + the repo root FUNCTIONS.md), so the 17MB
decompile/Tek2230.rep/ database never has to be the thing that's
backed up or moved to a new machine - only this script and its 2 text
sources need to survive.

Run this any time FUNCTIONAL_NAMES or FUNCTIONS.md changes, or after
recreating the 4 programs from scratch on a fresh machine (see
docs/architecture/ghidra-project.md's "Rebuilding this project from
scratch" section for the full disaster-recovery runbook).

Usage: run from anywhere - all paths are absolute.
    python apply_names.py
"""
import re
import sys
import pyghidra

REPO_ROOT = r"C:\repo\_learning\tektronix-2230"
sys.path.insert(0, REPO_ROOT + r"\disasm")

pyghidra.start()

from ghidra.base.project import GhidraProject
from ghidra.program.model.symbol import SourceType
from ghidra.program.model.listing import CodeUnit
from ghidra.app.cmd.function import CreateFunctionCmd
from ghidra.app.cmd.disassemble import DisassembleCommand
from ghidra.util.task import TaskMonitor

import gen_disasm_x86 as g

PROJECT_DIR = REPO_ROOT + r"\decompile"

# (program, low, high) - which of the 4 programs owns a given physical
# address. 0x90000-0x97FFF only ever applies to the shadow program
# directly, at its own natural address - never computed as an alias of
# the 0x80000-0x87FFF range (that exact mistake was made and reverted
# once already, see docs/architecture/ghidra-project.md).
PROGRAMS = [
    ("160-3633-14.bin", 0xE0000, 0xEFFFF),
    ("160-3532-14.bin", 0xF0000, 0xFFFFF),
    ("160-2998-14.bin", 0x80000, 0x8FFFF),
    ("160-2998-14-shadow.bin", 0x90000, 0x97FFF),
]


def resolve_program(phys):
    for name, lo, hi in PROGRAMS:
        if lo <= phys <= hi:
            return name
    return None


ROW_RE = re.compile(
    r"^\|\s*`(0x[0-9A-Fa-f]+)`\s*\|\s*`([A-Za-z_][A-Za-z0-9_]*)`[^|]*\|\s*(.*?)\s*\|\s*([^|]*?)\s*\|$"
)


def parse_functions_md():
    """Returns {phys: {"name": ..., "desc": ..., "confidence": ...}}."""
    rows = {}
    for line in open(REPO_ROOT + r"\FUNCTIONS.md", encoding="utf-8"):
        m = ROW_RE.match(line.rstrip())
        if not m:
            continue
        addr_hex, name, desc, confidence = m.groups()
        rows[int(addr_hex, 16)] = {"name": name, "desc": desc, "confidence": confidence}
    return rows


def apply_to_program(project, prog_name, entries, descs):
    program = project.openProgram("/", prog_name, False)
    af = program.getAddressFactory()
    fm = program.getFunctionManager()
    listing = program.getListing()

    tx = program.startTransaction("apply confirmed function names")
    applied = created = labeled_only = 0
    skipped = []
    try:
        for phys, name in entries:
            seg, off = phys >> 16, phys & 0xFFFF
            try:
                addr = af.getAddress("%04x:%04x" % (seg, off))
            except Exception as e:
                skipped.append((hex(phys), name, f"bad address: {e}"))
                continue

            func = fm.getFunctionAt(addr)
            if func is None:
                if listing.getInstructionAt(addr) is None:
                    DisassembleCommand(addr, None, True).applyTo(program, TaskMonitor.DUMMY)
                if CreateFunctionCmd(addr).applyTo(program, TaskMonitor.DUMMY):
                    func = fm.getFunctionAt(addr)
                    created += 1

            if func is not None:
                try:
                    func.setName(name, SourceType.USER_DEFINED)
                    applied += 1
                except Exception as e:
                    skipped.append((hex(phys), name, f"setName failed: {e}"))
            else:
                try:
                    program.getSymbolTable().createLabel(addr, name, SourceType.USER_DEFINED)
                    labeled_only += 1
                except Exception as e:
                    skipped.append((hex(phys), name, f"label failed: {e}"))
                    continue

            info = descs.get(phys)
            if info:
                comment = f"{info['name']} (confidence: {info['confidence']})\n\nEvidence: {info['desc']}"
                listing.setComment(addr, CodeUnit.PLATE_COMMENT, comment)
    finally:
        program.endTransaction(tx, True)

    print(f"{prog_name}: renamed_existing={applied - created} newly_created_func={created} "
          f"label_only={labeled_only} skipped={len(skipped)}")
    for s in skipped[:10]:
        print("  skipped:", s)

    project.save(program)
    project.close(program)


def main():
    descs = parse_functions_md()
    print(f"parsed {len(descs)} FUNCTIONS.md rows")

    by_program = {}
    for phys, name in g.FUNCTIONAL_NAMES.items():
        prog = resolve_program(phys)
        if prog is None:
            print(f"SKIP {hex(phys)} ({name}): no program covers this address")
            continue
        by_program.setdefault(prog, []).append((phys, name))
    print(f"{sum(len(v) for v in by_program.values())} FUNCTIONAL_NAMES entries to apply")

    project = GhidraProject.openProject(PROJECT_DIR, "Tek2230", False)
    for prog_name, entries in by_program.items():
        apply_to_program(project, prog_name, entries, descs)
    project.close()
    print("DONE")


if __name__ == "__main__":
    main()
