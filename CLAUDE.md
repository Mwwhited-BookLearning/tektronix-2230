# Tektronix 2230 firmware reverse-engineering

This repo is a personal learning project reverse-engineering firmware
EPROM dumps pulled from a Tektronix 2230 oscilloscope. There's no
external deadline or audience — it's meant to be picked up and resumed
across many sessions, so **read `CONTEXT.md` and `TODO.md` before
re-deriving anything**; the hard-won facts (CPU identity, memory map)
are already settled and documented there and in `docs/` (start at
`docs/README.md`).

## Layout

- `binary/` — the raw EPROM dumps (`160-XXXX-NN.bin`), read-only source
  material. Never edit these.
- `binary/aligned/` — **NOT original firmware.** NOP-padded
  reconstructions (`160-XXXX-NN_aligned.bin`) assembled from
  `disasm/160-XXXX-NN_readable.asm` by `disasm/gen_source_readable.py`.
  Same length and same addresses as the true original, but a handful
  of bytes differ (NASM's own preferred encoding substituted in place
  of an equivalent-but-differently-encoded original instruction). See
  `binary/aligned/README.md` — status is **provisional, review later**
  once the rest of the analysis is further along.
- `disasm/` — the disassembly project: generator scripts, listings, and
  the symbol table that gets refined over time.
- `decompile/` — a local Ghidra project (`Tek2230.gpr`/`.rep/`), a
  second, independent cross-check tool. **Not git-tracked** (the
  `.rep/` database is gitignored) — a local artifact, not something a
  fresh clone will have. See `docs/architecture/ghidra-project.md`
  before scripting against it — its segment-addressing convention is
  non-obvious and has already caused two real mistakes worth avoiding
  a second time.
- `emulator/` — a headless Unicorn-Engine-based tracer for running the
  real firmware from reset and observing runtime behavior directly,
  for questions static disassembly can't settle on its own (e.g. the
  stroke-font glyph-table hunt). See `emulator/docs/design.md` for
  scope, and its "Findings and gotchas" section before touching
  `emu.py`/`memory_map.py`/`timer.py` — booting a real 8086 in Unicorn
  has already surfaced several non-obvious gotchas (NMI-vs-maskable
  interrupt semantics, the 1MB address wraparound, RAM-vs-ROM mapping)
  worth not rediscovering.
- `docs/` — the authoritative technical log (CPU identity, ROM roles,
  every investigation/finding), split by topic under `docs/{area}/
  {feature}.md` since a single running-log file grew too long to
  navigate. **Start at `docs/README.md`** for the index; add new
  findings to whichever area file fits, or start a new one, and update
  the README's index. `disasm/NOTES.md` is a stub redirecting old
  section links here — don't add new content there.
- `CONTEXT.md` — why this project exists and what's been learned about
  the hardware/device itself.
- `HARDWARE.md` — primary-source photos of the physical unit (front
  panel, rear-panel connectors, DIP switches) in `hardware/photos/`,
  with notes cross-referencing them against firmware findings. Add to
  this whenever new hardware photos come in; split into multiple files
  if it grows unwieldy.
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
- `PSEUDOCODE.md` — hand-written C reconstructions of routines
  understood well enough to read as code, not just a disassembly
  listing. Only for routines already in `FUNCTIONS.md`; mark anything
  not confirmed as a comment rather than stating it as fact.
- `STRINGS.md` — every printable string found in the raw ROM binaries
  plus known constants, categorized and curated; machine-readable
  companion is `disasm/strings_<rom>.json` (regenerate with
  `disasm/gen_strings.py` if a ROM binary ever changes).
- `UNKNOWN_DATA.md` — every large (32+ byte) contiguous ROM byte range
  not yet accounted for by disassembly, a known string, or a known
  data table, exported with multi-format decodes (hex/oct/ascii/
  int/float/far-pointer) for manual or external analysis. Regenerate
  with `disasm/find_unknown_data.py <min_size> --markdown` if a ROM
  binary changes or more of the ROM gets identified.
- `TODO.md` — active/near-term work items.
- `STILL_PENDING_DECODE.md` — a status board of every open technical
  unknown in the firmware, organized by area (distinct from `TODO.md`:
  this is "what's still unknown," not "what to do next"). **Update
  whenever an item here gets resolved or a new one is found.**
- `changes/YYYY-MM-DD.md` — a dated log entry per session of completed
  work, for a human-readable history alongside git log.

## Conventions

- Disassembly labels start as address-based placeholders (`SUB_XXXXX`
  for call targets, `L_XXXXX` for branch targets). To rename one once
  its purpose is understood, add `{physical_address: "name"}` to
  `disasm/gen_disasm_x86.FUNCTIONAL_NAMES` (with a short comment citing
  the evidence) and regenerate — that's what actually makes the name
  appear in the `.lst`/`.asm`/`.symbols.json` outputs. Editing a
  `.symbols.json` file's `name`/`functional_name` directly doesn't
  stick; it gets overwritten the next time anything regenerates it.
  Add the matching entry to `FUNCTIONS.md`/`VARIABLES.md` at the same
  time — don't rename in only one place.
- Once a function's incoming stack parameters (`[bp+N]` operands, not
  local variables) are understood well enough to name, add
  `{physical_address: {offset: "name", ...}}` to `disasm/
  gen_disasm_x86.PARAMETER_NAMES` (offsets as they appear in the
  disassembly's own `[bp + N]` text, e.g. `6`, `0xa`) and regenerate
  via `gen_source_readable.py` — this annotates every `[bp+N]`
  reference to that offset with a trailing `-> name` comment in the
  `_readable.asm` output (comment-only, never rewrites the real
  operand, so it can't affect `binary/aligned/`'s NASM reassembly).
  Derive the offset-to-name mapping from the function's own
  disassembly (cross-check against a call site that passes
  self-describing literal values where one exists, e.g. `format_
  number`'s argument order was confirmed this way from `format_hex_
  word`/`format_decimal_word`'s fixed pushes) rather than guessing from
  a generic calling-convention assumption. Add the same `(name1,
  name2, ...)` signature to `FUNCTIONS.md`'s entry for that function at
  the same time.
- Don't re-litigate settled facts (currently: the CPU is a confirmed
  Intel 8088/8086, not the 6809 originally guessed) — check
  `docs/README.md` and its area files first.
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
  write it down in `docs/` (the relevant area file, e.g.
  `docs/comm-rom/address-map-and-cpu.md` for that example) so it gets
  reused next time instead of rediscovered.
- **To identify an unnamed function's purpose, check what string it
  references before anything else.** Don't just grep the function body
  for the diagnostic-message text directly (an earlier attempt did
  exactly that and found nothing) — instead find where it loads the
  fixed string-table segment (`0xFF7B` in the main ROM) paired with an
  offset, compute the physical address (`0xFF7B0 + offset` for
  `160-3532`), and read the actual bytes there with a one-off Python
  snippet. This identified 18 of `self_test_dispatcher`'s sibling
  subroutines in a single pass (see `docs/self-test/dispatcher-and-
  siblings.md` "Identified self_test_dispatcher's sibling subroutines")
  after call-order
  guessing and direct-reference grepping had both failed. When a
  function has *no* string reference at all, look for a distinctive
  *shape* instead (e.g. the 3 front-panel-switch tests were identified
  by their `update_menu_position`-range-scan pattern, not a string).
