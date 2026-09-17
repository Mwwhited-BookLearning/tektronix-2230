"""
Interactive REPL debugger for the Tek 2230 firmware emulator - shows
live register/flag state and the current instruction after every stop,
and lets you single-step, set breakpoints, and simulate front-panel
button presses or incoming serial bytes while the firmware runs.

Built on `debugger_core.py` (shared with `tui.py`, the Textual-based
dashboard alternative) and the same memory map/stubs as `emu.py` (the
headless batch tracer) - use `emu.py` for long, unattended runs (much
faster, no per-step Python round-trip), this one for interactive
exploration via a classic scrollback prompt, or `tui.py` for a live
full-screen dashboard view (needs `textual` installed - see
`requirements.txt`/`tui.bat`).

Usage:
    python interactive.py [--revision 13|14] [--tick-interval N]

Type `help` at the `(tek2230)` prompt for the command list.
"""
import argparse

from debugger_core import Debugger, HELP, QuitRequested, dispatch_command


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                  formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--revision", default="14", choices=["13", "14"])
    ap.add_argument("--tick-interval", type=int, default=2000,
                     help="instructions between synthetic INT2 ticks, "
                          "0 disables the scheduler timer (default 2000)")
    ap.add_argument("--comm-installed", action=argparse.BooleanOptionalAction,
                     default=True,
                     help="stub detect_comm_option_hw's presence probe "
                          "as 'installed' - on by default, matching this "
                          "project's real physical test units; pass "
                          "--no-comm-installed for the 'not installed' "
                          "behavior - see io_stubs.CommPresenceProbe")
    ap.add_argument("--continue-length", type=int, default=25000000,
                     help="instruction count `continue`/`c` runs for "
                          "when called with no explicit argument "
                          "(default 25000000); `continue <n>` also "
                          "updates this for the rest of the session")
    args = ap.parse_args()

    dbg = Debugger(args, output=print)
    print("Tek 2230 interactive emulator debugger. Type `help` for commands.")
    for line in dbg.status_lines():
        print(line)

    while True:
        try:
            line = input("\n(tek2230) ").strip()
        except EOFError:
            print()
            break
        if not line:
            continue
        try:
            for out_line in dispatch_command(dbg, line):
                print(out_line)
        except QuitRequested:
            break
        except ValueError as e:
            print(e)


if __name__ == "__main__":
    main()
