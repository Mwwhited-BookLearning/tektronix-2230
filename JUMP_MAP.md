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
:Far jump into 160-3633\n(0xE00B:0001 -> phys 0xE00B1);
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
:Comm ROM's own functions make far\ncalls into segments at 0x90000+\n(the real remaining memory mystery -\nsee MEMORY_MAP.md);
note right: these targets aren't\ncovered by any ROM dump we have -\nmay be RAM populated at runtime,\ncontents unknown
stop
@enduml
```

## Level 1 detail: (none yet)

As specific subsystem-init calls (`0xE3B1`, `0xFDB3`, `0xF9FE`,
`0xF156`, `0xE75C`, `0xFBCF`, `0xE925`, `0xE723`, ...) get identified,
add a Level-1 diagram here per subsystem showing its own internal
control flow, and link back to this file from `disasm/NOTES.md` and
`TODO.md`.
