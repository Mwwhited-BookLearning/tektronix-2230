# Active work

## Next up

- [ ] **User request**: decode the readout's stroke/vector font glyph
      table into SVG files + a catalog. Mechanism is fully understood
      (`draw_readout_char`'s pen/coarse/fine bit-packing, see
      `disasm/NOTES.md` "Attempted: locating the stroke-font glyph
      table"), but the table's physical address (`[0x1DB0]`'s value)
      hasn't been found - nothing writes it in proven or heuristic
      code. `[0x1DB8]`/`[0x1DBC]` (nearby) are ruled out as simple
      plot-scale byte caches, not font data - see `VARIABLES.md`
      "Acquisition/plot scaling". The `0xEA5E6`-`0xEB131` candidate
      region is ruled out (generic repetitive hook shapes, no
      letterforms). **Tool built 2026-09-13**: `disasm/decode_stroke_
      font.py` implements the confirmed bit-packing formula and
      renders any byte range to an SVG glyph catalog - reusable for any
      future candidate, and re-confirmed the `0xEA5E6` ruling with a
      proper tool (`disasm/stroke_font_candidates/0xEA5E6-0xEB131.svg`).
      **Realized the earlier search approach was structurally wrong**:
      `[0x1DB0]` points to a 128-entry far-pointer *array*, not glyph
      data directly - each entry points to that character's own
      (possibly non-contiguous) stroke bytes elsewhere. Added `scan_
      pointer_table()` to search for the pointer array's shape instead
      of a contiguous glyph run, but it found **zero candidates** in
      either main-ROM chip even with relaxed thresholds - genuinely
      inconclusive (scoring heuristic may be too strict, or the table
      lives in the comm ROM/an unmodeled region). Worth revisiting with
      different heuristics, or tracing `boot_init`'s data-driven init
      loops to find what sets `[0x1DB0]`/`[0x1CC4]` directly instead of
      searching blind.
- [ ] Find the comm ROM's actual **incoming**-data path. The ring
      buffer at `[0x448]`/`[0x44C]` (base `0xAF`, size `0x384`) turned
      out to be a TX queue (`serial_tx_buffer_put` producer,
      `service_comm_tx_queue` consumer), not an RX buffer as first
      assumed. No genuine incoming-byte ring buffer/interrupt handler
      has been identified yet - worth tracing if a comm-module board
      photo turns up a UART chip whose interrupt line can be followed
      back into the IVT.
- [ ] **REVIEW LATER**: `binary/aligned/*.bin` (NOP-padded, fully-
      readable reconstructions - see `binary/aligned/README.md`) were
      adopted as the reference binary for future checks. Revisit this
      choice once the rest of the analysis (self-test subroutine ID,
      menu tree, I/O port mapping) is further along, to confirm
      nothing was missed by not using the true original byte-for-byte.
- [ ] The full `ACQ_MODE_SETUP_TABLE` menu tree and the `DIAGNOSTICS`
      self-test menu are photographed and documented end-to-end in
      `HARDWARE.md`/`hardware/photos/INVENTORY.md`. Still open: trace
      the actual menu-rendering code that reads/draws these screens (an
      `update_menu_position`-driven state machine is the leading
      candidate) to tie each menu string to its backing code. Also
      identify the specific functions behind self-test leaves
      `ACQ_ACCESS`/`PRC_READBACK` and `CAL_AIDS`'s `BOX`/`CAL_V_POS` and
      `EXERCISERS`'s `CONFIGURATION`/`IO` - `TB_DIVIDER`/`CLK_DELAY`'s
      registers and the A/D converter identity are now confirmed (see
      `MEMORY_MAP.md`/`CONTEXT.md`).
- [ ] `write_readout_port_byte`/`init_readout_port_config`/`print_char`/
      `print_string_far` (all used exclusively for the self-test text
      banner) write to physical `0x406F0`-`0x406F3`, inside the comm-
      option's "Option UART/GPIB chips" 8-register bank. Desk research
      alone (TMS9914A datasheet ruling out its Data Out register at
      this offset; the RS-232 side having a real UART, "UART U1251",
      sharing this address block) had built a reasonably strong case
      for "genuine UART transmit register." **Tested directly on real
      hardware 2026-09-13 and it doesn't hold up**: the user has RS-232
      installed, confirmed the DIP-switch baud rate (600) by reading
      the switches directly, and ran an actual self-test from the
      `DIAGNOSTICS/TESTS` menu while a listener captured the port at
      the correct baud rate - **zero bytes came through**. If this
      cluster reached a real UART transmit register, the self-test
      should have produced something. Back to genuinely unresolved,
      now with real experimental data (see `disasm/NOTES.md`'s "Live
      hardware test" section for the full writeup and caveats on what
      else could still explain a null result). **Still short of a
      confident rename** either way
      (exact U1251 part number/register layout not confirmed) - see
      `disasm/NOTES.md`'s expanded "The readout/CRT display memory"
      section for the full trace. Next step if picked up again: check whether
      `init_readout_port_config`'s literal bytes (`0x29`/`0x23`/`0x06`)
      match documented UART/GPIB mode-register constants for a chip of
      this era.
- [ ] Reconcile `COMM/DATA/STOP_BITS`/`FLOW` (a runtime menu) against
      the rear-panel PARAMETERS DIP switch (`read_dip_switches_serial_
      config`) - both seem to configure overlapping RS-232 parameters;
      not yet clear which wins or whether the DIP switch only sets
      power-on defaults. Also: `COMM/DATA/ENCDG` offers ASCII/BINARY/
      HEX waveform-data coding but only the ASCII path
      (`print_signed_decimal_serial`/`print_param_list_response`) has
      been identified in code so far - find the binary/hex one(s).
- [ ] **Landing-artifact phenomenon** (real compiled `CALL`/`LCALL`/
      `JMP`/`LJMP` targets landing 1-4 bytes before where coherent code
      actually resumes): root cause confirmed statistically (`disasm/
      find_landing_artifacts.py` found 49 candidates; the majority land
      on an `ADD`-family opcode, far above chance - a common filler
      byte coincidentally decoding as a valid opcode). Only 2 of the 49
      were individually traced in depth (`write_hw_shift_register`,
      `SUB_EAC86`/`SUB_EAD08`), both turning into real findings rather
      than near-misses. The remaining ~47 weren't individually chased -
      worth a look only if one stands out (many independent call sites
      is the best tell). See `disasm/NOTES.md`'s "Systematic landing-
      artifact sweep" section.
- [ ] `init_far_pointer_table_sysrom`'s embedded RAM-init table (`ES=
      0x209`, physical `0x2090-0x21F0`) led to 15 new proven entry
      points, mostly a plot-scale-computation preamble immediately
      before `draw_pending_line_segment` (see `VARIABLES.md`
      "Acquisition/plot scaling"). Still open: **no code anywhere in
      the corpus loads `ES`/`DS`=`0x209` via a literal immediate** - how
      (or whether) these functions actually get invoked in practice
      isn't proven. See `disasm/NOTES.md` "Found: a whole family of
      never-reached functions via the RAM far-pointer init table".
- [ ] Identify and mark data regions (ASCII strings, tables) inside the
      already-reached code so the listing stops trying to disassemble
      them as instructions.
- [ ] Narrow down what peripheral the I/O ports actually seen in code
      (`0x83`, `0xC4`, `0xD1`, and a DX-indexed range) correspond to -
      see `MEMORY_MAP.md` "I/O ports actually seen in code". These are
      true 8086 port-space (`in`/`out`) accesses, a different address
      space from the memory-mapped `0x40000+` window the service manual
      documents, so that manual's Table 3-1 doesn't cover them
      directly. The AUX-connector pen-lift relay doesn't cleanly fit
      `write_hw_shift_register`'s multi-bit shift-and-strobe shape (Pen-
      Down is a single digital line, per the service manual) - the X/Y
      analog plot-output DACs are a better structural fit but the
      specific register/DAC feeding them wasn't named in the manual
      excerpts read so far.
- [ ] **In progress: renaming EVERY identifiable routine, not just
      opportunistically** (standing request: "keep going, don't stop
      until everything is renamed"). Working through the proven-only
      set (`sysrom_3532_3633.symbols.json`) ordered by reference count,
      highest first - 262/282 named. The remaining 20 have each been
      individually investigated and have documented reasons they can't
      be safely named (see `disasm/NOTES.md`'s dated session entries
      and `changes/` for the running list). The heuristic-only layer
      (tens of thousands more, across all 3 ROMs) is a much lower-
      confidence, much larger tail - the realistic goal is "every
      proven-reachable routine named," not literally every heuristic
      placeholder. Mechanically: add `{address: "name"}` to `gen_
      disasm_x86.FUNCTIONAL_NAMES`, add the matching `FUNCTIONS.md`
      entry, then regenerate everything (`gen_disasm_x86.py`, `gen_
      disasm_mainrom_heuristic.py`, `gen_source.py`, `gen_source_
      readable.py`) and re-verify byte-identical/length-matching before
      committing.
- [ ] Which `[0x1B83]` value (`0x1E` vs `0x14`) specifically means
      "comm option installed" isn't resolved - see `detect_comm_
      option_hw` in `FUNCTIONS.md` and `disasm/NOTES.md` "Found the
      actual source of [0x1B83]". The write-probe address is confirmed
      as the general-purpose "Time Base Mode Register U4119" (not a
      comm-specific latch), consistent with the two-branch ambiguity,
      but doesn't pin down the exact bit semantics.
- [ ] Which physical front-panel control each of the 3 `update_menu_
      position`-range-scan self-tests (`selftest_front_panel_switch_a`/
      `_b`, `selftest_comm_option_switch`) corresponds to isn't
      confirmed. **`[0x758]`/`SWB2`'s exact bit map is now confirmed
      2026-09-13**, not just address-matched - see `VARIABLES.md`. Its
      bits (`MEM 1`/`2`/`3`, `MENU ADV`, `SELECT C1/C2`, `MENU`, `1K/
      4K`, `POS/SEL`) are all *menu/memory* controls, not the specific
      analog VOLTS/DIV-style switches these 3 self-tests are believed
      to exercise - so this confirmed byte doesn't directly answer
      *this* item, but the technique (compare a bit mask's code
      structure to the manual's named bits, not just match addresses)
      is proven and worth repeating for `[0x4E7]`/`[0x4E8]` and
      `[0x759]`/`SWB1` once a literal-address read site for either is
      found. `[0x4E7]`/`[0x4E8]`'s `&0x80` "accelerate" pattern still
      isn't tied to a specific named `SWB1`/`SWB2` bit - remains open.
- [ ] Found the comm option board's DIP-switch reader (`read_dip_
      switches_serial_config`/`read_dip_switches_gpib_config`, see
      `HARDWARE.md`) - still open: map each of the 10 physical switch
      positions to which specific decoded bit(s) it controls. `[0x629]`
      (GPIB/RS-232 mode) still isn't confirmed as switch-sourced. The
      service manual confirms `0x406BC` is the right register ("Option
      Parameters Latch (in)") but doesn't give a bit-by-bit switch map
      in the sections read so far.

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

See `changes/` for a dated log of completed work per session. Keep
this file trimmed to active/pending items only — when something gets
resolved, move its detail into the current day's `changes/YYYY-MM-DD.md`
entry instead of leaving a long `[x]`-marked writeup here.
