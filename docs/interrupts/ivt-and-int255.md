# Interrupt vector table and INT 255

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Interrupt vector table entries (real code entry points)

The reset routine and two later routines (`SUB_E5E53`, inside what's
now `INT255_HANDLER_EARLY`'s neighborhood) install real 8086 interrupt
vectors by writing directly into low memory with `es=0` (or `es=0x3F`,
whose paragraph base `0x3F0` still lands inside the 1KB IVT that spans
physical `0x000-0x3FF`). Found by tracing `mov <reg>, imm` / `mov
es,<reg>` / `mov word [es:bx(+disp)], <reg>` triples through the
already-decoded code:

| IVT slot (int #) | Handler installed | When |
|---|---|---|
| 0x008 (INT 2, NMI) | `E5D1:0057` (`INT2_HANDLER_EARLY`) | at reset |
| 0x004 (INT 1, trap) | `E5D1:019D` (`INT1_HANDLER`) | at reset |
| 0x3FC (INT 255) | `E5D1:0090` (`INT255_HANDLER_EARLY`) | at reset |
| 0x3FC (INT 255) | `E60B:0005` (`INT255_HANDLER_LATE`) | later, overwrites the above |
| 0x008 (INT 2, NMI) | `E60B:003A` (`INT2_HANDLER_LATE`) | later, overwrites the reset-time NMI handler |

**`INT2_HANDLER_EARLY` read closely (renamed from `SUB_E5D67`)**: it's
the direct counterpart of `INT255_HANDLER_EARLY`/`_LATE` (same
prologue shape, same "early installed at reset, later replaced"
lifecycle) - reads the hardware tick bytes at physical `0x403FFA`/
`0x403FFB` into `[0x758]`/`[0x759]`, increments the tick counter
`[0x752]`, ORs `[0x1AF2]` into the pending-work flags `[0x1AEE]`, and
calls `delay_read_128w`, all before `iret`. `INT2_HANDLER_LATE` (below)
adds the actual task-scheduler behavior on top once installed - the
EARLY/LATE split for INT2 mirrors the diagnostic-vs-normal-mode
distinction already suspected for INT255.

**`INT255_HANDLER_LATE` calls `run_continuous_selftest_tick` (renamed
from `SUB_E62EE`) on every tick** - this ties `PRC_READBACK`
(`verify_prc_readback_pattern`) into a genuine background watchdog: it
dispatches to `verify_prc_readback_pattern` when `[0x1B5E]` is set (or
a fallback `SUB_FDB8F` otherwise), tracks 3 independent sticky/debounce
failure counters, and on a clean result calls through a hook function
pointer at `[0x740]`. So `PRC_READBACK` isn't only a menu-invoked self-
test - it (or its fallback) genuinely runs continuously in the
background via the interrupt-driven scheduler, consistent with it
checking live hardware readback rather than a one-shot boot check.

These 4 non-`INT2_HANDLER_EARLY` handlers are entry points nothing in
the program's direct call graph would ever reach (only the
corresponding interrupt firing calls them), so they were added to
`gen_disasm_x86.py`'s `ENTRY_POINTS` list and seeded directly. This
raised coverage from 9,369 to 10,360 instructions. Re-running the same
vector-tracing search against the newly-reached code found no further
handlers - this specific lead is exhausted for now, though other
interrupt-installing sites may still exist in code we haven't reached
by other means.

INT 1 (trap/single-step) being explicitly handled is a little unusual
for shipped production firmware and might be diagnostic/self-test
infrastructure left in; INT 2 (NMI) and INT 255 (a software-only
vector, not a CPU exception) getting reprogrammed mid-run suggests the
firmware switches between distinct operating states (e.g. a
diagnostic/POST mode vs. normal-run mode) with different NMI/software-
interrupt handling per state - worth confirming once
`INT255_HANDLER_EARLY`/`_LATE` and `INT2_HANDLER_LATE` are read closely.
## MAJOR CORRECTION: INT 255 is NOT "a software-only vector" - it's the real hardware Maskable Interrupt (`INTR`), confirmed from the manual

Found 2026-09-13 reading further into the service manual's Theory of
Operation narrative text (not a table this time - the prose right
after the ROM/decoder description, page 3-26): *"Other interrupts to
the Microprocessor cause vectoring to addresses that start the
interrupt handling routines. The NMI (non-maskable interrupt) vector
is at 00008, and the Maskable Interrupt (INTR) is vectored to 03FC
(both interrupt vectors are in RAM)."*

`0x008` and `0x3FC` are exactly this project's own already-documented
`INT2`/`INT255` IVT slots. **This overturns the "INT 255 is a
software-only vector, not a CPU exception" assumption** stated a few
paragraphs above (and repeated elsewhere in this file) - `INT255_
HANDLER_LATE` is the real, hardware-driven Maskable Interrupt Service
Routine, fired whenever any peripheral asserts the CPU's `INTR` pin
(gated by the `IF` flag) - **including the RS-232 option's UART**,
whose combined `DR+INTR`/`TBRE` signal the Option 12 Theory of
Operation section separately confirms drives *"the microprocessor's
maskable interrupt."* This directly resolves the reachability caveat
raised earlier this session about the comm task loop, and gives a
real, primary-source-confirmed path from "byte arrives at the UART"
to "firmware code runs":

**`INT255_HANDLER_LATE` → `run_continuous_selftest_tick` → the
`[0x740]` hook (previously "not yet traced" per `FUNCTIONS.md`) → when
`[0x1BF9]` (comm installed) is set, `init_comm_dispatch_table` points
this hook at physical `0x96F54`** (comm-ROM alias, decoded directly via
`capstone` since it's outside the direct-mapped `160-2998-14.lst`
range). Read that function's full body - it's a real, `[0x629]`-aware
(RS-232-vs-GPIB) per-tick comm status poller:
- For RS-232 (`[0x629]==0`), skips the GPIB-specific block entirely
  and goes straight to: reads `[0x6DA]` (the GPIB/comm DIP-switch-
  config byte, bit `0x80`), compares against the last-seen value
  (`[0x45E]`) and, on a change, updates flag bits in `[0x732+0x97]`
  and calls `0x839D1` (the same target `poll_dip_switch_change`-style
  code elsewhere calls on a switch change).
- Checks bit `0x02` of that same byte - if set, **records a 4-byte
  snapshot of the comm channel status structure `[0x6D6]` (`+1`, `+3`,
  `+0`) into a circular log buffer** (far pointer `[0x464]`, 32
  four-byte slots spanning `0x46C`-`0x46C+0x80`, wrapping around).
- Checks the RX-ready flag `[0x596]` (the same "ready" byte `set_comm_
  queue_busy`/`update_comm_tx_ready_flag` manage - see the Interrupt
  Mask Latch section above) alongside failure counters, and bit `0x04`
  of the status byte - which, if set, calls `0x8006:0x9C` (a further
  comm-ROM dispatch target, not yet traced).

**This is almost certainly the real "something arrived, go handle it"
heartbeat this whole session was looking for** - reached via a
confirmed hardware interrupt chain, not the polling loop (`FUNC_2998_
39F5`) whose reachability couldn't be established earlier. Given `INT
255`/`INT 2` fire on every tick regardless of what's running, this
tick-driven poller (not a byte-at-a-time interrupt handler) is
plausibly *how* incoming RS-232 activity actually gets serviced in
this firmware - polled from a guaranteed-frequent hardware tick,
rather than a dedicated per-byte ISR.

**Follow-up, same session**: traced both call targets immediately -
**neither is new code**, both are already-documented housekeeping:
`0x839D1` is `FUNC_2998_39D1`, which just re-invokes `draw_boot_splash_
and_option_icon` (redrawing the installed-option status icon whenever
the DIP-switch byte's bit `0x80` changes - not boot-only after all,
apparently also live-refreshed); `0x8006:0x9C` is exactly `0x800FC` =
`update_comm_tx_ready_flag`. The GPIB-branch's own `0x8006:0` call is
`0x80060` = `set_comm_flow_hold`. **None of these three leads to a
byte-reception/keyword-matching function** - `poll_comm_status_tick`
really does look like a status-*sync* poller (redraw an icon, keep the
TX-ready/flow-control flags in step with the comm channel's own status
byte), not the place incoming bytes actually get read and parsed. The
real byte-reception path is **still not found** - it's back to either
`process_gpib_command_byte`'s chain (via the still-unconfirmed-
reachable `FUNC_2998_39F5`), or a genuinely different, not-yet-found
hook. This tick-driven interrupt chain was still worth fully tracing
(it resolved the hardware-interrupt-exists question definitively and
produced real, useful documentation), it just isn't the specific
answer to "where does `ID?` get parsed."
