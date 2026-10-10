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

## 3. `160-3633` file `0xAE64`-`0xB061`: a vector shape table - encoding fully decoded and rendered; whether it's icons or a rough font is genuinely unresolved

The encoding is now fully decoded and a real rendering tool exists
(`disasm/decode_vector_icons.py`) - but rendering it and actually
looking at the output (rather than just the numeric radius/bbox
checks from the previous pass) **walked back the "rotary dial" story
from a confident conclusion to one of two open possibilities**. This
is exactly the kind of thing that only shows up once you look at the
picture, not the numbers - recorded here in full including the
reasoning that changed, not just the final state.

**The encoding** (unchanged, still solid): each closed shape is a
2-byte "pen-up move to `(x,y)`" point, both bytes with bit 7 set
(`x|0x80, y|0x80`), followed by N 2-byte "pen-down line to `(x,y)`"
points with bit 7 clear - the same pen-bit convention already
confirmed for `draw_readout_char`'s stroke font
(`docs/display/vector-display-and-stroke-font.md`), just with a wider
2-byte-per-point coordinate (7 usable bits each) instead of the font's
packed byte. Verified: for every genuinely closed shape, its preceding
"moveto" header lands exactly on one of its own traced points, at
distance 0.0. **Also checked and ruled out**: reinterpreting the same
bytes as 16-bit word pairs instead of independent 8-bit x/y bytes
(prompted by noticing the HPGL plot output uses 1023-count, i.e.
10-bit, resolution) - that reading turns the clean circle into scatter
(radius spread jumps from 0.7 units to 6872 units), so 8-bit-per-axis
is correct for *this* table. The HPGL plotter's 10-bit resolution is a
different, later-stage output path (the rear-panel X-Y plotter DAC),
not evidence this on-screen table should be reread as 16-bit.

**14 shapes found** (`disasm/decode_vector_icons.py`'s default run
against this region - see `scratchpad/vector_icons/` for the
rendered SVGs and quicklook PNGs this session produced, gitignored
scratch output, regenerate with the tool if needed):

- Shape 0 (31 pts, no header) - the countdown ramp prefix, kept but
  clearly not vector data (renders as a meaningless "L" shape).
- **Shapes 1 and 9**: the same 40-point circle (radius≈14), appearing
  twice as an exact cyclic rotation of the same point list.
- **Shapes 2 and 11**: a second, larger, less-perfectly-circular closed
  shape (30-31 pts, radius 14.4-20.9), also appearing twice, close but
  not byte-identical.
- Shape 3: a straight vertical line, ~28 units long.
- Shape 4: an 8-point closed octagon, radius exactly `√10` (3.16),
  centered at the same point as the big circles (17,17).
- Shapes 5, 6, 7: three medium, open (non-closing) curved/angular
  shapes, similar in size to each other.
- Shapes 8, 10, 12: short 1-2 point tick marks/line segments.
- Shape 13 (9 pts, no header after the `0xFF 0xFF` end marker) - this
  is the tail junk this project already separately identified as an
  unrelated small index/lookup table, not vector data - correctly
  excluded from the drawing by treating `0xFF 0xFF` as "no header"
  rather than a bogus `(127,127)` coordinate (an earlier version of
  the tool got this wrong before the fix).

**What actually happened on rendering**: at each shape's own scale
(`catalog_quicklook.png`), shapes 5/6/7 look like a "C", a "U", and a
"P" - three plausible letterforms. Re-rendered at a single **uniform**
scale across every shape (not auto-fit per cell, which can make a
2-point tick look as big as a 40-point circle) - at uniform scale, the
"C"/"U"/"P"-like shapes are consistently similarly sized to each other
(and much smaller than the two big circles), which is what you'd
expect from real font glyphs, not from arbitrary icon fragments.
**But** re-rendering the same shapes with the Y-axis flip disabled
(the flip was an unconfirmed assumption, borrowed from
`decode_stroke_font.py`'s own convention, not independently verified
for this table) turns "U" into something closer to "n", and "P" into
something closer to "b"/"6" - plausible letters either way, but
different ones, and neither orientation makes *all three* shapes read
unambiguously as consistent, confident letterforms at once.

**Current honest state**: this is either (a) a small set of UI icons
(the two big circles sharing a center plus a needle-length line is
still consistent with a dial/knob indicator, and shapes 5-7 could be
unrelated smaller symbols), or (b) a rough, low-resolution vector font
distinct from the already-confirmed compact stroke font at `[0x1DB0]`
- and the rendered evidence doesn't cleanly settle which. **Not a
confident conclusion either way** - this walks back the previous
draft's "very likely a rotary dial/knob indicator" framing, which was
based on the numeric circle-fit alone and didn't hold up as well once
actually rendered and looked at.

This is still **distinct from the already-confirmed stroke font**:
different coordinate encoding (2 bytes/axis vs. a packed nibble),
different address range, and this address range was never one of the
stroke-font search's own candidates - whatever it turns out to be, it
isn't the same table `draw_readout_char` reads from `[0x1DB0]`.

**Not confirmed**: which hypothesis (icons vs. font) is right, the
correct Y-axis orientation, or what the smaller open-arc shapes (5, 6,
7) represent either way.

**Searched for a bigger, sibling table elsewhere - found nothing.**
14 shapes is too few for a real character set (no room for a full
alphabet), so if this were a font there should be more of it
somewhere. Added `scan_chip_for_shape_clusters` to
`decode_vector_icons.py` (slides a window across a whole chip,
scores by how many genuinely-closed, plausibly-sized shapes it
contains) and ran it across all three ROMs. Result: every window that
scored highly is just a different overlapping slice of this *same*
509-byte island (`0xEA7F8`-`0xEADD4`, all within ~2KB of each other,
all really just showing this one table's own shapes) - **no
comparable cluster exists anywhere else**, in any of the three chips,
under this specific pen-bit-2-byte encoding. This doesn't rule out a
"real" character ROM existing under a *different* encoding (in
particular the already-confirmed compact 1-byte stroke font at
`[0x1DB0]`, whose table location this project has separately searched
for at length without success - see `docs/display/vector-display-
and-stroke-font.md`) - it specifically means this encoding, this
table, is a one-off, not part of a larger family this same technique
would find.

**Checked for a caller, found none**: searched all three ROMs' actual
disassembly listings (not a raw byte scan) for any `lcall`/`ljmp`
whose resolved target lands in this address range - zero hits in
either the proven or heuristic layers. A raw byte-level scan first
appeared to find one (`select_next_ready_task` doing an `lcall
EB00:0007`, landing 1 byte inside shape 11's own header - which would
have been a great "another landing artifact, and this one's call-
verified" story), but that turned out to be a false positive: opcode
`0x9A` coincidentally shows up as *data* inside two unrelated,
adjacent instructions there (`mov word [0x79A], 0` immediately
followed by `jmp short`), not as a real far call. Worth recording
precisely so this specific false lead doesn't get rediscovered and
re-chased. This table's reachability from already-disassembled code
remains genuinely unconfirmed - full detail and the rendered SVGs in
`docs/display/vector-icons/`.

**Checked the tempting menu-text proximity lead, and it doesn't hold
up.** The 209-byte gap immediately after this table (file `0xB062`-
`0xB132`, before the next real function) is genuine, already-cataloged
UI text: `"SAVE REF"`, `"Cursor moves box, SEL for choice"`,
`"S/Div & Trig select col"`, plus the `SELECT_MODE` timebase/trigger
matrix's row and column labels (`STRINGS.md` "Timebase/range labels
and misc UI"). That's specific enough to identify the exact physical
screen: `hardware/photos/20260911_005751019_iOS.jpg` (`ACQ_MODE_SETUP_
TABLE/SELECT_MODE`) shows it directly, "Cursor moves box, SEL for
choice" printed at the bottom **and the actual box visible on screen**
- a plain rectangular outline highlighting one cell of the UN-TRIG/TRIG
× sweep-speed matrix. That's a simple 4-corner rectangle, trivially
computed from a known cell width/height with the same point-plotting
primitives already confirmed elsewhere (`plot_readout_point` et al.) -
it doesn't need pre-stored vector shape data at all, and doesn't
resemble this table's circles/ovals/letter-like shapes in any way.
**The address proximity is very likely coincidental** (or at most
"same compiled source file's static data section," not "used
together at runtime") - this specific menu is not the table's
consumer. Recorded here so a future session doesn't re-chase this
exact connection; the real consumer, if any, is still unfound.

**Follow-up, 2026-10-09: rendered all 14 shapes at both Y-orientations
(previously only shapes 5-7 had been checked both ways) - no new
letterform candidates turned up.** Rasterized `decode_vector_icons.py`'s
`catalog.svg` output (both with the tool's default Y-flip and with
`--no-flip-y`) via headless Edge
(`msedge --headless=new --screenshot=... --window-size=2400,1400
file:///...svg` - no `cairosvg`/`rsvg-convert`/Inkscape/ImageMagick
available in this environment, but `msedge.exe` is) and inspected both
renders directly. Result, shape by shape:

- Shapes 1/9 (the 40-pt circle) and shape 4 (the octagon) are
  symmetric enough that the flip is visually indistinguishable - still
  just a circle/octagon either way.
- Shape 3 (the line) is unaffected by the flip, as expected.
- Shape 0 (already known non-vector-data, the countdown-ramp prefix)
  renders as a simple right-angle corner either way (corner-at-top
  "Γ"-like vs. corner-at-bottom "L"-like) - doesn't read as a
  deliberate letterform in either orientation, consistent with its
  already-documented "meaningless" status.
- Shapes 2/11 (the two large open arcs) stay irregular open
  crescent/arc shapes in both orientations, never resolving into a
  clean letterform - still most consistent with the dial/knob-icon
  reading (their bounding boxes roughly share the same canvas/center
  as the two big circles) rather than a font glyph.
- Shapes 8/10/12 (1-2 point tick marks) are too short to read as
  letters either way.
- Shape 13 (already-identified unrelated tail index-table junk) stays
  a meaningless zigzag in both orientations.

Shapes 5/6/7 remain the only letter-like shapes, and the original
finding holds unchanged: shape 5 reads as "C" in *both* orientations
(open shape with enough symmetry that the flip barely matters); shape
6 flips "U" (Y-flip on) / "n" (Y-flip off); shape 7 flips "P" (Y-flip
on) / "b" (Y-flip off) - no single orientation makes all three read as
one consistent alphabet.

**This was a real test of the font hypothesis, and it came back
negative**: if several of the other 11 shapes had also turned into
clean letterforms under one consistent orientation, that would have
been real evidence for "rough font." None did - the font-vs-icon
question and the correct-orientation question are exactly as open as
before, but now backed by having actually looked at all 14 shapes both
ways rather than extrapolating from 3 of them.

## 4. `160-3532` file `0xBEE6`-`0xC263`: only the first ~44 bytes are near a known string block; the other ~850 bytes are unrelated (revised, partially resolved)

894 bytes of clean 4-byte records: `(small count, 16-bit value)`. A
follow-up pass checked the 16-bit value against every already-
cataloged string offset in `strings_160-3532.json` and found something
real, but weaker than first thought:

**Only the first 11 records** (44 bytes) have their 16-bit value
landing within a few bytes of an *already-documented, already-named*
diagnostic string cluster - the self-test messages near the end of
`160-3532` (`STRINGS.md` lines ~150-155/180: `" comm_stat u1x23"`,
`" comm_param u1x22"`, `"ROM/RAM/NMI :"`, `"2230/2220 Power up tests
complete."`, `"line stuck high"` - the same strings this project's
`JUMP_MAP.md` already ties to `selftest_display_irq_idle`/`_active`).
The offsets are close (`-19` to `+14` bytes from the nearest string's
own start) but **not an exact, uniform pointer match** - there's no
single constant that lines every one of the 11 up exactly, so this
isn't simply "pointer + fixed bias." It's real proximity to a real,
already-identified string cluster, not coincidence (11 out of 11 land
within a 34-byte spread that's otherwise a small fraction of the
chip), but the exact addressing scheme (mid-string references on
purpose? a slightly different field/byte alignment than assumed?)
isn't nailed down.

**The remaining ~850 bytes (records 12 onward) don't correlate with
any known string at all** - their values are small and cluster near
the very start of the chip (`0x0000-0x0705`) instead, which is a
completely different neighborhood. This means the original "894-byte
table" framing was wrong: at least two different, unrelated data
structures were lumped into one block by `find_unknown_data.py`'s
gap-merging, and only the first fragment has a real, characterized
lead.

**Not confirmed**: the exact addressing/field scheme for the first 11
records, or what the small "count" field (`3, 3, 0, 3, 9, 4, 4, 8, 6,
4, 1`) means. The rest of the block (records 12+) is back to fully
unknown.

**Methodological note**: while checking the neighboring ROM for this,
found a separate, unrelated example of this project's heuristic
disassembler producing an *impossible* decode - `160-3633` physical
`0xEAC90`-`0xEADA0` heuristically "decodes" as including a `minps
xmm4,xmm4` (an SSE instruction, which cannot exist on an 8088) among
other clearly-bogus reads. That region is **not** flagged in
`UNKNOWN_DATA.md` at all, because the heuristic scanner did "cover"
those bytes - just with garbage. `find_unknown_data.py` only finds
*uncovered* gaps; it has no way to flag *wrongly-covered* ones. A
useful follow-up tool: scan already-"covered" heuristic regions for
decode oddities (opcodes/instructions that don't exist on the 8088,
implausibly long runs of the same ADD-immediate shape from what's
probably all-zero data, etc.) to find more hidden data regions this
export missed entirely. Checked whether *that* specific region was
more of the vector-icon data from finding 3 (given how close it sits
to finding 3's own table) - it isn't: its point-pairs have bounding
boxes spanning the full 0-255 byte range, nothing like a coordinate
system, so it's some other kind of data, not more icon shapes.

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

## 6. `160-2998` file `0x0824C-0x08A57`: a second, structurally-different pointer table immediately adjacent to the confirmed command-keyword-table cluster (well-supported, not fully decoded)

Found this while chasing the SELECT C1/C2 investigation (see
`TODO.md`), checking whether this span - the largest contiguous
unreached comm-ROM byte range - held the missing output-suppression
mechanism. It doesn't (no `[0x758]`/`[0x1B48]` byte pattern anywhere in
it, and see below for what it actually is), but the span itself is a
real finding worth recording on its own.

This 2060-byte block ends at file offset `0x8A57` - **exactly one byte
before** the already-confirmed command-keyword-table cluster begins at
`0x8A58` (`docs/comm-rom/command-keyword-table.md`,
`disasm/decode_comm_keyword_table.py`). That boundary is unlikely to be
coincidental.

It is NOT the same structure as the confirmed dispatch table there
(6-byte records, marker byte `0xFF`, a fixed segment `0xDE90`). Fitting
different fixed-width record models against every 2-byte field at every
stride/phase, the best fit is 4 bytes per record, phase 0 - `[offset
u16][segment u16]`, little-endian - where **77% of records** decode to
a segment plausible for this ROM's own address space (`0x8000-0x97FF`,
the comm ROM's confirmed range and its `0x90000` alias, or `0xE000-
0xFFFF`, the main ROM's), well above what 515 random 4-byte windows
would produce by chance. Stronger: of those plausible-segment records,
**41% resolve (segment*16 + offset) to the exact physical address of
an already-identified function entry point** (mostly the comm ROM's
own heuristic push-bp-prologue matches, `disasm/gen_disasm_2998.py`) -
hitting a sparse set of known label starts that often, by pure chance,
would not happen anywhere near this rate.

**Working hypothesis, not confirmed**: a second table in the same
command-table cluster, this one resolving a command ID (or similar) to
an executable *handler* address, complementary to the confirmed
table's id-to-*display-string* mapping. Not proven: the record
boundaries aren't perfectly clean (77%, not 100%, hence not a fully
solved fixed-width model), so the true record width/stride and any
interleaved non-pointer fields (small integers, flag bytes - the
visual byte dump shows short runs that don't fit the 4-byte pointer
pattern) remain unresolved. No caller referencing this table's base
address was found in already-reached code, so what indexes it (and
whether it's even live vs. an orphaned/superseded table) is also open.

**Relevance to SELECT C1/C2**: none found. Every plausible target this
table's entries resolve to is comm-ROM/GPIB-RS232 command-handling
code (or main-ROM code reachable via a normal far call) - nothing
resembling front-panel switch state, which the comm ROM board has no
reason to read directly (that's `[0x758]`, read and gated entirely in
the main ROM). This closes off the "unread comm-ROM span" lead from
`TODO.md`'s SELECT C1/C2 entry.

## Summary table

| # | Chip | File offset | Confidence | What |
|---|---|---|---|---|
| 1 | 3532 | `0x1A33-0x213A` | Well-supported | Real ~100-entry jump table; 2 of 4 real callers land 1 byte into it (landing artifact); enclosing function not proven-reachable |
| 2 | 3633 | `0x88DE-0x8C29` | Plausible | Per-item Y-position/width record table; consumer (`SUB_E88CB`) suspected but not confirmed |
| 3 | 3633 | `0xAE64-0xB061` | **Encoding confirmed**, purpose genuinely unresolved | Vector shape table using the same pen-bit convention as `draw_readout_char`, rendered with the new `disasm/decode_vector_icons.py`; contains a 40-pt circle duplicated as a cyclic point-list rotation, a small circle sharing its center, a needle-length line, and 3 medium shapes that look letter-like at one scale but don't confirm as a clean alphabet - could be UI icons or a rough font, rendering didn't settle which |
| 4 | 3532 | `0xBEE6-0xC263` | Partially resolved | Only the first 44 bytes are near an already-known self-test string cluster (not an exact match); the other ~850 bytes are a different, still-unknown structure - the original "one table" framing was wrong |
| 5 | 2998 | `0x80EC-0x8211` | Plausible | Grouped incrementing-ID record table; tentatively status/error-category-related, not a clean match yet |
| 6 | 2998 | `0x0824C-0x08A57` | Well-supported, not fully decoded | Second pointer/dispatch table immediately adjacent to the confirmed command-keyword-table cluster; likely id-to-handler-address, complementary to the confirmed id-to-display-string table; not the SELECT C1/C2 mechanism |

None of the addresses discussed here were renamed in
`gen_disasm_x86.FUNCTIONAL_NAMES`/`FUNCTIONS.md` - none reached this
project's "mechanism confirmed" bar for a rename. See
`STILL_PENDING_DECODE.md` for these as tracked open items.

## Follow-up, 2026-09-22

A second manual pass, picking up a handful of the ~44 blocks the
2026-09-15 pass didn't individually chase. Four quick looks, one real
finding worth acting on.

### 7. `160-3633` file `0xFF99-0xFFED` (block 20): not data at all - a coverage-tooling bug orphaned 85 bytes of real code (confirmed)

This block sits right after code the heuristic disassembler already
has (`...0xEFF80-0xEFF96`, ending in a `jmp` at `0xEFF96`) and right
before more code it already has (resuming at `0xEFFEF`). Manually
decoding all 85 bytes byte-by-byte confirms it's a single coherent,
valid instruction stream the whole way through - not a coincidence:

- `mov di, [bp-0xA]` then three `shl di,1` (`di = param*8`), `les bx,
  [0x1C94]`, `es: mov dl, [bx+di+7]` - an indexed 8-byte-stride
  far-pointer table lookup, result zero-extended into `dx` and stashed
  at `[bp-0x10]`.
- A second indexed test re-derives `di = param*10` against
  `[di+0x1BE2]` bit 0, and branches on both that flag and `[bp-8]`
  through two short paths - **both of which converge back at
  `0xFFEF`**, exactly where the heuristic disassembler's own listing
  already has valid code waiting. That convergence is the real proof
  this is genuine, reachable control flow, not 85 bytes that merely
  happen to decode cleanly.
- One path computes `di = param*9`, does a second far-pointer lookup
  via a *different* RAM pointer (`[0x1DDC]`), sets a flag bit, and
  `lcall`s `0xF7BF:0x043B` before a final `jmp` forward.

**Root cause of why this showed up as "unknown" at all**: that final
`jmp`'s 3 bytes (`e9 27 00`) straddle the block's own labeled end at
`0xFFED` - the trailing `00` byte at `0xFFEE` is genuinely part of
this instruction, not a new one. The block-boundary math (in whichever
of `find_unknown_data.py`/the coverage scanner drew this block's
edges) stopped 1 byte short of where the real instruction actually
ends, orphaning that single byte - which is exactly what caused the
*heuristic listing itself* to mis-decode a bogus instruction starting
at `0xFFEE` (`add byte ptr [bp+di-0xfba], cl`), a false "landing
artifact" that isn't a real compiler/linker quirk at all, just this
off-by-one. Both this jump and the earlier one at `0xFFB8`
(`74 48`/`75 2d`, not shown above) resolve to targets *past* `0xFFFF`
- they wrap into `160-3532`'s own file-offset space (`0xF0004`/
  `0xF0016`) under this project's already-established combined
128KB main-ROM addressing convention (`MEMORY_MAP.md`). **Worth
checking as a next step**: whether the coverage/heuristic tooling
systematically fails to follow any jump whose target wraps past a
chip's nominal 64KB half this way - if so, there may be more spurious
"unknown data" blocks immediately upstream of a 0xFFFF-crossing jump
elsewhere in `160-3633`/`160-3532`, not just this one instance.
Two new candidate RAM-pointer variables came out of this trace,
`[0x1C94]` and `[0x1DDC]` - added to `VARIABLES.md`'s "not yet
identified but seen referenced" list, purpose still unknown.

### 8. End-of-chip blocks are just unprogrammed EPROM filler (confirmed, mundane)

Checked the last block of each of the 3 chips - `160-3633` block 20
covers the sub-case above, but `160-3532` block 20 (`0xFFFC0-0xFFFEF`,
right before the real 8088 reset-vector bytes at `0xFFFF0`, which
*are* already disassembled) and `160-2998` block 9
(`0x8FE91-0x8FFFFF`) are both **entirely `0xFF` bytes** (`160-3532`'s
has two leading `0x00` bytes, then all `0xFF`). This is the standard
signature of unprogrammed/erased EPROM space, not a real table -
closed, no further action needed.

### 9. `160-3633` block 1 (`0xE0205-0xE0269`): a plausible but unconfirmed ROM-quadrant address table

Sits immediately after `boot_init`'s own final backward `jmp` (to
`L_E00C2`, `0xE00C2`) - genuinely skipped over at runtime by that
branch, not a landing-artifact false positive like block 20 above (no
convergence back into already-known code found downstream). Decoded
as far pointers, the first 5 entries are `E000:8000`\-`0xE8000`,
`F000:8000`\-`0xF8000`, `E000:0000`\-`0xE0000`, `F000:0000`\-`0xF0000`,
`0000:0000`\-`0x00000` - i.e. the base address and midpoint of
*exactly* `160-3633` and `160-3532`'s own 64KB chip spans. The
remaining ~30 words continue with `0x0000`/`0x4000`/`0x8000`/`0xC000`/
`0xFFFF`-style values against segments `E000`/`F000`/`0000` - the four
16KB-aligned boundaries of a 64KB space, consistent with a ROM
self-test table that reports pass/fail per 16KB quadrant (matching the
already-known `"ROM/RAM/NMI :"` self-test category in `STRINGS.md`).
**Not confirmed**: an exhaustive grep for any code that computes a
pointer into `0xE0205` (or indexes off a register loaded from it)
found nothing - either it's read only via a fully dynamic/computed
address this search can't find, or it's dead/superseded data. Treat
the "self-test ROM quadrant table" framing as a hypothesis, not a
finding.

### `160-2998` block 8 (`0x88D1C-0x88D3D`, 34 bytes): looked, no hypothesis yet

All 34 bytes are small integers in the range 2-6 (mostly 3) - too
uniform to be far pointers (already ruled out per the general
convention above), and not an obvious counter/ramp either. Sits ~700
bytes after the confirmed command-keyword-table cluster
(`0x88005-0x88A57`) with real code in between, so not obviously part
of that same structure. No further progress; left open.

## Follow-up, 2026-09-22 (second pass): a systematic sweep for the wrapping-jump mechanism, and one more confirmed code block

Finding 7 above flagged a "worth checking as a next step": does the
coverage tooling systematically miss code sitting just upstream of any
jump whose target wraps past `0xFFFF` into the neighboring chip's
file-offset space? `disasm/find_sandwiched_unknown_blocks.py` answers
this directly - it re-walks the full proven+heuristic entry-point
corpus, then for every remaining `UNKNOWN_DATA.md`-style gap checks (a)
whether it's "sandwiched" (covered code immediately before *and* after
the gap - the shape block 20 had) and (b) whether the covered
instruction immediately before it is a jmp/call/conditional-jump whose
*unmasked* Capstone target is in `0x10000-0x1FFFF` (this chip's other
64KB half).

**The wrapping-jump mechanism specifically is confirmed isolated to
block 20** - across every chip's entire proven+heuristic instruction
corpus, exactly one instance exists project-wide (the same two jumps
already described in finding 7). No second wrapping-jump-orphaned
block was found.

**But the "sandwiched" shape alone is far too weak a signal to stop
there**: 37 of the 49 total unknown blocks across all three chips
match it - unsurprising in a packed ROM where most bytes are code, so
it doesn't discriminate real orphaned code from ordinary data tables
that merely happen to sit between two functions. Manually
disassembling the highest-signal candidates anyway (ranked by a crude
"% cleanly decoded" proxy, then eyeballed) turned up:

- **One more fully confirmed instance - see finding 10 below.**
- Several `160-3532` "sandwiched" blocks in the `0x1A86-0x213A` range
  (file offsets `0x1A86`, `0x1B0E`, `0x1BFE`, `0x1C6D`, `0x1D1E`,
  `0x205F`, `0x208A`) are **not new mysteries at all** - they fall
  entirely inside finding 1's already-documented ~100-entry jump
  table (`0x1A33-0x213A`). Finding 1 already notes only 2 of that
  table's 4 known real callers land cleanly on an entry; these
  sub-ranges are simply table entries the recursive-descent walker
  never got an instruction-level entry point into, not independent
  unknowns.
- Six more blocks that *look* like real code by eyeball (clean
  `push bp`-style prologues, or direct references to already-tracked
  RAM variables) but aren't yet rigorously confirmed the way block 13
  was - no far-pointer/caller search done, no control-flow convergence
  verified. Flagged here as open leads, not findings:
  - `160-3633` `0xE77F8-0xE783C` (69B) - signed abs-value/negation
    logic (`cmp dx,0; jge; neg dx; neg ax; sbb dx,0`) on stack
    parameters `[bp+0xa]`/`[bp+0xc]` - resembles a signed
    division/remainder helper.
  - `160-3633` `0xE9180-0xE91EF` (112B) - a repeated
    `push di; lcall 0xF0EB:0x151; ...` pattern against varying `di`
    constants - looks like a table/dispatch lookup called with
    different item IDs.
  - `160-3633` `0xE97A2-0xE97C9` (40B) - a `cmp`/`loop`-driven `movsb`
    string-copy.
  - `160-3633` `0xE9404-0xE9471` (110B) - coherent conditional logic
    over `[0x530]` and a `[bp-0xc]` local, computing `di = index*4`.
  - `160-3532` `0xF08E7-0xF09BF` (217B) - touches the already-tracked
    plot-position variables `[0x6C0]`/`[0x6C4]`/`[0x6D6]`.
  - `160-3532` `0xF0A73-0xF0AA9` (55B) and `0xF6E23-0xF6E4B` (41B) -
    touch the plot-scale variable cluster (`[0x712]`/`[0x716]`/
    `[0x71A]`/`[0x722]`) and RAM variables `[0x1B72]`/`[0x1B64]`
    respectively.
- The rest of the inspected candidates are ordinary data/noise. Two
  are worth naming because they rule out code outright: `160-3633`
  `0xEACE6-0xEAD07` decodes to `int1` (a reserved opcode) and
  `fld`/`fdiv` (x87 FPU instructions - not present on this 8088-class
  design), and a couple of others (`0xEAD15-0xEAD9F`, `160-3532`
  `0xF8000-0xF802A`) don't decode as any valid instruction at all from
  their very first byte.

### 10. `160-3633` block 13 (`0xE956E-0xE95A0`, 51 bytes): two more real leaf subroutines, caller still unresolved (confirmed code, unresolved entry mechanism)

Manually decoding this "sandwiched" block finds two small,
`retf`-terminated far-called subroutines, not data:

- **Sub-routine 1** (`0x956E-0x9580`, 18 bytes): sets `es=0x4000`,
  `di=0x37F6` (physical `0x437F6`, the already-confirmed Front Panel
  A/D control latch, U6104), writes the byte `0x1D` to it, far-calls
  `0xF15A:0x0001` (physical `0xF15A1`, an existing heuristic-only
  symbol `SUB_F15A1`), then `retf`.
- **Sub-routine 2** (`0x9581-0x95A0`, 32 bytes): `push si`; sets
  `es=0x4000`, `di=0x37F6`, `si=0x37FB` (physical `0x437FB`, the
  already-confirmed Main Front Panel Input, U6103 / `fp_intstat`);
  tests bit `0x4` of `fp_intstat`; both branches converge on reading
  the flat-DS variable `[0x4E4]` and writing it to the A/D control
  latch at `[es:di]`; `pop si; retf`.

Unlike block 20 (finding 7), no wrapping jump is involved - these are
entered via far-call/`retf` semantics, and the code itself is
unambiguously real (it manipulates two independently-confirmed
hardware registers in a coherent, non-garbage sequence). What's
**not** resolved: no literal far-pointer bytes anywhere in any of the
three ROM binaries reference either entry address
(`0xE000:956E`/`0xE000:9581`, checked via raw byte-pattern search), and
neither address appears in the already-documented 82-entry RAM
far-pointer table (`docs/acquisition-and-plotting/ram-far-pointer-
table.md`) or anywhere else in the project's docs/symbols. The real
caller is presumably a computed/indirect far pointer not yet found -
the same open-ended "unresolved far-pointer call" shape as that RAM
far-pointer table family, just not yet traced to its source.

**Net effect on finding 7's "worth checking" lead**: the *specific*
wrapping-jump mechanism is now confirmed isolated to block 20 alone.
But the *general* phenomenon it's a symptom of - real, reachable code
that the coverage tooling has no entry point into, because nothing in
the proven/heuristic corpus computes its actual (indirect/far) caller
- recurs here via a different root cause. Worth remembering as a
category, not just this one instance: an unresolved-caller block
doesn't mean "probably data," it can mean "definitely code, caller not
found yet."

## Updated summary table

| # | Chip | Physical | Confidence | What |
|---|---|---|---|---|
| 7 | 3633 | `0xEFF99-0xEFFED` | **Confirmed** | Not data - genuine reachable code a coverage-tooling off-by-one orphaned; also explains a previously-unexplained "landing artifact" at this exact spot as a boundary bug, not a compiler/linker quirk |
| 8 | 3532 / 2998 | `0xFFFC0-0xFFFEF` / `0x8FE91-0x8FFFF` | Confirmed, mundane | Unprogrammed EPROM filler (`0xFF`) before/after each chip's real fixed vectors |
| 9 | 3633 | `0xE0205-0xE0269` | Plausible, unconfirmed | ROM base/quadrant address table, sits right after `boot_init`'s own final branch; no consumer found |
| 10 | 3633 | `0xE956E-0xE95A0` | **Confirmed code**, caller unresolved | Two `retf`-terminated leaf subroutines writing to the confirmed front-panel A/D control latch and reading `fp_intstat`; no far-pointer reference to either entry found anywhere in the project |
| - | 2998 | `0x88D1C-0x88D3D` | Unresolved | Small-int (2-6) table, no hypothesis yet |

All 6 of the "unconfirmed lead" blocks listed in the 2026-09-22 second pass
above were traced to a conclusion in the 2026-10-09 follow-up below - none
remain open.

### 11. The 6 remaining unconfirmed leads, traced one at a time (2026-10-09 follow-up)

Per `TODO.md`'s own "next step if picked up" pointer, each of the 6 blocks
flagged in the 2026-09-22 second pass was manually disassembled and then
checked for (a) convergence into already-known code immediately past the
gap, (b) membership inside an already-reached function's own body, or (c) a
real caller - the same bar findings 7 and 10 above were held to. All 6
turned out to be genuine code; none were false leads. In order of
decreasing novelty:

- **`160-3633` `0xE77F8-0xE783C` (69B) - a brand-new standalone function,
  not previously represented in the symbol table at all.** Manually
  disassembling the full range (through its own `retf 4` epilogue, ending
  exactly 1 byte before `udiv32`'s own entry at `0xE783D`) found a complete
  function with its own `push bp; push bx; push cx; mov bp,sp` prologue -
  structurally identical to `sdiv32` (`0xE77AE`) except for exactly one
  difference: where `sdiv32` saves `dx XOR [bp+0xc]` (the XOR of both
  operands' sign bits) before computing `abs()` of each, this function
  saves plain `dx` (the dividend's sign alone), so the divisor's sign never
  affects the final result - only its magnitude does. It also calls a
  different point inside `udiv32` (`0xE7866`, `+0x29` into `udiv32`'s body,
  a secondary entry - not `udiv32`'s own primary entry at `0xE783D` that
  `sdiv32` itself calls; not reconciled further). Root cause for why
  neither discovery mechanism had ever found it: no literal far-pointer
  bytes reference `0xE000:77F8` anywhere in any of the 3 ROMs (checked
  directly - the caller is still genuinely unresolved, same "confirmed
  code, caller not found" category as finding 10 above), and the mainrom
  heuristic's push-bp prologue scanner structurally cannot match it either
  - its signature requires `mov bp,sp` *byte-adjacent* to `push bp`, and
  this function has `push bx; push cx` in between. A project-wide scan for
  that exact "delayed prologue" shape turned up only 3 instances total
  across `160-3633`/`160-3532`: `ashr32` and `sdiv32` themselves (both
  already known, both missed by the same scanner for the same reason) and
  this one. Named `sdiv32_unsigned_divisor` and added as a manual
  `ENTRY_POINTS` tuple in `gen_disasm_x86.py` (a `FUNCTIONAL_NAMES` entry
  alone doesn't make the walker visit an address it was never going to
  reach) - see `FUNCTIONS.md`.
  **Renamed to `smod32` 2026-10-09**: finally traced the "secondary entry
  into `udiv32`" mentioned above (`0xE7866`) and its own target
  (`0xE7895`) - they're a previously-unnamed sibling pair (`umod32`/
  `umod32_core`) sharing `udiv32_core`'s restoring-division loop body but
  returning the remainder instead of the quotient. `sdiv32_unsigned_
  divisor`'s actual operation is therefore signed 32-bit modulo (dividend-
  sign-only, reapplied to an unsigned remainder - the standard truncating-
  `%` rule), not a division variant - see `FUNCTIONS.md`'s `smod32`/
  `umod32`/`umod32_core` entries for the full evidence.

- **`160-3633` `0xE9180-0xE91EF` (112B) - the shared tail of an existing
  switch/case dispatcher, not a new function.** Falls through cleanly from
  4 already-known, adjacent dispatch-case labels (`L_E9120`/`L_E913F`/
  `L_E915E`/`L_E917D`, each a `mov di,CONST` setting a different item-ID
  constant) and converges forward into already-known code at `L_E924B`
  (`ref_count` 5, i.e. already reached from 5 other places). No own frame,
  so not independently nameable - it's an outlined dispatcher tail, the
  same category as other no-own-frame fragments already left unnamed
  elsewhere in this project.

- **`160-3633` `0xE9404-0xE9471` (110B) - a gap inside an existing,
  already-reached-but-unnamed function (`FUNC_3633_93D8`), bridging
  straight into the already-named `merge_record_flags_if_changed`
  (`0xE9472`).** The `.lst`'s last decoded instruction before the gap is
  `L_E93FF`'s `cmp byte ptr [0x530], 0` (ending at `0xE9403`); the next
  decoded instruction anywhere is `merge_record_flags_if_changed`'s own
  first instruction at exactly `0xE9472` - i.e. the gap's documented
  boundaries (`0xE9404-0xE9471`) account for *every single byte* between
  the two, with nothing left over on either side. Manually disassembling
  the gap shows an entirely ordinary `je 0xe9420`-driven conditional
  computing `di = index*4` and reading `[0x1D50]`/[bp-0xc]`, then falling
  straight through into `merge_record_flags_if_changed` with no `jmp` at
  all - confirming the two are the same function, not two functions back
  to back (consistent with `merge_record_flags_if_changed`'s own
  `FUNCTIONS.md` entry already describing it as using `[bp-0x10]`, a
  caller-frame-relative reference, i.e. it has no prologue of its own and
  was always a fallthrough continuation of whatever precedes it). No
  explanation found for *why* the recursive-descent walker stopped
  precisely at an ordinary, unambiguous `je` - left as an open "why does
  coverage stop here" question rather than forced to a conclusion, the
  same honest treatment finding 9 above got.

- **`160-3633` `0xE97A2-0xE97C9` (40B) - not a mystery at all: squarely
  inside the already-fully-documented `extract_strided_channel_samples`
  (`0xE9744`).** Manually walking the complete function body from its
  documented entry at `0xE9744` through its teardown at `0xE97FA` passes
  directly through this "gap" as one of several unrolled byte/word-copy-
  stride loop variants - it was never a real gap, just an under-walked
  part of a function whose existence and purpose were already settled.
  No doc changes needed beyond this note; `FUNCTIONS.md`'s existing entry
  already covers it correctly ("2/3/6 bytes seen across the 3 entry
  points").

- **`160-3532` `0xF08E7-0xF09BF` (217B), `0xF0A73-0xF0AA9` (55B), and
  `0xF6E23-0xF6E4B` (41B) - all 3 are gaps sandwiched inside already-
  reached-but-unnamed functions, the same shape as the `0xE9404` case
  above.** Each gap's bytes decode cleanly (no invalid opcodes) and
  reference the already-tracked plot-position/plot-scale variable
  clusters, as already noted in the 2026-09-22 pass. Checking the symbol
  table this session found, for each one, an already-known (but unnamed)
  entry a short distance before the gap's start (`FUNC_3532_08CB`, 28
  bytes before `0xF08E7`; `SUB_F0A4E`, 37 bytes before `0xF0A73`; and
  `SUB_F6E05`, 30 bytes before `0xF6E23`) *and* an already-known
  entry/label picking back up at exactly the byte immediately following
  each gap's end (`SUB_F09C0` at `0xF09C0`, `SUB_F0AAA` at `0xF0AAA`, and
  `L_F6E4C` at `0xF6E4C`) - the same "gap bytes fully accounted for
  between two already-reached points" pattern already confirmed for the
  `0xE9404` case, just not walked instruction-by-instruction end to end
  the way that one was. Treated as confirmed real code at the same
  confidence tier as the `0xE9404` finding; full instruction-level walks
  of these 3 (and any resulting naming of `FUNC_3532_08CB`/`SUB_F0A4E`/
  `SUB_F6E05`) are left as a follow-up, not done this session.

## 2026-10-09 follow-up: `SUB_EA13B`/`SUB_EA2D6` are NOT real functions - their only caller's far-call targets land inside known string-table text

Picked up the two previously-flagged, never-investigated unnamed
proven functions from `sysrom_3532_3633.symbols.json` (after
correcting the earlier mislabeling of them as comm-ROM addresses - see
`changes/2026-10-09.md`). One (`SUB_E7895`) turned out to be real
(`umod32_core`, see above). The other, `SUB_EA2D6`, does not.

`SUB_EA2D6`'s `.lst` body (physical `0xEA2D6` onward) decodes into a
stream of individually-valid-looking but incoherent instructions -
`popaw`, `and byte ptr [...]`, `push`/`pop`/`dec` of single registers,
`bound`, `outsw`, `arpl`, `imul` with odd immediate operands - the
classic signature of an x86 disassembler decoding plain ASCII text
(many lowercase letters and punctuation alias to valid opcodes on an
8086). Checked directly: physical `0xEA2D6` is file offset `0xA2D6` in
`binary/160-3633-14.bin`, and `disasm/strings_160-3633.json` already
documents a string at offset `0xa2b9`, `"Press CURSOR SELECT to Start
a PLOT"` (36 chars + null terminator, ending exactly at `0xa2dd` where
the next string, `"Enable plotting of graticule"`, begins). `0xA2D6`
falls at character 29 of that string - squarely inside it, with no
gap or boundary ambiguity. Reading the raw bytes directly confirms it:
`SELECT to Start a PLOT\x00Enable plotting of graticule...`.

`SUB_EA2D6`'s only confirmed caller, `SUB_F173E`, does the exact same
thing with a **second** far call to `SUB_EA13B` (physical `0xEA13B` =
file offset `0xA13B`), which likewise lands inside a different known
string: `"Points before trigger, PRE or POST"` (offset `0xa120`,
`0xA13B` is character 27 of 35). Both call targets are mid-string-
literal, not function entries.

This matters because `SUB_F173E` isn't a stray heuristic guess - it's
one of the 15 addresses in `gen_disasm_x86.ENTRY_POINTS`' "found by
fully decoding `init_far_pointer_table_sysrom`'s own embedded RAM-init
table" batch (see that comment in `gen_disasm_x86.py`, just above the
`sdiv32_unsigned_divisor`/`smod32` entry), whose blanket claim is
*"every one of these decodes as coherent, non-garbage x86."*
`SUB_F173E`'s own body (`push dx; lcall SUB_EA13B; add [0x6aa],ax; les
di,[bp-0xc]; inc word ptr [bp-0xc]; mov dl,es:[di]; mov [bp-7],dl; cmp
dl,0; jne +9; push [0x6aa]; push [0x678]; lcall SUB_EA2D6; ...`) does
read as a plausible, self-consistent "copy a far string byte-by-byte
until a null terminator, then call a completion handler" loop - the
claim holds up for *this* function's own body by eyeball. But that
claim evidently doesn't extend to verifying what its *call targets*
actually are, and in this case both of `SUB_F173E`'s far-call targets
turn out to be string-table bytes, not code.

**Left unresolved, not forced to a conclusion** (per this project's
"don't guess real hardware content" standard) - three live
possibilities, none confirmed:

1. `SUB_F173E`'s own decode is itself a desync artifact despite
   looking superficially coherent (classic x86 disassembly risk: a
   short, by-chance-valid instruction sequence that was never actually
   executed) - which would mean `SUB_F173E` doesn't belong in the
   "proven" set at all, and by extension neither do `SUB_EA13B`/
   `SUB_EA2D6`, which exist in the symbol table *only* because
   `SUB_F173E` calls them.
2. `SUB_F173E` is real code, but its far-call operand bytes are being
   misread somehow (segment/offset byte order, or a hardware-level
   address-line/bank-select quirk not yet documented anywhere in
   `MEMORY_MAP.md` or the emulator's "Findings and gotchas" - though
   no evidence for this beyond the contradiction itself has been
   found, and the CPU is a confirmed plain-real-mode 8088/8086 with no
   documented reason physical address computation would differ from
   `segment*0x10 + offset` here).
3. The ROM genuinely contains a far call whose target is a string
   literal, for some reason not yet understood (e.g. self-modifying
   code that patches this call's operand at runtime before it's ever
   executed - the two pushed values just before the `SUB_EA2D6` call,
   `[0x6aa]`/`[0x678]`, are plain data words, not obviously code, so
   this seems unlikely but hasn't been ruled out).

**Conclusion for naming purposes**: `SUB_EA2D6` and `SUB_EA13B` are
**not safely nameable** - there is no confirmed evidence either is
real executable code, and strong direct evidence (readable English
text at their exact target addresses) that they are not. Left
unnamed; `TODO.md`'s renaming-progress item should treat both as
"investigated, found to be non-code" rather than "not yet looked at."

## 2026-10-09 follow-up #2: `SUB_EAC86`/`SUB_EADA0` are the same desync pattern, but with a much harder contradiction - a confirmed, named, important caller

Picked up the highest-ref-count remaining unnamed proven function,
`SUB_EAC86` (ref_count=4). Same desync signature as `SUB_EA2D6`, but a
different flavor and a genuinely harder puzzle.

**The data-table evidence is airtight.** `SUB_EAC86`'s body (physical
`0xEAC86` onward) decodes as a repeating 16-byte-period record -
`02 D8 F1 00 <2 bytes> 00 E8 E0 00 00 00 00 21 00 <incrementing byte>`
- not readable text, but a structured binary pattern. Checked against
`UNKNOWN_DATA.md`'s existing, independently-generated gap list: **block
16** (file-offset-derived, phys `0x0EA730-0x0EAC85`) ends at the byte
*immediately before* `SUB_EAC86` starts (`0xEAC86`), and the garbage
decode runs in an unbroken stream - `add`/`or`/`int1`/`iret` chains,
same `02/04 D8 F1` header repeating - until an `iret` at exactly
`0xEACE5`, one byte before **block 17** (phys `0x0EACE6-0x0EAD07`)
begins with the identical `02 D8 F1 00 ...` pattern. Zero-byte gap on
both sides: `SUB_EAC86`'s "code" is precisely the missing link between
two already-documented unexplained-data blocks, strongly suggesting
one continuous data table that the garbage disassembly merely hid from
`UNKNOWN_DATA.md`'s own gap detector (confirmed via raw byte reads of
`binary/160-3633-14.bin`, not just the `.lst`). A byte-level scan for a
`push bp` (`0x55`) prologue anywhere in the surrounding `0xEAC60`-
`0xEACF0` range found none, ruling out a simple alignment-off-by-a-
few-bytes explanation for a hidden real function nearby.

`SUB_EAC86`'s one apparent internal `call` (line 13526, physical
`0xEACBD`, `call SUB_EADA0 ; 0xa60`) is `SUB_EADA0`'s *only* reference
anywhere in the disassembly - the same "proven only because its
caller incidentally decodes a call instruction" inheritance pattern as
`SUB_EA13B`/`SUB_EA2D6` inheriting from `SUB_F173E`. Read directly:
`SUB_EADA0`'s own body (phys `0xEADA0` onward) is a long, repetitive
run of nothing but `add`/`adc` with small immediate or register-pair
operands (`add ax,[bp+si]`; `add al,1`; `add ax,0x600`; `adc al,7`...
`adc al,0xf`) - exactly what a disassembler produces walking over a
table of small sequential/incrementing byte values, since opcodes
`0x00`-`0x15` are almost entirely `add`/`adc` variants. No plausible
function shape at all. Both `SUB_EAC86` and `SUB_EADA0` read as pure
data-table garbage individually, just as cleanly as `SUB_EA2D6`/
`SUB_EA13B` did.

**But unlike the `SUB_F173E` case, `SUB_EAC86`'s callers are not
another shaky, same-batch entry point - they're already-confirmed,
already-named, operationally important code.** Traced all 4 of
`SUB_EAC86`'s callers:

- Three far calls from `160-3532-14` (`SUB_F5898`, physical `0xF5898`,
  at phys `0xF58B1`/`0xF58DA`/`0xF58F9`) - each preceded by a clean,
  purposeful argument sequence: `push es; push <computed far ptr>; mov
  bx,0xff7b; push bx; mov dx,<small constant>; push dx; lcall
  SUB_EAC86`, with the small constant varying (`0x20f`/`0x221`/
  `0x233`) across the three calls. `0xFF7B` is this project's
  already-confirmed fixed string-table segment (see `CLAUDE.md`'s
  "check what string it references" convention, and every prior
  `self_test_dispatcher`-sibling identification) - this is the exact
  calling shape of "resolve the string/record at `0xFF7B:offset`, copy
  it to this destination far pointer," not a coincidence.
- `SUB_F5898` itself is called exactly once, from `0xED7F1`, inside
  `draw_boot_splash_and_option_icon` (phys `0xED7DF`, **already
  renamed and documented** in `FUNCTIONS.md` - "calls `SUB_F5898` (the
  'TEKTRONIX' boot-splash stroke-data builder) to draw the logo").
  `draw_boot_splash_and_option_icon` is itself called once from the
  comm ROM's boot sequence (phys `0x839E3`) - this is not a fringe or
  speculative code path, it's the confirmed boot-splash/logo-drawing
  routine.

So the contradiction here is sharper than the `SUB_F173E` case: instead
of "a shaky entry point calls into string data," it's "a previous
session's already-confirmed, FUNCTIONS.md-documented, boot-critical
routine (draw the TEKTRONIX splash logo) calls 3 times, with a clean
and purposeful string-table-style argument convention, directly into
bytes that independently and unambiguously look like an inert data
table - one that's already catalogued as unexplained in
`UNKNOWN_DATA.md` on both sides of `SUB_EAC86`."

**Tried the emulator to settle it empirically, inconclusive.** Booted
`emulator/interactive.py` from reset with a breakpoint at `0xEAC86`
(both default and `--comm-installed` explicit). The breakpoint was
never hit - the run halts via a real `hlt` instruction at phys
`0xF1611` after ~5.2M instructions, having logged several self-test
failures already documented as open/unstubbed gaps in `emulator/
README.md` (`ROMS : MISMATCH,14,4C,14`, `COMM_ROM` checksum mismatch,
`COMM_LB` failures, `MM_ACQ`/`CDT` - all pre-existing, known
limitations, not something introduced by this check). This means the
emulator currently can't confirm *or* refute whether `0xEAC86` is ever
reached on a real, fully-passing boot - the self-test failure path it
takes instead may itself skip the splash-drawing sequence entirely.
Worth re-trying once those self-test stub gaps (`TODO.md`) are closed
further.

**One more wrinkle worth recording**: `SUB_F5898` itself has no
prologue - its very first instruction (phys `0xF5898`, `les ax, ptr
[si]`) immediately follows the label, and the function reads `[bp-0xc]`
/`[bp-0xa]` right away without ever executing its own `push bp; mov
bp, sp`. It ends with `mov sp, bp; pop bp; retf` - the shape of a
*caller's* epilogue, not a normal callee's. That means `SUB_F5898`
only works correctly if `bp` already points at a valid frame set up by
whoever called it (here, `draw_boot_splash_and_option_icon`), and its
own ending `pop bp; retf` looks like it would tear down and return
through *that caller's* frame rather than just returning to the
instruction after its own `lcall` site. This is in tension with
`FUNCTIONS.md`'s existing description of `draw_boot_splash_and_option_
icon` continuing to run *after* the `SUB_F5898` call (checking `[bp+
0xA]` and drawing a second graphic) - a continuation that this
prologue-less, caller-frame-reusing shape makes harder to explain under
plain call/return semantics. Not chased further this session beyond
noting it; it's additional evidence this call chain deserves a closer,
dedicated look (ideally with the emulator, once it can boot far
enough) rather than being taken at face value.

**Left unresolved, not forced to a conclusion.** Live possibilities:

1. The `02/04 D8 F1`-pattern region (`UNKNOWN_DATA.md` blocks 16-19)
   is not what it looks like - maybe it's a sparse table whose entries
   are read as *data* by other code (an index/offset table), and
   `SUB_F5898`'s far call is itself based on a wrong/stale operand (an
   editing mistake somewhere upstream in the real ROM, or a dead/
   superseded code path never actually reached at runtime - note
   `SUB_F5898` and `draw_boot_splash_and_option_icon` are each called
   exactly once, so there's no redundant, independently-confirmed call
   site to cross-check against).
2. This exact byte range of `binary/160-3633-14.bin` is a genuine dump
   error (a stuck bit or misread run during the original EPROM
   extraction) rather than a disassembly artifact - no corroborating
   evidence either way has been found (no second independent dump to
   diff against, no ROM checksum/CRC verification exists in this
   project yet), but it would cleanly explain why a clearly-purposeful,
   already-trusted caller points at bytes that decode as neither code
   nor any recognizable data shape.
3. `draw_boot_splash_and_option_icon`/`SUB_F5898`'s own "mechanism
   confirmed" status from the earlier session is itself not as solid
   as documented - worth revisiting if this anomaly resurfaces
   elsewhere.

**Conclusion for naming purposes**: `SUB_EAC86` and `SUB_EADA0` are
**not safely nameable** - same standard as `SUB_EA2D6`/`SUB_EA13B`.
Left unnamed. This case is notable enough to flag distinctly in
`STILL_PENDING_DECODE.md` rather than folding it into the earlier
finding, since the caller-side evidence (a confirmed, boot-critical,
already-named routine) is a materially different and stronger
contradiction than `SUB_F173E`'s.
