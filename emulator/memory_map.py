"""
Memory map wiring for the Tek 2230 emulator, sourced directly from
MEMORY_MAP.md's "Confirmed regions" table - see emulator/docs/design.md
for the design-phase rationale. Do not hand-edit addresses here without
also updating MEMORY_MAP.md (or vice versa) - the two must stay in sync
per this project's own convention against docs drifting apart.
"""
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
BINARY_DIR = REPO_ROOT / "binary"

PAGE = 0x1000  # Unicorn requires page-aligned, page-sized mappings


def _round_up(n, align):
    return (n + align - 1) // align * align


class Region:
    def __init__(self, name, start, size, writable, data=None):
        self.name = name
        self.start = start
        self.size = _round_up(size, PAGE)
        self.writable = writable
        self.data = data  # bytes to load at `start`, or None for zero-filled RAM


def load_rom_regions(revision="14"):
    """The 3 ROM images plus the comm ROM's confirmed lower/upper-half
    split (0x88000-0x8FFFF is RAM, NOT a ROM continuation - see
    docs/design.md's note on this exact mistake caught while writing
    the design doc)."""
    sys_low = (BINARY_DIR / f"160-3633-{revision}.bin").read_bytes()
    sys_high = (BINARY_DIR / f"160-3532-{revision}.bin").read_bytes()
    comm = (BINARY_DIR / f"160-2998-{revision}.bin").read_bytes()
    assert len(sys_low) == 0x10000, len(sys_low)
    assert len(sys_high) == 0x10000, len(sys_high)
    assert len(comm) == 0x10000, len(comm)

    return [
        Region("sys_rom_low", 0xE0000, 0x10000, writable=False, data=sys_low),
        Region("sys_rom_high", 0xF0000, 0x10000, writable=False, data=sys_high),
        Region("comm_rom_low", 0x80000, 0x8000, writable=False, data=comm[0x0000:0x8000]),
        Region("comm_rom_high", 0x90000, 0x8000, writable=False, data=comm[0x8000:0x10000]),
    ]


def ram_regions():
    """RAM regions confirmed in MEMORY_MAP.md. Anything touched at
    runtime outside these gets reported by the unmapped-access hook in
    emu.py rather than silently guessed at here - see design.md's note
    on the 0x02090-0x021F0 far-pointer table's open status."""
    return [
        Region("low_ram_ivt_and_vars", 0x00000, 0x10000, writable=True),
        # Single region for 0x40000-0x4FFFF: stack, I/O window, and the
        # acquisition-memory sub-range (0x48000-0x4BFFF) all fall
        # inside it, so mapping this as one block avoids an overlap.
        Region("stack_io_and_acquisition", 0x40000, 0x10000, writable=True),
        Region("comm_option_nvram", 0x88000, 0x8000, writable=True),
    ]


def all_regions(revision="14"):
    return load_rom_regions(revision) + ram_regions()
