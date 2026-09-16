# RS-232: early hardware investigation

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Live hardware test: the `write_readout_port_byte`/UART theory does NOT hold up on real hardware

The user has a real 2230 with the RS-232 option installed, a USB-
serial adapter wired to the rear-panel RS-232-C port, and read the
physical `PARAMETERS` DIP switches directly off the unit as `0110000000`
(confirmed via the switch's own printed reference as selecting 600
baud). This let the "genuine UART on RS-232-equipped units" theory
(see the "Follow-up, same day" `write_readout_port_byte` section
above) be tested directly instead of staying speculative.

**Method**: `disasm/listen_serial.py` (new small tool, opens a COM
port and logs every received byte as hex + ASCII, regardless of
whether it decodes cleanly - the point is to see raw bytes even at a
wrong baud rate guess). Tested 3 scenarios:

1. Plain power-cycle, listening at 9600 baud (a wrong guess, before
   the real 600 baud was confirmed): exactly 2 bytes, both `0x00`,
   a few seconds apart.
2. Plain power-cycle again, same 9600 baud: **the identical result**
   (2 bytes, both `0x00`).
3. Plain power-cycle at the *correct* 600 baud: **the same 2×`0x00`
   result again**.
4. **An actual self-test run from the `DIAGNOSTICS/TESTS` menu**
   (not just a power-cycle) at the correct 600 baud: **zero bytes**.

**Interpretation**: result set {1,2,3} getting the *identical* byte
pattern at two baud rates 16x apart (`600` vs `9600`) is itself
strong evidence that the 2×`0x00` signal is **not real serial data**
at all - genuine text garbled by a wrong baud-rate guess produces
*different* garbage at different rates, not the same 2 bytes. Far
more likely explanation: a hardware transient on the RS-232 line
(e.g. the line driver chip's own supply rail settling during power-on/
off) that many USB-serial adapters report as a spurious byte
regardless of the configured baud rate, unrelated to firmware
behavior entirely.

**Result 4 is the important one**: with the actual self-test code
path exercised (not just power-on) at the *confirmed-correct* baud
rate, **nothing at all** came through. If `write_readout_port_byte`'s
writes were genuinely reaching a UART transmit-data register, running
the real self-test should have produced *something*, even imperfectly
framed. Getting a clean zero here is real evidence against the "self-
test banner goes out the RS-232 UART" theory - stronger than the desk
research alone could produce either way (that research could only
narrow down *candidate* explanations, not test the actual running
firmware).

**Not fully conclusive** - a few things could still explain the null
result without fully clearing the UART theory: the specific self-test
invoked by the user might not be the exact one `print_selftest_
banner`'s callers are reached from; the comm option's presence-
detection (`[0x1B83]`) might not be recognizing the board as installed
for whatever specific reason gates this path; or there could be a
flow-control/handshake line (RTS/CTS, DSR/DTR) the adapter isn't
asserting that the real UART needs before it will transmit. Still,
taken at face value this is the **first negative evidence against**
the UART re-interpretation from actual hardware, tempering the
"substantially resolved" language used earlier today - back to
genuinely unresolved, now with real experimental data on both sides
rather than just documentation-derived inference.

**Correction, later session (service manual reading): this "zero
bytes" result is fully explained regardless of whether the UART theory
is right or wrong, so it's weaker evidence than it looked.** The
service manual states plainly: *"If the instrument contains the
RS-232-C Option, an ASCII version of **all errors found** during
power-up is sent to the option."* This is **error-conditional** - a
healthy instrument that passes every diagnostic test sends nothing at
all over RS-232, by design, regardless of whether
`write_readout_port_byte`'s cluster is a real UART register. Every
"zero bytes" result gathered so far (power-cycles and a manually-run
self-test, at 3 different baud-rate guesses, on hardware later
confirmed healthy) is consistent with **both** "the theory is wrong"
**and** "the theory is right but nothing ever failed to report." The
flow-control caveat above also turned out to be partially real (some
cables on this hardware do need RTS/DTR asserted - see
`hardware/manuals/2230_programming/PRACTICAL_GUIDE.md` - though that
was found via the *command/response* protocol, not this specific
self-test path).

**Found a much better test, then hit a dead end**: the service manual
separately documents a `DIAGNOSTICS/EXERCISERS/COM-OPTION/DEBUG`
screen, described as unconditional, not error-gated - *"This exerciser
is used in debugging the communications option. Debug outputs a test
message and displays any incoming messages (data) on the crt."* This
would have transmitted something over RS-232 just by being entered,
pass or fail, making it the right experiment for testing this theory
directly. **Checked live the same session: this entry does not exist**
in the real `EXERCISERS` menu on this Option-12 unit - confirmed by
photo, the actual children are only `CONFIGURATION`/`IO`/
`A_TO_D_TESTS`. See `HARDWARE.md`'s menu tree. The service manual
likely covers the whole 2200 series in one document and this entry may
be specific to a different model or to Option 10 (GPIB) rather than
Option 12 - not confirmed either way. **Dead end for testing the UART
theory this way**; still genuinely open, per `STILL_PENDING_DECODE.md`.

## Follow-up live hardware session: cable fully validated, `COMM_LOOPBACK` proven internal-only, comm-detection failure now the leading theory

Extended the RS-232 investigation above with a much more thorough
hardware troubleshooting pass, on **two different physical 2230
units**, both confirmed Option 12 (RS-232, not GPIB - cross-checked
against the operator's manual's Option 10 vs Option 12 sections to
make sure the right pin/switch tables were being used).

**Ruled out, in order**:
1. **PC/adapter side**: a loopback at the USB-serial adapter itself
   (`serial.Serial` write then read back) round-tripped a test string
   byte-for-byte. Adapter, driver, and COM port are all fine.
2. **The DIP-switch table itself**: got the actual operator's manual
   (`hardware/070-4998-02.pdf`, Tables 7-11/7-12/7-13 for RS-232,
   Table 7-6 for GPIB) and confirmed the full switch-to-function
   mapping directly rather than inferring it - see `HARDWARE.md`.
   Both units' live switch readings (`0110000000`=600 baud) matched
   the manual's table, and switch 8=`0` on both = **CR-only**
   terminator, matching PuTTY's default - ruling out a terminator
   mismatch. **Correction, 2026-09-14**: the `1110000000`=9600 reading
   noted here at the time was itself wrong - the user had misread
   Table 7-12's own bit-order header ("`4321`", meaning switch 4 is
   the MSB, not switch 1) and `1110000000` was actually running at
   **1200 baud**. `0110000000`/600 happened to be a bit-palindrome so
   it validated under either reading, masking the error until the very
   next test. See `HARDWARE.md`'s corrected DIP-switch section for the
   full writeup - all baud-rate tests in this session's live-hardware
   sections that used `1110000000` were actually running at 1200, not
   9600, though this doesn't change any of the conclusions reached
   (the tests were about detecting *whether* real data appeared, not
   about the specific baud value).
3. **Switch changes not latched**: manual confirms "changes to the
   PARAMETER switch after power on will not be read until the next
   power on occurs" - power-cycled after switch changes each time to
   make sure the new setting was actually in effect before testing.
4. **REMOTE lockout**: manual confirms queries (like `ID?`) are
   answered even with `REMOTE OFF` (the power-on default) - only
   commands that *change* a setting are rejected in that state. Not
   the blocker.
5. **The cable itself, thoroughly**: built a real 3-pair loopback plug
   (`2-3` TX/RX, `4-5` RTS/CTS, `6-20` DSR/DTR, later also tying in
   pin 8/RLSD-DCD) and looped it through the *actual* cable (not just
   the adapter) - a test string round-tripped byte-for-byte with
   `CTS`/`DSR` both reporting asserted. Cable, connectors, and all
   three signal pairs are fully sound on both DTE and DCE ends.

**Sending `ID?` (a real, confirmed command from the operator's manual's
command reference, expected response format `ID TEK/2230,V81.1,VERS:
09;`) got zero bytes back** - tested directly via a Python/`pyserial`
script (bypassing PuTTY entirely) on **both** scopes, on **both** the
DTE and DCE connectors, at the confirmed-correct baud/parity/
terminator, immediately after a power cycle. Uniformly silent every
time.

**The `COMM_LOOPBACK` self-test (`DIAGNOSTICS/TESTS/SYSTEM` menu)
reports `UNTESTED` on both units** - not `PASSED`, not `FAILED`, not
`Not installed` either (see `format_selftest_result_string`,
`0xE0C3D`: `0x20`=not installed, `0x02`=failed, `0x01`=passed, else
untested - so this is genuinely the "none of the above" catch-all
case). Tested with **no** external loopback connector attached, then
again with the full validated loopback plug (`2-3`/`4-5`/`6-20`, then
again with pin 8 added into both groups) attached to the scope's RS-
232 port - **identical `UNTESTED` result every time**, completely
insensitive to what's on the external line. This is decisive: `COMM_
LOOPBACK` is confirmed to be checking something purely internal to the
comm board, not the actual external RS-232 wiring (correcting nothing
about the earlier "no wiring needed" answer given to the user - if
anything, this is now directly confirmed rather than just assumed).

**Leading unifying theory**: the firmware's comm-hardware-detection
logic (`detect_comm_option_hw`, `[0x1B83]` - see "Found: the actual
source of `[0x1B83]`" above) isn't recognizing either unit's comm
board as installed, for whatever reason actually drives that
write-probe's outcome. This single explanation covers **everything**
observed in both live-hardware sessions:
- Why `COMM_LOOPBACK` never produces a real pass/fail (its self-test
  leaf is plausibly skipped/short-circuited before doing anything,
  leaving a status byte that was never written to a real value)
- Why the self-test banner text never reached RS-232 in the first
  session (same gating, if `write_readout_port_byte`'s cluster is
  reached only when the comm option is considered present)
- Why `ID?` gets no response now, on either unit, over an
  electrically-proven-good line

**Not proven** - this is the best current explanation, not a closed
case: it's still consistent with *some* deeper physical fault common
to both units (unlikely but not impossible), or with the real command
parser genuinely requiring something not yet tried (a different
command entirely, `HELp?` instead of `ID?`, or a required leading
byte/prefix not yet identified in the manual excerpts read so far).
The next concrete step, if picked up again: trace exactly what other
code paths gate on `[0x1B83]`'s value (particularly whatever `COMM_
LOOPBACK` and the command-parser entry point check), and see whether
either unit's specific write-probe addresses (`0x40000+0x7DE`
write, `0x40000+0x377E` bit `0x1000` readback) could plausibly fail
even with genuinely-present, genuinely-working comm hardware - i.e.
whether the *detection* itself might be the actual bug/limitation,
separate from the comm board's real capability.

**Strong new lead, from `hardware/070-6090-00.pdf`** (the F10/GPIB and
F12/RS-232 *option installation instructions*, provided by the user
2026-09-13 - a short 6-page document, image-only scan, read via
`pymupdf` page rendering since it has no text layer): installing
either option requires **moving jumper `P9107` on the Storage circuit
board "one pin over toward the center"** (step 22, Figure 3). A jumper
that must be physically relocated for the comm option to work is
exactly the kind of thing that could explain firmware not recognizing
the board as installed - if `P9107` isn't in the position the option
expects on one or both of the two physical test units, that alone
could account for the comm-detection failure theory above, with no
code-level bug needed at all. Worth physically checking `P9107`'s
position on both units before assuming a firmware/detection bug -
genuinely the single most actionable next physical step, cheaper than
tracing more disassembly. (Also resolved a side question from the same
document: the "2 ROMs" shipped with the F10/F12 option kit are
**replacement main system ROMs** (`U9109`/`U9110`, i.e. `sys_rom_0`/
`sys_rom_1` - Table 1 confirms `160-3633-XX`/`160-3532-XX` for a 2230),
required because the option requires a matching main-ROM firmware
version - not a second, undiscovered comm-board ROM. No teardown
needed to answer that particular question.)

## Correction: `COMM_LOOPBACK`'s `UNTESTED` result fully traced - NOT a `[0x1B83]` comm-detection failure after all

Picked back up via pure code tracing (no hardware needed) per the
user's "keep reviewing code without me" request. Read the actual
bodies of `selftest_comm_loopback_a`/`_b` (`0xE1D28`/`0xE1DB3`) and
what they call - `selftest_comm_readback` (`0xE20B0`) and `selftest_
comm_fget_flag` (`0xE1FBC`) - which hadn't been fully traced
instruction-by-instruction before today. This **overturns** the
"comm-hardware-detection failure via `[0x1B83]`" theory from the
session above - the real mechanism is fully understood now and doesn't
implicate `[0x1B83]` at all:

1. **Both `_a` and `_b` start by calling `check_comm_installed_gate`
   (`0xE4571`)**, which checks `[0x1BF9]&1` (set by `check_comm_option_
   installed`'s ROM-header-checksum probe at physical `0x80004` -
   entirely internal to the comm ROM chip itself, no external RS-232
   wiring involved). If this bit is clear, both return `0x20`
   ("Not installed") immediately. **Since the user's screens showed
   `UNTESTED`, not `Not installed`, this proves the comm ROM header
   probe succeeds on both units** - the comm board genuinely is
   detected as present at this level. `[0x1B83]` isn't even read by
   this gate.
2. **`selftest_comm_loopback_a` calls `selftest_comm_readback`**,
   which resets `[0x1BFA]=0`, then reads a status byte from physical
   `0x4067C` (masked/shifted from 2 different sub-fields) into a local,
   and compares it against 5 possible values: `0x40`/`0x60`/`0xC0`/
   `0xD0`/`0xE0`. **All 5 count as PASSED** for `_a` itself - but only
   the `0xD0` case *also* sets `[0x1BFA]=1` before returning; any other
   passing value leaves `[0x1BFA]=0`. Any value outside that set of 5
   is a genuine FAIL (prints an error report line, clears `[0x1BF9]`).
3. **`selftest_comm_loopback_b` calls `selftest_comm_fget_flag`**,
   which checks `[0x1BFA]` first: if nonzero (i.e. `_a`'s readback was
   exactly `0xD0`), it runs its own real hardware check (writes/reads
   physical `0x406F0`-`0x406F3`/`0x4067C`/`0x406BC`, already documented
   above). **If `[0x1BFA]` is zero** (the readback landed on `0x40`/
   `0x60`/`0xC0`/`0xE0` instead of `0xD0`), it instead checks
   `[0x1B7A]`: if that's also zero, returns `1` (PASSED, no real check
   run at all); **if `[0x1B7A]` is nonzero, returns `0x80`** - a value
   `format_selftest_result_string` doesn't recognize as pass/fail/not-
   installed, so it prints the generic **`UNTESTED`**.
4. `[0x1B7A]` (see `VARIABLES.md`) is read at dozens of self-test-
   adjacent sites but only written at 2 (heuristic-only) sites - set to
   `1` on what looks like entry into the full automated self-test
   sequence, cleared to `0` near a `reset_display_and_notify_comm`
   call. The user's live tests were run via `DIAGNOSTICS/TESTS/SYSTEM`
   (the full sequence, per the menu path visible in their screenshots)
   - exactly the context where `[0x1B7A]` would be `1`.

**Conclusion**: `COMM_LOOPBACK` showing `UNTESTED` is fully explained
by the comm-board's readback register (physical `0x4067C`) returning
one of 4 "pass but not `0xD0`" values while the test runs inside the
full sequence - **not** a firmware failure to detect the comm option
(that detection, `[0x1BF9]`'s header check, already succeeded earlier
in the same run). This is a real, mechanistic answer, not a guess -
every branch above was read directly from the disassembly. It also
means the `P9107` jumper lead above is **less likely** to be the
answer for *this specific* symptom (`COMM_LOOPBACK`/`UNTESTED`) than
first thought, though it could still matter for the separate `ID?`-
gets-nothing puzzle, since that's a different code path (the comm
ROM's own RS-232 command parser, not yet traced this session - the
comm ROM's disassembly coverage is much thinner than the main ROM's).

**Worth live-testing if picked up again**: run `COMM_LOOPBACK` from
`DIAGNOSTICS/EXERCISERS` standalone (not the full `TESTS/SYSTEM`
battery) - if `[0x1B7A]` really is only set during the full sequence,
running it standalone should show a real `PASSED`/`FAILED` instead of
`UNTESTED`, which would confirm this whole trace end-to-end against
real hardware. Also unexplored: what physical condition makes the
`0x4067C` readback land on exactly `0xD0` vs. the other 4 passing
values - possibly a GPIB-vs-RS-232 board variant distinction (`0xD0`
might specifically mean "RS-232 board with the FGET-capable
variant"), not yet traced back to a specific bit/signal name.

## Traced the comm ROM's byte-dispatch/parser core, looking into what RS-232 commands are expected

Following up on the separate, still-open `ID?`-gets-nothing puzzle
(distinct from the `COMM_LOOPBACK` correction above), went looking for
the comm ROM's actual incoming-command parser - not the TX-side
(`enqueue_comm_char`/`service_comm_tx_queue`, already well documented)
but the RX/command-recognition side, which `TODO.md` has flagged as
genuinely unexplored ("no genuine incoming-byte ring buffer/interrupt
handler identified yet").

**Found a real, active byte-classification/dispatch core**:
`process_gpib_command_byte` (`0x8526B`) reads a small DS-relative
scratch cell `[6]` (the "current byte", reused across many call sites
- not a fixed low-memory/IVT address despite the small literal, since
it's DS-relative and this ROM's data segment sits at paragraph `0x41`,
i.e. physical `~0x416`), checks bit `0x80` on it (very plausibly a
GPIB ATN/address-byte flag, packed into the same byte as the data),
clamps it to a max of `0x42`, and uses it to index a **4-byte-per-
entry table** via far pointer `[0x712]` to build a target address in
`[0x582]`/`[0x584]` - classic byte-class/dispatch-table shape. It then
**unconditionally calls a second function** (tentatively named
`advance_comm_input_state`, physical `0x97905` - in the `0x90000`
comm-ROM alias range, so not visible in `160-2998-14.lst`'s direct
mapping; decoded directly via a one-off `capstone` script instead,
same technique as earlier sessions) which does its own lookup into the
**same** `[0x712]` table (this time indexed by a parallel cell,
`[0x580]`), and - **explicitly gated on `[0x629]`** (the RS-232-vs-
other mode flag) - updates the comm channel status structure at
`[0x6D6]` (`+3`=command register, set to `0x98` or `0x18` depending on
branch; `+5`=a flags byte OR'd with a bit from I/O port `[0]`) and
toggles bits in a flags byte at `[0x732+0x97]`.

**What this confirms**: the RS-232 path through this parser is real
and actively maintained code, not a GPIB-only stub with RS-232 quietly
unsupported - the `[0x629]` branches do genuinely different, non-
trivial work in both directions. This is reassuring for the "why does
`ID?` get nothing" investigation: it rules out "RS-232 command parsing
was simply never implemented" as an explanation.

**What's still NOT found, despite this**:
1. **`[0x712]`'s actual contents/initialization**. It's clearly a real
   far-pointer table (used identically by two different functions,
   with a consistent 4-byte stride and a sensible max-index clamp),
   but no literal `mov [0x712], ...` write exists anywhere in the
   direct-mapped listing, and it's **not** one of the comm ROM's own
   `init_far_pointer_table`'s 31 destination slots either (re-decoded
   that table directly this session to check - all 31 destinations
   land in physical `0x8FED6`-`0x8FF60`, nowhere near `0x712`). So the
   table's real character-class/dispatch data - which would show
   exactly which input bytes/keywords this parser recognizes - is
   still not located. Worth checking the heuristic listing or a
   broader search for whatever *does* set `[0x712]` next time this is
   picked up.
2. **The actual UART-receive entry point** - where a real incoming
   RS-232 byte first lands in `[6]`/`[0x580]` in the first place (an
   interrupt handler, or a polled read of the comm option's UART/GPIB
   chip registers at `0x406F0`-`0x406F3`). Not found this pass either -
   `SUB_97B94` (initially suspected as a "read next byte" primitive
   given how it's called from a loop right before this dispatch chain)
   turned out to already be identified as `set_comm_critical_flag`
   (`FUNCTIONS.md`) - a critical-section flag setter, not a receive
   function. So the genuine RX path remains exactly as open as
   `TODO.md` already said.

**Net effect on the `ID?` puzzle**: still unresolved, but narrowed -
there IS a real, mode-aware, RS-232-branching parser core actively
processing classified bytes; the missing pieces are (a) what the
dispatch table actually recognizes, and (b) how a byte from the actual
RS-232 wire reaches this code at all. Either one, if found, would be a
much more direct route to explaining the silence than further hardware
troubleshooting at this point - the electrical layer has already been
thoroughly proven sound (see the "Follow-up live hardware session"
section above).

### Follow-up: found `[0x712]`'s actual contents

Re-derived the DS segment `process_gpib_command_byte`/`0x97905` run
under: `FUNC_2998_39F5` (the function containing the `L_83A04` main
comm-task loop that calls `process_gpib_command_byte`) swaps `DS` to
segment `0x8F80` at its own entry (`mov di,0x8f80; push di; lcall
set_ds_return_old`) and never restores it before entering that loop -
so `[0x712]` there is physical `0x8F800+0x712 = 0x8FF12`. That address
**is** one of the comm ROM's own `init_far_pointer_table`'s 31
destinations after all (I'd checked the wrong segment assumption
initially, `0x41` instead of `0x8F80`) - entry `dest_off=0x42` points
there, with stored far pointer `0x900F:0x0004`, i.e. physical
`0x900F4` (in the `0x90000` comm-ROM alias - file offset `0x80F4` in
`160-2998-14.bin`).

**Dumped all 67 entries (index `0x00`-`0x42`, matching the clamp
range) directly from the binary.** Each entry's first byte groups
cleanly into contiguous ASCII ranges:

| Handler ID | ASCII range covered |
|---|---|
| `0` | `NUL` (0x00) only |
| `1` | `SOH` (0x01) only |
| `2` | 0x02-0x1F (remaining control codes) plus `<space>`-`%` (0x20-0x25) |
| `3` | `&` `'` `(` (0x26-0x28) - **also the clamp target for every character above `0x42`**, i.e. all of `C`-`Z` and beyond effectively fall into this handler too |
| `4` | `)` `*` `+` `,` `-` `.` `/` `0` `1` `2` (0x29-0x32) |
| `5` | `3` `4` `5` `6` `7` `8` `9` `:` `;` `<` `=` (0x33-0x3D) |
| `8` | `>` `?` `@` `A` `B` (0x3E-0x42) |

**Confirmed**: this table exists, is indexed by raw ASCII code (not by
keyword), and groups characters into a small number (7 distinct
handler IDs seen: `0,1,2,3,4,5,8` - `6`/`7` don't appear in this
range) of behavior classes via each entry's first byte. The remaining
3 bytes per entry increment steadily *within* each handler group
(looks like a per-character slot/sequence value passed to whichever
handler routine `0-8` corresponds to - handler dispatch itself, e.g. a
jump table on this byte, not yet located).

**Inferred, not confirmed**: handlers `4` and `5` together cover
*exactly* the 10 digits (`0`-`9`) plus immediately-adjacent punctuation
(`+ - . / : ; < =`) - a very clean fit for "numeric parameter
lexing" (the command language needs to parse things like channel
numbers, `STOP 1`/`STOP 2`, decimal values, etc.). Handler `2`'s broad
bucket (whitespace/control codes plus early punctuation) reads as a
plausible "ordinary/no special meaning" default class. Handler `3`
being both a specific 3-character range *and* the fallback for
everything past `B` suggests it's the generic "treat as an ordinary
text character" case - which would mean **individual letters aren't
distinguished by this particular table at all** (every letter `C`
onward collapses to the same handler as `&`). If true, that means
actual keyword recognition (matching an accumulated run of letters
against `ID`/`SET`/`CURVE`/etc. from `STRINGS.md`'s length-prefixed
keyword table) must happen in a **different** function entirely - this
table is doing low-level lexical classification, not keyword
dispatch. That downstream keyword-matching function is the next
concrete thing to look for, not yet found.

## Traced the interrupt mask latch's real outputs - the "never unmasked" theory doesn't hold up either

Direct follow-up to the `TODO.md` lead about the Interrupt Mask Latch
(`0x406F8`-`0x406FB`, `U1236`) potentially never being unmasked for
the RS-232 port, which would explain the `DR` (byte-received)
interrupt never reaching the CPU. Prompted by the user asking to keep
looking, specifically in the **comm ROM** (not main ROM) for this.

**Found the indirection**: the comm ROM never loads the latch's
address via a literal immediate (confirmed by scanning the raw
`160-2998-14.bin` for every possible `mov reg, 0x6f8`-style opcode -
zero hits). Instead it goes through a RAM-resident far pointer,
**`[0x6E2]`** (already a known variable - `update_comm_tx_ready_flag`
and `set_comm_queue_busy` both use it, per `FUNCTIONS.md`) - and that
pointer's own initial value comes from the *same* comm-ROM `init_far_
pointer_table` re-decoded a few sections up: `dest_off=0x12` (physical
`0x8FEE2`) holds far pointer `0x406F:0x0008`, i.e. physical `0x406F8`
- the latch's base address, confirming `[0x6E2]` and the manual's
latch are the same hardware.

**Mapped all 4 outputs actually referenced in code**:
- **`0D`** (`es:[di]`, physical `0x406F8`): the **RX-ready flag** -
  `set_comm_queue_busy`'s critical-section engage/disengage directly
  sets this to `0` (masked) or `1` (unmasked). Given the manual says
  one of these 4 outputs masks "the RS-232-C port" interrupt, and this
  bit's behavior is a textbook interrupt-safe critical section (mask
  during a busy/nested section, restore on full unwind), **this is
  almost certainly the UART's `DR` interrupt enable/mask bit itself** -
  "rx-ready" isn't just a software flag, it's the actual hardware mask.
- **`1D`** (`es:[di+1]`, physical `0x406F9`): the **TX-ready flag**,
  same story for `TBRE` (transmit-buffer-empty) - set/cleared by
  `update_comm_tx_ready_flag` and the same `set_comm_queue_busy`.
- **`3D`** (`es:[di+3]`, physical `0x406FB`): used for two *different*
  purposes - `selftest_comm_readback`'s own self-test toggle (already
  documented), and `poll_dip_switch_change`'s bracketing strobe around
  2 reads of the GPIB DIP-switch byte (`[0x6DA]`) to detect changed
  bits. Neither looks like "interrupt masking" - more likely this is
  the manual's "diagnostics" output, repurposed here as a general
  strobe/self-test bit rather than a true interrupt mask.
- **`2D`** (physical `0x406FA`): **no reference found anywhere** in
  the comm ROM's code (proven or heuristic, direct or alias-decoded).
  Not resolved - possibly drives the `RLSD`/`DCD`-generation circuitry
  the manual separately mentions ("The RLSD signal is generated by
  Interrupt Mask Latch U1236"), which might only need setting once
  rather than being dynamically toggled, explaining why it's harder to
  find.

**RESOLVED 2026-09-16, from the user's own direct service-manual
schematic review** (`TODO.md`'s "Architect Notes" section - a
component-level trace of the RS-232 option board, not just the
prose/table excerpts used above). `U1235` is `U1236`'s actual
schematic reference on this trace (74HCT259, "8bit latch", labeled
directly as the Interrupt Mask Latch) with its 4 output pins mapped
individually:

| Latch output (this doc) | `U1235` pin (user's trace) | Confirmed role |
|---|---|---|
| `0D` | `Q0` | `DR + INTR` mask - **exact match**, independently confirmed twice now |
| `1D` | `Q1` | `TBRE` mask - **exact match**, independently confirmed twice now |
| `2D` | `Q2` | `RLSO` (buffered to the RS-232 DCE connector's pin 8, i.e. `RLSD`/`DCD`) - **this is the missing confirmation**, exactly matching this doc's own "possibly drives RLSD/DCD" guess above, now from primary-source schematic detail rather than inference |
| `3D` | `Q3` | `DIAG` - matches this doc's own "diagnostics output" conclusion |

All 4 outputs now agree between this project's independent code-trace
and the user's independent schematic trace, with zero contradictions -
a strong cross-check in both directions. `2D`/`RLSO` being a DCD-
generation line (not a UART interrupt mask at all) also explains why
no code was ever found toggling it dynamically: unlike `0D`/`1D`
(genuine interrupt enable/disable bits toggled per critical section),
a modem-control-signal driver bit is far more likely to be set once
(or left at its `BRST`-forced power-on default) than repeatedly
toggled - consistent with the "no reference found" result above being
a real absence, not a search failure.

**The "never unmasked" theory is weakened, not confirmed**: `set_comm_
queue_busy`'s disengage path (which restores `0D`=1, unmasking RX)
**is** called during normal comm-channel initialization - traced a
concrete call site (`0x8E584`, inside the same init sequence as
`init_comm_channel_state_a`/ring-buffer setup) that calls it with
`engage=0` directly. Assuming the nesting counter `[0x5A1]` starts at
`0` (a fresh boot) and no earlier code engages it without a matching
disengage, this call *should* leave RX genuinely unmasked by the end
of channel init. The masking *mechanism* is correctly implemented and
appears to be properly exercised - this doesn't look like a simple
"forgot to unmask" bug.

**Where this leaves the `ID?` puzzle**: narrowed further, but not
solved. The interrupt masking/unmasking code is real, mode-aware, and
looks correct on inspection. What's still open:
1. Whether this init sequence (and the cross-ROM `comm_rom_boot_init`/
   `reinit_comm_channel` chain leading to it) is **actually reached**
   on real hardware - ties back to the still-unresolved `[0x1B83]`/
   `[0x1BF9]` comm-detection questions. `COMM_LOOPBACK` showing
   `UNTESTED` rather than "Not installed" earlier this session is
   evidence `[0x1BF9]&1` gets set, which is a good sign this chain
   *does* run - but that's a different check than confirming this
   *specific* init function executes.
2. Whether the CPU's own interrupt infrastructure for this line is
   correctly wired at the 8088 level (IVT entry installed, maskable
   interrupt flag `IF` actually enabled after boot, etc.) - not
   checked this pass.
3. The keyword-matching function from the section above (still not
   found) - even a perfectly interrupt-driven UART needs something to
   actually recognize `ID?` once a byte does arrive.

**SUPERSEDED by a later finding this same session - see "MAJOR
CORRECTION: INT 255 is NOT 'a software-only vector'" further down.**
The speculation below (written before that finding) turned out to
have the right instinct but the wrong conclusion: `INT 255` *is* a
real hardware `INTR` vector after all (confirmed directly from the
manual's own text), and tracing where it actually leads uncovered the
real per-tick comm-status poller (`poll_comm_status_tick`,
`0x96F54`) - a much stronger candidate for "how incoming RS-232
activity gets serviced" than the unreachable `FUNC_2998_39F5` polling
loop this paragraph was originally worried about. Left in place below
for the historical reasoning trail.

**Follow-up check, and an important caveat**: went looking for whether
this system's CPU-level interrupt plumbing could even support a real
hardware `INTR`-pin interrupt for the UART (the manual says the
combined `DR+INTR`/`TBRE` signal drives "the microprocessor's maskable
interrupt", i.e. 8086 `INTR`, gated by the `IF` flag - a different
mechanism than `INT2`/NMI, which this project already knows drives the
scheduler tick and **cannot** be masked by `IF` at all). This project's
own interrupt-vector-table trace (see "Interrupt vector table entries"
above) only ever found vectors installed for **INT1 (trap), INT2
(NMI), and INT255 (software)** - no vector for a hardware `INTR`-style
interrupt has ever turned up, despite that search being run to
exhaustion previously. `sti` (re-enable maskable interrupts) does
appear a few times in the **main ROM** but **never once in the comm
ROM** itself. Taken together, this raises a real possibility that this
firmware's comm-option handling is **pure cooperative polling**, not
actually hardware-interrupt-driven at the CPU level - the Interrupt Mask
Latch's "ready" bits might just be a software-convention flag pair
(gating the polling loop's own logic) rather than something that ever
triggers a real 8086 `INTR` service routine in practice.

**However, this can't be confirmed either, because the polling loop
itself (`FUNC_2998_39F5`, containing the `L_83A04` loop that eventually
reaches `process_gpib_command_byte`) has no confirmed caller** - not a
literal `lcall` target anywhere in the reachable disassembly, and not
even a raw pointer to its offset (`0x39F5`) found anywhere in the whole
`160-2998-14.bin` binary (checked directly, byte-for-byte). This is an
important caveat on everything traced through this function this
session: **its reachability in real, running firmware is not
established** - it could be dead/heuristic-only code, or reached via a
near/relative call this project's tooling doesn't track the same way,
or reached via a genuinely different mechanism (a function-pointer
table entry, perhaps `create_task`-style) not yet found. Worth checking
directly next time - either find what actually starts this loop, or
find the *real* comm-task entry point if this isn't it.
