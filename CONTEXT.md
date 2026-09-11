# Project context

## What this is

Six firmware EPROM dumps (`binary/160-*.bin`, 64KB each) pulled from a
**Tektronix 2230** oscilloscope — a 100MHz dual-channel scope combining
a nonstorage analog mode with digital storage (DSO) and microprocessor-
driven alphanumeric CRT readout, released in the mid-1980s. The goal is
to understand the firmware well enough to fully disassemble and
annotate it: what the hardware is, how the CPU boots, and what the main
routines actually do.

## The hardware (per TekWiki, w140.com/tekwiki/wiki/2230)

- CPU: **Intel 8088** (confirmed — see `disasm/NOTES.md` for how this
  was pinned down after an initial wrong guess of a 6809). A photo of
  the main board (`HARDWARE.md`, `hardware/photos/IMG_1412.jpg`)
  confirms the physical chip as an Intel **`P8088-2`** (1978 date
  code) — the `-2` is Intel's 8 MHz speed grade, vs. the base part's
  5 MHz.
  - firmware in a pair of 27512 (64Kx8) EPROMs
  - six TMS4426 16Kx4 DRAMs (main memory)
  - a pair of MCM2016H 2Kx8 SRAMs (acquisition memory)
  - A/D converter: Sony CX20052A, 8-bit, 20MHz, ECL output
  - GPIB/RS-232 option: adds a daughter board with battery-backed SRAM
    (HM6116 2Kx8 CMOS)
- Board designators found as strings inside the ROMs themselves:
  - **A10** — the main digital/acquisition board, holds the two system
    ROM chips (sockets U9109/U9110)
  - **A23/A24** — the GPIB/RS-232 communications option board, holds
    its own ROM (socket pair U1243/U1343)

## The six ROM files

| Part number | Board/socket | Role |
|---|---|---|
| `160-3633-*` | A10, U9109 ("sys_rom_0") | main system ROM, low 64K half |
| `160-3532-*` | A10, U9110 ("sys_rom_1") | main system ROM, high 64K half (holds reset vector) |
| `160-2998-*` | A23/A24, U1243/U1343 ("comm_rom_0") | GPIB/RS-232 option ROM |

Each is available in (at least) two firmware revisions in this repo,
suffixed `-13` and `-14` (TekWiki separately references `-01`/`-15`
revisions we don't have copies of — there may be more revisions out
there than what's in `binary/`).

Full technical detail — physical memory map, header format, per-
revision diffs, disassembly tooling and status — lives in
`disasm/NOTES.md`, which is the file to update as understanding grows.
This file is for the higher-level "what and why"; that one is the
detailed engineering log.

## Source manuals

The official Tektronix 2230 Service Manual (070-4999-00) is publicly
available (e.g. via Tektronix's own site, TekWiki, and archive.org) and
would be the authoritative reference for schematics/memory maps if
deeper hardware-level questions come up — not yet consulted in depth,
only used so far to confirm the CPU identity via TekWiki's summary page.
