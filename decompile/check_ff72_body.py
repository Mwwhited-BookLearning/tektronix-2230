"""Read-only: does Ghidra's existing auto-analysis already cover
0xEFF99-0xEFFEE (the heuristic scanner's untraced continuation of
FUNC_3633_FF72) as part of that same function, confirming SUB_EFFEE
lands mid-instruction inside real code rather than dead garbage?"""
import pyghidra
pyghidra.start()

from ghidra.base.project import GhidraProject

PROJECT_DIR = r"C:\repo\_learning\tektronix-2230\decompile"
project = GhidraProject.openProject(PROJECT_DIR, "Tek2230", True)
program = project.openProgram("/", "160-3633-14.bin", True)

addr_factory = program.getAddressFactory()
fm = program.getFunctionManager()
listing = program.getListing()

for label, addr_str in [
    ("FUNC_3633_FF72 start", "000e:ff72"),
    ("mid-gap 0xEFF99", "000e:ff99"),
    ("jmp at 0xEFFEC", "000e:ffec"),
    ("SUB_EFFEE landing 0xEFFEE", "000e:ffee"),
]:
    addr = addr_factory.getAddress(addr_str)
    func = fm.getFunctionContaining(addr)
    at = fm.getFunctionAt(addr)
    instr = listing.getInstructionContaining(addr)
    instr_at = listing.getInstructionAt(addr)
    print(f"{label} ({addr_str}): containing function = {func} body={func.getBody() if func else None}")
    print(f"  function AT addr = {at}")
    print(f"  instruction containing addr = {instr}")
    print(f"  instruction AT addr (None if mid-instruction) = {instr_at}")

project.close()
