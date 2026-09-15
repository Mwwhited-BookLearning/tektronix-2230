# Deep dive into `UNKNOWN_DATA.md`'s blocks, 2026-09-15

User request: don't just export the unidentified ROM regions, actually
look at them. This is a manual follow-up pass over the 49 blocks in
`UNKNOWN_DATA.md`, prioritized by an automated pattern-scoring script
(stride-autocorrelation, entropy, monotonic-run length - see the method
below) rather than eyeballing all 49 in order. Five genuine findings
came out of it, at very different confidence levels - each is called
out honestly as "confirmed," "well-supported," or "plausible but
unconfirmed."

## Method: ranking blocks before reading them

`find_unknown_data.py`'s first-20-bytes samples aren't enough to judge
a whole block. For each of the 49 blocks, computed against the *full*
block bytes:

- **Entropy** (bits/byte) - low entropy flags padding/repetition.
- **Stride-autocorrelation** for strides 1-8 - fraction of byte pairs
  `N` apart that are equal; high scores flag a fixed-size repeating
  record.
- **Longest monotonic run** of 16-bit little-endian words, forward and
  reversed - flags counters, ramps, or sorted tables.
- Trivial `ALL-0xFF`/`ALL-0x00` flags - unused ROM space, not worth
  looking at further.

This ranking is what pointed at blocks 2-5 below; block 1 was already
under investigation from the prior conversation turn.

## 1. `160-3532` file `0x1A33`-`0x213A`: a real ~100-entry jump table, and two of its real callers land on it as a landing artifact (well-supported)

Already characterized as a probable jump table in the previous turn
(a repeating `E9 xx xx C3` - `jmp near rel16` + a padding `ret` -
4-byte record, ~100 entries, targets increasing monotonically from
file offset `0x1A3E` to past `0x2137`). This pass traced it to a real
caller and found a second instance of this project's already-known
"landing artifact" phenomenon (see "Systematic landing-artifact sweep"
below on this same page) - newly confirmed as systematic across
multiple call sites in one function, not a single one-off.

**The caller**: `FUNC_3633_E9FA` (physical `0xEE9FA`, 160-3633) is a
6-case dispatcher on the byte at `[0x340]` (values `0`-`5`), scaling
two word arguments (`[bp+6]`, `[bp+8]`) by 4 as indices into a
far-pointer array based at `[0x1C80]+0x3C`, then calling one of 4
handler routines with `(word, far_ptr, word, far_ptr)`-shaped
arguments:

```
case 0, 1 -> L_EEA40 -> lcall SUB_F1808   (0xF180:0x0008 = phys 0xF1808)
case 2, 5 -> L_EEA62 -> lcall SUB_F199E   (0xF180:0x019E = phys 0xF199E)
case 3    -> L_EEA83 -> lcall SUB_F1A34   (0xF180:0x0234 = phys 0xF1A34)
case 4    -> L_EEA9E -> lcall SUB_F1A7C   (0xF180:0x027C = phys 0xF1A7C)
default   -> no-op
```

`[0x340]` is genuinely written with all 6 values elsewhere (physical
`0xEE76E`-`0xEE7B1`, a mode-selection block that also calls the
already-confirmed `compute_acq_channel_scan_counts` right after -
placing this whole area in the acquisition-mode/channel-configuration
subsystem, consistent with the neighboring already-named
`clear_readout_attrs_for_item` at `0xEEA58`, which is a genuinely
separate, unrelated function that just happens to sit immediately
after this one in ROM - not landing-artifact garbage, despite an
earlier draft of this investigation briefly suspecting it was).

**Two of the four handlers decode as real, sensible code, with no
landing-artifact issue**:
- `SUB_F1808` (`0xF1808`): `push dx; push es; or dl,0x80; and dl,0xFE;
  les di,[0x1DDC]; mov es:[di+0x144],dl; jmp ...` - clean, coherent,
  manipulates a flag byte through an ES:DI-based far pointer.
- `SUB_F199E` (`0xF199E`): `mov di,[bp+6]; shl di,1` (x5, i.e.
  `di *= 32`); `mov dx,di; mov ax,[bp+0xA]` - clean, coherent, a
  32-byte-stride table-index computation.

**The other two land exactly 1 byte inside the jump table's own `jmp`
instruction**, decoding as nonsense if read literally from the call
target:
- `SUB_F1A34` lands on file offset `0x1A34`, the low byte of the
  jmp-table entry that actually starts at `0x1A33` (`E9 02 00 C3`).
  Read literally from `0x1A34`: `add al,[bx+si]; ret` (3 bytes) - a
  **near** `ret` immediately after a **far** call, which cannot be
  correct calling convention (a far call must be matched by `retf`,
  not `ret`, or the return address on the stack is consumed wrong).
- `SUB_F1A7C` lands on file offset `0x1A7C`, 1 byte inside the entry
  starting at `0x1A7B` (`E9 7B 00 C3`... - see the raw dump below).
  Read literally: `add word [bx+si], 0xE9C3` then `add [bx],cl` - again
  nonsense, and again a near-opcode reading of what's actually the
  jump table's own bytes.

A stack-argument accounting check across all 3 real call sites in this
function (`SUB_F199E`, `SUB_F1A34`, `SUB_F1A7C`) found a **consistent
4-byte shortfall**: the caller pushes 12-16 bytes of arguments before
each call but only cleans up 8-12 bytes afterward (`add sp, 8`/`0xA`/
`0xC` respectively) - always exactly 4 bytes less than pushed. This is
systematic, not a counting mistake, and is **not resolved** - it may
indicate a callee-pops-some-argument-bytes convention this project
hasn't modeled correctly yet, independent of the landing-artifact
question.

**What's still open**: `FUNC_3633_E9FA` itself has `ref_count: 0` in
the heuristic symbol table - it's only reachable via the compiler
push-bp signature scan, with no confirmed caller found yet. So while
the internal logic (and the landing-artifact anomaly) is real and
well-characterized, whether this function - and therefore whether
`SUB_F1A34`/`SUB_F1A7C` ever actually execute on real hardware - is
still unconfirmed. This is the same "heuristic-only, never proven
reachable" category as other items in `STILL_PENDING_DECODE.md`.

Raw bytes for reference (file offsets, `160-3532`):
```
0x1A32: C3 E9 02 00 C3 E9 04 00 C3 E9 1C 00 C3 E9 2B 00 ...
        (ret) [E9 02 00 C3] [E9 04 00 C3] [E9 1C 00 C3] [E9 2B 00 C3] ...
              ^SUB_F1A34 call lands here (0x1A34), 1 byte into the jmp
```

## 2. `160-3633` file `0x88DE`-`0x8C29`: probable per-item Y-position/width table (plausible, unconfirmed consumer)

843 bytes, cleanly decodes as 10-byte records `(0xFFFF, 0x0000,
0x0000, Y, 0x0038)` where the constant tail field `0x0038` = 56
(a plausible fixed width/height) and `Y` forms short arithmetic runs:
`66, 78, 84, 90, 96` (step +6, one +12), then a large jump to
`1268, 1264, 1256, 1260` (a different group, less clean), then more
smaller-range groups. The repeating structure and the visible groups
of evenly-spaced `Y` values is a strong tell for a **per-line or
per-item vertical position table** - plausibly related to the
readout/print-record subsystem (`init_print_region`'s own argument
shape, e.g. `(0xFA, 0x339)`/`(0x7D, 0x307)` from `PSEUDOCODE.md`'s
`print_selftest_banner`, is the same order of magnitude as the `1268`
group here).

The immediately-preceding function, `SUB_E88CB` (physical `0xE88CB`,
proven-reachable, `ref_count: 1`, called from `0xE7C1C` with exactly
one word argument and no further use of a return value), heuristically
decodes as `inc word [bx+si]` followed immediately by garbage that
cascades into this same table's own bytes being mis-read as more
instructions. This is very likely **another instance of the
"short real function immediately followed by a data table, and the
heuristic disassembler doesn't know where the function ends"**
problem - i.e., `SUB_E88CB` is probably a genuine 1-2 instruction
function that operates on this table via `[bx+si]` (consistent with
the caller passing exactly one word - plausibly a record index,
pre-scaled elsewhere), not evidence the table itself is bogus. This is
the same category as the standing `TODO.md` item "identify and mark
data regions inside already-reached code so the listing stops trying
to disassemble them" - a good concrete example to fix that item with,
if picked up.

**Not confirmed**: what exactly consumes this table, or what the `Y`
groups correspond to on screen. No literal pointer to this table's
address (`0xE88DE`) was found anywhere in the proven or heuristic
disassembly.

## 3. `160-3633` file `0xAE64`-`0xB061`: a vector icon/graphic table - includes a mathematically-confirmed circle (confirmed shape, unconfirmed purpose)

This is the strongest new finding of this pass. The block opens with
a clean linear countdown (`0x0F0F` down to `0x0000`, wrapping to
`0xFFFF` - 31 words, almost certainly an unrelated small ramp/delay
table, *not* part of the vector data below it), then a long run of
byte-pairs that looks, by eye, like several distinct closed-curve
outlines separated by odd 2-byte headers/terminators whose own
encoding wasn't determined (byte-pair values like `91 9F`, `85 9D`,
`9A 8B`, `FF FF`).

Extracted the first, largest such run as `(x,y)` point pairs and
checked it numerically:

```
40 points, bounding box x:[3,31] y:[3,31], center (17,17)
radius: min 13.42, max 14.14, average 13.77
```

That's a circle - 40 points sampled around a radius-~14 circle
centered at `(17,17)`, with all radii within about ±0.4 of the mean
(exactly the variation you'd expect from integer-rounded pixel
coordinates on a real circle, not coincidence). A second, smaller run
came back as a **perfect circle** (radius exactly 3.16 = √10,
constant, zero variation, 8 points) centered at `(17,17)` - the same
center as the big circle, suggesting a filled/ringed dot at the center
of the larger circle, or two views of the same icon at different
sizes. A third run turned out to be degenerate - not a curve at all,
but a straight vertical line (`x` constant at 17, `y` from 17 to 42) -
plausibly a radial "needle"/pointer line, though its coordinate range
extends past the big circle's own bounding box, so it may belong to a
different icon rather than being that circle's needle.

Several smaller runs (radius 2.5-4.7, less cleanly circular) look like
smaller marks, arcs, or tick-like details - their exact boundaries in
the byte stream were eyeballed, not derived from a confirmed
delimiter encoding, so treat those specifically as a rough first pass,
not a settled decode.

**This is very likely a small library of UI vector icons** (dial/knob
position indicators, a cursor/crosshair marker, or similar CRT
graphics used somewhere in the menu or calibration display) - drawn
with the same kind of pen/vector mechanism as `draw_readout_char`
(see `docs/display/vector-display-and-stroke-font.md`), but
**distinct from the stroke font itself**: these are smooth curves
(circles), not letterforms, and this address range was never one of
the stroke-font search's own candidates.

**Not confirmed**: the exact shape-delimiter encoding, how many
distinct icons are really in this table, what draws them, or what
they're used for on screen. Worth a proper re-pass with an actual
delimiter hypothesis (the 2-byte headers are a good next lead) rather
than eyeballed splits.

## 4. `160-3532` file `0xBEE6`-`0xC263`: small-int + near-top-of-ROM-address pairs (plausible, unconfirmed)

894 bytes of clean 4-byte records: `(small count, 16-bit value)`,
where the 16-bit value is consistently in the `0xFC00-0xFC70` range -
read as a plain unsigned offset within the `160-3532` chip's own
64KB segment (base `0xF0000`), that's physical `0xFFC00-0xFFC70`,
**about 1KB before the CPU reset vector** (`0xFFFF0`, confirmed
elsewhere in `JUMP_MAP.md`). That's a distinctive, plausible location
for a table of entry points into the same final-ROM-page region that
already holds the early interrupt-vector installs (`INT1`/`INT2`/
`INT255` handlers, per `JUMP_MAP.md`'s Level-0 boot diagram). The
paired small "count" values (`3, 3, 0, 3, 9, 4, 4, 8, 6, 4, 1, 0, ...`)
don't obviously match a known enumeration yet.

**Not confirmed**: no literal reference to this table's own address
was found, and the small-count field's meaning is unexplored. Flagging
this primarily because the address range is suggestive, not because
the mechanism is understood.

## 5. `160-2998` (comm ROM) file `0x80EC`-`0x8211`: a grouped, incrementing-ID record table (plausible, tentatively comm-error/status related)

293 bytes. After an initial descending-by-4 byte ramp (`0x1C, 0x18,
0x14, 0x10, 0x0C, 0x08, 0x04, 0x04` - the same *shape* as finding 3's
countdown table, though there's no evidence the two are related) and
a short 4-byte header (`01 41 91 01`), the rest is a clean run of
4-byte records `(group, subgroup, id_lo, id_hi)` where `(id_lo,
id_hi)` forms a monotonically increasing little-endian 16-bit value
and `group` takes only a handful of distinct values (`2, 3, 4, 5, 8`)
across the whole table:

```
02 61 65 00   02 61 66 00   ...   02 61 6D 00   (group 2, ids 0x0065-0x006D)
02 61 97 00   ...           02 61 9D 00         (group 2, ids 0x0097-0x009D)
02 62 C9 00   ...           02 62 CE 00         (group 2, ids 0x00C9-0x00CE)
02 62 FB 00   ...           02 62 09 01         (group 2, ids 0x00FB-0x0109, crossing the byte boundary)
03 63 5F 01   03 62 60 01   03 62 61 01         (group 3, ids 0x015F-0x0161)
04 61 C3 01 ... 04 65 CC 01                     (group 4, ids 0x01C3-0x01CC)
05 65 27 02 ... 05 65 31 02                     (group 5, ids 0x0227-0x0231)
08 C1 EF 02 ... 08 C5 F3 02                     (group 8, ids 0x02EF-0x02F3)
```

The `group` field's small handful of values, in the comm ROM,
immediately suggested the manual's Table 7-34 status/event
categories (`docs/comm-rom/rs232-live-session-2026-09-14.md`'s sibling
doc `hardware/manuals/2230_programming/README.md` "Table 7-34: Status
Event and Error Categories" - 7 categories: Command Error, Execution
Error, Internal Error, Power On, Operation Complete, Execution
Warning, No Status). **The match isn't clean enough to claim** - this
table has only 5 distinct group values (`2,3,4,5,8`) against the
manual's 7 categories, and no direct numeric correspondence was found.
Recording this as a plausible hypothesis worth a second look (e.g.
against the specific numeric status-byte values `33/49/97/113` etc.
from the same table), not a confirmed identification.

**Not confirmed**: what the incrementing ID actually indexes (a
string table offset? an internal error-code list? something else
entirely), or what reads this table.

## Summary table

| # | Chip | File offset | Confidence | What |
|---|---|---|---|---|
| 1 | 3532 | `0x1A33-0x213A` | Well-supported | Real ~100-entry jump table; 2 of 4 real callers land 1 byte into it (landing artifact); enclosing function not proven-reachable |
| 2 | 3633 | `0x88DE-0x8C29` | Plausible | Per-item Y-position/width record table; consumer (`SUB_E88CB`) suspected but not confirmed |
| 3 | 3633 | `0xAE64-0xB061` | **Confirmed shape**, unconfirmed purpose | Vector icon table; contains a mathematically-verified circle (r≈14) and a perfect small circle (r=3.16); likely a dial/cursor/icon library, not the stroke font |
| 4 | 3532 | `0xBEE6-0xC263` | Plausible | Small-int + near-reset-vector-address record table; suggestive location, mechanism unconfirmed |
| 5 | 2998 | `0x80EC-0x8211` | Plausible | Grouped incrementing-ID record table; tentatively status/error-category-related, not a clean match yet |

None of the addresses discussed here were renamed in
`gen_disasm_x86.FUNCTIONAL_NAMES`/`FUNCTIONS.md` - none reached this
project's "mechanism confirmed" bar for a rename. See
`STILL_PENDING_DECODE.md` for these as tracked open items.
