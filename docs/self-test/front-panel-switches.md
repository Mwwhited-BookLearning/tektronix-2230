# Self-test: front-panel switch bit validation

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## `[0x758]` bit-level validated as `SWB2` by comparing code structure to the named bits

Prompted directly: rather than just matching *addresses* to the
service manual's chip designators, check whether the *code's own
bit-test structure* matches the *semantic meaning* of the named bits at
that address - a stronger form of validation than an address match
alone.

First, resolved a notation trap: this project's own shorthand
`0x403FFA`/`0x403FFB` for `[0x758]`/`[0x759]`'s source addresses means
"segment `0x4000`, offset `0x3FFA`/`0x3FFB`" (i.e. physical `0x40000+
0x3FFA` = `0x43FFA`), **not** the literal 6-hex-digit number `0x403FFA`
(which would be outside the 8088's 20-bit address space entirely).
Written correctly, `[0x758]`'s source is `0x43FFA` = **`U9301`** - and
the service manual's Table 6-16 ("Display Format") names `U9301`'s
signal as **`SWB2`**, with `U9302` (`0x43FFB`, `[0x759]`) as `SWB1`.

Got the actual page image for Table 6-17 ("Display Format Bit
Definitions") rather than relying on garbled OCR text, and read
`SWB2`'s bit map precisely (bit numbering 1-8 in the manual = bit0-7
here): bit0=`MEM 3`, bit1=`MEM 1`, bit2=`POS/SEL`, bit3=`1K/4K`,
bit4=`MENU`, bit5=`MEM 2`, bit6=`MENU ADV`, bit7=`SELECT C1/C2`.

**The validation**: the self-test report-display loop (`run_selftest_
sequence`'s neighborhood, first checked at `0xE3B6A` and repeated 7
more times in the same function) masks `[0x758] & 0x63` as one
condition and separately checks `[0x758] & 0x80` as another. `0x63` =
binary `01100011` = bits 0, 1, 5, 6 set = **exactly `MEM 3` + `MEM 1` +
`MEM 2` + `MENU ADV`** - i.e. all 4 of the buttons the operator's
manual identifies as the menu-navigation controls (the 3 repurposed
`Menu Select`/`DISPLAY ON/OFF` buttons plus `SAVE REF/►`, see
`HARDWARE.md`'s "Menu navigation control scheme"). The separate `0x80`
check is exactly `SELECT C1/C2` - matching the service manual's own
plainly-stated behavior: the maintenance chapter documents that
holding that button during a running exerciser test causes the test
to loop on the first error. Both the code's bit grouping and its
behavior match the named switches precisely - not just a coincidental
address hit. This is a strong confirmation of the whole `U9301`/
`U9302` = `SWB2`/`SWB1` identification, and pins down `[0x758]`'s bit
map exactly (see `VARIABLES.md`). `[0x759]`/`SWB1`'s bit map is
transcribed too, but has no independent code-level validation yet
since nothing was found reading it back with a fixed literal address.
