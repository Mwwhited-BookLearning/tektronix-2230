"""One-off: does Ghidra's own CreateFunctionCmd recognize these
addresses as valid function boundaries? Cross-check for the
SUB_EFDC2/SUB_EFFEE/FUNC_3633_9611 landing-artifact investigation
(see docs/decode-anomalies/landing-artifacts-and-jump-tables.md's
2026-10-11 follow-up). Not a persistent project script - ad hoc,
read-only probe (transaction is rolled back, project is never saved).
"""
import pyghidra
pyghidra.start()

from ghidra.base.project import GhidraProject
from ghidra.app.cmd.function import CreateFunctionCmd
from ghidra.util.task import ConsoleTaskMonitor

PROJECT_DIR = r"C:\repo\_learning\tektronix-2230\decompile"
project = GhidraProject.openProject(PROJECT_DIR, "Tek2230", True)
program = project.openProgram("/", "160-3633-14.bin", True)
monitor = ConsoleTaskMonitor()

addr_factory = program.getAddressFactory()
fm = program.getFunctionManager()

for label, addr_str in [
    ("FUNC_3633_9611", "000e:9611"),
    ("SUB_EFDC2", "000e:fdc2"),
    ("SUB_EFFEE", "000e:ffee"),
]:
    addr = addr_factory.getAddress(addr_str)
    existing = fm.getFunctionAt(addr)
    print(f"{label} ({addr_str}): existing function = {existing}")

    tx = program.startTransaction(f"probe CreateFunctionCmd at {addr_str}")
    try:
        cmd = CreateFunctionCmd(addr)
        ok = cmd.applyTo(program, monitor)
        print(f"  CreateFunctionCmd.applyTo -> {ok}  msg={cmd.getStatusMsg()!r}")
        after = fm.getFunctionAt(addr)
        if after is not None:
            print(f"  function created: {after}  body={after.getBody()}")
    finally:
        program.endTransaction(tx, False)  # always roll back - read-only probe

    # confirm rollback took effect
    print(f"  function at addr after rollback -> {fm.getFunctionAt(addr)}")

project.close()
print("done - no save() called, project untouched")
