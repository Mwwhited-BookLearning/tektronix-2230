# Active work

## Next up

- [ ] **REVIEW LATER**: `binary/aligned/*.bin` (NOP-padded, fully-
      readable reconstructions - see `binary/aligned/README.md` and
      `disasm/NOTES.md` "NOP-aligned readable reconstruction") were
      adopted as the reference binary for future checks. Revisit this
      choice once the rest of the analysis (self-test subroutine ID,
      menu tree, I/O port mapping) is further along, to confirm
      nothing was missed by not using the true original byte-for-byte.
- [ ] Cross-reference the front-panel menu/parameter tree names found
      in `160-3633` (`ACQ_MODE_SETUP_TABLE`, `SELECT_MODE`, etc. - see
      `STRINGS.md`) against the code that reads them, to build the
      actual menu structure for `JUMP_MAP.md`/the "user-facing flows"
      documentation goal.
- [ ] Handle the ~39-instruction decode-drift cluster in `3633` around
      physical `0xEA1A0-0xEA615` (386-only features that can't be real
      on this 8086/8088 - see `disasm/NOTES.md`). Doesn't threaten the
      buildable `.asm`'s correctness (safety net already excludes it),
      but reaching it via pure fallthrough with no owning label means
      there's a real function boundary nearby the recursive descent
      doesn't know about - worth finding for a cleaner listing.
- [ ] Investigate whether capstone is misdecoding the undocumented
      8086 1-byte opcode `0x0F` (`POP CS`) as a 286+-style SSE/MMX
      two-byte escape prefix - found at `SUB_F6382` (`160-3532`),
      where reading `0x0F` as `pop cs` instead of the start of `movd`
      makes a separate fallthrough path and the far-call entry cleanly
      reconverge a few bytes later, which doesn't happen under
      capstone's decode. See `disasm/NOTES.md` "A third decode
      anomaly: SUB_F6382, likely capstone misreading opcode 0x0F". If
      confirmed, worth checking whether this also explains other
      `0x0F`-led decode oddities elsewhere (it does *not* explain
      `SUB_EAC86`, which doesn't start with `0x0F`).
- [ ] Investigate `SUB_EAC86` (`160-3633`, proven set) - decodes as
      unambiguous garbage (including an impossible SSE instruction)
      despite being a **clean, unambiguous far-call target** reached
      identically from 3 separate places across 2 ROMs (`160-3532` x2,
      comm ROM x1). Different and more puzzling than the
      `0xEA1A0-0xEA615` fallthrough cluster above - no nearby byte
      shift produces a clean prologue either. Left unrenamed rather
      than guess. See `disasm/NOTES.md` "A second, more puzzling
      decode anomaly: SUB_EAC86" for what's been ruled out and
      candidate explanations (address-decode alias? dead code?).
- [ ] Identify what peripheral `0x41000`/`0x42000` (single-byte read
      ports, near the confirmed readout/CRT write port) actually are -
      front-panel switch/encoder status and CRT controller status are
      both plausible; check the service manual's I/O map when
      available. See `MEMORY_MAP.md`.
- [ ] Confirm whether the `0x48000-0x4FFFF` "second plane" (written by
      `append_readout_char`, read back by `print_scratch_buffer_range`)
      is an attribute/inverse-video plane, a shadow copy, or per-
      channel diagnostic scratch space - see `disasm/NOTES.md` "The
      readout/CRT display memory".
- [ ] Identify and mark data regions (ASCII strings, tables) inside the
      already-reached code so the listing stops trying to disassemble
      them as instructions.
- [ ] Investigate the 2 `push` instructions with a stray `0x67` prefix
      that `validate_2998.py` flagged as real mismatches, landing right
      at the start of page 1's header/copyright text - almost certainly
      decode drift into data at the deepest heuristic reach, but worth
      a quick look to confirm.
- [ ] Narrow down what peripheral the I/O ports actually seen in code
      (`0x83`, `0xC4`, `0xD1`, and a DX-indexed range) correspond to —
      see `MEMORY_MAP.md` "I/O ports actually seen in code". `0xD1`/
      `0xC4` are now confirmed as a serial shift-register-style
      hardware write (`write_hw_shift_register`) - peripheral identity
      still open.
- [ ] **In progress: renaming EVERY identifiable routine, not just
      opportunistically** (explicit user request: "keep going, don't
      stop until everything is renamed"). Working through the
      proven-only set (`sysrom_3532_3633.symbols.json`) ordered by
      reference count, highest first - 136/278 named as of the latest
      session (see `disasm/NOTES.md`'s dated session entries and
      `changes/` for the running list and confidence notes). The
      heuristic-only layer (tens of thousands more, across all 3 ROMs)
      is a much lower-confidence, much larger tail - realistic goal is
      "every proven-reachable routine named," not literally every
      heuristic placeholder. Mechanically: add `{address: "name"}` to
      `gen_disasm_x86.FUNCTIONAL_NAMES` (this is what actually makes
      the name show up in the `.lst`/`.asm`/`.symbols.json` outputs -
      editing `.symbols.json` directly gets overwritten on the next
      regenerate), add the matching `FUNCTIONS.md` entry, then
      regenerate everything (`gen_disasm_x86.py`,
      `gen_disasm_mainrom_heuristic.py`, `gen_source.py`,
      `gen_source_readable.py`) and re-verify byte-identical/length-
      matching before committing.
- [ ] Confirm whether the x87 (`fdiv` etc.) instructions mean there's a
      real 8087 math coprocessor in the design (plausible for a scope
      doing voltage/time calculations) — check against the service
      manual's parts list. Stronger evidence found: `convert_sample_
      value` (`0xF1001`) opens with a genuine `fmul` in the middle of
      otherwise ordinary compiled-C integer code (mixed with `mul32`/
      `sdiv32`), not part of any known decode-drift cluster - see
      `disasm/NOTES.md` "Found: the firmware's assert()/panic
      mechanism".
- [ ] Which `[0x1B83]` value (`0x1E` vs `0x14`) specifically means
      "comm option installed" isn't resolved yet - see
      `detect_comm_option_hw` in `FUNCTIONS.md` and `disasm/NOTES.md`
      "Found: the actual source of [0x1B83]". `0x1E` results from two
      branches with opposite-looking conditions, which doesn't fit a
      simple binary flag cleanly.
- [ ] Which physical front-panel control each of the 3
      `update_menu_position`-range-scan self-tests
      (`selftest_front_panel_switch_a`/`_b`, `selftest_comm_option_
      switch`) corresponds to isn't confirmed. **New**: `HARDWARE.md`
      now has a front-panel photo with all control-group labels
      (VERTICAL MODE, ACQUISITION, TRIGGER, etc.) - use it once tracing
      `[0x4E7]`/`[0x4E8]` bit ranges.
- [ ] What hardware `0x403FFA`/`0x403FFB` belong to (read every timer
      tick by `scheduler_tick_service`) isn't confirmed - front-panel
      key/encoder status is the leading candidate.
- [ ] **New from hardware photos** (`HARDWARE.md`): the comm option
      board's rear panel has a 10-position "PARAMETERS" DIP switch -
      no code has been found yet that reads a literal DIP-switch I/O
      port; find what sources `[0x4ED]` (parity mode), `[0x4EF]`
      (CR/LF option), `[0x629]` (GPIB/RS-232 mode?), and the GPIB
      address table `SUB_97905` reads from - likely all or partly this
      switch bank, read once at startup.
- [ ] **New from hardware photos**: the same rear panel's 9-pin
      "AUXILIARY CONNECTOR" has a pen-lift relay plus analog X/Y
      outputs - a direct X-Y plotter interface. Check whether
      `write_hw_shift_register` (ports `0xD1`/`0xC4`) is what drives
      this relay in sync with the HPGL PU/PD state (`[0x6CA]`), as an
      alternative/addition to the current "front-panel setting" guess.
      (The two RS-232 connectors are confirmed by the user to be just
      DTE/DCE pinouts of the *same* serial port for cabling
      convenience, not a firmware-visible mode select - `[0x629]`
      stays open as "GPIB vs RS-232" or something else, just not
      DTE/DCE.)

## Ongoing documentation goal

As routines are understood, build up (not just individual function
names but the bigger picture):
- user-facing flows and menu structure (what the front panel/CRT UI
  actually lets someone do, and how the code implements it)
- configuration options and where they're stored
- the full memory map (RAM regions, I/O port assignments, peripheral
  registers) — started in `MEMORY_MAP.md`; keep it updated as more
  segments/ports are identified
- a "jump map" of major control-flow relationships as activity
  diagrams — started in `JUMP_MAP.md`, high-level first, drilling into
  more detail as more routines are understood
- a function list — started in `FUNCTIONS.md`, one entry per
  identified routine (address, current label, what it does, evidence)
  across all three ROMs
- a variable list — started in `VARIABLES.md`, same idea for memory
  locations
- a strings/constants catalog — started in `STRINGS.md` (curated) with
  a machine-readable companion in `disasm/strings_<rom>.json`
  (regenerate via `disasm/gen_strings.py`)
- clean pseudo-C reconstructions of well-understood routines — started
  in `PSEUDOCODE.md`; add one whenever a `FUNCTIONS.md` entry reaches
  "Confirmed" confidence
- peripherals: the A/D converter, front-panel controls, GPIB/RS-232
  hardware, and how the firmware talks to each

When a diagram would help (state machines, memory maps, menu trees),
embed PlantUML directly in the relevant markdown file rather than
building a separate image asset.

## Done

See `changes/` for a dated log of completed work per session.
