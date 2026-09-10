#!/usr/bin/env python
"""
Recursive-descent disassembler for the Tektronix 2230 main system ROM pair.

Confirmed facts (see NOTES.md):
  - CPU: Intel 8088/8086, 16-bit real mode.
  - 160-3633-14.bin (silkscreen "sys_rom_0", socket U9109) is mapped at
    physical 0xE0000-0xEFFFF.
  - 160-3532-14.bin (silkscreen "sys_rom_1", socket U9110) is mapped at
    physical 0xF0000-0xFFFFF and contains the CPU reset vector at FFFF0
    (a far JMP into the 3633 half).
  - Together they form one 128KB address space for the acquisition/
    display CPU. The comm/GPIB ROM (160-2998) is a separate, still-
    unmapped piece (bank-switched option-board ROM) and is NOT handled
    by this script yet.

Approach: recursive descent from known entry points, tracking (segment,
offset) so near vs far transfers are handled correctly, instead of a
blind linear sweep. This only disassembles code actually reachable from
a real entry point, which is far more trustworthy than sweeping every
byte. Anything not reached is left as unclassified data for a later pass.
"""
import json
import re
import capstone as cs

FAR_TARGET_RE = re.compile(r"^\s*(?:0x)?([0-9a-fA-F]+)\s*[:,]\s*(?:0x)?([0-9a-fA-F]+)\s*$")

CHIPS = {
    "3633": {"path": "../binary/160-3633-14.bin", "phys_base": 0xE0000},
    "3532": {"path": "../binary/160-3532-14.bin", "phys_base": 0xF0000},
    # Not bank-switched after all - confirmed by checking every far-call
    # target landing in 0x80000-0x8FFFF against this file's own function
    # starts: 82/84 land exactly on one. This is a plain 64KB device at
    # a fixed physical address, the same simple pattern as the two main-
    # ROM halves above (there was no need to bank-switch: the 8088's 1MB
    # address space has plenty of room free). See NOTES.md "The comm ROM
    # is NOT bank-switched".
    "2998": {"path": "../binary/160-2998-14.bin", "phys_base": 0x80000},
    # 0x90000-0x97FFF is an address-decode ALIAS of the comm ROM's own
    # upper half (0x88000-0x8FFFF, file offset 0x8000-0xFFFF) - not a
    # separate device. Confirmed by brute-forcing every possible base
    # offset against 82 observed far-call targets in that range: only
    # base=0x88000 (equivalently, this alias's own base of 0x90000,
    # pointing at file offset 0x8000+) gives 82/82 exact matches against
    # the comm ROM's own function-start signatures. Almost certainly
    # incomplete address-line decoding in the chip-select logic. See
    # NOTES.md "The 0x90000-0x97FFF region is fully resolved".
    "2998_alias_90000": {"path": "../binary/160-2998-14.bin",
                          "phys_base": 0x90000, "slice": (0x8000, 0x10000)},
}

ENTRY_POINTS = [
    # (segment, offset, label)
    (0xF000, 0xFFF0, "RESET"),
    (0xE5D1, 0x00B4, "ENTRY_E5D1_B4"),
    (0xE5D1, 0x00C7, "ENTRY_E5D1_C7"),
    # Found by tracing interrupt-vector-table writes (mov word [es:bx],
    # imm with es=0 or es=0x3f, i.e. real IVT slots) in the already-
    # decoded code: these handler addresses are never reached by any
    # direct call/jmp in the program, only by the corresponding
    # hardware/software interrupt actually firing.
    (0xE5D1, 0x019D, "INT1_HANDLER"),           # INT 1 (single-step/trap)
    (0xE5D1, 0x0090, "INT255_HANDLER_EARLY"),   # INT 255, installed at reset
    (0xE60B, 0x0005, "INT255_HANDLER_LATE"),    # INT 255, reinstalled later
    (0xE60B, 0x003A, "INT2_HANDLER_LATE"),      # INT 2 (NMI), reinstalled later
    # Both 16KB pages at file offset 0x8000 and 0xC000 in the comm/GPIB
    # ROM (160-2998) open with an identical, deliberate far jump here
    # (`EA 00 00 4C E6` = `ljmp 0xE64C:0`) - a directly-observed, high-
    # confidence entry point (unlike the comm ROM's OWN disassembly,
    # which is seeded heuristically - see gen_disasm_2998.py).
    (0xE64C, 0x0000, "COMM_ROM_BOOTSTUB_TARGET"),
]

# Semantic names for routines/branch targets whose purpose has been
# understood well enough to name (see FUNCTIONS.md/VARIABLES.md for
# the evidence behind each one - keep the two in sync). Keyed by
# physical address; render() prefers these over the address-based
# SUB_XXXXX/L_XXXXX placeholder. Unlike ENTRY_POINTS, these aren't
# necessarily entry points themselves - just names for labels the
# recursive descent already finds on its own.
FUNCTIONAL_NAMES = {
    0xE00B1: "boot_init",                    # RESET's target: cli, clear
                                              # RAM, first stack setup
    0xE416F: "print_selftest_banner",        # CORRECTED (was misnamed
                                              # self_test_dispatcher): only
                                              # prints the "before"/"after"
                                              # banner lines and sets
                                              # [0x1B10]=3 - no OR-fold test
                                              # calls at all. See NOTES.md
                                              # "self_test_dispatcher was
                                              # misnamed"
    0xE4244: "self_test_dispatcher",         # the REAL dispatcher: ~14
                                              # calls to per-subsystem test
                                              # routines, each OR-folded into
                                              # [bp-0xA] - see NOTES.md
                                              # "Found: the self-test
                                              # dispatcher"
    0xE44F1: "check_comm_option_installed",  # ROM-header-checksum +
                                              # RAM/IO presence probe -
                                              # see NOTES.md "Found: the
                                              # option-board presence/
                                              # RAM-detection routine"
    0xE0AF5: "print_string_far",             # loops a far-pointer
                                              # nul-terminated string,
                                              # print_char per byte - see
                                              # NOTES.md "The readout/CRT
                                              # display memory"
    0xE0B2A: "print_char",                   # thin wrapper: unpacks one
                                              # byte arg, calls
                                              # write_readout_port_byte
    0xE0B50: "write_readout_port_byte",      # writes the passed byte to
                                              # the fixed physical address
                                              # 0x40000+0x6F0 - see NOTES.md
                                              # "The readout/CRT display
                                              # memory"
    0xE39F0: "append_readout_char",          # appends a (char, attribute)
                                              # byte pair into the readout
                                              # line buffer at [0x1AF4],
                                              # duplicating into a second
                                              # plane 0x8000 higher in the
                                              # same segment - see NOTES.md
    0xFBC09: "memcpy_far",                   # generic far-pointer block
                                              # copy: (dest, src, len)

    # --- Renamed during the "rename everything" pass (session:
    # keep going until everything is renamed) - see NOTES.md "The
    # readout vector display list" and FUNCTIONS.md for evidence. ---
    0x83241: "putchar_serial",                # thin wrapper: stores the
                                               # byte argument at local
                                               # [5], calls serial_tx_
                                               # buffer_put
    0x83259: "putchar_serial_seg",            # same as putchar_serial
                                               # but swaps DS to a fixed
                                               # segment (0x8F80) first
                                               # via set_ds_return_old
    0x839D1: "comm_call_main_rom",            # swaps DS to the main
                                               # ROM's low-RAM segment
                                               # (0x41, via set_ds_
                                               # return_old), calls a
                                               # main-ROM handler
                                               # (SUB_ED7DF), swaps back -
                                               # a cross-ROM callback
                                               # wrapper, same pattern as
                                               # set_comm_critical_flag
    0x9605A: "checksum_bytes",                # (far ptr, count) - sums
                                               # `count` bytes starting
                                               # at the pointer into a
                                               # byte accumulator
                                               # (wrapping), returns it -
                                               # a plain byte checksum
    0x96087: "memset_far",                    # (far ptr, fill_byte,
                                               # count) - writes
                                               # fill_byte to `count`
                                               # consecutive bytes
    0xF9FEE: "clear_channel2_status",         # writes 0 to physical
                                               # 0x42000 - the SAME
                                               # fixed address
                                               # read_channel2_status
                                               # reads, confirming it's
                                               # a real read/write
                                               # hardware register, not
                                               # just a coincidental ROM
                                               # address
    0xFA00C: "clear_channel1_status",         # writes 0 to physical
                                               # 0x41000 - see
                                               # clear_channel2_status
    0x9470E: "set_ds_return_old",             # push ds; mov ds,[bp+6];
                                               # pop ax - swaps DS to the
                                               # caller-given segment,
                                               # returns the OLD ds in ax
                                               # so the caller can restore
                                               # it later with a 2nd call
    0xE31DC: "strncat_far",                   # (dest,src far ptrs, max
                                               # len) - finds dest's NUL,
                                               # appends src up to the
                                               # limit, re-terminates
    0xE323F: "strncpy_far",                   # (dest,src far ptrs, max
                                               # len) - bounded copy from
                                               # src start (no dest-end
                                               # search, unlike strncat)
    0xE327F: "format_number",                 # (value, radix, width,
                                               # overflow-flag, ...) ->
                                               # far ptr to ASCII digits
                                               # built backwards into the
                                               # fixed scratch buffer at
                                               # [0x1B34]; sign/space/'*'-
                                               # overflow prefix handling
    0xE3372: "format_hex_word",               # format_number wrapper:
                                               # radix=16, width=5
    0xE3395: "format_decimal_word",           # format_number wrapper:
                                               # radix=10, width=6
    0xE33B8: "format_word_radix",             # format_number wrapper:
                                               # caller-supplied radix,
                                               # width=5
    0xE34CB: "format_byte_hex",               # byte -> 2 hex ASCII
                                               # digits + NUL, written to
                                               # the fixed scratch buffer
                                               # at [0x1B4A]; simpler/
                                               # separate from
                                               # format_number
    0xE35EF: "init_print_record",             # sets a print record's
                                               # attribute byte + position
                                               # (defaults to the current
                                               # global cursor [0x1AF4] if
                                               # no override given, else
                                               # moves the global cursor)
    0xE3662: "pack_row_col_bits",              # bit-packs two 5-bit
                                               # fields into a print
                                               # record's bytes 1 & 3,
                                               # preserving each byte's
                                               # high 3 bits
    0xE36A5: "set_position_record",           # encodes two coordinates
                                               # (>>3, character-cell
                                               # granularity) into a print
                                               # record's bytes 1-4
    0xE3520: "build_print_record",            # orchestrates
                                               # init_print_record ->
                                               # set_position_record ->
                                               # pack_row_col_bits into
                                               # one combined record
    0xE3567: "init_print_region",             # build_print_record
                                               # wrapper with default
                                               # 0x10x0x10 cell size, no
                                               # position override - the
                                               # common case
    0xE3766: "mark_readout_delimiter",        # writes attribute=2 (the
                                               # same code E3930 uses as
                                               # its wraparound marker)
                                               # into the next 2 readout
                                               # buffer cells' attribute
                                               # plane and advances the
                                               # cursor by 2
    0xE374E: "close_print_record",            # calls mark_readout_
                                               # delimiter, then tags the
                                               # caller's record[0] with
                                               # completion code 0x11 -
                                               # CORRECTED: earlier notes
                                               # wrongly assumed this
                                               # printed a string (it
                                               # doesn't traverse [bp+6]
                                               # at all, just tags it)
    0xE3736: "close_print_record_b",          # same shape as
                                               # close_print_record but
                                               # tags with 0x39 instead of
                                               # 0x11 - a different record
                                               # "kind"/completion code,
                                               # exact meaning of 0x11 vs
                                               # 0x39 not confirmed
    0xE3821: "print_readout_string",          # loops a far-pointer nul-
                                               # terminated string calling
                                               # draw_readout_char per
                                               # byte - the vector-display-
                                               # list sibling of
                                               # print_string_far (which
                                               # instead goes straight to
                                               # the 0x40000+0x6F0 port)
    0xE3854: "draw_readout_char",             # looks up a character's
                                               # stroke list in the font
                                               # table at [0x1DB0], walks
                                               # each stroke byte (pen-up/
                                               # down bit + packed coarse/
                                               # fine deltas), calls
                                               # plot_readout_point_
                                               # relative per stroke - the
                                               # CRT readout's character
                                               # rasterizer
    0xE3900: "plot_readout_point_relative",   # (dx,dy,attr) - adds the
                                               # offsets to the current
                                               # base position ([0x1AF8]/
                                               # [0x1AFA]) and calls
                                               # plot_readout_point
    0xE07B4: "print_selftest_report_line",    # the outer per-cycle
                                               # report-printing routine
                                               # that wraps
                                               # print_selftest_banner;
                                               # computes a row Y coord
                                               # from [0x1B10]*0x32 and
                                               # prints one report line
                                               # per call
    0xE0ADD: "wait_readout_tick",             # busy-waits for [0x752]
                                               # (a counter, presumably
                                               # incremented by an
                                               # interrupt handler) to
                                               # change value - throttles
                                               # print_string_far's per-
                                               # character output loop to
                                               # the readout hardware's
                                               # actual speed
    0xE0E56: "clear_selftest_status_flags",   # zeroes 4 fixed status
                                               # bytes ([0x256]-[0x259])
                                               # plus the byte at each of
                                               # 3 far pointers set up by
                                               # SUB_E4443 ([0x326],
                                               # [0x336], [0x33A])
    0xE0FD0: "selftest_measure_and_report",   # one of self_test_
                                               # dispatcher's ~14 test
                                               # calls (identified this
                                               # session): enable/run/
                                               # disable pattern via 3
                                               # calls to selftest_
                                               # measure_mode (idx 1,3,2)
    0xE0FF5: "selftest_measure_mode",         # idx==1: enable a measure
                                               # mode ([0x1B5E]=1);
                                               # idx==2: disable (the
                                               # complementary reset);
                                               # else (incl. idx==3):
                                               # run the actual
                                               # measurement (SUB_E296E +
                                               # SUB_E0C3D) and return its
                                               # result code in ax
    0x96B68: "serial_tx_buffer_put",          # comm ROM: writes a byte
                                               # into a ring buffer at
                                               # [0x44C] with wraparound
                                               # at a fixed boundary
                                               # (0x433 bytes); byte
                                               # source depends on mode
                                               # flags [0x459]/[0x629] -
                                               # likely the GPIB/RS-232
                                               # transmit buffer
    0x97B94: "set_comm_critical_flag",        # comm ROM: sets critical-
                                               # section flag [0x5A3] to
                                               # the given value; on the
                                               # "leaving" edge (new
                                               # value 0, old value
                                               # nonzero, [0x5A1] set),
                                               # swaps DS to the main
                                               # ROM's low-RAM segment
                                               # (0x41, via set_ds_
                                               # return_old) and calls a
                                               # main-ROM routine before
                                               # swapping back - a cross-
                                               # ROM notify-on-unlock
                                               # pattern
    0xF0078: "divide_scale_default",          # (value at [bp+8]) - pre-
                                               # loads dx:ax from the
                                               # global default divisor
                                               # at [0x6D2] then falls
                                               # into divide_scale;
                                               # shares its body with a
                                               # 2nd, explicit-divisor
                                               # entry point at 0xF0086
    0xF0086: "divide_scale",                  # (dividend dx:ax, divisor
                                               # [bp+8]) - 32-bit divide
                                               # (SUB_E777D) + scale
                                               # (SUB_E7764) + a 3rd call
                                               # (SUB_E7F39) updating
                                               # [0x6E6]/[0x6E8]; called
                                               # directly or via divide_
                                               # scale_default above
    0xF03F4: "reset_acq_buffers_stub",        # tiny stub (one
                                               # computation then a
                                               # shared-tail jump) that
                                               # reaches the acquisition-
                                               # buffer-size init block
                                               # (8 buffers set to 0x800
                                               # = 2048 bytes each - a
                                               # plausible DSO record
                                               # length) - see NOTES.md
                                               # "Possible waveform
                                               # acquisition buffer init"
    0xF0414: "print_and_reset_acq_buffers",   # prints a status string
                                               # (0xFF7B:0x362) and does
                                               # a scale computation,
                                               # then falls through into
                                               # the same shared
                                               # acquisition-buffer-init
                                               # tail as reset_acq_
                                               # buffers_stub
    0xE4429: "read_channel1_status",          # reads one fixed byte
                                               # from physical 0x41000 -
                                               # called alternately with
                                               # read_channel2_status in
                                               # a self-test loop that
                                               # takes repeated readings;
                                               # "channel1" is inferred
                                               # from the 2-channel-scope
                                               # context, not confirmed
    0xE440A: "read_channel2_status",          # reads one fixed byte
                                               # from physical 0x42000 -
                                               # see read_channel1_status
    0xE5B34: "selftest_display_result_mode",  # idx==1/2 toggle the SAME
                                               # [0x1B5E] flag selftest_
                                               # measure_mode uses
                                               # (shared "measurement
                                               # active" flag?); idx==3/4
                                               # fall through to
                                               # positioning + printing a
                                               # result via
                                               # set_position_record/
                                               # SUB_E2DC9
    0xE06B6: "update_menu_position",          # (min, max, op_nibble) -
                                               # bounded index tracker
                                               # with clamping/wraparound
                                               # at [0x1B50]; op codes
                                               # select init/cancel/
                                               # increment variants,
                                               # reads two front-panel
                                               # button-state bytes
                                               # ([0x4E7]/[0x4E8], bit 7)
                                               # for a "both held"
                                               # accelerate case - a
                                               # menu-navigation cursor
    0xE2AB0: "selftest_init_channel_hw",      # calls clear_selftest_
                                               # status_flags and
                                               # read_channel1_status,
                                               # then writes a short
                                               # sequence of command
                                               # codes (0x1D, 9, 0x1D)
                                               # combined with the
                                               # caller's channel-select
                                               # bits to a fixed far-
                                               # pointer register at
                                               # [0x1D20] - looks like a
                                               # front-end/ADC init
                                               # sequence, not confirmed
    0xE5D2D: "enable_interrupts",             # sti; retf - one
                                               # instruction, trivial
    0xE5D2F: "disable_interrupts",            # cli; retf - one
                                               # instruction, trivial
    0xE3599: "init_print_region_home",        # build_print_record
                                               # wrapper: fixed 0x10x0x10
                                               # cell size, position
                                               # override = [0x45E] (a
                                               # saved copy of the
                                               # readout buffer's own
                                               # base pointer, [0x1CC4])
                                               # - starts a print record
                                               # at the readout "home"
                                               # position
    0xE35C2: "build_print_region",            # build_print_record
                                               # wrapper passing all 4
                                               # position/size args
                                               # through unchanged - the
                                               # general form behind
                                               # init_print_region's
                                               # fixed-size convenience
                                               # wrapper; used by
                                               # print_selftest_report_
                                               # line for its computed
                                               # per-row position
    0xE7911: "format_string_va",              # walks a format string
                                               # looking for '%' (0x25),
                                               # dispatching per-
                                               # specifier handlers that
                                               # pull the next vararg
                                               # off the stack (tracked
                                               # via a caller-stack-
                                               # relative far pointer at
                                               # [bp+0xa]) - the core
                                               # printf-style variadic
                                               # formatting engine behind
                                               # this firmware's %d/%c
                                               # format strings (see
                                               # STRINGS.md); dispatches
                                               # to format_number-style
                                               # helpers per specifier
    0xE591D: "print_report_frame_mode",       # idx-based dispatcher,
                                               # same shape as
                                               # selftest_display_
                                               # result_mode: idx==1
                                               # resets; idx==4 draws a
                                               # bordered frame (two
                                               # print regions + a
                                               # 150-point line via
                                               # plot_readout_point_
                                               # scaled) - part of the
                                               # report-display state
                                               # machine
    0xFDB3E: "clear_measurement_mode_bit",    # clears bit 0x10 of
                                               # [0x258] and commits it
                                               # to the shared hardware
                                               # register cluster
                                               # ([0x336]) - part of the
                                               # configure_measurement_hw
                                               # family
    0xE553B: "report_screen_mode",            # idx-based dispatcher for
                                               # the self-test report
                                               # screen: idx==1 resets;
                                               # idx==2 initializes 8
                                               # print regions in a loop
                                               # (init_print_region_home
                                               # at 9-byte spacing - the
                                               # report table's 8 rows);
                                               # idx==4 sets up the
                                               # title/frame region
    0xE693C: "restart_current_task",          # if [0x1ACD] (current
                                               # task index) is nonzero,
                                               # sets that task's
                                               # [+0x744] flag and calls
                                               # create_task again - a
                                               # task-restart/respawn
                                               # trigger, part of the
                                               # task scheduler
    0xE6A8E: "mark_task_ready",               # (task idx) - clears
                                               # that task's [+0x744]
                                               # flag, increments its
                                               # ready-flags byte
                                               # ([idx+0x1A91]), then
                                               # calls SUB_E61E3 - likely
                                               # a "wake up task" primitive
    0xFDC73: "sync_status_byte_to_hw",        # writes [0x258] into the
                                               # far-pointer hardware
                                               # register [0x336] -
                                               # called from switch_to_
                                               # next_task, so likely a
                                               # general "flush pending
                                               # status" step, not
                                               # measurement-specific
    0xE58AD: "draw_readout_line",             # (x1, y1, x_max, y_max,
                                               # dx, dy) - steps from
                                               # (x1,y1) toward (x_max,
                                               # y_max) by (dx,dy),
                                               # calling plot_readout_
                                               # point_scaled per step -
                                               # a line-drawing primitive
                                               # for the readout vector
                                               # display
    0xE5D31: "delay_read_128w",               # cld; ds:si=0:0; rep
                                               # lodsw x0x80 (128 words)
                                               # - reads through 256
                                               # bytes of low memory
                                               # without using the
                                               # result; called right
                                               # before install_late_
                                               # interrupt_vectors -
                                               # likely a deliberate
                                               # timing delay (hardware
                                               # settling) rather than a
                                               # memory test, given
                                               # sibling routines use
                                               # different word counts
                                               # for what look like
                                               # different delay lengths
    0xEE13B: "write_hw_shift_register",       # writes ax to port 0xD1
                                               # three times (each
                                               # preceded by shl di,1)
                                               # then to port 0xC4 once
                                               # - a serial shift-
                                               # register-style hardware
                                               # write (0xD1=data/clock,
                                               # 0xC4=latch/strobe, exact
                                               # roles not confirmed).
                                               # THE function behind
                                               # MEMORY_MAP.md's
                                               # previously-mysterious
                                               # I/O ports 0xD1/0xC4 -
                                               # see NOTES.md "Found:
                                               # the hardware shift-
                                               # register write"
    0xE260A: "verify_timebase_prc",           # configures the shared
                                               # measurement hardware
                                               # register cluster
                                               # ([0x32E]/[0x336]/
                                               # [0x33A]), reads back a
                                               # 12-bit value from
                                               # [0x332], compares
                                               # against an expected
                                               # value, and reports
                                               # "PRC ... read-back
                                               # <hex>" on mismatch -
                                               # references sweep-speed
                                               # divider names (hs/2,
                                               # ps/2...) suggesting PRC
                                               # is a timebase/reference-
                                               # clock counter
    0xF0C81: "reset_plot_home_or_acq",        # checks [0x6C0] bit 0x80:
                                               # if set, calls update_
                                               # plot_position(0,0) - a
                                               # "home the pen" reset;
                                               # if clear, falls through
                                               # to a shared acquisition-
                                               # buffer-reset tail
                                               # (matching reset_acq_
                                               # buffers_stub's target)
    0xF706E: "update_display_mode_flags",     # packs several bit-flags
                                               # from a caller-given word
                                               # into a combined status
                                               # byte, stores it to
                                               # [0x1B72] if changed
                                               # (setting a "changed"
                                               # flag at [0x53A]), then
                                               # does mode-specific
                                               # follow-up based on
                                               # [0x4FC]
    0xE0BA3: "print_scratch_buffer_range",    # opens a print region
                                               # (build_print_region),
                                               # then loops appending
                                               # bytes read from the
                                               # [0x31E]-based (physical
                                               # 0x48000) scratch buffer
                                               # to the readout via
                                               # append_readout_char,
                                               # closes with close_print_
                                               # record_b - prints a
                                               # range of the
                                               # acquisition/diagnostic
                                               # scratch buffer to the
                                               # readout display
    0xE1116: "verify_pattern_with_report",    # compares a byte from the
                                               # [0x31E]-based (physical
                                               # 0x48000) scratch buffer
                                               # against an expected
                                               # value in a loop; on
                                               # mismatch, builds a
                                               # detailed error message
                                               # ("latent"/"END_OF_
                                               # RECORD"-area text +
                                               # offset + [0x795]'s
                                               # value) rather than a
                                               # plain pass/fail - a
                                               # data-integrity check
                                               # with rich diagnostics,
                                               # likely acquisition-
                                               # memory-related given
                                               # neighboring strings
                                               # ("acq_mem")
    0xE40CE: "print_boot_rom_id_banner",      # gated on [0x1B48]!=0 (the
                                               # test-mode byte); prints
                                               # "2230/2220 boot : 160-"
                                               # plus ROM ID/revision
                                               # bytes from [0x1DD4]+2/
                                               # +3/+4 (via format_byte_
                                               # hex) and "POWER UP
                                               # FAILURES"-area text, all
                                               # via print_string_far (to
                                               # the CRT readout, not a
                                               # separate serial port)
    0xE43A0: "init_selftest_report_screen",   # saves the readout
                                               # buffer's base pointer
                                               # ([0x1CC4]) to [0x45E]/
                                               # [0x460] (used by
                                               # init_print_region_home's
                                               # default position),
                                               # calls report_screen_mode
                                               # with idx 1 then 4
                                               # (reset, then init title/
                                               # frame), sets a flag bit,
                                               # and stores the report
                                               # area's linear address
                                               # via seg_off_to_linear.
                                               # Called from
                                               # run_selftest_sequence
    0xE3B12: "run_selftest_sequence",         # THE top-level self-test
                                               # orchestrator: writes a
                                               # marker byte at physical
                                               # 0x00000, initializes the
                                               # readout display-list
                                               # buffer, calls SUB_E4443/
                                               # SUB_E75C0/SUB_E128D
                                               # setup, decides a test-
                                               # mode byte [0x1B48] from
                                               # [0x758], calls
                                               # init_selftest_report_
                                               # screen (SUB_E43A0), then
                                               # print_selftest_report_
                                               # line ONCE followed
                                               # immediately by
                                               # self_test_dispatcher -
                                               # ties together every
                                               # self-test finding from
                                               # this whole investigation
    0xE7E0D: "plot_line_to",                  # HPGL "PD%d,%d;" (pen-
                                               # down draw) counterpart
                                               # to update_plot_position's
                                               # "PU" (pen-up move)
    0xE30B5: "verify_adc_calibration",        # compares the 12-bit
                                               # value field of status
                                               # register [0x322]
                                               # against a caller-given
                                               # expected value; on
                                               # mismatch, builds an
                                               # "ACQ_AB ... uncaled :
                                               # min=<hex>" error message
    0xE7D7D: "update_plot_position",          # (x, y) - mode-dispatched
                                               # on [0x6CA]: mode 0
                                               # computes a position via
                                               # SUB_E80E4; mode 1 emits
                                               # the HPGL command
                                               # "PU%d,%d;" (pen-up
                                               # move) via format_
                                               # string_va and stores
                                               # the position; modes 2/3
                                               # store directly. Part of
                                               # the HPGL plotter output
                                               # driver - see MEMORY_MAP.md
                                               # "I/O ports" (port 0x83)
    0xE5E53: "install_late_interrupt_vectors", # installs INT255_HANDLER_
                                               # LATE (0x3FC) and INT2_
                                               # HANDLER_LATE (0x008),
                                               # both pointing into
                                               # segment 0xE60B - matches
                                               # the "late" IVT writes
                                               # documented in NOTES.md
                                               # "Interrupt vector table
                                               # entries"
    0xFBC2F: "array_index_16",                # (base far ptr, index) ->
                                               # base + index*16 - 16-
                                               # byte-record array
                                               # indexing, used by the
                                               # F8272 record-builder
                                               # cluster (160-3532's
                                               # parallel to build_
                                               # print_record)
    0xFBC4D: "copy_word_far",                 # copies one word from a
                                               # far source ptr to a far
                                               # dest ptr
    0xFBC69: "pack_low5_bits",                # packs the low 5 bits of
                                               # a value into a record
                                               # byte, preserving its
                                               # high 3 bits - same
                                               # pattern as
                                               # pack_row_col_bits
                                               # (0xE3662) but a separate
                                               # implementation in
                                               # 160-3532
    0xF8272: "build_print_record_3532",       # takes many params and
                                               # packs them into a
                                               # record via array_index_
                                               # 16/pack_low5_bits/
                                               # set_position_record_
                                               # 3532/copy_word_far -
                                               # 160-3532's parallel to
                                               # build_print_record
    0xFBC84: "set_position_record_3532",      # value>>3 (character-
                                               # cell scaling) written
                                               # across 2 record bytes -
                                               # same pattern as
                                               # set_position_record
                                               # (0xE36A5) but in
                                               # 160-3532
    0xE0C3D: "format_selftest_result_string", # (status_bits) - builds a
                                               # PASSED/FAILED/UNTESTED/
                                               # "Not installed" text
                                               # into buffer [0x476]
                                               # based on which bits are
                                               # set (0x20=not installed,
                                               # 0x02=failed, 0x01=passed,
                                               # else untested); returns
                                               # status_bits unchanged.
                                               # THE missing link for how
                                               # self-test results become
                                               # printed text
    0xE296E: "selftest_front_panel_adc",      # references "FP_a2d"/
                                               # "gnd ="/"TIME-OUT" -
                                               # tests the front-panel
                                               # A/D converter; called
                                               # from selftest_measure_
                                               # mode's "run" case (idx
                                               # 3), whose result feeds
                                               # format_selftest_result_
                                               # string
    0xE20B0: "selftest_comm_readback",        # references "COMM_RB"/
                                               # "rb(1)="/"rb(0)=" -
                                               # called (as the 2nd
                                               # phase) by selftest_comm_
                                               # loopback_a. Reads/writes
                                               # physical 0x40000+0x67C/
                                               # 0x6F8 (the readout
                                               # memory window, not the
                                               # comm ROM's own 0x80000
                                               # address) - this address
                                               # choice is surprising and
                                               # not yet reconciled, see
                                               # NOTES.md
    0xE1B89: "ram_pattern_test",              # (start far ptr, end far
                                               # ptr, step, mask) -
                                               # generic RAM test engine:
                                               # writes an alternating
                                               # 0xAA/0x55 pattern across
                                               # the range, then reads
                                               # back and compares
                                               # (masked) - the shared
                                               # implementation behind
                                               # the SYS_RAM/NIB_RAM/
                                               # ACQ_RAM/COMM_RAM self-
                                               # tests
    # --- self_test_dispatcher's sibling subroutines, identified by
    # cross-referencing the diagnostic-message strings each one
    # actually references against STRINGS.md's known self-test labels
    # (HS_ACQ/MM_ACQ/XY_ACQ, ACQ_RAM, ROMS/MISMATCH, COMM_ROM/COMM_RAM/
    # CMOS, COMM_LB, CDT, front-panel controls) - see NOTES.md
    # "Identified self_test_dispatcher's sibling subroutines". ---
    0xE26D6: "selftest_mm_acq",               # references string
                                               # "MM_ACQ" (0xFF7B:0x626)
    0xE28FE: "selftest_hs_acq",               # references "HS_ACQ"
                                               # (0xFF7B:0x618)
    0xE286C: "selftest_xy_acq",               # references "XY_ACQ"
                                               # (0xFF7B:0x642)
    0xE1B16: "selftest_acq_ram",              # references "ACQ_RAM
                                               # even"/"ACQ_RAM odd"
                                               # (0xFF7B:0x748)
    0xE16EA: "selftest_rom_checksum",         # references "ROMS"/
                                               # "MISMATCH" (0xFF7B:0x772)
    0xE1E3E: "selftest_comm_rom",             # references "COMM_ROM"
                                               # (0xFF7B:0x6ae); checksums
                                               # both the comm ROM's real
                                               # address (0x80000) and its
                                               # 0x90000 alias range
    0xE1E90: "selftest_comm_ram",             # references "COMM_RAM"/
                                               # "CMOS NOT SUPPORTED"
                                               # (0xFF7B:0x6b7)
    0xE1F18: "selftest_cmos",                 # references "CMOS"/
                                               # "reformated"/"recovered"
                                               # (0xFF7B:0x6d4)
    0xE1D28: "selftest_comm_loopback_a",      # gated by
                                               # check_comm_installed_gate,
                                               # calls SUB_E20B0 which
                                               # references "COMM_LB"
    0xE1DB3: "selftest_comm_loopback_b",      # gated by
                                               # check_comm_installed_gate,
                                               # calls SUB_E1FBC which
                                               # references "COMM_LB"/
                                               # "FGET NOT SET"/"FGET NOT
                                               # CLEAR"
    0xE4571: "check_comm_installed_gate",     # checks [0x1BF9]&1 (set
                                               # by check_comm_option_
                                               # installed); if clear,
                                               # copies a "not installed"
                                               # string and returns 1
                                               # (skip), else returns 0
    0xE3F2C: "selftest_display_irq_idle",     # references "MI"/"line
                                               # stuck high"/"Display
                                               # controller" (0xFF7B:0x4bd)
                                               # - checks the display
                                               # controller's interrupt
                                               # line before any operation
    0xE3F99: "selftest_display_irq_active",   # references "Display
                                               # controller"/"TIMEOUT"/
                                               # "unable to reset"
                                               # (0xFF7B:0x4d1) - draws a
                                               # test shape via
                                               # plot_readout_point then
                                               # verifies the interrupt
                                               # fires
    0xE2CEC: "selftest_cursor_delta_time",    # wrapper; its sole
                                               # implementation
                                               # (measure_cursor_delta_
                                               # time) references "CDT"/
                                               # "PRE-DETRIG"/"TIME-OUT"
                                               # (0xFF7B:0x5a8)
    0xE2CFB: "measure_cursor_delta_time",     # implementation for
                                               # selftest_cursor_delta_
                                               # time - calls wait_
                                               # stable_measurement twice
    0xE227E: "selftest_front_panel_switch_a", # wrapper: scans
                                               # update_menu_position
                                               # over range 0-8 (9
                                               # positions) via its step
                                               # helper (0xE22AF)
    0xE2FC8: "selftest_front_panel_switch_b", # wrapper: scans
                                               # update_menu_position
                                               # over range 0-0x15 (21
                                               # positions) via its step
                                               # helper (0xE2FFC)
    0xE252A: "selftest_comm_option_switch",   # conditional on
                                               # [0x1B83]==0x1E (comm
                                               # option RAM/IO
                                               # confirmed); scans
                                               # update_menu_position
                                               # over range 0-0x18 (24
                                               # positions) via its step
                                               # helper (0xE255E) -
                                               # likely a comm-board-
                                               # specific switch (GPIB
                                               # address/baud rate?)
    0xE0DCC: "configure_measurement_hw",      # (5 params) writes them
                                               # into the shared
                                               # "hardware register"
                                               # cluster ([0x326],
                                               # [0x336], [0x33A],
                                               # [0x32E], plus a reverse-
                                               # indexed lookup into the
                                               # [0x31E]-based scratch
                                               # buffer) also used by
                                               # selftest_init_channel_hw/
                                               # clear_selftest_status_
                                               # flags - see NOTES.md
                                               # "Possible ADC/measurement
                                               # self-test hardware"
    0xE12F4: "run_adc_selftest",              # clears a lookup-table
                                               # range, calls configure_
                                               # measurement_hw, polls a
                                               # busy bit (0x8000) at
                                               # far-pointer status
                                               # register [0x322] with a
                                               # timeout, reads a 12-bit
                                               # result (mask 0xFFF),
                                               # compares to a reference,
                                               # prints pass/fail via
                                               # print_selftest_report_
                                               # line - looks like a
                                               # 12-bit-ADC-based
                                               # measurement self-test
    0xE2DC9: "wait_stable_measurement",       # waits for a byte at far
                                               # pointer [0x32A] to
                                               # stabilize across
                                               # consecutive reads
                                               # (timeout via [bp-0xC]),
                                               # then reads status bits
                                               # 0x2000/0x4000 from
                                               # [0x322] and reports an
                                               # error (format_hex_word +
                                               # print_selftest_report_
                                               # line) if the expected
                                               # bit isn't set
    0xE37B1: "plot_readout_point_scaled",     # divides both coordinate
                                               # args by a local count
                                               # (hardcoded to 1 in this
                                               # build - a no-op divide,
                                               # vestige of generic code
                                               # supporting a count > 1)
                                               # and calls
                                               # plot_readout_point_
                                               # relative
    0xE6224: "create_task",                   # saves a full register
                                               # context onto a NEW
                                               # stack, stores that
                                               # stack's SP/SS into the
                                               # per-task table
                                               # [0x1A9D+idx*4] (the
                                               # SAME table switch_to_
                                               # next_task reads), saves
                                               # an entry-point far
                                               # pointer to [0x7C0]/
                                               # [0x7C2], sets the task's
                                               # ready flag, and jumps
                                               # directly into switch_to_
                                               # next_task - the "spawn a
                                               # task" counterpart to
                                               # the task scheduler found
                                               # last session; see
                                               # NOTES.md "A small task
                                               # scheduler"
    0xE6166: "switch_to_next_task",           # loads SP/SS from the
                                               # per-task context table
                                               # at [0x1A9D + idx*4]
                                               # (idx = [0x1ACD]) and
                                               # resumes it via iret -
                                               # the "switch in" half of
                                               # a small preemptive
                                               # task-switcher driven by
                                               # INT2_HANDLER_LATE - see
                                               # NOTES.md "A small task
                                               # scheduler"
    0xE6D2F: "seg_off_to_linear",             # (offset, segment) ->
                                               # offset + segment*16 -
                                               # the same seg:off -> flat
                                               # address math this
                                               # project's own tooling
                                               # uses, but as a compiled-
                                               # in runtime helper
    0xE4217: "print_banner_line",             # (far-ptr string) - calls
                                               # print_readout_string on
                                               # the given string, then
                                               # print_string_far twice
                                               # more on two FIXED strings
                                               # in the 0xFF7B string
                                               # table - used by
                                               # print_selftest_banner for
                                               # both its "before"/"after"
                                               # lines
    0xE3930: "plot_readout_point",            # (x,y,attr) - THE
                                               # fundamental primitive:
                                               # appends a (y,x) coordinate
                                               # pair + duplicated
                                               # attribute to the readout
                                               # vector display-list
                                               # buffer at [0x1CC4],
                                               # handles circular-buffer
                                               # wraparound (marker
                                               # attribute=2, matches
                                               # mark_readout_delimiter)
}

CALL_MNEMONICS = {"call", "lcall"}
JUMP_MNEMONICS = {"jmp", "ljmp"}
COND_JUMP_PREFIX = "j"  # je, jne, jg, jl, ... (capstone x86 conditional jumps)
RET_MNEMONICS = {"ret", "retf", "iret"}
STOP_MNEMONICS = {"hlt"} | RET_MNEMONICS


def load_chips(chip_defs=None):
    if chip_defs is None:
        chip_defs = CHIPS
    data = {}
    for name, info in chip_defs.items():
        buf = open(info["path"], "rb").read()
        if "slice" in info:
            lo, hi = info["slice"]
            buf = buf[lo:hi]
        data[name] = {"buf": buf, "base": info["phys_base"], "size": len(buf)}
    return data


def phys_to_chip_offset(chips, phys):
    for name, c in chips.items():
        if c["base"] <= phys < c["base"] + c["size"]:
            return name, phys - c["base"]
    return None, None


def main(chip_defs=None, entry_points=None):
    chips = load_chips(chip_defs)
    entry_points = ENTRY_POINTS if entry_points is None else entry_points
    md = cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_16)
    md.detail = False

    visited = {}   # phys addr -> instruction dict
    queue = []     # (seg, off) to process
    labels = {}    # phys addr -> {"name":..., "kind":..., "refs":[...]}

    def seg_off_to_phys(seg, off):
        return ((seg << 4) + off) & 0xFFFFF

    def add_label(phys, kind, from_phys):
        lab = labels.setdefault(phys, {"kind": kind, "refs": []})
        if kind == "sub" and lab["kind"] != "sub":
            lab["kind"] = "sub"
        lab["refs"].append(from_phys)

    for seg, off, name in entry_points:
        phys = seg_off_to_phys(seg, off)
        labels[phys] = {"kind": "entry", "refs": [], "fixed_name": name}
        queue.append((seg, off))

    while queue:
        seg, off = queue.pop()
        cs_val = seg  # current code segment for this walk

        while True:
            phys = seg_off_to_phys(cs_val, off)
            if phys in visited:
                break  # already decoded, converges into existing flow
            chip_name, chip_off = phys_to_chip_offset(chips, phys)
            if chip_name is None:
                break  # ran off into unmapped memory (RAM/IO) - stop this path
            buf = chips[chip_name]["buf"]
            chunk = buf[chip_off:chip_off + 16]
            insns = list(md.disasm(chunk, off))
            if not insns:
                break
            insn = insns[0]

            visited[phys] = {
                "phys": phys, "seg": cs_val, "off": off,
                "chip": chip_name, "chip_off": chip_off,
                "mnem": insn.mnemonic, "op": insn.op_str,
                "size": insn.size,
                "raw": buf[chip_off:chip_off + insn.size].hex(),
            }

            mnem = insn.mnemonic
            nxt_off = (off + insn.size) & 0xFFFF

            if mnem in ("ljmp", "lcall"):
                m = FAR_TARGET_RE.match(insn.op_str)
                if m:
                    tseg = int(m.group(1), 16)
                    toff = int(m.group(2), 16)
                    tphys = seg_off_to_phys(tseg, toff)
                    add_label(tphys, "sub" if mnem == "lcall" else "loc", phys)
                    queue.append((tseg, toff))
                    if mnem == "lcall":
                        off = nxt_off
                        continue
                    else:
                        break
                # unresolved indirect far jmp/call
                if mnem == "ljmp":
                    break
                off = nxt_off
                continue

            if mnem == "jmp":
                if insn.op_str.startswith("0x"):
                    toff = int(insn.op_str, 16)
                    tphys = seg_off_to_phys(cs_val, toff)
                    add_label(tphys, "loc", phys)
                    off = toff
                    continue
                break  # unresolved indirect jmp (register/memory) - stop this path

            if mnem == "call":
                if insn.op_str.startswith("0x"):
                    toff = int(insn.op_str, 16)
                    tphys = seg_off_to_phys(cs_val, toff)
                    add_label(tphys, "sub", phys)
                    queue.append((cs_val, toff))
                # unresolved indirect call: target unknown, but a call always
                # returns to nxt_off, so fall through either way
                off = nxt_off
                continue

            if mnem.startswith(COND_JUMP_PREFIX) and mnem not in ("jmp",) and insn.op_str.startswith("0x"):
                toff = int(insn.op_str, 16)
                tphys = seg_off_to_phys(cs_val, toff)
                add_label(tphys, "loc", phys)
                queue.append((cs_val, toff))
                off = nxt_off
                continue

            if mnem in STOP_MNEMONICS:
                break

            off = nxt_off

    return chips, visited, labels


def render(chips, visited, labels, out_path, sym_path):
    for phys, lab in labels.items():
        if phys in FUNCTIONAL_NAMES:
            lab["name"] = FUNCTIONAL_NAMES[phys]
        elif "fixed_name" in lab:
            lab["name"] = lab["fixed_name"]
        else:
            lab["name"] = ("SUB_%05X" % phys) if lab["kind"] == "sub" else ("L_%05X" % phys)

    ordered = sorted(visited.values(), key=lambda e: e["phys"])
    with open(out_path, "w") as f:
        f.write("; Tektronix 2230 main system ROM (160-3532-14 + 160-3633-14)\n")
        f.write("; Intel 8088/8086 real mode, recursive-descent from reset vector.\n")
        f.write("; CONFIRMED mapping: 3633=0xE0000-0xEFFFF, 3532=0xF0000-0xFFFFF\n")
        f.write("; Labels are address-based placeholders; rename to functional\n")
        f.write("; names in the .symbols.json as their purpose is understood.\n\n")

        last_phys = None
        for e in ordered:
            phys = e["phys"]
            if last_phys is not None and phys != last_phys:
                f.write("\n")
            lab = labels.get(phys)
            if lab:
                f.write(f"{lab['name']}:\n")

            op = e["op"]
            mnem = e["mnem"]
            resolved = None
            if mnem in ("jmp", "call") and op.startswith("0x"):
                toff = int(op, 16)
                tphys = ((e["seg"] << 4) + toff) & 0xFFFFF
                resolved = labels.get(tphys, {}).get("name")
            elif mnem in ("ljmp", "lcall"):
                m = FAR_TARGET_RE.match(op)
                if m:
                    tseg = int(m.group(1), 16)
                    toff = int(m.group(2), 16)
                    tphys = ((tseg << 4) + toff) & 0xFFFFF
                    resolved = labels.get(tphys, {}).get("name")
            elif mnem.startswith("j") and mnem != "jmp" and op.startswith("0x"):
                toff = int(op, 16)
                tphys = ((e["seg"] << 4) + toff) & 0xFFFFF
                resolved = labels.get(tphys, {}).get("name")

            if resolved:
                op = f"{resolved}  ; {op}"

            f.write(f"    {e['chip']}:{e['chip_off']:04X}  [{phys:05X}]  "
                     f"{e['raw']:<14s}  {mnem:<7s} {op}\n")
            last_phys = phys + e["size"]

    sym_out = {
        ("%05X" % phys): {
            "name": lab["name"],
            "kind": lab["kind"],
            "ref_count": len(lab["refs"]),
            "functional_name": FUNCTIONAL_NAMES.get(phys),
            "notes": None,
        }
        for phys, lab in sorted(labels.items())
    }
    with open(sym_path, "w") as f:
        json.dump(sym_out, f, indent=2)


if __name__ == "__main__":
    chips, visited, labels = main()
    render(chips, visited, labels, "sysrom_3532_3633.lst", "sysrom_3532_3633.symbols.json")
    print(f"visited {len(visited)} instructions, {len(labels)} labels")
