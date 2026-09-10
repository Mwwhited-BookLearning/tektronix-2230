#!/usr/bin/env python
"""
Recursive-descent disassembler for the Tektronix 2230 main system ROM pair.

Confirmed facts (see NOTES.md):
  - CPU: Intel 8088/8086, 16-bit real mode.
  - 160-3633-14.bin (silkscreen "sys_rom_0", socket U9109) is mapped at
    physical 0xE0000-0xEFFFF.
  - 160-3532-14.bin (silkscreen "sys_rom_1", socket U9110) is mapped at
    physical 0xF0000-0xFFFFF and contains the CPU reset vector at FFFF0
    (a far JMP into the 3633 half).
  - Together they form one 128KB address space for the acquisition/
    display CPU. The comm/GPIB ROM (160-2998) is a separate, still-
    unmapped piece (bank-switched option-board ROM) and is NOT handled
    by this script yet.

Approach: recursive descent from known entry points, tracking (segment,
offset) so near vs far transfers are handled correctly, instead of a
blind linear sweep. This only disassembles code actually reachable from
a real entry point, which is far more trustworthy than sweeping every
byte. Anything not reached is left as unclassified data for a later pass.
"""
import json
import re
import capstone as cs

FAR_TARGET_RE = re.compile(r"^\s*(?:0x)?([0-9a-fA-F]+)\s*[:,]\s*(?:0x)?([0-9a-fA-F]+)\s*$")

CHIPS = {
    "3633": {"path": "../binary/160-3633-14.bin", "phys_base": 0xE0000},
    "3532": {"path": "../binary/160-3532-14.bin", "phys_base": 0xF0000},
    # Not bank-switched after all - confirmed by checking every far-call
    # target landing in 0x80000-0x8FFFF against this file's own function
    # starts: 82/84 land exactly on one. This is a plain 64KB device at
    # a fixed physical address, the same simple pattern as the two main-
    # ROM halves above (there was no need to bank-switch: the 8088's 1MB
    # address space has plenty of room free). See NOTES.md "The comm ROM
    # is NOT bank-switched".
    "2998": {"path": "../binary/160-2998-14.bin", "phys_base": 0x80000},
    # 0x90000-0x97FFF is an address-decode ALIAS of the comm ROM's own
    # upper half (0x88000-0x8FFFF, file offset 0x8000-0xFFFF) - not a
    # separate device. Confirmed by brute-forcing every possible base
    # offset against 82 observed far-call targets in that range: only
    # base=0x88000 (equivalently, this alias's own base of 0x90000,
    # pointing at file offset 0x8000+) gives 82/82 exact matches against
    # the comm ROM's own function-start signatures. Almost certainly
    # incomplete address-line decoding in the chip-select logic. See
    # NOTES.md "The 0x90000-0x97FFF region is fully resolved".
    "2998_alias_90000": {"path": "../binary/160-2998-14.bin",
                          "phys_base": 0x90000, "slice": (0x8000, 0x10000)},
}

ENTRY_POINTS = [
    # (segment, offset, label)
    (0xF000, 0xFFF0, "RESET"),
    (0xE5D1, 0x00B4, "ENTRY_E5D1_B4"),
    (0xE5D1, 0x00C7, "ENTRY_E5D1_C7"),
    # Found by tracing interrupt-vector-table writes (mov word [es:bx],
    # imm with es=0 or es=0x3f, i.e. real IVT slots) in the already-
    # decoded code: these handler addresses are never reached by any
    # direct call/jmp in the program, only by the corresponding
    # hardware/software interrupt actually firing.
    (0xE5D1, 0x019D, "INT1_HANDLER"),           # INT 1 (single-step/trap)
    (0xE5D1, 0x0090, "INT255_HANDLER_EARLY"),   # INT 255, installed at reset
    (0xE60B, 0x0005, "INT255_HANDLER_LATE"),    # INT 255, reinstalled later
    (0xE60B, 0x003A, "INT2_HANDLER_LATE"),      # INT 2 (NMI), reinstalled later
    # Both 16KB pages at file offset 0x8000 and 0xC000 in the comm/GPIB
    # ROM (160-2998) open with an identical, deliberate far jump here
    # (`EA 00 00 4C E6` = `ljmp 0xE64C:0`) - a directly-observed, high-
    # confidence entry point (unlike the comm ROM's OWN disassembly,
    # which is seeded heuristically - see gen_disasm_2998.py).
    (0xE64C, 0x0000, "COMM_ROM_BOOTSTUB_TARGET"),
]

# Semantic names for routines/branch targets whose purpose has been
# understood well enough to name (see FUNCTIONS.md/VARIABLES.md for
# the evidence behind each one - keep the two in sync). Keyed by
# physical address; render() prefers these over the address-based
# SUB_XXXXX/L_XXXXX placeholder. Unlike ENTRY_POINTS, these aren't
# necessarily entry points themselves - just names for labels the
# recursive descent already finds on its own.
FUNCTIONAL_NAMES = {
    0xE00B1: "boot_init",                    # RESET's target: cli, clear
                                              # RAM, first stack setup
    0xE416F: "print_selftest_banner",        # CORRECTED (was misnamed
                                              # self_test_dispatcher): only
                                              # prints the "before"/"after"
                                              # banner lines and sets
                                              # [0x1B10]=3 - no OR-fold test
                                              # calls at all. See NOTES.md
                                              # "self_test_dispatcher was
                                              # misnamed"
    0xE4244: "self_test_dispatcher",         # the REAL dispatcher: ~14
                                              # calls to per-subsystem test
                                              # routines, each OR-folded into
                                              # [bp-0xA] - see NOTES.md
                                              # "Found: the self-test
                                              # dispatcher"
    0xE44F1: "check_comm_option_installed",  # ROM-header-checksum +
                                              # RAM/IO presence probe -
                                              # see NOTES.md "Found: the
                                              # option-board presence/
                                              # RAM-detection routine"
    0xE0AF5: "print_string_far",             # loops a far-pointer
                                              # nul-terminated string,
                                              # print_char per byte - see
                                              # NOTES.md "The readout/CRT
                                              # display memory"
    0xE0B2A: "print_char",                   # thin wrapper: unpacks one
                                              # byte arg, calls
                                              # write_readout_port_byte
    0xE0B50: "write_readout_port_byte",      # writes the passed byte to
                                              # the fixed physical address
                                              # 0x40000+0x6F0 - see NOTES.md
                                              # "The readout/CRT display
                                              # memory"
    0xE39F0: "append_readout_char",          # appends a (char, attribute)
                                              # byte pair into the readout
                                              # line buffer at [0x1AF4],
                                              # duplicating into a second
                                              # plane 0x8000 higher in the
                                              # same segment - see NOTES.md
    0xFBC09: "memcpy_far",                   # generic far-pointer block
                                              # copy: (dest, src, len)

    # --- Renamed during the "rename everything" pass (session:
    # keep going until everything is renamed) - see NOTES.md "The
    # readout vector display list" and FUNCTIONS.md for evidence. ---
    0x9470E: "set_ds_return_old",             # push ds; mov ds,[bp+6];
                                               # pop ax - swaps DS to the
                                               # caller-given segment,
                                               # returns the OLD ds in ax
                                               # so the caller can restore
                                               # it later with a 2nd call
    0xE31DC: "strncat_far",                   # (dest,src far ptrs, max
                                               # len) - finds dest's NUL,
                                               # appends src up to the
                                               # limit, re-terminates
    0xE323F: "strncpy_far",                   # (dest,src far ptrs, max
                                               # len) - bounded copy from
                                               # src start (no dest-end
                                               # search, unlike strncat)
    0xE327F: "format_number",                 # (value, radix, width,
                                               # overflow-flag, ...) ->
                                               # far ptr to ASCII digits
                                               # built backwards into the
                                               # fixed scratch buffer at
                                               # [0x1B34]; sign/space/'*'-
                                               # overflow prefix handling
    0xE3372: "format_hex_word",               # format_number wrapper:
                                               # radix=16, width=5
    0xE3395: "format_decimal_word",           # format_number wrapper:
                                               # radix=10, width=6
    0xE33B8: "format_word_radix",             # format_number wrapper:
                                               # caller-supplied radix,
                                               # width=5
    0xE34CB: "format_byte_hex",               # byte -> 2 hex ASCII
                                               # digits + NUL, written to
                                               # the fixed scratch buffer
                                               # at [0x1B4A]; simpler/
                                               # separate from
                                               # format_number
    0xE35EF: "init_print_record",             # sets a print record's
                                               # attribute byte + position
                                               # (defaults to the current
                                               # global cursor [0x1AF4] if
                                               # no override given, else
                                               # moves the global cursor)
    0xE3662: "pack_row_col_bits",              # bit-packs two 5-bit
                                               # fields into a print
                                               # record's bytes 1 & 3,
                                               # preserving each byte's
                                               # high 3 bits
    0xE36A5: "set_position_record",           # encodes two coordinates
                                               # (>>3, character-cell
                                               # granularity) into a print
                                               # record's bytes 1-4
    0xE3520: "build_print_record",            # orchestrates
                                               # init_print_record ->
                                               # set_position_record ->
                                               # pack_row_col_bits into
                                               # one combined record
    0xE3567: "init_print_region",             # build_print_record
                                               # wrapper with default
                                               # 0x10x0x10 cell size, no
                                               # position override - the
                                               # common case
    0xE3766: "mark_readout_delimiter",        # writes attribute=2 (the
                                               # same code E3930 uses as
                                               # its wraparound marker)
                                               # into the next 2 readout
                                               # buffer cells' attribute
                                               # plane and advances the
                                               # cursor by 2
    0xE374E: "close_print_record",            # calls mark_readout_
                                               # delimiter, then tags the
                                               # caller's record[0] with
                                               # completion code 0x11 -
                                               # CORRECTED: earlier notes
                                               # wrongly assumed this
                                               # printed a string (it
                                               # doesn't traverse [bp+6]
                                               # at all, just tags it)
    0xE3736: "close_print_record_b",          # same shape as
                                               # close_print_record but
                                               # tags with 0x39 instead of
                                               # 0x11 - a different record
                                               # "kind"/completion code,
                                               # exact meaning of 0x11 vs
                                               # 0x39 not confirmed
    0xE3821: "print_readout_string",          # loops a far-pointer nul-
                                               # terminated string calling
                                               # draw_readout_char per
                                               # byte - the vector-display-
                                               # list sibling of
                                               # print_string_far (which
                                               # instead goes straight to
                                               # the 0x40000+0x6F0 port)
    0xE3854: "draw_readout_char",             # looks up a character's
                                               # stroke list in the font
                                               # table at [0x1DB0], walks
                                               # each stroke byte (pen-up/
                                               # down bit + packed coarse/
                                               # fine deltas), calls
                                               # plot_readout_point_
                                               # relative per stroke - the
                                               # CRT readout's character
                                               # rasterizer
    0xE3900: "plot_readout_point_relative",   # (dx,dy,attr) - adds the
                                               # offsets to the current
                                               # base position ([0x1AF8]/
                                               # [0x1AFA]) and calls
                                               # plot_readout_point
    0xE4217: "print_banner_line",             # (far-ptr string) - calls
                                               # print_readout_string on
                                               # the given string, then
                                               # print_string_far twice
                                               # more on two FIXED strings
                                               # in the 0xFF7B string
                                               # table - used by
                                               # print_selftest_banner for
                                               # both its "before"/"after"
                                               # lines
    0xE3930: "plot_readout_point",            # (x,y,attr) - THE
                                               # fundamental primitive:
                                               # appends a (y,x) coordinate
                                               # pair + duplicated
                                               # attribute to the readout
                                               # vector display-list
                                               # buffer at [0x1CC4],
                                               # handles circular-buffer
                                               # wraparound (marker
                                               # attribute=2, matches
                                               # mark_readout_delimiter)
}

CALL_MNEMONICS = {"call", "lcall"}
JUMP_MNEMONICS = {"jmp", "ljmp"}
COND_JUMP_PREFIX = "j"  # je, jne, jg, jl, ... (capstone x86 conditional jumps)
RET_MNEMONICS = {"ret", "retf", "iret"}
STOP_MNEMONICS = {"hlt"} | RET_MNEMONICS


def load_chips(chip_defs=None):
    if chip_defs is None:
        chip_defs = CHIPS
    data = {}
    for name, info in chip_defs.items():
        buf = open(info["path"], "rb").read()
        if "slice" in info:
            lo, hi = info["slice"]
            buf = buf[lo:hi]
        data[name] = {"buf": buf, "base": info["phys_base"], "size": len(buf)}
    return data


def phys_to_chip_offset(chips, phys):
    for name, c in chips.items():
        if c["base"] <= phys < c["base"] + c["size"]:
            return name, phys - c["base"]
    return None, None


def main(chip_defs=None, entry_points=None):
    chips = load_chips(chip_defs)
    entry_points = ENTRY_POINTS if entry_points is None else entry_points
    md = cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_16)
    md.detail = False

    visited = {}   # phys addr -> instruction dict
    queue = []     # (seg, off) to process
    labels = {}    # phys addr -> {"name":..., "kind":..., "refs":[...]}

    def seg_off_to_phys(seg, off):
        return ((seg << 4) + off) & 0xFFFFF

    def add_label(phys, kind, from_phys):
        lab = labels.setdefault(phys, {"kind": kind, "refs": []})
        if kind == "sub" and lab["kind"] != "sub":
            lab["kind"] = "sub"
        lab["refs"].append(from_phys)

    for seg, off, name in entry_points:
        phys = seg_off_to_phys(seg, off)
        labels[phys] = {"kind": "entry", "refs": [], "fixed_name": name}
        queue.append((seg, off))

    while queue:
        seg, off = queue.pop()
        cs_val = seg  # current code segment for this walk

        while True:
            phys = seg_off_to_phys(cs_val, off)
            if phys in visited:
                break  # already decoded, converges into existing flow
            chip_name, chip_off = phys_to_chip_offset(chips, phys)
            if chip_name is None:
                break  # ran off into unmapped memory (RAM/IO) - stop this path
            buf = chips[chip_name]["buf"]
            chunk = buf[chip_off:chip_off + 16]
            insns = list(md.disasm(chunk, off))
            if not insns:
                break
            insn = insns[0]

            visited[phys] = {
                "phys": phys, "seg": cs_val, "off": off,
                "chip": chip_name, "chip_off": chip_off,
                "mnem": insn.mnemonic, "op": insn.op_str,
                "size": insn.size,
                "raw": buf[chip_off:chip_off + insn.size].hex(),
            }

            mnem = insn.mnemonic
            nxt_off = (off + insn.size) & 0xFFFF

            if mnem in ("ljmp", "lcall"):
                m = FAR_TARGET_RE.match(insn.op_str)
                if m:
                    tseg = int(m.group(1), 16)
                    toff = int(m.group(2), 16)
                    tphys = seg_off_to_phys(tseg, toff)
                    add_label(tphys, "sub" if mnem == "lcall" else "loc", phys)
                    queue.append((tseg, toff))
                    if mnem == "lcall":
                        off = nxt_off
                        continue
                    else:
                        break
                # unresolved indirect far jmp/call
                if mnem == "ljmp":
                    break
                off = nxt_off
                continue

            if mnem == "jmp":
                if insn.op_str.startswith("0x"):
                    toff = int(insn.op_str, 16)
                    tphys = seg_off_to_phys(cs_val, toff)
                    add_label(tphys, "loc", phys)
                    off = toff
                    continue
                break  # unresolved indirect jmp (register/memory) - stop this path

            if mnem == "call":
                if insn.op_str.startswith("0x"):
                    toff = int(insn.op_str, 16)
                    tphys = seg_off_to_phys(cs_val, toff)
                    add_label(tphys, "sub", phys)
                    queue.append((cs_val, toff))
                # unresolved indirect call: target unknown, but a call always
                # returns to nxt_off, so fall through either way
                off = nxt_off
                continue

            if mnem.startswith(COND_JUMP_PREFIX) and mnem not in ("jmp",) and insn.op_str.startswith("0x"):
                toff = int(insn.op_str, 16)
                tphys = seg_off_to_phys(cs_val, toff)
                add_label(tphys, "loc", phys)
                queue.append((cs_val, toff))
                off = nxt_off
                continue

            if mnem in STOP_MNEMONICS:
                break

            off = nxt_off

    return chips, visited, labels


def render(chips, visited, labels, out_path, sym_path):
    for phys, lab in labels.items():
        if phys in FUNCTIONAL_NAMES:
            lab["name"] = FUNCTIONAL_NAMES[phys]
        elif "fixed_name" in lab:
            lab["name"] = lab["fixed_name"]
        else:
            lab["name"] = ("SUB_%05X" % phys) if lab["kind"] == "sub" else ("L_%05X" % phys)

    ordered = sorted(visited.values(), key=lambda e: e["phys"])
    with open(out_path, "w") as f:
        f.write("; Tektronix 2230 main system ROM (160-3532-14 + 160-3633-14)\n")
        f.write("; Intel 8088/8086 real mode, recursive-descent from reset vector.\n")
        f.write("; CONFIRMED mapping: 3633=0xE0000-0xEFFFF, 3532=0xF0000-0xFFFFF\n")
        f.write("; Labels are address-based placeholders; rename to functional\n")
        f.write("; names in the .symbols.json as their purpose is understood.\n\n")

        last_phys = None
        for e in ordered:
            phys = e["phys"]
            if last_phys is not None and phys != last_phys:
                f.write("\n")
            lab = labels.get(phys)
            if lab:
                f.write(f"{lab['name']}:\n")

            op = e["op"]
            mnem = e["mnem"]
            resolved = None
            if mnem in ("jmp", "call") and op.startswith("0x"):
                toff = int(op, 16)
                tphys = ((e["seg"] << 4) + toff) & 0xFFFFF
                resolved = labels.get(tphys, {}).get("name")
            elif mnem in ("ljmp", "lcall"):
                m = FAR_TARGET_RE.match(op)
                if m:
                    tseg = int(m.group(1), 16)
                    toff = int(m.group(2), 16)
                    tphys = ((tseg << 4) + toff) & 0xFFFFF
                    resolved = labels.get(tphys, {}).get("name")
            elif mnem.startswith("j") and mnem != "jmp" and op.startswith("0x"):
                toff = int(op, 16)
                tphys = ((e["seg"] << 4) + toff) & 0xFFFFF
                resolved = labels.get(tphys, {}).get("name")

            if resolved:
                op = f"{resolved}  ; {op}"

            f.write(f"    {e['chip']}:{e['chip_off']:04X}  [{phys:05X}]  "
                     f"{e['raw']:<14s}  {mnem:<7s} {op}\n")
            last_phys = phys + e["size"]

    sym_out = {
        ("%05X" % phys): {
            "name": lab["name"],
            "kind": lab["kind"],
            "ref_count": len(lab["refs"]),
            "functional_name": FUNCTIONAL_NAMES.get(phys),
            "notes": None,
        }
        for phys, lab in sorted(labels.items())
    }
    with open(sym_path, "w") as f:
        json.dump(sym_out, f, indent=2)


if __name__ == "__main__":
    chips, visited, labels = main()
    render(chips, visited, labels, "sysrom_3532_3633.lst", "sysrom_3532_3633.symbols.json")
    print(f"visited {len(visited)} instructions, {len(labels)} labels")
