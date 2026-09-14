# The RAM far-pointer init table and its never-reached functions

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Found: a whole family of never-reached functions via the RAM far-pointer init table

Prompted by a direct question ("is there anything discoverable in the
uncovered ~9% - lost test code, calibration, wrong address mappings,
or compiled-but-never-called functions?"). Method: dumped the exact
byte ranges *not* reached by either the proven or heuristic passes
(`disasm/compute_coverage.py`'s underlying gap data), then manually
classified each gap as expected data (string tables, the `0xFF7B0`
message table, ROM headers, `0xFF`-padding) versus something that
deserved a closer look.

**The single biggest find**: `init_far_pointer_table_sysrom`
(`0xE5EAE`, renamed earlier this session) has never had its own
*embedded data table* actually decoded - it was renamed purely from
its 4-instruction driver loop. Doing that now: the table lives right
after the driver code at `0xE5ECD`-`0xE60B4` (488 bytes, previously an
undocumented "gap"), and decodes cleanly as **82 `(dest_offset,
far_ptr)` entries**, `ES=0x209` (i.e. real destination = physical
`0x209*16 + dest_offset`, range `0x2090`-`0x21F0`) - a genuine, large
RAM-resident far-pointer table initialized once at boot, entirely
separate from the low "flat" `DS=0` variable space (`[0x30]`, `[0x34]`,
etc.) that most of this project's tracked variables live in - **do
not confuse the two just because an offset number matches**.

Checking every target address against the proven+heuristic coverage
map found **15 addresses that are never reached by either method**,
and every single one decodes as coherent, non-garbage x86 (verified by
hand, not just "heuristically plausible"). Added all 15 as new
`ENTRY_POINTS` (`gen_disasm_x86.py`) and re-ran the recursive descent:
coverage grew from 13,510 to 14,799 proven instructions (+1,289, from
following real calls *out of* these 15 into 2 more previously-unseen
functions), still byte-identical end to end. This is a genuine,
verified expansion of the proven set, not speculation.

**What's actually in there**, by sub-cluster:

1. **A plot-position/pen-drawing family** (`0xF0A4A` through `0xF0C26`,
   ~7 entries packed into a ~300-byte span) touching the *exact same*
   variables as the already-confirmed `draw_pending_line_segment`/
   `reset_plot_home_or_acq` cluster (`[0x6BE]`/`[0x6BC]`/`[0x6C0]`/
   `[0x6C1]`), some calling the confirmed `update_plot_position`/
   `plot_line_to` directly, plus 2 more far pointers (`[0x1DB8]`/
   `[0x1DBC]`) adjacent to the still-unlocated stroke-font pointer
   `[0x1DB0]`. This is almost certainly **more entry points into that
   same multi-entry plot module** - the module is bigger than what the
   proven call graph alone revealed.
2. **A scale-factor family** (`0xF09C0`/`0xF09C6`/`0xF09EA`/`0xF0AAA`)
   computing ratios via `imul`/`idiv` against a cluster of variables
   (`[0x712]`, `[0x716]`, `[0x718]`, `[0x71A]`, `[0x71C]`, `[0x71E]`,
   `[0x722]`, `[0x724]`) and far ptr `[0x1DB4]`.
3. **Two clean, confidently-named new functions**: `merge_record_
   flags_if_changed` (`0xE9472`) and, via a further transitive call
   from an unlabeled region near `0xF9404`, `init_print_record_fields`
   (`0xFAD6E`) and `compute_print_cell_size` (`0xF9650`) - both fit the
   `build_print_record_3532` record layout precisely.
4. **A second, independent instance of the `SUB_EAC86` phenomenon**:
   `0xF173E` makes a completely unambiguous `LCALL` to physical
   `0xEA13B` - which, when read as bytes rather than assumed to be
   code, is the *start of a string table* (`"or POST\0Display
   formatting\0delta time or 1/delta_time\0..."` - already-catalogued
   text, matching `STRINGS.md`'s "on-screen help/description lines"
   section exactly).
   This is a **different call site, targeting a different address,
   than the original `SUB_EAC86`/`SUB_F5898` mystery** - finding the
   exact same "a real compiled LCALL lands on data, not code" shape
   twice, independently, is strong evidence for candidate explanation
   (b) from that original writeup (dead/never-actually-executed code
   left in the build, gated by a condition that's always false in
   practice) over a decode-tooling bug, since a shared tooling bug
   couldn't produce two independent instances like this.

**Not (yet) resolved**: *how* code ever reaches these 15 addresses in
practice. No literal `mov reg, 0x209` (or any bracket reference into
the `0x2090`-`0x21F0` range) exists anywhere in the entire proven or
heuristic corpus - so either (a) something computes the segment/offset
indirectly through a chain of variables this project hasn't traced
yet, or (b) most of this table is genuinely dead initialization for a
feature/build configuration that doesn't run in practice, consistent
with finding #4 above. Given how cleanly the plot-position and scale-
factor clusters mesh with already-confirmed code, "genuinely used but
via an indirection this project hasn't found the trigger for" seems
more likely than "entirely dead" for those two clusters specifically -
worth revisiting if a future session finds where `[0x2090]`-family
addresses get loaded into a segment register.

**Practical takeaway for future sessions**: the other far-pointer init
tables in this codebase (the comm ROM's own `init_far_pointer_table`,
and any others found later) are worth this same treatment - decode the
*data*, not just the driver loop, and cross-check every target against
the coverage map. This one single table yielded 15 new entry points
and 2 more transitively-found functions from roughly an hour of
targeted analysis; it's a much higher-yield technique than reading
through the remaining un-prologued-entry cluster function-by-function.

**Follow-up, tried on the comm ROM's own `init_far_pointer_table`
(`0x80133`)**: decoded its embedded table too (31 entries, `ES=0x8FED`
base, terminated by a `-1` sentinel rather than a fixed count - a
different, slightly more general table shape than the sysrom one, and
called cross-ROM from both `0xE1F03` in the main ROM and `0x9629E` in
the comm-ROM alias). Unlike the sysrom table, **this one didn't yield
new functions** - most of its far-pointer targets in the `0x90000`
comm-ROM-alias range decode as outright garbage when force-disassembled
(`movntps xmmword ptr...`, `popaw`, `das` and other impossible/absurd
instruction sequences), and the handful that land on already-visited
addresses land *mid-instruction* inside existing code, not on a real
entry boundary. **Still a worthwhile check, for a different reason**:
3 of its entries write far pointers to `0x406F0` ("Option UART/GPIB
chips"), `0x4067C` ("Option Status Latch"), and `0x406BC` ("Option
Parameters Latch") - the *exact* addresses confirmed from the service
manual's Table 3-1 (see `MEMORY_MAP.md`) - independent confirmation,
from the comm ROM's own initialization code this time, that these are
real, intentional hardware-register pointers. 3 more entries forward
to plain flat-RAM cells (`[0x548]`/`[0x1B64]`/`[0x3E3]` under `DS=
0x41`), and one entry - `far_ptr=F1D8:0865` (physical `0xF25E5`) -
is the **exact same target** the sysrom table's own entry #54 points
to (see above), landing mid-instruction inside an already-covered main-
ROM function (`mov byte ptr [0x65F], 0x54`) in both cases. Two
independent far-pointer tables, in two different ROMs, agreeing on
this one specific byte is too precise to be coincidence, but what it
actually *means* isn't understood - flagged here rather than guessed.
**Conclusion**: not every far-pointer init table in this codebase is a
clean function-pointer array like the sysrom one turned out to be -
some (like this one) are a genuine mix of hardware-register pointers,
RAM-forwarding pointers, and what look like stale/non-code entries.
Worth the same "decode and check" treatment on any newly-found table,
but don't expect every one to pay off the way the sysrom table did.
