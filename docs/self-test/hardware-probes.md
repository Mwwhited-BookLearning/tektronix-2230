# Self-test: ADC/measurement hardware probes

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Possible ADC/measurement self-test hardware

Found while renaming the `SUB_E12F4`/`SUB_E2DC9`/`SUB_E0DCC` cluster
(`160-3633`) - flagged as a lead at the end of the previous renaming
session. These three, plus `selftest_init_channel_hw` (`0xE2AB0`) and
`clear_selftest_status_flags` (`0xE0E56`) from earlier, all read/write
a shared set of "hardware register" variables via far pointers:
`[0x322]`, `[0x326]`, `[0x32A]`, `[0x32E]`, `[0x336]`, `[0x33A]`, plus
the `[0x31E]`-based (physical `0x48000`) scratch buffer used as a
lookup table.

`configure_measurement_hw` (`0xE0DCC`) writes 5 caller-given parameters
into this register cluster (including two *reverse-indexed* lookups -
`0x1000 - param` and `0xFF0 - param` - into the `[0x31E]`/`[0x32E]`
buffers, suggesting a calibration/reference table addressed from its
end). `run_adc_selftest` (`0xE12F4`) then polls `[0x322]` for a busy
bit (`0x8000`) with a timeout, reads a **12-bit result** (mask `0xFFF`)
once the busy bit clears, and compares it against a reference value -
classic ADC status/data register shape (busy flag + N-bit result in
the same word). `wait_stable_measurement` (`0xE2DC9`) is a sibling that
instead waits for a byte at `[0x32A]` to stop changing across
consecutive reads (a debounce/settle pattern), then checks two more
status bits (`0x2000`, `0x4000`) in `[0x322]`.

**Working hypothesis**: this is the firmware's generic **ADC-based
self-test primitive**, reused across several of `self_test_dispatcher`'s
still-unidentified subroutines (`SUB_E3F2C`, `SUB_E3F99`, etc. are good
candidates to check next - if any of them call `configure_measurement_
hw`/`run_adc_selftest` directly, that would both identify them and
narrow down which physical measurement each one performs). **Not
confirmed**: which physical ADC/ADC-mux this addresses, or whether
`[0x322]`'s bits 13/14 (checked by `wait_stable_measurement`) mean
"calibration valid"/"lock detected" (plausible for a timebase PLL) or
something else entirely.

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
