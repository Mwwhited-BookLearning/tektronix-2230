# Vector display list and the stroke-font hunt

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## The readout vector display list (a second, separate print mechanism)

Found while working through the "rename everything" pass, tracing the
print-primitive cluster around `160-3633:0x3200-0x3A00`. There are
**two entirely separate mechanisms** for putting text on the readout,
not one:

1. **The hardware-port path** (documented above): `print_string_far`
   -> `print_char` -> `write_readout_port_byte`, writing straight to
   the fixed physical port `0x40000+0x6F0`.
2. **The vector display-list path** (new this session):
   `print_readout_string` (`0xE3821`) -> `draw_readout_char` (`0xE3854`)
   -> `plot_readout_point_relative` (`0xE3900`) ->
   `plot_readout_point` (`0xE3930`), which **appends a `(y, x)`
   coordinate pair plus a duplicated attribute byte** to a buffer at
   `[0x1CC4]` (tracked via a running write-pointer at `[0x1AF4]`/
   `[0x1AF6]`, with a *second plane* offset stored in `[0x1C02]` - the
   same dual-plane pattern `append_readout_char` uses). This is a
   **vector-stroke display list**, not a bitmap: `draw_readout_char`
   looks a character up in a stroke-font table at `[0x1DB0]` (indexed
   by `char & 0x7F`, 4 bytes/char - almost certainly a far-pointer
   table into a per-character stroke-list), then walks each stroke
   byte extracting a pen-up/down bit (`0x80`), a coarse component
   (bits 4-6, `>>4`), and a fine component (bits 0-3), calling
   `plot_readout_point_relative` once per stroke. This is exactly the
   classic Tektronix "readout" character-generator format used on
   other scopes of this era (each character is a short list of vector
   strokes, not a bitmap) - strong independent corroboration that this
   scope's CRT readout (channel/volts/time labels drawn alongside the
   analog trace) really is stroke/vector-based, matching `CONTEXT.md`'s
   hardware overview.

   `plot_readout_point` also handles **circular-buffer wraparound**:
   if the write pointer advances past `[0x1CC4]+0x1400` (5120 bytes),
   it wraps back to `[0x1CC4]+0x13FF` and writes a marker byte
   (attribute `2`) - the *same* marker value `mark_readout_delimiter`
   (`0xE3766`) uses, suggesting `2` is a reserved "delimiter/marker"
   attribute code throughout this buffer format, not a real
   character attribute.

   Print **records** in this system (`init_print_record`/
   `build_print_record`/`init_print_region` .. `close_print_record`)
   bracket a line: `init_print_region` opens one (position + default
   16x16 cell size), the line's text is drawn into the display list,
   and `close_print_record` tags the record's first byte with a
   completion code (`0x11` for the "normal" close, `0x39` for a second
   variant, `close_print_record_b` - exact meaning of the two codes
   not confirmed). **Correction**: earlier pseudocode wrongly assumed
   `close_print_record` (then still `SUB_E374E`) printed a string via
   its far-pointer argument - it doesn't traverse that pointer at all,
   it only tags the record. Fixed in `PSEUDOCODE.md`.

**Update 2026-09-13: `[0x1CC4]`'s actual value is now confirmed, and
the old `0x4000`-segment assumption above was wrong.** Directly asked
"what memory address does the vector-graphics display list actually
use" led to decoding it out of `init_far_pointer_table_sysrom`'s RAM
far-pointer init table (the same table decoded earlier this session -
see "Found: a whole family of never-reached functions"): entry
`dest_off=0x0044` (physical destination `0x2090+0x44=0x20D4`, i.e.
`[0x1CC4]` under the standard `DS=0x41` flat mapping) holds far pointer
**`0220:0000` = physical `0x02200`**. Verified the table-decode itself
against 3 already-independently-confirmed entries (`SUB_F09EA`, `SUB_
E9472`, `SUB_F173E`) before trusting this - all matched exactly, so the
decode mechanism is solid.

**So the vector/stroke display-list buffer lives in plain RAM at
physical `0x02200`** (with its "second plane" duplicate ~0x8000 bytes
higher, around physical `0xA200`) - **not** a hardware I/O port, and
**not** the same `0x40000`-based readout/CRT memory the hardware-port
text path uses. It's an ordinary low-RAM scratch buffer, separate from
both. This directly answers "what address does the vector graphics
generation use" - it's a software-staged command buffer, not a
hardware sink; whatever hardware actually turns these buffered points
into a drawn vector on the CRT (if anything does, on this build) hasn't
been identified.

**Loose thread found while checking this, not resolved**: the same
table entry cluster (`dest_off` `0x130`/`0x134`/`0x138`/`0x13C`,
spaced exactly 4 bytes apart - matching `[0x1DB0]`/`[0x1DB4]`/
`[0x1DB8]`/`[0x1DBC]`'s real spacing, strong evidence these are the
right variables and not a coincidence) also appears to initialize the
still-unlocated stroke-font pointer `[0x1DB0]` and `[0x1DB4]`. But
those two entries' resulting far pointers (`E9A3:0000` for `[0x1DB0]`,
`E947:0002` for `[0x1DB4]`) land on **real compiled code** (a normal
`push bp; mov bp,sp` function prologue right after `[0x1DB0]`'s target;
`E947:0002` is `merge_record_flags_if_changed`'s own address for
`[0x1DB4]`), not a plausible glyph-pointer-array header. Checking a
few sample per-character second-level lookups (assuming `[0x1DB0]`
really does point to a 128-entry `char*4`-indexed array starting right
there) produced incoherent, seemingly-random far pointers, not a
sensible pattern. **Not claiming this resolves the stroke-font table**
- flagging it honestly as an unresolved puzzle: either these two
specific entries are stale/leftover initialization values (matching
this session's broader theme of dead/unused table rows), or the real
per-character lookup structure is more complex than assumed. `[0x1DB8]`/
`[0x1DBC]`'s entries (`0033:0008`→phys `0x338`, `002E:0008`→phys
`0x2E8`, both low-memory/IVT-region addresses) are more plausible for
their already-confirmed simple byte-cache usage, for what it's worth.

## Attempted: locating the stroke-font glyph table for SVG extraction

Per the user's request (they noticed vector graphics/icons on the CRT
in the hardware photos and asked whether the ROM's raw-data segments
might be this kind of vector data, wanting SVGs + a catalog if it can
be decoded): the mechanism is fully understood and already reflected
in the naming above (`draw_readout_char` reads a per-character far
pointer from a table at `[0x1DB0]`, indexed `(char & 0x7F) * 4`; each
stroke byte packs pen-up/down (`0x80`), a coarse component (`(byte &
0x70) >> 4`, 0-7), and a fine component (`byte & 0xF`, 0-15), consumed
one byte at a time until a `0x00` terminator - confirmed by directly
tracing `plot_readout_point`'s parameter order: `[bp+6]` = Y
(coarse + a baseline captured once per character from `[0x1AF8]`),
`[bp+8]` = X (the raw fine nibble), `[bp+0xa]` = a pen attribute
derived from both the stroke's own bit 7 and the character code's own
bit 7).

**What's missing**: `[0x1DB0]`'s actual value (i.e. where the glyph
table physically lives) - no write to it was found in the
proven-reachable code, so it's presumably set during an early boot
step not yet traced (`boot_init` does have a generic table-driven
initialization loop around physical `0xE0155`/`0xE017D`/`0xE01AF` in
`160-3633`, but it turned out to be a RAM-sizing/fill routine keyed by
a flag byte, not a simple pointer-copy table - not fully traced
further).

**Tried and ruled out**: wrote a script to scan both ROMs for a
128-entry far-pointer-array signature (segment mostly constant,
offsets mostly ascending, each pointing to a short byte run terminated
by `0x00`). It found a strong-looking candidate at physical `0xF1A30`
in `160-3532` (pointing into `160-3633` at segment `0xE9C3`) - but
directly reading that target region as raw bytes shows it's real
compiled code immediately followed by the plain-ASCII menu string
table (`HEAD\0ACQ_MODE_SETUP_TABLE\0SELECT_MODE\0SWP_...`), not glyph
data. The "table" was a coincidental byte-pattern match, not real
pointers - confirmed false positive, not pursued further as a lead.

**Next steps if resuming this**: either (a) trace `boot_init`'s
data-driven initialization loop(s) fully to find whatever sets
`[0x1DB0]` (and `[0x1CC4]`, also never found being written - likely
set by the same or a nearby step), or (b) tighten the heuristic search
to also validate that decoded strokes, when actually rendered,
produce a coherent connected shape for known letters (e.g. render
candidate glyphs for `'A'`/`'H'`/`'O'` and check visually/
programmatically for a plausible letterform) rather than just
checking byte-run length. No SVGs or catalog were produced this pass -
don't claim otherwise if this note is read out of context.

**Follow-up, 2026-09-13: built the actual decode-and-render tool, per
direct request.** `disasm/decode_stroke_font.py` implements the
confirmed bit-packing formula exactly (`coarse=(B&0x70)>>4`→Y,
`fine=B&0xF`→X, `pen_down=bool(B&0x80)`) and renders any byte range as
an SVG catalog of glyph cells - a real, reusable tool, not a one-off
script. Two things came out of using it:

1. **Re-confirmed the `0xEA5E6`-`0xEB131` negative result** with a
   proper tool instead of a throwaway PNG script - `disasm/stroke_font_
   candidates/0xEA5E6-0xEB131.svg` shows the same generic repetitive
   "hook"/checkmark shapes as before, no letterforms. This candidate
   stays ruled out.
2. **Realized the search approach up to this point was structurally
   wrong and tried a better one.** `[0x1DB0]` doesn't point directly
   at glyph *data* - it points at a 128-entry array of far pointers,
   each of which points to that character's own, possibly *non-
   contiguous*, stroke bytes elsewhere in ROM (re-confirmed precisely
   by re-reading `draw_readout_char`'s two `les`/index steps). This
   means the earlier "scan for a long contiguous run of small 0x00-
   terminated fragments" approach (this note and the `0xEA5E6` search)
   was looking for the wrong shape entirely - added `scan_pointer_
   table()` to search instead for the *pointer array itself*: a
   512-byte run where sampled character entries (space/digit/upper/
   lower) all resolve to physically plausible ROM addresses that
   themselves look like short, real stroke-byte runs (bytes whose
   upper coarse nibble stays in the valid 0-7 range). **Result: zero
   candidates found in either main-ROM chip even with relaxed
   thresholds** - genuinely inconclusive, not a confirmed rule-out.
   Either the scoring heuristic is still off (a real stroke run may
   legitimately use nibble values this filter rejects), the table
   lives in the comm ROM instead (not yet tried), or it lives in a
   region genuinely unreached by this project's `buf` reconstruction
   in some way not yet identified. Worth revisiting with looser/
   different heuristics rather than treating this as a final answer.

## Follow-up, 2026-09-14: fixed a real bug in the pointer-table scorer; still no real candidate; tried ground-truth byte matching, hit a concrete quantization puzzle

**Found and fixed a genuine bug in `_stroke_run_score`.** It penalized
any stroke byte with bit `0x08` set, on the theory that "coarse is
only 3 bits so this bit must be invalid" - but bit `0x08` is bit 3,
which belongs to the *fine* nibble (`0x0F`, bits 0-3), not coarse
(`0x70`, bits 4-6). Every byte value `0x01`-`0xFF` is a syntactically
legal stroke byte under this encoding (1 pen bit + 3 coarse bits + 4
fine bits accounts for all 8 bits with no reserved combinations) - the
old scorer was rejecting exactly half of all valid fine values. Fixed
to just score by run length; also swapped `space` out of the default
sample-character set (a real space glyph plausibly has zero strokes,
which the old "must have a run" check always failed regardless of
whether the table was real) and added a `--min-hits`/`--min-run` CLI
option to loosen the "every sample char must resolve" requirement.

**Reran across all three chips, including the comm ROM (never tried
as the table's *location* before)**: still **zero candidates** at the
strict "all 6 sample chars resolve" setting, on every chip. Loosening
to 4-of-6 surfaced candidates in `3532` and `2998` - **but resolving
every printable character's entry for the top `3532` candidate
(`0xF7C00`) shows wildly inconsistent segments jumping randomly across
all three ROM chips per adjacent character** (`'!' -> 0x8b15:7307`,
`'"' -> 0xd78b:f87e` (3633), `'#' -> 0x04e8:1608` (implausible), `'$' ->`
...) - the opposite of what a real font table would look like (which
should mostly reuse one or two consistent segments). **Confirmed false
positive, not a real table** - loosening the byte-validity filter just
lets random noise back in, since (per the bug-fix finding above) there
genuinely is no invalid stroke-byte pattern to filter on. Blind
byte-pattern scanning may be fundamentally unable to distinguish real
stroke data from coincidental noise here.

**Tried a different angle instead: match against real, live-captured
ground truth.** This project has an actual HPGL capture of an isolated
"2V" readout label (`scratchpad/plot1.hpgl`, from the 2026-09-14
`plot_hpgl_to_svg.py` testing session) - real vector strokes for the
digit `2` traced directly off the CRT, not a guess. The idea: derive
the expected native `(coarse, fine)` sequence for a known character
from real hardware output, then search the ROM for a matching byte
sequence (masking off the pen bit, since its exact semantics per byte
aren't needed for a coordinate-sequence match) - sidestepping blind
statistical scanning entirely.

The `2` glyph's plotted points: X uses `{10,14,18,22,26}` (GCD of
differences = exactly 4, mapping cleanly to native fine levels
`0,1,2,3,4` - well within the 0-15 range). **Y hits a real, unresolved
obstacle**: its 8 distinct plotted values `{115,119,127,131,135,139,
143,147}` also have GCD exactly 4, but mapping them the same way
(baseline = 115 -> coarse 0) needs native levels `{0,1,3,4,5,6,7,8}` -
**9 needed, when the coarse field is only 3 bits wide (max 8 distinct
values, 0-7)**. The value `8` doesn't fit. This isn't a rounding
error - the GCD is exact and forced by the data. Possible
explanations, none confirmed: the "baseline captured once per
character" (`[0x1AF8]`) isn't simply the character's own minimum
plotted Y, so 115 doesn't actually correspond to coarse `0`; the HPGL
output pipeline applies a scale factor that doesn't preserve a clean
integer relationship to the native grid (unlikely given how exact the
GCD came out, but not ruled out); or the true native Y step is finer
than 4 and two of these 8 plotted points are meant to land on the
*same* coarse code (needs independent confirmation, e.g. from a
second captured character, to distinguish from the first two
theories). **Checked the second character in the same capture (`V`) - confirms
this is systematic, not a fluke.** `V`'s plotted points hit the exact
same 8 distinct Y values as `2` (`115,119,127,131,135,139,143,147`,
same GCD-4 step, same gap at 123, same overflow needing level `8`).
Both characters sit on the same text line and apparently share
*exactly* the same Y-grid footprint despite being completely different
shapes - strong evidence the baseline/step is a per-line (or
per-print-record) constant, not something derived per-glyph from each
character's own bounding box, and that the "9 slots needed" overflow
is a real, reproducible property of this specific mapping attempt
rather than noise from one sample. **Still not resolved** - ruling out
"random coincidence" narrows it to a genuine off-by-one somewhere in
the baseline/step assumption (tried shifting the baseline to several
other candidate reference points, including a signed/two's-complement
reinterpretation of the 3-bit coarse field - every variant tried still
overflows by exactly one slot somewhere, since the real constraint is
the *span* `(147-115)/4=8` requiring 9 representable values no matter
where the baseline sits). The most likely remaining explanation: the
true native step is not exactly 4 HPGL units, and the HPGL output
pipeline's own plot-scaling math (already known elsewhere in this
codebase to use `imul`/`idiv` fixed-point ratios, not fixed integer
factors) doesn't preserve as clean a relationship as hoped - solving
for the real transform would need either a captured character with a
different, independently-verifiable relative shape (to set up 2+
equations) or finding the ROM's own plot-scale-for-readout-text
constant directly rather than reverse-solving from output samples.

## Tried tracing the RS-232 `MESsage` command to `draw_readout_char` - confirmed there's no direct call

A good idea worth recording precisely why it didn't pan out: `MESsage
<NR1>:"..."` writes arbitrary text to a readout row over RS-232, so
finding its handler would hand this hunt a second way in (trace
forward from a known caller, instead of scanning ROM bytes blindly).
An exhaustive byte-level scan for every far-`CALL` across all three
chips confirmed **the comm ROM never directly calls
`print_readout_string`/`draw_readout_char` (or any of their siblings)
anywhere** - all 40 real call sites into that family land in
already-identified self-test/boot-banner code, none in the comm ROM.
Full writeup: `docs/comm-rom/rs232-live-session-2026-09-14.md`'s
"Tried tracing `MESsage`" section. Whatever handles `MESsage` goes
through an intermediary that isn't a literal call to a fixed address -
most likely the same still-unfound mechanism behind "who walks the
command-ID dispatch table" - so solving that would likely unlock this
approach too.

## Follow-up, 2026-09-15: the exact draw_readout_char/plot_readout_point arithmetic, and why the HPGL-matching puzzle above may be attacking the wrong stage

Went back to the disassembly line-by-line (`draw_readout_char` at
`0xE3854`, `plot_readout_point` at `0xE3930`, `plot_readout_point_
relative` at `0xE3900`, `print_readout_string` at `0xE3821`) to pin
down the *exact* arithmetic behind "coarse -> Y, fine -> X" rather than
the high-level description above - this was prompted by trying to
resolve the "9 slots needed for an 8-value 3-bit field" puzzle from
the HPGL-matching section below, and it surfaces something that
changes how that puzzle should be attacked.

**Confirmed, byte-exact**, from `draw_readout_char`'s stroke loop:
```
attr  = stroke_byte & 0x80 ? char_default_pen : 0    // char_default_pen = 1 if (char & 0x80)==0 else 3
fine  = stroke_byte & 0x0F                            // 0-15, pushed RAW, no baseline added
coarse= (stroke_byte & 0x70) >> 4                     // 0-7
Y     = baseline + coarse                             // RAW ADD - not coarse*4, not scaled at all
X     = fine                                          // RAW, unscaled, no per-character offset
plot_readout_point(Y, X, attr)
```
`baseline` is `[0x1AF8]`, read into a local **once, at the very start
of `draw_readout_char`, before the stroke loop begins** - literally
verified as `al=[0x1AF8]` executing before any stroke byte is
processed, confirming "captured once per character" precisely (not
once per line, not recomputed per stroke). `plot_readout_point` itself
does no arithmetic at all - it just overwrites `[0x1AF8]`/`[0x1AFA]`
directly with whatever `Y`/`X` it's given and appends `(Y, attr, X,
attr)` to the display list at `[0x1CC4]`.

**This means the "coarse" field is added directly (1 native unit per
step), not multiplied by 4** - the confirmed HPGL step of 4 units
seen in every capture below **cannot come from this arithmetic at
all**. `draw_readout_char`/`plot_readout_point` operate in an internal
display-list coordinate space; there must be a **separate, not-yet-
traced downstream stage** that walks `[0x1CC4]`'s buffer and converts
these internal `(Y, X)` values into whatever the CRT/plotter actually
receives - that's where the real HPGL step-4 scale factor (and
whatever combines each character's local `X:0-15` cell coordinate
with its position along the line - `print_readout_string` itself,
confirmed above, does **not** advance any X position between
characters; it just calls `draw_readout_char` once per byte in a
plain loop) must live. **This wasn't identified or traced this
session** - finding it is now the more promising next step than
continuing to reverse-solve the scale factor from HPGL samples alone,
since those samples are two transform stages removed from the raw
stroke bytes (stroke byte -> internal display list -> *unfound
renderer* -> HPGL), not one stage as the section below effectively
assumed.

**Does this resolve the "9 slots" puzzle below?** Partially - it
explains *why* naive baseline-fitting hit a wall (the model was
missing an entire pipeline stage, so no amount of curve-fitting
against the wrong equation would ever land cleanly), but it does not
resolve it outright: even reasoning per-character (using each
letter's own local baseline) rather than pooling both captured
characters together, `V`'s own points alone already span 8 native
`coarse` steps (0-7) *plus* one more (its first plotted point needs
what would be `coarse=8` under a naive baseline read) - so the
overflow isn't purely a pooling artifact either. Whatever the
downstream renderer does with `baseline`+`coarse` isn't a simple
`Y_hpgl = A + 4*(baseline+coarse)` - solving it precisely needs that
renderer's own disassembly, not more sample-fitting.

## Found a second, independent reader of `[0x1DB0]` - `FUNC_3633_E60C` (real, structurally confirmed; not proven-reachable)

Went hunting for "whatever reads `[0x1CC4]` and produces real output"
per the previous section's own next-step suggestion - traced through
~30 reference sites and found something more specific and more
valuable than a renderer: **a second function that independently reads
the exact same `[0x1DB0]` stroke-font pointer table** `draw_readout_
char` uses, with the identical indexing and extraction formula:

```
3633:E612  mov dl, [bp+6]     ; char code argument
3633:E618  shl dx, 1          ; dx *= 4  (same "char*4" indexing as draw_readout_char)
3633:E61A  shl dx, 1
3633:E61C  les di, ptr [0x1DB0]        ; SAME table pointer
3633:E622  les ax, ptr es:[bx+di]      ; SAME "read a far pointer per character" step
```

Its stroke-fetch loop (`L_EE6C3`/`L_EE62E`, physical `0xEE62E`-
`0xEE705`) fetches bytes from that far pointer one at a time until a
`0x00` terminator (identical shape to `draw_readout_char`'s own loop,
down to checking the *same* `[0x1CC4]+0x1400` = 5120-byte wraparound
boundary `plot_readout_point` uses), and for each stroke byte extracts
`fine = byte & 0xF` and `coarse = (byte & 0x70) >> 4` using the
identical mask-and-shift sequence (`and dx,0x70` / `sar dx,1` x4) as
`draw_readout_char`. This isn't a coincidental byte-pattern match -
it's the same formula, applied to the same source table, independently
implemented a second time.

**What it does differently**: instead of calling `plot_readout_point`
to append into the display list via the `[0x1AF4]`-tracked write
cursor, this function writes `fine` and `coarse + [0x46E]` (a running
accumulator, **added** directly, not `draw_readout_char`'s fixed
per-character `baseline`) into a buffer via the `[0x45E]` pointer
(already documented elsewhere as "a saved copy of the readout buffer's
own base pointer `[0x1CC4]`" - see `init_print_region_home`/`init_
selftest_report_screen` in `FUNCTIONS.md`), tagging each with the same
delimiter-marker value (`2`) `mark_readout_delimiter` uses. `[0x46E]`
gets incremented elsewhere (physical `0xEC706`/`0xEC71B`) by the return
value of a call to `0xF6510`, made with a byte argument that's a
character code at one of its two call sites. **Correction, checked
right after first writing this up**: `0xF6510` is *not* a
character-width function - reading its actual body shows a min/max
bounding-box clamp against a `0xFFF` (4095) limit over a `×14`-stride
record table (fields at `+0x576`/`+0x578`/`+0x580`), structurally
unrelated to characters or the stroke font. What `[0x46E]` really
accumulates, and why one of the two calls into `0xF6510` passes a
character byte, is genuinely unresolved - don't trust the earlier
"character-width measurement" framing if it's echoed anywhere else.

**This function - and the giant function it's embedded in,
`FUNC_3633_DF56` (`0xEDF56`) - are both heuristic-only, `ref_count: 0`
in the symbol table: no confirmed caller was found.** So this is a
real, structurally solid second consumer of the stroke-font table, not
a landing-artifact coincidence - but not proven to execute on real
hardware. Worth noting for anyone who names it later: don't confuse it
with the *other* label found nearby, `dispatch_item_handler_if_enabled`
(`0xEDFFD`, already documented in `FUNCTIONS.md`) - that label sits on
a landing artifact (`sub ax,[bx]; or ah,dh`, 3 bytes before its real
semantic start at `0xEE004`), and its own already-confirmed mechanism
(the `[0x1D10]` per-item handler table, called from `tag_position_
marker_and_dispatch` at `0xF5D89` in `160-3532`) is unrelated to the
stroke-font reading found here, even though both sit inside the same
outer function and were easy to conflate mid-trace.

**Still not found**: the actual HPGL-scale/output stage (this function
doesn't reveal a `×4` factor either - `fine` and `coarse` are still
combined by raw addition here, just with a running accumulator instead
of a fixed baseline), and neither this function nor its containing
`FUNC_3633_DF56` have a confirmed caller to trace forward from. If
picked up again: figure out what `[0x46E]` actually represents (its
real feeder isn't a width lookup, per the correction above), and try
find_landing_artifacts.py-style caller-count ranking on far calls into
this whole address neighborhood (`0xEDF56`-`0xEE705`) to see if
anything real reaches it despite the heuristic scanner's `ref_count: 0`.

## A separate candidate vector shape table, `160-3633` `0xAE64`-`0xB061` - not the same table as this glyph hunt

Found 2026-09-15 while investigating `UNKNOWN_DATA.md`'s exported
blocks, not by continuing this specific hunt - flagged here for
cross-reference since it uses the same pen-bit drawing convention
traced above (bit 7 = pen up/down), just with a wider 2-byte-per-axis
coordinate instead of the packed nibble format. Rendered with a new
tool, `disasm/decode_vector_icons.py` - committed output in
`docs/display/vector-icons/`. Whether it's a small UI icon set or a
different, rougher font is genuinely unresolved (rendering the shapes
and looking at them didn't settle it either way - see
`docs/decode-anomalies/unknown-data-deep-dive-2026-09-15.md` finding 3
for the full reasoning). This address range was never a candidate in
the stroke-font search above, and no code has been found calling into
it either - it's a second, independent open question, not a
resolution of this one.
