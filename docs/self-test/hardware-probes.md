# Self-test: ADC/measurement hardware probes

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Acquisition-memory self-test hardware (was: "possible ADC/measurement" - renamed, see correction below)

Found while renaming the `SUB_E12F4`/`SUB_E2DC9`/`SUB_E0DCC` cluster
(`160-3633`, now `run_adc_selftest`/`wait_stable_measurement`/
`configure_measurement_hw`) - flagged as a lead at the end of the
previous renaming session. These three, plus `selftest_init_channel_hw` (`0xE2AB0`) and
`clear_selftest_status_flags` (`0xE0E56`) from earlier, all read/write
a shared set of "hardware register" variables via far pointers:
`[0x322]`, `[0x326]`, `[0x32A]`, `[0x32E]`, `[0x336]`, `[0x33A]`, plus
the `[0x31E]`-based (physical `0x48000`) scratch buffer used as a
lookup table.

`configure_measurement_hw` (`0xE0DCC`) writes 5 caller-given parameters
into this register cluster (including a reverse-indexed (`0xFF0 -
param`) word write into `[0x32E]`, and a reverse-indexed *read* -
never a write - of one byte from the `[0x31E]`-based buffer). `run_adc_
selftest` (`0xE12F4`) then polls `[0x322]` for a busy bit (`0x8000`)
with a timeout, reads a **12-bit result** (mask `0xFFF`) once the busy
bit clears, and compares it against a reference value. `wait_stable_
measurement` (`0xE2DC9`) is a sibling that instead waits for a byte at
`[0x32A]` to stop changing across consecutive reads (a debounce/settle
pattern), then checks two more status bits (`0x2000`, `0x4000`) in
`[0x322]`.

**Register identities: CONFIRMED, 2026-10-09**, by reading `init_
selftest_register_group`'s (`0xE4443`) body directly and confirming it
has exactly **one call site** (`run_selftest_sequence`, always with
`group=1`), so these far pointers are fixed once for the entire
self-test run, to:

| Far ptr | Physical | Identity (Table 3-1) |
|---|---|---|
| `[0x31E]` | `0x48000` | scratch buffer (not yet in Table 3-1) |
| `[0x322]` | `0x4377E` | **Acquisition Memory Address Buffer Low bits U3427** |
| `[0x326]` | `0x437BE` | Acquisition Mode Register U3310 |
| `[0x32A]` | `0x437F7` | (not yet in Table 3-1) |
| `[0x32E]` | `0x437EE` | Record Counter U4115/U4116/U4117 |
| `[0x332]` | `0x437DE` | B Delay Timer U4123/U4124 |
| `[0x336]` | `0x407DE` | (not yet in Table 3-1) |
| `[0x33A]` | `0x407EE` | (not yet in Table 3-1) |

**This resolves the "which physical ADC" question this file asked for
a long time: there isn't one.** `[0x322]` is the *exact same physical
register* (`0x4377E`, U3427) that `verify_adc_calibration` compares for
the real `ACQ_AB` address-bus-walk self-test (see `FUNCTIONS.md`'s
`0xE2FFC`/`step_acq_ab_addr_walk` entry and `dispatcher-and-siblings.
md`). So `run_adc_selftest`/`wait_stable_measurement` aren't a generic
"ADC self-test primitive" at all - they're reading back the
**acquisition memory address buffer**, exactly like `ACQ_AB` does, just
with different expected patterns. That's also why the failure message
literally says `"acq_mem cntr"` (acquisition-memory counter) - a label
this project had been treating as an arbitrary string, not yet
connected to the register's real identity.

**Correction to an earlier (incorrect) correction**: a prior session's
`FUNCTIONS.md` edit claimed `selftest_front_panel_adc` (`0xE296E`,
references the string `FP_a2d`) "identifies the peripheral behind"
this exact cluster. Re-reading `selftest_front_panel_adc`'s full body
this session shows that's wrong - it never calls `configure_
measurement_hw`/`run_adc_selftest` and never touches `[0x322]`/
`[0x326]`/etc. at all; it calls `selftest_init_channel_hw` (its own
separate, interrupt-driven `[0x1D20]`-based register) three times
instead. The "front-panel A/D converter" identity belongs to *that*
different cluster, not this one - the two were mistakenly merged.
`FUNCTIONS.md`'s `0xE296E`/`0xE12F4` entries have been corrected to
undo this.

**`[0x31E]`'s physical identity is also confirmed** (`MEMORY_MAP.md`'s
`0x48000-0x4BFFF` row, from Table 3-1): it's genuine **Acquisition
Memory - 4 images of Acquisition RAM U3418/U3419**, not a
counter/PROM as this file originally speculated once the read-vs-write
asymmetry was found. Since it's real RAM (not a register with
hardware-driven content), `configure_measurement_hw` not writing the
ramp `verify_pattern_with_report` expects means **something else must
fill it** - either real acquisition hardware during a capture cycle
(DMA-like, outside firmware control) or firmware code in this call
chain not yet traced. Not yet found which. Also still open: whether
`[0x322]`'s bits 13/14 (checked by `wait_stable_measurement`) mean
something specific to the address-buffer register (e.g.
"latched"/"settled") beyond the busy flag `run_adc_selftest` already
uses.

**Confirmed live in the emulator, 2026-10-09 - traced the `HS_ACQ`/
`TBD hs/2` self-test failures found after fixing `ACQ_AB`** (see
`emulator/docs/design.md`'s own section and `changes/2026-10-09.md`)
to their exact source, now explained by the register identities above:

- `HS_ACQ` is `selftest_hs_acq` (`0xE28FE`) calling `run_adc_selftest`
  then `verify_pattern_with_report`. `run_adc_selftest`'s mismatch
  message (`"acq_mem cntr %d <> %d"`) is built from the live `[0x322]`
  (physical `0x4377E`, U3427) 12-bit field (actual) vs. `[bp+0xc]+
  [bp+0x16]` - two of `selftest_hs_acq`'s own literal call-site args
  (`0xa2+9=0xAB`) - hand-confirmed bit-for-bit against the captured
  `HS_ACQ : acq_mem cntr 800 <> 0AB` line. The emulator reads back
  `0x800` because nothing drives `0x4377E` as real acquisition
  hardware would - it's whatever default value is sitting in that RAM
  cell.
- `TBD hs/2` comes from a *different* call path: `run_adc_selftest_
  range` (`0xE22AF`, itself called from `selftest_front_panel_switch_a`
  - see `dispatcher-and-siblings.md`'s existing hypothesis that this
  sibling is misnamed) sweeps `run_indexed_adc_selftest` over index
  0-8 against the 10-byte device table at `[0x1DCC]`; index 0's record
  name is `"hs/2"`, concatenated onto the literal `"TBD "` prefix
  (confirmed: this prefix string is hardcoded in `run_indexed_adc_
  selftest` itself, not a per-record flag - **every** device in this
  table that hasn't been given a real name in this firmware build
  reports as `"TBD " + name"`, not just this one). This also bottoms
  out in `run_adc_selftest`/`verify_pattern_with_report`, which is why
  its failure has the exact same `"acq_mem cntr"` / `"fill @"` shape as
  `HS_ACQ` - same unstubbed `0x4377E` default (`0x800`), different
  caller-supplied threshold (`<> 01A` this time).
- **Net effect for the emulator**: `[0x322]`'s physical identity
  (`0x4377E`, U3427) is now just as confirmed as `ACQ_AB`'s - in fact
  it's the *same register* `ACQ_AB`'s existing `io_stubs.
  AcqAbAddrWalkStub` already deals with. A coupling stub for `HS_ACQ`/
  `TBD hs/2` is no longer blocked on hardware identity; what's still
  missing is the real acquisition hardware's write-then-readback
  behavior for *this* read pattern (a plain 12-bit value + busy flag,
  not an address-walk), and - separately - `[0x31E]`'s real backing for
  `verify_pattern_with_report`'s ramp check. Tracked in `TODO.md`'s
  `emulator/` next-steps item.

**`acq_mem cntr` mismatch resolved, same day**: `io_stubs.
AdcSelftestReadbackStub` hooks the exact read instruction inside
`run_adc_selftest` (`0xE137A`) and writes `[bp+0xc]+[bp+0x16]` (read
live off the caller's own stack frame, not hardcoded) into `0x4377E`'s
low 12 bits before the read executes - the same formula confirmed
above, just computed generically instead of per-call-site. Because
`run_adc_selftest`'s comparison code is identical for every caller,
this fixes **both** `HS_ACQ` and `TBD hs/2` in one stub, without ever
needing `run_indexed_adc_selftest`'s own `[0x1DCC]` table decoded -
verified live: both `"acq_mem cntr"` mismatch lines are gone from a
full boot trace, with the busy-wait loop itself left untouched (RAM's
default-0 busy bit already matched every captured run). Confirmed via
an A/B run (same trace with/without the stub, `git stash`) that the
boot's eventual `halt_cpu` panic stop (`0xF1611`) happens identically
either way - a pre-existing, unrelated stopping point, not a
regression this stub introduced. The `verify_pattern_with_report`
"fill @" ramp-pattern mismatch (the `[0x31E]`/`0x48000` buffer's
real-hardware-fill question, above) is untouched by this stub and
remains the one open piece of this investigation.

## Possible waveform acquisition buffer init (updated: likely a plot-scale cache, not a buffer)

Found while renaming `160-3532:0x03F4`/`0x0414`/`0x0446` (all three
converge, via a tiny stub or a short print-then-fall-through, on the
same shared tail block at `L_F0678`). That block initializes **8
separate values to the identical value `0x800`
(2048 decimal)**: `[0x6F8]`, `[0x6F6]`, `[0x6F4]`, `[0x6F2]`, `[0x6E8]`,
`[0x6E6]`, `[0x6B4]`, `[0x6B2]`, plus a handful of other fixed values
(`[0x6FA]`-based byte `=0xD5`, `[0x700]`/`[0x702]=0`, `[0x704]=0x14`,
`[0x70E]=0`) and a call to `update_plot_position(0x200, 0x200)`.

**Updated interpretation** (a later session correctly identified the
neighboring `scale_and_plot_point`/`scale_and_plot_point_default`
pair, previously miscalled `divide_scale`/`divide_scale_default` -
they do fixed-point multiply-by-reciprocal-then-shift scaling, not
divide): `[0x6E6]`/`[0x6E8]` are exactly the two variables
`scale_and_plot_point` **caches its scaled point into** for drawing
the next line segment. Given that, this init block is much more
likely a **plot-position/scale cache reset to a midpoint default**
than a literal "waveform buffer" - `0x800` = 2048 is exactly the
midpoint of a 12-bit range (`0-4095`), consistent with `[0x322]`'s
confirmed 12-bit ADC value field (see `run_adc_selftest`) and a
sensible "no data yet, assume centered" starting value for a scaled
plot point. The other two pairs (`[0x6F8]`/`[0x6F6]`/`[0x6F4]`/
`[0x6F2]` and `[0x6B4]`/`[0x6B2]`) plausibly reset the *other* cached
points this rendering pipeline tracks (e.g. per-channel or per-axis
"last plotted point" caches), all to the same centered default.

**Still not confirmed**: the exact number/purpose of all 8 values
individually, or whether this is really scoped to acquisition/plot
rendering at all rather than something else (a GPIB/plot output buffer
set, for instance - the comm ROM code is
physically adjacent in the address space story but this is the MAIN
ROM, so that's less likely). Revisit if the self-test subroutines or
menu-string cross-referencing work ever turns up a direct link to
"record length" or a channel-buffer concept.

**Update:** the shared tail itself (`L_F0678`, reached via `SUB_F0446`'s
un-prologued "push es; jmp" stub) is now renamed
`reset_all_channel_plot_caches`. Its entry style (no `push bp`/`mov bp,
sp` of its own, relying on a frame already established by whatever
reaches it) is the same shared-tail pattern already confirmed harmless
for `convert_sample_value`.
