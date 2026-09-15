# Comm ROM: 160-2998-13 vs -14 byte diff

Both physical test units used for live hardware sessions run different
comm-ROM revisions (Scope 1 = `-13`, Scope 2 = `-14`, confirmed via the
`/DIAGNOSTICS/EXERCISERS/CONFIGURATION` screen - see `HARDWARE.md`),
but this project's comm-ROM disassembly has only ever covered `-14`.
Unlike the two main ROMs (`160-3532`/`160-3633`), which are
byte-identical between revisions except a 4-byte header, `160-2998-13`
and `-14` genuinely differ - this doc is the first x86-level look at
those differences.

## Overall diff

A straight byte-for-byte diff of the two 64KB images finds **133
differing bytes total**, in exactly three runs:

| File offset | Physical | Length | What |
|---|---|---|---|
| `0x0000-0x0005` | `0x80000-0x80005` | 6 | ROM self-ID header (revision/checksum bytes - expected per-revision difference, same convention as the main ROMs' 4-byte header) |
| `0x4000-0x4042` | `0x84000-0x84042` | 67 | See "The 0x84000 region" below |
| `0xC000-0xC03E` | `0x8C000-0x8C03E` | 63 | See "The 0x8C000 region" below |

Every other byte in the 64KB image is identical between the two
revisions. Both differing regions happen to start exactly at a
16KB-page boundary (`0x4000`, `0xC000`) - that's what earlier TODO
notes meant by "two ~16KB-aligned regions," not that the regions
themselves are 16KB long (they're not; they're a few dozen bytes each).

## The 0x84000 region

**`160-2998-13`** has real executable code here - a short chain of
config-byte checks, decoded from the shared bytes just before the
diff run (`0x83FFD`: `80 3E 23 ...`, a `cmp byte [imm16], imm8`
whose address operand is only fully determined once the diverging
bytes are read):

```
0x83FFD  cmp byte [0x0623], 0x00
0x84003  jnz 0x84078
0x84005  cmp byte [0x0002], 0x00
0x8400A  jz  0x8402F
0x8400C  cmp byte [0x05FA], 0x04
0x84011  jz  0x8401A
0x84013  cmp byte [0x05FA], 0x06
0x84018  jnz 0x84027
0x8401A  jmp 0x8401F
0x8401F  mov byte [0x05FA], 0x06
0x84024  jmp 0x8402F
0x84027  mov byte [0x0623], 0x04
0x8402C  jmp 0x84078
0x8402F  cmp byte [0x05FA], 0x06
0x84034  jnz 0x8407B
0x84036  cmp byte [0x05F7], 0x00
0x8403B  jnz 0x8404A
0x8403D  mov byte [0x05F7], 0x03
0x84042  ...continues into shared bytes (0x84043 onward matches -14 exactly)
```

None of `[0x0623]`, `[0x0002]`, `[0x05FA]`, `[0x05F7]` are documented
elsewhere in this project yet - this looks like a small, previously
unseen config-selection state machine, not yet otherwise identified.
Not pursued further here since it's specific to `-13`, which this
project doesn't otherwise disassemble.

**`160-2998-14`** has a *different* thing at the exact same 67-byte
slot: the tail end of the ROM's own copyright string -
`"Copyright (C) Tektronix, Inc., 1985, 86, 87, xx. All righ"`
(cut off mid-word, no NUL terminator - confirmed via
`disasm/strings_160-2998.json` entry `offset: "0x400a"`,
`null_terminated: false`). The **complete, properly NUL-terminated**
version of the same string already exists at file offset `0x000a`
(physical `0x8000A`) in *both* revisions - already cataloged in
`STRINGS.md`. So this is a second, truncated, non-functional copy,
not new string data.

Best explanation: a link/build artifact of `-14`'s toolchain reusing
the copyright-string literal to fill the space vacated when `-13`'s
config-check code was removed or moved elsewhere, rather than a
deliberate second banner. The truncation point (cut off mid-word,
right where the original 67-byte code slot ran out) and the exact
byte-count match both support "leftover filler," not "someone wrote a
second, intentionally-short banner string here."

**Open question, not resolved**: whether real control flow in `-14`
ever falls through into this address (in which case the CPU would be
executing the string's ASCII bytes as garbage instructions) or whether
`-14`'s bytes execute a harmless fallthrough that happens to remain a
valid opcode stream and reconverges with `-13` in step. The addresses
before and after the diff window are byte-identical between revisions,
so no code change *elsewhere* in the image redirects flow around this
spot. Given the copyright text isn't a plausible thing to execute as
code, the safer reading is that this address range simply isn't
reached in practice by `-14` (the block that used to fall into it in
`-13` was presumably restructured, not just truncated) - but that
hasn't been proven.

**Disassembly-quality caveat**: this project's existing heuristic
disassembly of `160-2998-14` (built from a `push bp`/`mov bp,sp`
signature scan, not a traced call graph - see
`docs/comm-rom/address-map-and-cpu.md`) decoded these string bytes as
if they were instructions, producing several spurious labels in
`160-2998-14.symbols.json`: `L_84005`, `L_84009`, `L_8401C`, `L_84079`,
`L_8407B`, `L_84081`, `L_84085`, `L_84087`, `L_8408D`, `L_84091`,
`FUNC_2998_4095`, `FUNC_2998_409F`, `FUNC_2998_40A9`. None of these are
real functions/branch targets - they're decode artifacts of running a
generic disassembler over an embedded string literal, the same class
of problem as the `landing-artifacts-and-jump-tables.md` findings
elsewhere, just caused by data-in-code rather than by call-target
alignment. Don't chase any of these names as real routines.

## The 0x8C000 region

This one **corrects a claim** in
`docs/comm-rom/address-map-and-cpu.md` ("Both 16KB pages at file
offset `0x8000` and `0xC000` in `160-2998` open with byte `0xEA` ...
both comm-ROM pages 2 and 3 independently encode the identical jump").
That's true only for `-14`. A whole-image scan of both revisions for
`0xEA` (far-JMP) opcodes whose target segment falls in the known
main-ROM range (`0xE000-0xE800`) finds:

- **`-13`**: exactly **one** such jump, at file offset `0x8000`
  (physical `0x88000`) - `ljmp 0xE64C:0000`.
- **`-14`**: **two** - the same one at `0x8000`, plus a second at file
  offset `0xC000` (physical `0x8C000`) - identical target.

In `-13`, file offset `0xC000` isn't a boot-stub location at all - it
falls in the *middle* of an ordinary compiled function. The function's
`push bp; mov bp,sp; sub sp,8` prologue starts at file offset `0xBFCD`
(physical `0x8BFCD`), well before the page-3 boundary, and its code
runs straight across `0xC000` uninterrupted:

```
0x8BFCD  push bp
0x8BFCE  mov bp, sp
0x8BFD0  sub sp, 0x8
0x8BFD3  push cx
0x8BFD4  les di, [bp+0x26]        ; far pointer arg, [0x0732]-style
0x8BFD8  mov dx, [di+0x8A]
0x8BFDC  sar dx, 1
0x8BFDE  sar dx, 1
0x8BFE0  sar dx, 1
0x8BFE2  sar dx, 1
0x8BFE4  sar dx, 1
0x8BFE6  sar dx, 1
0x8BFE8  sar dx, 1
0x8BFEA  mov [bp-0x8], dx
0x8BFED  cmp word [bp-0x8], 0x0
0x8BFF1  jz 0x8BFFE
   ...
0x8C000  mov [bp-0x8], bx           ; page-3 boundary falls HERE, mid-function
0x8C002  jmp 0x8C007
0x8C007  mov word [bp-0x8], 0x1
0x8C00D  push word [bp-0x8]
0x8C010  les di, [bp+0x2A]
0x8C014  jmp far 0xE64C:0000        ; -14's replacement jump lands exactly here-ish, see below
```

(Instruction boundaries past `0x8C000` are approximate - not fully
re-verified against a proper recursive decode, just walked by hand
from the raw bytes to confirm the general shape. The `les di,
[bp+0x2A]` / far-jump pattern is worth a closer look later - it's
consistent with this being one of the same kind of "shift ES:DI to a
caller-supplied far pointer, then jump" trampolines seen elsewhere in
this project, e.g. `create_task`, but not confirmed.)

So: `-13`'s page 3 was **not yet given its own boot-stub jump** - that
was added later, in `-14`, patched into the exact 63-byte slot
previously occupied by the tail of this function (the function's
prologue and early body, before `0x8C000`, are untouched and identical
between revisions). Whatever `-13` did with the removed tail (the rest
of the function, including whatever it did after computing `[bp-0x8]`
and before it would have reached a far jump) isn't preserved anywhere
in `-13` past this patch point - it's just gone, replaced.

**Practical implication**: if this project's disassembly of `160-2998`
is ever extended to cover `-13` specifically (motivated by Scope 1
running that revision), the `0x88000` boot-stub jump is the only
guaranteed page-boot entry point for that revision - don't assume
page 3 mirrors page 2 in `-13`, and expect the function at
`0x8BFCD` to be a genuinely different, `-13`-only routine worth tracing
if picked up.

## Summary / what's still open

- Neither region has been chased to full resolution - the `-13`-only
  config-check chain (`[0x0623]`/`[0x0002]`/`[0x05FA]`/`[0x05F7]`) and
  the `-13`-only function at `0x8BFCD` are both genuinely new leads,
  not previously documented, but low priority since `-13` isn't this
  project's disassembled baseline.
- `-14`'s duplicated/truncated copyright string at `0x84000` most
  likely isn't executed in practice, but that's inferred, not proven.
- The spurious heuristic-layer labels listed above should eventually
  be suppressed or annotated as known-bad in `gen_disasm_2998.py` if
  anyone works in that area again - not done here since fixing the
  generator wasn't the goal of this pass.
