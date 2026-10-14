# Subsystem object model: boot, input, comm dispatch, and display

A top-down pass tying together findings that were previously scattered
across many single-topic files. This is a *synthesis* document — it
doesn't introduce new disassembly findings, it draws the object-model
boundaries between subsystems already documented elsewhere and asks,
for each one, "where do its inputs come from, and where does its
output go." Citations point back to the files that did the primary
research; this file is the map, not the territory.

Two things this pass deliberately does, per the request that prompted
it: (1) draws a hard line between the comm ROM's **byte-level serial
decoder** (UART framing, ring buffers) and its **SCPI-like command
dispatch layer** (keyword/grammar parsing) — these are two different
subsystems that happen to live in the same ROM and are easy to
conflate; (2) states plainly, where current documentation has nothing,
that nothing exists yet — rather than guessing. See "Confirmed gaps"
at the end for the full list.

## Object model: major subsystems and their relationships

```plantuml
@startuml
skinparam componentStyle rectangle

package "Boot / Init" {
  [boot_init / finish_boot_init_and_start_scheduler]
  [Module detection\n(check_comm_option_installed,\ndetect_comm_option_hw)]
  [Config load\n(poll_dip_switch_change)]
}

package "Task Scheduler" {
  [switch_to_next_task\n(12-slot tick table)]
}

package "Front-Panel Input" {
  [scheduler_tick_service\n(SWB1/SWB2 poll + edge-detect)]
}

package "Serial Command Decoder" {
  [UART registers\n(0x406F0-0x406F7)]
  [poll_comm_status_tick\n(RX producer)]
  [RX ring buffer\n([0x464]/[0x468], 32x4B]
  [TX queue\n([0x448]/[0x44A]/[0x44C]]
  [consume_comm_rx_ring_buffer / dispatch_comm_rx_status_bits\n(RX consumers)]
  [parse_next_gpib_command_byte]
}

package "SCPI-like Command Dispatch" {
  [comm_call_main_rom\n(lexer/parser, [0x5A4] state)]
  [Keyword-table walkers\n(FUNC_2998_5115/519A)]
  [SET/GET flag family\n(get/set_comm_config_flag, ...)]
  [REFA-REFZ reference-\nmemory records]
}

package "Self-Test / Diagnostics" {
  [self_test_dispatcher]
  [print_selftest_banner]
}

package "Menu / Item-List Engine" {
  [[0x1D10] handler table\n(render_item_list_row, ...)]
}

package "Acquisition & Plotting" {
  [handle_acq_mode_change]
  [Number formatter\n(snapshot_index_and_format_number)]
}

package "Display" {
  [CRT readout text memory\n(0x40000-0x4FFFF)]
  [write_readout_port_byte]
  [Vector display list\n(0x02200, stroke font)]
}

package "Configuration" {
  [Staged settings commit\n(commit_pending_settings_and_refresh_display)]
}

package "NVRAM / Persistence" {
  [??? -- GAP, nothing found]
}

[boot_init / finish_boot_init_and_start_scheduler] --> [switch_to_next_task\n(12-slot tick table)]
[boot_init / finish_boot_init_and_start_scheduler] --> [Module detection\n(check_comm_option_installed,\ndetect_comm_option_hw)]
[Module detection\n(check_comm_option_installed,\ndetect_comm_option_hw)] --> [Config load\n(poll_dip_switch_change)]
[Config load\n(poll_dip_switch_change)] ..> [??? -- GAP, nothing found] : no load from\nnon-volatile store

[switch_to_next_task\n(12-slot tick table)] --> [scheduler_tick_service\n(SWB1/SWB2 poll + edge-detect)]
[switch_to_next_task\n(12-slot tick table)] --> [poll_comm_status_tick\n(RX producer)]

[UART registers\n(0x406F0-0x406F7)] --> [poll_comm_status_tick\n(RX producer)]
[poll_comm_status_tick\n(RX producer)] --> [RX ring buffer\n([0x464]/[0x468], 32x4B]
[RX ring buffer\n([0x464]/[0x468], 32x4B] --> [consume_comm_rx_ring_buffer / dispatch_comm_rx_status_bits\n(RX consumers)]
[consume_comm_rx_ring_buffer / dispatch_comm_rx_status_bits\n(RX consumers)] --> [parse_next_gpib_command_byte]
[consume_comm_rx_ring_buffer / dispatch_comm_rx_status_bits\n(RX consumers)] --> [TX queue\n([0x448]/[0x44A]/[0x44C]]
[TX queue\n([0x448]/[0x44A]/[0x44C]] --> [UART registers\n(0x406F0-0x406F7)]

[parse_next_gpib_command_byte] --> [comm_call_main_rom\n(lexer/parser, [0x5A4] state)]
[comm_call_main_rom\n(lexer/parser, [0x5A4] state)] --> [Keyword-table walkers\n(FUNC_2998_5115/519A)]
[comm_call_main_rom\n(lexer/parser, [0x5A4] state)] --> [SET/GET flag family\n(get/set_comm_config_flag, ...)]
[SET/GET flag family\n(get/set_comm_config_flag, ...)] --> [REFA-REFZ reference-\nmemory records]
[comm_call_main_rom\n(lexer/parser, [0x5A4] state)] --> [handle_acq_mode_change]

[scheduler_tick_service\n(SWB1/SWB2 poll + edge-detect)] --> [[0x1D10] handler table\n(render_item_list_row, ...)] : self-test only,\nconfirmed
[[0x1D10] handler table\n(render_item_list_row, ...)] --> [handle_acq_mode_change]
[handle_acq_mode_change] --> [Number formatter\n(snapshot_index_and_format_number)]
[Number formatter\n(snapshot_index_and_format_number)] --> [CRT readout text memory\n(0x40000-0x4FFFF)]

[self_test_dispatcher] --> [print_selftest_banner]
[print_selftest_banner] --> [write_readout_port_byte]
[write_readout_port_byte] --> [CRT readout text memory\n(0x40000-0x4FFFF)]
[write_readout_port_byte] .right.> [UART registers\n(0x406F0-0x406F7)] : same physical\naddress on\nOption-12 hw (!)

[self_test_dispatcher] ..> [UART registers\n(0x406F0-0x406F7)] : COMM_LOOPBACK probe

[[0x1D10] handler table\n(render_item_list_row, ...)] --> [Staged settings commit\n(commit_pending_settings_and_refresh_display)]
[Staged settings commit\n(commit_pending_settings_and_refresh_display)] ..> [??? -- GAP, nothing found] : no save to\nnon-volatile store

note bottom of [Vector display list\n(0x02200, stroke font)]
  Producer chain exists (print_readout_string ->
  draw_readout_char -> plot_readout_point_relative ->
  plot_readout_point) but the hardware CONSUMER that
  draws this list to the CRT has never been found.
  Also: draw_readout_char is gated by [0x1B83] and
  returns immediately without drawing on both of this
  project's physical test units (Option-12/RS-232
  equipped), so this path is practically dead on the
  hardware actually tested against.
end note

note bottom of [??? -- GAP, nothing found]
  Confirmed absent, not merely undocumented:
  no page-flip/bank-switch/double-buffer display
  mechanism, and no NVRAM/EEPROM settings-persistence
  mechanism, exist anywhere in current docs.
  selftest_cmos is diagnostic-only (tests the CMOS
  chip), not a load/save path.
end note
@enduml
```

## 1. Serial command decoder vs. SCPI-like command dispatch

This is the split the rest of the project's docs had implicitly made
but never drawn explicitly. They are genuinely two different jobs,
done by different code, with a clean handoff point
(`parse_next_gpib_command_byte` → `process_gpib_command_byte`):

- **Serial command decoder** — owns UART framing and buffering only.
  It knows nothing about command grammar; it just gets bytes off the
  wire (or queues them to go back out) reliably. See
  `docs/comm-rom/rs232-flow-control-and-open-puzzle.md`.
- **SCPI-like command dispatch** — owns command *meaning*: lexing a
  command line into tokens, matching tokens against keyword tables,
  validating/applying arguments, and formatting responses. It never
  touches the UART registers directly. See
  `docs/comm-rom/command-parser-token-scan-and-plot-handler.md` and
  `VARIABLES.md`'s "Comm ROM" section (lines ~178-217).

```plantuml
@startuml
box "Serial Command Decoder (byte framing, no grammar knowledge)" #LightBlue
participant "UART HW\n0x406F0-0x406F7" as UART
participant "poll_comm_status_tick\n(0x96F54/0x8EF54)" as Poll
participant "RX ring buffer\n[0x464]/[0x468] base 0x46C\n32 x 4-byte records" as RX
participant "consume_comm_rx_ring_buffer\n(0x8F12B)" as F12B
participant "dispatch_comm_rx_status_bits\n(0x8F5AC)" as F5AC
end box

box "SCPI-like Command Dispatch (grammar/keyword layer)" #LightYellow
participant "parse_next_gpib_\ncommand_byte\n(0x8F905)" as Parse
participant "process_gpib_\ncommand_byte" as Proc
participant "comm_call_main_rom\n(0x839F5)\nstate [0x5A4]" as Lexer
participant "Keyword walkers\nFUNC_2998_5115/519A" as KW
participant "SET/GET flag family" as Flags
end box

UART -> Poll : every scheduler tick\n(task slot via\ninit_comm_dispatch_table)
note right of Poll
  Reads [0x6DA] (status/IRQ latch),
  tests bit 0x02 (data ready); if set,
  reads the byte off [0x6D6] plus two
  more register bytes (+1, +3) via a
  plain `mov`, not `in` - no genuine
  port I/O in this ROM.
end note
Poll -> RX : write 4-byte record\n(data, reg+1, reg+3, status)

== task-scheduler slot 5, conditional on [0x1BFA] (COMM_LOOPBACK result) ==
RX -> F12B : [0x1BFA]==0 (common case)
RX -> F5AC : [0x1BFA]!=0 (probe read 0xD0)

F12B -> F12B : translate_comm_rx_event_type on record's\n+1 byte (masked 0x9F)\n-> translated event-type byte
F12B -> F12B : dispatch bits\n(0x80->SUB_97253, 0x10->SUB_97583)
F12B -> "TX queue\n[0x448]/[0x44A]/[0x44C]" : service once RX drained

F5AC -> F5AC : keyed on [0x598]\n(raw status byte)
F5AC -> Parse : bit 0x04 set

Parse -> Parse : stage byte from\nds:0x586/ds:0x590\ninto [0x580]
Parse -> Proc : classification/dispatch
Proc -> Lexer : feed into parser

Lexer -> Lexer : [0x5A4] indexes 24-entry\nfar-call table at [0x72A]\n(0x83B1A-0x85098)
Lexer -> KW : match token against\n26-letter x 6-byte tables\n([0x6EA]/[0x6F6])
KW -> Lexer : resolved command ID -> [0x604]
Lexer -> Flags : SET/GET on matched\ncommand (get/set_comm_\nconfig_flag, flag_value_\nto_keyword_ptr, compute_\nresponse_format_flags)
Flags -> "REFA-REFZ records" : acquire/release,\nchecksums, interlocks
@enduml
```

**Key point this makes explicit**: the "12-slot scheduler table"
feeding the serial decoder and the "24-entry far-call jump table"
feeding the SCPI dispatcher are two *different* tables at two
different addresses, serving two different purposes, and it would be
easy (this project already made exactly this mistake once — see
`changes/2026-10-14.md`'s "duplicate rediscovery" correction) to
conflate a slot in one with an entry in the other. Keep them visually
and conceptually distinct.

## 2. Text/status output generation

Two genuinely separate output paths exist, and — per the earlier
`JUMP_MAP.md` "Root cause found, 2026-09-18" finding — they may
collide at the hardware level on comm-option-equipped units:

```plantuml
@startuml
box "Path A: self-test / diagnostic banners" #LightGreen
participant "print_selftest_banner" as Banner
participant "print_string_far\n(0xE0AF5)" as PSF
participant "print_char" as PC
participant "write_readout_port_byte" as WRPB
end box

box "Path B: SCPI command responses" #LightSalmon
participant "SET/GET flag family" as Flags2
participant "Response formatter\n(compute_response_\nformat_flags)" as Fmt
participant "TX queue\n[0x448]/[0x44A]/[0x44C]" as TXQ
end box

participant "Physical 0x40000+0x6F0\n(= UART data register,\nconfirmed same address)" as Phys

Banner -> PSF : per self-test line
PSF -> PSF : check [0x1B48]==0?\n(SELECT C1/C2 held alone)
alt [0x1B48]==0
  PSF -> PSF : return immediately,\nprint NOTHING
  note right: confirmed root cause,\n2026-09-18 - not a bug,\nthis is what the ROM does
else [0x1B48] != 0
  PSF -> PC : per character,\nwith wait_readout_tick pacing
  PC -> WRPB : write byte
  WRPB -> Phys : fixed target,\nno software cursor advance
end

Flags2 -> Fmt : build response text
Fmt -> TXQ : enqueue bytes
TXQ -> Phys : drained by poll_comm_\nstatus_tick's TX service

note over Phys
  Both paths ultimately target the SAME physical
  address on Option-12 (RS-232) hardware - this
  project's real-hardware SELECT-C1/C2 finding
  (genuine 9600-baud diagnostic text over RS-232
  when held) suggests write_readout_port_byte's
  "CRT readout" text is, on comm-equipped units,
  actually serial output - not yet fully confirmed
  against schematics; see JUMP_MAP.md's "Root cause
  found, 2026-09-18" section and "still genuinely open"
  note for the full open-puzzle writeup.
end note
@enduml
```

The decimal/unit-suffix **number formatter**
(`snapshot_index_and_format_number`, shared `SUB_F5184`/`L_F50FA`
digit loop) feeds Path A — it's how menu-item values like "1.23mV" or
"500ns" get turned into the character stream that
`copy_char_cell_template_and_sync` lays into print records before
`print_char` ever runs. See
`docs/acquisition-and-plotting/mode-dispatcher-and-formatting.md`.

## 3. Serial FIFO buffers

| Buffer | Pointers | Base | Size/shape | Direction | Notes |
|---|---|---|---|---|---|
| RX ring buffer | `[0x464]` (write) / `[0x468]` (read) | `0x46C` | 32 × 4-byte records (`0x80` bytes) | UART → firmware | Records: data byte, reg+1, reg+3, status byte. Correct wraparound confirmed. |
| TX queue | `[0x448]`/`[0x44A]`/`[0x44C]` | `0xAF`-sized | 1-byte records | firmware → UART | Simpler than RX — no per-byte status needed for outgoing data. Serviced by `consume_comm_rx_ring_buffer` once RX is drained. |

**Documentation gap, closed 2026-10-14**: neither buffer previously had
a row in `VARIABLES.md`'s "Comm ROM" table — `[0x464]` only existed
there under its unrelated main-ROM meaning (the `[0x1D10]` menu-item
index), with a stale "not reconciled" caveat. Both buffers now have
their own rows there (`[0x464]`/`[0x466]`, `[0x468]`/`[0x46A]`,
`[0x448]`/`[0x44A]`/`[0x44C]`), and the main-ROM `[0x464]` entry's
caveat now points at them instead of just flagging the collision as
unresolved. This is a genuine same-address reuse across the two ROMs'
separate contexts, not an error.

## 4. CRT layout buffers and screen data paging

```plantuml
@startuml
left to right direction
rectangle "Vector display list\n(physical 0x02200, 0x1400 bytes)" as VDL {
  rectangle "(Y, attr, X, attr)\n4-byte entries,\nwraparound marker" as VDLfmt
}
rectangle "CRT readout text memory\n(physical 0x40000-0x4FFFF)" as TXT {
  rectangle "Dual-plane evidence\n(+0x8000 offset)\n-- inferred, NOT\nconfirmed char+attr" as TXTfmt
}

"print_readout_string" --> "draw_readout_char" --> "plot_readout_point_relative" --> "plot_readout_point" --> VDL
note right of VDL
  Consumer (what actually scans this
  list and paints it to the CRT) has
  never been found - open since the
  original stroke-font hunt.
  Also: draw_readout_char checks
  [0x1B83] and returns immediately
  WITHOUT drawing on both of this
  project's physical units (both are
  Option-12/RS-232 equipped) - this
  path is effectively dead code on
  real tested hardware, not just
  undocumented.
end note

"print_string_far" --> "print_char" --> "write_readout_port_byte" --> TXT
note right of TXT
  Fixed target address (0x40000+0x6F0),
  no software cursor advance. Call sites
  exclusively in self-test-report code.
  This address is confirmed identical to
  the real UART data register's physical
  address - see section 2 above.
end note
@enduml
```

**Screen data paging: confirmed absent.** An exhaustive grep of
`MEMORY_MAP.md`, `FUNCTIONS.md`, `VARIABLES.md`, and every
`docs/display/*` file turned up zero evidence of page-flipping,
bank-switching, or double-buffering for either buffer above. Every
"bank"/"page"/"flip" hit in the existing docs is unrelated: ACQ RAM
banks, front-panel switch banks, service-manual page numbers, and the
vector-icon Y-axis-flip rendering option. This is a genuine gap in
the firmware's documented behavior (or the mechanism genuinely doesn't
exist and the CRT is single-buffered/directly-scanned) — not a hint
that paging logic is waiting to be found with more digging.

Separately, the vector icons at `0xAE64`-`0xB061` are a *different*
buffer/table (different encoding) from the stroke-font display list
above — still unresolved whether they're icons or a rough font. See
`docs/display/vector-icons/README.md`.

## 5. User input: front panel vs. serial — confirmed separate paths

```plantuml
@startuml
box "Front panel" #LightBlue
participant "INT2_HANDLER_LATE\n(per tick)" as INT2
participant "scheduler_tick_service\n(0xE6524)" as Tick
participant "SWB1/SWB2\nphysical 0x403FFA/0x403FFB" as SWB
participant "[0x7B4]\n(edge-detect state)" as Edge
participant "update_menu_position" as UMP
end box

box "Serial / GPIB" #LightSalmon
participant "UART HW" as UART2
participant "poll_comm_status_tick" as PCT2
participant "...decoder chain\n(section 1)" as Chain
participant "comm_call_main_rom" as SCPI2
end box

INT2 -> Tick : every tick
Tick -> SWB : read [0x758]/[0x759]
Tick -> Edge : compare against\nprevious state
Edge -> UMP : on edge,\nCONFIRMED only for\nself-test routines
note right of UMP
  No general-purpose normal-operation
  menu-state-machine consumer has
  been traced. This is a real,
  confirmed gap - not yet found,
  possibly because it's driven some
  other way not yet identified.
end note

UART2 -> PCT2 : every tick\n(separate scheduler slot)
PCT2 -> Chain
Chain -> SCPI2

note over UMP, SCPI2
  No merge point between these two
  input paths has been found anywhere
  in the disassembly. They are
  structurally independent all the
  way from hardware poll to their
  respective (partially-unknown)
  consumers.
end note
@enduml
```

## 6. Boot-sequence branches: diagnostics, module detection, configuration

```plantuml
@startuml
start
:CPU reset -> boot_init\n(see JUMP_MAP.md Level 0\nfor the full low-level sequence);

partition "Diagnostic-verbosity gate (NOT a true debug mode)" {
  :Read [0x758] (SWB2) at boot;
  :Derive [0x1B48] in {0, 1, 3}\n(exact bit logic: JUMP_MAP.md\n"Early boot-time flag setup");
  note right
    PARTIAL finding: there is no
    dedicated "debug mode" - this is
    a verbosity switch only. [0x1B48]==0
    (SELECT C1/C2 held alone) suppresses
    on-screen/printed diagnostic banners
    entirely; MEM1/2/3/MENU ADV holds
    take priority and select a different
    verbosity level.

    Real hardware (both -13 and -14 ROM
    revisions, user-confirmed) dumps
    genuine 9600-baud diagnostic text
    over RS-232 when SELECT C1/C2 is
    held - the emulator doesn't yet
    reproduce this (open item, see
    JUMP_MAP.md's "still genuinely open"
    note).
  end note
}

partition "Subsystem / module detection (CONFIRMED, two independent probes)" {
  :check_comm_option_installed:\nROM-header checksum @ 0x80004\n+ 0xAA55 RAM-probe @ 0x8A004\n-> packed into [0x1BF9];
  :detect_comm_option_hw (0xE75C0):\nhardware register toggle/readback\n-> [0x1B83];
  if ([0x1BF9] bit set?) then (comm option present)
    :comm_rom_boot_init (0x9628C)\ncalled from main ROM (0xE6D5A);
    :init_far_pointer_table,\ninit_comm_device_type_and_defaults,\npoll_dip_switch_change\n(reads rear-panel DIP switches);
  else (not present)
    :skip comm-ROM init entirely;
  endif
  note right
    [0x1B83]'s 0x1E vs 0x14 semantic
    split is still unresolved (gates
    selftest_tb_divider and the RAM/IO
    half of check_comm_option_installed
    elsewhere too).
  endnote
}

partition "Configuration settings" {
  :DIP-switch bank read\n(poll_dip_switch_change,\nsee HARDWARE.md);
  :Staged-settings commit\n(commit_pending_settings_and_\nrefresh_display, 0xE8D64,\ndecodes [0x4e0]-[0x4fc] into\n[0x1b6x]-[0x1b8x]);
  note right
    GAP, confirmed absent: no NVRAM/
    EEPROM load-or-save mechanism has
    been found anywhere. selftest_cmos
    only *tests* the CMOS chip
    (diagnostic), it doesn't read
    settings from it or write settings
    to it. Every boot starts from
    code-level defaults, as far as
    current documentation shows.
  end note
}

:self_test_dispatcher\n(see JUMP_MAP.md Level 1\nfor full detail);
:-> idle halt\n("PRESS MENU KEYS TO CONTINUE");
stop
@enduml
```

## Confirmed gaps (stated honestly, not guessed at)

These came up repeatedly while assembling this object model. Listing
them together so a future session doesn't have to re-derive "is this
actually missing, or did I just not find it":

1. **Screen data paging** — no page-flip/bank-switch/double-buffer
   mechanism for either CRT buffer exists anywhere in current
   documentation. See section 4.
2. **"Debug mode"** — doesn't exist as a distinct mode; what's real is
   a front-panel-gated diagnostic-*verbosity* switch (`[0x1B48]`). See
   section 6.
3. **Configuration persistence (NVRAM/EEPROM)** — no load/save
   mechanism found; settings appear to reset to code defaults every
   boot. See section 6.
4. **The vector display list's hardware consumer** — the code that
   *produces* stroke-font vector entries is well understood; nothing
   that reads the list back and draws it to the CRT has been found.
   Doubly moot on this project's own physical test hardware, where the
   producer itself is gated off. See section 4.
5. **Normal-operation front-panel menu consumer** — `update_menu_position`
   is confirmed only as a self-test consumer of `[0x758]`/`[0x759]`;
   no general-purpose menu-state-machine consumer for ordinary
   (non-self-test) operation has been traced. See section 5.
6. ~~Serial FIFO buffers lack `VARIABLES.md` entries~~ — **closed
   2026-10-14**: both buffers now have dedicated rows in `VARIABLES.md`'s
   "Comm ROM" table (section 3).

## See also

- `docs/comm-rom/rs232-flow-control-and-open-puzzle.md` — serial
  decoder detail, the `[0x1BFA]`-conditional task-slot-5 resolution
- `docs/comm-rom/command-parser-token-scan-and-plot-handler.md` — SCPI
  dispatch layer detail
- `docs/interrupts/task-scheduler.md` — the 12-slot tick table
- `docs/acquisition-and-plotting/mode-dispatcher-and-formatting.md` —
  number formatting, item-list handler cluster
- `docs/display/vector-display-and-stroke-font.md`,
  `docs/display/readout-memory.md` — CRT buffer detail
- `JUMP_MAP.md` — boot sequence, self-test dispatcher, the
  SELECT-C1/C2 open puzzle this file's section 2 depends on, and (as
  of 2026-10-14) its own control-flow-style companion diagrams for
  this file's sections 1 and 5 (the serial-decoder/SCPI-dispatch
  handoff, and the front-panel input poll path)
- `VARIABLES.md` lines ~178-217 — the comm-ROM RAM-structure table
  this file's sections 1 and 3 are drawn from
