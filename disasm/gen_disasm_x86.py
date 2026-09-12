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
    (0xE5D1, 0x0057, "INT2_HANDLER_EARLY"),     # INT 2 (NMI), installed at
                                               # reset (0xE5D67) - already
                                               # documented in NOTES.md's
                                               # IVT table but never
                                               # renamed to match the
                                               # INT255_HANDLER_EARLY/LATE
                                               # family. Reads the hardware
                                               # tick bytes at physical
                                               # 0x403FFA/0x403FFB into
                                               # [0x758]/[0x759],
                                               # increments the tick
                                               # counter [0x752], ORs
                                               # [0x1AF2] into the pending-
                                               # work flags [0x1AEE], and
                                               # calls delay_read_128w -
                                               # later replaced by
                                               # INT2_HANDLER_LATE (which
                                               # adds the task-scheduler
                                               # behavior on top)
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
    0x950C2: "build_gpib_message_checksum",   # comm ROM: indexes a
                                               # 78-byte record table at
                                               # [0x742] and a 12-byte
                                               # record table at [0x73E]
                                               # by a caller-given index,
                                               # computes checksums
                                               # (via checksum_bytes)
                                               # over message regions
                                               # derived from those
                                               # records' fields - part
                                               # of GPIB message
                                               # building/verification
    0x961E9: "init_gpib_record",              # comm ROM: (record_index)
                                               # - zeroes 5 word fields
                                               # and sets a status byte
                                               # to 1 within one 12-byte
                                               # entry of the [0x73E]
                                               # record table (the same
                                               # table build_gpib_
                                               # message_checksum reads
                                               # a length field from)
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
    0xF0078: "scale_and_plot_point_default",  # (value at [bp+8]) - pre-
                                               # loads dx:ax from a
                                               # global default
                                               # reciprocal constant at
                                               # [0x6D2] then falls into
                                               # scale_and_plot_point;
                                               # shares its body with a
                                               # 2nd, explicit-reciprocal
                                               # entry point at 0xF0086.
                                               # CORRECTED (was wrongly
                                               # named divide_scale[_
                                               # default] - the actual
                                               # math is multiply-by-
                                               # reciprocal-then-shift,
                                               # not divide, see NOTES.md
    0xF0086: "scale_and_plot_point",          # (raw_value dx:ax,
                                               # reciprocal [bp+8]) -
                                               # fixed-point scale via
                                               # mul32 + ashr32 (NOT a
                                               # real divide - multiply
                                               # by a precomputed
                                               # reciprocal constant,
                                               # then shift right), then
                                               # calls plot_scaled_point
                                               # to draw the scaled
                                               # value and cache it in
                                               # [0x6E6]/[0x6E8] for the
                                               # next line segment - the
                                               # per-sample step of an
                                               # acquisition-to-plotter
                                               # rendering pipeline.
                                               # CORRECTED (was wrongly
                                               # named divide_scale)
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
                                               # interrupt_vectors. NOTE:
                                               # a later find (scan_low_
                                               # ram_chunk0/1/2) shows 3
                                               # SIMILAR-shaped functions
                                               # are a genuine rotating
                                               # low-memory watchdog scan
                                               # (each covering a
                                               # DIFFERENT, non-
                                               # overlapping chunk), not
                                               # a delay - this one might
                                               # be the SAME kind of
                                               # integrity check (of the
                                               # first 256 bytes) rather
                                               # than a timing delay,
                                               # though it's called once
                                               # at startup, not tick-
                                               # rotated - name kept
                                               # since "delay" is still
                                               # plausible for THIS
                                               # specific one-time
                                               # startup call; see
                                               # NOTES.md
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
    0xE5D3D: "scan_low_ram_chunk0",           # rep lodsw x0x30 words
                                               # from 0000:0x00 (bytes
                                               # 0x00-0x5F) - result
                                               # unused; part of a 3-way
                                               # rotation (chunk0/1/2)
                                               # covering 0000:0x00-0xFF
                                               # (the first quarter of
                                               # the IVT) across 3
                                               # scheduler ticks - a
                                               # background low-memory
                                               # watchdog/integrity scan,
                                               # not a timing delay (see
                                               # scheduler_tick_service)
    0xE5D49: "scan_low_ram_chunk1",           # rep lodsw x0x30 words
                                               # from 0000:0x60 (bytes
                                               # 0x60-0xBF) - see
                                               # scan_low_ram_chunk0
    0xE5D58: "scan_low_ram_chunk2",           # rep lodsw x0x20 words
                                               # from 0000:0xC0 (bytes
                                               # 0xC0-0xFF) - see
                                               # scan_low_ram_chunk0
    0x80133: "init_far_pointer_table",         # comm ROM: sets DS to
                                               # its own segment (0x8013)
                                               # and applies a compiled-
                                               # in table (embedded
                                               # right after this code,
                                               # at offset 0x22): first
                                               # word is a destination
                                               # ES segment, then
                                               # repeated (dest_offset,
                                               # far_ptr_lo, far_ptr_hi)
                                               # triples (movsw x2) are
                                               # written to es:[offset]
                                               # until a -1 sentinel -
                                               # bulk-initializes a set
                                               # of RAM-resident far
                                               # pointer variables from
                                               # ROM data; called from
                                               # both the comm ROM and
                                               # the main ROM (0xE1F03)
    0x81404: "print_signed_decimal_serial",    # (n) - prints '-' via
                                               # putchar_serial and
                                               # negates n if negative,
                                               # then calls print_
                                               # unsigned_decimal_serial
    0x8142F: "print_unsigned_decimal_serial",  # (n) - converts n to
                                               # decimal digits via
                                               # repeated div by 10 into
                                               # a stack buffer, then
                                               # prints the resulting
                                               # string via SUB_82D01
    0xE1816: "verify_rom_checksum_and_report",  # (name_far_ptr,
                                               # range1_start,
                                               # range1_end,
                                               # range2_start,
                                               # range2_end) - computes
                                               # compute_range_checksum
                                               # over 2 address ranges,
                                               # chains them together
                                               # (2nd seeded with the
                                               # 1st's result), and
                                               # compares the combined
                                               # 16-bit checksum against
                                               # a stored expected value
                                               # read from a header
                                               # record; if it matches,
                                               # returns cleanly, else
                                               # builds a "<name>
                                               # <computed_hex>
                                               # <expected_hex>"
                                               # diagnostic message
                                               # (strncpy_far/strncat_
                                               # far/format_byte_hex)
                                               # for the self-test
                                               # report. Confirmed
                                               # caller: selftest_comm_
                                               # rom, checksumming the
                                               # comm ROM's two 32KB
                                               # halves (0x80000-
                                               # 0x87FFF, 0x90000-
                                               # 0x97FFF) against the
                                               # "COMM_ROM" string
                                               # label - the real
                                               # generic ROM-checksum-
                                               # and-report engine
                                               # behind selftest_rom_
                                               # checksum
    0xE094B: "init_selftest_report_record",     # writes a fixed 3-byte
                                               # header record (type=3,
                                               # 2, 0) at the buffer
                                               # pointed to by [0x1C80]
                                               # into [0x1B56]/[0x1B58]
                                               # - called once, right
                                               # after print_selftest_
                                               # banner, from print_
                                               # selftest_report_line's
                                               # first-call setup path
                                               # (guarded by [0x1B10])
                                               # - seeds the initial
                                               # readout display-list
                                               # entry for the self-
                                               # test report
    0xE5C65: "select_next_ready_task",          # task scheduler: picks
                                               # a starting group via
                                               # [0x1B76]/[0x760]
                                               # flags into [0x79A],
                                               # then walks BACKWARD
                                               # through the 12-entry
                                               # per-task ready-state
                                               # table at
                                               # [task_idx+0x1A91]
                                               # (the SAME table
                                               # update_plot_retry_
                                               # counters/spawn_task_
                                               # with_tag use - confirms
                                               # it's a genuine per-task
                                               # ready/priority table,
                                               # not plot-specific)
                                               # looking for a task
                                               # whose low nibble
                                               # (after masking off
                                               # bits 0xB0) is nonzero;
                                               # sets the CURRENT TASK
                                               # INDEX [0x1ACD] to the
                                               # task found, and
                                               # [0x1A8F] to 0 (task
                                               # index 8 - likely the
                                               # idle/background task)
                                               # or 0x200 (any other
                                               # task) - the task
                                               # scheduler's task-
                                               # selection logic
    0xE4858: "step_readout_window_pattern",     # sibling of the step_
                                               # progress_pattern_*
                                               # family, but targets
                                               # the readout-window
                                               # register range
                                               # 0x40000+0x6F8..0x6FF
                                               # (8 bytes) instead of a
                                               # single far-ptr
                                               # register: cycles a
                                               # counter [0x1B16]
                                               # through 0-15
                                               # (resetting on arg==1
                                               # or reaching 0xF),
                                               # writing bit
                                               # ([0x1B16]>>3, 0 or 1)
                                               # to whichever of the 8
                                               # destination bytes
                                               # `[0x1B16] & 7` selects
                                               # - a walking-pattern
                                               # exerciser for this
                                               # register family, same
                                               # role as init_selftest_
                                               # register_group's
                                               # "else" branch
    0xE3E97: "draw_display_test_pattern",       # called from within
                                               # run_selftest_sequence's
                                               # final wrap-up phase
                                               # (right after printing
                                               # "2230/2220 Power up
                                               # tests complete."),
                                               # gated on [0x1B83]==0x14
                                               # (comm option
                                               # installed): inits a
                                               # print region, plots an
                                               # initial point, then
                                               # draws two 250-point
                                               # diagonal lines via
                                               # plot_readout_point_
                                               # scaled - the readout
                                               # test pattern used by
                                               # the surrounding "MI /
                                               # line stuck high"
                                               # interrupt-line check
                                               # (diagnostic string at
                                               # 0xFF7B0+0x497)
    0xE2CD3: "reset_progress_pattern_e",        # clears [0x1B1C] to
                                               # 0xFF and es:[0x1D20]
                                               # (step_progress_
                                               # pattern_e's target
                                               # register) to 0xFF -
                                               # called once, at the
                                               # start of selftest_
                                               # display_irq_idle, to
                                               # reset that pattern's
                                               # state before the test
    0xE2FFC: "step_front_panel_switch_b_test", # per-position worker
                                               # for selftest_front_
                                               # panel_switch_b: scans
                                               # 0-0x15 (21 positions)
                                               # via update_menu_
                                               # position, computes a
                                               # shifted 0xFFE
                                               # threshold mask per
                                               # position, calls
                                               # verify_adc_calibration
                                               # (ADC readback check,
                                               # same shape as switch_
                                               # a's run_adc_selftest_
                                               # range/verify_adc_
                                               # calibration pairing),
                                               # formats the result and
                                               # a unit-label string
                                               # (from a RAM buffer at
                                               # [0x476]) into a report
                                               # message
    0xE060A: "verify_prc_readback_pattern",     # a 2-phase state
                                               # machine driven by
                                               # [0x1B5F]: phase 1
                                               # compares a bit read
                                               # back from far ptr
                                               # [0x322] (the CONFIRMED
                                               # comm-option probe
                                               # register 0x40377E)
                                               # against the expected
                                               # pattern state in
                                               # [0x257]/[0x258]; phase
                                               # 2 does the same for
                                               # far ptr [0x1D20]
                                               # against [0x1B1C] -
                                               # returns 0x20 when a
                                               # phase's pattern
                                               # verifies, 0 otherwise,
                                               # advancing/resetting
                                               # [0x1B5F] accordingly -
                                               # the verification half
                                               # of the PRC_READBACK
                                               # self-test whose
                                               # register table is set
                                               # up by init_selftest_
                                               # register_group and
                                               # driven by the step_
                                               # progress_pattern_*
                                               # helpers
    0xE4443: "init_selftest_register_group",    # (group) - initializes
                                               # a table of far
                                               # pointers used by the
                                               # step_progress_pattern_
                                               # * helpers and other
                                               # self-test code:
                                               # group==1 sets [0x31E]/
                                               # [0x322]/[0x326]/[0x32A]
                                               # /[0x32E]/[0x332]/
                                               # [0x336]/[0x33A] to
                                               # physical 0x48000 (the
                                               # readout second plane)
                                               # and several 0x437xx
                                               # registers, including
                                               # the CONFIRMED comm-
                                               # option probe pair at
                                               # 0x40377E/0x4007DE (see
                                               # detect_comm_option_hw)
                                               # - i.e. this group mixes
                                               # a display buffer with
                                               # real comm-option
                                               # hardware probes; else
                                               # sets [0x1AD0]/[0x1AD4]/
                                               # [0x1AD8]/[0x1ADC] to
                                               # the readout-window
                                               # register family
                                               # (0x406F0/0x4067C/
                                               # 0x406F8/0x406BC, the
                                               # same family selftest_
                                               # comm_fget_flag uses)
    0xE43F2: "report_and_read_channel1",       # sets report_screen_
                                               # mode(2) then calls
                                               # read_channel1_status -
                                               # called from run_
                                               # selftest_sequence when
                                               # its readback loop is
                                               # aborted/interrupted
    0xE46C3: "step_progress_pattern_a",         # (mode) - one of 5
                                               # near-identical helpers
                                               # called together by
                                               # refresh_report_display
                                               # (part of run_selftest_
                                               # sequence's progress
                                               # display): maintains a
                                               # shifting/wrapping bit
                                               # pattern in [0x1B0C]
                                               # and writes the current
                                               # byte to a far-pointer
                                               # hardware register at
                                               # [0x31E]; mode==2 clears
                                               # it to 0 instead - a
                                               # walking-bit-pattern
                                               # progress/busy
                                               # indicator, exact
                                               # register identity not
                                               # confirmed
    0xE470B: "step_progress_pattern_b",         # sibling of step_
                                               # progress_pattern_a,
                                               # same shape, targeting
                                               # far-pointer register
                                               # [0x326] and counter
                                               # [0x1B0E]
    0xE4762: "step_progress_pattern_c",         # sibling of step_
                                               # progress_pattern_a,
                                               # targeting far-pointer
                                               # register [0x336] and
                                               # counter [0x1B10]
                                               # (also mirrors the byte
                                               # to [0x257]/[0x258])
    0xE47C5: "step_progress_pattern_d",         # sibling of step_
                                               # progress_pattern_a,
                                               # targeting far-pointer
                                               # register [0x33A] (same
                                               # target as write_
                                               # measurement_channel_
                                               # reg) and counter
                                               # [0x1B12]
    0xE480E: "step_progress_pattern_e",         # sibling of step_
                                               # progress_pattern_a,
                                               # targeting far-pointer
                                               # register [0x1D20] and
                                               # counter [0x1B14]
    0xE0D06: "save_restore_measurement_settings", # (mode) - mode==1
                                               # copies the current
                                               # measurement-settings
                                               # block ([0x256]-
                                               # [0x264], 5 bytes + 5
                                               # words) into a backup
                                               # area ([0x1B20]-
                                               # [0x1B32]); mode==2
                                               # copies it back -
                                               # a save/restore
                                               # snapshot, likely used
                                               # to preserve the user's
                                               # settings around
                                               # running self-tests
    0xE128D: "verify_adc_control_toggle",       # sets a control byte
                                               # to 0x10 then back to 0
                                               # (also mirrored to a
                                               # far-ptr target at
                                               # [0x326]), writing
                                               # [0x259] to the
                                               # measurement channel
                                               # register each time and
                                               # reading back bit
                                               # 0x1000 of a status
                                               # word at far ptr
                                               # [0x332]; stores
                                               # whether the readback
                                               # changed between the
                                               # two states into
                                               # [0x1B4E] - a hardware
                                               # sanity check called
                                               # right before run_adc_
                                               # selftest
    0xE0B6C: "init_readout_port_config",        # writes 3 fixed config
                                               # bytes directly to the
                                               # readout port structure
                                               # at physical
                                               # 0x40000+0x6F0+1/+2/+3
                                               # (0x29/0x23/6) - resets
                                               # the readout/CRT
                                               # character-generator
                                               # hardware to a known
                                               # initial state
    0xE0B41: "reset_readout_port",             # thin wrapper: calls
                                               # init_readout_port_
                                               # config
    0xE0A23: "scroll_selftest_report_display",  # called periodically;
                                               # decrements a countdown
                                               # byte at report-record
                                               # offset 2 ([0x1B56]+2);
                                               # on expiry, resets the
                                               # countdown to 0x32 and
                                               # either (mode==2, from
                                               # init_selftest_report_
                                               # record) blanks the
                                               # channel display via
                                               # write_measurement_
                                               # channel_reg(0xFF) and
                                               # advances mode to 3, or
                                               # (other modes) writes
                                               # the next character of
                                               # the report string to
                                               # write_measurement_
                                               # channel_reg, wrapping
                                               # mode back to 2 once
                                               # the string (length =
                                               # record byte 0) has
                                               # been fully shown -
                                               # scrolls the self-test
                                               # report text across a
                                               # small channel/status
                                               # display register
    0xE097B: "append_selftest_report_char",     # (char) - appends a
                                               # byte to the current
                                               # self-test report
                                               # record at [0x1B56]
                                               # (the record init_
                                               # selftest_report_record
                                               # seeds with a length
                                               # counter at offset 0):
                                               # if the record already
                                               # has more than 3 bytes
                                               # and both the new and
                                               # previous byte share a
                                               # nonzero high nibble,
                                               # ORs the new byte into
                                               # the previous slot
                                               # (nibble-packing)
                                               # instead of appending;
                                               # otherwise appends
                                               # normally, increments
                                               # the length counter,
                                               # and null-terminates
    0xE230B: "run_indexed_adc_selftest",        # (index) - builds a
                                               # descriptive test name
                                               # by copying a base
                                               # string from the fixed
                                               # string table (0xFF7B0
                                               # +0x6A8) and appending
                                               # a per-index name from
                                               # a 10-byte-per-record
                                               # device table at
                                               # [0x1DCC] (+2 offset),
                                               # then calls the already
                                               # -confirmed run_adc_
                                               # selftest with per-
                                               # record min/max
                                               # threshold values
                                               # (offsets +0/+1/+8/+9)
    0xE22AF: "run_adc_selftest_range",         # (start_flags) - loops
                                               # calling update_menu_
                                               # position(0, 8, ...) to
                                               # scan up to 8 positions
                                               # (the same range-scan
                                               # shape as the front-
                                               # panel-switch self-
                                               # tests), calling run_
                                               # indexed_adc_selftest
                                               # for each position
                                               # until the sentinel
                                               # 0xFFFE is returned,
                                               # OR-folding each
                                               # result into the
                                               # overall pass/fail
                                               # accumulator
    0xE1FBC: "selftest_comm_fget_flag",         # the real body of
                                               # selftest_comm_
                                               # loopback_b (called
                                               # after its installed-
                                               # gate check): writes a
                                               # command byte (0x86,
                                               # then 6) to the comm
                                               # register at physical
                                               # 0x406F3 (0x40000+
                                               # 0x6F0+3), reads
                                               # back status bits at
                                               # 0x4067C (bit 4) and
                                               # 0x406BC (bit 0x80),
                                               # and reports "COMM_LB /
                                               # FGET NOT SET" or
                                               # "FGET NOT CLEAR" via
                                               # print_selftest_report_
                                               # line if the flag
                                               # doesn't behave as
                                               # expected
    0x8526B: "process_gpib_command_byte",       # comm ROM: handles a
                                               # received GPIB command/
                                               # data byte at [6] under
                                               # the comm critical-
                                               # section lock ([0x5A3]
                                               # saved/engaged/
                                               # restored directly,
                                               # like set_comm_
                                               # critical_flag but
                                               # inline): if bit 0x80
                                               # is set (address/
                                               # command byte), sets
                                               # [0x590]=0x80; else
                                               # clamps [6] to a valid
                                               # range (max 0x42, else
                                               # reset to 0x26) and
                                               # records it into a per-
                                               # row lookup slot in the
                                               # scratch buffer at
                                               # [0x586], indexed via
                                               # the address-to-index
                                               # table at [0x712] (same
                                               # table service_comm_tx_
                                               # queue/process_gpib_
                                               # command_byte both use).
                                               # Always calls SUB_97905;
                                               # if in GPIB mode with
                                               # [0x61C]/[0x57F] set,
                                               # triggers a query
                                               # response via print_
                                               # param_list_response.
                                               # Mechanism confirmed;
                                               # exact GPIB protocol
                                               # semantics (command vs.
                                               # data byte handling)
                                               # not confirmed
    0x96EC6: "release_comm_hold_critical",      # comm ROM: RS-232-only
                                               # ([0x629]==0) wrapper -
                                               # under DS switch to
                                               # 0x8F80, engages the
                                               # comm critical flag
                                               # (set_comm_critical_
                                               # flag(0xFFFF), saving
                                               # the old value), calls
                                               # release_comm_hold(2),
                                               # then restores the
                                               # critical flag to its
                                               # saved value
    0x96F0D: "engage_comm_hold_critical",       # comm ROM: mirror of
                                               # release_comm_hold_
                                               # critical - same RS-232
                                               # -only critical-section
                                               # wrapper, but calls
                                               # engage_comm_hold(2)
                                               # instead
    0xE004F: "plot_point_with_flag_update",     # (arg) - calls scale_
                                               # and_plot_point_default
                                               # (0x8F80), then if
                                               # [0x61A] is set, ORs
                                               # arg into [0x61B], then
                                               # calls scale_and_plot_
                                               # point_default again
                                               # with the first call's
                                               # saved AX result -
                                               # called once, from the
                                               # plot-output task area
                                               # (0xE6E2B); see disasm/
                                               # NOTES.md "Open puzzle"
                                               # for the same bracketing
                                               # shape seen in the comm
                                               # ROM's compute_parity_
                                               # mode_code
    0x97BE6: "engage_comm_hold",                # comm ROM: (reason
                                               # bitmask) - ORs reason
                                               # into a hold bitmask
                                               # [0x45C]; if this is
                                               # the first hold engaged
                                               # (was 0), updates the
                                               # channel status byte
                                               # es:[0x6D6+2] and, if
                                               # enabled ([0x45B]),
                                               # requests XOFF via
                                               # [0x460] bit 0x2 (see
                                               # get_xon_xoff_byte),
                                               # immediately servicing
                                               # the tx queue if data
                                               # is pending ([0x45A])
    0x97C28: "release_comm_hold",              # comm ROM: (reason
                                               # bitmask) - AND-NOTs
                                               # reason out of the hold
                                               # bitmask [0x45C]; if
                                               # this releases the LAST
                                               # hold (now 0), updates
                                               # the channel status
                                               # byte and, if enabled,
                                               # requests XON via
                                               # [0x460] bit 0x1,
                                               # servicing the tx queue
                                               # if pending - counterpart
                                               # to engage_comm_hold
    0x96C5F: "send_response_terminator",        # comm ROM: if a
                                               # pending-terminator
                                               # flag [0x459] is set,
                                               # sends CR+LF (directly
                                               # via serial_tx_buffer_
                                               # put for GPIB mode with
                                               # [0x4F1] bit 0x20 set,
                                               # or via send_serial_
                                               # newline otherwise for
                                               # RS-232), then marks
                                               # completion flags
                                               # [0x5A3]/[0x456] and
                                               # calls set_comm_
                                               # critical_flag(0)
    0x96E5F: "finish_comm_response",           # comm ROM: checks a
                                               # channel-ready bit
                                               # (es:[0x6D6+2] bit 1);
                                               # if set and no
                                               # terminator already
                                               # pending, sends a 0xFF
                                               # marker byte via
                                               # serial_tx_buffer_put
                                               # then calls send_
                                               # response_terminator;
                                               # if data was left
                                               # pending ([0x45A]),
                                               # sets busy flags
                                               # ([0x5A3]/[0x68F]) and
                                               # calls set_comm_
                                               # critical_flag(0)
    0x96872: "spawn_task_with_tag",             # comm ROM: (tag) -
                                               # stores tag into the
                                               # CURRENTLY running
                                               # task's per-task
                                               # scratch byte at
                                               # [task_index+0x744]
                                               # (task_index read from
                                               # [0x1ACD], the task-
                                               # scheduler's current-
                                               # task index), then
                                               # calls create_task() to
                                               # spawn a new task -
                                               # called by serial_tx_
                                               # buffer_put(0xA) to
                                               # launch a background
                                               # comm-tx-servicing task.
                                               # Note: [x+0x744] is the
                                               # same per-item table
                                               # update_plot_retry_
                                               # counters iterates over
                                               # 12 entries - suggests
                                               # that table is really a
                                               # general per-task
                                               # scratch byte, not
                                               # specifically a "plot
                                               # retry counter"
    0x96800: "compute_parity_mode_code",        # comm ROM: dispatches
                                               # on the DIP-switch-
                                               # derived parity value
                                               # [0x461] to produce a
                                               # standard parity mode
                                               # code stored into the
                                               # config array at es:
                                               # [0x73A+0x20]: 0->1,
                                               # 1->4, 2->(3 if RS-232
                                               # mode and [0x4ED]!=0,
                                               # else 0), else->0.
                                               # Also calls scale_and_
                                               # plot_point_default
                                               # (confirmed address
                                               # 0xF0078) at entry with
                                               # arg 0x8F80, saves its
                                               # AX result, and calls
                                               # it again at exit with
                                               # that saved value - the
                                               # same argument/result
                                               # shape as the DS-switch
                                               # helper set_ds_return_
                                               # old, but calling the
                                               # plot-scaling function
                                               # instead; not resolved
                                               # whether this is a real
                                               # cross-purpose reuse or
                                               # a labeling mistake -
                                               # see disasm/NOTES.md
    0x916D6: "handle_gpib_device_clear",        # comm ROM: clears a
                                               # 36-entry table at
                                               # [0x6BC]/[0x1DDC]+0xA2
                                               # (address/status list),
                                               # then dispatches on the
                                               # current command code
                                               # [0x686] (0x11 -> set
                                               # [0x468]=1; 0x21 ->
                                               # [0x548]=0xFFFF; else,
                                               # if [0x1B78] clear,
                                               # resets [0x686]=0 and
                                               # busy-waits via
                                               # restart_current_task
                                               # until [0x548]==0);
                                               # finally resets
                                               # [0x686]=0 and calls the
                                               # main ROM's SUB_E9858
                                               # (cross-ROM) to reset/
                                               # clear a display region
                                               # - shape matches a
                                               # GPIB Device Clear (DCL)
                                               # handler
    0x9628C: "comm_rom_boot_init",              # called exactly once,
                                               # cross-ROM, from the
                                               # main ROM's boot
                                               # sequence (0xE6D5A),
                                               # gated by `[0x1BF9]!=0`
                                               # (the comm-option-
                                               # installed result byte
                                               # from check_comm_
                                               # option_installed) -
                                               # bootstraps the comm
                                               # ROM: init_far_pointer_
                                               # table, init_comm_
                                               # device_type_and_
                                               # defaults, then poll_
                                               # dip_switch_change, all
                                               # wrapped in DS-segment
                                               # switches to/from
                                               # 0x8F80
    0x85E66: "init_comm_device_type_and_defaults", # detects device
                                               # type the same way as
                                               # init_comm_default_
                                               # params (es:
                                               # [0x732+0x1F]==0x1E ->
                                               # [0x4F5]=1 else 4),
                                               # sets buffer-size-like
                                               # defaults [0x4FC]/
                                               # [0x4F8]/[0x4FA], then
                                               # calls init_comm_
                                               # default_params(1) for
                                               # the rest
    0x962C2: "poll_dip_switch_change",          # toggles a strobe/
                                               # chip-select-looking
                                               # flag (es:[0x6E2+3],
                                               # 0 then 1) bracketing
                                               # two reads of the GPIB-
                                               # config DIP switch byte
                                               # (far ptr [0x6DA]) and
                                               # XORs them to detect
                                               # which bits changed,
                                               # then branches on bits
                                               # 0x40/0x80 of the
                                               # difference
    0x966E7: "read_dip_switches_serial_config", # comm ROM: reads a
                                               # hardware config-switch
                                               # byte via far ptr
                                               # [0x6DE] (inverted -
                                               # active-low switch
                                               # bank) and decodes it
                                               # into a baud-rate-like
                                               # code [0x4EC] (bits
                                               # 0-3, special-cased to
                                               # 0x1F when all 4 set)
                                               # and a parity mode
                                               # [0x4ED] (bit 4 gates
                                               # 0 vs bits 5-6+1); also
                                               # reads a second switch
                                               # byte via far ptr
                                               # [0x6DA] for a CR/LF
                                               # option [0x4EF] (bit 3)
                                               # and another config
                                               # byte [0x461] (bits
                                               # 4-5) - very likely the
                                               # firmware-side reader
                                               # for the comm option
                                               # board's physical
                                               # "PARAMETERS" DIP
                                               # switch bank (see
                                               # HARDWARE.md)
    0x96781: "read_dip_switches_gpib_config",   # comm ROM: reads the
                                               # same two hardware
                                               # switch bytes ([0x6DE]/
                                               # [0x6DA]) as read_dip_
                                               # switches_serial_config
                                               # but decodes the GPIB-
                                               # relevant fields
                                               # instead: a 5-bit GPIB
                                               # primary address
                                               # (0-30) into [0x4F0],
                                               # and mode flag bits
                                               # into [0x4F1]/[0x461]
    0x96696: "reset_gpib_addr_scratch",         # clears the 10-byte
                                               # scratch buffer at
                                               # [0x586] (same buffer
                                               # used for building a
                                               # GPIB device-address
                                               # list elsewhere),
                                               # conditionally calls
                                               # SUB_97905 when in GPIB
                                               # mode ([0x629]!=0) and
                                               # not addressed as
                                               # device 1 ([0x580]!=1),
                                               # then sets [0x581]=0x80
    0x924D2: "build_comm_status_message",       # comm ROM: (index 0-3)
                                               # picks one of 4 far-
                                               # pointer message chunks
                                               # (0x693/0x69D/0x6A7/
                                               # 0x6B1, 10 bytes apart)
                                               # by index (default/
                                               # index<0 falls back to
                                               # a base template at
                                               # 0x65E), scans the base
                                               # template for its
                                               # length, appends the
                                               # selected chunk onto it
                                               # via SUB_EAC86(dest,
                                               # src) (see NOTES.md -
                                               # same call signature as
                                               # the 160-3532 stroke-
                                               # data-append call
                                               # sites), terminates
                                               # with 0xFF, and clears
                                               # [0x6BB]
    0x821F7: "init_comm_default_params",        # comm ROM: (mode) - a
                                               # no-op unless mode==1;
                                               # when it is, sets
                                               # [0x4F5] to 1 or 4
                                               # depending on whether a
                                               # status byte at es:
                                               # [0x732+0x1F] equals
                                               # 0x1E (device-type
                                               # select, likely comm-
                                               # option-installed vs
                                               # not), then initializes
                                               # a batch of comm
                                               # parameter defaults
                                               # ([0x557]/[0x559]/
                                               # [0x55A]/[0x55B]/
                                               # [0x55C]/[0x560]/
                                               # [0x562]/[0x55E])
    0x97B01: "reinit_comm_channel",             # comm ROM's own top-
                                               # level channel
                                               # reinitialization
                                               # sequence (analogous to
                                               # the main ROM's reinit_
                                               # system_state): if a
                                               # flag at es:[0x732+0x93]
                                               # is set, calls reinit_
                                               # system_state itself
                                               # (cross-ROM, wrapped in
                                               # DS segment switches);
                                               # conditionally clears a
                                               # config flag via get_/
                                               # set_comm_config_flag;
                                               # unconditionally calls
                                               # reset_comm_parser_
                                               # state, init_comm_
                                               # channel_state_a, init_
                                               # comm_rx_queue_and_
                                               # ready_flags, SUB_96696;
                                               # resets more flags
                                               # ([0xC]/[0x454]/[0x7B8]);
                                               # and calls SUB_850A2 or
                                               # resets [0x5A4]
                                               # depending on its value
    0x94488: "get_comm_config_flag",            # (index) - reads a byte
                                               # from a config/flag
                                               # array at far ptr
                                               # [0x73A], zero-extended
    0x944A2: "set_comm_config_flag",            # (index, value) -
                                               # writes a byte into the
                                               # config/flag array at
                                               # far ptr [0x73A]
                                               # (get_comm_config_
                                               # flag's counterpart),
                                               # then invalidates a
                                               # cache (clears es:
                                               # [0x736] word to
                                               # 0xFFFF) and sets bit 0
                                               # of [0x603] (plus bit 1
                                               # too if [0x604]==0)
    0x96597: "init_comm_channel_state_a",       # comm ROM: resets
                                               # several default far
                                               # pointers (to offset
                                               # 0xF and 0x46C within
                                               # DS) and counters/flags
                                               # to 0, then sets the
                                               # channel status byte
                                               # es:[0x6D6+3] to 2 or 3
                                               # depending on [0x629]/
                                               # [0x4F1] - first half of
                                               # a channel (re)init
                                               # sequence, always
                                               # called immediately
                                               # before init_comm_tx_
                                               # queue_and_ready_flags
    0x96634: "init_comm_tx_queue_and_ready_flags", # comm ROM:
                                               # **CORRECTED from an
                                               # earlier wrong name
                                               # init_comm_rx_queue_
                                               # and_ready_flags** -
                                               # resets the tx ring
                                               # buffer's read/write
                                               # pointers ([0x448]/
                                               # [0x44C], same base
                                               # 0xAF used by service_
                                               # comm_tx_queue) and
                                               # related flags, then
                                               # either sets the channel
                                               # status byte es:
                                               # [0x6D6+3]=5 ([0x629]
                                               # set, GPIB) or clears
                                               # tx-ready (es:[0x6E2+1]/
                                               # [0x596], RS-232) -
                                               # second half of the pair
                                               # started by init_comm_
                                               # channel_state_a
    0x800FC: "update_comm_tx_ready_flag",       # (clear) - single-shot
                                               # toggle of the tx-ready
                                               # byte pair ([0x596] and
                                               # es:[di+1] via far ptr
                                               # [0x6E2]): clear!=0 sets
                                               # not-ready; clear==0
                                               # sets ready ONLY if
                                               # [0x45A] indicates data
                                               # is actually pending -
                                               # called by enqueue_comm_
                                               # char(0) right after
                                               # storing a new byte
    0x8009B: "set_comm_queue_busy",            # (engage) - nestable,
                                               # interrupt-safe (cli/
                                               # pushf) critical-section
                                               # marker for the comm rx/
                                               # tx queue: engage!=0
                                               # pushes a level onto the
                                               # [0x5A1] nesting counter
                                               # and clears both ready
                                               # flags ([0x597]/[0x596]
                                               # and the es:[0x6E2] word
                                               # pair); engage==0 pops a
                                               # level and, once fully
                                               # unnested, restores rx-
                                               # ready unconditionally
                                               # and tx-ready only if
                                               # [0x45A] indicates data
                                               # is pending (same gate
                                               # as update_comm_tx_
                                               # ready_flag)
    0x80060: "set_comm_flow_hold",              # (engage) - guarded
                                               # on/off toggle of
                                               # [0x5A1]/[0x5A2]: if
                                               # engage!=0 and [0x5A1]
                                               # was 0, sets both to
                                               # 0xFF and writes 0x93
                                               # into the comm status
                                               # structure at es:
                                               # [0x6D6+3]; if engage==0
                                               # and [0x5A1] was
                                               # nonzero, clears both to
                                               # 0 and writes 0x13
                                               # (XOFF) there instead -
                                               # looks like asserting/
                                               # releasing a flow-
                                               # control hold, guarded
                                               # against redundant
                                               # transitions
    0x85238: "reset_comm_parser_state",        # clears a cluster of
                                               # comm ROM parser/command
                                               # state variables
                                               # ([0x62A]/[0x63A]/
                                               # [0x63B]/[0x632]/[6]/
                                               # [0x61B]/[0x5A4]/[3]) -
                                               # called as part of the
                                               # comm channel
                                               # reinitialization
                                               # sequence in SUB_97B01
    0x82D01: "print_string_serial",            # (far_str_ptr) - loops
                                               # sending each byte via
                                               # serial_tx_buffer_put
                                               # until a null terminator
    0x82D2B: "print_padded_label",             # prints a run of `si`
                                               # characters (source
                                               # picked from either a
                                               # table at [0x6EE] or a
                                               # far ptr [0x60A],
                                               # depending on [0x4F2]),
                                               # uppercasing lowercase
                                               # letters, then a
                                               # trailing space -
                                               # prints a fixed-width
                                               # GPIB-style field label
    0x82EB9: "print_param_list_response",      # loops calling get_
                                               # next_param_value and
                                               # printing each result
                                               # via print_signed_
                                               # decimal_serial,
                                               # separated by `,`
                                               # (0x2C) while more
                                               # values remain
                                               # ([0x580]!=[0x590]),
                                               # terminated by `;`
                                               # (0x3B) - classic IEEE-
                                               # 488/GPIB query-response
                                               # value-list formatting
    0x82F14: "get_next_param_value",           # saves the previous
                                               # [0x580] into [0x581],
                                               # then fetches a value
                                               # via SUB_97905([0x57F])
                                               # and returns it masked
                                               # to a byte
    0xFC624: "ack_comm_hw_status_bits",        # (out_status_ptr,
                                               # req_byte_ptr) - reads
                                               # the comm-option hw
                                               # status register at
                                               # physical 0x40377E into
                                               # *out_status_ptr, then
                                               # for each of 3 request
                                               # bits (0x80/0x20/0x40)
                                               # in *req_byte_ptr, if
                                               # both the request bit
                                               # and the matching status
                                               # bit are set, clears the
                                               # bit and writes the
                                               # updated byte to the
                                               # comm-option hw control
                                               # register at physical
                                               # 0x4007DE - same address
                                               # pair as detect_comm_
                                               # option_hw; returns a
                                               # bitmask of which
                                               # request bits were
                                               # acknowledged
    0x95F69: "init_comm_device_table",         # comm ROM: zeroes bytes
                                               # 0-9 and sets byte 0xA=1
                                               # (an "enabled" default?)
                                               # for each of 26 12-byte
                                               # records at far ptr
                                               # [0x73E], then memset_
                                               # far's a large block at
                                               # [0x742] and clears
                                               # several fields at
                                               # [0x746] - a comm-
                                               # channel/device table
                                               # init routine, entered
                                               # via a DS switch to
                                               # segment 0x8F80
    0xE45A6: "compute_range_checksum",         # (seed, start_far_ptr,
                                               # end_far_ptr) - classic
                                               # shift-and-add-with-
                                               # carry running checksum
                                               # over an inclusive byte
                                               # range: cx=(cx<<1)+
                                               # carry-adjusted byte,
                                               # for each byte from
                                               # start to end
    0xE5839: "draw_box_outline",               # draws a rectangle's 4
                                               # edges via 4 calls to
                                               # draw_readout_line,
                                               # connecting the corners
                                               # implied by its 5 word
                                               # args (x/y/w/h/style)
    0xE71C9: "update_plot_retry_counters",     # iterates a 12-entry
                                               # per-item table: when a
                                               # per-item countdown at
                                               # [item+0x744] reaches 0,
                                               # increments a per-item
                                               # state/attempt counter
                                               # at [item+0x1A91] (with
                                               # bit-masked gating -
                                               # 0xB0 then 0x40); called
                                               # from the plot-output
                                               # task's main loop.
                                               # Mechanism confirmed
                                               # (matches the identical
                                               # inline single-item
                                               # version at [0x1A9B] =
                                               # item #10 of the same
                                               # array); exact meaning
                                               # of the 12 items and the
                                               # counter's role (retry
                                               # limit? backoff timer?)
                                               # not confirmed
    0x96CB5: "send_serial_newline",            # comm ROM: sends CR
                                               # (0xD) via serial_tx_
                                               # buffer_put, and also
                                               # sends LF (0xA)
                                               # afterward if [0x629]
                                               # is clear AND [0x4EF]
                                               # (a CR+LF line-ending
                                               # option) is set -
                                               # otherwise sends CR only
    0xE79D3: "putchar_serial_with_newline_handling", # (char) - if
                                               # char==0x0A (LF),
                                               # switches DS into the
                                               # comm ROM's segment via
                                               # set_ds_return_old and
                                               # calls send_serial_
                                               # newline, then restores
                                               # DS; otherwise sends the
                                               # byte directly via
                                               # putchar_serial_seg
    0xF0446: "reset_all_channel_plot_caches",  # resets 8 plot-position
                                               # caches ([0x6B2]/[0x6B4]/
                                               # [0x6E6]/[0x6E8]/[0x6F2]/
                                               # [0x6F4]/[0x6F6]/[0x6F8],
                                               # same family as the
                                               # confirmed [0x6E6]/
                                               # [0x6E8] plot-scale
                                               # cache) to the midpoint
                                               # 0x800, clears [0x700]/
                                               # [0x702]/[0x70E], sets
                                               # [0x704]=0x14, writes
                                               # 0xD5 to a channel-state
                                               # byte via far ptr
                                               # [0x6FA], and resets the
                                               # plot cursor to
                                               # (0x200,0x200) via
                                               # update_plot_position -
                                               # entered via jmp from an
                                               # un-prologued "push es"
                                               # stub, same shared-tail
                                               # pattern as convert_
                                               # sample_value
    0xF8E98: "deselect_item_pair",             # (pair_index) - calls
                                               # set_item_active_flag
                                               # (2*pair_index, 0) and
                                               # set_item_active_flag
                                               # (2*pair_index+1, 0) -
                                               # clears the "selected"
                                               # bit for both items of
                                               # a pair
    0xF8F24: "disable_item_pair",              # (pair_index) - calls
                                               # deselect_item_pair,
                                               # then also clears bit 1
                                               # ("enabled/valid") of
                                               # [item+0x1BE2] for both
                                               # items of the pair -
                                               # removes them from
                                               # selectability entirely,
                                               # not just deselecting
    0xF0CCC: "start_plot_output_task",         # calls SUB_E8E03(2), then
                                               # format_string_va("%c%c",
                                               # 0x1B, 0x40) - sends an
                                               # ESC @ device-reset
                                               # sequence (standard
                                               # printer/plotter reset) -
                                               # then falls through to
                                               # a shared tail that
                                               # clears the HPGL PU/PD
                                               # mode var [0x6CA], sets
                                               # a countdown [0x792]=
                                               # 0x64 and ready-flag
                                               # [0x78D]=1, and spawns a
                                               # background task via
                                               # create_task - restarts
                                               # the plotter/hardcopy
                                               # output job
    0x9751A: "get_xon_xoff_byte",              # comm ROM: checks [0x460]
                                               # flow-control-request
                                               # bits - bit 2 -> clears
                                               # it, returns AL=0x13
                                               # (XOFF/DC3); else bit 1
                                               # -> clears it, returns
                                               # AL=0x11 (XON/DC1); else
                                               # returns AL=0 (nothing
                                               # pending)
    0x974E1: "enqueue_comm_char",              # comm ROM: applies the
                                               # parity mode in [0x4ED]
                                               # to the arg byte (0=no
                                               # change, else strip bit
                                               # 7, and if ==3 force bit
                                               # 7 set - space/mark
                                               # parity), stores it into
                                               # the tx queue at [0x6D6],
                                               # clears [0x45A], and
                                               # calls SUB_800FC(0)
    0x97431: "service_comm_tx_queue",          # comm ROM: **CORRECTED
                                               # from an earlier wrong
                                               # name service_comm_rx_
                                               # queue** - direction was
                                               # backwards; this is the
                                               # CONSUMER side of the tx
                                               # ring buffer that
                                               # serial_tx_buffer_put
                                               # (application code)
                                               # PRODUCES into. Pulls
                                               # one byte from the ring
                                               # buffer ([0x448]/[0x44A]
                                               # read ptr, base 0xAF,
                                               # size 0x384, wrapping),
                                               # forwards a pending
                                               # XON/XOFF byte (get_
                                               # xon_xoff_byte) or the
                                               # next queued outgoing
                                               # data byte via enqueue_
                                               # comm_char (which pushes
                                               # it toward the real
                                               # hardware tx path), and
                                               # sets [0x455] (queue-
                                               # empty flag) once the
                                               # read pointer catches up
                                               # to the write pointer
                                               # [0x44C]
    0xE6C85: "reset_display_and_notify_comm", # if [0x766] set: notifies
                                               # comm ROM (notify_comm_
                                               # rom_a), clears [0x1B76]
                                               # bit 0x40, disables ints,
                                               # sets [0x1B8D] flags,
                                               # clears measurement mode
                                               # (via SUB_E6D20), calls
                                               # SUB_E6884 + refresh_
                                               # display_position_cache,
                                               # sets [0x1B76] bit 0x80,
                                               # runs finish_acq_reset_
                                               # and_clear_mode + SUB_
                                               # E7322, re-enables ints,
                                               # notifies comm ROM again
                                               # (notify_comm_rom_b), and
                                               # conditionally calls
                                               # reinit_system_state
    0xE6D04: "reset_display_if_idle",         # calls reset_display_and_
                                               # notify_comm only if
                                               # [0x1B76] masked with
                                               # 0xDF (all bits but 0x20)
                                               # is already zero
    0xE6CE8: "finish_acq_reset_and_clear_mode", # sets [0x761]=1, clears
                                               # measurement mode (via
                                               # clear_measurement_mode_
                                               # relay), then calls
                                               # SUB_FB8E8(0) - the tail
                                               # step of reset_display_
                                               # and_notify_comm, also
                                               # called directly from
                                               # handle_acq_mode_change
    0xE6D20: "clear_measurement_mode_relay",  # thin far-call relay to
                                               # clear_measurement_mode_
                                               # bit (no own logic)
    0xE80E4: "handle_acq_mode_change",        # dispatches on bits of a
                                               # "what changed" flags
                                               # word (arg at [bp-8]):
                                               # bit 0x40 -> acq timeout
                                               # handling (arm/expire a
                                               # countdown via [0x54A],
                                               # snapshot scheduler tick
                                               # [0x752] into a deadline
                                               # [0x544]/[0x546]=+0x783,
                                               # flag [0x1B76]); bits
                                               # 0x23/2/0x20 etc -> calls
                                               # reset_acq_buffers_stub,
                                               # print_and_reset_acq_
                                               # buffers, update_display_
                                               # mode_flags, reset_plot_
                                               # home_or_acq per which
                                               # bit(s) fired
    0xF1001: "convert_sample_value",          # starts with a real x87
                                               # `fmul` (entered via
                                               # fallthrough, no own
                                               # prologue - shares a
                                               # caller's stack frame),
                                               # then calls mul32 and
                                               # sdiv32 - a mixed float/
                                               # integer value
                                               # conversion, likely
                                               # ADC-count-to-engineering
                                               # -unit scaling; possible
                                               # evidence of a real 8087
                                               # coprocessor (see TODO.md)
    0xE9255: "assert_and_halt",               # computes a value via
                                               # SUB_F1001, then calls
                                               # halt_cpu with that value
                                               # plus a small constant
                                               # tag (0xb/0xc/2 across 3
                                               # checks seen) - the
                                               # firmware's assertion-
                                               # failure/panic mechanism:
                                               # if a computed bound is
                                               # exceeded, halt with a
                                               # diagnostic code
    0xE777D: "mul32",                         # 32-bit x 32-bit -> 32-bit
                                               # (truncated) multiply,
                                               # classic 3-partial-
                                               # product algorithm.
                                               # Called by scale_and_
                                               # plot_point - previously
                                               # miscalled a "divide" in
                                               # an earlier session's
                                               # comment; corrected
    0xE7764: "ashr32",                        # 32-bit arithmetic right
                                               # shift by CX bits
                                               # (sar+rcr loop). Called
                                               # by scale_and_plot_point
                                               # right after mul32 - the
                                               # pair together implement
                                               # "multiply by a fixed-
                                               # point reciprocal, then
                                               # shift" fast scaling
    0xE77AE: "sdiv32",                        # the REAL signed 32-bit
                                               # divide (sign via XOR of
                                               # the high words, abs()
                                               # both operands, call the
                                               # unsigned core, reapply
                                               # sign) - found while
                                               # correcting mul32/ashr32
                                               # above; not currently
                                               # known to be called from
                                               # anywhere in the proven
                                               # set
    0xE7F39: "plot_scaled_point",             # conditionally plots a
                                               # scaled acquisition point
                                               # via SUB_E90A5 (mode 0
                                               # gated on [0x70E]/[0x6CA]
                                               # matching the HPGL PU/PD
                                               # mode variable), then
                                               # unconditionally plots it
                                               # again as the new "from"
                                               # point (mode 1) and
                                               # caches it in [0x70A]/
                                               # [0x70C] for the next
                                               # line segment
    0xE7416: "refresh_display_position_cache", # skips recomputation if
                                               # [0x1BF7] is clear and 3
                                               # cached config values
                                               # ([0x72C]/[0x730]/[0x72E]
                                               # vs current [0x1B7A]/
                                               # [0x1B78]/[0x686]) still
                                               # match and [0x79C]==0;
                                               # otherwise updates the
                                               # cache and recomputes a
                                               # display record's linear
                                               # address via seg_off_to_
                                               # linear - a change-
                                               # detection guard avoiding
                                               # redundant recomputation
    0xE6DC4: "reinit_system_state",           # disables interrupts,
                                               # resets a large group of
                                               # global flags/state
                                               # ([0x1BF7], [0x78E],
                                               # [0x548], bit 0x10 of
                                               # [0x1B76], [0x762],
                                               # [0x79C], [0x1A97]),
                                               # conditionally notifies
                                               # the comm ROM if the
                                               # option is installed
                                               # ([0x1BF9]), calls
                                               # divide_scale_default
                                               # for both the comm-ROM
                                               # (0x8F80) and main-ROM
                                               # (0x41) segments, then
                                               # re-enables interrupts -
                                               # a system reinit/reset-
                                               # to-known-state routine,
                                               # exact trigger (boot?
                                               # mode change?) not
                                               # confirmed
    0xE6EE0: "notify_comm_rom_a",             # if comm option installed
                                               # ([0x1BF9]) and [0x1BFA]
                                               # clear, calls a comm-ROM
                                               # handler (SUB_96F0D) -
                                               # a conditional cross-ROM
                                               # notification, twin to
                                               # notify_comm_rom_b
    0xE6EFD: "notify_comm_rom_b",             # same gating as notify_
                                               # comm_rom_a plus an
                                               # additional check
                                               # ([0x1ACD]==0xA, task
                                               # index 10), calls a
                                               # different comm-ROM
                                               # handler (SUB_96EC6)
    0xE75C0: "detect_comm_option_hw",         # THE source of [0x1B83]
                                               # (the "comm option" byte
                                               # checked throughout
                                               # self_test_dispatcher/
                                               # check_comm_option_
                                               # installed). Writes 0 to
                                               # physical 0x40000+0x7DE,
                                               # reads bit 0x1000 of a
                                               # word at 0x40000+0x377E:
                                               # if already set,
                                               # [0x1B83]=0x1E; else
                                               # writes 1 to 0x7DE (a
                                               # write-then-readback
                                               # presence probe, same
                                               # idea as check_comm_
                                               # option_installed's
                                               # 0xAA55 test) and re-
                                               # reads 0x377E - if the
                                               # bit is now set,
                                               # [0x1B83]=0x14; if still
                                               # clear, [0x1B83]=0x1E.
                                               # Exact semantic meaning
                                               # of 0x1E vs 0x14 (which
                                               # specifically means
                                               # "installed") not fully
                                               # resolved - see NOTES.md
    0xE61E3: "create_task_b",                 # near-identical duplicate
                                               # of create_task's body
                                               # (same pushf/cli/push-9-
                                               # regs, same param-
                                               # reordering xchg
                                               # sequence, same [0x1A9D+
                                               # idx*4] table write, same
                                               # ready-flag OR) - called
                                               # specifically from
                                               # mark_task_ready, exact
                                               # reason for the
                                               # duplication (rather than
                                               # sharing create_task)
                                               # not confirmed
    0xE6524: "scheduler_tick_service",        # called unconditionally
                                               # from BOTH paths inside
                                               # INT2_HANDLER_LATE (every
                                               # timer tick, whether or
                                               # not a task switch
                                               # happens): reads 2
                                               # hardware status bytes
                                               # from fixed physical
                                               # addresses 0x403FFA/
                                               # 0x403FFB (memory-mapped
                                               # I/O sitting just above
                                               # the boot-time stack's
                                               # top) into [0x758]/
                                               # [0x759], XORs [0x758]
                                               # against a previous
                                               # snapshot for edge
                                               # detection, and
                                               # increments [0x752] -
                                               # the SAME counter
                                               # wait_readout_tick polls
                                               # - plus other periodic
                                               # debounce/cycling state.
                                               # THE per-tick hardware-
                                               # polling heartbeat behind
                                               # the task scheduler - see
                                               # NOTES.md "A small task
                                               # scheduler"
    0xE5D28: "sync_and_enable_interrupts",    # calls sync_status_byte_
                                               # to_hw, then falls
                                               # through into enable_
                                               # interrupts (sti; retf) -
                                               # a "leave critical
                                               # section" combo: flush
                                               # pending status, then
                                               # re-enable interrupts
    0xF8E66: "set_item_pair_active",          # (pair_index) - activates
                                               # both items in a pair
                                               # (indices pair_index*2
                                               # and pair_index*2+1) via
                                               # two calls to
                                               # set_item_active_flag
    0xE0AAA: "write_measurement_channel_reg", # writes the byte argument
                                               # into the shared hardware
                                               # register [0x33A] - a
                                               # simple setter for the
                                               # same configure_
                                               # measurement_hw register
                                               # cluster
    0xE4680: "refresh_report_display",        # (mode) - calls 5-6
                                               # sub-functions in
                                               # sequence with the same
                                               # mode argument; one
                                               # maintains a rotating
                                               # bit/offset cursor in
                                               # [0x1B0C] and clears
                                               # bytes at that offset in
                                               # the [0x31E]-based
                                               # scratch buffer - called
                                               # from print_selftest_
                                               # report_line's context,
                                               # likely a per-cycle
                                               # report-line refresh step
    0xF1611: "halt_cpu",                      # a single hlt instruction
                                               # - but called from
                                               # elsewhere (see
                                               # assert_and_halt) WITH
                                               # pushed diagnostic
                                               # arguments that are never
                                               # actually used/cleaned
                                               # (the hlt never returns,
                                               # so the caller's "add
                                               # sp,N" cleanup after the
                                               # call is unreachable) -
                                               # this is genuinely the
                                               # firmware's equivalent of
                                               # a C `abort()`/assertion-
                                               # failure trap, not a
                                               # normal function
    0xF8EC8: "set_item_active_flag",          # (item_index, set_flag) -
                                               # sets or clears a bit in
                                               # a byte-per-item flag
                                               # array (at
                                               # [item_index+0x1BE2])
                                               # and the matching field
                                               # in a parallel 20-byte-
                                               # per-item record array
                                               # (offsets 0xA6/0xB0) -
                                               # exact item type (menu
                                               # entry? channel?) not
                                               # confirmed
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
    0xE252A: "selftest_tb_divider",            # **CORRECTED from an
                                               # earlier wrong name
                                               # selftest_comm_option_
                                               # switch**: its step
                                               # helper (SUB_E255E)
                                               # calls verify_timebase_
                                               # prc with a shifted-
                                               # 0xFFF threshold mask,
                                               # matching the real
                                               # on-screen name
                                               # "TB_DIVIDER" confirmed
                                               # in HARDWARE.md's
                                               # DIAGNOSTICS/TESTS/
                                               # ACQUISITION menu photo.
                                               # Still conditional on
                                               # [0x1B83]==0x1E (comm
                                               # option RAM/IO
                                               # confirmed) - reason
                                               # for that gate not
                                               # understood, possibly
                                               # the timebase divider
                                               # under test needs the
                                               # comm board's EXT CLK
                                               # circuitry; scans
                                               # update_menu_position
                                               # over range 0-0x18 (24
                                               # positions) via its step
                                               # helper, step_tb_
                                               # divider_test (0xE255E)
    0xE255E: "step_tb_divider_test",           # per-position worker
                                               # for selftest_tb_
                                               # divider: converts the
                                               # scanned position into
                                               # a shifted power-of-
                                               # ~2/~16 threshold mask
                                               # (0xFFF shifted left or
                                               # right depending on
                                               # position vs. 0xC) and
                                               # calls verify_timebase_
                                               # prc with it
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
    0xF830E: "read_acq_sample_with_wrap",     # (far_ptr record, type_idx,
                                               # position) - resolves
                                               # `position` against the
                                               # channel's circular
                                               # acquisition buffer (bound
                                               # [0x2A], end ptr
                                               # [record+0x68]),
                                               # special-casing type_idx==0
                                               # and ==4/count==8; on
                                               # out-of-[0,0x2000] range
                                               # sets clip flag [0x3B0] and
                                               # returns 0; otherwise reads
                                               # the byte at the resolved
                                               # offset and, if
                                               # [chan*16+0x18C] bit 0x10
                                               # is set, packs it as a 4-
                                               # bit nibble (matches the
                                               # nibble-packing step also
                                               # seen leading into
                                               # convert_sample_value) -
                                               # called 4x in a row from
                                               # 0xF4186 area computing
                                               # position(idx+1) -
                                               # position(idx) deltas for
                                               # adjacent-sample line
                                               # drawing
    0x97905: "parse_next_gpib_command_byte",  # (comm ROM) - the GPIB
                                               # command-byte-stream
                                               # cursor: on first entry
                                               # ([0x61C]==0) resets the
                                               # read pointer [0x582]/
                                               # [0x584] to the start of
                                               # the command buffer
                                               # [0x586]; otherwise
                                               # advances it past the
                                               # current byte using the
                                               # confirmed device-address
                                               # table [0x712] to look up
                                               # a per-device flag first;
                                               # scans forward to the next
                                               # non-NUL byte, stores it in
                                               # [0x580]/[0x581], and (bit
                                               # 0x80 clear, i.e. not a
                                               # GPIB address byte) looks
                                               # up its data byte via
                                               # [0x712][byte*4+1] into
                                               # [0x57F]; when [0x629] is
                                               # set (an RS-232/GPIB comm-
                                               # mode strap) also folds a
                                               # bit from [0] into that
                                               # byte and writes it plus a
                                               # tag (0x98) to a UART-like
                                               # register pair via far ptr
                                               # [0x6D6] - called
                                               # unconditionally as the
                                               # first step of
                                               # process_gpib_command_byte
    0xE5676: "draw_selftest_report_frame",    # (far ptr region) - draws
                                               # 2 nested box outlines via
                                               # draw_box_outline (at
                                               # y=0x19,h=0xe1 with two
                                               # different x/w pairs),
                                               # bracketed by
                                               # init_print_region_home/
                                               # close_print_record/
                                               # init_print_region -
                                               # called exactly once,
                                               # latched by [0x1AFD],
                                               # from report_screen_mode's
                                               # mode==4 case - the self-
                                               # test report screen's
                                               # frame/border
    0xFBD86: "clear_attr_bits_at_prev_delimiter", # (far_ptr buf, start,
                                               # mode) - scans backward
                                               # from `start` through the
                                               # readout text buffer
                                               # [0x1C80] for a 0/0xFF
                                               # delimiter byte, using a
                                               # byte/word/dword stride
                                               # selected by `mode`
                                               # (0/1/else); once found,
                                               # clears bits 0-1 of the
                                               # paired attribute-plane
                                               # byte (same offset, ES=
                                               # DS+0x800) if bit 0 was
                                               # set - called twice from
                                               # 0xFB935/0xFB95A, once
                                               # scanning the whole buffer
                                               # (0x2000) and once a
                                               # specific item's end
                                               # position from the
                                               # [0x1C94] item table
    0xFBCF3: "compute_readout_buffer_length_and_flag", # computes the
                                               # linear-address delta
                                               # between far pointers
                                               # [0x1C84] and [0x1C80]
                                               # (via seg_off_to_linear
                                               # x2) into [0x1C02] - the
                                               # same running length/
                                               # position value read by
                                               # read_acq_sample_with_wrap
                                               # and the "TEKTRONIX" logo
                                               # builder SUB_F5898 -
                                               # builds a far ptr to the
                                               # buffer's current end at
                                               # [0x36]/[0x38], calls
                                               # SUB_FBCCF then sets flag
                                               # bit [0x22]|=0x2000
                                               # (sibling of the 0x1000
                                               # bit set elsewhere in the
                                               # same report-screen flow)
    0xE5EAE: "init_far_pointer_table_sysrom", # the main-ROM counterpart of
                                               # the comm ROM's confirmed
                                               # init_far_pointer_table:
                                               # reads a segment word then
                                               # a table of (dest_offset,
                                               # value_dword) entries -
                                               # terminated by dest==-1 -
                                               # from a fixed location
                                               # (segment 0xE5D1, the same
                                               # code segment
                                               # INT2_HANDLER_EARLY lives
                                               # in - "compiled-in table
                                               # right after the code",
                                               # same shape as the comm ROM
                                               # version) and copies each
                                               # dword to es:[dest] - bulk
                                               # RAM-resident-variable init
    0xE62EE: "run_continuous_selftest_tick",  # called every tick from
                                               # INT255_HANDLER_LATE:
                                               # dispatches to
                                               # verify_prc_readback_pattern
                                               # (if [0x1B5E]) or SUB_FDB8F
                                               # otherwise, storing the
                                               # result in [0x795]; tracks 2
                                               # independent sticky/debounce
                                               # failure counters
                                               # ([0x1A9A] from bit 2,
                                               # [0x1A99] from bits 0-1) and
                                               # a 3rd ([0x1A96], gated by
                                               # [0x1BFA]); on a clean
                                               # result ([0x795]==0) calls
                                               # a hook function pointer at
                                               # [0x740] - the background
                                               # continuous self-test/
                                               # watchdog engine, not the
                                               # foreground
                                               # run_selftest_sequence
    0xE6844: "escalate_acq_timeout_reset",    # the severe half of the
                                               # acquisition-stall
                                               # watchdog: called only when
                                               # the tick counter [0x754]
                                               # exceeds 2x the timeout
                                               # [0x790] (the milder single-
                                               # timeout case just clears
                                               # channel status directly);
                                               # tracks how often this
                                               # escalation fires via
                                               # [0x7B0], latching
                                               # [0x1A9B]=1 permanently
                                               # after >10 escalations,
                                               # then force-clears both
                                               # channels via
                                               # clear_channel1_status/
                                               # clear_channel2_status and
                                               # resets [0x754]
    0xE6884: "compute_acq_channel_scan_counts", # derives 2 base scan-
                                               # count values from the
                                               # acquisition mode
                                               # [0x1B8C] (`(mode-8)/3`
                                               # and `(mode-8)/4`, or a
                                               # fixed 4/3 pair outside
                                               # `[8,0x13]`), then scales
                                               # each per-channel by
                                               # x1/x2/x3 based on
                                               # [0x1B6E] (ch2) and
                                               # [0x1B6A] (ch1) mode
                                               # selectors, storing the
                                               # results into
                                               # [0x7AC]-[0x7AF]
    0xE6D4D: "init_comm_dispatch_table",      # called once from the
                                               # boot sequence right
                                               # after run_selftest_
                                               # sequence: if the comm
                                               # option is installed
                                               # ([0x1BF9]), calls
                                               # comm_rom_boot_init then
                                               # points the far-pointer
                                               # dispatch slots
                                               # [0x738]/[0x73A],
                                               # [0x73C]/[0x73E], and
                                               # [0x740]/[0x742] (the
                                               # hook called by
                                               # run_continuous_selftest_
                                               # tick) into the comm ROM
                                               # (segment 0x96F5); if not
                                               # installed, points all 3
                                               # at main-ROM no-op/stub
                                               # targets in segment
                                               # 0xE6A8 instead - the
                                               # comm-option-present-vs-
                                               # absent dispatch switch
    0xE7238: "init_acq_channel_error_table",  # clears a 12-entry byte
                                               # array at [di+0x79D] plus
                                               # [0x732]/[0x733], and sets
                                               # the "initialized" latch
                                               # [0x734]=1 - one-time setup
                                               # for the acq-channel-error
                                               # tracking used by
                                               # ack_acq_channel_error_flag
    0xE7322: "ack_acq_channel_error_flag",    # critical-section
                                               # (disable_interrupts/
                                               # sync_and_enable_
                                               # interrupts) check-and-
                                               # clear of error flag
                                               # [0x7A5] bit 0: lazily
                                               # sets the same
                                               # "initialized" latch
                                               # [0x734] used by
                                               # init_acq_channel_error_
                                               # table, and on a set flag
                                               # increments the shared
                                               # debounced failure counter
                                               # [0x1A99] (same counter
                                               # run_continuous_selftest_
                                               # tick feeds) before
                                               # clearing [0x7A5]
    0xE783D: "udiv32",                        # the unsigned 32-bit /
                                               # 32-bit -> 32-bit divide
                                               # sdiv32 calls after
                                               # computing operand signs
                                               # and taking absolute
                                               # values - builds a small
                                               # ES:SS stack-overlay scratch
                                               # frame and calls
                                               # udiv32_core to do the
                                               # actual shift-subtract
                                               # division, then reads the
                                               # quotient back out of that
                                               # frame
    0xE788F: "udiv32_core",                   # the restoring-division
                                               # shift-subtract loop
                                               # proper (near `call`, SI:DI
                                               # accumulate the quotient
                                               # bit by bit) - divide-by-
                                               # zero short-circuits to a
                                               # saturated 0xFFFFFFFF
                                               # result; called only from
                                               # udiv32
    0xE7A0A: "print_string_serial_seg",       # (far_str_ptr) - loops
                                               # sending each byte via
                                               # putchar_serial_seg until
                                               # a NUL terminator - the
                                               # putchar_serial_seg
                                               # counterpart of the
                                               # confirmed
                                               # print_string_serial
                                               # (which uses
                                               # serial_tx_buffer_put)
    0xFDB65: "engage_selftest_mode_bit_and_sample", # clears bit 4 and
                                               # sets bit 1 of the shared
                                               # comm self-test control
                                               # byte [0x258], writes it
                                               # to hardware via far ptr
                                               # [0x336] (the same
                                               # register
                                               # sync_status_byte_to_hw
                                               # writes, and one of
                                               # init_selftest_register_
                                               # group's confirmed
                                               # register family), then
                                               # takes one readback
                                               # sample from far ptr
                                               # [0x31E]+0x1000
    0xFDB8F: "poll_comm_probe_until_match",   # the fallback
                                               # run_continuous_selftest_
                                               # tick dispatches to when
                                               # [0x1B5E] is clear: while
                                               # [0x258] bit 0x10 is set,
                                               # loops calling
                                               # ack_comm_hw_status_bits
                                               # and comparing [0x262]
                                               # against a live read of
                                               # the confirmed comm probe
                                               # register (physical
                                               # 0x40377E) until they
                                               # match, then updates
                                               # [0x266]/[0x1A] and gates
                                               # the result on
                                               # [0x1B76] bit 2
    0xFF469: "clear_item_flag_bit_all",       # (which_bit) - clears bit
                                               # 0 (if `which_bit==0`) or
                                               # bit 1 (otherwise) of
                                               # `[i+0x268]` for all 165
                                               # (`0xA5`) entries `i` -
                                               # a bulk per-item flag
                                               # clear, called with
                                               # `di=1` from the boot/
                                               # reinit sequence
    0xF0C2A: "draw_pending_line_segment",     # entered via 5 identical
                                               # far-call sites (always
                                               # with arg 1, unused role
                                               # not confirmed), and also
                                               # falls through internally
                                               # from earlier plot-loop
                                               # code (same family as the
                                               # already-named
                                               # reset_acq_buffers_stub/
                                               # print_and_reset_acq_
                                               # buffers, both odd-opener
                                               # members of this cluster):
                                               # conditionally homes the
                                               # pen via update_plot_
                                               # position (if [0x6C0] bit
                                               # 0x10), always draws via
                                               # plot_line_to to
                                               # [0x6BC]/[0x6BE], then on
                                               # every 3rd segment
                                               # ([0x6C1]%3==2) advances 2
                                               # loop counters and falls
                                               # into reset_plot_home_or_
                                               # acq; every caller checks
                                               # the returned AX as a
                                               # boolean gate, but the
                                               # exact AX semantics
                                               # (falls out of whichever
                                               # path was taken, not an
                                               # explicit `mov ax,`) are
                                               # not confirmed
    0xED9BC: "compute_and_draw_scale_marker", # (value) - clamps
                                               # [0x1BEC] to [0x20,
                                               # 0x1000] against `value`
                                               # (adding or subtracting
                                               # depending on [0x1B82]),
                                               # picks a boundary
                                               # constant (0x400 or
                                               # 0x1000) and a divisor
                                               # shift (>>3, optionally
                                               # mirrored against that
                                               # boundary depending on
                                               # [0x1B70]/[0x1B82]),
                                               # calls SUB_F5D89 to
                                               # build a "/1" or "/4"-
                                               # style label (bytes
                                               # 0x2F/'1'or'4'/0x4B='K')
                                               # into a small record,
                                               # then calls SUB_F6382
                                               # (the confirmed decode-
                                               # anomaly function) with
                                               # far ptr [0x1D00] to
                                               # draw/place it - the
                                               # timebase/frequency
                                               # scale-marker label
                                               # builder for the
                                               # readout graticule
    0xF4150: "compute_and_format_sample_delta_readout", # called from
                                               # write_hw_shift_register:
                                               # computes the delta
                                               # between 2 adjacent
                                               # acquisition samples
                                               # (via read_acq_sample_
                                               # with_wrap) for each of
                                               # 2 records ([bp-0x14]/
                                               # [bp-0x10]), using the
                                               # channel-config bit
                                               # ([0x570]-indexed
                                               # [x+0x18C] bit 0x20/
                                               # 0x80) to pick which;
                                               # negates and flags a
                                               # "-" sign character
                                               # ([0x65E]) unless
                                               # [0x67D] overrides;
                                               # formats the result via
                                               # SUB_ED0AE into a
                                               # readout string, and if
                                               # it changed since the
                                               # cached value [0x684],
                                               # calls SUB_E97DC/
                                               # SUB_E99DF and
                                               # conditionally
                                               # build_comm_status_
                                               # message when the comm
                                               # option is installed -
                                               # a cursor/measurement-
                                               # delta readout updater
    0xEDFFD: "dispatch_item_handler_if_enabled", # takes no stack args -
                                               # reads the current item
                                               # index from global
                                               # [0x464] and looks up
                                               # its record at
                                               # [index*0x10+0x1D10];
                                               # if the record's [+6]
                                               # or [+8] field is
                                               # nonzero and the enable
                                               # byte [0x466] bit 0x80
                                               # is set, calls through
                                               # the handler far
                                               # pointer at [+6] with a
                                               # fixed arg (4) - called
                                               # from tag_position_
                                               # marker_and_dispatch
    0xF5D89: "tag_position_marker_and_dispatch", # called from
                                               # compute_and_draw_scale_
                                               # marker with (far ptr
                                               # record, value): tags 2
                                               # byte fields of the
                                               # record ([+0xC]/[+0x1A])
                                               # with a flag byte, and
                                               # if the record's own 2
                                               # word fields ([0] vs
                                               # [2]) differ, calls
                                               # dispatch_item_handler_
                                               # if_enabled and ORs its
                                               # result into `value`;
                                               # then sets global flags
                                               # [0x548] bit 1/2 from
                                               # `value` bits 0x100/
                                               # 0x200 and returns
                                               # `value`
    0xED7DF: "draw_boot_splash_and_option_icon", # called once from
                                               # the comm ROM's boot
                                               # sequence (0x839E3):
                                               # calls SUB_F5898 (the
                                               # "TEKTRONIX" boot-
                                               # splash stroke-data
                                               # builder) to draw the
                                               # logo, then checks a
                                               # far-pointer flag byte
                                               # ([bp+0xA]); if set,
                                               # clears it and computes
                                               # a screen position from
                                               # [0x3C2]/[0x3C6] (column/
                                               # row-like scaling by
                                               # 0x80/0x32) to draw a
                                               # second graphic element
                                               # via SUB_F6382 (the same
                                               # drawing primitive
                                               # compute_and_draw_scale_
                                               # marker uses) - likely
                                               # an installed-option
                                               # indicator icon on the
                                               # boot splash screen
    0xE804F: "update_indexed_value_if_changed", # compares a table
                                               # entry at `es:[bx+si]`
                                               # against `dx`; if equal,
                                               # returns immediately -
                                               # else writes `dx` into
                                               # the per-index table at
                                               # far ptr `[0x1D1C]`,
                                               # marks a "changed" byte
                                               # at `[+4]` of that same
                                               # entry, and sets global
                                               # flag `[0x532]=1`.
                                               # Called right alongside
                                               # `SUB_E8E29` from 2
                                               # parallel code paths
                                               # that both go on to
                                               # update the plot-
                                               # position cache
                                               # `[0x6B2]`/`[0x6B4]` -
                                               # the simpler ("just set
                                               # it") half of that
                                               # update, vs.
                                               # `SUB_E8E29`'s more
                                               # elaborate bounds-
                                               # checked version
    0xEEA58: "clear_readout_attrs_for_item",  # (item, count) - reads
                                               # a far ptr base from
                                               # [0x36] plus a per-item
                                               # offset table
                                               # ([bx+0x3C], bx=
                                               # count<<2), picks a
                                               # clear_attr_bits_at_
                                               # prev_delimiter mode
                                               # (0/1/2) from [0x1CC]
                                               # bits 0x10/0x20 (gated
                                               # by [0x233] bits 1/2
                                               # and the `item`
                                               # argument), then calls
                                               # clear_attr_bits_at_
                                               # prev_delimiter with
                                               # that mode, [0x1D4],
                                               # and the computed
                                               # position - a sibling
                                               # of clear_readout_
                                               # attrs_and_flag_dirty
                                               # for this ROM
    0xF7603: "apply_pending_position_delta",  # reads a signed pending
                                               # adjustment from
                                               # [0x52C] (always reset
                                               # to 0 once consumed -
                                               # a one-shot "apply this"
                                               # flag, likely fed by a
                                               # front-panel encoder/
                                               # knob), clamps the
                                               # resulting position to
                                               # [0,0xFFF], updates a
                                               # stored position value
                                               # via far ptr [bp-0xC]
                                               # and computes the
                                               # delta; if the position
                                               # actually moved, flags
                                               # bit 0x100 of
                                               # [([0x570]<<1)+0x550] -
                                               # same per-item flag
                                               # table and channel/item
                                               # index [0x570] that
                                               # sync_shift_register_
                                               # output uses - the
                                               # scale-position-
                                               # adjustment half of that
                                               # same subsystem
    0xF750A: "sync_shift_register_output",    # a secondary entry point
                                               # into update_display_
                                               # mode_flags (jumps
                                               # backward into its
                                               # L_F7504 scan loop and
                                               # shares its [bp-8]
                                               # "changed flags" local -
                                               # found via a systematic
                                               # loop-vs-function pass,
                                               # see disasm/
                                               # analyze_loops_vs_
                                               # functions.py): scans a
                                               # per-item flag byte pair
                                               # ([idx+0x1BE2]/
                                               # [idx+0x1BE3] bit 0) for
                                               # the first "not yet
                                               # synced" index, computes
                                               # a shift count into
                                               # [0x570], and calls
                                               # write_hw_shift_register
                                               # - the same per-item
                                               # flag bytes SUB_EFB64/
                                               # SUB_EFBA5's bit-1 checks
                                               # also use, consistent
                                               # with a multi-bit per-
                                               # output-line enable byte
                                               # for the shift-register-
                                               # driven hardware (AUX
                                               # pen-lift relay/X-Y
                                               # candidate per
                                               # HARDWARE.md)
    0xFB8E8: "clear_readout_attrs_and_flag_dirty", # (skip_clear) -
                                               # unless `skip_clear!=0`,
                                               # picks a mode (0/1/2)
                                               # from `[0x18C]` bits
                                               # `0x10`/`0x20` and calls
                                               # clear_attr_bits_at_prev_
                                               # delimiter twice - once
                                               # over the whole readout
                                               # buffer (`0x2000`), once
                                               # up to a specific item's
                                               # end position from the
                                               # `[0x1C94]` item table;
                                               # either way, unconditionally
                                               # ORs `[0x22]|=0x1000` (the
                                               # sibling of the `0x2000`
                                               # bit `compute_readout_
                                               # buffer_length_and_flag`
                                               # sets) to flag the
                                               # readout content changed
    0xFBCCF: "init_default_print_cell_dimensions", # writes 5 fixed
                                               # byte constants to
                                               # [0x30]-[0x34] (9, 0xA,
                                               # 0x14, 0x10, 0xA) -
                                               # [0x34] specifically is
                                               # read as a per-row
                                               # `memcpy_far` stride
                                               # (`*0xA`) by the print-
                                               # record character-row
                                               # copier near `0xEF3D6`-
                                               # `0xEF440`, consistent
                                               # with these being
                                               # default print-record
                                               # cell width/height
                                               # fields; called from
                                               # compute_readout_buffer_
                                               # length_and_flag right
                                               # before it sets up the
                                               # far ptr [0x36]/[0x38]
    0xF8234: "load_print_record_templates",   # copies 2 fixed 0xAA
                                               # (170)-byte compiled-in
                                               # template blocks (from
                                               # [0x1C9C]/[0x1CA0]) via
                                               # memcpy_far into the
                                               # scratch far-pointer
                                               # slots [0x1E8]/[0x1EC] -
                                               # loads the source
                                               # templates that
                                               # build_print_record_
                                               # sequence's
                                               # build_print_record_3532
                                               # calls read from
    0xF80A6: "build_print_record_sequence",   # builds a row of print/
                                               # plot-job records via
                                               # repeated
                                               # build_print_record_3532
                                               # calls, each reading from
                                               # a growing offset into
                                               # the templates loaded by
                                               # load_print_record_
                                               # templates ([0x1E8]+0,
                                               # +0x22, ...) and writing
                                               # to a growing offset into
                                               # the destination array
                                               # at [0x1DDC]+0x5A
                                               # (+9 per record)
    0xFBBE9: "memset_word_pattern_far",       # (far_ptr dest, byte_count,
                                               # fill_lo, fill_hi) - word-
                                               # granularity sibling of
                                               # the confirmed memset_far:
                                               # `rep stosw` filling
                                               # `byte_count/2` words with
                                               # the packed `fill_hi:
                                               # fill_lo` byte pair -
                                               # called from SUB_F911C to
                                               # pad the readout buffer's
                                               # tail with a repeating
                                               # `0x0101` pattern
    0xF911C: "pad_readout_buffer_remainder",  # computes 2 far pointers
                                               # relative to the readout
                                               # buffer base [0x1C80]
                                               # (+0x4851 and -0x37AF)
                                               # into fixed scratch vars
                                               # [0x1E8]/[0x1EA] and
                                               # [0x1EC]/[0x1EE], calls
                                               # load_print_record_
                                               # templates/build_print_
                                               # record_sequence, then
                                               # memset_word_pattern_far's
                                               # 0x4800 bytes from the
                                               # buffer's current end
                                               # (far ptr [0x36], set by
                                               # compute_readout_buffer_
                                               # length_and_flag) with a
                                               # 0x0101 fill pattern -
                                               # called only from
                                               # compute_readout_buffer_
                                               # length_and_flag
    0xE7F03: "poll_error_flag_and_reset_channels", # checks status byte
                                               # [0x403]; while set,
                                               # increments a tick
                                               # counter [0x728] and once
                                               # it exceeds 500 (0x1F4)
                                               # force-clears both
                                               # channels via
                                               # clear_channel1_status/
                                               # clear_channel2_status and
                                               # resets the counter;
                                               # returns whether [0x403]
                                               # was set - another
                                               # persistent-error watchdog
                                               # in the same family as
                                               # escalate_acq_timeout_reset
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
