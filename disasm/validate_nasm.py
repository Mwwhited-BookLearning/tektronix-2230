#!/usr/bin/env python
"""
Round-trip validation of the x86 disassembly using NASM.

Each decoded instruction is checked in isolation: emitted into a fixed-
width padded "slot" with its own `org <real address>` so NASM computes
relative-branch/far-pointer math against the correct original address,
while slots stay contiguous in the assembled output (independent of
each instruction's own encoded length). This avoids the cascading
byte-shift you get from diffing one giant flat reassembly, where a
single differently-encoded instruction shifts every following byte.

For instructions with a well-known encoding ambiguity (register-to-
register op where either operand order encodes the identical effect,
e.g. `mov bp, sp` as 8B EC or `mov sp, bp` reversed as 89 E5), a
mismatch is retried with the operands swapped and counted separately
as "alternate encoding" rather than a real decode error.
"""
import re
import subprocess
import sys
import gen_disasm_x86 as g

NASM_EXE = sys.argv[1] if len(sys.argv) > 1 else "nasm"
SLOT = 16  # bytes reserved per instruction; x86-16 instructions here are <= ~8

STRING_MNEMONICS = {
    "movsb", "movsw", "stosb", "stosw", "lodsb", "lodsw",
    "cmpsb", "cmpsw", "scasb", "scasw",
}
NO_OPERAND_MNEMONICS = {
    "cdq", "cwde", "cbw", "cwd", "clc", "cld", "cli", "cmc", "hlt",
    "iret", "popf", "pushf", "ret", "retf", "stc", "sti", "int1",
    "nop", "cmpsb", "movsb", "stosb", "lodsb", "movsw", "stosw",
    "lodsw", "cmpsw", "scasb", "scasw",
}
COMMUTATIVE_REGREG_MNEMONICS = {
    "mov", "add", "sub", "and", "or", "xor", "cmp", "test", "adc", "sbb",
}
# Rare instructions encountered while expanding coverage that haven't been
# individually verified for correct NASM round-tripping yet (string I/O
# ports, array-bounds check) - conservatively fall back to raw db rather
# than risk a silently-wrong conversion. Revisit if they turn out to be
# common enough to be worth handling properly.
NOT_YET_HANDLED_MNEMONICS = {
    "insb", "insw", "insd", "outsb", "outsw", "outsd", "bound",
}

SEG_OVERRIDE_RE = re.compile(r"\b(cs|ds|es|ss):")
PTR_RE = re.compile(r"\b(byte|word|dword|qword) ptr\b")
BARE_PTR_RE = re.compile(r"(?<!\w)ptr\s+")
REGREG_RE = re.compile(r"^(\w+),\s*(\w+)$")
IMM_OPERAND_RE = re.compile(r"^(?P<dst>.+),\s*(?P<imm>-?\d+|0x[0-9a-fA-F]+)$")
# capstone sometimes omits the "0x" prefix on near-branch targets (seen
# already for far targets too - see gen_disasm_x86.FAR_TARGET_RE) - match
# either form rather than assuming "0x" is always present.
NEAR_HEX_RE = re.compile(r"^(?:0x)?[0-9a-fA-F]+$")
WORD_REGS = {"ax", "bx", "cx", "dx", "si", "di", "bp", "sp"}


# first opcode byte for the AX-specific "<op> AX, imm16" encoding, which
# is ALWAYS a 16-bit immediate (no sign-extended-imm8 alternative) -
# unlike the generic reg/mem,imm16 group-1 opcode (0x81), which DOES
# have a same-size-in-bytes sign-extended-imm8 sibling (0x83) that NASM
# will silently prefer whenever the immediate fits in a signed byte.
AX_IMM_OPCODE = {
    "add": 0x05, "or": 0x0D, "adc": 0x15, "sbb": 0x1D,
    "and": 0x25, "sub": 0x2D, "xor": 0x35, "cmp": 0x3D,
}
GROUP1_LONG_OPCODE = 0x81


SEGMENT_PREFIX_BYTES = {0x26, 0x2E, 0x36, 0x3E}  # es:/cs:/ss:/ds:


def real_opcode_byte(orig_bytes):
    """Skip a leading segment-override prefix byte (0x26/0x2E/0x36/0x3E)
    to find the actual opcode byte - needed because a segment-overridden
    instruction's first byte is the prefix, not the opcode, which would
    otherwise make widen_immediate() silently fail to recognize e.g. an
    `or word [es:di+0xa], imm` using the wide 0x81 encoding."""
    for b in orig_bytes:
        if b not in SEGMENT_PREFIX_BYTES:
            return b
    return orig_bytes[0]


def widen_immediate(mnem, op2, first_orig_byte):
    """NASM auto-shrinks a word-sized destination's immediate to the
    sign-extended imm8 encoding (0x83) whenever the value fits in a
    signed byte. Only add `strict word` when the ORIGINAL bytes prove
    the wider encoding (0x81, or the AX-specific opcode) was actually
    used - otherwise this would force the wrong (wider) encoding onto
    an original that legitimately used the short form."""
    if mnem not in AX_IMM_OPCODE:
        return op2
    if first_orig_byte not in (GROUP1_LONG_OPCODE, AX_IMM_OPCODE[mnem]):
        return op2
    m = IMM_OPERAND_RE.match(op2)
    if not m:
        return op2
    dst = m.group("dst").strip()
    imm = m.group("imm")
    is_word = dst in WORD_REGS or dst.startswith("word ")
    if not is_word:
        return op2
    return f"{dst}, strict word {imm}"


SEG_OUTSIDE_BRACKET_RE = re.compile(r"\b(cs|ds|es|ss):\[")


def fix_mem_operand(op):
    op = PTR_RE.sub(lambda m: m.group(1), op)
    op = BARE_PTR_RE.sub("", op)
    # capstone writes segment overrides as "es:[bx]" (prefix immediately
    # before the bracket); NASM requires the override *inside* the
    # brackets ("[es:bx]").
    op = SEG_OUTSIDE_BRACKET_RE.sub(lambda m: f"[{m.group(1)}:", op)
    return op


def string_segment_prefix(mnem, op):
    """String instructions' *destination* (DI side, for stos/movs/cmps/
    scas) is hardwired to ES and never takes a real prefix byte, even
    though capstone always prints it as `es:[di]`. Only the *source*
    side (SI, default DS) can carry a real segment-override prefix
    byte - and lodsb/lodsw have only a source operand. Get this wrong
    (as an earlier version of this script did) and e.g. `es lodsb`
    silently loses its real 0x26 override prefix byte."""
    if mnem in ("stosb", "stosw", "scasb", "scasw"):
        return ""  # only operand is the fixed ES:DI destination
    if mnem in ("lodsb", "lodsw"):
        m = SEG_OVERRIDE_RE.search(op)  # sole operand is the source (SI)
        seg = m.group(1) if m else "ds"
    else:  # movsb/movsw/cmpsb/cmpsw: "dest(DI,fixed-ES), source(SI)"
        parts = op.split(",", 1)
        source_text = parts[1] if len(parts) == 2 else ""
        m = SEG_OVERRIDE_RE.search(source_text)
        seg = m.group(1) if m else "ds"
    return "" if seg == "ds" else f"{seg} "


def convert(mnem, op, size, seg, chip_name, chip_base, orig_bytes):
    def near_target_addr(off_str):
        toff = int(off_str, 16) & 0xFFFF
        phys = ((seg << 4) + toff) & 0xFFFFF
        if not (chip_base <= phys < chip_base + 0x10000):
            return None
        return phys - chip_base

    if mnem.startswith("f"):
        # x87 FPU instructions (fadd/fdiv/fld/fst/...) - no x86 integer
        # mnemonic starts with "f", so this reliably catches them. Not
        # handled yet (capstone's "st(N)" operand syntax needs
        # translating for NASM); fall back to raw db rather than risk a
        # wrong conversion for a CPU feature (8087 coprocessor) we
        # haven't investigated yet - see TODO.md.
        return None

    if mnem in NOT_YET_HANDLED_MNEMONICS:
        return None

    if mnem == "lock add":
        return f"lock add {fix_mem_operand(op)}"

    if mnem.startswith("rep "):
        base = mnem.split(" ", 1)[1]
        if base not in STRING_MNEMONICS:
            return None
        prefix = string_segment_prefix(base, op)
        return f"{prefix}rep {base}"

    if mnem in STRING_MNEMONICS:
        prefix = string_segment_prefix(mnem, op)
        return f"{prefix}{mnem}"

    if mnem in ("ret", "retf") and op:
        return f"{mnem} {op}"

    # capstone names the 0x98/0x99 sign-extend opcodes after their 32-bit
    # (386+) behavior (cwde/cdq) regardless of mode; in 16-bit real mode
    # they're cbw/cwd, and writing cdq/cwde under BITS 16 makes NASM add
    # an unwanted 0x66 operand-size override prefix that isn't present
    # in the original byte.
    if mnem == "cdq":
        return "cwd"
    if mnem == "cwde":
        return "cbw"

    if mnem in NO_OPERAND_MNEMONICS:
        return mnem

    if mnem in ("jmp", "call") and NEAR_HEX_RE.match(op):
        target = near_target_addr(op)
        if target is None:
            return None
        kw = "short " if size == 2 and mnem == "jmp" else ""
        return f"{mnem} {kw}0x{target:04x}"

    if mnem.startswith("j") and mnem not in ("jmp",) and NEAR_HEX_RE.match(op):
        # 8086/8088 has no near-conditional-jump encoding (that's 386+) -
        # every jcc here MUST be the 2-byte short form. Without an explicit
        # "short", NASM will happily emit the invalid-on-this-CPU 0F 8x form.
        target = near_target_addr(op)
        if target is None:
            return None
        return f"{mnem} short 0x{target:04x}"

    if mnem == "loop" and NEAR_HEX_RE.match(op):
        target = near_target_addr(op)
        if target is None:
            return None
        return f"{mnem} 0x{target:04x}"

    if mnem in ("ljmp", "lcall"):
        m = g.FAR_TARGET_RE.match(op)
        if m:
            tseg = int(m.group(1), 16)
            toff = int(m.group(2), 16)
            return f"{mnem[1:]} 0x{tseg:04x}:0x{toff:04x}"
        if "[" in op:  # indirect far call/jmp through a memory pointer
            return f"{mnem[1:]} far {fix_mem_operand(op)}"
        return None

    if mnem in ("les", "lds"):
        return f"{mnem} {fix_mem_operand(op)}"

    op2 = fix_mem_operand(op)
    if "ptr" in op2:
        return None
    op2 = widen_immediate(mnem, op2, real_opcode_byte(orig_bytes))
    return f"{mnem} {op2}".rstrip()


DIRECTION_SWAPPABLE_OPCODES = {
    0x00, 0x01, 0x02, 0x03,  # add
    0x08, 0x09, 0x0A, 0x0B,  # or
    0x10, 0x11, 0x12, 0x13,  # adc
    0x18, 0x19, 0x1A, 0x1B,  # sbb
    0x20, 0x21, 0x22, 0x23,  # and
    0x28, 0x29, 0x2A, 0x2B,  # sub
    0x30, 0x31, 0x32, 0x33,  # xor
    0x38, 0x39, 0x3A, 0x3B,  # cmp
    0x88, 0x89, 0x8A, 0x8B,  # mov
}


def alt_direction_encoding(raw):
    """For a 2-byte register-to-register ALU/MOV encoding, return the
    other opcode+ModRM byte pair that expresses the identical operation
    (the 'd' direction bit and reg/rm ModRM subfields are inverses of
    each other for these instructions when mod==11), or None if not
    applicable."""
    if len(raw) != 2:
        return None
    opcode, modrm = raw[0], raw[1]
    if opcode not in DIRECTION_SWAPPABLE_OPCODES:
        return None
    if (modrm & 0xC0) != 0xC0:
        return None  # not register-direct addressing
    reg = (modrm >> 3) & 0x7
    rm = modrm & 0x7
    alt_opcode = opcode ^ 0x02
    alt_modrm = 0xC0 | (rm << 3) | reg
    return bytes([alt_opcode, alt_modrm])


def alt_duplicate_opcode(orig):
    """0x82 is an undocumented exact duplicate of 0x80 (group-1 Eb,Ib -
    byte-sized add/or/adc/.../cmp with an imm8; the 's' sign-extend bit
    that distinguishes 0x80 from 0x82 in the 16/32-bit-operand cases is
    meaningless for an 8-bit destination, so both encode identically).
    NASM always emits 0x80; if the ROM used 0x82, treat it as the same
    already-recognized equivalence class as the other opcode-choice
    ambiguities rather than a real mismatch."""
    if not orig or orig[0] != 0x82:
        return None
    return bytes([0x80]) + orig[1:]


def alt_displacement_encoding(orig):
    """NASM prefers the shorter mod=01 (disp8) addressing form whenever
    a mod=10 (disp16) displacement's value fits in a signed byte, even
    though these ROMs consistently use the explicit disp16 form. Given
    the ORIGINAL (disp16) bytes, return what NASM's preferred (disp8)
    encoding of the identical effective address would look like, so it
    can be compared directly against NASM's actual output."""
    if len(orig) < 4:
        return None
    opcode, modrm = orig[0], orig[1]
    mod = (modrm >> 6) & 0x3
    if mod != 0x02:
        return None
    disp = orig[2] | (orig[3] << 8)
    fits_signed_byte = disp <= 0x7F or disp >= 0xFF80
    if not fits_signed_byte:
        return None
    alt_modrm = (0x01 << 6) | (modrm & 0x3F)
    return bytes([opcode, alt_modrm, orig[2]]) + orig[4:]


def build_batch(chip_name, buf, visited, chip_base):
    entries = sorted(
        (e for e in visited.values() if e["chip"] == chip_name),
        key=lambda e: e["chip_off"],
    )
    lines = ["BITS 16"]
    slots = []  # (addr, size, mnem, op, orig_bytes, converted_line_or_None)
    for e in entries:
        addr = e["chip_off"]
        orig = buf[addr:addr + e["size"]]
        nasm_line = convert(e["mnem"], e["op"], e["size"], e["seg"], chip_name, chip_base, orig)
        sect = f"s{addr:04x}"
        lines.append(f"section {sect} vstart=0x{addr:x}")
        lines.append(f"{sect}:")
        if nasm_line is not None:
            lines.append(f"    {nasm_line}")
        else:
            lines.append("    db " + ", ".join(f"0x{b:02x}" for b in orig))
        lines.append(f"    times ({SLOT} - ($ - {sect})) db 0x90")
        slots.append((addr, e["size"], e["mnem"], e["op"], orig, nasm_line))
    return "\n".join(lines) + "\n", slots


def assemble(nasm_exe, src_path, out_path):
    result = subprocess.run([nasm_exe, "-f", "bin", "-o", out_path, src_path],
                             capture_output=True, text=True)
    return result


def main():
    chips, visited, labels = g.main()

    total = 0
    unconverted = 0
    exact = 0
    alt_encoding = 0
    real_mismatch = []

    for name, info in chips.items():
        src, slots = build_batch(name, info["buf"], visited, info["base"])
        src_path = f"_validate_{name}.asm"
        out_path = f"_validate_{name}.bin"
        open(src_path, "w").write(src)
        r = assemble(NASM_EXE, src_path, out_path)
        print(f"=== {name} === ({len(slots)} instructions)")
        if r.returncode != 0:
            print("  NASM FAILED:")
            print(r.stderr[:4000])
            continue

        assembled = open(out_path, "rb").read()

        for i, (addr, size, mnem, op, orig, nasm_line) in enumerate(slots):
            total += 1
            slot_off = i * SLOT
            got = assembled[slot_off:slot_off + size]
            if nasm_line is None:
                unconverted += 1
                continue
            if got == orig:
                exact += 1
                continue
            alt = alt_direction_encoding(got[:size])
            if alt is not None and alt == orig:
                alt_encoding += 1
                continue
            alt2 = alt_displacement_encoding(orig)
            if alt2 is not None and got[:len(alt2)] == alt2:
                alt_encoding += 1
                continue
            alt3 = alt_duplicate_opcode(orig)
            if alt3 is not None and got[:len(alt3)] == alt3:
                alt_encoding += 1
                continue
            real_mismatch.append((name, addr, mnem, op, nasm_line, orig, got))

        print(f"  exact matches so far: {exact}, alt-encoding: {alt_encoding}, "
              f"unconverted: {unconverted}, mismatches: {len(real_mismatch)}")

    print()
    print(f"TOTAL: {total} instructions")
    print(f"  exact byte match:      {exact}")
    print(f"  alt-encoding match:    {alt_encoding} (semantically identical, different valid opcode choice)")
    print(f"  not converted (skipped, raw db, unverified): {unconverted}")
    print(f"  real mismatches:       {len(real_mismatch)}")
    if real_mismatch:
        print()
        print("First 40 real mismatches:")
        for name, addr, mnem, op, nasm_line, orig, got in real_mismatch[:40]:
            print(f"  {name}:{addr:04x}  {mnem} {op!r}  ->nasm-> {nasm_line!r}  "
                  f"orig={orig.hex()}  got={got.hex()}")


if __name__ == "__main__":
    main()
