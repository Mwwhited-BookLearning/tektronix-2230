# Jump map

Control-flow relationships between major code regions, as activity
diagrams. Starts high-level (system boot, major subsystem boundaries)
and drills into more detail in later sections as specific routines get
understood — add new, more detailed diagrams here rather than
replacing the high-level one, so the overview stays readable.

Every step below is a directly observed jump/call in the disassembly
(see `disasm/sysrom_3532_3633.lst` and `disasm/160-2998-14.lst` for the
raw instructions) — addresses are physical (`chip:offset` per
`disasm/NOTES.md`'s address map). Where the *purpose* of a step is
inferred rather than confirmed, it's marked "(guess)".

## Level 0: system boot, high level

```plantuml
@startuml
start

:CPU reset\n(physical 0xFFFF0, in 160-3532);
:Far jump into 160-3633\n(0xE00B:0001 -> boot_init @ 0xE00B1);
:Set up initial stack (SS:SP = 4000:3FFA),\nclear a RAM buffer;

if (byte at cs:[bx+0x1AF] == 0xFF?) then (yes)
  :take one setup path;
else (no)
  :take the other setup path;
endif
note right: exact meaning of this\nconfig/option byte not\nconfirmed yet

:Far jump to 0xE5D1:00B4\n(ENTRY_E5D1_B4, in 160-3633);
:Clear ES=DS=0x4000 RAM,\nre-load stack pointer;

:Continue into 0xE5D1:00C7\n(ENTRY_E5D1_C7);
:Checksum the low 512 bytes\n(rep lodsw x 0xFF words from 0:0)\n(guess: ROM/config validation);
:Install INT2 (NMI) handler\n-> 0xE5D1:0057;
:Install INT1 (trap) handler\n-> 0xE5D1:019D;
:Install INT 255 handler (early)\n-> 0xE5D1:0090;

:Far call SUB_E5EAE (0xE5D1:019E)\n(guess: runs INT1 handler's init once);
:Far call SUB_E5D67 (0xE5D1:0057)\n(guess: runs INT2 handler's init once);

:Far jump to 0xE6A8:002B\n(SUB_E6AAB, first proper\ncompiled-C-style function found);

partition "SUB_E6AAB: looks like the real init dispatcher" {
  :Call a shared utility twice with\ndifferent pointers, subtract results\n(guess: measuring a buffer/table length);
  :Call subsystem init routines at\n0xE3B1, 0xFDB3, 0xF9FE, 0xF156\n(guess: display / acquisition / front\npanel / other module init - which\nis which not yet identified);
  :Zero/initialize several 12-entry\narrays (channel or trace state?);
  :Call more routines at 0xE75C,\n0xFBCF, 0xE925, 0xE723;
  :Reinstall INT 255 handler (late)\n-> 0xE60B:0005;
  :Reinstall INT2 (NMI) handler (late)\n-> 0xE60B:003A;
  note right: why the same two vectors\nget reprogrammed here is not\nconfirmed - possibly switching\nfrom a POST/diagnostic mode\ninto normal-run mode
  :Far call 0xE5D1:0143;
  :Far call 0xE60B:00B6;
}
:retf (returns to... unknown -\nno caller found yet; this may be\ncalled FROM somewhere rather than\nbeing the true top of the boot path);

stop
@enduml
```

## Level 0: main ROM <-> comm ROM relationship

```plantuml
@startuml
start
:Main ROM's proven-reachable code\nmakes several far calls directly\ninto the comm ROM (0x80000-0x8FFFF);
:Comm ROM's own boot-stub jump\n(both its page-2 and page-3 halves)\nfar-jumps to 0xE64C:0000, landing\ninside the main ROM's own code;
:Comm ROM's own functions make far\ncalls into 0x90000-0x97FFF;
note right: resolved - this is an\naddress-decode ALIAS of the comm\nROM's own 0x88000-0x8FFFF (same\nbytes, second physical address).\nNot a separate device - see\nMEMORY_MAP.md.
stop
@enduml
```

## Level 1 detail: self-test dispatcher

**Corrected this session**: `0xE416F` was misnamed `self_test_dispatcher`
- it actually contains zero test calls, just banner-printing. Renamed
to `print_selftest_banner`. The real OR-fold test dispatcher is
`0xE4244`, called from an unrelated site (`0xE3DEE`). Both routines
happen to be called near each other inside the same outer
report-printing function (`SUB_E07B4`), which is what caused the
original mix-up. See `disasm/NOTES.md` "self_test_dispatcher was
misnamed".

```plantuml
@startuml
start
partition "print_selftest_banner (0xE416F)" {
  :Called from 0xE07F8,\nguarded by [0x1B10]==0;
  :Print "before" banner text\n(SUB_E3567, SUB_E3930, SUB_E3854,\nSUB_E4217 -> print_string_far x2);
  :Print "after" banner text\n(SUB_E374E, SUB_E3930, SUB_E3854,\nSUB_E4217 again);
  :mov [0x1B10], 3;
  note right: NO test calls at all -\npreviously misattributed here.\nSUB_E374E/SUB_E3821/print_string_far\nare display primitives, not tests\n(see MEMORY_MAP.md readout/CRT entry)
}
:Caller far-calls SUB_E094B next\n(same outer routine, unconditional);
partition "SUB_E094B (not yet renamed)" {
  :Load far pointer from [0x1C80]\ninto [0x1B56]/[0x1B58];
  :Write a fixed 3-byte record (3, 2, 0) there;
  note right: looks like initializing a\nsmall counter/record structure, not\nlogging a specific test result - this\ncall site has no test result available\n\nSUB_E097B (companion, 1 call site) then\npacks/dedups nibble values into the\nsame structure
}
stop
@enduml
```

```plantuml
@startuml
start
:Called unconditionally from 0xE3DEE\n(a different, unrelated call site);

partition "self_test_dispatcher (0xE4244, renamed - was SUB_E4244)" {
  if ([0x1B7A]==1?) then (yes)
  else (no)
    :selftest_display_irq_idle\n(MI/line stuck high);
    :selftest_display_irq_active\n(Display controller/TIMEOUT);
  endif
  note right: CORRECTED - an earlier\nread of this function missed this\nchunk of its body entirely and\nwrongly attributed several LATER\ncalls to the surrounding caller\ninstead of here

  :selftest_front_panel_switch_b\n(range 0-0x15, control not confirmed);
  :selftest_acq_ram\n(ACQ_RAM even/odd);

  if ([0x1B83]==0x1E?) then (yes)
    :selftest_tb_divider\n(range 0-0x18, TB_DIVIDER - see HARDWARE.md);
  endif

  if ([0x1B7A]!=1?) then (yes)
    :inline: configure_measurement_hw\n+ poll [0x322] busy bit + clear_\nselftest_status_flags;
    note right: NOT OR-folded -\ninformational, like check_comm_\noption_installed
  endif

  :selftest_hs_acq (HS_ACQ);
  :selftest_front_panel_switch_a\n(range 0-8, control not confirmed);
  :selftest_mm_acq (MM_ACQ);
  :selftest_xy_acq (XY_ACQ);
  :selftest_cursor_delta_time (CDT);
  :selftest_measure_and_report\n(peripheral not confirmed);
  note right: each OR-folded call is\nfollowed by "or [bp-0xA], ax" and\n"mov [0x1B18], 1" - EXCEPT the two\ninformational blocks noted above.\nAll identified via string cross-\nreference or distinctive shape -\nsee NOTES.md/FUNCTIONS.md

  :check_comm_option_installed\n(comm/GPIB option presence + RAM/IO\ncheck - see FUNCTIONS.md);
  note right: also NOT OR'd -\ninformational, not pass/fail

  :selftest_rom_checksum (ROMS/MISMATCH);
  :selftest_comm_rom (COMM_ROM checksum);
  :selftest_comm_loopback_a (COMM_LB);
  :selftest_comm_loopback_b\n(COMM_LB/FGET NOT SET/CLEAR);
  :selftest_comm_ram (COMM_RAM);
  :selftest_cmos (CMOS/reformated/recovered);
  note right: last 5 gated by\ncheck_comm_installed_gate\n(0xE4571) where applicable
}
:retf, result in\naccumulator [bp-0xA] -> ax;
stop
@enduml
```

`check_comm_option_installed` itself:

```plantuml
@startuml
start
:Read word at ES:DI\n(hardcoded here to 0x8000:0004 =\nphysical 0x80004, the comm ROM's\nown BCD-revision + complement bytes);
if (high byte + low byte == 0xFF?) then (yes)
  :Valid ROM header checksum found\n-> set bit 1 of [0x1BF9];
  :Save current word at ES:[DI+0xA000]\n(a DIFFERENT word, physical 0x8A004 -\nnot the same address just checked);
  :Write test pattern 0xAA55 there;
  if (read-back == 0xAA55\nAND [0x1B83] == 0x1E?) then (yes)
    :Also RAM/IO-backed\n-> set bit 2 of [0x1BF9];
  else (no)
  endif
  :Restore the saved word;
else (no)
  :No option board detected here;
endif
:retf;
stop
@enduml
```

**Cross-ROM handoff found**: `comm_rom_boot_init` (`0x9628C`, comm ROM)
is called exactly once, from the main ROM at `0xE6D5A`, gated on
`[0x1BF9]!=0` (i.e. only when `check_comm_option_installed` found the
option board present, per the checksum diagram above) - this is the
main boot sequence handing off to the comm ROM's own initialization
(`init_far_pointer_table`, `init_comm_device_type_and_defaults`,
`poll_dip_switch_change`, reading the rear-panel DIP switch bank - see
`HARDWARE.md`) once its presence is confirmed.

## Level 1 detail: (more as identified)

As the remaining subsystem-test subroutines above get identified, add
a Level-1 diagram here per subsystem showing its own internal control
flow (and update `FUNCTIONS.md` + the relevant `.symbols.json` at the
same time). Still open (still-unnamed, per `TODO.md`'s renaming
tracker): `SUB_F156E`/`SUB_F1581` and `SUB_FBCF3`.
