"""
I/O register stubs - see emulator/docs/design.md's "I/O port and MMIO
stub strategy". Each stub models just enough real hardware behavior to
let a specific piece of firmware logic reach a real, meaningful
conclusion instead of reading inert RAM - see individual docstrings for
which real register(s) each one is standing in for and why.
"""
class CommPresenceProbe:
    """Models detect_comm_option_hw's (physical 0xE75C0) hardware
    presence test: it writes a bit to the confirmed "Time Base Mode
    Register" (MEMORY_MAP.md, physical 0x4007DE) and reads back a bit
    of the confirmed "Acquisition Memory Address Buffer" (physical
    0x4377E), expecting the two to be coupled on real hardware when the
    comm option is installed (some specific wiring quirk the manual
    excerpts read so far don't detail - not needed to model here, only
    the observable behavior matters).

    With no coupling at all (plain RAM, this project's default stub),
    the probe deterministically reads back "not installed" - confirmed
    live 2026-09-16 (see docs/display/vector-display-and-stroke-font.md
    "Live emulation confirms..."). This stub adds the coupling so the
    probe can conclude "installed" instead, matching the real physical
    test units this project has hardware access to (both confirmed
    Option 12 - RS-232 - equipped)."""

    # 0x407DE, not the "0x4007DE" MEMORY_MAP.md's own row header shows -
    # that's a pre-existing typo in the doc (its own body text says
    # "Table 3-1, address 0x407DE" one sentence later, and the real
    # disassembly confirms it: es=0x4000, di=0x7de -> 0x4000*16+0x7de =
    # 0x407DE). Caught here because a wrong 7-hex-digit constant made
    # this stub's hook silently target an address detect_comm_option_hw
    # never touches, so the fix had no observable effect at all until
    # traced back to this typo. Also fixed in MEMORY_MAP.md itself.
    WRITE_ADDR = 0x407DE
    READBACK_ADDR = 0x4377E
    PRESENCE_BIT = 0x1000

    def install(self, emu, uc_module):
        emu.hook_add(uc_module.UC_HOOK_MEM_WRITE, self._on_write,
                     None, self.WRITE_ADDR, self.WRITE_ADDR)

    def _on_write(self, uc_eng, access, address, size, value, user_data):
        bit = self.PRESENCE_BIT if (value & 1) else 0
        current = int.from_bytes(uc_eng.mem_read(self.READBACK_ADDR, 2), "little")
        new_value = (current & ~self.PRESENCE_BIT) | bit
        uc_eng.mem_write(self.READBACK_ADDR, new_value.to_bytes(2, "little"))
