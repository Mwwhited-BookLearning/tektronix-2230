# Decode anomalies: deliberate dual-entry-point byte sharing

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Resolved (partially): SUB_E90A5/SUB_E92B0 are call targets landing 1 byte into a "mov di, tag" instruction

An earlier session's investigation of `SUB_E90A5` and `SUB_E92B0` (both
called via genuine `lcall`, both opening with the same
capstone-decoded-as-garbage bytes `1f 00 57 9a ...` / `00 57 9a ...`)
is now explained, at least mechanically. In both cases, the call
target address is exactly **one byte past** the start of a `mov di,
TAG` instruction (`BF <tag_lo> 00`, 3 bytes) that a separate,
already-understood fallthrough/jump path executes normally as part of
the same repeating pattern used throughout `assert_and_halt`: `mov di,
TAG; push di; lcall convert_sample_value; push di; push ax; lcall
halt_cpu`. Landing 1 byte in means the CPU instead starts decoding
from the immediate's low byte (which, for tag values `< 0x100`, equals
the tag itself) and the always-zero high byte, producing exactly the
"garbage" opcodes seen (e.g. `1f` = `pop ds`, `00 57 9a` = `add byte
[bx-0x66], dl`, etc). For `SUB_E92B0` specifically, tracing both
byte streams forward shows they **reconverge exactly at `0xE92B7`**
with identical subsequent bytes (`mov word [bp-0xa], ax`, the start of
the next `convert_sample_value` result store) - strong confirmation
this is a real, reproducible byte-level coincidence and not a
transcription error.

**Not fully resolved:** *why* something calls directly into what looks
like the middle of another check's tag-setup instruction, rather than
its intended start. Possibly these call sites intentionally reuse a
"skip the redundant tag load, the value we want is already what's
sitting in that immediate's low byte" trick, or possibly this is
itself a symptom of the same `0x0F`-style capstone-vs-real-8086
opcode-table mismatch documented for `SUB_F6382` interacting badly
with an unrelated coincidence here - not pursued further. Left
`SUB_E90A5`/`SUB_E92B0` unrenamed; they are not independent functions
worth naming, just artifacts of where a real call happens to land
inside `assert_and_halt`'s/`plot_scaled_point`'s tag-dispatch code.

**A third instance, found via `analyze_loops_vs_functions.py`'s
long-jump lens (see below): `L_EDA0A`, `SUB_F1581`'s tail-jump
target.** `SUB_F1581` (`push 0x5102; jmp` a huge `0x3B7A`-byte
displacement) lands at physical `0xEDA0A`, which is **2 bytes into**
`compute_and_draw_scale_marker`'s own `mov word [0x1BEC], 0x20`
instruction (`C7 06 EC 1B 20 00`, `0xEDA0A` = the `EC` byte of that
instruction's displacement field). Decoding from there produces `in
al, dx` (`0xEC`) followed immediately by a clean reconvergence with the
*real* next instruction at `0xEDA0E` (`jmp L_EDA28`) - the same
"landing mid-instruction, then cleanly reconverging a few bytes later"
shape as `SUB_E90A5`/`SUB_E92B0` above, not `SUB_EAC86`'s outright
incoherent garbage. This also **closes an old open thread** from an
earlier session (`TODO.md` used to note `SUB_F156E`/`SUB_F1581` land
"inside another unnamed, unexplored function in a way that suggests
cross-ROM code sharing, not yet reconciled") - it's not cross-ROM
sharing, it's this same landing-artifact class, now identified
precisely. Still not resolved *why* the compiled jump target is 2
bytes short of the real instruction boundary; still not renaming
`SUB_F1581`/`SUB_F156E`, and `L_EDA0A` itself isn't a real separate
routine - `compute_and_draw_scale_marker` (the function `L_EDA0A`
lands inside) was named on the strength of the other ~90% of its body,
which is completely coherent.
## A third decode anomaly: SUB_F6382, likely capstone misreading opcode 0x0F

`SUB_F6382` (`160-3532`, 3 far-call sites from `160-3633`) opens with
`0f 7e 05` -> capstone reads this as the SSE2/MMX instruction `movd
dword ptr [di], mm0`, impossible on an 8086/8088. Unlike `SUB_EAC86`
(pure garbage for many bytes with no coherent reconvergence - see
below), this one is a single misdecoded instruction: everything from
the byte right after it (`0xF6385` onward: `mov word [bp-0xa],
0x0f9a`) is completely ordinary, bp-relative compiled-C code, and the
function's tail (checking `[0x1B83]` against `0x14` - the same comm-
option-installed value from `detect_comm_option_hw`!) is entirely
coherent.

The likely explanation: on a real 8086/8088, opcode byte `0x0F` alone
is the **undocumented 1-byte `POP CS`** instruction, not a two-byte
SSE/MMX escape prefix (that reuse of `0x0F` didn't happen until the
80286). Capstone's 16-bit mode still decodes `0x0F` as a multi-byte
escape, so it's very likely misreading a real (if unusual/undocumented)
`POP CS` as `movd`. Support for this: decoding `0x0F` as a 1-byte
`pop cs` at `0xF6382`, the very next byte `0x7E` naturally starts a
fresh `jle` instruction at `0xF6383`, landing exactly on the same
`cmp byte [0x1B83], 0x14` block (`0xF638A`) that a separate fallthrough
path into this same code reaches via its own, differently-encoded
comparison - i.e. under the `pop cs` reading, both the far-call entry
and the fallthrough entry cleanly reconverge, which doesn't happen
under capstone's `movd` reading. (Whether a real `POP CS` immediately
after a far call is itself sound - it would clobber the just-pushed
return address - isn't resolved; possibly the routine's own `retf 4`
stack cleanup account for it, or this entry point is never actually
exercised on real hardware. Not pursued further.)

**Follow-up: renamed anyway.** The single misdecoded opcode doesn't
block understanding the rest of the function, which is completely
coherent - `SUB_F6382` is now `draw_marker_box_and_update_position`
(and its sibling `SUB_F635E` is `draw_wide_marker_box`): together they
write a small box (`+/-0x32` or `+/-0x1FF`) into a far-pointer record
centered at `[bp-0xA]`, and under a specific `[0x1B83]==0x14`
condition also copy a matched field into position globals `[0x5C8]`/
`[0x590]` with change flags. **Caution**: don't assume `[0x1B83]==
0x14` here means anything cursor/measurement-related just because the
"update position" shape resembles a cursor update - `[0x1B83]` is
`detect_comm_option_hw`'s result value (0x14 vs 0x1E), and the already-
named `draw_display_test_pattern` gates on the exact same `==0x14`
value for an unrelated interrupt-line test pattern. What `[0x1B83]==
0x14` specifically enables in *this* function isn't confirmed - see
`FUNCTIONS.md`'s hedged wording. Distinct from `SUB_EAC86`: that one
decodes as incoherent garbage for many consecutive instructions with
no clean reconvergence at any nearby byte shift, while this one is a
single misdecoded opcode immediately followed by clean code - which is
exactly why it was safe to name despite the anomaly.
## A second, more puzzling decode anomaly: SUB_EAC86

Found while renaming: `SUB_EAC86` (`160-3633`, in the *proven*, not
heuristic, set) decodes as unambiguous garbage from its very first
byte - `add bl,al` / `int1` / more nonsense, eventually including
`minps xmm4,xmm4` (an SSE instruction, decades newer than anything
this hardware could execute). This is a **different, more serious**
case than the already-documented `0xEA1A0-0xEA615` fallthrough cluster
above:

- It's **outside** that cluster's address range entirely.
- It's reached via a **clean, unambiguous far call** (`lcall 0xEA34:
  0x946`, computing to physical `0xEAC86` with no ambiguity), not
  fallthrough - and the *same* literal `(0xEA34, 0x946)` target is
  called from **4 separate places**: 3 times in `160-3532` (all inside
  one small routine near `0xF58B1`) and once from the comm ROM
  (`2998_alias_90000`, inside `SUB_924D2`/`build_comm_status_message`).
  A shared target called this consistently from two different ROMs
  strongly implies it's supposed to be a real, working function.
- **New: the calling convention is now fully understood**, even though
  the bytes at the target still aren't. All 4 call sites push the
  identical shape of arguments - two far pointers, `(dest_far_ptr,
  src_far_ptr)` (`push es; push offset` twice) - consistent with a
  small "append/copy a chunk from src to dest" utility. In `160-3532`,
  `src` is always the fixed string-table segment `0xFF7B` at offsets
  `0x20F`/`0x221`/`0x233`, each exactly `0x12` (18) bytes apart; reading
  those bytes shows **not printable text** but a repeating pattern of
  small negative-looking bytes (`0xF8`/`0xFB`/`0xFA`) terminated by
  `0xFF`, then pairs starting with `0x07` - the same shape as the
  already-documented vector/stroke-font draw-command data used by the
  readout display list (see "a second, separate print mechanism"
  below) - immediately followed in the table by the plain ASCII text
  `" TEKTRONIX  2220"`. This strongly suggests the caller is assembling
  a **boot-splash "TEKTRONIX" logo** by appending 3 stroke-data chunks
  in sequence (each call advances the destination offset by `0xE`=14
  bytes), i.e. `SUB_EAC86`'s *intended* role is something like
  `append_stroke_or_string_chunk(dest, src)`. This makes the "it's
  supposed to be a real function" case even stronger, but doesn't
  explain why its actual bytes are garbage - still not resolved.
- Checked for an off-by-a-few-bytes misalignment (the classic
  "recursive descent walked into the middle of an instruction"
  failure mode) by dumping the raw bytes a few positions before and
  after `0xEAC86` - no nearby shift produces a clean `55 8B EC`-style
  prologue either. The bytes genuinely look like non-code data at
  every alignment checked.

**Not resolved.** Left `SUB_EAC86` unrenamed rather than guess a
purpose for what might not be reachable code in practice - naming it
would imply confidence that isn't there. Candidate explanations, none
confirmed: (a) another address-decode alias like the confirmed
`0x90000` one, where the real hardware serves different bytes at this
physical address than our flat EPROM dump does; (b) genuinely dead
code (a stale far-call target left over from an earlier build) that
happens to still be called 3x but never actually executed in practice
(e.g. gated by a runtime condition that's always false); (c) some
other decode subtlety not yet identified. Worth revisiting if the
`0x90000`-alias-style brute-force technique (documented in
`CLAUDE.md`) ever gets pointed at this specific address.

**Follow-up: `SUB_F5898`'s sole caller is now named.** `0xED7F1` is
inside `draw_boot_splash_and_option_icon` (`SUB_ED7DF`, renamed),
called once from the comm ROM's own boot sequence (`0x839E3`) - direct
confirmation this really is boot-time splash-screen code, not a
speculative label. That function also draws a second, position-
computed graphic via `SUB_F6382` right after the splash logo, gated by
a flag byte - likely an installed-option indicator icon alongside the
"TEKTRONIX" logo.

**Follow-up while renaming other functions in the same neighborhood:**
both `SUB_F5898` (`160-3532`, the "small routine near `0xF58B1`"
mentioned above, sole caller `0xED7F1`) and `SUB_E97DC` (`160-3633`,
called from `SUB_F5898` and from `0xF4294`) exhibit the *same* anomaly
shape as `SUB_EAC86` itself, not just proximity to it:

- `SUB_F5898` opens with `les ax, ptr [si]` and immediately uses
  `[bp-0xC]`/`[bp-0xA]` as locals - with no `push bp; mov bp,sp`
  anywhere in the proven listing above it, and no stack-passed
  arguments read via a positive `bp` offset despite being reached via
  a genuine far call (`0xED7F1`) that pushes a far pointer. It reads
  as though `bp` and `si` are expected to already hold a caller-
  established context (a "pass a struct via register instead of the
  stack" convention) rather than as a self-contained function.
- `SUB_E97DC` is even more pointed: it opens mid-copy-loop
  (`movsw`/`add si,N`/`loop`) with no setup for `cx`/`si`/`di`/`ds` at
  all, and physically sits inside a *cluster* of near-identical
  copy-loop variants (strides of 2/3/4/6 bytes, separated by `nop`
  padding) starting around `0xE97BC` - i.e. it is one of several
  fixed offsets into a shared block, selected by the caller computing
  the exact entry matching its own field width, and that block itself
  references `[bp+0x13]` (line `0xE97D6`: `cmp byte [bp+0x13],0`)
  despite no visible prologue establishing `bp` anywhere nearby either.
- Both are reached via **clean, unambiguous far calls** (not
  fallthrough-into-garbage), and both are internally coherent, valid
  8086 instructions the whole way through - unlike `SUB_EAC86`'s
  outright garbage. This rules out a decode-drift explanation for
  *these two*, but reinforces that this whole boot-splash-adjacent
  code region uses a register/`bp`-passing convention this project's
  tooling doesn't model (we only track stack-passed far-call args).

**Not renamed**, for the same reason as `SUB_EAC86`: the mechanism
(what's actually in `bp`/`si` on entry) isn't understood well enough
to state a purpose with confidence. Recorded here rather than guessed,
since it strengthens the case that `0xEAC86`'s neighborhood is a
single coherent (if unusual) subsystem - the "TEKTRONIX" boot-splash
renderer - built around an implicit-register calling convention, not
three unrelated anomalies.

**The phenomenon is not confined to the boot-splash neighborhood.**
While working through the remaining unnamed proven-set functions,
several more clean far-call targets turned up opening with an
instruction that doesn't make sense as a true entry point (no
`push bp; mov bp,sp`, and/or an operand referencing a register that
nothing sets up beforehand), splitting into the same two buckets:

- **Outright garbage** (same bucket as `SUB_EAC86`): `SUB_E99DF`
  (`160-3633`) opens `int1` / `add cl,dl` / `ljmp 0x830A:0x5689` -
  that far jump target isn't a sane address either. Called once, from
  `0xF42AB`. Its own body calls `SUB_EADA0` (`call`, near, from deep
  inside the garbage), which itself decodes as a long run of bare
  `add ax, <imm>` instructions - technically valid opcodes, but with
  no other instructions mixed in, indistinguishable from data that
  happens to disassemble as `add ax,imm16` (opcode `0x05`). Neither
  renamed.
- **Coherent code, ambiguous entry** (same bucket as `SUB_F5898`/
  `SUB_E97DC`): `SUB_E8E03`/`SUB_E8E29` (adjacent, `160-3633`, called
  from `0xF0CD7`/`0xE7E5D` respectively - the second is likely reached
  by fallthrough from the first too) call real, already-confirmed
  functions (`handle_gpib_device_clear`, `scale_and_plot_point`) with
  sensible-looking logic once past the first instruction or two.
  `SUB_ED9BC` (called from `0xE6740`) and `SUB_EEA58` (called from
  `0xE8E20`) are the same shape, and `SUB_ED9BC` in particular falls
  through into the `[0x1BEC]` scale-value clamp code documented
  elsewhere (the same clamp region `SUB_F1581` tail-jumps into via
  `L_EDA0A` after pushing an unconsumed-looking constant `0x5102`) -
  i.e. the acquisition/plot scale-clamp subsystem has its *own*
  cluster of these, separate from the boot-splash one.

None of these six were renamed. Taken together with the boot-splash
cluster, this now looks like a **project-wide convention** (values
passed in registers/`bp` across certain far calls, established by
matching caller/callee code this tooling doesn't model) rather than a
one-off anomaly - worth keeping in mind before assuming any remaining
odd-looking function opener is decode drift.

**Refined hypothesis: `bp` may be a persistent per-task/per-step
context pointer, not a fresh stack frame, for this whole class.**
Found more instances in the self-test/hardware-output area:
`SUB_F750A` (called via fallthrough from `L_F7504`, itself inside
another un-prologued block) reads/writes `[bp-8]`/`[bp-0xA]`/`[bp-0xC]`
freely, calls the confirmed `write_hw_shift_register`, and touches
`[di+0x550]` - the same per-task/per-item flag word `SUB_F0C2A`'s
family also touches. `SUB_F7603` (called from `0xEF4FB`, a clean far
call, no fallthrough predecessor - genuinely reached "cold") likewise
uses `[bp-8]`/`[bp-0xC]`/`[bp-0xE]`/`[bp-0x12]` from its very first
instruction with no setup. Since `SUB_F7603` is reached with no
fallthrough predecessor at all, its `bp` **must** already be valid
before the call - i.e. some enclosing/parent function set up a frame
once and then makes several `lcall`s into different "step" entry
points that all share and mutate that *same* frame's locals, rather
than each far call getting a fresh one. This would explain every
instance found so far (boot-splash chunk-appenders, copy-loop-stride
variants, scale-clamp helpers, and now these hardware-output steps):
they're not broken functions, they're **secondary entry points into an
already-open stack frame**, called by something that intentionally
never returns until all the steps finish. Not yet proven (would need
to find and read the actual enclosing frame-owner for one of these),
but a much better-fitting explanation than a tooling/decode bug at
this point - worth pursuing before assuming any single instance is
unique.

**Partial counter-evidence found immediately after writing the above**:
traced `SUB_F750A`'s and `SUB_F7603`'s actual caller (`0xEF467`-ish, in
`160-3633`) and both are called as **ordinary sibling subroutines** -
regular pushed args, `add sp,N` cleanup after each - from the same
enclosing function, not as sequential steps of one un-returning
sequence. That weakens the "one open frame, multiple step entries"
theory as literally stated. Raw-byte-verified (via a direct Python read
of `160-3532-14.bin`) that there is genuinely no `55 8B EC`-style
prologue hiding at either address - the compiled bytes really do start
mid-body. Still unexplained: **most likely explanation now is some
compiler/toolchain frame-sharing optimization for tightly-coupled
static helper functions** (each "helper" reuses its caller's `bp`
layout by convention, established at compile time rather than
literally sharing one runtime frame across multiple calls) - but this
is speculation, not confirmed. Still not renaming any of these.

**Follow-up (now with `SUB_F750A`/`SUB_F7603` both named): this
"frame-sharing helper" theory now has a plausible mechanical basis.**
Both end with a bare `retf` (no immediate, i.e. **caller-cleans-
stack**), the opposite of the callee-cleans `retf N` convention used
almost everywhere else in this codebase (including `SUB_F5D89`,
called from the very same neighborhood, which *does* end `retf 2`).
Their shared caller (`0xEF467`-ish) calls both with a completely
ordinary `push`-args-then-`lcall` sequence - no special `mov bp, ...`
trick is visible at the call site. Put together, the likely
explanation: these functions were originally part of their caller's
own body (sharing its `bp`-relative locals directly, which is why
their `[bp-N]` references only make sense in light of the *caller's*
stack layout, not a fresh frame), and the compiler's code-size
optimizer "outlined" them into separately-callable units - skipping
frame setup/teardown since it could prove they're only ever invoked
from that one context with a guaranteed-compatible frame, and leaving
stack cleanup to the caller for exactly the local scratch space that
would need it. This would explain every symptom found in this cluster
(no prologue, `bp`-relative addressing that "just works" only from
specific callers, mixed `retf` conventions) without needing self-
modifying code, a decode-tooling bug, or literally-shared open frames
across independent `lcall`s. **Still not proven** (would need to find
the caller's own frame layout and show the offsets genuinely alias its
locals) but it's the most coherent explanation yet, and was enough
supporting context to confidently name both `SUB_F750A`
(`sync_shift_register_output`) and `SUB_F7603`
(`apply_pending_position_delta`) - see `FUNCTIONS.md` - even without a
conventional prologue.
## `SUB_EAC86` fully resolved: it's the *same* non-code data blob as the `SUB_F173E`→`0xEA13B` finding above

Following up on finding #4 above (`SUB_F173E`'s `LCALL` landing on a
string table at `0xEA13B`), traced how far that non-code region
actually extends - and it turns out to be much bigger than the
already-documented `0xEA1A0-0xEA615` heuristic "decode-drift cluster"
(see "The main ROM has a heuristic layer too"), and it directly
contains the project's oldest, most-discussed anomaly.

**The full extent of the non-code region, confirmed two ways**:
- The proven symbol table has **zero labels of any kind** between
  `L_EA697` (the last address the proven walk's own garbage-decode
  fallthrough reaches) and the next real call target, and between that
  and `snapshot_index_and_format_number` (`0xED0AE`) - a ~9.7KB span
  with nothing proven in it except call targets that themselves decode
  as garbage.
- The heuristic push-bp scanner independently confirms where **real
  code actually resumes**: nothing resembling a function start exists
  anywhere in this span until `0xEB132`, where a completely ordinary
  `push bp; mov bp,sp; sub sp,0xa` prologue appears out of nowhere.
  So the true non-code region is `0xEA13B`-`0xEB131` (**~4083 bytes**),
  not the ~1655 bytes originally estimated.

**What's actually in those ~4083 bytes**:
1. `0xEA13B`-`~0xEA5E6` (~1200 bytes): the already-catalogued readout
   help/description text (`"Points before trigger, PRE or POST"`,
   `"Display formatting"`, ... through `"CENTER POS:PRESS CURSOR
   SEL:ADJ BAL"`) - confirmed exact string-for-string match against
   `strings_160-3633.json`/`STRINGS.md`.
2. `~0xEA5E6`-`0xEB131` (~2890 bytes): **not text** (mostly non-
   printable bytes) but also not random garbage - it has a
   suspiciously regular structure: paired byte values where one side
   of the pair increments steadily while the other forms a symmetric
   zigzag (e.g. `03,02 / 04,01 / 05,00 / 06,01 / 07,02 / 08,03 ...`),
   and elsewhere a run of 16-bit little-endian values stepping by
   `0x100` each entry. This is genuine structured data, not decode
   noise - originally flagged as a real lead for the still-unlocated
   stroke-font glyph table.

   **Tested and NOT confirmed (2026-09-13)**: decoded `draw_readout_
   char`'s exact bit-packing formula precisely by re-tracing `plot_
   readout_point`'s argument order (`[bp+6]`=coarse-component-derived
   Y, `[bp+8]`=fine-nibble X, `[bp+0xa]`=pen attribute, matching the
   already-documented mapping) and rendered this candidate region as a
   grid of "glyphs" (splitting on `0x00` bytes as the stroke terminator,
   per the documented format). Result: **405 fragments averaging only
   ~5 bytes each** (way more, and much shorter, than the ~96-128
   entries a real font would have) and every rendered shape is a
   generic, repetitive angular "hook"/checkmark - no recognizable
   letterforms at all. This is a genuine negative result, not just an
   inconclusive one: `0x00` appears far too often in this region to be
   acting as a per-character terminator, meaning it's much more likely
   this is the previously-noted "paired incrementing/zigzag" numeric
   table (whatever it actually is) and **not** the stroke-font data
   after all. Downgraded from "real lead" to "checked and probably
   wrong" - the stroke-font table's true location is still unknown.

**`SUB_EAC86` and `SUB_EAD08` are both confirmed to land inside this
exact region**, called from genuinely legitimate compiled code:
- `SUB_EAC86` (`0xEAC86`): the original, most-discussed anomaly in this
  project (see "A second, more puzzling decode anomaly" above) - its 4
  real external callers (3 in `160-3532`, 1 from the comm ROM) are
  unaffected by this finding, but the "why is the target garbage" part
  of that open question is now answered: it's not a decode-tooling bug
  or an address-decode alias, it's a real `LCALL` landing inside this
  same non-code blob, exactly like `SUB_F173E`→`0xEA13B`.
- `SUB_EAD08` (`0xEAD08`): a **4th confirmed instance**, found this
  pass - a genuine, unambiguous `lcall SUB_EAD08` from `0xE6BD0` (real,
  ordinary compiled code, immediately followed by a normal epilogue
  that doesn't check any return value).
- `SUB_EADA0` (`0xEADA0`) is a weaker, different case: its only
  "caller" (`call SUB_EADA0` at `0xEACBD`) is itself *inside* `SUB_
  EAC86`'s own cascading garbage decode - a coincidental `E8 E0 00`
  byte pattern within the data, not a genuine reference from real
  compiled code. Worth keeping this distinction: `SUB_EAC86`/`SUB_
  EAD08` are proven real call targets into data; `SUB_EADA0` is just
  another address inside the same data that happens to get
  auto-discovered as a side effect.

**This closes out the old `0xEA1A0-0xEA615` decode-drift-cluster TODO
item** (it undersold the region's true size by ~2.5x and mischaracterized
it as a mysterious missing function boundary) **and substantially
answers the long-open `SUB_EAC86` question**: it, and now 3 of its
siblings, are unambiguously real call targets into a real, identifiable
(if not 100% catalogued) non-code data region - the same "call lands on
data, not code" phenomenon as `SUB_F173E`, just discovered years... err,
sessions earlier, before the connection between the two was made. This
is now 4 independent confirmed instances (`SUB_EAC86`×1 target/4
callers, `SUB_EAD08`, `SUB_F173E`→`0xEA13B`, plus the still-uncertain
`SUB_EADA0`) of the same shape - strong, repeated evidence for dead/
stale call sites left in the shipped ROM rather than any kind of
decode-tooling artifact.
