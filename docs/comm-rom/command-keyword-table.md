# Comm ROM: the full RS-232/GPIB command-keyword table

This is the complete extracted contents of the command-keyword tables
first found during the 2026-09-14 live session (see
[`rs232-live-session-2026-09-14.md`](rs232-live-session-2026-09-14.md)
for how they were found) - that writeup only sampled a few entries
inline; this file has the full data. Extracted by
[`disasm/decode_comm_keyword_table.py`](../../disasm/decode_comm_keyword_table.py)
from `binary/160-2998-14.bin`; machine-readable companion at
[`disasm/comm_keyword_tables.json`](../../disasm/comm_keyword_tables.json)
(same convention as `STRINGS.md`/`disasm/strings_<rom>.json`).
Re-run the script if the comm ROM binary ever changes.

There are three related structures, all in one contiguous region of
the comm ROM (file offsets `0x8A58`-`0x8F1E`):

1. An **argument/value keyword table** - names and enum values used
   *inside* command bodies (`DATa ENCdg:BINary`'s `ENCdg` and
   `BINary`, `TRIggerd`'s `MODe` and `PPAuto`, etc.).
2. A **dispatch/index table** - 6-byte records resolving a numeric
   command ID to a far pointer.
3. A **top-level command header table** - the canonical uppercase text
   for every command the parser recognizes (what a query response
   echoes back, regardless of how abbreviated the request was).

All three are confirmed against live hardware: every entry in table 3
below either appears in a live `HELp?` response or is discussed as an
open question; the checksum/encoding findings from the same session
(binary/hex/ASCII `CURVe?`) are documented separately in
[`rs232-live-session-2026-09-14.md`](rs232-live-session-2026-09-14.md).

## 1. Argument/value keyword table (110 entries, `0x8A59`-`0x8D14`)

Two back-to-back alphabetical runs, evidently two separate source
tables laid out contiguously: a general parameter-name/value list
(`ABOrt` through `YUNit`), followed by a shorter run that's
specifically vertical/horizontal/trigger **mode enum values**
(`AC`/`DC`/`GND` coupling, `ADD`/`CHOp`/`ALT` vertical mode,
`ASWeep`/`BSWeep` horizontal mode, `NORmal`/`PPAuto`/`PRE` trigger
mode, `UNKnown` as a catch-all).

| Address | Len | Text | | Address | Len | Text |
|---|---|---|---|---|---|---|
| `0x88a58` | 5 | `ABOrt` | | `0x88bd3` | 3 | `REF` |
| `0x88a5e` | 7 | `ACCpeak` | | `0x88bd7` | 10 | `REPetitive` |
| `0x88a66` | 3 | `ACQ` | | `0x88be2` | 5 | `RESet` |
| `0x88a6a` | 3 | `ARM` | | `0x88be8` | 4 | `ROLl` |
| `0x88a6e` | 5 | `ASCii` | | `0x88bed` | 2 | `RP` |
| `0x88a74` | 7 | `ASEcdiv` | | `0x88bf3` | 6 | `SAMple` |
| `0x88a7c` | 4 | `AUTo` | | `0x88bfa` | 4 | `SAVe` |
| `0x88a81` | 7 | `AVErage` | | `0x88bff` | 4 | `SCAn` |
| `0x88a8a` | 8 | `BASegain` | | `0x88c04` | 3 | `SEC` |
| `0x88a93` | 6 | `BINary` | | `0x88c08` | 6 | `SELect` |
| `0x88a9a` | 6 | `BIT/nr` | | `0x88c0f` | 6 | `SMOoth` |
| `0x88aa1` | 6 | `BN.Fmt` | | `0x88c16` | 6 | `SOUrce` |
| `0x88aa8` | 7 | `BSEcdiv` | | `0x88c1d` | 5 | `SPEed` |
| `0x88ab0` | 6 | `BYT/nr` | | `0x88c23` | 5 | `STArt` |
| `0x88ab8` | 3 | `CH1` | | `0x88c29` | 3 | `STR` |
| `0x88abc` | 3 | `CH2` | | `0x88c2d` | 8 | `SWPcount` |
| `0x88ac0` | 7 | `CHAnnel` | | `0x88c37` | 6 | `TARget` |
| `0x88ac8` | 6 | `CHKsm0` | | `0x88c3e` | 4 | `TJEt` |
| `0x88acf` | 4 | `CLKs` | | `0x88c43` | 9 | `TRIGCount` |
| `0x88ad4` | 8 | `COUpling` | | `0x88c4d` | 8 | `TRIGMode` |
| `0x88add` | 6 | `CRVchk` | | `0x88c59` | 5 | `UNIts` |
| `0x88ae4` | 7 | `CURRent` | | `0x88c5f` | 6 | `UNLock` |
| `0x88aec` | 5 | `CURS1` | | `0x88c69` | 5 | `VALue` |
| `0x88af2` | 5 | `CURS2` | | `0x88c6f` | 7 | `VECtors` |
| `0x88afb` | 7 | `DEFault` | | `0x88c77` | 5 | `VGAin` |
| `0x88b03` | 7 | `DISplay` | | `0x88c7d` | 5 | `VOLts` |
| `0x88b0b` | 4 | `DIVs` | | `0x88c83` | 9 | `VPOsition` |
| `0x88b10` | 4 | `DONe` | | `0x88c8e` | 6 | `WEIght` |
| `0x88b16` | 5 | `EMPty` | | `0x88c95` | 4 | `WFId` |
| `0x88b1c` | 5 | `ENCdg` | | `0x88c9b` | 5 | `XINcr` |
| `0x88b22` | 3 | `ENV` | | `0x88ca1` | 5 | `XMUlt` |
| `0x88b26` | 4 | `EPS7` | | `0x88ca7` | 4 | `XOFf` |
| `0x88b2b` | 4 | `EPS8` | | `0x88cac` | 5 | `XUNit` |
| `0x88b30` | 6 | `EXTclk` | | `0x88cb2` | 2 | `XY` |
| `0x88b38` | 4 | `FILl` | | `0x88cb8` | 5 | `YMUlt` |
| `0x88b3d` | 6 | `FORmat` | | `0x88cbe` | 4 | `YOFf` |
| `0x88b44` | 4 | `FREe` | | `0x88cc3` | 5 | `YUNit` |
| `0x88b4a` | 2 | `GO` | | `0x88cca` | 2 | `AC` *(enum run starts)* |
| `0x88b4d` | 4 | `GRAt` | | `0x88ccd` | 3 | `ADD` |
| `0x88b53` | 3 | `HEX` | | `0x88cd1` | 5 | `AINtb` |
| `0x88b57` | 4 | `HMAg` | | `0x88cd7` | 3 | `ALT` |
| `0x88b5c` | 4 | `HPGl` | | `0x88cdb` | 6 | `ASWeep` |
| `0x88b61` | 5 | `HSRec` | | `0x88ce2` | 6 | `BSWeep` |
| `0x88b68` | 6 | `INVert` | | `0x88ce9` | 4 | `CHOp` |
| `0x88b72` | 4 | `LOCk` | | `0x88cee` | 2 | `DC` |
| `0x88b77` | 5 | `LSRec` | | `0x88cf1` | 3 | `GND` |
| `0x88b7e` | 4 | `MODe` | | `0x88cf5` | 6 | `NORmal` |
| `0x88b84` | 5 | `NR.Pt` | | `0x88cfc` | 7 | `PERcent` |
| `0x88b8a` | 9 | `NUMsweeps` | | `0x88d04` | 4 | `POST` |
| `0x88b95` | 3 | `OFF` | | `0x88d09` | 6 | `PPAuto` |
| `0x88b99` | 2 | `ON` | | `0x88d10` | 3 | `PRE` |
| `0x88b9d` | 7 | `PEAkdet` | | `0x88d14` | 7 | `UNKnown` |
| `0x88ba5` | 4 | `PERM` | | | | |
| `0x88baa` | 6 | `POInts` | | | | |
| `0x88bb1` | 8 | `POSition` | | | | |
| `0x88bba` | 7 | `PROtect` | | | | |
| `0x88bc2` | 6 | `PT.Fmt` | | | | |
| `0x88bc9` | 6 | `PT.Off` | | | | |

## 2. The dispatch/index table (26 records, `0x8D4A`-`0x8DE6`)

Each 6-byte record: `[id_byte][0xFF marker][2-byte little-endian
offset][2-byte segment, always 0xDE,0x90]`. The far pointer
(`segment*16 + offset`) resolves through the confirmed
`0x90000`-`0x97FFF` address-decode alias back to a real file offset
(`file_off = 0x8DE0 + offset`) - landing exactly on the length byte of
a header-table entry (table 3) for every non-zero `id`.

**Correction, 2026-10-11**: the start address was previously given as
`0x8D3E`. The real start is `0x8D4A` (confirmed two ways: `26*6 =
0x9C` bytes exactly spans `0x8D4A`-`0x8DE6` matching the table's own
end boundary with no slack, and this is exactly the address
`init_far_pointer_table` points `[0x6EA]` at - see section 6 below).
The 12 bytes at `0x8D3E`-`0x8D49` (two phantom "records" with
marker/segment bytes that don't fit the `0xFF`/`0x90xx` pattern at
all) aren't part of this table; they were never re-examined after the
original extraction script's off-by-two-records guess at the start
boundary.

| id | record offset | → file offset | → resolves to |
|---|---|---|---|
| `0x00` | `0x0006` | `0x8de6` | *(NULL sentinel, see below)* |
| `0x03` | `0x0024` | `0x8e04` | `BYTe` |
| `0x04` | `0x002a` | `0x8e0a` | `CH1` |
| `0x08` | `0x0040` | `0x8e20` | `DATa` |
| `0x0c` | `0x005a` | `0x8e3a` | `ERRor` |
| `0x0e` | `0x0067` | `0x8e47` | `FLOw` |
| `0x00` | `0x013c` | `0x8f1c` | *(NULL)* |
| `0x0f` | `0x006d` | `0x8e4d` | `HELp` |
| `0x11` | `0x007e` | `0x8e5e` | `ID` |
| `0x13` | `0x0087` | `0x8e67` | `JMP` |
| `0x00` | `0x013c` | `0x8f1c` | *(NULL)* |
| `0x14` | `0x008c` | `0x8e6c` | `LONg` |
| `0x15` | `0x0092` | `0x8e72` | `MESsage` |
| `0x00` | `0x013c` | `0x8f1c` | *(NULL)* |
| `0x16` | `0x009b` | `0x8e7b` | `OPC` |
| `0x17` | `0x00a0` | `0x8e80` | `PLOt` |
| `0x00` | `0x013c` | `0x8f1c` | *(NULL)* |
| `0x19` | `0x00ac` | `0x8e8c` | `REAdout` |
| `0x21` | `0x00e9` | `0x8ec9` | `SAVeref` |
| `0x28` | `0x0117` | `0x8ef7` | `TRIggerd` |
| `0x00` | `0x013c` | `0x8f1c` | *(NULL)* |
| `0x29` | `0x0121` | `0x8f01` | `VMOde` |
| `0x2a` | `0x0128` | `0x8f08` | `WAVfrm` |
| `0x00` | `0x013c` | `0x8f1c` | *(NULL)* |
| `0x00` | `0x013c` | `0x8f1c` | *(NULL)* |
| `0x2c` | `0x0137` | `0x8f17` | `ZZZZ` |

`id=0x00` entries (6 of the 26 records) all point at a single shared
`0x00` byte (file offset `0x8F1C`) - a NULL/empty-string sentinel, not
a real keyword. The literal `ZZZZ` entry at `id=0x2c` is a made-up
placeholder that closes out the header table (confirms its end
boundary; `ZZZZ` never appears in any live response).

## 3. Top-level command header table (45 entries incl. terminator, `0x8DE7`-`0x8F17`)

Matches the live `HELp?` response for **40 of 44** real entries
exactly. The 4 that don't (`ADDress`, `BYTe`, `JMP`, `SEGment`) are a
genuinely open question - see below.

| Address | Len | Text | In live `HELp?`? |
|---|---|---|---|
| `0x88de6` | 11 | `ACQuisition` | yes |
| `0x88df2` | 7 | `ADDress` | **no** |
| `0x88dfa` | 8 | `ATRigger` | yes |
| `0x88e04` | 4 | `BYTe` | **no** |
| `0x88e0a` | 3 | `CH1` | yes |
| `0x88e0e` | 3 | `CH2` | yes |
| `0x88e12` | 6 | `CURSor` | yes |
| `0x88e19` | 5 | `CURVe` | yes |
| `0x88e20` | 4 | `DATa` | yes |
| `0x88e25` | 5 | `DELAy` | yes |
| `0x88e2b` | 6 | `DELTAT` | yes |
| `0x88e32` | 6 | `DELTAV` | yes |
| `0x88e3a` | 5 | `ERRor` | yes |
| `0x88e40` | 5 | `EVEnt` | yes |
| `0x88e47` | 4 | `FLOw` | yes |
| `0x88e4d` | 4 | `HELp` | yes |
| `0x88e52` | 10 | `HORizontal` | yes |
| `0x88e5e` | 2 | `ID` | yes |
| `0x88e61` | 4 | `INIt` | yes |
| `0x88e67` | 3 | `JMP` | **no** |
| `0x88e6c` | 4 | `LONg` | yes |
| `0x88e72` | 7 | `MESsage` | yes |
| `0x88e7b` | 3 | `OPC` | yes |
| `0x88e80` | 4 | `PLOt` | yes |
| `0x88e85` | 5 | `PROBe` | yes |
| `0x88e8c` | 7 | `REAdout` | yes |
| `0x88e94` | 7 | `REFDisp` | yes |
| `0x88e9c` | 7 | `REFFrom` | yes |
| `0x88ea4` | 8 | `REFOrmat` | yes |
| `0x88ead` | 7 | `REFProt` | yes |
| `0x88eb5` | 7 | `REFStat` | yes |
| `0x88ebd` | 6 | `REMote` | yes |
| `0x88ec4` | 3 | `RQS` | yes |
| `0x88ec9` | 7 | `SAVeref` | yes |
| `0x88ed1` | 7 | `SEGment` | **no** |
| `0x88ed9` | 3 | `SET` | yes |
| `0x88edd` | 6 | `SGLswp` | yes |
| `0x88ee4` | 6 | `STAtus` | yes |
| `0x88eeb` | 4 | `STOP` | yes |
| `0x88ef0` | 5 | `STORe` | yes |
| `0x88ef7` | 8 | `TRIggerd` | yes |
| `0x88f01` | 5 | `VMOde` | yes |
| `0x88f08` | 6 | `WAVfrm` | yes |
| `0x88f0f` | 6 | `WFMpre` | yes |
| `0x88f17` | 4 | `ZZZZ` | terminator, not a command |

## Open questions this raises

- **Only 26 of the header table's 44 real entries have a dispatch
  record in this narrow slice.** Either there's a second/wider
  dispatch table elsewhere (not yet found), the missing ~18 commands
  are reached some other way entirely (e.g. a different dispatch
  mechanism for commands vs. queries), or the "numeric command ID"
  framing is itself slightly off. Not resolved.
- **`ADDress`, `BYTe`, `JMP`, and `SEGment` sit in the header table
  (and 2 of them - `BYTe` and, implicitly, values near `JMP` - even
  have dispatch records) but never appear in a live `HELp?` response.**
  These read like real internal tokens the parser recognizes (not
  believable as random noise - they're proper length-prefixed entries
  in the right shape) but aren't advertised as public top-level
  commands. Best guess: internal/diagnostic-only command headers, or
  this string pool doesn't actually separate "top-level commands" from
  "argument keywords" as cleanly as tables 1 and 3 suggest - `BYTe` in
  particular looks suspiciously similar to table 1's `BYT/nr` (a real
  `WFMpre` sub-argument) without quite matching it. Not resolved.
- **The metadata bytes between entries (skipped, not decoded) vary
  from 0 to 3 bytes per entry** - almost certainly per-keyword flags,
  a minimum-abbreviation-length byte, and/or a dispatch ID, but the
  exact encoding hasn't been cracked. Decoding these fully would
  likely also resolve the dispatch-table-coverage question above.
- **The code that actually walks either table has still not been
  found** in the disassembly - see `STILL_PENDING_DECODE.md`.
  **Candidate found 2026-10-09, resolved same day - NOT this table**:
  `FUNC_2998_5115`/`FUNC_2998_519A` (comm ROM, physical `0x85115`/
  `0x8519A`, both directly called from `comm_call_main_rom`) index a
  26-entry, 6-byte-record, first-letter-bucketed table
  (`([4]-0x41)*6`) via far-pointer bases `[0x6EA]`/`[0x6F6]`
  respectively, and resolve a match into `[0x604]` - confirmed to be
  this table's own `id=0x17`→`PLOt` value via a later comparison
  inside `FUNC_2998_44D0`. This looked like solid evidence of a
  letter-bucketed keyword table with a different shape than tables 1/2
  above, but tracing every reference to `[0x6EA]`/`[0x6F6]` across both
  ROMs found **zero comm-ROM writes to either address** - both pairs'
  only writers are unrelated main-ROM HPGL/plot-cache code
  (`FUNC_3633_7EBF`, `reset_all_channel_plot_caches`; see
  `VARIABLES.md`'s `[0x6EA]`/`[0x6EC]` entry). This is genuine
  cross-subsystem address reuse (the same pattern already documented
  for `[0x712]`), not an undiscovered comm-ROM table initializer -
  `FUNC_2998_5115`/`519A`'s real letter-table base is still unknown.
  See `docs/comm-rom/command-parser-token-scan-and-plot-handler.md`
  for the full writeup.
  **Reopened and fully resolved, 2026-10-11**: this conclusion was
  wrong. `init_far_pointer_table` (comm ROM, physical `0x80133` - see
  section 6 below) DOES write both `[0x6EA]` and `[0x6F6]`, via a
  generic `movsw`-pair copy loop that a literal-text grep for `mov
  [0x6ea], ...`/`mov [0x6f6], ...` can't see (the destination is
  computed register-indirectly through `ES:DI`, not encoded as a
  direct-displacement operand anywhere). `[0x6EA]` turns out to point
  at section 2's own dispatch/index table (now that its start address
  is corrected to `0x8D4A` - see above - it's the exact same table,
  not a second one). `[0x6F6]` points at a genuinely separate,
  previously-undocumented second 26-entry letter-bucket table at
  physical `0x889BC`, in the same 6-byte-record format, indexing into
  table 1 (the argument/keyword table, section 1) instead of table 3.
  Both tables' first-letter-bucket structure is now directly confirmed
  by decoding all 26 slots of each: slot `N`'s resolved text starts
  with the letter `chr(0x41+N)` for every non-empty slot (`A`→
  `ACQuisition`/`ABOrt`, `B`→`BYTe`/`BASegain`, ... `Z`→`ZZZZ`/*(empty,
  no Z keyword)*), and every empty slot resolves to a single shared
  sentinel address (`0x88F1C` for the table-3 side, `0x88CC9` for the
  table-1 side) rather than scattering garbage. The `[0x6F6]`/table-1
  side has one minor unresolved wrinkle: several slots (`D`, `L`, `R`,
  `S`, `U`, `V`, `Y`) resolve 2-3 bytes *before* their keyword's actual
  length byte, landing in the still-undecoded per-entry metadata gap
  noted in the second open-question bullet above, rather than exactly
  on it - cosmetic evidence that table 1's metadata bytes are read as
  part of the bucket lookup, not yet evidence against the bucket
  theory itself. So: `FUNC_2998_5115`/`519A`'s real letter-table bases
  are resolved - they're table 2 and this new table-1 index,
  respectively - though the register-indirect `ES:DI` write path is
  the reason the earlier grep-based writer search came up empty, not
  a sign the writer doesn't exist.

## 4. New, partially-decoded structure immediately before table 1: a flag-value query/stringify dispatch (found 2026-10-09)

While chasing an unrelated lead (the `PLOt FORmat` software-override
write site, see `STILL_PENDING_DECODE.md`), found that the comm ROM's
unidentified byte range immediately *before* table 1 (`UNKNOWN_DATA.
md`'s "2998 block 6", file offset `0x0824C`-`0x08728`, physical
`0x8824C`-`0x088728`) is not generic unlabeled data - it's a genuine
dispatch table, previously uncharacterized.

**Layout**: opens with two length-prefixed strings, `OFf` (3 bytes)
and `ON` (2 bytes), followed by exactly one `0x00` pad/alignment byte,
then ~100 variable-length binary records starting at file offset
`0x8254`. The first 6 records cleanly follow a fixed 12-byte stride -
`[4-byte key/marker][4-byte far pointer A][4-byte far pointer B]`.
**Correction, 2026-10-11**: the table previously shown here under the
header "Far ptr B" actually displayed far pointer **A**'s raw bytes
while crediting it with far pointer **B**'s resolution target - an
internal inconsistency from the original 2026-10-09 write-up. Both
pointers are now decoded directly from `binary/160-2998-14.bin` (file
offset `0x8254 + record*12`):

| Record | Key bytes | Far ptr A (seg:off) | → resolves into | Far ptr B (seg:off) | → resolves to |
|---|---|---|---|---|---|
| 1 | `3e ff 8f e3` | `907c:004c` | `0x8880C` (block 7) | `941f:0221` | `FUNC_2998_C411` |
| 2 | `2b ff af e3` | `907c:0022` | `0x887E2` (block 7) | `941f:0234` | `FUNC_2998_C424` |
| 3 | `2f ff af e4` | `907c:0034` | `0x887F4` (block 7) | `941f:0247` | `FUNC_2998_C437` |
| 4 | `45 ff af e4` | `907c:0076` | `0x88836` (block 7) | `941f:025a` | `FUNC_2998_C44A` |
| 5 | `40 ff af e2` | `907c:005e` | `0x8881E` (block 7) | `941f:026d` | `FUNC_2998_C45D` |
| 6 | `15 ff 2f c5` | `907c:0004` | `0x887C4` (block 7) | `941f:0280` | `FUNC_2998_C470` |

Far pointer A (always segment `0x907c` in these 6 records) resolves
through the confirmed `0x90000`-`0x97FFF` alias into the *next*
unidentified block (`UNKNOWN_DATA.md`'s block 7, `0x0872B`-`0x08A57`)
- **now itself decoded, see section 5 below**: it's not generic packed
data but a second shared dispatch/reference table, and far pointer A's
6 distinct values are 6 different *starting offsets* into that one
shared table, not 6 independent sub-tables. **Records past #6 do not
keep the clean 12-byte stride** - some are only 8 bytes (key +
far-pointer-B only, no far-pointer-A), and at least one apparent 4-byte
far pointer (`f5 05 c9 82`, resolving to physical `0x82c9f5`... i.e.
*outside* the comm ROM's own `0x80000`-`0x8FFFF` range, so almost
certainly a shared/fallback pointer referenced by several neighboring
short records rather than a record of its own) recurs multiple times.
The exact variable-length packing rule for records past #6 isn't
decoded - treat *this* table (block 6) as confirmed only for its first
6 entries.

**What the 6 confirmed handlers do**: `FUNC_2998_C411`/`C424`/`C437`/
`C44A`/`C45D` each push one small constant (`0x3a`, `0x41`, `0x48`,
`0x50`, `0x58` respectively) and `lcall` a shared helper, now renamed
`flag_value_to_keyword_ptr` (`0x944D5`, native def site
`FUNC_2998_C4D5` at `0x8C4D5` - see below); `FUNC_2998_C470` instead
calls `SUB_94559` first to compute a value, then pushes *that* and
calls the same helper. `flag_value_to_keyword_ptr` itself calls the
already-named `get_comm_config_flag(index)` (`FUNCTIONS.md`'s
`0x94488`/`(index)` - "reads a byte from a config/flag array at far
ptr `[0x73A]`"), with a special case (raw `index==0x58`'s nonzero flag
forces the dispatched value to `3`), then maps the resulting value
(`1`/`2`/`3`/anything else) to one of 4 far pointers and returns one of
them - i.e. it turns a stored flag's raw value into a keyword-table
pointer.

**Correction, 2026-10-11 (supersedes the "Working hypothesis" above)**:
the prior write-up described this as reading "one of 4 far pointers
read from a table based at `[0x6F2]`," implying `[0x6F2]` is itself a
small 4-entry pointer array, and guessed the destination was a generic
`OFf`/`ON` boolean stringifier. Neither holds up under a direct read of
`flag_value_to_keyword_ptr`'s actual body (`160-2998-14.lst:17791`
onward): `[0x6F2]`/`[0x6F4]` is a **single** far pointer (confirmed this
session to be initialized by the comm ROM's own `init_far_pointer_table`,
entry `dest_offset=0x22` -> far ptr `0x90a5:0x0008`; see `VARIABLES.md`),
and the 4 branches each `les di, ptr [0x6f2]` then *add* a different
literal offset to `di` (`+6`/`+0x29`/`+0x145`/`+0x19b`) rather than
indexing a pointer array. Resolving `[0x6F2]`'s base through the
confirmed `0x90000`-`0x97FFF` alias (`native = raw - 0x8000` - the
delta this session mis-remembered as `-0x10000` at first, which is worth
flagging so a future session doesn't repeat it) gives physical
`0x88A58` - **the exact start of table 1 above** (`0x88a58`/`ABOrt`).
So `flag_value_to_keyword_ptr` doesn't return an independent
response-string pointer at all; it returns a pointer *into table 1*,
and the 4 possible offsets land exactly on 4 of table 1's own entries
(verified byte-for-byte against `binary/160-2998-14.bin`):

| Dispatched value | Offset from `[0x6F2]` | Lands on (table 1 entry) |
|---|---|---|
| `1` | `+6` | `ACCpeak` |
| `2` | `+0x29` | `AVErage` |
| `3` (or forced, if `index==0x58`'s flag is nonzero) | `+0x145` | `PEAkdet` |
| anything else (default) | `+0x19b` | `SAMple` |

Three of these four (`AVErage`, `PEAkdet`, `SAMple`) are textbook
Tektronix **acquisition-mode** keywords; the fourth (`ACCpeak`) doesn't
cleanly match a known mode name but sits immediately next to `ACQ`/`ARM`
in the table. Each of the 6 handlers' fixed flag index (`0x3a`/`0x41`/
`0x48`/`0x50`/`0x58`) represents a different acquisition-mode-style
setting, and this function converts that setting's raw 0-3 flag value
into the matching mode keyword for a query response - not a generic
boolean `OFf`/`ON` stringifier as previously guessed (the leading
`OFf`/`ON` strings at the start of this byte range are real, but
nothing in `flag_value_to_keyword_ptr`'s own code path reaches them).
**This is a different mechanism from `FORmat`'s write site** (which
stays unresolved, see `STILL_PENDING_DECODE.md`) - no record found so
far ties back to `[0x461]` or any `FORmat`-specific handling.
**Resolved, 2026-10-11 (later pass, see the "Resolved" subsection at
the end of the SET-side section below)**: the 5 flag-index constants
are the `ACQuisition` command's `REPetitive`(`0x3a`)/`HSRec`(`0x41`)/
`LSRec`(`0x48`)/`SCAn`(`0x50`)/`ROLl`(`0x58`) sub-arguments, and the
computed-index path (`FUNC_2998_C470`/`C6C2`) is `CURRent`. The 4-byte
"key" field mentioned below is a separate, still-undecoded structure
from this function's own flag-index constants, which don't appear as
literal bytes in that key field.

**Loose end found in passing, now resolved**: `get_comm_config_flag`/
`set_comm_config_flag`'s `les di, ptr [0x73a]` and
`init_comm_dispatch_table`'s `[0x738]/[0x73a]` dispatch-slot write
(`sysrom_3532_3633.lst:10745`) looked like they overlapped by one
word - `[0x73a]` couldn't simultaneously be the *segment* half of one
far pointer and the *offset* start of another. Re-derived from raw
instruction bytes: it's not a real conflict, both readings are
correct, they just aren't the same physical byte. `init_comm_dispatch_
table` runs in the **main ROM** with the ordinary `DS=0`, so its
`[0x73a]` is literal physical `0x0073A`. `get_comm_config_flag`/
`set_comm_config_flag` run in the **comm ROM**, which (per the
pervasive `mov di, 0x8f80 / push di / lcall set_ds_return_old`
bootstrap idiom seen throughout `160-2998-14.lst`, e.g. at the very
start of `comm_rom_boot_init` physical `0x8E28C`, confirmed from
`set_ds_return_old`'s own body at `0x8C70E` doing
`push ds / mov ds, [bp+6] / pop ax` - i.e. literally "set DS to the
argument, return the old DS") standardly runs under `DS=0x8f80`, not
0. So their `[0x73a]` is physical `0x8f800 + 0x73a = 0x8FF3A` - a
separate byte in what's almost certainly the comm board's own private
RAM window near the top of its 64KB address space (`0x8F800`-`0x8FFFF`
ish), not the sysrom's global variable at raw `0x73A`. This is the
same **cross-subsystem address reuse** pattern already established
elsewhere in this project (e.g. `[0x712]`), just via a `DS` swap
instead of a different physical subsystem. Not independently
confirmed at runtime (would need the emulator to trace the actual `DS`
register live through this exact call chain) but strongly evidenced
statically: no other code sets any other `DS` value anywhere near this
call path, and `compute_parity_mode_code` (`0x96800`) independently
corroborates the same `es:[0x73A+0x20]`-style indexing into this same
array. Both `FUNCTIONS.md` entries can stand as written; they just
need a note that their `[0x73A]` is `DS`-relative (`DS=0x8f80` in
comm-ROM context), not the bare physical address.

## 5. "Block 7" decoded: a large shared function-reference table, and the SET-side counterpart of the flag family (found 2026-10-11)

Section 4's far pointer A values (segment always `0x907c`) resolve
into `UNKNOWN_DATA.md`'s "block 7" (file offset `0x872B`-`0x8A57`,
physical `0x8872B`-`0x8A057`), previously undecoded. It is not generic
data - it's a second shared table of 6-byte records: `[1-byte id][1-
byte marker][2-byte offset][2-byte segment]`. The marker byte has at
least 8 observed values (`0x02`/`0x06`/`0x0e`/`0x0f`/`0x22`/`0x26`/
`0x2b`/`0x2e`/`0x2f`), seemingly paired low/high variants of a few base
values rather than a strict 2-bit flag; its exact meaning isn't
decoded. **The segment field uses two different resolution rules
depending on its own value** - a wrinkle not previously documented:

- `segment >= 0x9000`: resolve through the confirmed `0x90000`-
  `0x97FFF` alias, `native = segment*16 + offset - 0x8000` (lands in
  the comm ROM's upper half, `0x88000`-`0x8FFFF`).
- `segment < 0x9000`: resolve directly, `native = segment*16 +
  offset` (lands in the comm ROM's own lower half, `0x80000`-
  `0x87FFF`, no alias correction - this half was never aliased in the
  first place, see `docs/comm-rom/address-map-and-cpu.md`).

Both conventions coexist in the *same* contiguous record stream - some
records use one, some the other - which is why an earlier pass at this
table (assuming a single fixed segment) only found 18 valid-looking
targets before giving up.

**Walking the table from each of section 4's 6 far-pointer-A starting
offsets, applying the correct per-record resolution rule, resolves
every single record to a real, already-disassembled function entry
point** (`push bp` prologue, confirmed by direct `.lst` grep) - a 100%
hit rate across every record checked so far. Six of the seven starting
offsets (`0x8880C`, `0x887E2`, `0x887F4`, `0x88836`, `0x8881E`,
`0x887C4`) each only walk a short, mutually-overlapping ~6-record
window before cycling back to earlier targets - evidence these are
*entry points into one shared array*, not independent tables, exactly
as suspected. The seventh (`0x8884E`) continues linearly far past that
point: walking it for at least 39 consecutive 6-byte records (file
offset `0x884E`-`0x8938`) yields **35 distinct confirmed real function
targets** in total (up from the 18 originally found), all but 6 of
which have never been reached by any other known static-reference
path:

| Native address | Symbol | `ref_count` | Notes |
|---|---|---|---|
| `0x8C1FA`/`C22F`/`C264`/`C2A6`/`C2E8`/`C301`/`C31B`/`C356` | `FUNC_2998_C1FA` etc. | 0 | the SET-side flag family, see below |
| `0x8BCDB`/`8BCC2`/`8BD0E`/`8BCF5` | `FUNC_2998_BCDB` etc. | 0 | not yet traced |
| `0x8BF75`/`8BF8D` | `FUNC_2998_BF75`/`BF8D` | 0 | not yet traced |
| `0x8A0DB`/`8A0EA`/`8A28A`/`8A2D2`/`8A3D3`/`8A426` | `FUNC_2998_A0DB` etc. | 0 | not yet traced |
| `0x811A4`/`811CC`/`810F6`/`8110A`/`8115C` | `FUNC_2998_11A4` etc. | 0 | comm-ROM *lower* half - first evidence this table reaches outside its own upper-half neighborhood |
| `0x8538A`/`85367`/`8545E`/`8540C`/`853BA`/`853E3`/`85435`/`85490`/`85477` | `FUNC_2998_538A` etc. | 0 | comm-ROM lower half |
| `0x8ACF2`/`8AD05`/`8AED2`/`8A621`/`8A7E3`/`8A832`/`8A5B8` | `FUNC_2998_ACF2` etc. | 0 | upper half |
| `0x8B2BC`/`8B252`/`8B339` | `FUNC_2998_B2BC` etc. | 0 | upper half |
| `0x85D87` | `FUNC_2998_5D87` | 0 | lower half |

All 35 have `ref_count: 0` and `functional_name: null` in
`disasm/160-2998-14.symbols.json` - matching this project's established
"exhausted static-reference search, no caller found anywhere" pattern.
This strongly suggests this table **is** the real, previously-
unrecognized reference mechanism for at least these 35 of the comm
ROM's 310 total orphan (`ref_count: 0`) `FUNC_*` entries (~11%). The
table's true start/end boundaries and the full semantics of its `id`/
`marker` bytes remain open - the walk above stopped at 39 records out
of a region big enough for roughly 135; continuing it, and finding
what code actually *reads* the `id`/`marker` fields to pick a record
at runtime (the whole point of a dispatch table, still not found), are
the natural next steps. See `STILL_PENDING_DECODE.md`.

### The SET-side counterpart of the acquisition-mode-style flag family

Of the 35, the 8 at `FUNC_2998_C1FA`/`C22F`/`C264`/`C2A6`/`C2E8`/
`C301`/`C31B`/`C356` (physical `0x8C1FA`-`0x8C356`) turned out to be
the **SET-side** counterpart of the GET-side flag family already
documented in section 4 above (`flag_value_to_keyword_ptr`'s 5 flag
indices `0x3a`/`0x41`/`0x48`/`0x50`/`0x58`). Traced in full:

- `FUNC_2998_C1FA`, `C22F`, `C264`, `C2A6`, `C2E8`, `C301`, `C31B`,
  `C356` each call `set_comm_config_flag` with one of the 5 fixed flag
  indices, most deriving the 0-3 value to store from a brand-new raw
  byte **`[0x5fe]`** (see `VARIABLES.md`), compared against literal
  values `7`, `1`, `0x43` (ASCII `'C'`), `0x35` (ASCII `'5'`) - i.e.
  this byte looks like it holds a raw incoming command-argument
  character, and these functions translate specific argument
  characters into one of the flag's 4 stored states.
- `SUB_94559`/`FUNC_2998_C559` (`0x8C559`) dynamically selects *which*
  of the 5 flag indices to use for a given call, based on new fields of
  the already-documented `[0x732]`/`[0x734]` "current command/device
  context record": `+0x28` (compared against thresholds `0x13`/`8`/
  `6`), `+0xf` (masked `&3`, compared to `1`), and `+0x17` (`==0`
  check). Called from `FUNC_2998_C470` and `FUNC_2998_C6C2`.
- `SUB_945CF`/`FUNC_2998_C5CF` (`0x8C5CF`) converts a 0-3 "channel/
  reference index" parameter (`[bp+8]`) into a bit-position bitmask
  (`0→1`, `2→4`, `3→8`, else→`2`), then further restricts it with a
  bitwise-AND mask that depends on *both* the flag index (`[bp+6]`)
  *and* whether `[0x732]+0x1f` (the already-documented "current
  command's numeric dispatch ID" field, confirmed live at runtime in
  `rs232-live-session-2026-09-14.md`) equals `0x14` - **the confirmed
  numeric ID for the `LONg` top-level command**
  (`docs/comm-rom/command-keyword-table.md` section 2 above, and
  cross-checked live against `disasm/comm_keyword_tables.json`'s
  alphabetical order in `rs232-live-session-2026-09-14.md`). `LONg` is
  a real, documented RS-232/GPIB command (`docs/programming-manual.md`,
  `docs/options.md`): `LONg ON`/`OFF` controls whether query *replies*
  use the full command word or its minimum-abbreviation short form
  (e.g. `LONg ON` → `TRIGGERD` in a reply; `LONg OFF` → `TRIG`). This
  is the first concrete tie between the entire 5-flag-index family and
  a specific named command - though the mechanism here (an extra
  bitmask restriction specifically *when the current command is
  `LONg`*) reads more like "this flag's valid bit range narrows while
  processing a `LONg` command" than "this flag directly stores `LONg`'s
  own ON/OFF state" - **not yet confirmed which, if either**.
- `SUB_946C2`/`FUNC_2998_C6C2` (`0x8C6C2`) orchestrates `SUB_94559`+
  `SUB_945CF`, and mirrors the GET side's documented special case for
  flag index `0x58` (section 4 above: a nonzero flag forces the
  dispatched value to `3`) with its own SET-side special case: flag
  index `0x58` *and* an incoming value of `3` gets forced down to `1`
  before the `set_comm_config_flag` call. Reinforces that `0x58` is
  categorically narrower (fewer valid states) than the other 4 flag
  indices.

Still open (partially resolved 2026-10-12, see the new subsection
below): what `[0x5fe]`'s own value is actually read from (which
command's argument parsing writes it) - the write site itself is now
found, though the exact numeric meaning of its comparison literals
(`1`/`7`/`0x43`/`0x35`) is not; and the downstream use of
`SUB_945CF`'s bitmask result beyond the `set_comm_config_flag` call. No
renames have been made yet for any of these - deferred per the
project's existing convention until the real-world meaning is clearer.

### Resolved, 2026-10-11 (later pass): the 5 flag indices are the `ACQuisition` command's `REPetitive`/`HSRec`/`LSRec`/`SCAn`/`ROLl` sub-arguments; the computed-index path is `CURRent`

`docs/programming-manual.md` table 7-27 ("Acquisition Commands")
documents exactly 6 `ACQuisition` sub-arguments whose values are drawn
from the same 4-keyword set `flag_value_to_keyword_ptr` dispatches onto
(`ACCpeak`/`AVErage`/`PEAkdet`/`SAMple`): `CURRent`, `HSRec`, `LSRec`,
`REPetitive`, `ROLl`, `SCAn` - a 1:1 count match with the 5 fixed flag
indices plus the one computed-index handler (`FUNC_2998_C470`/`C6C2`
via `SUB_94559`). The manual also documents, per sub-argument, *which*
of the 4 keywords are actually legal - a restriction that turns out to
exactly fingerprint each flag index's SET-side value domain:

| Flag index | SET-side handler | Values it can store | Manual's legal values for... | Match |
|---|---|---|---|---|
| `0x58` | `C2E8`/`C301` (unconditional `0`/`1`) | 2 (`SAMple`, `PEAkdet` after GET's nonzero-forces-`3` collapse) | `ROLl`: `{PEAkdet, SAMple}` only | **`0x58` = `ROLl`** |
| `0x3a` | `C1FA` (`[0x5fe]==7`→`2`(AVErage), `==1`→`1`(ACCpeak), else→`0`(SAMple); no `3`/PEAkdet case) | 3, no `PEAkdet` | `REPetitive`: `{ACCpeak, AVErage, SAMple}`, no PEAkdet | `0x3a` = `REPetitive` |
| `0x41` | `C22F` (byte-for-byte identical structure to `C1FA`) | 3, no `PEAkdet` | `HSRec`: `{ACCpeak, AVErage, SAMple}`, no PEAkdet | `0x41` = `HSRec` |
| `0x48` | `C264` (decodes `[0x5fe]` against `7`/`1`/`0x43` into all of `0`-`3`, no explicit `0x35` case - the 4th value is the `else` fallback) | 4 (full set) | `LSRec`: all 4 keywords legal | `0x48` = `LSRec` |
| `0x50` | `C2A6` (byte-for-byte identical decode to `C264`) | 4 (full set) | `SCAn`: all 4 keywords legal | `0x50` = `SCAn` |
| computed (`SUB_94559`) | `C470`/`C6C2` | n/a - index itself is derived at runtime, not fixed | `CURRent`: "depends on the current acquisition type and SEC/DIV setting," not a stored flag | computed = `CURRent` |

`0x58`=`ROLl` is the strongest link (a distinctive 2-value domain
matching a restriction unique to one command among the six). `0x3a`
and `0x41` have byte-for-byte identical handler bodies, so their
3-value domains alone can't distinguish `REPetitive` from `HSRec` -
both manual entries exclude `PEAkdet` identically. The tie-breaker is
block 6's own GET-side record order: `FUNC_2998_C411`/`C424`/`C437`/
`C44A`/`C45D`/`C470` push flag indices `0x3a`/`0x41`/`0x48`/`0x50`/
`0x58`/computed **in that exact order** (confirmed by direct read,
`160-2998-14.lst:17700`-`17759`), which matches the field order of the
manual's own `ACQuisition?` example query response string -
`"ACQ REP:AVE,HSR:SAM,LSR:PEA,SCA:PEA,ROL:PEA,..."` - field-for-field:
`REP`,`HSR`,`LSR`,`SCA`,`ROL`. Given `0x58`'s independently-confirmed
identity lands correctly at the 5th position in both orderings, the
same positional correspondence is taken as resolving `0x3a`=`REPetitive`
and `0x41`=`HSRec` (first two positions) and `0x48`=`LSRec`,
`0x50`=`SCAn` (third/fourth positions) - **the `0x48`/`0x50` and
`0x3a`/`0x41` pairings rest on this positional argument alone**, since
each pair's two handlers are structurally indistinguishable from each
other; the 2-vs-3-vs-4-value-domain fingerprinting only proves which
*pair* a flag index belongs to, not which specific member of the pair.
No renames made yet pending a convenient batch with other comm-ROM
flag-index cleanup, but the real-world identity of all 6 settings is
now considered resolved for documentation purposes.

## 6. `init_far_pointer_table` decoded; resolves the `[0x6EA]`/`[0x6F6]` "zero writes" mystery and finds a second letter-bucket table (found 2026-10-11)

While looking for block 7's reader (still not found - see
`STILL_PENDING_DECODE.md`), decoded `init_far_pointer_table` (comm ROM,
physical `0x80133`) fully. It's a small generic RAM-pointer
initializer: `DS=0x8013`, data starts at `SI=0x22` (physical
`0x80152`) with one header word (an `ES` segment value, `0x8fed`),
then 31 repeating 6-byte `(dest_offset:word, far_ptr:dword)` entries
terminated by `dest_offset=-1`, copied via a `movsw;movsw` pair into
`ES:[DI]` where `DI = dest_offset` per entry - i.e. a straight
`[0x8fed0 + dest_offset] = far_ptr` write for each entry.

`ES(0x8fed)*16 - DS(0x8f80)*16 = 0x6D0`, so any `dest_offset` here
converts to this project's established `[0x6xx]`/`[0x7xx]`-under-
`DS=0x8f80` naming convention via `offset_in_DS_convention =
dest_offset + 0x6D0` (checks out exactly against the already-known
`[0x6F2]` entry: `dest_offset=0x22` → `0x22+0x6D0=0x6F2`).

Two of the 31 entries are `dest_offset=0x1a` (→ `[0x6EA]`, far pointer
`0x90d4:0x0a` → native `0x88D4A`) and `dest_offset=0x26` (→ `[0x6F6]`,
far pointer `0x909b:0x0c` → native `0x889BC`). **This means
`init_far_pointer_table` is a comm-ROM writer of both `[0x6EA]` and
`[0x6F6]`**, contradicting section "Open questions"'s earlier
(2026-10-09) conclusion that tracing every reference found "zero
comm-ROM writes to either address." That earlier grep looked for a
literal `mov [0x6ea], ...`/`mov [0x6f6], ...`-style direct-displacement
instruction and found none, correctly - but missed this write because
it's done register-indirectly through `ES:DI` inside a generic copy
loop, with the destination offset sitting in the initializer's own
data table rather than in an instruction operand. The main-ROM
HPGL/plot-cache writes to displacement `0x6EA`/`0x6EC` found by that
same grep (`FUNC_3633_7EBF`, `reset_all_channel_plot_caches`) are
real, but address a *different* physical byte (main ROM runs under a
different `DS`) - that part of the "cross-subsystem address reuse"
conclusion still stands; it just wasn't the whole story.

Decoding the two target addresses (see the "Reopened and fully
resolved" note under section 2's open questions above for the full
letter-bucket analysis):

- `0x88D4A` (`[0x6EA]`'s target) **is section 2's own dispatch/index
  table**, byte-for-byte, once section 2's start address is corrected
  from the old (wrong) `0x8D3E` to `0x8D4A`. Not a second table at
  all - `[0x6EA]` is simply table 2's base far pointer, finally found.
- `0x889BC` (`[0x6F6]`'s target) is a genuinely new, previously
  undocumented second 26-entry table in the identical 6-byte-record
  format, sitting just past the nominal end of "block 7" (file offset
  `0x89BC`, vs. block 7's documented nominal range ending around
  `0x8A57`). Its 26 slots resolve into table 1 (the argument/keyword
  table, section 1) rather than table 3, with the same first-letter
  bucketing: slot 0 (`A`) → `0x88A58` = table 1's very first entry,
  `ABOrt`.

Neither table's slot count varies (`N`=26) despite the two source
tables (3 and 1) having very different entry counts (45 vs. 110) -
consistent with these being genuine one-slot-per-letter indexes (with
repeated empty-sentinel slots for absent letters) rather than anything
proportional to the underlying table's size.

`[0x6FA]` (table-start-of-block-7, `dest_offset=0x2a`) remains the
single interesting loose end from that first pass: it's initialized
correctly but still has no confirmed static reader anywhere in the
disassembly (checked via grep for the literal text `[0x6fa]` and the
raw byte pattern `fa06`, both empty) - possibly read only via the same
kind of register-indirect addressing that hid this section's own
finding, or not read by any currently-reachable code path at all.

## 7. The rest of `init_far_pointer_table` decoded - resolves table 3's base pointer, the per-entry "metadata byte," and (headline result) the `[0x732+0x1F]` write-site mystery (found 2026-10-11)

Continuing from section 6, decoded the remaining 24 of the table's 31
entries (full list: `dest_offset` → `[0x6xx]`/`[0x7xx]` name → raw far
pointer → resolved native address, all re-verified byte-for-byte
against `binary/160-2998-14.bin` with a corrected alias-bounding rule
- the `0x90000`-`0x97FFF` alias correction only applies for
`0x9000 <= segment < 0x9800`; an earlier pass in this same session
mis-applied it to any `segment >= 0x9000` with no upper bound and
briefly mis-resolved entry `dest_offset=0x90` as `0xEA5E5` instead of
the correct `0xF25E5`, a genuine un-aliased main-ROM address - self-
caught before anything was written down):

| `dest_offset` | `[0x6xx]` name | far ptr (seg:off) | native | Identified as |
|---|---|---|---|---|
| `0x16` | `[0x6E6]` | `90de:0006` | `0x88DE6` | **Table 3's own base pointer** - exact start of the top-level command header table (section 3), confirmed byte-for-byte (`0x0bACQuisition...`) |
| `0x1e` | `[0x6EE]` | `90d1:000c` | `0x88D1C` | **The per-command minimum-abbreviation-length byte array** - see below |
| `0x36` | `[0x706]` | `9024:000c` | `0x8824C` | Block 6's header-inclusive start (the `OFf`/`ON` length-prefixed strings) |
| `0x2e` | `[0x6FE]` | `9025:0004` | `0x88254` | Block 6's first real 12-byte record |
| `0x3a` | `[0x70A]` | `9023:0002` | `0x88232` | A null-terminated ID string, `"TEK/2230 SYS:0.0,COMM:0.0"` - see below |
| `0x62` | `[0x732]` | `0041:1b64` | `0x1F74` | **Forwards into the main ROM's flat `DS=0x41` variable pool, landing on the already-documented `[0x1B64]`** - see below |
| `0x66` | `[0x736]` | `0041:0548` | `0x958` | Forwards to flat `[0x548]` (a display-dirty-flag byte set by `tag_position_marker_and_dispatch`) |
| `0x6a` | `[0x73A]` | `0041:03e3` | `0x7F3` | Forwards to flat `[0x3E3]` (the per-item alt-value step/position byte array) - see below |
| `0x72` | `[0x742]` | `8e80:000e` | `0x8E80E` | Not yet characterized |
| `0x6e` | `[0x73E]` | `8eff:000a` | `0x8EFFA` | Not yet characterized |
| `0x7a` | `[0x74A]` | `8800:0000` | `0x88000` | The comm ROM's own absolute start (offset 0) - a self-referential pointer, purpose not characterized |
| `0x76` | `[0x746]` | `8e80:0000` | `0x8E800` | Not yet characterized |
| `0x90` | `[0x760]` | `f1d8:0865` | `0xF25E5` | A genuine main-ROM address (not alias-corrected - segment is outside `[0x9000,0x9800)`); **the exact same target** `init_far_pointer_table_sysrom`'s own entry #54 points to (`docs/acquisition-and-plotting/ram-far-pointer-table.md`'s "two independent far-pointer tables... agreeing on this one specific byte" finding) - still flagged there as "too precise to be coincidence, meaning not understood," unchanged by this pass |
| `0x52` | `[0x722]` | `900c:000c` | `0x880CC` | A far pointer to the literal ASCII string `"#H"`, printed as a prefix before numeric fields - see section 9 |
| `0xa` | `[0x6DA]` | `4067:000c` | `0x4067C` | **"Option Status Latch" (Table 3-1)** - already confirmed in `MEMORY_MAP.md` |
| `0xe` | `[0x6DE]` | `406b:000c` | `0x406BC` | **"Option Parameters Latch (in)" (Table 3-1)** - already confirmed in `MEMORY_MAP.md`; not yet cross-referenced in `VARIABLES.md`'s comm-ROM section until now |
| `0x6`/`0x12` | `[0x6D6]`/`[0x6E2]` | `406f:0000`/`406f:0008` | `0x406F0`/`0x406F8` | **"Option UART/GPIB chips" (Table 3-1)** - already confirmed |
| `0x32` | `[0x702]` | `85e0:000c` | `0x85E0C` | The master command-ID -> block-6-record-slice descriptor table, indexed by `[0x604]*2` - see section 10 |
| `0x46` | `[0x716]` | `900e:0004` | `0x880E4` | A baud-rate lookup table feeding the RS-232 UART register bank, indexed by the DIP-switch baud index `[0x4EC]` - see section 9 |
| `0x3e` | `[0x70E]` | `9020:0000` | `0x88200` | Uncharacterized, dense cluster - decodes as a run of short length-prefixed numeric-text tokens (`'1'`,`'2'`,`'5'`,`'1'`,`'2'`,`'4'`,`'2'`,`'4'`,`'8'`,`'15625'`...), hypothesized but unconfirmed as baud-rate-related constants |
| `0x42` | `[0x712]` | `900f:0004` | `0x880F4` | Uncharacterized, dense cluster - repeating `02 61 XX 00` records decoding as 2-letter lowercase strings (`"ae"`,`"af"`,`"ag"`,...), hypothesized but unconfirmed as a diagnostic/error-code mnemonic table |
| `0x4e` | `[0x71E]` | `900d:0000` | `0x880D0` | A command-ID membership/validity list, gated by the device-variant byte `[0x732]+0x1F` - see section 9 |
| `0x5a` | `[0x72A]` | `9006:0000` | `0x88060` | **`comm_call_main_rom`'s own parser-state jump-table base** - see below |
| `0x4a` | `[0x71A]` | `900d:000e` | `0x880DE` | A parity lookup table feeding the RS-232 UART register bank, indexed by the DIP-switch parity index `[0x4ED]` - see section 9 |
| `0x56` | `[0x726]` | `900c:0000` | `0x880C0` | A per-command GET-response field-index list, reusing `ACQuisition`'s block-6 response-building mechanism for a different (unidentified) command - see section 9 |
| `0x5e` | `[0x72E]` | `9000:0006` | `0x88006` | A 10-byte-stride record array, shaped like a numeric range/limit-check table - see section 9 |

### Table 3's base pointer and the per-command "metadata byte" resolved

`[0x6E6]` resolving to `0x88DE6` finally gives table 3 (section 3) a
confirmed base pointer - previously its start address was only known
from the raw byte layout, not tied to any comm-ROM variable.

`[0x6EE]` resolving to `0x88D1C` is the more interesting find: this is
a flat 45-byte array, parallel to table 3's 45 entries (44 real
commands + the `ZZZZ` terminator) in the exact same order, where byte
`i` equals **the minimum valid abbreviation length for entry `i`'s
command** - i.e. the count of leading uppercase-letter/digit
characters in that entry's text (`ACQuisition` → `3`, `CURSor` → `4`,
`DELTAT` → `6` since it's all-caps, `ID` → `2`, etc. - the standard
Tektronix "capitalized prefix is the shortest valid abbreviation"
convention already used throughout `docs/programming-manual.md`).
Verified with a direct script reading both the array and all 45 of
table 3's entries from `binary/160-2998-14.bin`: **0 mismatches across
all 45 entries**. This resolves section 1's long-standing open
question about the "metadata bytes between entries... hasn't been
cracked" - at least for table 3's per-entry metadata; table 1's
separate, still-undecoded gap bytes are a distinct question (see
below).

### The ID string

`[0x70A]` resolves to a null-terminated ASCII string, `"TEK/2230
SYS:0.0,COMM:0.0"`, sitting immediately before block 6's header (string
ends, then block 6's `0x03 OFf 0x02 ON` starts with zero gap bytes).
Table 3 has an `ID` entry (section 3 above), and this is a textbook
Tektronix `ID?` response format (`TEK/<model> SYS:<rev>,COMM:<rev>`
per `docs/programming-manual.md`'s `ID` command description) - **this
is plausibly the firmware's actual boot-time `ID?` response string**,
though no caller/reader of `[0x70A]` has been traced yet to confirm it
(same caveat as the dispatch-table entries above: the thing that
*reads* this table of base pointers by index, rather than the pointers
themselves, is still not found for most of them).

### Headline result: `[0x732]`, `[0x736]`, and `[0x73A]` are not comm-ROM-private structures - they forward into the main ROM's flat `DS=0x41` variable pool

All three of `init_far_pointer_table`'s "segment `0x41`" entries use
the project's already-established flat-RAM naming convention directly
as their far pointer's own fields (segment `0x41`, offset = the
bracket number) - i.e. `far_ptr=0041:1b64` *is* `[0x1B64]` in this
project's own notation, with no further translation needed. Each of
the three targets turns out to already be a documented, independently-
discovered variable from a completely unrelated main-ROM subsystem:

- `[0x732]` → `0041:1b64` = **`[0x1B64]`**, the first of the two
  indices `find_item_table_indices` (`0xF6CEA`) computes for the
  readout-label builder (`VARIABLES.md`'s `[0x1B64]`/`[0x1B67]` row,
  found 2026-10-10 - completely unrelated to anything comm-ROM).
- `[0x736]` → `0041:0548` = **`[0x548]`**, a display-dirty-flag byte
  set by `tag_position_marker_and_dispatch` (`FUNCTIONS.md:208`).
- `[0x73A]` → `0041:03e3` = **`[0x3E3]`**, the per-item alt-value
  step/position byte array indexed by `step_item_subvalue_fwd`/`_back`
  (`VARIABLES.md:55`) - and already independently confirmed (section 4
  above) as the base `get_comm_config_flag`/`set_comm_config_flag`
  read/write through `les di, ptr [0x73a]`. **This means the comm
  ROM's "config flags" aren't a comm-private array at all - they are
  literally the same per-item step/position bytes the front-panel item
  stepper uses**, and the 5 flag-index constants (`0x3a`/`0x41`/`0x48`/
  `0x50`/`0x58`) this project has been treating as opaque "flag
  indices" are almost certainly **item indices into the main item
  table** (items `0x3a`(58)/`0x41`(65)/`0x48`(72)/`0x50`(80)/`0x58`(88)
  specifically). This doesn't by itself identify which real settings
  those 5 item indices are, but it reframes the open question
  concretely and is worth chasing by cross-referencing the main item
  table's own per-index records next session.

This isn't a new phenomenon for this project - `docs/acquisition-and-
plotting/ram-far-pointer-table.md`'s "Follow-up" on this same
`init_far_pointer_table` already flagged "3 more entries forward to
plain flat-RAM cells (`[0x548]`/`[0x1B64]`/`[0x3E3]` under `DS=0x41`)"
as a loose end without identifying which comm-ROM variable each one
corresponded to. This section supplies that missing link.

### The `[0x732]`/`[0x1B64]` identity resolves the long-unfound `[0x732+0x1F]` write site

This is the most significant consequence. `[0x732]` is read via `les
di,[0x732]` (89 sites) / `les bx,[0x732]` (18 sites) throughout the
comm ROM, and `[0x732+0x1F]` specifically holds a byte this project has
confirmed (section 2, and `compute_response_format_flags`) matches
`0x14`/`0x15`/`0x1E` - the numeric dispatch IDs for `LONg`/`MESsage`/
`REFStat`. **An exhaustive search for whoever writes `[0x732+0x1F]`
found nothing anywhere in either ROM** (`STILL_PENDING_DECODE.md`,
`rs232-live-session-2026-09-14.md`) - every write form via both
confirmed base registers, immediate and register-sourced, was checked
and came up empty.

Now that `[0x732]`'s own boot value is known (`0041:1b64`, i.e. `ES=
0x41`, `DI=0x1B64` after the `les`), `es:[di+0x1F]` resolves to
physical `0x41*16 + 0x1B64 + 0x1F = 0x1F93` - which is **exactly**
`[0x1B83]` under this project's own flat-`DS=0x41` convention
(`0x410 + 0x1B83 = 0x1F93`). `[0x1B83]` is already fully documented
(`VARIABLES.md:25`) as **the comm-option-board detection result,
written by `detect_comm_option_hw` (`0xE75C0`) to exactly `0x1E` or
`0x14`** - the same two values independently found at `[0x732+0x1F]`.

This is not a coincidental value match; the arithmetic shows these are
**the same physical RAM byte**: `[0x732+0x1F]` and `[0x1B83]` are one
and the same, reached via two completely different paths (a comm-ROM
far-pointer dereference vs. a main-ROM direct-displacement access)
that this project had been investigating as two unrelated things. This
fully explains why no write site for `[0x732+0x1F]` could be found by
searching the comm ROM: **the write happens in the main ROM**, via the
already-confirmed `mov byte ptr [0x1b83], 0x1e`/`0x14` instructions
inside `detect_comm_option_hw` (physical `0xE75F6`/`0xE7614`/`0xE761C`)
- a totally different instruction form, in a different ROM, under a
different name, that the comm-ROM-focused search had no reason to look
at.

It also reframes (without fully resolving) two existing open
questions:

- `compute_response_format_flags`'s own "open question" (`FUNCTIONS.
  md:297`) - whether `[0x732+0x1F]`'s boot-time value of `0x1E` is "a
  genuine device-type code that coincidentally matches `REFStat`'s
  command ID" - is answered on the "coincidence" side: it's not a
  coincidence of *value*, because it's the same *byte* as the already-
  understood hardware-detection flag `[0x1B83]`, which only ever holds
  `0x14` or `0x1E` (never `0x15`). `init_comm_device_type_and_defaults`
  checking `es:[0x732+0x1F]==0x1E` at boot time is, concretely,
  checking `detect_comm_option_hw`'s own result.
- This leaves an open sub-question of its own: if `[0x732+0x1F]` really
  is just `[0x1B83]` (a static hardware-variant flag set once at boot,
  never `0x15`), **how does `compute_response_format_flags`'s `0x15`
  (`MESsage`) case ever fire**, since nothing writes `0x15` to
  `[0x1B83]` anywhere in either ROM? Two possibilities, neither
  confirmed: (a) that branch is simply dead code in the shipped
  firmware (consistent with this project's precedent of finding dead
  branches elsewhere, e.g. `test_gpib_lon_or_ton_active`), or (b) a
  second, still-undiscovered write path to this same byte exists for
  genuine per-command dispatch-ID tagging while a command is actively
  being parsed, coexisting with the boot-time hardware-detection write.
  Not resolved - flagged for a future session rather than guessed.

### A second resolved mystery: `[0x72A]`'s own write site, and its jump-table contents

`VARIABLES.md`'s existing `[0x5A4]` entry documents `comm_call_main_rom`'s
parser-state byte `[0x5A4]` indexing "a 4-byte-per-entry far-call jump
table at `[0x72A]`" (`al=[0x5A4]; ax=al*4; les di,[0x72A]; lcall
es:[bx+di]`), but flags `[0x72A]`'s own write site as not found.
`init_far_pointer_table`'s entry `dest_offset=0x5a` **is** that write
site: it sets `[0x72A]` to raw far pointer `9006:0000` at boot - which,
read directly by `les di,[0x72A]` (no software alias correction; the
comm ROM's own address-decode hardware handles the `0x90000`-`0x97FFF`
aliasing transparently), lands on native `0x88060`. Dumping that
address as a 24-entry array of 4-byte far pointers (the shape
`comm_call_main_rom`'s indexing implies) resolves **every single
entry** to a real, consistent address in the comm ROM's own lower half
(`0x83B1A`-`0x85098`, all segment `0x839f`, ascending) - a clean,
dedicated per-parser-state handler table, exactly matching the
already-documented 23-ish distinct state values `[0x5A4]` is observed
to take. The per-state semantics of each of the 24 entries aren't
decoded yet, but the table's existence, base address, and boot
initialization are now fully confirmed.

### Still uncharacterized

The remaining entries (`[0x742]`, `[0x73E]`, `[0x74A]`, `[0x746]`) are
recorded in the table above with their resolved addresses but not yet
decoded past a first byte-level glance - left as open leads in
`STILL_PENDING_DECODE.md`. The dense cluster packed into
`0x88006`-`0x880F4` (`[0x722]`/`[0x716]`/`[0x70E]`/`[0x712]`/`[0x71E]`/
`[0x72A]`/`[0x71A]`/`[0x726]`/`[0x72E]`) turned out, on 2026-10-12, to
be several unrelated small structures rather than one mystery table -
see section 9 below. `[0x702]` is now resolved too, as the master
command-ID descriptor table for block 6's shared record array - see
section 10. Tracing `[0x702]` also surfaced a second, much larger
still-unresolved structure centered on `[0x73E]`/`[0x746]`/`[0x5F8]`
(85-105 references each) - flagged as a new open item, not yet
characterized beyond its record stride and index variable; see
section 10's own closing subsection.

## 8. `[0x5fe]`'s write site found; its exact numeric encoding still not pinned down (found 2026-10-12)

Section 5's SET-side flag-family writeup left one open question: which
command's argument-parsing code actually *writes* `[0x5fe]` before the
8 flag-decoding functions read it. Traced in full:

- The write site is `FUNC_2998_43F0` (physical `0x8444C`):
  `mov al, byte ptr [0x608]` followed immediately by `mov byte ptr
  [0x5fe], al`, inside a branch gated on `[0x5f7]==0` being false (i.e.
  the function's *second* pass through its main loop - the first pass
  writes the same `[0x608]` value into a sibling byte `[0x606]`
  instead, via a structurally identical branch just above). No static
  caller was found by name (`grep` for `FUNC_2998_43F0` turns up only
  its own label), but its address falls inside the parser-state
  handler table's already-confirmed range (`0x83B1A`-`0x85098`, see
  section 7 above) - consistent with it being reached only through
  `[0x5A4]`/`[0x72A]`'s register-indirect dispatch, not a direct call.
- `[0x608]` is itself set by `FUNC_2998_5115`/`FUNC_2998_519A` (the
  letter-bucket table walkers resolved in section 6 above, via
  `[0x6EA]`/`[0x6F6]`) from the first byte of the matched letter
  bucket's 6-byte record, then incremented once per rejected candidate
  by `FUNC_2998_3048`'s forward abbreviation-matching scan
  (`inc byte ptr [0x608]` at physical `0x831C5`) as it advances through
  consecutive table-1 entries within that letter. So `[0x5fe]` holds a
  per-match value seeded from the matched keyword's first-letter bucket
  and incremented by its position within that bucket's run - mechanically
  consistent with (but not proven identical to) table 1's global
  alphabetical index of the matched argument/value keyword.
- Numerically testing that "global alphabetical index" hypothesis
  against `disasm/comm_keyword_tables.json`'s confirmed real indices
  (re-verified by an independent manual byte-level walk of table 1 from
  `ABOrt`, not just the JSON): `ACCpeak`=1, `AVErage`=7, `PEAkdet`=51,
  `SAMple`=63. `FUNC_2998_C1FA`/`C22F`'s comparison literals `1`/`7`
  match `ACCpeak`/`AVErage` **exactly**, but `FUNC_2998_C264`/`C2A6`'s
  literal `0x43` (67) for the value they resolve to `0`
  (`SAMple`/enum-`0`) is off by `+4` from `SAMple`'s real index `63`,
  and `FUNC_2998_C31B`'s literal `0x35` (53) for its `PEAkdet`/enum-`3`
  value is off by `+2` from `PEAkdet`'s real index `51` - a
  **mismatch that is not explained by a table-parsing error**: the
  manual byte-level re-walk independently reproduced the same real
  indices as the JSON (including every metadata-byte gap in between),
  and the metadata bytes themselves don't encode anything resembling
  the comparison literals either (see below). So `[0x5fe]`'s raw value
  is **not** simply table 1's absolute alphabetical index, despite the
  exact match for the first two entries - that match may be coincidence
  (both `ACCpeak` and `AVErage` are early, `A`-prefixed entries where a
  per-letter-bucket-relative index and a global index would agree).
- Side finding while checking the metadata-byte hypothesis: table 1
  entries with a 3-byte metadata gap (`CURS2`, `INVert`, `PT.Off`, `RP`)
  all have the same shape `00 01 <ASCII char>`, where `<ASCII char>` is
  consistently the **first letter of the very next table-1 entry**
  (`CURS2`->`'D'`/`DEFault`, `INVert`->`'L'`/`LOCk`, `PT.Off`->`'R'`/
  `REF`, `RP`->`'S'`/`SAMple`) - confirmed byte-for-byte for all four.
  Entries with a 1-byte gap just have a bare `00`. This looks like a
  genuine per-entry field (plausibly a "next letter-bucket starts here"
  hint), not parser noise, but is a side lead, not a resolution of the
  `[0x5fe]` puzzle - noted here for whoever picks this up next rather
  than chased further this session.

**Net result**: `[0x5fe]`'s write site, and the general mechanism that
feeds it (letter-bucket lookup + scan-position increment, shared with
the already-understood command-ID resolution path), are now confirmed.
The exact arithmetic turning a matched keyword into `[0x5fe]`'s raw
byte value is not - flagged here as an open loose end rather than
guessed at further. This does not weaken section 5's flag-index-to-
sub-argument mapping, which rests on value-domain *size* and the
block-6 push-order argument, neither of which depends on `[0x5fe]`'s
own numeric encoding.

## 9. The "dense cluster" (`0x88006`-`0x880F4`) is several unrelated small structures, not one table (found 2026-10-12)

Section 7's table flagged 9 `init_far_pointer_table` entries packed
into the narrow range `0x88006`-`0x880F4` as "uncharacterized, dense
cluster" - previously assumed (across several sessions) to plausibly
be one related table. Grepped every one of these 9 flat offsets for
its own *runtime reader* (as opposed to just the boot-init entry that
sets the pointer) and read each call site - they resolve to **at least
5 unrelated structures**, now mechanism-confirmed by direct trace:

- **`[0x716]`/`[0x71A]`: a baud-rate table and a parity table feeding
  the RS-232 UART hardware.** Both are read together in one place
  (`0x8E4FF`-`0x8E524`, right after a call to
  `read_dip_switches_serial_config` and a `cmp byte ptr [0x4ec], 0x1f`
  "not Off Line" guard): `[0x716][[0x4EC]]` (the DIP-switch-decoded
  baud-rate index, confirmed in `MEMORY_MAP.md`) is OR'd with `2` and
  stored to `[0x59f]`, which is then written to `es:[di+3]` of the far
  pointer at `[0x6D6]` - the service manual's confirmed "Option
  UART/GPIB chips (I/O)" register bank (`MEMORY_MAP.md`, physical
  `0x406F0`-`0x406F7`). `[0x71A][[0x4ED]]` (the DIP-switch-decoded
  parity index) is AND'd with `0x3F` and stored to `[0x59a]`, written
  to `es:[bx+1]` of the same `[0x6D6]` bank. `docs/options.md`'s own
  text on the UART (`U1251`, an Intel 8251) says its "internal baud
  rate generator... crystal Y1251... is divided by **software
  selectable ratios**" - exactly the shape of a lookup table turning a
  DIP-switch index into a hardware-register byte. Whether `[0x716]`'s
  byte is literally the 8251's own Mode-register baud-factor bits or a
  separate baud-generator-latch divisor code (both live in the same
  8-byte I/O bank per the manual) isn't distinguished yet - flagging
  as a plausible but unconfirmed detail, not the overall finding.
- **`[0x71E]`: a command-ID membership/validity list, read inside the
  already-known command-ID-resolution function (`FUNC_2998_57E5`,
  `0x857E5`).** After the `[0x608]`-based letter-match (see section 8)
  rules out several fixed command IDs (`1`/`3`/`0x22`/`0x2c`/`0x13`),
  the function loads `[0x71E]` (or `[0x71E]+8`, selected by a device-
  variant byte at `[0x732]+0x1F` - i.e. the already-confirmed
  `[0x1B83]` field - being `0x15`) and linearly scans its bytes for one
  matching `[0x608]` (the matched command ID), gating whatever follows
  on membership in this list. Reads as "is this command ID in the
  variant-specific allow-list" - plausibly GPIB-only vs RS-232-only (or
  2200-series-only) command subsets, not proven which.
- **`[0x722]`: a short, literal null-terminated ASCII string, `"#H"`**
  (confirmed by direct byte read at its target physical address,
  `0x880CC`: `23 48 00`). Printed via `print_string_serial` at the
  start of two sibling functions (`FUNC_2998_5985`, `FUNC_2998_59D9`)
  that otherwise print decimal numbers - one splits a 16-bit value at
  `[0x612]` into high/low bytes (`sar` ×8) and prints each as a
  separate decimal field after the `"#H"` prefix (looks like a
  `"#H<hi>.<lo>"`-shaped version/revision tag, comparable in spirit to
  the already-confirmed `[0x70A]` `ID?`-response string, but not
  confirmed as such); the other prints `"#H"` then a loop of `si` raw
  bytes read from a separate buffer at `[0x61E]`. Which comm command(s)
  call into this - and so what `"#H"` actually labels - not traced.
- **`[0x726]`: a per-command GET-response field-index list**, read by
  `FUNC_2998_5B9B` (`0x85B9B`) in the exact same "config-flag index ->
  `compute_response_format_flags` -> `;`-separated field" loop already
  confirmed for `ACQuisition`'s block-6 GET response (section 4) - but
  with a *different* literal byte sequence (`19 00 06 17 20 16 14 fe 0e
  26 ff ff`, i.e. config-flag indices `0x19`/`0`/`6`/`0x17`/`0x20`/
  `0x16`/`0x14`/`0x0e`/`0x26` with a `0xFE` separator/EOI marker and a
  double-`0xFF` terminator), confirming this GET-response-building
  mechanism is reused by at least one other comm command besides
  `ACQuisition` - which command, not yet identified (would need
  matching this 9-field list against `docs/programming-manual.md`'s
  other command response shapes).
- **`[0x72E]`: a 10-byte-stride record array**, read by `FUNC_2998_
  373A` (`0x8373A`) indexed by `[bp+6]*10` (a caller-supplied argument,
  not a fixed index). Each record's word at `+0`/`+2` is compared
  (via the cross-ROM helper `SUB_E7753`) against a pushed pair of
  values from `[0x578]`/`[0x57a]`, and a flag bit (`+9` bit `0x80`) is
  checked afterward - the shape of a **numeric range/limit-check
  table** (per-something min/max thresholds), but which argument
  `[bp+6]` represents, and what `[0x578]`/`[0x57a]` hold, aren't
  traced.

**Net result**: the "dense cluster" framing (implying one
undiscovered shared table) was itself the obstacle - these are 5
independent structures that happen to sit in the same ~240-byte
region, each already reachable from a distinct, real caller once
grepped for by its own flat offset rather than assumed to be part of
a single blob. Mechanism confirmed for all 5; full identity (which
UART register field, which command's allow-list/response-list, what
`"#H"` labels, what `[0x72E]`'s records bound) remains open for a
future pass. `[0x70E]`/`[0x712]` (the baud-rate-text and 2-letter-
mnemonic tables from earlier sessions) are unaffected by this -
already separately characterized, see the table in section 7.

## 10. `[0x702]` is the master command-ID -> block-6-record-slice descriptor table (found 2026-10-12)

Section 7 had flagged `[0x702]` ("the comm ROM's own lower half") as
not yet characterized. Its only 4 readers (`0x84519`, `0x84527`,
`0x84529`... all inside one function around physical `0x84512`-
`0x84551`) resolve it cleanly:

- `[0x604]` (the already-confirmed resolved command ID - e.g. `PLOt`
  = `0x17`, per section 7's `FUNC_2998_5115`/`519A` finding) is used as
  a **word index** (`*2`) into `[0x702]`, a 2-byte-per-entry table.
- Entry byte 0 (`es:[bx+di]`) is a **record count**, stored in
  `[bp-8]` and later used as the loop bound for a `for` loop
  (`L_845CA`-`L_845F6`) that walks 12-byte records.
- Entry byte 1 (`es:[bx+si+1]`) is multiplied by `0xC` (12) and added
  to the `[0x6FE]` far pointer's offset - `[0x6FE]` being section 7's
  already-confirmed base of block 6's first real 12-byte record (the
  `ACQuisition` GET-response/argument-matching record array,
  `command-keyword-table.md` section 4). The result becomes the
  starting far pointer (`[bp-0xc]`/`[bp-0xa]`) for the loop.
- The loop body calls the same argument/keyword-matching helpers
  (`FUNC_2998_477F`/`FUNC_2998_4835`, with literal masks like `0xC02F`/
  `0xC00F`/`0xC01F`) already seen operating on block 6's 12-byte
  records elsewhere, advancing the pointer by `0xC` each iteration and
  incrementing a counter compared against the entry's byte-0 count.

So **`[0x702][id*2]` = `{record_count, start_record_index}` for
command ID `id`**: it tells the shared dispatcher which contiguous
slice of the single block-6 record array (based at `[0x6FE]`) belongs
to that specific command. This generalizes the previously
command-specific "block 6" mechanism (seen fixed for `ACQuisition`'s 5
flags in section 4, and reused via a separate field-index list for one
other command via `[0x726]` in section 9) into a single, uniform,
command-ID-driven lookup covering (in principle) every comm command
that has block-6-style arguments or GET-response fields - not just the
ones already traced by name. Not yet cross-validated against a named
command's own known record count (would need `ACQuisition`'s own
`[0x604]`-space ID, not yet determined - its table-3 position (`11`)
and its `[0x604]`-space ID appear to be different numbering schemes).

### A second, larger structure found while tracing this: `[0x73E]`/`[0x746]`/`[0x742]`/`[0x74A]` - the lettered reference-memory (`REFA`-`REFZ`) record array, and a strong `REFProt`/`REFStat? PROTect` identification (found 2026-10-13)

While confirming `[0x702]`, grepped the remaining "not yet
characterized" section-7 entries and found `[0x73E]`/`[0x746]` are
each referenced **85-105 times** across a large block of code
(physical roughly `0x8B1E8`-`0x8CD94`, lines ~15826-20117 of the
listing) - clearly a central, heavily-used structure. Tracing a
cluster of small sibling functions with no static callers
(`FUNC_2998_B1E8` through `FUNC_2998_B622`, same
no-caller-found/register-indirect-dispatch shape already seen for
other comm-ROM sibling clusters, e.g. "block 7" in section 5 and the
dense-cluster functions in section 9) resolved this:

- **`[0x73E]` is a far pointer to a 12-byte-stride, 26-record array -
  one record per lettered reference memory `REFA`-`REFZ`.** Confirmed
  by `FUNC_2998_C85B` (physical `0x8C85B`), a separate entry point
  that takes a raw ASCII lowercase letter in `[bp+6]` (not the numeric
  selector below) and computes the record index directly as
  `letter - 'a'` (`add dx, -0x61`), landing in `0..25` - then uses that
  same index, times stride `0xC`, against `[0x73E]`, and times stride
  `0x4E` (78) against the parallel array `[0x742]` (see below). This
  pins both arrays' record count at 26 and confirms they're indexed in
  lockstep by the same per-letter index.
- **`[0x5F8]` is the command-level reference-memory selector**: `1-4`
  = the four numbered references `REF1`-`REF4`; `5-30` = the 26
  lettered references `REFA`-`REFZ`. It's resolved into `[0x73E]`/
  `[0x742]`'s zero-based record index via a helper chain
  `SUB_93B1C` -> `SUB_93B7E` -> `SUB_96126`; the index arithmetic is
  consistent with a plain `[0x5F8] - 5` once resolved (matching
  `FUNC_2998_C85B`'s independent `letter - 'a'` math one-for-one, since
  letter `'a'` = lettered reference #1 = `[0x5F8]` value `5`).
  Different call sites range-check `[0x5F8]` differently depending on
  whether numbered references are a valid target for that particular
  operation (see below); out-of-range writes `0x15` to the generic
  comm-ROM error-status byte `[0x623]` (already documented via
  `VARIABLES.md`'s `[0x5F4]` row as holding several unrelated codes -
  **not** confirmed to be the same `0x15` as the separate, still-open
  `[0x1B83]`/`MESsage` dead-code question elsewhere in this doc, which
  concerns a different variable entirely).
- Record field `+0xA` (of `[0x73E]`'s per-letter record) is a **flag
  byte encoding that reference's write-protection state**:
  `FUNC_2998_B3A3` (the GET/query-side resolver) tests bit `0x04`
  first - **correction, 2026-10-13: it's the *clear* case that's
  tested first, not the set case as originally transcribed here** -
  bit `0x04` *clear* -> keyword-table-1 offset `+0x207` (`"UNLock"`);
  else (bit `0x04` set) test bit `0x40`: *set* -> `+0x14D` (`"PERM"`);
  *clear* -> `+0x11A` (`"LOCk"`). It special-cases `[0x5F8]` in `1..4`
  (the numbered references) to return `"UNLock"` directly without
  touching the array at all - numbered references have no protection
  state.
- **Three more sibling SET-side functions identified (2026-10-13,
  later pass), with one re-identified away from `REFProt`**:
  `FUNC_2998_B252` (`and`s the `+0xA` byte with `0xBB`, clearing bits
  `0x04` and `0x40` together) and `FUNC_2998_B2BC` (clears `0x40` via
  `and 0xBF`, then sets `0x04` via `or 4`) are both range-checked to
  `5..30` only (same lettered-only restriction as the GET-side's
  implicit split) and are the two best-fitting candidates for
  `REFProt`'s `LOCked`/`UNLocked` SET operations respectively.
  `FUNC_2998_B339` (`or`s `0x44` - both bits - without clearing
  anything first) is **also** range-checked to `5..30`; originally
  reassigned here to a *different* command, `REFDisp REF{A-Z}:EMPTY`
  ("erases the named reference if it is not protected,"
  `docs/programming-manual.md` line 852), reasoning it would otherwise
  be "redundant with `B2BC`." **Reverted, 2026-10-13 (see `PERM`'s
  SET-side resolved, below): that reassignment was a mistake.**
  `B2BC` sets bit `0x04` *alone* (`LOCk`), not both bits - `B339`
  setting *both* `0x04` and `0x40` unconditionally was never redundant
  with it, and matches `PERM` (both bits set) exactly under every
  version of the `B3A3`/`B568` bit-mapping, old and corrected alike.
  `FUNC_2998_E1E9` (found later the same day, see below) turned out to
  be the far better `REFDisp REF{A-Z}:EMPTY` fit anyway (a full record
  wipe, not a bit-OR), freeing `B339` to go back to being `REFProt`'s
  third, previously-"not found" SET-side: **`B252` (both bits clear) =
  `UNLocked`, `B2BC` (`0x04` alone) = `LOCk`, `B339` (both bits set) =
  `PERM`** - a clean, structurally identical three-function family
  (same `5..30` range check, same `SUB_93B1C`->`SUB_93B7E`->
  `SUB_96126` resolver/interlock chain, same `[0x603]` dirty-flag set on
  success - confirmed by direct read of `B339`'s body, physical
  `0x8B339`-`0x8B3A2`), differing only in which bits the final
  instruction touches.
- **Correction (2026-10-13, later pass) to the `SUB_93B1C` ->
  `SUB_93B7E` -> `SUB_96126` resolver-chain description above**: direct
  reads of all three functions' bodies show the "index arithmetic
  consistent with `[0x5F8] - 5`" phrasing was an imprecise blend of two
  different functions' work, and that `SUB_96126` is not an index
  resolver at all:
  - `SUB_93B1C` (physical `0x8BB1C`) is a plain switch on its one word
    argument: `0`/`1`/`2`/`3` pass through unchanged, `4` (`REF4`) maps
    to `1` (**the same output as `REF1`** - a genuine, directly-read
    anomaly, not an index/offset computation and not yet explained),
    and anything else (the raw `5..30` lettered range) computes
    `(input + 0x5C) & 0xFF`, which for `5..30` lands exactly on ASCII
    `'a'..'z'`. It converts the numeric selector to a **letter
    character**, not to an array index.
  - `SUB_93B7E` (physical `0x8BB7E`, immediately following `SUB_93B1C`
    in the listing) is the actual letter-to-index step: it takes a
    byte, and if it's in `0x61..0x7A` (`'a'..'z'`) returns
    `byte - 0x61` (the `0..25` record index); otherwise returns the
    sentinel `0xFFFF`. This is the function doing the `-5`-equivalent
    math, operating on `SUB_93B1C`'s *output*, not on `[0x5F8]`
    directly.
  - `SUB_96126` (physical `0x8E126`) takes **no arguments** and does
    not touch the resolved index at all. It briefly disables
    interrupts, reads far pointer `[0x746]`'s pointed-to structure's
    words at `+0`/`+2`/`+4`, and: if all three are zero, writes status
    code `0x6D9` to a newly-identified variable `[0x690]`, sets that
    structure's `+0` word to `1`, and returns `0`; otherwise writes
    `0x6E4` to `[0x690]` and returns `1`. This is a simple
    test-and-set **interlock/busy flag** guarding exclusive access to
    the `[0x746]`-pointed pool-header structure, unrelated to
    letter/index resolution despite sitting third in the call sequence
    at every observed call site.
  - At every call site seen so far (`FUNC_2998_B252`/`B2BC`/`B339`,
    below), `SUB_96126`'s return value gates the whole operation: `0`
    (interlock was free) lets the function proceed to use the
    `SUB_93B7E`-resolved `0..25` index against `[0x73E]`; any other
    value (interlock already busy) skips straight to error code
    `0x21` without touching the record at all. On the success path,
    after modifying the `[0x73E]` record's `+0xA` flag byte, all three
    functions explicitly reset the `[0x746]`-pointed structure's `+0`
    word back to `0` (releasing the interlock `SUB_96126` had just set)
    and `or`-set bit `0x01` of `[0x603]` (a separate dirty/redraw-style
    flag from `FUNC_2998_B622`'s `[0x736]` bit `0x200`). A companion
    accessor, `FUNC_2998_E1AF` (physical `0x8E1AF`), `memcpy_far`s 14
    (`0xE`) bytes out of the same `[0x746]`-pointed structure into a
    caller buffer - consistent with (and not contradicting) the
    already-documented `+6`/`+8` "free-space count"/"pool base
    pointer" fields below, now pinned at a 14-byte total structure
    size (7 words, `+0` through `+0xC`).
  - **`[0x690]` resolved as a much larger family of codes, and its
    "real-world text" question answered (2026-10-13)**: `SUB_96126`'s
    two writes (`0x6D9`/`0x6E4`) are not the whole picture. Grepping
    every `[0x690]` access across both ROM listings finds **41 distinct
    write sites total, each storing a different literal word value**
    (`0xF4` through `0x6E4`, strictly increasing with address), spread
    across only six functions: `FUNC_2998_C71C` (= `SUB_9471C`, the
    acquire-for-read function traced below), `FUNC_2998_C85B` (the
    per-lettered-reference entry point identified above - see the
    `SAVeref` correction immediately below),
    `FUNC_2998_D1AA`, `FUNC_2998_D476`, `FUNC_2998_DB69`, and
    `SUB_96126` itself - i.e. each of these functions sets a different
    fine-grained reason code at nearly every one of its own internal
    exit points, not a shared 2-state flag. **No read site for
    `[0x690]` exists anywhere in either ROM's disassembly** (checked
    for every access pattern other than the literal-immediate writes
    above), and none of the 41 write sites loads the `0xFF7B` string-
    table segment first, so there is no firmware-internal text these
    codes map to - the "real-world text... not yet looked up" framing
    was the wrong question. **Correction, 2026-10-13, later pass**: the
    premise "`C85B` is a query-response builder" (below) doesn't hold up
    - direct re-reading of its body (see the `SAVeref` identification
    below) shows it's a SET-side store/write handler, not a GET-side
    query-response builder, so the `STATUS <code>;` hypothesis's
    supporting evidence needs revising even though the hypothesis
    itself isn't ruled out: if `[0x690]` really backs that
    substitution, its caller must be reachable from a *command*-processing
    path (e.g. a deferred-error-reporting mechanism for a `SAVeref`/
    `REFProt`-style command, not literally a live `?`-query handler).
    Still the leading hypothesis, still unconfirmed, still missing its
    actual caller (the comm ROM's "block 6"/"block 7" shared
    function-reference tables - section 5 above - were checked
    exhaustively for a reference to any of these six functions' physical
    addresses and found none, as was the first 6-record span of section
    4's flag-dispatch table past its documented stride; the caller
    remains genuinely not found by any mechanism tried so far).
  - Two functions immediately following `SUB_96126` in the listing,
    `FUNC_2998_E1AF` (above) and **`FUNC_2998_E1E9`** (physical
    `0x8E1E9`), share its no-static-caller shape. `FUNC_2998_E1E9`
    takes a single `0..25` record-index argument and: zeroes
    `[0x73E]`'s record words at `+0`/`+2`/`+4`/`+6`/`+8` and sets its
    `+0xA` byte to the literal value `1` (a **fourth** distinct value
    for that byte, not matching any of the `0x04`/`0x40`/`0x44`
    protection-state masks `FUNC_2998_B3A3`/`B568` decode); then
    `memset_far`s the *entire* corresponding 78-byte `[0x742]` record
    to zero. This is a full record wipe across both arrays, not a
    protection-bit toggle - a **stronger-fitting candidate for
    `REFDisp REF{A-Z}:EMPTY`** than `FUNC_2998_B339` (which only sets
    protection bits `0x44` and never clears `[0x742]`'s waveform data
    at all). **Resolved, 2026-10-13, later pass: this is exactly the
    evidence that sends `FUNC_2998_B339` back to `REFProt REF{A-Z}
    :PERM`** (see the corrected bullet above) rather than leaving it
    "uncertain" - with `E1E9` taking the `EMPTY` role cleanly, `B339`'s
    own shape (bit-OR only, no record wipe, identical structure to
    `B252`/`B2BC`) no longer needs a second home and fits its original,
    bit-mapping-confirmed identity. `[0x73E]`'s record therefore has at
    least 5 more word fields (`+0`/`+2`/`+4`/`+6`/`+8`) beyond the
    already-documented `+0xA` flag byte, whose normal (non-wiped)
    contents are not yet characterized.
- **`B3A3`-vs-`B568` bit-mapping inconsistency, resolved as mostly a
  transcription error (2026-10-13)**: re-read `FUNC_2998_B3A3`'s raw
  instruction sequence directly (the bullet above was itself wrong
  about which branch of its own bit-`0x04` test was which - now
  corrected). With the corrected mapping, `B3A3` (clear->`UNLock`;
  set & `0x40` clear->`LOCk`; both set->`PERM`) and `FUNC_2998_B568`/
  `FUNC_2998_D067` (`(byte & 0x44) == 0`->`'U'`; `== 4`->`'L'`; else->
  `'P'`) **agree on 3 of the 4 possible raw-byte states**: all-clear
  (`UNLock`/`U`), bit `0x04` set alone (`LOCk`/`L`), and both bits set
  (`PERM`/`P`). They disagree only on the fourth, bit-`0x40`-set-alone
  state (`0x04` clear, `0x40` set): `B3A3` would call it `UNLock`
  (its bit-`0x04` test short-circuits before ever looking at `0x40`),
  while `B568` would call it `PERM`. This is a genuine latent
  difference, not just a doc error - but harmless in practice *if*
  nothing ever produces that raw byte value. None of the three known
  `+0xA`-byte-modifying functions (`FUNC_2998_B252`/`B2BC`/`B339`)
  produce it: all three always touch bit `0x04` and `0x40` together
  (`and 0xBB`, `and 0xBF`+`or 4`, `or 0x44`), never bit `0x40` alone. A
  whole-ROM grep for an immediate `or ..., 0x40` (the simplest way to
  set that bit alone) found **no matches at all** in the comm ROM -
  ruling out one easy hypothesis for `PERM`'s still-missing SET-side
  and mildly reinforcing the existing guess that `PERM` protection may
  not be set via a simple immediate-OR flag toggle in this ROM at all
  (consistent with "permanent" protection being meant to be one-way,
  possibly requiring a different mechanism such as a register-computed
  OR, a value loaded as a full replacement byte, or code outside this
  ROM entirely). If `PERM`'s SET-side is ever found setting bit `0x40`
  alone, that would both confirm this latent discrepancy is reachable
  and mean `B3A3` misreports that state as `UNLock` - a real firmware
  quirk, not just a docs artifact.
- `FUNC_2998_B600` (physical `0x8B600`) divides `[0x746]+6` by `1024`
  and prints it as a signed decimal - shaped like a "KB free" readout
  for the `[0x742]` waveform-data heap.
- **`FUNC_2998_B622` (physical `0x8B622`) re-examined via Ghidra's
  decompiled C (`decompile/exports/160-2998-14.c` line 9471,
  `FUN_0008_b622`) - it is not a simple cross-check, it's the actual
  validating SET-side implementation that performs a reference-memory
  copy, strongly reinforcing (not just weakly corroborating) the
  `DATa SOUrce`/`DATa TARget` candidate identity for `[0x4F6]`/`[0x5F8]`.**
  Decompiled logic (the string-literal artifacts `*"B"` in the raw
  Ghidra output are a known segment-addressing quirk - see
  `docs/architecture/ghidra-project.md` - and resolve to `[0x5F7]`;
  confirmed against the `.lst` disassembly directly):
  - Validates `[0x5F8]` (target selector) against the device-variant
    byte `[0x732]+0x1F` (`[0x1B83]`) and the new byte `[0x5F7]`: when
    the variant byte `== 0x1E`, requires `[0x5F7] == 4` and
    `1 <= [0x5F8] <= 30` (the full numbered+lettered range), else
    errors `0x15` (or `7` if `[0x5F7] == 0`, a distinct error code).
    On any other variant value, if `[0x5F7] != 0` it only accepts
    `[0x5F8]` in `{0, 1, 4}` (erroring `0x15` otherwise), and - whether
    that check passed or was skipped entirely because `[0x5F7] == 0` -
    it then **unconditionally forces `[0x5F8] = 4` and `[0x4F6] = 0`**
    (target = `REF4`, source = live acquisition) regardless of the
    caller's requested values. This is an exact match for the manual's
    "REF4 is the only available reference memory for the 2220 and 2221
    instruments": on those (and any other non-`0x1E`-variant) models,
    any in-range request silently collapses to `ACQ`->`REF4`.
    **`[0x5F7]` is therefore a model/option feature-gate byte for
    lettered-reference support** - `4` enables the full `REFA`-`REFZ`
    range (paired with variant `0x1E`), `0` appears to mean "no
    reference-selection option at all" (skips validation, forces the
    `ACQ`/`REF4` default, and reports the distinct error code `7` when
    the variant byte would otherwise have demanded `[0x5F7] == 4`).
    Exact real-world meaning of the value `4` itself (vs. e.g. `1`,
    `2`, `3`, never observed being tested here) is not pinned down.
  - After validation, resolves **both** `[0x4F6]` (source) and
    `[0x5F8]` (target) through `SUB_93B1C` - the same resolver already
    confirmed for `[0x5F8]` in the `REFProt` writeup above - confirming
    `[0x4F6]` is not merely a 2-state `ACQ`/`REF` boolean (as
    `FUNC_2998_B1E8`/`B1FC`/`B227`'s narrower report path treats it,
    below) but can hold the **full** `0`(ACQ)/`1..4`(numbered)/
    `5..30`(lettered) source selector, using the identical encoding as
    `[0x5F8]`.
  - Rejects the specific combination `{[0x4F6], [0x5F8]} == {1, 4}`
    in either order (source=`REF1`+target=`REF4`, or vice versa) with
    error `0x15` - a hardware/format constraint between those two
    specific numbered references, not yet explained.
  - For `[0x4F6] == 0` (`ACQ` source): requires `[0x5F8]` in `1..4`
    (numbered only - **lettered references cannot be a copy target
    when the source is live acquisition**, in this reading); on
    success calls `draw_pending_line_segment`/`SUB_93925`/
    `reset_plot_home_or_acq` (physical `0x9C35`-ish - **not yet named**)
    - a live-acquisition-to-reference copy that also drives the
      on-screen plot, consistent with the front-panel/GPIB "store to
      reference" operation completing immediately rather than queuing.
  - For `[0x4F6] != 0` (a reference source, numbered or lettered):
    branches on whether the source is lettered (fetches its waveform
    pointer via `SUB_9471C` keyed by the resolved source index, then
    `SUB_94D9C`) or numbered (checks a capability bit `0x02` in a
    per-reference pair of bytes inside the `[0x732]`-pointed struct at
    `+0x7E`+index, erroring `0x1B` if absent, then reads a per-slot
    byte at a `0x10`-stride table based at physical `0x18C`). Both
    paths derive a `1`-or-`4` category value (`local_1a`, role not
    confirmed - plausibly a record-length class) from a capability bit,
    which - **only when either endpoint is `REF1` or `REF4`** (the
    `local_20` value computed from `[0x4F6]`/`[0x5F8]` being `1` or
    `4`) - must exactly match that endpoint's own category, else errors
    `0x1B`; otherwise calls `FUNC_2998_7609` (physical `0x87609`, not
    yet named) to perform the actual reference-to-reference copy, then
    `reset_plot_home_or_acq`. On success, if the target is numbered
    (`local_14 != 0`) it also sets bit `0x200` in a word at the far
    pointer `[0x736]` - plausibly a "redraw/refresh pending" flag.
  - `SUB_93925`, `SUB_9471C`, `SUB_94D9C`, and `FUNC_2998_7609` are
    all newly-surfaced, not-yet-individually-traced callees; their
    exact roles (beyond "performs the copy"/"fetches the waveform
    pointer") are open items.
  - **`SUB_93925` traced (2026-10-13, ground-truth re-trace) = physical
    `0x8B925`**, called with `[bp+6]`/`[bp+8]` = the same source/target
    `1`-or-`4` "category" values from `B622`'s analysis above. It's a
    post-validation **display-mode/acquisition-state update
    dispatcher**: branches on the `(source, target)` category pair
    (`(1,1)`, `(4,1)`, and a default/else case gated by a per-reference
    capability bit at `[0x732]+0x80`/`+0x81` bit `0x02` and byte
    `[0x19C]`), each branch calling some combination of
    `update_display_mode_flags`, `print_and_reset_acq_buffers`
    (argument `1`, `2`, or `3`), `disable_item_pair` (argument `1`,
    `2`, or `3`), `SUB_97C95`/`SUB_97C6E` (a poll-and-retry loop around
    `restart_current_task`), and - on every successful branch -
    `or`-setting bit `0x200` of `[0x736]` (confirming, independent of
    `FUNC_2998_B622`'s own direct set, that this is the shared
    "redraw/refresh pending" flag). One branch instead falls through to
    an error path writing `0x1B` to `[0x623]`. The real-world meaning
    of each category-pair branch (which UI/acquisition state transition
    it corresponds to) isn't pinned down - this is confirmed at the
    level of "what it calls and when," not "why."
  - **`SUB_9471C`/`SUB_94D9C` traced (2026-10-13, ground-truth re-trace)
    = physical `0x8C71C`/`0x8CD9C`** - a matched acquire/release pair
    for a lettered reference's waveform data, confirming `[0x746]+2`/
    `+4` are genuine reference counters rather than dirty-flag fields
    (correcting the characterization below):
    - `SUB_9471C` takes a raw ASCII letter byte `[bp+6]` (duplicating
      `SUB_93B7E`'s `letter-0x61` math inline rather than calling it)
      and a caller output-status far pointer `[bp+8]`. It checks
      `[0x73E]`'s record `+0xA` bit `0x02` first - if clear, returns a
      null pointer with status `2` (`[0x690]=0x117`): "this letter has
      no waveform data at all," a capability bit distinct from the
      `0x04`/`0x40`/`0x44` protection-state bits and `E1E9`'s wipe
      sentinel `0x01`. If set, it disables interrupts and checks
      `+0xA` bit `0x10` OR `[0x746]+4 != 0` OR `[0x746]+0 != 0` - any
      true means "busy" (status `1`, `[0x690]=0xF4`, no pointer
      returned); otherwise "available" (status `0`, `[0x690]=0xFA`):
      it sets `+0xA` bit `0x08`, increments the record's own `+8`
      field, and increments `[0x746]+2`, then returns a far pointer to
      the letter's `[0x742]` record (`base + index*78`).
    - `SUB_94D9C` is the inverse: given the same raw letter byte (not
      a pre-resolved index; returns `0` directly for anything outside
      `'a'..'z'`), it reads `+0xA` and tests bits `0x18`
      (`0x08|0x10`) - if neither is set, returns error `8` ("nothing
      to release"). If bit `0x08` is set (a read-acquired handle,
      checked ahead of `0x10` when both happen to be set): calls
      `validate_gpib_record_consistency`, then with interrupts
      disabled **decrements** `[0x746]+2` and the record's own `+8`
      field; only once that `+8` field reaches `0` does it clear
      `+0xA` bit `0x08` - i.e. `+8` is itself a per-record outstanding
      handle count, and `[0x746]+2` is the equivalent pool-wide total
      across all 26 letters. If bit `0x08` is clear (only `0x10` set):
      calls `build_gpib_message_checksum` instead, then clears `+0xA`
      bit `0x10` and decrements `[0x746]+4` - a second, separately-
      tracked counter. Both branches return their respective helper
      call's status code; the invalid-letter and "nothing to release"
      paths return `0`/`8` directly. `retf 2` confirms a single
      word-sized argument (the letter).
    - Together these resolve `[0x746]+2`/`+4` as **two independent
      pool-wide outstanding-handle counters** - `+2` for "read-
      acquired" handles (bit `0x08`, backed by each record's own `+8`
      sub-count), `+4` for "exclusively-locked" handles (bit `0x10`,
      with no per-record sub-count observed) - and explain `SUB_96126`'s
      "`+0`/`+2`/`+4` all zero" idle test from the correction above: it
      is checking that no reference is currently checked out for
      reading *and* no reference is exclusively locked, in addition to
      no other caller already holding its own `+0` interlock, before
      letting `B252`/`B2BC`/`B339` touch a record's protection bits.
      `SUB_9471C` only ever sets bit `0x08`/increments `+2` - no
      function setting bit `0x10`/incrementing `+4` (the presumed
      "exclusive lock acquire" counterpart to `SUB_9471C`) has been
      found yet; it's either not yet traced or reached through a
      different entry point than this cluster. Plausible but unproven
      reading of why `SUB_94D9C` calls two differently-named helpers on
      release: re-validating consistency when giving up read access
      (`validate_gpib_record_consistency`) vs. rebuilding a checksum
      when giving up exclusive/write access (`build_gpib_message_
      checksum`), consistent with those functions' own documented
      roles in `FUNCTIONS.md` - but this is inference from naming and
      timing, not a traced causal link.
  - **Correction (2026-10-13, ground-truth re-trace) to the "`1`-or-`4`
    category value... from a capability bit" phrasing above**: direct
    reads of `FUNC_2998_B622`'s own body (not just its callees) show
    this was imprecise - there are **two distinct, independently-
    computed values**, not one, and neither comes directly from a
    capability bit:
    - A **source/target "format class"** `[bp-0xa]`/`[bp-0x12]` (one
      computed for `[0x4F6]`, one for `[0x5F8]`, right after the
      `SUB_93B1C` calls): `0`/`1`/`2`/`3` pass through unchanged for a
      selector in that range, a selector of `4` maps to `1` (the same
      `REF4`->`REF1` alias already seen in `SUB_93B1C` - consistent,
      since by this point `[0x4F6]`/`[0x5F8]` have *not yet* been
      converted to letters, so this is a second, independent place the
      alias is applied), and anything `>4` (the lettered range) maps to
      `0` - i.e. lettered sources and the live acquisition (`ACQ`,
      value `0`) collapse to the *same* class `0` here.
    - A separate **"either endpoint is REF1-or-REF4" priority value**
      `[bp-0x1e]`: `1` if `[0x4F6]==1` or `[0x5F8]==1`; else `4` if
      `[0x4F6]==4` or `[0x5F8]==4`; else `0` (neither endpoint is
      numbered-and-exactly-1-or-4). This is the value actually passed
      to `SUB_93925` (as its `[bp+8]` target-category argument,
      alongside `[bp-0x12]` as `[bp+6]`) on the immediate-`ACQ`-source
      path - correcting the earlier description of `SUB_93925`'s own
      arguments as a single shared category.
    - For the non-`ACQ`, reference-source path: when the source is
      **numbered** (`[bp-0xa]` in `1..3`, i.e. original `[0x4F6]` in
      `{1,2,3,4}` since `4` aliases to `1`), the `[0x732]+0x7E`-indexed
      capability-bit check gates a read of **one byte** from the
      `0x10`-stride `[0x18C]` table (indexed by `[0x4F6]`'s *class*,
      not the raw selector) - its bit `0x01` sets a new **record-
      format-class value** `[bp-0x18]` to `4` (set) or `1` (clear).
      When the source is **lettered** (`[bp-0xa]==0` and `[0x4F6]>4`):
      instead calls `SUB_9471C`/`SUB_94D9C` back-to-back (acquire then
      immediately release) purely to peek at the fetched `[0x742]`
      record's own first byte (offset `+0`) - bit `0x01` of *that*
      byte sets the same `[bp-0x18]` to `4` or `1`, by the identical
      rule. Either way, `[bp-0x18]` ends up a `1`-or-`4` **record-
      format class**, derived from a format-class capability bit
      specific to the source endpoint (table-driven for numbered,
      record-embedded for lettered) - not from `[0x4F6]`/`[0x5F8]`
      directly.
    - The actual compatibility gate: **only when `[bp-0x1e] != 0`**
      (either endpoint is specifically `REF1` or `REF4`) must
      `[bp-0x18]` exactly equal `[bp-0x1e]` (both `1`, or both `4`) or
      the copy is rejected with error `0x1B`. When `[bp-0x1e]==0`
      (neither endpoint is `REF1`/`REF4`), this check is **skipped
      entirely** and the copy proceeds unconditionally. This is a
      **separate mechanism** from the earlier, unconditional
      `{[0x4F6],[0x5F8]} == {1,4}` pairing rejection (error `0x15`,
      checked before any of this) - together they strongly suggest
      `REF1` and `REF4` are fixed, format-specific storage slots (one
      "short"/class-`1`, one "long"/class-`4`, which is which not
      pinned down) that reject any copy whose source's actual record
      format doesn't match the specific numbered endpoint involved,
      while lettered references and `REF2`/`REF3` are more flexible.
      This substantially narrows (without fully explaining the
      hardware/format reason for) the previously-open "`{REF1,REF4}`
      pairing rejection's rationale" item.
    - On success (either the match-required or match-skipped path),
      both branches call the same sequence: `draw_pending_line_segment`
      (arg `3`) -> `FUNC_2998_7609` (physical `0x87609`, args: resolved
      source letter `[bp-7]`, resolved target letter `[bp-0xf]`, and a
      status-out far pointer) -> `reset_plot_home_or_acq` (arg `3`);
      any nonzero status from either call sets error `0x1B`, else (if
      `[bp-0x12] != 0`, i.e. target is numbered) `or`-sets `[0x736]`
      bit `0x200` (the shared redraw-pending flag, confirmed again).
  - **`FUNC_2998_7609` traced (2026-10-13)** - the actual
    reference-to-reference copy executor. Dispatches on its resolved
    source-letter argument: `1..3` (numbered `REF1`/`REF2`/`REF3`, and
    transitively `REF4` too, since it was already aliased to `1` by
    `SUB_93B1C` *before* reaching this function - this function cannot
    distinguish `REF1` from `REF4` at all) calls `FUNC_2998_6B4B`
    (below) to translate the numbered reference into a `[0x742]`-
    record-shaped descriptor on the stack; anything else (a lettered
    `'a'..'z'` byte) calls `SUB_9471C` directly to fetch the real
    `[0x742]` record pointer for that letter. Both paths converge on
    shared logic: a target-format compatibility check (reads
    `[0x1BE4]`/`[0x1BE5]` and sums `[0x46]`/`[0x4A]` against a `0xFFF`
    threshold to decide whether to call `disable_item_pair` once more),
    then `print_and_reset_acq_buffers`, a `SUB_97C95`/`SUB_97C6E`-gated
    poll loop around `restart_current_task` (identical shape to
    `SUB_93925`'s own retry loop), and finally the actual payload
    transfer: pointer arithmetic against three more absolute tables
    (`[0x1C02]`, `[0x1C80]`, `[0x1C94]`) to locate the destination
    record's base, a `memcpy_far` of a small fixed header-ish region,
    and then a byte-by-byte scan of the source payload counting bytes
    whose low 2 bits equal `3` or `1` to recompute a record/point
    count pair written into the destination record's `+0x32`/`+0x34`
    fields. This last scan is a genuinely new, not-previously-
    documented structural finding: **individual stored waveform-point
    bytes carry a 2-bit status tag in their low bits**, and this
    routine re-derives record/point boundaries from that tag when
    repackaging data into the `[0x742]` record format - plausible
    candidates for the tag's meaning (segment-start/segment-end/
    continuation markers) are not confirmed; flagging as open rather
    than guessing further.
  - **`FUNC_2998_6B4B` traced (2026-10-13)** = physical `0x86B4B`, the
    numbered-reference-to-record translator called by `FUNC_2998_7609`
    above. Given a numbered-reference class (`1..3`) and an output far
    pointer, it gates on the same per-reference capability-bit-pair
    table `FUNC_2998_B622` already consults via `[0x732]+0x7E`+index -
    here referenced directly at its **absolute address `[0x1BE2]`**,
    which arithmetically confirms `0x1BE2 == 0x1B64` (the already-known
    `[0x732]` resolution) `+ 0x7E` exactly, tying the two references
    together as the literal same byte for the first time rather than
    just "same offset, presumably same table." It then fills the
    output record with fields pulled from several more absolute
    main-ROM tables - `[0x1DC]`+index, a 10-byte-stride table at
    `[0x1C14]`, the same `[0x1BE2]` pair copied into `+0x26`/`+0x27`,
    and 14-byte-stride tables at `[0x576]`/`[0x578]`/`[0x580]` - into
    output offsets `+0x10` through `+0x31`, consistent with assembling
    a `[0x742]`-style 78-byte descriptor for a numbered reference's
    data from scattered per-reference tables rather than a single
    contiguous struct (unlike lettered references, which already have
    one). The remainder of the function (from offset `+0x32` onward)
    performs the same point-tag-scanning length computation described
    in `FUNC_2998_7609` above. Individual table fields' real-world
    meaning (beyond "feeds a `[0x742]`-shaped record") not
    characterized one-by-one.
  - **Point-tag-scan mechanism traced to its exact logic (2026-10-13)**,
    closing out the "2-bit status tag" open item above with a solid
    mechanical (if not yet semantic) answer. The scan is gated by bits
    `0x30` of the **same** `[0x18C]`+class*16 table byte `FUNC_2998_
    B622` already reads bit `0x01` from (one more confirmed overlap
    between the two functions' table usage): when `(byte & 0x30) ==
    0x20`, the scan walks the source buffer **one byte at a time**;
    otherwise it walks it **two bytes at a time** (skipping the low
    byte of each pair) - i.e. this field encodes whether each stored
    sample occupies one byte or two (8-bit-packed vs. 16-bit/word
    samples), selecting the scan's stride accordingly. Within either
    stride, the scan is a sequential "skip until marker" walk, not a
    running tally: it advances through the buffer counting every byte
    it passes, stops counting (and stores the count so far into
    `+0x32`) the moment it hits a byte whose low 2 bits equal `3`, then
    resumes counting from there and stops again (storing that second
    count into `+0x34`) at the first byte whose low 2 bits equal `1`.
    So the two output fields are **distances-to-first-occurrence** of
    two distinct marker values embedded in the sample stream, not
    independent tallies of how many `3`-tagged or `1`-tagged bytes
    exist overall. This is consistent with the tag marking sequential
    structural boundaries (e.g. "end of this record" = `3`, then some
    second marker = `1`, read off positionally) rather than a simple
    per-byte classification - but the real-world meaning of *why* `3`
    comes first and `1` second, and what either specifically denotes,
    remains unconfirmed.
  - **`FUNC_2998_6F6F` identified (2026-10-13)**, physical `0x86F6F`,
    immediately adjacent to `FUNC_2998_6B4B` in the listing and its
    exact **inverse**: a scatter/"install" counterpart where
    `FUNC_2998_6B4B` is a gather/"translate" function. It has a single
    caller, inside `FUNC_2998_7609` itself (the call at physical
    `0x87AC6`), reached after `7609`'s payload-copy-and-count-recompute
    sequence finishes assembling a local `[0x742]`-shaped record on its
    own stack (`[bp-0x5c]`) describing the just-written destination
    data. `7609` passes that local record and the destination's
    numbered class (`1..3`) to `FUNC_2998_6F6F`, which reads the
    record's fields back out (capability bits at `+0x10`/`+0x11` and
    `+0x26`/`+0x27`, a block at `+0x1C`, and fields at `+0x28` through
    `+0x4B`) and writes them into the very same scattered absolute
    tables `FUNC_2998_6B4B` reads from to build such a record in the
    first place (`[0x1DC]`, the 10-byte-stride `[0x1C14]` table via
    `memcpy_far`, the 14-byte-stride `[0x574]`/`[0x576]`/`[0x578]`/
    `[0x580]` tables, `[0x564]`, the main per-reference array's own
    `+0x3E` field, and the `0x14`-byte-stride `b0`-`b4` table also used
    by `SUB_9471C`'s record layout). Confirms `retf 2` with the same
    stack shape as `FUNC_2998_6B4B` (word class arg + far pointer arg)
    but the **opposite data-flow direction** through that far pointer
    - input record here, output record there. Also reveals that each
    numbered reference actually occupies **two adjacent table-row
    slots** in several of these tables (indices `class*2` and
    `class*2+1`, both filled from the same source record) rather than
    one row per reference - not yet explained, but mechanically clear
    from the index arithmetic in both this function and its caller.
    **Resolved, later same day**: the two slots are not a duplicated
    copy - they're two genuinely **different, independently-tracked
    sub-items packed as adjacent field groups in the single input
    record**. Confirmed directly from `FUNC_2998_6F6F`'s own field
    reads: the capability bit written to slot `class*2` comes from
    record offset `+0x26`, while the bit written to slot `class*2+1`
    comes from the very next record byte, `+0x27` (both bit `0x02`,
    both landing in the same `[0x1BE2]`-based capability array just at
    adjacent indices - this is in fact the same "byte-pair" `6B4B`'s
    scan and `FUNC_2998_718D`'s capability check already read, just
    now seen as two adjacent indices of one array rather than two
    separate base addresses). The `0x14`-byte-stride `b0`-`b4` table
    shows the same pattern at larger scale: record bytes `+0x42`
    through `+0x46` (5 consecutive bytes) populate slot `class*2`'s
    `b0`-`b4` row, and the *next* 5 record bytes, `+0x47` through
    `+0x4b`, populate slot `class*2+1`'s row - two complete, disjoint
    5-field groups from one record, not one group copied twice. Given
    `FUNC_2998_6B4B`'s point-tag scan (above) already produces exactly
    two distances per scan (`+0x32`: distance to the first tag-`3`
    byte; `+0x34`: distance from there to the first tag-`1` byte), the
    natural - but not yet directly proven - reading is that each
    reference's record carries data for **two markers/tags** (very
    plausibly those same two scan results, or two cursor-like
    waveform-point markers derived from them), each needing its own
    capability bit, its own `b0`-`b4` descriptor, and its own row in
    the other scattered per-reference tables - hence two adjacent
    slots per reference rather than one. The real-world identity of
    "marker 1" vs "marker 2" (and how it maps to `3`-tag vs `1`-tag)
    is still unconfirmed.
    Immediately after the call, `FUNC_2998_7609` itself (not
    `FUNC_2998_6F6F`) sets a couple of related flags directly - two
    `+0xa6`/`+0xa8`/`+0xaa` fields in the `0x14`-stride table (one per
    slot) gated on the assembled record's own `+0x3A`/`+0x40` fields,
    an `|= 0xb5f` on a `[0x55A]`+class*2 flag word, and a call to the
    already-named `set_item_pair_active` - consistent with this whole
    sequence being `7609`'s final "commit the copied data as the new
    contents of numbered reference `class`" step when the copy's
    destination is numbered rather than lettered.
  - **`FUNC_2998_718D` identified (2026-10-13)**, physical `0x8718D`, a
    "step to the next/previous reference satisfying a capability bit"
    scanner found while chasing the two-adjacent-slots question above
    (it didn't explain that, but surfaced this instead). Given a
    starting reference (numbered `1..4` or lettered via `[bp+6]`), a
    signed step (`+1`/`-1` via `[bp+8]`), and a wraparound-mode flag
    (`[bp+0xa]`), it walks in that direction - up to 30 steps - testing
    each candidate's capability bit `0x02` (lettered: `[0x1BE2]`/
    `[0x1BE3]`+index*2, the same pair `FUNC_2998_B622`/`6B4B` already
    use; numbered: delegates to `SUB_95067`) and returns the first
    reference that passes, or `0` if it wraps back to the start without
    finding one. The wraparound-mode flag selects between two distinct
    behaviors: **nonzero** treats `1..4` and `'a'..'z'` as one combined
    cyclic range with explicit transition points at both ends (`'a'`<->
    `'z'`, and `4`<->`'a'`/`'z'` depending on direction); **zero**
    clamps strictly within `'a'..'z'`, never considering the numbered
    references at all.
    
    Its callers (six found, at physical `0x85F76`/`0x85FBA`/`0x86268`/
    `0x862A5`/`0x86453`/`0x86496`) all share one shape: a signed
    accumulator at `[0x4C6]` is incremented each poll by a delta read
    from `[0x52A]`, and once it crosses roughly `+250`/`-250` it resets
    to `0`, sets a "selection changed" flag at `[0x46A]`, and calls
    `FUNC_2998_718D` to step the current selection (held per-context in
    a small array at `[0x4CA]`, one byte per context) one position in
    the direction of the overflow. This is mechanically a debounced-
    encoder "step on threshold crossing" pattern; whether `[0x52A]`'s
    delta actually originates from a physical front-panel knob read by
    this comm ROM, or from something else entirely (e.g. a remote
    command simulating a step), has **not** been traced to its source
    and is flagged as open rather than assumed.
  - **`[0x52A]`'s write site searched for and not found (2026-10-13)**:
    three independent search techniques across the entire comm ROM
    disassembly all come up empty. (1) Direct-displacement stores -
    grepped every `[0x52a]` reference; all 12 are the same 6 read
    sites (a `cmp word ptr [0x52a], 0` immediately followed by a
    `mov ax, word ptr [0x52a]`, at physical `0x85F2A`, `0x86239`,
    `0x86424`, `0x87DC4`, `0x87ED7`, `0x87F58` - matching the 6
    `FUNC_2998_718D` callers above), none of them writes. (2)
    Immediate-loaded-register forms - grepped the raw byte pattern for
    the literal `0x052a` (`2a 05` little-endian) anywhere in the ROM,
    which would also catch a `mov bx, 0x52a` followed by an indirect
    store; the only hits are the same 6 read sites' own encoded
    immediates, no additional occurrences. (3) Hardware I/O - this ROM
    contains exactly two port instructions in its entire disassembly,
    `in ax, 0x5d` (physical `0x84092`) and `out 0xff, al` (physical
    `0x8C004`); neither is positioned near, or obviously feeds, any of
    the 6 `[0x52A]` read sites. Taken together, this makes a
    register-indirect write using some other, not-literally-`0x52a`
    base computation the only remaining software explanation - but
    given the value's behavior (a small signed per-poll delta that
    accumulates toward a debounce threshold, read only, never reset by
    software), the more likely explanation is that `[0x52A]` is a
    **memory-mapped hardware register** - e.g. a front-panel
    rotary-encoder up/down counter latch wired directly onto the comm
    co-processor's bus at that address - that external hardware
    updates directly, with no CPU store instruction ever executing.
    Unconfirmed (would need the schematic to verify) but consistent
    with everything found so far; recorded as the leading hypothesis
    rather than a proven fact.
- **Candidate real-world identity (strong, not yet 100% proven by a
  direct string/ID trace): `REFProt REF{A-Z}:{LOCked, PERM, or
  UNLocked}` and its counterpart query `REFStat? PROTect`**
  (`docs/programming-manual.md` lines ~852-865, ~882). The match is
  unusually clean: the three decoded keywords (`LOCk`/`PERM`/`UNLock`)
  are exactly `REFProt`'s three documented states; `REFProt` is
  documented as applying only to the lettered references `REFA`-`REFZ`
  (never the numbered ones), matching `FUNC_2998_B252`/`B2BC`'s
  shared SET-side rejection of `1..4`; and `REFStat? PROTect` is documented as
  returning a **thirty**-character `U`/`L`/`P` string "from REF1 to
  REFZ" - `4` numbered (always `'U'`, matching `FUNC_2998_B568`'s own
  hardcoding of `'U'` for its 4 numbered-reference slots, independent
  of the array) `+ 26` lettered (individually tracked via the array,
  each resolved through `SUB_95067`/`FUNC_2998_D067`) `= 30`, exactly
  matching `[0x5F8]`'s full `1..30` range.
- **`FUNC_2998_B051` identified as `REFDisp?` (confirmed by direct
  string match, 2026-10-13)**, physical `0x8B051`. Takes no stack
  arguments (plain `retf`, not `retf 2`) and reads `[0x5F8]` (the
  shared `DATa SOUrce`/`TARget` reference selector) directly. First
  validates/defaults `[0x5F8]` exactly like `REFProt`/`DATa SOU/TAR`
  do - but with the **documented `REFDisp?` default-argument rule**
  byte-for-byte: on the 2230 variant (`[0x732+0x1F]==0x1E`), an absent
  explicit argument (`[0x5F7]==0`) defaults `[0x5F8]` to `1` (`REF1`);
  on the 2220/2221 variant, it defaults to `4` (`REF4`) *and* rejects
  any other explicit value with an error - matching the manual's
  `REFDisp?`/`ALL REFDisp?` entries exactly ("returns the status of
  REF1... for the 2230; returns the status of REF4 for the 2220 and
  2221", programming-manual.md lines 866-868). For numbered references
  (`1..4`, with `4` aliased to `1` as everywhere else), it reads a
  2-bit status field from `es:[0x732+0x7E]`, indexed at **both**
  `class*2` and `class*2+1` (the same two-adjacent-slots scheme as
  `FUNC_2998_6B4B`/`6F6F`) and **ORs the two together**; for lettered
  references (`5..30`) it instead delegates to `SUB_93B1C`/`SUB_95067`
  (the same capability-bit resolver already traced) and keeps only bit
  `0x02`. The combined 2-bit result selects one of three fixed
  keyword-table-1 string offsets from `[0x6F2]` (`+0xbe`, `+0x13d`,
  `+0x141`) - read directly from `binary/160-2998-14.bin` and confirmed
  byte-exact to be **`"EMPty"`, `"OFF"`, and `"ON"`** respectively
  (value bit `0x02` clear -> `EMPty`; set and bit `0x01` clear ->
  `OFF`; both set -> `ON`). This is an exact, string-confirmed match
  for `REFDisp?`'s documented three-way response (programming-manual.md
  lines 850, 866-868) - not a guess. Since lettered references can
  never be displayed (manual: "may not be displayed... directly"),
  their path only ever yields `EMPty` or `OFF`, consistent with the
  manual's narrower `REFDisp REF{A-Z}:EMPTY`-only lettered command.
  **This also sharpens the two-adjacent-slots finding above**: ORing
  `class*2`'s and `class*2+1`'s display-status bits together to decide
  "is this reference's trace on" reads much more naturally as **the
  reference's two stored channels (`REFOrmat CHAnnel:{CH1, CH2}`)**
  than as the two point-tag-scan markers originally guessed - a
  numbered reference can apparently hold a saved trace for each of
  CH1 and CH2 independently, and is reported `ON` if either channel's
  trace is currently displayed. Not yet proven over the point-tag
  hypothesis, but now the better-fitting of the two.
- **Numbered-`REFDisp` SET-side candidates found (2026-10-13)**:
  `FUNC_2998_AD17` (physical `0x8AD17`, `retf 2`) and `FUNC_2998_AF8A`
  (physical `0x8AF8A`, `retf 2`), sitting immediately before
  `FUNC_2998_B051` in the listing. Neither is ever reached by a
  `call`/`lcall` the disassembler could resolve by name - consistent
  with the still-unfound register-indirect command dispatcher (section
  2's "code that actually walks either table has still not been
  found") - so the identification rests on behavioral fit, not a
  confirmed call site:
  - Both switch on the global `[0x5F8]` class selector (`1..4`,
    aliasing `4`->`1` for the 6B4B-gated branch exactly as elsewhere)
    and, for classes `2`/`3`, `FUNC_2998_AD17` first re-reads the exact
    same `es:[0x732+0x7E]`-indexed 2-bit status `FUNC_2998_B051` reads
    (`class*2`/`class*2+1`, ORed) and requires bit `0x02` set (i.e.
    not-`EMPty`) before proceeding - error `0x1b` otherwise. Classes
    `1`/`4` instead gate on a bit returned by `FUNC_2998_6B4B`.
  - **Neither function contains a single instruction that writes
    `es:[...+0x7E]`/`[0x1BE2]` directly** - which is why the whole-ROM
    search for that write site (12th follow-up below) came up empty.
    Both instead make a far call into shared **main-ROM** menu-item
    infrastructure, already independently identified and confirmed in
    `FUNCTIONS.md`: `set_item_pair_active` (`0xF8E66`) and
    `disable_item_pair` (`0xF8F24`), both `(pair_index)` taking the
    same class value directly. `disable_item_pair` -> `deselect_item_
    pair` -> `set_item_active_flag(2*pair_index[+1], 0)` clears what
    `deselect_item_pair`'s entry calls the "selected" bit, **then**
    `disable_item_pair` itself separately clears bit `1` ("enabled/
    valid") of `[item_index+0x1BE2]` for both items of the pair.
    `set_item_active_flag`'s own entry confirms it is the function that
    actually toggles bits in `[item_index+0x1BE2]` - and `[0x1BE2]` was
    already proven, in the 15th follow-up above, to be the exact same
    array as `[0x732]+0x7E` (`0x1BE2 == 0x1B64 + 0x7E`, and `[0x732]`
    resolves to `[0x1B64]`). So the write path is real, it just lives
    in the main ROM's generic, shared "menu item active/enabled" code
    rather than anywhere in the comm ROM - the comm ROM only ever
    reaches it through these two far calls.
  - Decoded against `FUNC_2998_B051`'s own bit mapping (`0x02` clear ->
    `EMPty`; `0x02` set & `0x01` clear -> `OFF`; both set -> `ON`):
    `FUNC_2998_AF8A` clearing *both* bits (via `disable_item_pair`)
    matches `EMPty` exactly, making it the stronger candidate for
    **`REFDisp REF{1-4}:EMPTY`**, not `:OFF` as first assumed from its
    name alone. `FUNC_2998_AD17`'s `[bp+6]!=0` branch, which calls
    `set_item_pair_active` (sets only the "selected"/bit-`0x01` side,
    guarded by the pre-existing bit-`0x02` check above - i.e. you can't
    turn a reference ON while it's `EMPty`) fits **`REFDisp REF{1-4}:
    ON`** well. `FUNC_2998_AD17`'s `[bp+6]==0` branch calls only
    `print_and_reset_acq_buffers` (no flag-bit touch at all - confirmed
    by direct read of `print_and_reset_acq_buffers`'s own body, physical
    `0xF0414` in the main ROM: it resets acquisition-buffer pointers and
    calls `update_plot_position`, nothing else; the "maybe it clears the
    bit internally" hedge below is ruled out) - **not yet matched to
    `:OFF`** or to any other documented `REFDisp` state. **Checked
    2026-10-13 whether this is path-specific**: the class-`2`/`3` branch
    (`L_8AE31`, which gates on `es:[0x732+0x7E]` directly rather than
    `FUNC_2998_6B4B`) does the exact same thing on its own `[bp+6]==0`
    side - `print_and_reset_acq_buffers` only, no `deselect_item_pair`/
    `disable_item_pair` call either. Identical treatment across both
    independently-gated branches rules out "oversight in one code path"
    and makes it look deliberate, which only sharpens the puzzle: if
    `[bp+6]==0` really is `:OFF`, turning an already-`:ON` reference
    `:OFF` would leave the "selected" bit set and the display
    unchanged, per `FUNC_2998_B051`'s own decode. Possible explanations,
    still unresolved: `AD17` isn't exclusively a `REFDisp` handler (it
    may be shared with another command using `[bp+6]` for an unrelated
    purpose), or `[bp+6]`'s boolean isn't ON-vs-OFF at all.
  - The device-variant gate at entry (`es:[0x732+0x1F]==0x14` -> error
    `0x1e`, skipping the whole function) uses a *different* variant
    byte value than the already-documented `0x1E`-for-2230 constant,
    so which real model `0x14` denotes (if it's a variant byte at all
    in this context) is unresolved - flagged, not chased further here.
  - **Class-1/class-4 gate traced in full, 2026-10-13, later pass**: the
    shared `L_8AD5B` path both classes jump into calls `FUNC_2998_6B4B`
    with its class argument **hardcoded to `1`**, not `[bp-0xa]`'s real
    normalized class - i.e. it always fetches REF1's own translated
    record, never REF4's, even on the class-4 side. It then checks bit
    `0x01` (not the `0x02` "has data" bit the class-2/3 branch checks)
    of that record's offset-`+0` byte, with **opposite required
    polarity**: class `1` requires the bit **clear** to proceed (`jne`
    to the error path at `ADEF`/error `0x22` if set), class `4` requires
    it **set** (same error if clear). This is a real, mechanically
    confirmed asymmetry, not a copy-paste artifact (both branches
    converge on the same `ADA3`/`[bp+6]` check afterward). A plausible
    reading, tying back to the programming manual's own note that "REF4
    is the only available reference memory for the 2220/2221... and
    occupies the REF1-REF3 memory locations in the 2230" (`docs/
    programming-manual.md` line 851): on this ROM's primary 2230 target,
    "class 4" may not be a distinct physical store at all but a request
    to treat REF1-3's combined space as one large waveform, so checking
    REF1's own record with the opposite bit sense would plausibly be
    testing "is REF1 free/not individually in use" (allowing the
    combined-mode request) vs. class 1's own "is REF1 already in a
    usable state." **Not confirmed** - `FUNC_2998_6B4B`'s offset-`+0`
    byte itself has no established meaning yet (its documented field
    map only covers `+0x10` through the point-tag-scan tail from
    `+0x32`); this is a plausible explanation for the asymmetry, not a
    settled one. Doesn't resolve the `[bp+6]==0`-isn't-`:OFF` puzzle
    above, which remains open.
- `FUNC_2998_B1E8`/`B1FC`/`B227` (physical `0x8B1E8`-`0x8B24B`) are a
  simpler, narrower 2-state **report** path over the same `[0x4F6]`
  byte that `FUNC_2998_B622` (above) validates/sets with the full
  selector range: `FUNC_2998_B1E8` sets `[0x4F6]` to `0` unconditionally
  (forcing `ACQ`), `FUNC_2998_B1FC` sets it to `[0x5F8]`'s low byte
  (after the same `1..30` range check) - i.e. "make the source match
  the target," a plausible `DATa TARget`-sets-`SOUrce`-too shorthand,
  not a separate flag domain. `FUNC_2998_B227` then reports
  keyword-table-1 offset `+0xE` (`"ACQ"`) if `[0x4F6]==0`, else `+0x17B`
  (`"REF"`) - collapsing `[0x4F6]`'s full `1..30` range down to a
  boolean for this particular report, consistent with `B622`'s finding
  that `[0x4F6]` is the full source selector underneath. Candidate
  identity, now reinforced by `B622`'s confirmed copy-operation
  semantics above: `DATa SOUrce`, whose query response in the manual's
  own example literally reads `DAT SOU:ACQ,TAR:REF1,...`
  (programming-manual.md line 909) - i.e. the data source is either the
  live acquisition (`"ACQ"`, value `0`) or a specific reference memory
  (`"REF"` + number, any nonzero `[0x5F8]`/`[0x4F6]`). `REFOrmat
  TARget`/`CURSor TARget` (lines 757-759, 852-872), which draw the same
  ACQ-vs-reference distinction, remain alternative candidates for a
  *different* command sharing the same keyword pair, but no longer for
  `[0x4F6]`/`[0x5F8]` themselves given `B622`'s direct validation
  behavior.
- `[0x742]` is a **parallel 78-byte (`0x4E`)-stride, 26-record array**,
  indexed in lockstep with `[0x73E]` (confirmed by `FUNC_2998_C85B`'s
  shared index into both). Its fields include far-pointer pairs
  (`+0x36`/`+0x38`, `+0x3C`/`+0x3E`) and length words (`+0x3A`,
  `+0x40`) that get `memcpy_far`'d and pointer-arithmetic'd together in
  `FUNC_2998_C85B` and its continuation (physical `0x8C7AB`-`0x8CD94`+)
  - consistent with storing the actual saved waveform curve
  data/length for each lettered reference (the data behind `REFOrmat`
  and the reference's own stored trace), not `REFProt`'s simple flag.
  Not characterized further (field-by-field).
- `[0x746]` is a 4-byte **far-pointer variable** (confirmed directly:
  every access is `les reg, ptr [0x746]` followed by `es:[reg+N]`) to a
  separate **pool-header structure**, now pinned at 14 bytes total
  (`SUB_96126`/`FUNC_2998_E126` and `FUNC_2998_E1AF`, above) with
  fields referenced at `+0`, `+2`, `+4`, `+6`, `+8` - `+8` acts as a
  pool base pointer that per-record offsets get added to; `+6` behaves
  like a free-space count/limit (`FUNC_2998_B600` divides it by 1024
  and prints it); `+0` is `SUB_96126`'s single-owner interlock/busy
  flag (above); `+2`/`+4` are **two independent pool-wide outstanding-
  handle counters** (2026-10-13, later pass: `SUB_9471C`/`SUB_94D9C`
  above) - `+2` counts "read-acquired" lettered-reference handles
  (`[0x73E]` record `+0xA` bit `0x08`), `+4` counts "exclusively-
  locked" handles (bit `0x10`) - rather than a per-`[0x73E]`-record
  dirty counter as previously guessed. `[0x74A]` is a second, separate far pointer used
  alongside `[0x746]+8` in the same compaction code; its distinct role
  from `[0x746]` itself isn't pinned down. Together these look like a
  shared waveform-data heap/compaction manager for the 26 lettered
  references' stored curve data (`[0x742]`'s payload) - a real,
  distinct subsystem neighboring the protection-flag mechanism, not
  traced field-by-field.
- **`FUNC_2998_C85B`'s full body re-read (2026-10-13, later pass):
  it's `SAVeref REF{A-Z}`'s SET-side store handler, not a "query-response
  builder" (correcting that framing above)**. Sequence, matched
  directly against the manual's `SAVeref REF{A-Z}` description ("Saves
  the waveform selected by the `REFFrom` command into the named
  reference," `docs/programming-manual.md` line 885):
  1. Calls `SUB_96126` to acquire the pool-wide single-owner interlock
     (error `0x14D` and abort if already held - a different, later-
     found use of the same interlock already covered above for
     `B252`/`B2BC`/`B339`).
  2. Resolves the destination letter (`[bp+6]`, raw ASCII `'a'`-`'z'`)
     to a `0..25` index exactly like `SUB_9471C`/`D1AA` (`letter -
     0x61`), and checks that record's `[0x73E]+0xA` flag byte bit `0x04`
     (the `LOCk`/`PERM` protection bit) - error `0x165` (status word set
     to `0x40`) and abort if set: **a locked or permanently-protected
     reference correctly refuses `SAVeref`**, matching the manual's
     implicit protection semantics for the lock family documented above.
  3. If bit `0x02` is clear (destination currently empty, no existing
     waveform), skips straight to an allocate-new-space path (not traced
     further here). If set (destination already holds data), compares
     the existing destination length fields (`[0x742]` record `+0x3A`+
     `+0x40`) against the source record's own matching fields (read from
     a far-pointer struct passed in at `[bp+8]` - the actual
     `REFFrom`-selected source, already resolved to a `[0x742]`-shaped
     descriptor by the caller before this function runs): on an exact
     length match (`[0x690]=0x177`), proceeds to overwrite in place;
     otherwise (`[0x690]` not reached on this branch in the portion
     read) takes a different, not-yet-traced path - presumably a
     reallocation, consistent with `[0x746]`'s pool/compaction role
     above.
  4. On the in-place-overwrite path: `memcpy_far`s the source's data
     into the destination's storage, updates the destination `[0x742]`
     record's pointer/length fields (`+0x36`/`+0x38`, `+0x3A`, `+0x3C`/
     `+0x3E`) from the source's own, then `or`-sets `[0x73E]+0xA` bit
     `0x10` and increments `[0x746]+4` - **the same "exclusively-locked"
     bit/pool-wide counter pair `SUB_9471C`/`SUB_94D9C` already resolved
     above**, here acquired inline (not via a separate acquire call) and
     left set on return rather than released within this function -
     consistent with `SUB_94D9C`'s bit-`0x10` release branch (which
     calls `build_gpib_message_checksum` before clearing it) being the
     actual, separate release point once the newly-written data is
     checksummed/finalized, not something `C85B` itself does. Finally
     clears `[0x746]+0` (releasing the transient pool interlock from
     step 1, per the same pattern already seen in `B252`/`B2BC`/`B339`).
  This is a coherent, self-consistent reading of the entire function
  body against already-established bit/counter semantics, not an
  isolated guess - but the unequal-length/reallocation branch and the
  very first "destination empty" branch (`L_8C99F`) weren't traced in
  this pass, so the identification is **SAVeref, strong confidence**,
  not yet a byte-for-byte-complete account of the function.
  **Completed 2026-10-13, later pass**: the remaining branches were
  traced - see "`FUNC_2998_C85B` (`SAVeref`) ruled out as
  `validate_gpib_device_records`'s caller" below, which covers the
  "destination empty" and unequal-length paths and corrects the
  "reallocation" guess (it's an insufficient-room error/`SUB_95D81`
  call, not a compaction retry).
- **`FUNC_2998_D1AA`/`D476`'s identities closed, 2026-10-13, later
  pass: they're `validate_gpib_record_consistency`/`scan_and_repair_
  gpib_records`, already named at their 0x9000-segment-aliased
  addresses (`0x951AA`/`0x95476`), just missing the matching
  `FUNCTIONAL_NAMES` entry at their own native comm-ROM address.**
  Reading `D1AA` in full byte-for-byte matches
  `validate_gpib_record_consistency`'s existing `FUNCTIONS.md`
  description exactly - confirmed, not just a plausible guess - and
  fills in detail that description didn't already spell out:
  - Takes a single already-resolved `0..25` record index (not a raw
    letter, unlike `SUB_9471C`/`C85B`), and performs 6 independent
    consistency checks, each ORing a distinct bit into a returned flags
    word and writing its own diagnostic code to `[0x690]` on failure:
    1. `0x387`/bit `1`: the two pointer offsets stored at `[0x73E]+0`/
       `+4` (each relative to the `[0x74A]` pool base) must exactly
       equal the two full far pointers stored directly in `[0x742]+
       0x36`/`+0x38` and `+0x3C`/`+0x3E` - proving `[0x73E]+0`/`+4` are
       **compact offset-encoded duplicates of the same two pointers**
       `[0x742]` stores as full far pointers, not independent fields.
    2. `0x38F`/bit `1` (shared): both of those offsets must fall within
       `0..0x6800` (the pool's full span).
    3. `0x39B`/bit `2`: `[0x73E]+2`/`+6` (two length words) must exactly
       equal `[0x742]+0x3A`/`+0x40` - **`[0x73E]` caches duplicate
       copies of `[0x742]`'s own authoritative length fields**.
    4. `0x3AB`/bit `2` (shared): the combined length (`+2`+`+6`) must be
       `0x400`-`0x2000` bytes, 1024-byte-aligned, and fit within the
       pool bound once added to the base offset.
    5. bit `0x80`: flags an internally-contradictory `[0x73E]+0xA`
       state - bit `0x01` (`init_gpib_record`'s "just wiped" sentinel)
       set together with any bit outside `0x01`/`0x04`, or bit `0x02`
       ("has data") set together with a **newly-identified bit `0x80`**
       on the same flag byte (see below - set by `scan_and_repair_gpib_
       records`, not previously characterized).
    6. bit `0x200` ("busy"): set if bits `0x08`/`0x10` (read-acquired/
       exclusively-locked) are set, or the record's own `+8` handle
       count is nonzero.
    If the accumulated flags are `0` or exactly `0x200` (no actual
    corruption found), it goes on to verify **two stored checksum
    bytes**, computed via `checksum_bytes`: `[0x73E]+0xB` (new field -
    a payload checksum over the waveform data itself, length = the
    combined `[0x742]` length fields, base = the `[0x74A]`-relative
    pointer) and `[0x742]+0x4C` (new field - a combined header checksum
    over `[0x742]`'s own first `0x4C`=76 bytes plus `[0x73E]`'s own
    first 8 bytes). Mismatches set `[0x690]=0x3EF`/bit `4` and
    `[0x690]=0x3F6`/bit `0x80` respectively.
  - `scan_and_repair_gpib_records` (`D476`) is the boot/reinit-time
    integrity scrub that drives it: on entry, if `[0x746]+0` (the pool
    interlock) or `+2`/`+4` (the two handle counters) were already
    nonzero, it force-clears them and records that fact as bits
    `0x100`/`0x200` into a **newly-identified `[0x746]+0xA`** word -
    completing the 14-byte `[0x746]` struct's field list (the last 2 of
    its 7 words, alongside a parallel `+0xC` repair counter, incremented
    on every repair this pass makes). It then loops all 26 lettered
    references: if a record's `+0xA` bit `0x01` ("wiped") is set, it
    double-checks every field that *should* already be zero really is
    (both `[0x73E]`'s and `[0x742]`'s) - if not, that's a real
    inconsistency (`[0x690]=0x444`): it force-zeroes everything,
    `memset_far`s the whole `[0x742]` record, **sets the new bit `0x80`
    on `[0x73E]+0xA`** (hence `validate_gpib_record_consistency`'s own
    check for it above), and marks the record wiped again. Otherwise it
    calls `validate_gpib_record_consistency` directly: on any failure
    (`[0x690]=0x459`), it ORs the failure bitmask into `[0x746]+0xA` and
    bumps `+0xC`, then picks a repair strategy by masking out the
    checksum-mismatch (`4`) and busy (`0x200`) bits - if nothing
    structural remains, it's a **minor failure**: calls `init_gpib_
    record` (full wipe, `[0x690]=0x462`) to reset the record outright;
    if a structural bit (`1`/`2`/`0x80`) remains, it's **more severe**:
    instead of wiping, it sets bit `0x20` on `[0x73E]+0xA` (a
    **newly-identified "needs attention/stale-checksum" flag**, see
    below) and force-clears bits `0x08`/`0x10` plus the `+8` handle
    count (forcibly releasing any stuck read/exclusive lock) without
    touching the waveform data itself.
  - **`FUNC_2998_DB69` identity closed, 2026-10-13, later pass: it's
    `validate_gpib_device_records`**, already named at its
    0x9000-segment-aliased address (`0x95B69`), via the exact same
    native-address-twin pattern as the four functions above
    (`0x8DB69` = `0x95B69 - 0x8000`). This is the **sixth and last**
    of the nineteenth follow-up's `[0x690]`-writing orphan functions,
    closing that list completely. Reading the full ~192-line body at
    its native address (`(far ptr device_array, count)`, matching
    `FUNCTIONS.md`'s already-recorded `(device_off, device_seg,
    count)` stack order) shows its real behavior is considerably more
    specific than that entry's existing "searches ... for a matching/
    available entry" description: it's a **pool defragmentation/
    compaction pass** over the lettered-reference waveform-data heap,
    not a lookup. For each 6-byte entry in the caller-supplied plan
    array (`+0` = a target/compare position, `+2` = the `0..25`
    `[0x73E]`/`[0x742]` record index, `+4` = a delete flag): if the
    entry's `+4` flag is nonzero, the record is discarded outright
    (`[0x690]=0x5AA`, `init_gpib_record` + `build_gpib_message_
    checksum`); if `+4==0` and the entry's `+0` field already sits at
    or before the running compaction cursor, the record is left in
    place and just its length is folded into the cursor (no physical
    move); otherwise (`+0` lies past the cursor and `+4==0`) the
    record's waveform bytes are physically byte-copied down to the
    cursor position (`[0x690]=0x585`), `[0x73E]+0`/`+4` and
    `[0x742]+0x36..+0x3E` are rewritten to the new pool-base-relative
    offset, and `build_gpib_message_checksum` recomputes its
    checksums. If the running cursor would ever exceed the `0x6800`
    pool bound, the whole pass fails (returns `1`); otherwise, on
    success, it writes the final cursor into **`[0x746]+8`** and
    `0x6800` minus that cursor into **`[0x746]+6`**. This **reframes
    `[0x746]+8`**: not a static "pool base pointer" as first guessed,
    but a **live watermark - the offset one past the last byte
    currently in use** (consistent with `+6` already being confirmed
    as the free-space count: `limit - watermark`). At the time this
    bullet was written no static caller had been found (same
    no-caller/register-indirect-dispatch shape as the rest of this
    cluster) - **this is now resolved, see the `scan_and_repair_gpib_
    records` bullet below**. Updated `FUNCTIONS.md`'s `0x95B69` entry
    and added the matching `0x8DB69` `FUNCTIONAL_NAMES`/
    `PARAMETER_NAMES` entries.
  - **`scan_and_repair_gpib_records` (`0x8D476`/`0x95476`) confirmed as
    `validate_gpib_device_records`'s real static caller, 2026-10-13,
    later pass**: closes the "no static caller found" question left
    open by the bullet above. The call site is at physical `0x8DB0A`
    (immediately before the `validate_gpib_device_records:` label
    itself), and a label-boundary check of the whole intervening range
    confirmed every label between `scan_and_repair_gpib_records`'s own
    entry point and that call site is a local `L_8Dxxx` jump target,
    not a separate function - the call genuinely sits inside this
    function's body. This also fills in the second half of this
    function's own behavior, previously only characterized through its
    first-half per-record consistency scan/repair loop: after that
    loop, it builds a 6-byte-stride **plan array on its own stack
    frame** (`[bp-0xa2]`), one entry per record with `[0x73E]+0xA` bit
    `0x02` ("has data") set - `+0` = that record's pool offset
    (`[0x73E]+0`), `+2` = the record index, `+4` = a delete flag
    (initialized `0`). If more than one entry, it insertion-sorts the
    array by `+0` ascending. It then walks the sorted array checking
    for pool-layout anomalies against `[0x746]+0xA`/`+0xC` (the same
    diagnostic word/counter the first-half loop also writes to): the
    first entry's offset should be `0` (nonzero sets bit `0x10`,
    anomaly); each consecutive pair's end-of-data (offset + the
    record's own length) should exactly equal the next entry's offset
    (equal = contiguous/OK; greater = **overlap**, marks both entries'
    delete flags and sets bit `0x08`; less = **gap**, sets bit `0x10`);
    and the last entry's computed end-of-data should match the pool's
    live watermark `[0x746]+8` (mismatch marks that entry's delete flag,
    sets bit `0x20`, and **directly overwrites `[0x746]+8`** with the
    corrected value). This is exactly the input format `validate_gpib_
    device_records` (the bullet above) was already documented to
    expect - strong structural confirmation this is the genuine
    designed caller, not a coincidental register-indirect hit. Final
    decision: if the accumulated repair count `[0x746]+0xC` exceeds
    `6`, gives up entirely (calls `init_comm_device_table`, returns
    `0xFFFF`, no call to `validate_gpib_device_records` at all); else
    if the diagnostic word `[0x746]+0xA` is nonzero, calls `validate_
    gpib_device_records` at `0x8DB0A` with a far pointer to the local
    plan array plus its count - success returns `1`, failure calls
    `init_comm_device_table` and returns `0xFFFF`; else (nothing
    flagged by either half) returns `0` cleanly. Updated `FUNCTIONS.md`'s
    `0x95476` and `0x95B69` rows to reflect this.
  - **`FUNC_2998_C85B` (`SAVeref`) ruled out as `validate_gpib_device_
    records`'s caller, 2026-10-13, later pass**: the hypothesis above
    - that `SAVeref`'s length-mismatch path calls the pool-compaction
    function to make room - was checked by reading `C85B`'s entire
    body (physical `0x8C85B`-`0x8CD99`, all ~445 lines) end to end.
    Its only `lcall` targets anywhere in the function are
    `set_ds_return_old` (at every exit), `SUB_96126` (the pool
    interlock acquire, once near the top), `memcpy_far` (the actual
    waveform-data copy, on the length-match success path), and one
    previously-unidentified function, `SUB_95D81` (physical `0x8D581`
    via the alias rule, called once at `0x8CA5B`) - reached exactly on
    the branch where the combined source+destination length exceeds
    the destination's current capacity (`cmp si, ax; jg L_8CAC6`,
    where `ax` includes `[0x746]+6`'s free-space count). **No call to
    `validate_gpib_device_records`, at either its native (`0x8DB69`)
    or aliased (`0x9471,0x1459`/`0x95B69`) form, appears anywhere in
    this function.** The other insufficient-room branch (reached when
    the length check at `L_8CBF7`/`0x8CC12` fails, `cmp si, cx; jg
    L_8CC8E`) also does not compact - it falls straight through to an
    error return (`[0x690]=0x21A`, status code `4`) without attempting
    to free space. `SAVeref` is therefore **ruled out** as the missing
    caller; the caller remains unresolved, and `SUB_95D81` (not yet
    traced) is a new, more promising lead on the same length-mismatch
    path. This also completes a byte-for-byte account of `C85B`'s full
    body (the "not yet byte-for-byte-complete" caveat above is
    resolved): the previously-untraced "destination empty"
    (`L_8C99F`) and unequal-length branches are now accounted for -
    the latter is exactly this insufficient-room/`SUB_95D81` path,
    not a reallocation-via-compaction as guessed. Updated
    `FUNCTIONS.md`'s `0x95B69` entry, `STILL_PENDING_DECODE.md`, and
    `TODO.md` to retract the "leading candidate" framing.
  - **`SUB_95D81` resolved as `vacate_and_compact_gpib_record`; `SAVeref`
    confirmed to call it, 2026-10-13, later pass**: the lead flagged
    immediately above was traced to completion. `SUB_95D81`'s native
    address is `0x8DD81` (not the `0x8D581` initially guessed - the
    alias arithmetic for `seg=0x9471, off=0x1671` is `(0x9471<<4) +
    0x1671 = 0x94710 + 0x1671 = 0x95D81`, matching the label exactly;
    native = `0x95D81 - 0x8000 = 0x8DD81`). Reading its full body
    (`0x8DD81`-`0x8DF68`) shows it is the **single-record counterpart**
    to `validate_gpib_device_records`: given one record index (its
    only real parameter, `[bp+6]`), it marks that record's `[0x73E]+
    0xA` wipe-in-progress bit, then repeatedly finds whichever record
    immediately follows it in the packed pool (matching the vacated
    record's own current pool offset plus its length, with the
    has-data bit `0x02` set) and byte-copies it down to close the gap,
    rewriting that record's `[0x73E]+0`/`+4` and `[0x742]+0x36..+0x3E`
    fields and recomputing its checksum via
    `build_gpib_message_checksum`, advancing a "frontier" cursor each
    time until it reaches the pool's live watermark (`[0x746]+8`), at
    which point it writes the final frontier back into `[0x746]+8`,
    adds the reclaimed length into `[0x746]+6`, and finally calls
    `init_gpib_record` on the originally-vacated index to fully wipe/
    reinitialize it. Named `vacate_and_compact_gpib_record` and added
    to `FUNCTIONAL_NAMES`/`PARAMETER_NAMES` at both its native
    (`0x8DD81`) and aliased (`0x95D81`) addresses. **This positively
    confirms `SAVeref` DOES call a pool-compaction routine on its
    insufficient-room path** (`0x8CA5B`) - just this function, not
    `validate_gpib_device_records`/`DB69`. The two findings are
    complementary, not contradictory: `SAVeref` was only ever ruled
    out as `DB69`'s caller specifically; `DB69`'s real static caller
    remains unresolved. `SAVeref`'s exact room check before calling it:
    `si` (incoming source record's combined length) is compared
    against `ax` (destination record's own current combined length
    plus the pool's existing free-space count `[0x746]+6`); if
    `si > ax`, it fails outright (`[0x690]=0x21A`, no compaction
    attempted - the same error path already described above); if
    `si <= ax`, it sets `[0x690]=0x1AA`, calls
    `vacate_and_compact_gpib_record(dest_index)`, then `memcpy_far`s
    the new data into the pool at the resulting watermark and rewrites
    the destination's `[0x742]` pointer fields.
  - **`FUNC_2998_CEF5` resolved as the real `REFDisp REF{A-Z}:EMPTY`
    handler, named `erase_gpib_record`, 2026-10-13, later pass**: found
    while regenerating the listing above - a second, previously-
    unknown call to `vacate_and_compact_gpib_record` appears at
    `0x8CF5F`, inside a hitherto-unidentified function,
    `FUNC_2998_CEF5` (physical `0x8CEF5`-`0x8CFD9`). Reading its full
    body shows: given a raw ASCII letter (`[bp+6]`, same
    `add di, -0x61` idiom as `SAVeref`'s own letter parameter), it
    range-checks the resulting `0..25` index (error `0x10` if out of
    range), acquires the `SUB_96126` pool interlock (error `1` if
    busy), then checks `[0x73E]+0xA` bit `0x01` (already-wiped
    sentinel - returns `0x40`, "already empty", if set) and bit `0x04`
    (LOCK/protection - returns `2`, "locked", if set); only if neither
    is set does it call `vacate_and_compact_gpib_record(index)` to
    actually erase the record, returning `0` on success; it always
    releases the `[0x746]+0` interlock before returning on every path.
    This matches the programming manual's `REFDisp REF{A-Z}:EMPTY`
    description ("erases the named reference if it is not protected")
    exactly, and **resolves the long-open question in `VARIABLES.md`'s
    `[0x73E]` row of whether `B339` or `E1E9` is the real `EMPTY`
    implementation: neither is** - `B339` is `REFProt REF{A-Z}:PERM`
    (already settled separately), and `E1E9`/`init_gpib_record` is
    just the low-level full-wipe primitive this new function (via
    `vacate_and_compact_gpib_record`) ultimately delegates to. Named
    `erase_gpib_record` and added to `FUNCTIONAL_NAMES`/
    `PARAMETER_NAMES` at its native address (`0x8CEF5`; not reached via
    any `0x9000`-segment alias call site found so far, so no second
    entry needed). A sibling function immediately following it in the
    listing, `FUNC_2998_CFDA` (physical `0x8CFDA`), also takes a letter
    parameter and acquires the same interlock, but **toggles** (XORs)
    the `0x04` LOCK bit gated on the `0x40` PERM bit being clear - a
    distinct "toggle lock" operation, noticed but not yet traced or
    named; flagged as a new lead for a future pass.
  - `build_gpib_message_checksum` (`D0C2`, immediately preceding
    `D1AA`/`validate_gpib_record_consistency` in the listing) is the
    **write-side counterpart**: given an index, it computes and stores
    exactly the two checksums `validate_gpib_record_consistency`
    verifies (payload checksum -> `[0x73E]+0xB`, header checksum ->
    `[0x742]+0x4C`), then clears bit `0x20` on `[0x73E]+0xA` - the same
    bit `scan_and_repair_gpib_records` sets as "needs attention." This
    is the **checksum-commit/finalize step**, confirming `[0x73E]+0xA`
    bit `0x20` is a "stored checksums are stale, recompute before
    trusting them" dirty flag - and explains `SUB_94D9C`'s own
    exclusive-release branch (`[0x746]`'s acquire/release writeup
    above): calling `build_gpib_message_checksum` there is exactly
    "recompute and commit fresh checksums once new data has been
    written and the exclusive lock is being released," while its
    read-release branch calling `validate_gpib_record_consistency`
    instead is "verify nothing has gone stale before giving up read
    access." Added `FUNCTIONAL_NAMES` entries for all four functions'
    native comm-ROM addresses (`0x8D1AA`, `0x8D476`, `0x8D0C2`, and
    `FUNC_2998_E1E9`'s own native address `0x8E1E9` = `init_gpib_
    record`, already named at its `0x961E9` alias) and regenerated -
    the `.lst`/`.asm` outputs now show all four under their real names
    rather than `FUNC_2998_*` placeholders.
  - `FUNC_2998_DB69` remains untraced this pass.

See `VARIABLES.md` for the per-variable rows and `STILL_PENDING_DECODE.md`/
`TODO.md` for what within this subsystem remains open (`[0x742]`'s
individual fields beyond the far-pointer/length pairs; `[0x746]`/
`[0x74A]`'s exact pool-management semantics; the lock/perm SET-side
write patterns other than `FUNC_2998_B339`'s unlock case; and firm
confirmation of the `"ACQ"`/`"REF"` 2-state resolver's owning
command).
