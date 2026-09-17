"""
Headless boot tracer - milestone 1 of emulator/docs/design.md.

Boots the CPU from the real reset vector (0xFFFF0, per MEMORY_MAP.md)
and runs, logging every unmapped-memory access and every I/O port
access, so gaps in memory_map.py surface as concrete faults instead of
being guessed at ahead of time.

Usage:
    python emu.py [--max-instructions N] [--revision 13|14]
"""
import argparse
import ctypes
import sys

import unicorn as uc
from unicorn import x86_const as x86

import memory_map as mm
from timer import TickScheduler
from io_stubs import CommPresenceProbe, DiagnosticTextCapture, install_all_fixed_reads

# A genuine 8086 (20 address lines, unlike 286+) wraps any computed
# physical address above 1MB back into the bottom of the address space
# - CS:IP or a data segment near 0xFFFF with a large offset can compute
# up to physical 0x10FFEF (0xFFFF*16+0xFFFF), which real hardware
# silently aliases to 0x00000-0xFFEF, not a fault. Unicorn's x86 core
# doesn't apply this masking on its own (confirmed by testing: without
# this alias, the boot trace hit UC_ERR_READ_UNMAPPED at physical
# 0x104A21, whose low-20-bit-masked equivalent 0x4A21 sits cleanly
# inside the low-RAM region - a real 8086 would have just read that).
# Modeled with mem_map_ptr sharing the SAME host buffer at both
# 0x00000 and 0x100000, a true zero-overhead alias rather than a
# read/write hook pair.
HMA_ALIAS_BASE = 0x100000
HMA_ALIAS_SIZE = 0x10000

# Registers seen in code as literal, fixed I/O ports (MEMORY_MAP.md's
# "I/O ports actually seen in code" table) - log-only for now, no
# stubbed read value needed since all 3 are write-only in every site
# found by static disassembly.
KNOWN_IO_PORTS = {0x83, 0xC4, 0xD1}


def hook_mem_invalid(uc_eng, access, address, size, value, user_data):
    ip = uc_eng.reg_read(x86.UC_X86_REG_EIP)
    cs = uc_eng.reg_read(x86.UC_X86_REG_CS)
    kind = {
        uc.UC_MEM_READ_UNMAPPED: "READ",
        uc.UC_MEM_WRITE_UNMAPPED: "WRITE",
        uc.UC_MEM_FETCH_UNMAPPED: "FETCH",
        uc.UC_MEM_READ_PROT: "READ(prot)",
        uc.UC_MEM_WRITE_PROT: "WRITE(prot)",
        uc.UC_MEM_FETCH_PROT: "FETCH(prot)",
    }.get(access, str(access))
    print(f"[UNMAPPED] {kind} addr=0x{address:06X} size={size} "
          f"value=0x{value:X} at CS:IP={cs:04X}:{ip:04X}")
    user_data["unmapped_hits"] += 1
    return False  # do not resume; this is a real gap in the memory map


def hook_io(uc_eng, port, size, value, user_data, is_write):
    direction = "OUT" if is_write else "IN"
    ip = uc_eng.reg_read(x86.UC_X86_REG_EIP)
    cs = uc_eng.reg_read(x86.UC_X86_REG_CS)
    note = "" if port in KNOWN_IO_PORTS else " (UNEXPECTED PORT)"
    print(f"[IO] {direction} port=0x{port:X} size={size} value=0x{value:X} "
          f"at CS:IP={cs:04X}:{ip:04X}{note}")
    user_data["io_hits"] += 1
    return 0  # value returned to the CPU on IN


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--max-instructions", type=int, default=2000,
                     help="stop after this many instructions (default 2000)")
    ap.add_argument("--revision", default="14", choices=["13", "14"])
    ap.add_argument("--tick-interval", type=int, default=2000,
                     help="instructions between synthetic INT2 ticks, "
                          "0 disables the scheduler timer entirely "
                          "(default 2000)")
    ap.add_argument("--comm-installed", action=argparse.BooleanOptionalAction,
                     default=True,
                     help="stub detect_comm_option_hw's hardware probe "
                          "to read back as 'comm option installed', "
                          "matching this project's real physical test "
                          "units (both confirmed Option 12/RS-232) - "
                          "on by default since that's what the real "
                          "hardware this project has access to actually "
                          "is; pass --no-comm-installed for the "
                          "'not installed' plain-RAM behavior instead - "
                          "see io_stubs.CommPresenceProbe")
    ap.add_argument("--stub-registers", action=argparse.BooleanOptionalAction,
                     default=True,
                     help="force-fix reads of the display-chip busy "
                          "bits and the comm/front-panel status "
                          "registers this project now has a real or "
                          "documented captured value for (Table 3-1 + "
                          "the RS-232 schematic trace + the exerciser-"
                          "screen photos), instead of plain RAM's "
                          "default 'reads back whatever was last "
                          "written' - see io_stubs.install_all_fixed_reads")
    ap.add_argument("--show-diag-text", action="store_true",
                     help="print every byte write_readout_port_byte "
                          "sends, grouped into lines - the self-test/"
                          "POST diagnostic text channel confirmed "
                          "2026-09-16 (see io_stubs.DiagnosticTextCapture)")
    args = ap.parse_args()

    emu = uc.Uc(uc.UC_ARCH_X86, uc.UC_MODE_16)
    user_data = {"unmapped_hits": 0, "io_hits": 0, "count": 0}
    ticker = TickScheduler(args.tick_interval) if args.tick_interval else None

    low_ram_buffer = None  # keeps the ctypes buffer alive for its aliases' lifetime
    for region in mm.all_regions(args.revision):
        perms = uc.UC_PROT_READ | uc.UC_PROT_EXEC
        if region.writable:
            perms |= uc.UC_PROT_WRITE
        if region.name == "low_ram_ivt_and_vars":
            low_ram_buffer = ctypes.create_string_buffer(region.size)
            ptr = ctypes.addressof(low_ram_buffer)
            emu.mem_map_ptr(region.start, region.size, perms, ptr)
            emu.mem_map_ptr(HMA_ALIAS_BASE, HMA_ALIAS_SIZE, perms, ptr)
            print(f"mapped {region.name:28s} 0x{region.start:06X}-"
                  f"0x{region.start + region.size - 1:06X} RW "
                  f"(+ HMA wraparound alias at 0x{HMA_ALIAS_BASE:06X})")
            continue
        emu.mem_map(region.start, region.size, perms)
        if region.data is not None:
            emu.mem_write(region.start, region.data)
        print(f"mapped {region.name:28s} 0x{region.start:06X}-"
              f"0x{region.start + region.size - 1:06X} "
              f"{'RW' if region.writable else 'RO'}")

    if args.comm_installed:
        CommPresenceProbe().install(emu, uc)
        print("stubbed detect_comm_option_hw's presence probe as "
              "'installed' (--comm-installed)")

    if args.stub_registers:
        install_all_fixed_reads(emu, uc)

    diag = None
    if args.show_diag_text:
        diag = DiagnosticTextCapture()
        diag.install(emu, uc)
        print("watching write_readout_port_byte for diagnostic text "
              "(--show-diag-text)")

    emu.hook_add(uc.UC_HOOK_MEM_INVALID, hook_mem_invalid, user_data)
    emu.hook_add(uc.UC_HOOK_INSN, lambda e, port, size, ud=user_data:
                 hook_io(e, port, size, 0, ud, is_write=False),
                 None, 1, 0, x86.UC_X86_INS_IN)
    emu.hook_add(uc.UC_HOOK_INSN, lambda e, port, size, value, ud=user_data:
                 hook_io(e, port, size, value, ud, is_write=True),
                 None, 1, 0, x86.UC_X86_INS_OUT)

    def hook_code(uc_eng, address, size, ud):
        ud["count"] += 1
        if ticker is not None:
            ticker.step(uc_eng)
        if ud["count"] >= args.max_instructions:
            uc_eng.emu_stop()

    emu.hook_add(uc.UC_HOOK_CODE, hook_code, user_data)

    # Real 8086 reset: CS=0xF000, IP=0xFFF0 -> physical 0xFFFF0,
    # matching MEMORY_MAP.md's confirmed reset vector location.
    emu.reg_write(x86.UC_X86_REG_CS, 0xF000)
    emu.reg_write(x86.UC_X86_REG_IP, 0xFFF0)

    print("\nStarting emulation at CS:IP=F000:FFF0 (physical 0xFFFF0)...\n")
    try:
        emu.emu_start(0xFFFF0, 0x100000, count=args.max_instructions)
    except uc.UcError as e:
        print(f"\n[STOPPED] Unicorn error: {e}")

    if diag is not None:
        diag.flush()
        print(f"\n{len(diag.lines)} diagnostic-text lines captured.")

    print(f"\nExecuted {user_data['count']} instructions, "
          f"{user_data['io_hits']} I/O accesses, "
          f"{user_data['unmapped_hits']} unmapped-memory faults.")
    if ticker is not None:
        print(f"Synthetic INT2 ticks: {ticker.fired} fired, "
              f"{ticker.skipped} skipped (IF clear or vector not yet "
              f"installed).")
    if user_data["unmapped_hits"]:
        print("See memory_map.py - the fault addresses above are real "
              "gaps to add, not bugs in Unicorn.")
        sys.exit(1)


if __name__ == "__main__":
    main()
