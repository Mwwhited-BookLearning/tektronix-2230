"""
Synthetic scheduler-tick timer - milestone 3 of emulator/docs/design.md.

The tick-driven task scheduler (docs/interrupts/task-scheduler.md,
docs/interrupts/ivt-and-int255.md) needs INT 2 to actually fire
periodically or nothing beyond early boot ever runs -
wait_readout_tick (physical 0xE0AE9, confirmed by the milestone-1 boot
trace hanging there) busy-waits on [0x752], which only INT2_HANDLER_
EARLY/_LATE increment, and only an INT 2 firing invokes either.

Fires by hand-simulating real 8086 INT delivery (push FLAGS/CS/IP,
clear IF/TF, jump to the installed IVT vector) from inside a
UC_HOOK_CODE callback - modifying CS/IP there redirects Unicorn's next
fetch before the interrupted instruction executes, which is exactly
"interrupt happens between instructions." The handler's own IRET
restores the original FLAGS/CS/IP when done, so execution resumes
exactly where it was preempted, standard real-mode semantics.
"""
from unicorn import x86_const as x86

IF_FLAG = 0x200
TF_FLAG = 0x100


def fire_interrupt(emu, int_num, respect_if=True):
    """Return False without firing if IF is clear and `respect_if` -
    real hardware wouldn't deliver a maskable INTR in that state.

    INT 2 is special: on the real 8086, interrupt type 2 is hardwired
    to the NMI pin (confirmed in docs/interrupts/ivt-and-int255.md -
    the service manual states "the NMI vector is at 00008", physical
    0x008 = IVT slot 2), which by definition is non-maskable and
    ignores IF - so the scheduler tick must be fired with
    respect_if=False, or it silently never fires before the firmware's
    first `sti` (confirmed by testing: 10000/10000 ticks skipped as
    "masked" while still in early boot, before any `sti` executes)."""
    flags = emu.reg_read(x86.UC_X86_REG_EFLAGS)
    if respect_if and not (flags & IF_FLAG):
        return False

    # Real hardware has no such guard, but a genuinely unset IVT slot
    # (RAM still zeroed, this project's own reset code hasn't installed
    # the real handler yet - see ivt-and-int255.md, install happens "at
    # reset" but not in literally the first few instructions) means
    # 0000:0000 - jumping there corrupts the boot trace instead of
    # modeling anything real. Skip until a real vector exists.
    vector = emu.mem_read(int_num * 4, 4)
    new_ip = int.from_bytes(vector[0:2], "little")
    new_cs = int.from_bytes(vector[2:4], "little")
    if new_cs == 0 and new_ip == 0:
        return False

    sp = emu.reg_read(x86.UC_X86_REG_SP)
    ss = emu.reg_read(x86.UC_X86_REG_SS)
    cs = emu.reg_read(x86.UC_X86_REG_CS)
    ip = emu.reg_read(x86.UC_X86_REG_IP)

    for value in (flags, cs, ip):
        sp = (sp - 2) & 0xFFFF
        emu.mem_write(ss * 16 + sp, value.to_bytes(2, "little"))
    emu.reg_write(x86.UC_X86_REG_SP, sp)
    emu.reg_write(x86.UC_X86_REG_CS, new_cs)
    emu.reg_write(x86.UC_X86_REG_IP, new_ip)
    emu.reg_write(x86.UC_X86_REG_EFLAGS, flags & ~(IF_FLAG | TF_FLAG))
    return True


# INT2_HANDLER_EARLY's confirmed installed address (docs/interrupts/
# ivt-and-int255.md: "0x008 (INT 2, NMI) | E5D1:0057 | at reset"), as
# little-endian IP-then-CS bytes exactly as the firmware's own `mov
# word [es:bx], ...` install writes them. Used to gate the very first
# synthetic tick on real installation, not just "IVT slot 0x008 is
# nonzero" - the boot-time RAM-clear/pattern-test loop (physical
# 0xE0161, see docs/design.md's milestone-1 trace notes) transiently
# writes non-zero test patterns across low memory, IVT included,
# before the real vector is installed; a naive nonzero check fires on
# one of those and jumps to garbage (confirmed by testing: crashed at
# CS:IP F2F3:F4F5, an obvious pattern-fill value, not real code).
INT2_EARLY_HANDLER_BYTES = bytes([0x57, 0x00, 0xD1, 0xE5])  # IP=0057, CS=E5D1


class TickScheduler:
    """Wire into a UC_HOOK_CODE hook: call `.step()` every instruction.
    Fires INT 2 every `interval` instructions (instruction-counted, not
    wall-clock - determinism over realism, per docs/design.md).

    Waits for the confirmed real INT2 vector to actually be installed
    (see INT2_EARLY_HANDLER_BYTES) before firing at all - see the note
    there for why a simpler "is it nonzero yet" check doesn't work."""

    def __init__(self, interval=2000, int_num=2):
        self.interval = interval
        self.int_num = int_num
        self.instructions = 0
        self.fired = 0
        self.skipped = 0
        self.vector_confirmed = False

    def step(self, emu):
        self.instructions += 1
        if not self.vector_confirmed:
            if emu.mem_read(self.int_num * 4, 4) == INT2_EARLY_HANDLER_BYTES:
                self.vector_confirmed = True
            else:
                return
        if self.instructions % self.interval == 0:
            respect_if = self.int_num != 2  # INT 2 = NMI, never masked
            if fire_interrupt(emu, self.int_num, respect_if=respect_if):
                self.fired += 1
            else:
                self.skipped += 1
