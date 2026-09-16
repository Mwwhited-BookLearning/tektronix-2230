"""Export Ghidra's decompiled C for every function in a program to a
single text file under decompile/exports/ - a persistent, grep-able
artifact instead of re-invoking the decompiler interactively each
time PyGhidra needs to be spun up. Fast (~1-2s per ~200-400 function
program) - safe to regenerate after any batch of Ghidra edits.

Usage (run once per program, from anywhere - paths are absolute):
    python export_decompiled_c.py 160-3633-14.bin       ../decompile/exports/160-3633-14.c
    python export_decompiled_c.py 160-3532-14.bin       ../decompile/exports/160-3532-14.c
    python export_decompiled_c.py 160-2998-14.bin       ../decompile/exports/160-2998-14.c
    python export_decompiled_c.py 160-2998-14-shadow.bin ../decompile/exports/160-2998-14-shadow.c

See docs/architecture/ghidra-project.md's "Decompiled-C export" section
for what this is useful for (cross-checking parameter names/types,
finding label-only functions the decompiler can't resolve as callees)
and its limits (calling-convention/type inference is best-effort, not
ground truth - always verify against the real disassembly before
trusting a decompiled detail)."""
import sys
import time
import pyghidra
pyghidra.start()

from ghidra.base.project import GhidraProject
from ghidra.app.decompiler import DecompInterface
from ghidra.util.task import ConsoleTaskMonitor

PROJECT_DIR = r"C:\repo\_learning\tektronix-2230\decompile"
prog_name = sys.argv[1]
out_path = sys.argv[2]

project = GhidraProject.openProject(PROJECT_DIR, "Tek2230", True)
program = project.openProgram("/", prog_name, True)
monitor = ConsoleTaskMonitor()
ifc = DecompInterface()
ifc.openProgram(program)

fm = program.getFunctionManager()
funcs = list(fm.getFunctions(True))
print(f"{prog_name}: {len(funcs)} functions to decompile")

t0 = time.time()
ok_count = 0
fail_count = 0
with open(out_path, "w", encoding="utf-8") as f:
    for i, func in enumerate(funcs):
        try:
            res = ifc.decompileFunction(func, 30, monitor)
            if res.decompileCompleted():
                code = res.getDecompiledFunction().getC()
                f.write(code)
                f.write("\n")
                ok_count += 1
            else:
                f.write(f"/* {func.getName()} @ {func.getEntryPoint()}: "
                         f"DECOMPILE FAILED: {res.getErrorMessage()} */\n\n")
                fail_count += 1
        except Exception as e:
            f.write(f"/* {func.getName()} @ {func.getEntryPoint()}: EXCEPTION: {e} */\n\n")
            fail_count += 1
        if (i + 1) % 50 == 0:
            print(f"  {i+1}/{len(funcs)} done ({time.time()-t0:.1f}s elapsed)")

ifc.dispose()
project.close(program)
project.close()
print(f"done: {ok_count} ok, {fail_count} failed, {time.time()-t0:.1f}s total")
