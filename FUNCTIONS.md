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

The four rows marked **renamed** are wired into `gen_disasm_x86.py`'s
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
| `0xE416F` | `self_test_dispatcher` **(renamed)** | ~25 calls to per-subsystem test routines in sequence, each folding a return code into an accumulator at `[bp-0xA]`. Gated by `[0x1B10]==0`. See `disasm/NOTES.md` "Found: the self-test dispatcher" for the full call list | Confirmed |
| `0xE094B` | `SUB_E094B` | Called immediately after `self_test_dispatcher` returns. Loads a far pointer from `[0x1C80]` into `[0x1B56]/[0x1B58]` and writes a 3-byte record (`3, 2, 0`) at it - looks like it's logging test results into a small record/event buffer rather than printing directly (searched for direct references from the self-test subroutines to the diagnostic strings in `STRINGS.md`; found none - this indirection may be why) | Purpose partially inferred, not confirmed enough to rename |
| `0xE44F1` | `check_comm_option_installed` **(renamed)** | **Comm/GPIB option-board presence + RAM/IO detection.** Checks for a valid ROM header checksum at `ES:DI` (hardcoded to `0x8000:4`, i.e. physical `0x80004` = the comm ROM's own BCD-revision+complement bytes), then tests writability at a *different* word, `ES:[DI+0xA000]` = physical `0x8A004`. Result packed into status byte `[0x1BF9]` (bit 1 = valid header, bit 2 = also RAM/IO-backed, gated on `[0x1B83]==0x1E` too) | Confirmed |
| `0xFBC09` | `memcpy_far` **(renamed)** | Generic memcpy-style utility: `lds si,[bp+6]; les di,[bp+0xa]; mov cx,[bp+0xe]; shr cx,1; rep movsw`. Takes (src far ptr, dest far ptr, byte count) | Confirmed |

### The ~20 self-test subroutines are NOT yet identified by name

Tried to match the self-test dispatcher's ~20 still-unnamed subsystem-
test calls (`SUB_E374E`, `SUB_E3821`, `SUB_E0AF5`, etc. - full list in
`disasm/NOTES.md` "Found: the self-test dispatcher") to the diagnostic
message strings in `STRINGS.md` (`HS_ACQ`, `MM_ACQ`, `COMM_ROM`,
`SYS_RAM`, etc.) by searching all decoded code for direct references
(immediate operands matching each string's address) - **found zero
direct references**, from any of these subroutines. Each one does
have substantial real code (100-200+ instructions), so they're not
stubs; they most likely return a status code that `SUB_E094B` (above)
logs, with the actual message printing happening elsewhere (a separate
results-display routine, not yet found) rather than inside each test.
**Don't guess names for these from string proximity/call order alone
without that missing link** - the order tests run in isn't confirmed
to match the order their names are printed in.

## Comm/GPIB ROM (160-2998)

Nothing individually identified yet. The ~400 heuristically-found
functions (`FUNC_2998_XXXX` in `disasm/160-2998-14.lst`) are candidates
once specific ones are traced from a known caller.

## Variables

Moved to their own file — see `VARIABLES.md`.
