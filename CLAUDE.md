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
- `MEMORY_MAP.md` — the physical memory/IO map as understood so far
  (with a PlantUML diagram). **Update this whenever a new address
  range, segment, or I/O port gets identified or corrected** — it's
  meant to stay current, not just grow a "recent findings" tail.
- `JUMP_MAP.md` — control-flow relationships as PlantUML activity
  diagrams, high-level first with room for per-subsystem detail.
  **Update/extend this as new call chains or dispatchers are traced**
  (e.g. the self-test dispatcher, boot sequence) — don't let it go
  stale once a diagram's been drawn once.
- `FUNCTIONS.md` — one entry per identified routine across all three
  ROMs (address, label, purpose, confidence), the human-readable
  companion to the `.symbols.json` files. **Add an entry here every
  time a `functional_name` gets filled in** — the two should never
  drift apart.
- `VARIABLES.md` — same idea as `FUNCTIONS.md` but for memory
  locations/variables rather than routines.
- `STRINGS.md` — every printable string found in the raw ROM binaries
  plus known constants, categorized and curated; machine-readable
  companion is `disasm/strings_<rom>.json` (regenerate with
  `disasm/gen_strings.py` if a ROM binary ever changes).
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
  and how the firmware drives each peripheral (A/D converter, front
  panel, GPIB/RS-232) — see `TODO.md`'s "Ongoing documentation goal"
  section. `MEMORY_MAP.md`, `JUMP_MAP.md`, and `FUNCTIONS.md` are the
  concrete artifacts for this — keep them current, not just `TODO.md`.
- When a technique proves useful for resolving an addressing puzzle
  (e.g. "brute-force every candidate base offset against observed
  targets and keep whichever maximizes exact matches" — that's how the
  comm ROM's real address and its `0x90000` alias were both found),
  write it down in `disasm/NOTES.md` so it gets reused next time
  instead of rediscovered.
