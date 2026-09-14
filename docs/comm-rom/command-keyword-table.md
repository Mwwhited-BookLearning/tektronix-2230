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
