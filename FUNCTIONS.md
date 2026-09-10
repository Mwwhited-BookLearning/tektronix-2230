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

| Address | Label | Purpose | Confidence |
|---|---|---|---|
| `0xE00B1` | `L_E00B1` (entry, via `RESET`) | Early boot: `cli`, clear a RAM buffer, set up the first stack (`SS:SP=4000:3FFA`), branch on a config/option byte at `cs:[bx+0x1AF]` | Confirmed (proven boot path) |
| `0xE5D1B4` | `ENTRY_E5D1_B4` | Continues boot: clears ES=DS=0x4000 RAM, reloads SP | Confirmed |
| `0xE5D1C7` | `ENTRY_E5D1_C7` | Checksums the low 512 bytes (`rep lodsw` x 0xFF words from `0:0`), installs INT1/INT2/INT255 (early) interrupt vectors, far-calls their handlers once, then jumps to `SUB_E6AAB` | Confirmed |
| `0xE6AAB` | `SUB_E6AAB` | First proper compiled-C-style function found (real BP frame). Looks like the main init dispatcher: calls several subsystem-init routines, zeroes 12-entry arrays, reinstalls INT255/INT2 (late) vectors | Confirmed reachable; purpose mostly inferred |
| `0xE416F` | `SUB_E416F` | **Self-test dispatcher.** ~25 calls to per-subsystem test routines in sequence, each folding a return code into an accumulator at `[bp-0xA]`. Gated by `[0x1B10]==0`. See `disasm/NOTES.md` "Found: the self-test dispatcher" for the full call list | Confirmed |
| `0xE44F1` | `SUB_E44F1` | **Comm/GPIB option-board presence + RAM/IO detection.** Checks for a valid ROM header checksum at `ES:DI` (hardcoded to `0x8000:4`, i.e. physical `0x80004` = the comm ROM's own BCD-revision+complement bytes), then tests writability at the same address. Result packed into status byte `[0x1BF9]` (bit 1 = valid header, bit 2 = also RAM/IO-backed, gated on `[0x1B83]==0x1E` too) | Confirmed |
| `0xFBC09` | `SUB_FBC09` | Generic memcpy-style utility: `lds si,[bp+6]; les di,[bp+0xa]; mov cx,[bp+0xe]; shr cx,1; rep movsw`. Takes (src far ptr, dest far ptr, byte count) | Confirmed |

## Comm/GPIB ROM (160-2998)

Nothing individually identified yet beyond the option-detection target
address itself (see `0xE44F1` above, which checks *this* ROM's header,
not a routine *in* it). The ~400 heuristically-found functions
(`FUNC_2998_XXXX` in `disasm/160-2998-14.lst`) are candidates once
specific ones are traced from a known caller.

## Variables

Moved to their own file — see `VARIABLES.md`.
