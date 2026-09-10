# Tektronix 2230 firmware reverse-engineering

This repo is a personal learning project reverse-engineering firmware
EPROM dumps pulled from a Tektronix 2230 oscilloscope. There's no
external deadline or audience — it's meant to be picked up and resumed
across many sessions, so **read `CONTEXT.md` and `TODO.md` before
re-deriving anything**; the hard-won facts (CPU identity, memory map)
are already settled and documented there and in `disasm/NOTES.md`.

## Layout

- `binary/` — the raw EPROM dumps (`160-XXXX-NN.bin`), read-only source
  material. Never edit these.
- `disasm/` — the disassembly project: generator scripts, listings, and
  the symbol table that gets refined over time. See `disasm/NOTES.md`
  for the authoritative technical log (CPU, memory map, ROM roles).
- `CONTEXT.md` — why this project exists and what's been learned about
  the hardware/device itself.
- `TODO.md` — active/near-term work items.
- `changes/YYYY-MM-DD.md` — a dated log entry per session of completed
  work, for a human-readable history alongside git log.

## Conventions

- Disassembly labels start as address-based placeholders (`SUB_XXXXX`
  for call targets, `L_XXXXX` for branch targets) and get renamed to
  functional names in the `.symbols.json` files as their purpose is
  understood — don't rename in the `.lst` files directly, the generator
  script re-renders those from the symbol table.
- Don't re-litigate settled facts (currently: the CPU is a confirmed
  Intel 8088/8086, not the 6809 originally guessed) — check
  `disasm/NOTES.md` first.
- When adding a new session's findings, append a `changes/YYYY-MM-DD.md`
  entry rather than editing prior dated entries.
- When a diagram would help (state machines, memory maps, menu trees),
  embed PlantUML directly in the relevant markdown file rather than
  building a separate image asset.
- Beyond individual routine names, build up the bigger picture as it's
  understood: user-facing flows/menu structure, configuration options,
  the full memory map (RAM/I/O/peripheral registers, not just the ROM
  address map), and how the firmware drives each peripheral (A/D
  converter, front panel, GPIB/RS-232). See `TODO.md`'s "Ongoing
  documentation goal" section.
