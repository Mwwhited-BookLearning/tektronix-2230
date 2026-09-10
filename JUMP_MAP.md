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

```plantuml
@startuml
start
:Called from 0xE07F8,\nguarded by [0x1B10]==0;

partition "self_test_dispatcher (0xE416F)" {
  :Call subsystem test #1 (0xE374E);
  :Call subsystem test #2 (0xE3821);
  :Call subsystem test (0xE0AF5) - twice;
  :Call subsystem tests at 0xE3F2C,\n0xE3F99, 0xE2FC8, 0xE1B16, 0xE252A,\n0xE0ADD, 0xE0DCC, 0xE0E56, 0xE28FE,\n0xE227E, 0xE26D6, 0xE286C, 0xE2CEC,\n0xE0FD0;
  note right: each call above is followed\nby "or [bp-0xA], ax" - folding a\nreturn code into an accumulating\nresult word - and "mov [0x1B18], 1"\n\nNone of these ~20 subroutines directly\nreference the diagnostic strings in\nSTRINGS.md (checked) - see FUNCTIONS.md,\nstill unidentified by name

  :Call check_comm_option_installed\n(comm/GPIB option presence + RAM/IO\ncheck - see FUNCTIONS.md);
  note right: notably NOT OR'd into\nthe [bp-0xA] accumulator like its\nneighbors - informational, not a\npass/fail test

  :Call subsystem tests at 0xE16EA,\n0xE1E3E, 0xE1D28, 0xE1DB3, 0xE1E90,\n0xE1F18;
  :Call SUB_E553B (x3), SUB_E6D2F,\nSUB_E4429 (x2)\n(guess: end-of-sequence cleanup/\nreporting, outside the main\nper-subsystem loop);
}
:retf, result in accumulator\nat caller's [bp-0xA];
:Caller far-calls SUB_E094B next;
partition "SUB_E094B (not yet renamed)" {
  :Load far pointer from [0x1C80]\ninto [0x1B56]/[0x1B58];
  :Write a 3-byte record (3, 2, 0) there;
  note right: looks like logging the\ntest run into a small record/event\nbuffer, not printing directly - may\nexplain why no test subroutine\nreferences a message string itself
}
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
  :Save current word at that address;
  :Write test pattern 0xAA55;
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

## Level 1 detail: (more as identified)

As the remaining subsystem-test subroutines above get identified, add
a Level-1 diagram here per subsystem showing its own internal control
flow (and update `FUNCTIONS.md` + the relevant `.symbols.json` at the
same time). Also still open from the main boot sequence: what
`SUB_E3B1`/`SUB_FDB3`/`SUB_F9FE`/`SUB_F156` (called from `SUB_E6AAB`)
and `SUB_E75C`/`SUB_FBCF`/`SUB_E925`/`SUB_E723` actually do.
