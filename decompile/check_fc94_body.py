"""Read-only: what body does Ghidra's existing (committed) analysis
assign to the function containing FUNC_3633_FC94 (0xEFC94), and does
it include the 0xEFDC2-0xEFE29 SUB_EFDC2 tail? Cross-check for the
landing-artifact investigation. No writes, no transaction needed."""
import pyghidra
pyghidra.start()

from ghidra.base.project import GhidraProject

PROJECT_DIR = r"C:\repo\_learning\tektronix-2230\decompile"
project = GhidraProject.openProject(PROJECT_DIR, "Tek2230", True)
program = project.openProgram("/", "160-3633-14.bin", True)

addr_factory = program.getAddressFactory()
fm = program.getFunctionManager()

for label, addr_str in [
    ("FUNC_3633_FC94", "000e:fc94"),
    ("just before SUB_EFDC2 (0xEFDBF)", "000e:fdbf"),
    ("SUB_EFDC2 (0xEFDC2)", "000e:fdc2"),
    ("SUB_EFD99 (0xEFD99)", "000e:fd99"),
]:
    addr = addr_factory.getAddress(addr_str)
    func = fm.getFunctionContaining(addr)
    at = fm.getFunctionAt(addr)
    print(f"{label} ({addr_str}): containing function = {func}"
          f"  body={func.getBody() if func else None}; function AT addr = {at}")

project.close()
