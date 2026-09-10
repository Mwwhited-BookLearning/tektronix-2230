# Function list

One entry per routine whose purpose is understood (fully or partially),
across all three ROMs. This is the human-readable index; the
`.symbols.json` files under `disasm/` are the machine-readable one
(this file's `functional_name` should match the one recorded there).
Everything else decoded so far still carries an address-based
placeholder label (`SUB_XXXXX`/`L_XXXXX`) with no entry here yet — see
`disasm/sysrom_3532_3633.lst` / `disasm/160-2998-14.lst` for those.

Add an entry here (and the matching `functional_name` in the
`.symbols.json`) whenever a routine's purpose becomes reasonably clear
— even a partial/inferred understanding is worth recording, marked
"(guess)" where it's not confirmed.

## Main ROM (160-3633 / 160-3532)

The rows marked **renamed** are wired into `gen_disasm_x86.py`'s
`FUNCTIONAL_NAMES` dict, so the semantic name actually appears in
`sysrom_3532_3633.lst`/`160-3633-14.asm`/`.symbols.json` (both `name`
and `functional_name`) — not just recorded here. See that dict's
comments for the one-line version of the evidence; full writeups are
in `disasm/NOTES.md`.

| Address | Label | Purpose | Confidence |
|---|---|---|---|
| `0xE00B1` | `boot_init` **(renamed)** | Early boot: `cli`, clear a RAM buffer, set up the first stack (`SS:SP=4000:3FFA`), branch on a config/option byte at `cs:[bx+0x1AF]` | Confirmed (proven boot path) |
| `0xE5D1B4` | `ENTRY_E5D1_B4` | Continues boot: clears ES=DS=0x4000 RAM, reloads SP | Confirmed |
| `0xE5D1C7` | `ENTRY_E5D1_C7` | Checksums the low 512 bytes (`rep lodsw` x 0xFF words from `0:0`), installs INT1/INT2/INT255 (early) interrupt vectors, far-calls their handlers once, then jumps to `SUB_E6AAB` | Confirmed |
| `0xE6AAB` | `SUB_E6AAB` | First proper compiled-C-style function found (real BP frame). Looks like the main init dispatcher: calls several subsystem-init routines, zeroes 12-entry arrays, reinstalls INT255/INT2 (late) vectors | Confirmed reachable; purpose mostly inferred - not renamed yet pending more confidence |
| `0xE416F` | `print_selftest_banner` **(renamed; CORRECTED - was `self_test_dispatcher`)** | Prints the self-test "start"/"complete" banner lines (via `init_print_region`/`plot_readout_point`/`draw_readout_char`/`print_banner_line`/`close_print_record`) and sets `[0x1B10]=3`. **Contains no test calls at all** - the OR-fold test-dispatch pattern previously attributed to this address actually belongs to `0xE4244` (next row). Found by tracing what calls `SUB_E094B` and reading this routine end-to-end for the first time. See `disasm/NOTES.md` "self_test_dispatcher was misnamed" | Confirmed |
| `0xE4244` | `self_test_dispatcher` **(renamed; took over the name from `0xE416F` above)** | The real dispatcher: ~14 calls to per-subsystem test routines in sequence, each folding a return code into an accumulator at `[bp-0xA]`, returned in `ax`. One test (`SUB_E252A`) is conditionally skipped based on `[0x1B83]==0x1E`. Called from `0xE3DEE`, unconditionally (no `[0x1B10]` gate on this one - that gate belongs to `print_selftest_banner`'s caller instead). See `disasm/NOTES.md` "Found: the self-test dispatcher" for the full call list | Confirmed |
| `0xE094B` | `SUB_E094B` | Called immediately after `print_selftest_banner` (not `self_test_dispatcher` - corrected) returns. Loads a far pointer from `[0x1C80]` into `[0x1B56]/[0x1B58]` and writes a fixed 3-byte record (`3, 2, 0`) there - looks like initializing a small record/counter structure (capacity=3? item-size=2? count=0?) rather than logging a specific test's result, since it's called unconditionally with no test-result argument available at that call site | Purpose partially inferred, not confirmed enough to rename |
| `0xE097B` | `SUB_E097B` | Companion to `SUB_E094B` - operates on the same `[0x1B56]`-pointed structure, taking one byte argument. Compares/packs 4-bit nibbles (masks with `0xF0` twice) and either ORs a nibble into an existing slot or appends a new one, incrementing the structure's first byte (count?) each time. Only one call site found so far (from inside `print_selftest_banner`'s enclosing routine) - looks like a small dedup/insert operation into a nibble-packed set, not yet clear what values it's collecting | Purpose partially inferred, not confirmed enough to rename |
| `0xE44F1` | `check_comm_option_installed` **(renamed)** | **Comm/GPIB option-board presence + RAM/IO detection.** Checks for a valid ROM header checksum at `ES:DI` (hardcoded to `0x8000:4`, i.e. physical `0x80004` = the comm ROM's own BCD-revision+complement bytes), then tests writability at a *different* word, `ES:[DI+0xA000]` = physical `0x8A004`. Result packed into status byte `[0x1BF9]` (bit 1 = valid header, bit 2 = also RAM/IO-backed, gated on `[0x1B83]==0x1E` too) | Confirmed |
| `0xE0AF5` | `print_string_far` **(renamed)** | Loops a far-pointer nul-terminated string byte-by-byte, calling `print_char` per byte. Every observed call site passes a far pointer into segment `0xFF7B` (landing in `160-3532`), i.e. a fixed string table | Confirmed |
| `0xE0B2A` | `print_char` **(renamed)** | Thin wrapper: unpacks one byte argument, calls `write_readout_port_byte` | Confirmed |
| `0xE0B50` | `write_readout_port_byte` **(renamed)** | Writes the passed byte to the **fixed** physical address `0x40000+0x6F0` (never incremented) - see `disasm/NOTES.md` "The readout/CRT display memory" for why this is very likely a hardware port (character generator with an internal cursor), not plain RAM | Confirmed mechanism; hardware identity (readout/CRT character generator) is the best-fit inference, not yet confirmed against a schematic |
| `0xE39F0` | `append_readout_char` **(renamed)** | Appends a byte at the current write pointer of a running line buffer (`[0x1AF4]`), then writes a second byte at the same offset `+0x8000` - a dual-plane buffer in the same `0x4000` segment. See `disasm/NOTES.md` "The readout/CRT display memory" | Confirmed mechanism; which plane is "primary" vs "attribute/shadow" not yet confirmed |
| `0xFBC09` | `memcpy_far` **(renamed)** | Generic memcpy-style utility: `lds si,[bp+6]; les di,[bp+0xa]; mov cx,[bp+0xe]; shr cx,1; rep movsw`. Takes (src far ptr, dest far ptr, byte count) | Confirmed |
| `0x9470E` | `set_ds_return_old` **(renamed)** | `push ds; mov ds,[bp+6]; pop ax` - swaps `DS` to the caller-given segment word and returns the *old* `DS` in `ax`, so a later call with that saved value restores it. Called 40x (comm ROM side); every checked call site pairs a "swap in" with a matching "swap back" using the saved return value | Confirmed |
| `0xE31DC` | `strncat_far` **(renamed)** | `(dest far ptr, src far ptr, max_len)` - walks `dest` to its existing NUL terminator (decrementing `max_len` as it goes), then copies bytes from `src` until NUL or `max_len` is exhausted, re-terminates. A bounded string-append, i.e. `strncat` | Confirmed |
| `0xE323F` | `strncpy_far` **(renamed)** | `(dest far ptr, src far ptr, max_len)` - same shape as `strncat_far` but copies from `dest`'s *start* rather than finding its existing end first - a bounded string copy, i.e. `strncpy` | Confirmed |
| `0xE327F` | `format_number` **(renamed)** | `(value, radix, width, overflow_flag, extra)` - the core integer-to-ASCII formatter: divides by `radix` repeatedly, builds digits backwards into the fixed scratch buffer `[0x1B34]` (hex digits `>9` get `+0x37` for `A`-`F`), handles a leading `-`/space sign column and an `*` overflow marker, returns a far pointer to the resulting string. The likely engine behind this firmware's printf-style `%d`/`%c` formatting (see `STRINGS.md`) | Confirmed |
| `0xE3372` | `format_hex_word` **(renamed)** | `format_number` wrapper: fixed `radix=16, width=5` | Confirmed |
| `0xE3395` | `format_decimal_word` **(renamed)** | `format_number` wrapper: fixed `radix=10, width=6` | Confirmed |
| `0xE33B8` | `format_word_radix` **(renamed)** | `format_number` wrapper: caller-supplied `radix`, fixed `width=5` | Confirmed |
| `0xE34CB` | `format_byte_hex` **(renamed)** | Byte -> 2 hex ASCII digits + NUL, written to a *separate* fixed scratch buffer at `[0x1B4A]` (not `format_number`'s `[0x1B34]`) - a simpler, dedicated byte-to-hex formatter | Confirmed |
| `0xE35EF` | `init_print_record` **(renamed)** | Sets a caller-supplied print record's attribute byte and position: if given a `(0,0)` far-ptr override, defaults to the current global cursor `[0x1AF4]`; otherwise moves the global cursor to the given position first | Confirmed |
| `0xE3662` | `pack_row_col_bits` **(renamed)** | Bit-packs two 5-bit fields into a print record's bytes 1 and 3, preserving each byte's high 3 bits - a compact coordinate/attribute packing helper | Confirmed mechanism; exact bit-field semantics not fully mapped |
| `0xE36A5` | `set_position_record` **(renamed)** | Encodes two coordinates (each `>>3`, i.e. divided by 8 - character-cell granularity) into a print record's bytes 1-4 | Confirmed mechanism; the `>>3` strongly suggests pixel-to-character-cell conversion (8-pixel cells) |
| `0xE3520` | `build_print_record` **(renamed)** | Orchestrates `init_print_record` -> `set_position_record` -> `pack_row_col_bits` into one combined record-building call - the general entry point behind `init_print_region` | Confirmed |
| `0xE3567` | `init_print_region` **(renamed)** | `build_print_record` wrapper fixing the cell-size arguments to `0x10 x 0x10` (16x16) with no position override - the common "start a normal report line" case, used by `print_selftest_banner` | Confirmed |
| `0xE3766` | `mark_readout_delimiter` **(renamed)** | Writes attribute code `2` into the *next 2* readout-buffer cells' attribute plane and advances the cursor by 2, without writing anything to the primary plane. `2` is the same code `plot_readout_point`'s circular-buffer wraparound handler uses as its own marker - likely a reserved "delimiter" attribute throughout this buffer format | Confirmed mechanism; the marker's exact semantic role in whatever consumes this buffer isn't confirmed |
| `0xE374E` | `close_print_record` **(renamed; CORRECTED)** | Calls `mark_readout_delimiter`, then tags the caller's record (byte 0) with completion code `0x11`. **Correction**: earlier notes/pseudocode wrongly assumed this printed a string via its far-pointer argument (same shape as `print_string_far`) - it doesn't; it only tags the record. Fixed in `PSEUDOCODE.md` and `disasm/NOTES.md` | Confirmed |
| `0xE3736` | `close_print_record_b` **(renamed)** | Same shape as `close_print_record` but tags with `0x39` instead of `0x11` - a different record "kind"/completion code. Exact meaning of `0x11` vs `0x39` not confirmed | Confirmed mechanism; the two codes' meaning is a guess |
| `0xE3821` | `print_readout_string` **(renamed)** | Loops a far-pointer nul-terminated string, calling `draw_readout_char` per byte - the vector-display-list sibling of `print_string_far` (which instead writes straight to the `0x40000+0x6F0` hardware port) | Confirmed |
| `0xE3854` | `draw_readout_char` **(renamed)** | Looks a character up (`char & 0x7F`, `*4`) in a stroke-font table at `[0x1DB0]`, then walks each stroke byte of the resulting far pointer - extracting a pen-up/down bit (`0x80`), a coarse component (bits 4-6), and a fine component (bits 0-3) - calling `plot_readout_point_relative` once per stroke. The character rasterizer for the vector-stroke readout system; see `disasm/NOTES.md` "The readout vector display list" | Confirmed mechanism; the exact stroke-byte bit layout (which bits are dx vs dy) not fully decoded |
| `0xE3900` | `plot_readout_point_relative` **(renamed)** | `(dx, dy, attr)` - adds the offsets to the current base position (`[0x1AF8]`/`[0x1AFA]`) and calls `plot_readout_point` | Confirmed |
| `0xE3930` | `plot_readout_point` **(renamed)** | `(x, y, attr)` - THE fundamental primitive: updates the current position (`[0x1AF8]`/`[0x1AFA]`), appends a `(y, x)` coordinate pair plus a duplicated attribute byte to the readout vector display-list buffer at `[0x1CC4]` (tracked via `[0x1AF4]`/`[0x1AF6]`, second plane offset in `[0x1C02]`), and handles circular-buffer wraparound (marker attribute `2`, matching `mark_readout_delimiter`) | Confirmed |
| `0xE4217` | `print_banner_line` **(renamed)** | `(far-ptr string)` - calls `print_readout_string` on the given string, then `print_string_far` twice more on two *fixed* strings from the `0xFF7B` string table. Used by `print_selftest_banner` for both its "before"/"after" lines | Confirmed |
| `0xE07B4` | `print_selftest_report_line` **(renamed)** | The outer routine that wraps `print_selftest_banner`; computes a row Y coordinate from `[0x1B10]*0x32` and prints one self-test report line per call (via `SUB_E35C2`, `plot_readout_point`, `draw_readout_char`, and the other print primitives) | Confirmed mechanism; the exact report-line layout not fully mapped |
| `0xE0ADD` | `wait_readout_tick` **(renamed)** | Busy-waits for `[0x752]` (a counter, presumably interrupt-incremented) to change value - throttles `print_string_far`'s per-character loop to the readout hardware's real pace | Confirmed mechanism; what increments `[0x752]` not yet found |
| `0xE0E56` | `clear_selftest_status_flags` **(renamed)** | Zeroes 4 fixed status bytes (`[0x256]`-`[0x259]`) and the byte pointed to by each of 3 far pointers `SUB_E4443` sets up (`[0x326]`, `[0x336]`, `[0x33A]`) | Confirmed mechanism; overall purpose (a per-cycle self-test reset) inferred from context |
| `0xE0FD0` | `selftest_measure_and_report` **(renamed)** | One of `self_test_dispatcher`'s ~14 test calls, identified this session: enable/run/disable pattern via 3 calls to `selftest_measure_mode` with idx `1, 3, 2`. Which peripheral it measures isn't confirmed | Confirmed mechanism (enable/run/disable shape); peripheral identity not confirmed |
| `0xE0FF5` | `selftest_measure_mode` **(renamed)** | `idx==1`: enable a measurement mode (`[0x1B5E]=1`); `idx==2`: disable (complementary reset); else (incl. `idx==3`): run the actual measurement (calls `SUB_E296E` + `SUB_E0C3D`) and return its result code in `ax` | Confirmed |
| `0xE6D2F` | `seg_off_to_linear` **(renamed)** | `(offset, segment) -> offset + segment*16` - a compiled-in runtime helper for exactly the seg:off-to-flat-address math this project's own disassembly tooling uses, called 16x | Confirmed |
| `0xF0078` | `divide_scale_default` **(renamed)** | Pre-loads `dx:ax` from the global default divisor at `[0x6D2]`, then falls through into `divide_scale` below - a "use the default divisor" entry point | Confirmed mechanism; what the divisor represents physically not confirmed |
| `0xF0086` | `divide_scale` **(renamed)** | `(dividend dx:ax, divisor [bp+8])` - 32-bit divide (`SUB_E777D`) then scale (`SUB_E7764`) then a 3rd call (`SUB_E7F39`) updating `[0x6E6]`/`[0x6E8]` - a general fixed-point scale/convert utility, reachable directly or via `divide_scale_default` | Confirmed mechanism; the physical quantity being scaled (timebase? voltage?) not confirmed |
| `0xF03F4` | `reset_acq_buffers_stub` **(renamed)** | A tiny stub that reaches a shared tail block initializing **8 separate buffer-size variables to `0x800` (2048 bytes) each**, plus a handful of other fixed setup values - see `disasm/NOTES.md` "Possible waveform acquisition buffer init" | Mechanism confirmed; the "acquisition buffer" interpretation is a plausible but unconfirmed inference from the buffer count/size |
| `0xF0414` | `print_and_reset_acq_buffers` **(renamed)** | Prints a status string (`0xFF7B:0x362`) and does a scale computation, then falls through into the same shared buffer-init tail as `reset_acq_buffers_stub` | Same confidence caveat as `reset_acq_buffers_stub` |
| `0xE4429` | `read_channel1_status` **(renamed)** | Reads one fixed byte from physical `0x41000` - called alternately with `read_channel2_status` in a self-test loop that takes repeated readings, spaced by `wait_readout_tick` delays. "Channel 1" inferred from the 2-channel-scope context | Mechanism confirmed (and its R/W-register nature confirmed by `clear_channel1_status` writing the same address); channel identity is a plausible but unconfirmed inference |
| `0xE440A` | `read_channel2_status` **(renamed)** | Reads one fixed byte from physical `0x42000` - see `read_channel1_status` | Same confidence caveat |
| `0xFA00C` | `clear_channel1_status` **(renamed)** | Writes `0` to physical `0x41000` - the same fixed address `read_channel1_status` reads, confirming it's a real read/write hardware register | Confirmed mechanism |
| `0xF9FEE` | `clear_channel2_status` **(renamed)** | Writes `0` to physical `0x42000` - see `clear_channel1_status` | Confirmed mechanism |
| `0xE37B1` | `plot_readout_point_scaled` **(renamed)** | Divides both coordinate arguments by a local count (hardcoded to `1` in this build - a no-op divide, vestige of generic code once supporting a count > 1) and calls `plot_readout_point_relative` | Confirmed |
| `0xE5B34` | `selftest_display_result_mode` **(renamed)** | `idx==1`/`2` toggle the *same* `[0x1B5E]` flag `selftest_measure_mode` uses (a shared "measurement active" flag?); `idx==3`/`4` fall through to positioning + printing a result via `set_position_record`/`SUB_E2DC9` | Mechanism confirmed; relationship to `selftest_measure_mode`'s flag not fully resolved |
| `0xE06B6` | `update_menu_position` **(renamed)** | `(min, max, op_nibble)` - a bounded index/cursor tracker at `[0x1B50]` with clamping and wraparound between `min`/`max`. The op nibble selects init/cancel/increment variants; the increment path reads two front-panel button-state bytes (`[0x4E7]`/`[0x4E8]`, bit 7) and accelerates if both are set simultaneously. A menu/parameter-selection cursor - see `VARIABLES.md` | Confirmed mechanism; exact op-code meanings and which physical control feeds `[0x4E7]`/`[0x4E8]` not confirmed |
| `0xE2AB0` | `selftest_init_channel_hw` **(renamed)** | Calls `clear_selftest_status_flags` and `read_channel1_status`, then writes a short command sequence (`0x1D`, `9`, `0x1D`) combined with the caller's channel-select bits to a fixed far-pointer hardware register at `[0x1D20]` - looks like a front-end/ADC initialization sequence | Mechanism confirmed; hardware identity (which peripheral `[0x1D20]` addresses) not confirmed |
| `0xE5D2D` | `enable_interrupts` **(renamed)** | `sti; retf` - one instruction | Confirmed |
| `0xE5D2F` | `disable_interrupts` **(renamed)** | `cli; retf` - one instruction | Confirmed |
| `0xE3599` | `init_print_region_home` **(renamed)** | `build_print_record` wrapper: fixed `0x10x0x10` cell size, position override `= [0x45E]` (a saved copy of the readout buffer's own base pointer `[0x1CC4]`) - starts a print record at the readout's "home" position | Confirmed |
| `0xE35C2` | `build_print_region` **(renamed)** | `build_print_record` wrapper passing all 4 position/size arguments straight through - the general form behind `init_print_region`'s fixed-size convenience wrapper; used by `print_selftest_report_line` for its computed per-row position | Confirmed |
| `0xE6166` | `switch_to_next_task` **(renamed)** | Loads `SP`/`SS` from a per-task context table at `[0x1A9D + idx*4]` (`idx = [0x1ACD]`) and resumes it via the standard register-pop + `iret` epilogue - the "switch in" half of a small preemptive task-switcher driven by `INT2_HANDLER_LATE`. See `disasm/NOTES.md` "A small task scheduler" | Confirmed mechanism; how many tasks exist and what each does not confirmed |

### The ~14 self-test subroutines are NOT yet identified by name

Tried to match `self_test_dispatcher`'s (`0xE4244`) ~14 still-unnamed
subsystem-test calls (`SUB_E3F2C`, `SUB_E3F99`, `SUB_E2FC8`,
`SUB_E1B16`, `SUB_E252A`, `selftest_measure_and_report`, `SUB_E16EA`,
`SUB_E1E3E`, `SUB_E1D28`, `SUB_E1DB3`, `SUB_E1E90`, `SUB_E1F18` - full
list in `disasm/NOTES.md` "Found: the self-test dispatcher") to the
diagnostic
message strings in `STRINGS.md` (`HS_ACQ`, `MM_ACQ`, `COMM_ROM`,
`SYS_RAM`, etc.) by searching all decoded code for direct references
(immediate operands matching each string's address) - **found zero
direct references**, from any of these subroutines. Each one does
have substantial real code (100-200+ instructions), so they're not
stubs. **Correction this session**: three routines previously counted
in this unidentified list (`SUB_E374E`, `SUB_E3821`, and
`SUB_E0AF5`/now `print_string_far`) turned out to be display/print
primitives, not tests at all - found by tracing the literal arguments
they're called with. The remaining ~14 most likely return a status
code that gets OR-folded by `self_test_dispatcher` itself (not
`SUB_E094B`, which is called from the *other*, differently-purposed
routine - see above), with the actual message printing happening
elsewhere (a separate results-display routine, not yet found). **Don't
guess names for these from string proximity/call order alone without
that missing link** - the order tests run in isn't confirmed to match
the order their names are printed in.

## Comm/GPIB ROM (160-2998)

| Address | Label | Purpose | Confidence |
|---|---|---|---|
| `0x9470E` | `set_ds_return_old` **(renamed)** | `push ds; mov ds,[bp+6]; pop ax` - swaps `DS` to the caller-given segment word, returns the *old* `DS` in `ax` so a later call with that saved value restores it. Called 40x; every checked call site pairs a "swap in" with a matching "swap back" | Confirmed |
| `0x96B68` | `serial_tx_buffer_put` **(renamed)** | Writes a byte into a ring buffer at `[0x44C]`, wrapping at a fixed boundary (`0x433` bytes); the byte's source depends on mode flags `[0x459]`/`[0x629]` - likely the GPIB/RS-232 transmit buffer | Confirmed mechanism (ring buffer + wraparound); "transmit buffer" identity inferred from context, not confirmed against a schematic |
| `0x97B94` | `set_comm_critical_flag` **(renamed)** | Sets a critical-section-style flag `[0x5A3]` to the given value; on the "leaving" edge (new value `0`, old value nonzero, `[0x5A1]` set), swaps `DS` to the main ROM's low-RAM segment (`0x41`, via `set_ds_return_old`) and calls a main-ROM routine before swapping back - a cross-ROM notify-on-unlock pattern | Confirmed mechanism; the specific main-ROM callback's purpose not traced |
| `0x9605A` | `checksum_bytes` **(renamed)** | `(far ptr, count)` - sums `count` bytes into a byte accumulator (wrapping), returns it - a plain byte checksum | Confirmed |
| `0x96087` | `memset_far` **(renamed)** | `(far ptr, fill_byte, count)` - writes `fill_byte` to `count` consecutive bytes | Confirmed |

The remaining ~400 heuristically-found functions (`FUNC_2998_XXXX` in
`disasm/160-2998-14.lst`) are candidates once specific ones are traced
from a known caller.

## Variables

Moved to their own file — see `VARIABLES.md`.
