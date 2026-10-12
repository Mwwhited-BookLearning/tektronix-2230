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

### A second, larger, still-unresolved structure found while tracing this: `[0x73E]`/`[0x746]`/`[0x742]`/`[0x74A]`

While confirming `[0x702]`, grepped the remaining "not yet
characterized" section-7 entries and found `[0x73E]`/`[0x746]` are
each referenced **85-105 times** across a large block of code
(physical roughly `0x8B27D`-`0x8B335` and much further, lines
~15898-20117 of the listing) - clearly a central, heavily-used
structure, not a small table. What's confirmed so far:

- `[0x73E]` is a far pointer to a **12-byte-stride record array**,
  indexed by `word ptr [0x5F8]` (itself not yet documented anywhere)
  via a chain through `SUB_93B1C`/`SUB_93B7E`. Record field `+0xA` is a
  flag byte, modified with `and`/`or` against literal masks (`0xBB`,
  `0xBF`, `0x04` seen so far).
- `[0x746]` is a far pointer whose target word is reset to `0`
  immediately after each `[0x73E]` record update - shaped like a
  per-record "dirty"/pending-change counter or flag, not yet confirmed.
- `[0x5F8]` itself is validated against two range checks seen
  repeatedly (`>0x1E` -> error `0x623=0x15`; `<=4` -> also error
  `0x15` in at least one path; valid range looks like `5..0x1E`
  (`5..30`) in the function read so far, but other sites compare it
  against `1`/`4`/`5` with different branch shapes, so this may cover
  more than one valid range depending on context) - writes `0x15` to
  the generic comm-ROM error-code byte `[0x623]` on failure. **Not**
  confirmed to be the same `0x15` as the still-open `[0x1B83]`/
  `MESsage` dead-code question elsewhere in this doc - that's a
  different variable, and this may just be a coincidental shared
  literal; `[0x623]` is already documented (`VARIABLES.md`'s `[0x5F4]`
  row) as a general error-status cell with several unrelated codes
  written to it.
- `[0x742]`/`[0x74A]` are referenced throughout the same code region
  (27 and 14 hits respectively) but their specific role within it
  hasn't been traced yet.

This is evidently a substantial, real structure (far larger than a
"dense cluster" leftover) - flagging it as a distinct, newly-found open
item for a dedicated future pass rather than rushing a conclusion here.
See `STILL_PENDING_DECODE.md`/`TODO.md` for the forward pointer.
