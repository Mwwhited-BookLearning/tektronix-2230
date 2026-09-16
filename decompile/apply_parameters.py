"""Apply this project's confirmed function parameters to the Ghidra
project - the parameter-naming counterpart of apply_names.py. Reads
disasm/gen_disasm_x86.PARAMETER_NAMES directly (offsets + names) and
infers each one's byte/word size fresh from the corresponding
disasm/*_readable.asm file (scoped to that one function's own body -
see the "scoping bug" note in docs/architecture/ghidra-project.md
before changing this logic, it's easy to get wrong again the same way)
- no precomputed/committed sizes file, so there's exactly one source
of truth for offsets+names (PARAMETER_NAMES) and one for sizes (the
.asm files), both already git-tracked.

A function whose exact offsets aren't found in its own .asm body (a
handful of comm-ROM functions only reachable via heuristic entry
points not in the default readable.asm - see PARAMETER_NAMES's own
comments for which) falls back to word-sized, with a printed warning -
verify those against disasm/gen_disasm_x86.py's own comments, which
record the individually-confirmed size for every such case.

Run this any time PARAMETER_NAMES changes. See docs/architecture/
ghidra-project.md's "Applying PARAMETER_NAMES as real stack
parameters" section for the calibration story behind the API used
here.

Usage: run from anywhere - all paths are absolute.
    python apply_parameters.py
"""
import re
import sys
import pyghidra

REPO_ROOT = r"C:\repo\_learning\tektronix-2230"
sys.path.insert(0, REPO_ROOT + r"\disasm")

pyghidra.start()

from ghidra.base.project import GhidraProject
from ghidra.program.model.listing import ParameterImpl
from ghidra.program.model.symbol import SourceType
from ghidra.program.model.data import ByteDataType, WordDataType

import gen_disasm_x86 as g

PROJECT_DIR = REPO_ROOT + r"\decompile"

# (Ghidra program, physical low, physical high, segment, .asm file to
# size-infer from). The 2998/2998-shadow split matters here too - each
# has its own .asm file even though both come from the same physical
# ROM chip.
PROGRAMS = [
    ("160-3633-14.bin", 0xE0000, 0xEFFFF, 0x000E, "160-3633-14_readable.asm"),
    ("160-3532-14.bin", 0xF0000, 0xFFFFF, 0x000F, "160-3532-14_readable.asm"),
    ("160-2998-14.bin", 0x80000, 0x8FFFF, 0x0008, "160-2998-14_readable.asm"),
    ("160-2998-14-shadow.bin", 0x90000, 0x97FFF, 0x0009, "160-2998-14_readable.asm"),
]


def resolve(phys):
    for name, lo, hi, seg, asm_file in PROGRAMS:
        if lo <= phys <= hi:
            return name, seg, phys - lo, asm_file
    return None, None, None, None


def load_asm_lines():
    cache = {}
    for _, _, _, _, asm_file in PROGRAMS:
        if asm_file not in cache:
            cache[asm_file] = open(REPO_ROOT + r"\disasm\\" + asm_file,
                                    encoding="utf-8").read().splitlines()
    return cache


def function_body_lines(chip_lines, func_name):
    """This project's own internal branch labels (L_XXXXX:) look
    exactly like a function boundary (no leading whitespace, ends with
    ':') - only a label NOT matching that pattern is a real function
    boundary. Getting this wrong silently pulls byte/word sizes from
    the wrong function (found and fixed once already, see
    docs/architecture/ghidra-project.md)."""
    start = None
    for i, line in enumerate(chip_lines):
        if line.rstrip() == func_name + ":":
            start = i + 1
            break
    if start is None:
        return None
    end = len(chip_lines)
    for i in range(start, len(chip_lines)):
        stripped = chip_lines[i].rstrip()
        if not stripped or stripped[0].isspace() or not stripped.endswith(":"):
            continue
        if stripped[:-1].startswith("L_"):
            continue
        end = i
        break
    return chip_lines[start:end]


def infer_size(body_text, off):
    off_hex = format(off, "x") if off >= 10 else str(off)
    pat = re.compile(r"\b(byte|word)\s*\[bp\s*\+\s*(?:0x)?" + re.escape(off_hex) + r"\]",
                      re.IGNORECASE)
    m = pat.search(body_text)
    return m.group(1).lower() if m else None


def main():
    asm_lines = load_asm_lines()
    by_program = {}
    unresolved_sizes = []

    for phys, offsets in g.PARAMETER_NAMES.items():
        prog_name, seg, off_base, asm_file = resolve(phys)
        if prog_name is None:
            print(f"SKIP {hex(phys)}: no program covers this address")
            continue
        func_name = g.FUNCTIONAL_NAMES.get(phys)
        body = function_body_lines(asm_lines[asm_file], func_name) if func_name else None
        body_text = "\n".join(body) if body else ""

        sized = {}
        for off, param_name in offsets.items():
            size = infer_size(body_text, off) if body_text else None
            if size is None:
                size = g.PARAMETER_SIZE_OVERRIDES.get((phys, off), "word")
                unresolved_sizes.append((hex(phys), func_name, off, param_name, size))
            sized[off] = (param_name, size)
        by_program.setdefault(prog_name, []).append((phys, seg, off_base, sized))

    if unresolved_sizes:
        print(f"{len(unresolved_sizes)} offsets not found in the default "
              f"readable.asm - resolved via PARAMETER_SIZE_OVERRIDES or the "
              f"word default instead (address, function, offset, name, size used):")
        for u in unresolved_sizes:
            print("  ", u)

    project = GhidraProject.openProject(PROJECT_DIR, "Tek2230", False)
    total_funcs = total_params = 0
    for prog_name, entries in by_program.items():
        program = project.openProgram("/", prog_name, False)
        try:
            fm = program.getFunctionManager()
            af = program.getAddressFactory()
            tx = program.startTransaction(f"apply parameter names ({prog_name})")
            ok = False
            try:
                for phys, seg, off, sized in entries:
                    addr = af.getAddress("%04x:%04x" % (seg, off))
                    func = fm.getFunctionAt(addr) or fm.getFunctionContaining(addr)
                    if func is None:
                        print(f"  {hex(phys)}: no Ghidra function here, skipping")
                        continue
                    while func.getParameterCount() > 0:
                        func.removeParameter(0)
                    for param_off in sorted(sized):
                        param_name, size = sized[param_off]
                        dt = ByteDataType.dataType if size == "byte" else WordDataType.dataType
                        func.addParameter(ParameterImpl(param_name, dt, param_off, program),
                                           SourceType.USER_DEFINED)
                        total_params += 1
                    total_funcs += 1
                ok = True
            finally:
                program.endTransaction(tx, ok)
            project.save(program)
            print(f"{prog_name}: applied to {len(entries)} functions")
        finally:
            project.close(program)
    project.close()
    print(f"\nDone: {total_funcs} functions, {total_params} parameters applied.")


if __name__ == "__main__":
    main()
