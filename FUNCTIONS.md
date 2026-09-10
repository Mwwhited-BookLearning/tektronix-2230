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
| `0xE416F` | `print_selftest_banner` **(renamed; CORRECTED - was `self_test_dispatcher`)** | Prints the self-test "start"/"complete" banner lines (via `SUB_E3567`/`SUB_E3930`/`SUB_E3854`/`SUB_E4217`/`SUB_E374E`) and sets `[0x1B10]=3`. **Contains no test calls at all** - the OR-fold test-dispatch pattern previously attributed to this address actually belongs to `0xE4244` (next row). Found by tracing what calls `SUB_E094B` and reading this routine end-to-end for the first time. See `disasm/NOTES.md` "self_test_dispatcher was misnamed" | Confirmed |
| `0xE4244` | `self_test_dispatcher` **(renamed; took over the name from `0xE416F` above)** | The real dispatcher: ~14 calls to per-subsystem test routines in sequence, each folding a return code into an accumulator at `[bp-0xA]`, returned in `ax`. One test (`SUB_E252A`) is conditionally skipped based on `[0x1B83]==0x1E`. Called from `0xE3DEE`, unconditionally (no `[0x1B10]` gate on this one - that gate belongs to `print_selftest_banner`'s caller instead). See `disasm/NOTES.md` "Found: the self-test dispatcher" for the full call list | Confirmed |
| `0xE094B` | `SUB_E094B` | Called immediately after `print_selftest_banner` (not `self_test_dispatcher` - corrected) returns. Loads a far pointer from `[0x1C80]` into `[0x1B56]/[0x1B58]` and writes a fixed 3-byte record (`3, 2, 0`) there - looks like initializing a small record/counter structure (capacity=3? item-size=2? count=0?) rather than logging a specific test's result, since it's called unconditionally with no test-result argument available at that call site | Purpose partially inferred, not confirmed enough to rename |
| `0xE097B` | `SUB_E097B` | Companion to `SUB_E094B` - operates on the same `[0x1B56]`-pointed structure, taking one byte argument. Compares/packs 4-bit nibbles (masks with `0xF0` twice) and either ORs a nibble into an existing slot or appends a new one, incrementing the structure's first byte (count?) each time. Only one call site found so far (from inside `print_selftest_banner`'s enclosing routine) - looks like a small dedup/insert operation into a nibble-packed set, not yet clear what values it's collecting | Purpose partially inferred, not confirmed enough to rename |
| `0xE44F1` | `check_comm_option_installed` **(renamed)** | **Comm/GPIB option-board presence + RAM/IO detection.** Checks for a valid ROM header checksum at `ES:DI` (hardcoded to `0x8000:4`, i.e. physical `0x80004` = the comm ROM's own BCD-revision+complement bytes), then tests writability at a *different* word, `ES:[DI+0xA000]` = physical `0x8A004`. Result packed into status byte `[0x1BF9]` (bit 1 = valid header, bit 2 = also RAM/IO-backed, gated on `[0x1B83]==0x1E` too) | Confirmed |
| `0xE0AF5` | `print_string_far` **(renamed)** | Loops a far-pointer nul-terminated string byte-by-byte, calling `print_char` per byte. Every observed call site passes a far pointer into segment `0xFF7B` (landing in `160-3532`), i.e. a fixed string table | Confirmed |
| `0xE0B2A` | `print_char` **(renamed)** | Thin wrapper: unpacks one byte argument, calls `write_readout_port_byte` | Confirmed |
| `0xE0B50` | `write_readout_port_byte` **(renamed)** | Writes the passed byte to the **fixed** physical address `0x40000+0x6F0` (never incremented) - see `disasm/NOTES.md` "The readout/CRT display memory" for why this is very likely a hardware port (character generator with an internal cursor), not plain RAM | Confirmed mechanism; hardware identity (readout/CRT character generator) is the best-fit inference, not yet confirmed against a schematic |
| `0xE39F0` | `append_readout_char` **(renamed)** | Appends a byte at the current write pointer of a running line buffer (`[0x1AF4]`), then writes a second byte at the same offset `+0x8000` - a dual-plane buffer in the same `0x4000` segment. See `disasm/NOTES.md` "The readout/CRT display memory" | Confirmed mechanism; which plane is "primary" vs "attribute/shadow" not yet confirmed |
| `0xFBC09` | `memcpy_far` **(renamed)** | Generic memcpy-style utility: `lds si,[bp+6]; les di,[bp+0xa]; mov cx,[bp+0xe]; shr cx,1; rep movsw`. Takes (src far ptr, dest far ptr, byte count) | Confirmed |

### The ~14 self-test subroutines are NOT yet identified by name

Tried to match `self_test_dispatcher`'s (`0xE4244`) ~14 still-unnamed
subsystem-test calls (`SUB_E3F2C`, `SUB_E3F99`, `SUB_E2FC8`,
`SUB_E1B16`, `SUB_E252A`, `SUB_E0FD0`, `SUB_E16EA`, `SUB_E1E3E`,
`SUB_E1D28`, `SUB_E1DB3`, `SUB_E1E90`, `SUB_E1F18` - full list in
`disasm/NOTES.md` "Found: the self-test dispatcher") to the diagnostic
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

Nothing individually identified yet. The ~400 heuristically-found
functions (`FUNC_2998_XXXX` in `disasm/160-2998-14.lst`) are candidates
once specific ones are traced from a known caller.

## Variables

Moved to their own file — see `VARIABLES.md`.
