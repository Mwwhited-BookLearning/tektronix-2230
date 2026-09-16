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

## Applying `PARAMETER_NAMES` as real stack parameters, 2026-09-16

Once `disasm/gen_disasm_x86.PARAMETER_NAMES` (see `CLAUDE.md`'s
Conventions section) had 72 confirmed functions in it, applied them
all to Ghidra as real stack-based `Parameter` objects (not just plate
comments) so the decompiler shows them directly in every call site,
not only at the function's own definition.

**The API, calibrated on one function first**: `ParameterImpl(name,
dataType, stackOffset, program)` takes this project's own `bp+N`
offset convention **directly** - no translation needed, despite
Ghidra's own *display* convention showing a different number (its
`getVariableStorage()` prints `Stack[0x4]` for what this project calls
`bp+6` - a fixed 2-byte difference, because Ghidra's stack-offset
numbering doesn't count the saved `BP` register the way `bp+N` does).
Confirmed by creating a real parameter and reading it back before
trusting the pattern for a 72-function batch.

**Byte vs. word size matters and is easy to get wrong via unscoped
search** - found and fixed 2 real bugs in the size-inference step
before trusting the results:
1. A regex search for the `byte`/`word` prefix on a given `[bp+N]`
   text pattern, run against a whole chip's `_readable.asm` text
   rather than scoped to the one function's own body, picks up
   whichever function's matching text happens to appear *first* in the
   file - completely unrelated to the function actually being sized.
   Fixed by first slicing out just that function's own line range
   (from its label to the next *non-internal* label - a bug on the
   first attempt too, since this project's internal branch labels
   `L_XXXXX:` look exactly like a function boundary unless explicitly
   excluded).
2. `array_index_16`'s own entry was itself wrong (pre-existing, not
   introduced by the size-fix pass) - `retf 4` (only 4 bytes of
   arguments) proves it takes 2 plain words, not `(base far ptr,
   index)` as `FUNCTIONS.md` said; the "index" argument doing the
   `shl` by 4 is actually at `bp+8`, not `bp+0xa` as a 3rd offset.
   Fixed in `gen_disasm_x86.py`/`FUNCTIONS.md` and reapplied.

Applying and removing an existing (auto-analysis-guessed) parameter
list cleanly: `func.removeParameter(0)` in a loop while
`func.getParameterCount() > 0`, then `func.addParameter(Parameter,
SourceType.USER_DEFINED)` per confirmed offset in increasing order.
One transaction per program (`program.startTransaction()`/
`endTransaction()`), saved with **`project.save(program)`** - calling
`program.save(comment, monitor)` directly instead raises `Unable to
lock due to active transaction` even after the transaction is properly
closed; use the `GhidraProject` method, not the `Program` one.

## Decompiled-C export: a real, fast way to cross-check parameter work

`decompile/export_decompiled_c.py` dumps every function's decompiled C
in a program to a single text file under `decompile/exports/` -
**~1-2 seconds per 200-400-function program**, cheap enough to
regenerate after any batch of edits. This answers a question worth
recording for next time: **yes, Ghidra's decompiler can meaningfully
help the hand-written disassembly/parameter-naming work**, in three
concrete ways demonstrated 2026-09-16:

1. **Live decompilation sometimes recovers parameters the cached
   auto-analysis missed entirely.** `draw_readout_char`'s cached
   `Function.getSignature()` showed `(void)` - zero parameters - but
   asking the decompiler to actually decompile it live showed 1 byte
   parameter at `Stack[0x4]`, matching this project's own independent
   finding (`char`) exactly. The cached signature and a fresh
   `DecompInterface.decompileFunction()` call can disagree; the live
   one is more reliable.
2. **It's a genuine independent cross-check for byte/word sizing**,
   not just naming - `set_item_active_flag(word item_index, word
   set_flag)`'s cached signature had already (correctly, independent
   of this project) inferred 2 stack parameters before any manual
   naming pass touched it, confirming the parameter *count* the manual
   read had found; conversely, seeing the exported C helps catch
   Ghidra's own size mistakes when a program's calling convention
   isn't set correctly (the comm ROM's `160-2998-14.bin` shows
   `calling convention: unknown`, not `__stdcall16far` like the main
   ROM programs - not yet fixed, a real next step to improve accuracy
   there specifically).
3. **It surfaces which named functions are "real" Ghidra Functions
   vs. label-only fallbacks the decompiler can't resolve as callees.**
   Cross-checking every `FUNCTIONAL_NAMES` address against its own
   decompiled-C definition found 19 candidates that looked
   label-only from a first, callee-site-based text scan; running
   `CreateFunctionCmd` directly against each showed **14 were already
   fine** (the callee-site text scan has false positives - an indirect/
   computed call site can show a raw address fallback even when the
   target has a real name, since Ghidra can't always resolve an
   indirect call to a specific function), but the remaining **5
   genuinely fail `CreateFunctionCmd`**: `write_hw_shift_register`,
   `handle_acq_mode_change`, `compute_and_draw_scale_marker`,
   `clear_readout_attrs_for_item`, `start_plot_output_task`. **This is
   real, convergent evidence** with this project's own static-analysis
   findings - `handle_acq_mode_change` and `compute_and_draw_scale_
   marker` were *already* independently flagged this same day (see
   `changes/2026-09-16.md`/the parameter-naming batch notes) as
   landing on garbage/anomalous decode at their exact label address
   (a `push cs; cmp al,byte[di]...` nonsense opening and an `add
   si,ax` mid-instruction landing, respectively) - Ghidra's own
   function-boundary heuristics independently refuse to recognize
   these same 5 addresses as valid instruction boundaries. Worth
   resolving via this project's own dual-entry-point process
   (`docs/decode-anomalies/dual-entry-points.md`) before trying to
   force a Ghidra function there.

**Limits worth remembering**: the decompiler's inferred types/counts
are a *hypothesis* to cross-check against, not ground truth - it can
also be wrong (the `unknown`-calling-convention comm ROM case above).
**Open, unexplained quirk found while writing this section**:
`format_hex_word`'s decompiled body shows `func_0x000e327f();` (an
opaque, unnamed call) even though `format_number` - confirmed to be
the exact same physical address (`0xe31d:0xaf` resolves via ordinary
8086 real-mode arithmetic to `0xE327F`, the well-established segment-
aliasing pattern documented throughout this project) - is a fully
named, fully parameterized real Ghidra function (`format_number(word
value, word radix, word width, byte overflow_flag, word extra)`,
verified directly). Re-exporting after confirming this didn't change
the result. The `func_0x000e327f` label itself proves Ghidra resolved
the *address* correctly; something about how the far-call operand
(`0xe31d:0xaf`, a different but numerically-equal segment:offset pair
from the callee's own "home" segment `000e`) gets resolved during
decompilation isn't picking up the named function there. Not yet root-
caused - worth investigating before leaning on cross-chip/cross-alias
far calls specifically for decompiler-based cross-checks; direct calls
using a function's own home segment (the vast majority) don't show
this problem. Regenerate `decompile/exports/*.c` after any batch of
parameter or function-name changes to keep this cross-check current.

## Bookmarks: flagging "needs deep analysis" spots for later

`decompile/apply_bookmarks.py` writes Ghidra Bookmarks (`Window >
Bookmarks` in the GUI; also shown as gutter markers) at addresses this
project has flagged as unresolved but worth returning to, from a
single git-tracked `BOOKMARKS` list in the script itself - the same
"text is the source of truth, the script is the recipe" pattern as
`apply_names.py`/`apply_parameters.py`. Currently covers the 5
addresses that fail `CreateFunctionCmd` (category "Decode anomaly" -
see the previous section) and the candidate vector shape table
(category "Unresolved data", `0xEAE64`). Add an entry any time this
project finds something worth flagging but not yet resolved, rather
than only noting it in a doc - a bookmark surfaces immediately when
someone opens the project, a doc has to be remembered and searched.

## Rebuilding this project from scratch

Because `decompile/Tek2230.rep/` isn't (and shouldn't be) the thing
that gets backed up or carried to a new machine, here's the full
recipe to reconstruct the current analysis state from what *is*
git-tracked - the 3 ROM binaries plus `disasm/gen_disasm_x86.py`,
`FUNCTIONS.md`, and the 3 scripts in this folder:

1. Install Ghidra 12.x and PyGhidra (`pip install <ghidra_install>/
   Ghidra/Features/PyGhidra/pypkg`).
2. Create a new Ghidra project named `Tek2230` at `decompile/`.
3. Import 4 programs via `analyzeHeadless ... -import ... -loader
   BinaryLoader -loader-baseAddr "<segment>:0000"`, one per row of
   the "The four imported programs" table above - `160-3633-14.bin`
   and `160-3532-14.bin` whole, `160-2998-14.bin` whole (as
   `160-2998-14.bin`), and a second import of the **same** file's
   upper 32KB (file offset `0x8000`-`0xFFFF`, saved as a separate
   source file first) as `160-2998-14-shadow.bin` at base `0009:0000`.
   Run default auto-analysis on each.
4. `python decompile/apply_names.py` - applies every confirmed
   function name + description/confidence plate comment.
5. `python decompile/apply_parameters.py` - applies every confirmed
   function's stack parameters with correct byte/word sizes.
6. `python decompile/apply_bookmarks.py` - flags the known unresolved
   spots.
7. (optional) `python decompile/export_decompiled_c.py <program>
   decompile/exports/<program-without-.bin>.c` for each of the 4
   programs, to regenerate the decompiled-C cross-check artifacts.

Steps 4-7 are all idempotent (safe to rerun after any change to their
git-tracked source) and each takes a few seconds.
