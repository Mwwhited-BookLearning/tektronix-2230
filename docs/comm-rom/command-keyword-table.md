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

## 2. The dispatch/index table (26 records, `0x8D3E`-`0x8DE6`)

Each 6-byte record: `[id_byte][0xFF marker][2-byte little-endian
offset][2-byte segment, always 0xDE,0x90]`. The far pointer
(`segment*16 + offset`) resolves through the confirmed
`0x90000`-`0x97FFF` address-decode alias back to a real file offset
(`file_off = 0x8DE0 + offset`) - landing exactly on the length byte of
a header-table entry (table 3) for every non-zero `id`.

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
in the table. Best-fit (not proven) reading: each of the 6 handlers'
fixed flag index (`0x3a`/`0x41`/`0x48`/`0x50`/`0x58`) represents a
different acquisition-mode-style setting, and this function converts
that setting's raw 0-3 flag value into the matching mode keyword for a
query response - not a generic boolean `OFf`/`ON` stringifier as
previously guessed (the leading `OFf`/`ON` strings at the start of this
byte range are real, but nothing in `flag_value_to_keyword_ptr`'s own
code path reaches them). **This is a different mechanism from
`FORmat`'s write site** (which stays unresolved, see
`STILL_PENDING_DECODE.md`) - no record found so far ties back to
`[0x461]` or any `FORmat`-specific handling. Still not confirmed which
real-world setting each of the 5 flag-index constants corresponds to
(e.g. whether `0x3a`/`0x41`/`0x48`/`0x50` are per-channel or otherwise
distinct acquisition parameters) - the 4-byte "key" field mentioned
below is a separate, still-undecoded structure from this function's own
flag-index constants, which don't appear as literal bytes in that key
field.

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

Still open: which real-world setting each of the 5 flag indices
represents in user-facing terms (only `0x58`'s narrower range and the
`LONg`-processing tie-in are known); what `[0x5fe]`'s own value is
actually read from (which command's argument parsing writes it); and
the downstream use of `SUB_945CF`'s bitmask result beyond the
`set_comm_config_flag` call. No renames have been made yet for any of
these - deferred per the project's existing convention (`FUNCTIONS.md`'s
unnamed "settings-byte-translator family") until the real-world meaning
is clearer.
