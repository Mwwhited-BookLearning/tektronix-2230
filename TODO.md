# Active work

## Next up

- [ ] **User request**: decode the readout's stroke/vector font glyph
      table into SVG files + a catalog, and use recognizable drawn
      shapes for additional naming/context. Mechanism is fully
      understood (`draw_readout_char`'s pen/coarse/fine bit-packing,
      see `disasm/NOTES.md` "Attempted: locating the stroke-font glyph
      table"), but the table's physical address (`[0x1DB0]`'s value)
      hasn't been found - a heuristic binary scan found a false
      positive (real code + the menu string table, not glyph data).
      Next attempt should either trace `boot_init`'s data-driven init
      loop(s) to find what sets `[0x1DB0]`/`[0x1CC4]`, or render
      scan candidates and visually check for real letterforms instead
      of just checking byte-run length.
- [ ] Find the comm ROM's actual **incoming**-data path. The ring
      buffer at `[0x448]`/`[0x44C]` (base `0xAF`, size `0x384`) turned
      out to be a TX queue (`serial_tx_buffer_put` producer,
      `service_comm_tx_queue` consumer, both renamed this session
      after an earlier direction mistake - see `disasm/NOTES.md`
      "Direction correction"), not an RX buffer as first assumed. No
      genuine incoming-byte ring buffer/interrupt handler has been
      identified yet - worth tracing if the user's offered comm-module
      board photos turn up a UART chip whose interrupt line can be
      followed back into the IVT.

- [ ] **REVIEW LATER**: `binary/aligned/*.bin` (NOP-padded, fully-
      readable reconstructions - see `binary/aligned/README.md` and
      `disasm/NOTES.md` "NOP-aligned readable reconstruction") were
      adopted as the reference binary for future checks. Revisit this
      choice once the rest of the analysis (self-test subroutine ID,
      menu tree, I/O port mapping) is further along, to confirm
      nothing was missed by not using the true original byte-for-byte.
- [ ] The full `ACQ_MODE_SETUP_TABLE` menu tree and the `DIAGNOSTICS`
      self-test menu are now photographed and documented end-to-end in
      `HARDWARE.md`/`hardware/photos/INVENTORY.md` (screen-by-screen,
      2026-09-11). Still open: trace the actual menu-rendering code
      that reads/draws these screens (an `update_menu_position`-driven
      state machine is the leading candidate) to tie each menu string
      to its backing code, rather than just the backing variables
      already found for the acquisition-mode bit flags. Also:
      identify the specific functions behind the newly-named self-test
      leaves `TB_DIVIDER`, `CLK_DELAY`, `ACQ_ACCESS`, `PRC_READBACK`
      (all under `DIAGNOSTICS/TESTS`), and `CAL_AIDS`'s `BOX`/
      `CAL_V_POS`/`CAL_CLK_DLY` and `EXERCISERS`'s `CONFIGURATION`/
      `IO`/`A_TO_D_TESTS` - `A_TO_D_TESTS` in particular is a strong
      lead for the still-unconfirmed A/D converter identity.
- [ ] Reconcile `COMM/DATA/STOP_BITS`/`FLOW` (a runtime menu) against
      the rear-panel PARAMETERS DIP switch (`read_dip_switches_serial_
      config`) - both seem to configure overlapping RS-232 parameters;
      not yet clear which wins or whether the DIP switch only sets
      power-on defaults. Also: `COMM/DATA/ENCDG` offers ASCII/BINARY/
      HEX waveform-data coding but only the ASCII path
      (`print_signed_decimal_serial`/`print_param_list_response`) has
      been identified in code so far - find the binary/hex one(s).
- [ ] Handle the ~39-instruction decode-drift cluster in `3633` around
      physical `0xEA1A0-0xEA615` (386-only features that can't be real
      on this 8086/8088 - see `disasm/NOTES.md`). Doesn't threaten the
      buildable `.asm`'s correctness (safety net already excludes it),
      but reaching it via pure fallthrough with no owning label means
      there's a real function boundary nearby the recursive descent
      doesn't know about - worth finding for a cleaner listing.
- [x] Investigate whether capstone is misdecoding the undocumented
      8086 1-byte opcode `0x0F` (`POP CS`) as a 286+-style SSE/MMX
      two-byte escape prefix - found at `SUB_F6382` (`160-3532`).
      **Resolved 2026-09-12**: named anyway (`draw_marker_box_and_
      update_position`) since the rest of the body is coherent despite
      the single misdecoded opcode; the capstone-mismatch explanation
      stands undisputed. Confirmed it does *not* explain `SUB_EAC86` or
      any of the broader landing-artifact family below - those are a
      different, unrelated phenomenon.
- [ ] **Landing-artifact phenomenon - now the leading theory for `SUB_
      EAC86` and its relatives**: a genuine, unambiguous compiled
      `CALL`/`LCALL`/`JMP`/`LJMP` target lands 1-4 bytes *before* where
      coherent code actually resumes, so the disassembler decodes a
      coincidentally-valid (or garbage) instruction at the "short"
      address before reconverging. Confirmed instances now span `SUB_
      E90A5`/`SUB_E92B0` (original), `SUB_F6382` fallthrough, `L_EDA0A`
      (`SUB_F1581` → `compute_and_draw_scale_marker`, resolved 2026-09-
      11), `SUB_F6F4A`, `SUB_F1254`, and `0x88729` (the exact target of
      `SUB_E99DF`'s garbage `ljmp` - a real, isolated, but statically-
      unresolvable `ljmp [bp+di]` indexed off the caller `SUB_F4150`'s
      `bp`/`di`, not meaningless garbage as first assumed). Root-cause
      lead (not confirmed): landing 1 byte short very often lands on
      `0x00`, which is both an extremely common displacement/immediate
      byte AND a valid `ADD r/m8,r8` opcode, forming a spurious 2-byte
      `add` instruction that happens to look plausible. `disasm/find_
      landing_artifacts.py` systematically found ~49 candidates total
      (checks every call-target's own instruction for byte-overlap with
      any other independently-reached instruction); only ~7 individually
      examined so far (EBC3A, EC2E6, EE319, F3132, F4CE8, F784D, F6891) -
      **continue working through the remaining ~42 candidates** to see
      how many are genuinely this class vs. already-known shared-entry
      clusters vs. something new. See `disasm/NOTES.md`'s "Found:
      indirect jumps/calls through computed pointers" section.
- [ ] Investigate `SUB_EAC86` (`160-3633`, proven set) - decodes as
      unambiguous garbage (including an impossible SSE instruction)
      despite being a **clean, unambiguous far-call target** reached
      identically from 3 separate places across 2 ROMs (`160-3532` x2,
      comm ROM x1). Different and more puzzling than the
      `0xEA1A0-0xEA615` fallthrough cluster above - no nearby byte
      shift produces a clean prologue either. Left unrenamed rather
      than guess. See `disasm/NOTES.md` "A second, more puzzling
      decode anomaly: SUB_EAC86" for what's been ruled out and
      candidate explanations (address-decode alias? dead code?). Its
      two neighbors `SUB_F5898`/`SUB_E97DC` (the "TEKTRONIX" boot-
      splash builder and its shared copy-loop cluster) are coherent,
      valid code but rely on a `bp`/`si` implicit-register calling
      convention this project doesn't model yet - also left unnamed;
      see the "Follow-up" paragraph appended to that same NOTES.md
      section. **Update**: this isn't localized to the boot-splash
      area - `SUB_E99DF`/`SUB_EADA0` (outright garbage, like
      `SUB_EAC86`) and `SUB_E8E03`/`SUB_E8E29`/`SUB_ED9BC`/`SUB_EEA58`
      (coherent code, ambiguous entry, like `SUB_F5898`) are further
      instances found elsewhere in `160-3633`, including one inside the
      acquisition/plot scale-clamp subsystem. See the "not confined to
      the boot-splash neighborhood" paragraph in the same NOTES.md
      section. **Second independent instance found 2026-09-12**: `SUB_
      F173E` makes an equally unambiguous `LCALL` to `0xEA13B`, which
      decodes as the start of an (already-catalogued, in `STRINGS.md`)
      string table, not code - finding this exact shape twice,
      independently, is the strongest evidence yet that this is real
      dead/never-executed code left in the shipped ROM, not a decode-
      tooling bug. `SUB_EA13B`/`SUB_EA2D6` (transitively reached past
      that string table) remain unnamed.
- [ ] **New 2026-09-12**: `init_far_pointer_table_sysrom`'s own embedded
      82-entry `(dest_offset, far_ptr)` RAM-init table (targets `ES=
      0x209`, physical `0x2090-0x21F0`) led to 15 new `ENTRY_POINTS` and
      4 more transitively-found functions, all verified byte-identical.
      Two things still open: (1) **no code anywhere in the corpus loads
      `ES`/`DS`=`0x209` via a literal immediate** - how (or whether)
      these 15 functions actually get invoked in practice isn't proven;
      (2) most are still unnamed - a plot-position/pen-drawing family
      (`SUB_F09C0`/`F09C6`/`F09EA`/`F0A4A`/`F0A4E`/`F0AAA`/`F0B06`/
      `F0B62`/`F0BC2`/`F0BC6`/`F0BE2`/`F0BFA`/`F0C26`) touching the same
      variables as `draw_pending_line_segment`/`reset_plot_home_or_acq`,
      plus `SUB_F173E` above. Also 2 new far pointers `[0x1DB8]`/
      `[0x1DBC]` sitting right next to the still-unlocated stroke-font
      pointer `[0x1DB0]` (see the first item in this file) - worth
      checking whether either of these is actually the font table.
      See `disasm/NOTES.md` "Found: a whole family of never-reached
      functions via the RAM far-pointer init table" and `FUNCTIONS.md`.
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
      reference count, highest first - 261/282 named as of the latest
      session (2026-09-12; the total grew from 278 to 282 after the
      RAM far-pointer-table discovery added new entry points - see
      below). See `disasm/NOTES.md`'s dated session entries and
      `changes/` for the running list and confidence notes. The
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
- [ ] Found the comm option board's DIP-switch reader (`read_dip_
      switches_serial_config`/`read_dip_switches_gpib_config`, see
      `HARDWARE.md`) - still open: map each of the 10 physical switch
      positions to which specific decoded bit(s) it controls. `[0x629]`
      (GPIB/RS-232 mode) still isn't confirmed as switch-sourced.
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
