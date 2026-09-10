# Pseudocode reconstructions

Hand-written C reconstructions of the routines in `FUNCTIONS.md` that
are understood well enough to read as code rather than a disassembly
listing. This is *reconstructed*, not recovered original source — no
compiler banner or debug info survives in these ROMs, variable/
parameter names are inferred from usage (see `VARIABLES.md`), and
anything not confirmed is called out in a comment rather than stated
as fact. Cross-reference `disasm/sysrom_3532_3633.lst` for the exact
instructions behind each line if you need to double-check a claim.

Two of these (`memcpy_far`, `check_comm_option_installed`) are fully
understood and the pseudocode should be trustworthy line-for-line. The
other two (`boot_init`, `self_test_dispatcher`) are long and only
partially analyzed — the *structure* is accurate, but several steps
are still open questions, marked as such.

## memcpy_far (0xFBC09, in 160-3532)

Straightforward far-pointer block copy. Called from both main-ROM
halves.

```c
// far ptr dest, far ptr src, unsigned int byte_count
void memcpy_far(void far *dest, void far *src, unsigned int byte_count) {
    unsigned int word_count = byte_count >> 1;   // rep movsw does 2 bytes/iter
    unsigned int odd_byte    = byte_count & 1;    // rep movsb mops up the rest

    // DS:SI = src, ES:DI = dest (the compiler's calling convention
    // passes both as far pointers on the stack; this just loads them)
    _asm {
        push ds
        lds  si, src
        les  di, dest
        cld
        mov  cx, word_count
        rep  movsw
        mov  cx, odd_byte
        rep  movsb
        pop  ds
    }
}
```

The real assembly also saves/restores `CX` and the flags register
around the copy (`push cx`/`pushf` at entry, `popf`/`pop cx` at exit) -
omitted above since that's just this compiler's calling-convention
boilerplate, not part of the copy logic itself.

## check_comm_option_installed (0xE44F1, in 160-3633)

Probes whether the comm/GPIB option board is installed, and separately
whether it provides writable RAM/IO. Takes no parameters — both
addresses it checks are hardcoded.

```c
// Physical 0x80000 is the comm/GPIB option board's fixed address
// window (see MEMORY_MAP.md). Two DIFFERENT words within that window
// get checked - not the same one, despite testing related things.
#define COMM_OPTION_SEGMENT 0x8000
#define ROM_HEADER_OFFSET   0x0004   // bytes 4-5 of the comm ROM's own
                                     // 10-byte self-ID header (see
                                     // disasm/NOTES.md "Confirmed facts")
#define RAM_TEST_OFFSET     0xA004   // ROM_HEADER_OFFSET + 0xA000 - a
                                     // DIFFERENT word elsewhere in the
                                     // same 64KB window. Why exactly
                                     // this offset isn't confirmed -
                                     // see disasm/NOTES.md for the
                                     // "dedicated scratch register"
                                     // hypothesis.

void check_comm_option_installed(void) {
    comm_option_status = 0xFF;  // [0x1BF9]: "not yet determined" sentinel

    unsigned int header_word =
        *(unsigned int far *)MK_FP(COMM_OPTION_SEGMENT, ROM_HEADER_OFFSET);

    comm_option_status = 0;  // default: nothing found

    // every one of these ROMs' self-ID headers uses a byte + its
    // one's-complement, summing to 0xFF - re-derive that here as
    // proof a genuine Tektronix ROM header exists at this address
    unsigned char revision   = header_word & 0xFF;
    unsigned char complement = (header_word >> 8) & 0xFF;
    if ((unsigned char)(revision + complement) == 0xFF) {
        comm_option_status = 1;  // valid header found -> board present

        unsigned int far *test_addr =
            (unsigned int far *)MK_FP(COMM_OPTION_SEGMENT, RAM_TEST_OFFSET);
        unsigned int saved = *test_addr;
        *test_addr = 0xAA55;
        if (*test_addr == 0xAA55 && comm_option_config_byte == 0x1E) {
            comm_option_status |= 2;  // also RAM/IO-backed
        }
        *test_addr = saved;  // restore, since this might be real ROM
                              // (the write silently did nothing) or
                              // real RAM (the write must be undone)
    }
}
```

## boot_init (0xE00B1, in 160-3633) - partial

The CPU's real reset vector far-jumps here. Structurally accurate;
several steps are marked as open questions rather than guessed at.

```c
void boot_init(void) {
    disable_interrupts();  // cli

    // clear one byte of a small RAM buffer at 0x40000:0x07CE
    *(unsigned char far *)MK_FP(0x4000, 0x07CE) = 0;

    unsigned int config_selector = 0;  // dx in the real code - stays 0
                                       // through this function; not
                                       // yet confirmed why it's tracked
                                       // as a variable at all here
    for (;;) {
        setup_stack(0x4000, 0x3FFA);       // ss:sp = 4000:3FFA
        unsigned int return_addr = peek_stack_top();  // pop;push, doesn't
                                                        // consume it - purpose
                                                        // of reading this
                                                        // without using it
                                                        // unconfirmed

        // cs:[bx+0x1AF], bx = config_selector: a per-instance config/
        // option byte read directly out of THIS ROM (not RAM) - so
        // it's baked in at build/programming time, not runtime state
        unsigned char option_byte = code_segment_byte(0x1AF + config_selector);

        if (option_byte == 0xFF) {
            // "not configured" sentinel path
            install_config_word(config_selector, return_addr);  // DS=0x41,
                                                                  // [0x1AE4]
            far_jump(0xE5D1, 0x00B4);  // -> ENTRY_E5D1_B4, continues boot
            return;  // does not come back
        }

        // normal path: index a table of far pointers at
        // cs:[bx*4 + 0x155] / cs:[bx*4 + 0x17D] using config_selector*4
        // - a 4-byte-per-entry table of (something, something) far
        // pointers, indexed per config_selector. Not yet identified
        // what the table actually holds.
        void far *table_ptr_a = code_segment_far_ptr(0x155 + config_selector * 4);
        void far *table_ptr_b = code_segment_far_ptr(0x17D + config_selector * 4) + 1;

        unsigned char option_byte_2 = code_segment_byte(0x1AF + config_selector);
        if ((unsigned int)option_byte_2 > 1) {
            // TODO: fill_pattern_loop(table_ptr_a, table_ptr_b, config_selector,
            //   option_byte selects a fill pattern: 2 -> 0x0F0F,
            //   3 -> 0xF0F0, else -> 0xFFFF)
            // Looks like a RAM/display-memory fill/wipe using a
            // pattern chosen by option_byte, bounded by table_ptr_a/b.
            // Not fully traced - see disasm/sysrom_3532_3633.lst
            // starting at L_E0114 for the exact loop.
            continue;  // loops back to setup_stack() again in the
                       // real code under some conditions - exact
                       // loop-exit condition not yet confirmed
        }
        // else: falls through toward L_E018A (not yet reconstructed)
        break;
    }
}
```

## self_test_dispatcher (0xE416F, in 160-3633) - partial

Called from `0xE07F8`, guarded by `[0x1B10] == 0`. Runs ~25 subsystem
self-tests in sequence and returns an accumulated result. See
`disasm/NOTES.md` "Found: the self-test dispatcher" for the full
address list this summarizes.

```c
unsigned int self_test_dispatcher(void) {
    // Setup calls before the test loop starts - likely clearing/
    // preparing a results-display area, not yet individually traced:
    //   SUB_E3567(some_far_ptr_from(0x1DDC) + 0x20A, 0xFA, 0x339)
    //   SUB_E3930(0, 0, 0)
    //   SUB_E3854(0x20)
    //   ... a few more like this

    unsigned int result = 0;  // [bp-0xA] in the real code

    // ~20 of these - each one folds its subsystem test's return code
    // into `result`, and writes a literal 1 into [0x1B18] (role
    // unconfirmed - maybe a "test in progress" flag, maybe unused-
    // as-an-index despite the name suggesting one). None of these
    // subroutines have been individually identified yet - see
    // FUNCTIONS.md "The ~20 self-test subroutines are NOT yet
    // identified by name" for why (searched for direct references to
    // the diagnostic strings in STRINGS.md; found none).
    result |= subsystem_test_0x374E();
    result |= subsystem_test_0x3821();
    result |= subsystem_test_0x0AF5();
    result |= subsystem_test_0x0AF5();  // called twice
    result |= subsystem_test_0x3F2C();
    result |= subsystem_test_0x3F99();
    result |= subsystem_test_0x2FC8();
    result |= subsystem_test_0x1B16();
    result |= subsystem_test_0x252A();
    result |= subsystem_test_0x0ADD();
    result |= subsystem_test_0x0DCC();
    result |= subsystem_test_0x0E56();
    result |= subsystem_test_0x28FE();
    result |= subsystem_test_0x227E();
    result |= subsystem_test_0x26D6();
    result |= subsystem_test_0x286C();
    result |= subsystem_test_0x2CEC();
    result |= subsystem_test_0x0FD0();

    // NOT folded into `result` like its neighbors - informational,
    // not a pass/fail test (see FUNCTIONS.md)
    check_comm_option_installed();

    result |= subsystem_test_0x16EA();
    result |= subsystem_test_0x1E3E();
    result |= subsystem_test_0x1D28();
    result |= subsystem_test_0x1DB3();
    result |= subsystem_test_0x1E90();
    result |= subsystem_test_0x1F18();

    // A few more calls here (0xE553B x3, 0xE6D2F, 0xE4429 x2) that
    // look like they're outside the main per-subsystem loop - guessed
    // as end-of-sequence cleanup/reporting, not confirmed.

    return result;
}
```

Its caller immediately far-calls another routine (not yet renamed,
see `FUNCTIONS.md`'s `0xE094B` entry) with the result, which writes a
small 3-byte record into a buffer rather than printing anything
directly — the likely reason none of the ~20 test subroutines
reference a diagnostic message string themselves.
