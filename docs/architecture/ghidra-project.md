# The `decompile/` Ghidra project

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

There's a local Ghidra project at `decompile/Tek2230.gpr` /
`decompile/Tek2230.rep/`, used as a second, independent cross-check
tool alongside this project's own custom disassembler
(`disasm/gen_disasm_x86.py`). **Not git-tracked** (`decompile/
.gitignore` excludes `*.rep/`, the actual database - only the small
`.gpr` pointer file is tracked) - it's a local artifact, same category
as `binary/`'s raw dumps or `hardware/photos/`'s originals, not
something to expect a fresh clone to have.

## The four imported programs

| Program | Represents | Ghidra block |
|---|---|---|
| `160-3633-14.bin` | Main ROM, low half | `000e:0000`-`000e:ffff` |
| `160-3532-14.bin` | Main ROM, high half | `000f:0000`-`000f:ffff` |
| `160-2998-14.bin` | Comm ROM, real 64KB device | `0008:0000`-`0008:ffff` |
| `160-2998-14-shadow.bin` | Comm ROM's `0x90000`-`0x97FFF` alias (upper half `0x88000`-`0x8FFFF` only - see `docs/comm-rom/address-map-and-cpu.md`) | `0009:0000`-`0009:7fff` |

## The address convention - a real gotcha, worth getting right the first time

The language is genuine `x86:LE:16:Real Mode`, but **the segment
values used here are not standard 8086 segment registers** - they're
simply the top hex digit(s) of this project's own physical address
convention, chosen so a block "looks like" its physical base at a
glance. Ghidra's own `getOffset()` for an address like `000e:00b1`
returns `0x191` (`0x0E*16 + 0xB1`, ordinary real-mode arithmetic) -
**not** anything resembling `0xE00B1`. That's fine and never matters
in practice, because every block uses a *fixed* segment matching its
own base, and only the low 16 bits of a real physical address are ever
used as the *offset* part - so as long as you always pair the right
segment with the right chip, addressing is fully consistent even
though the raw arithmetic doesn't "mean" what you'd first assume:

```
physical 0xE00B1 (160-3633)  -> segment 0x000E, offset 0x00B1 -> "000e:00b1"
physical 0xF0078 (160-3532)  -> segment 0x000F, offset 0x0078 -> "000f:0078"
physical 0x80060 (160-2998)  -> segment 0x0008, offset 0x0060 -> "0008:0060"
physical 0x950C2 (comm alias)-> segment 0x0009, offset 0x50C2 -> "0009:50c2"
```

**The mistake to avoid**: writing the segment as a full 4-digit value
matching the physical address's leading digits directly (e.g.
`9000:0000` for physical `0x90000`) *parses successfully* - Ghidra
accepts it as a syntactically valid address - but it's a genuinely
different, wrong location (`getOffset()` gives `0x90000`, not `0x90`),
silently landing outside every real block. This exact mistake cost a
full debugging session (2026-09-16): a block moved to `9000:0000`
looked fine in every metadata check, right up until the actual bytes
turned out to be unreadable/uninitialized there.

## Comm-ROM alias name mapping - a second real mistake to avoid

`gen_disasm_x86.FUNCTIONAL_NAMES` has comm-ROM entries in **two
different address ranges** that both represent physical silicon in
`160-2998`, but are **not interchangeable**:

- `0x80000`-`0x87FFF` (18 entries) - the comm ROM's own lower half,
  **not** reachable via the `0x90000` alias at all.
- `0x90000`-`0x97FFF` (37 entries) - already natively keyed to the
  alias address directly; apply these to the shadow program as-is,
  **no arithmetic needed**.

**The mistake to avoid**: computing `alias_addr = addr - 0x80000 +
0x90000` for the lower-half entries and applying the result to the
shadow program. This looks plausible (lands in the right numeric
range) but is wrong - the alias only mirrors the *upper* half
(`0x88000`-`0x8FFFF`), so this formula places real, meaningful names
(`putchar_serial`, `compute_response_format_flags`, etc.) at
unrelated shadow addresses. Caught and reverted this exact mistake
2026-09-16 - if re-deriving a name-export script for this project,
only ever apply comm-ROM names to the shadow program when they're
*already* keyed to a `0x90000`-`0x97FFF` address in the source dict.

## State as of 2026-09-16

- `160-2998-14-shadow.bin` was previously broken two ways: mapped at
  the wrong address (`0xF0000` instead of the confirmed `0x90000`
  alias) and its actual byte content was all zeros (never populated
  with the real upper-half comm-ROM bytes) - both fixed by deleting
  the program and reimporting the correct 32KB slice (`binary/
  160-2998-14.bin` file offset `0x8000`-`0xFFFF`) via `analyzeHeadless
  -import ... -loader BinaryLoader -loader-baseAddr "0009:0000"`.
- Auto-analysis run on `160-3633-14.bin` (had zero functions
  previously) and the rebuilt shadow program.
- All 268 confirmed names from `gen_disasm_x86.FUNCTIONAL_NAMES`
  applied across the 4 programs as real Ghidra function renames (or
  labels, where a function couldn't be auto-created), each with a
  plate comment carrying the full `FUNCTIONS.md` description and
  confidence level: 156 (`160-3633`), 37 (`160-3532`), 18 (`160-2998`),
  37 (`160-2998-14-shadow`).
- Scripts used for all of this are in `scratchpad/ghidra_scripts/`
  (gitignored, not preserved as reusable project tooling) - if this
  gets redone after more names are added to `FUNCTIONAL_NAMES`, the
  approach was: PyGhidra (`pip install <ghidra_install>/Ghidra/
  Features/PyGhidra/pypkg`, then `import pyghidra; pyghidra.start()`)
  rather than classic Jython headless scripts, since Ghidra 12.x
  dropped Jython support for `.py` scripts in favor of PyGhidra.

**Known pitfall when scripting against this project**: passing a
plain Python `bytearray()` to `Memory.getBytes(Address, byte[])` via
PyGhidra/JPype silently reads back as all zeros - it does not raise an
error, it just doesn't work. Use `jpype.JArray(jpype.JByte)(n)` for
the buffer instead, or read single bytes via `Memory.getByte(Address)`
to sanity-check. Cost significant debugging time before being traced
to this (not a persistence bug at all - the writes were fine).
