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
