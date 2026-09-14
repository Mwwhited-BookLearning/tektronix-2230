# Open questions / next steps (historical)

Moved from `disasm/NOTES.md` (which had grown too long to navigate) -
see `docs/README.md` for the full table of contents.

## Open questions / next steps

1. Widen code coverage further. Jump-table dispatch does exist in a
   couple of places (see "Found: indirect jumps/calls through computed
   pointers" above - the task-scheduler and per-item-handler tables),
   but isn't the main lever for growing coverage further - finding more
   entry points nothing in-graph calls directly (interrupt handlers,
   as above, are one source; worth checking for others e.g. timer
   ISRs, or simply reading through `INT1_HANDLER`/`INT255_HANDLER_*`/
   `INT2_HANDLER_LATE` themselves, which may install further vectors).
2. Identify data regions (ASCII string tables like the command-name
   list found in the comm ROM, the acquisition-mode-setup strings in
   3633) within the *reached* code so the listing doesn't try to
   disassemble them as instructions.
3. Figure out the comm ROM (2998)'s actual mapping/bank-switching
   scheme and confirm whether it's driven by the same 8088 or a separate
   coprocessor.
4. Start filling in `functional_name` in the `.symbols.json` files as
   routines are understood (e.g. `SUB_E06B6` looks like a small
   dispatch/switch function keyed on a low nibble — candidate name once
   confirmed: something like `set_mode_from_param` or similar, pending
   more context on the 0x1B50/0x1B51/0x1B18 memory locations it touches).
