# Rendered vector shapes, `160-3633` physical `0xAE64`-`0xB061`

Committed renders from `disasm/decode_vector_icons.py`, generated
2026-09-15 while investigating the candidate vector shape table found
in `UNKNOWN_DATA.md` (see
`docs/decode-anomalies/unknown-data-deep-dive-2026-09-15.md` finding 3
for the full analysis, the encoding write-up, and why this is
**not** a confirmed icon/font identification - just a rendering of
the best current decode, committed so the shapes stay inspectable
without re-running the tool).

## Files

- `flip-y/` - rendered with the Y-axis flipped (this tool's default -
  matches `decode_stroke_font.py`'s own convention for the confirmed
  stroke font, unconfirmed for this table specifically)
- `no-flip-y/` - the same shapes with the Y-axis *not* flipped, for
  side-by-side comparison - neither orientation makes every shape read
  as an unambiguous, consistent letterform, which is exactly why the
  identity question in the linked finding is still open

Each directory has:
- `overlay.svg` - all 14 shapes drawn together at their own raw
  coordinates, color-coded, with a legend - the one to check first for
  whether shapes visually compose one coherent icon
- `catalog.svg` - each shape in its own labeled grid cell (shape
  index, byte offset, point count, whether it closes back to its own
  start)
- `shape_00.svg` through `shape_13.svg` - each shape standalone

## Regenerating

```
cd disasm
python decode_vector_icons.py --out-dir ../docs/display/vector-icons/flip-y
python decode_vector_icons.py --no-flip-y --out-dir ../docs/display/vector-icons/no-flip-y
```

Point `--chip`/`--start`/`--end` at a different candidate region to
render something else - the tool doesn't assume this specific address
range is correct, it just decodes whatever bytes you give it under the
pen-bit convention described in its module docstring.

## Reachability: searched for a caller, found none

Before committing these, searched all three ROMs for any genuine
`lcall`/`ljmp` instruction (the only two 8086 opcodes that embed a
literal far pointer as an instruction operand) targeting this address
range, by parsing the actual disassembly listings' own resolved
targets rather than a raw byte scan. **Zero hits** in either the
proven or heuristic layers, across all three chips. (An earlier raw
byte-level scan of the same range appeared to find one hit - a
`lcall EB00:0007` in `select_next_ready_task` - but that turned out to
be a false positive: byte `0x9A`, the far-call opcode, coincidentally
appears as *data* inside two unrelated adjacent instructions there,
not as a real call. Real instruction boundaries, not raw bytes, are
required for this kind of search - noted here so the same false
positive doesn't get rediscovered.) So this table's reachability from
already-disassembled code is still unconfirmed, the same open status
as several other findings in `STILL_PENDING_DECODE.md`.

## A tempting menu-text connection that a real photo disproved

The table is immediately followed in ROM by genuine, already-cataloged
UI text (`"SAVE REF"`, `"Cursor moves box, SEL for choice"`,
`"S/Div & Trig select col"` - the `SELECT_MODE` timebase/trigger
matrix screen). That's specific enough to identify the exact photo:
`hardware/photos/20260911_005751019_iOS.jpg` shows this screen live,
including the actual "box" the help text refers to - a plain
rectangular outline highlighting one cell of the matrix. That's a
trivial 4-corner shape computed on the fly, not something that needs
pre-stored vector data, and it looks nothing like this table's
circles/ovals/letter-like shapes. The address proximity is very
likely coincidental (or at most a same-source-file linker artifact,
not "used together at runtime") - checked and ruled out so it doesn't
get re-chased.

## Searched for a sibling table elsewhere - found none

14 shapes is too few for a real character set. Added
`scan_chip_for_shape_clusters` to the tool and ran it across all three
ROMs looking for another region with a comparable density of closed,
plausibly-sized pen-bit shapes. Every high-scoring window found was
just a different overlapping slice of this same 509-byte island -
no separate cluster exists anywhere else in any of the three chips
under this specific encoding. Doesn't rule out the already-confirmed,
separately-encoded stroke font at `[0x1DB0]` existing somewhere this
scan wouldn't detect (different, more compact byte format) - just
means this particular table is a one-off under its own encoding, not
part of a bigger family findable the same way.
