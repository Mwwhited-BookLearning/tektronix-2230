"""
Textual-based full-screen dashboard for the Tek 2230 firmware emulator
- a live registers panel, a scrolling trace/diagnostic/serial log, and
a command input, all built on the same `debugger_core.Debugger` engine
`interactive.py`'s REPL uses. Genuinely cross-platform: Textual has its
own native Windows Terminal/PowerShell driver (unlike plain `curses`,
which needs a separate `windows-curses` shim on Windows and still has
rough edges there) - see `requirements.txt`/`tui.bat`.

Not a replacement for `interactive.py` - both stay available. Use this
one when a live, always-visible dashboard is more useful than a
scrollback prompt; use the REPL for quick scripted sequences (it also
works when piped, which a Textual app cannot).

Usage:
    python tui.py [--revision 13|14] [--tick-interval N]

Commands typed into the input bar are identical to interactive.py's -
see debugger_core.HELP (bound to F1) for the full list. A few common
actions also have direct key bindings (step/run/continue/trace/quit)
so you don't have to type them.
"""
import argparse

from rich.text import Text
from textual.app import App, ComposeResult
from textual.containers import Horizontal
from textual.widgets import Header, Footer, Static, RichLog, Input

from debugger_core import Debugger, HELP, QuitRequested, dispatch_command


class Tek2230App(App):
    CSS = """
    #main { height: 1fr; }
    #registers {
        width: 44;
        border: solid $accent;
        padding: 1 2;
    }
    #log {
        border: solid $accent;
    }
    Input {
        dock: bottom;
    }
    """

    BINDINGS = [
        ("f1", "show_help", "Help"),
        ("f2", "do_step", "Step"),
        ("f3", "do_run", "Run"),
        ("f4", "do_continue", "Continue"),
        ("f5", "toggle_trace", "Trace on/off"),
        ("ctrl+q", "quit_app", "Quit"),
    ]

    def __init__(self, args):
        super().__init__()
        self.args = args
        self.dbg = None

    def compose(self) -> ComposeResult:
        yield Header(show_clock=True)
        with Horizontal(id="main"):
            yield Static(id="registers")
            yield RichLog(id="log", wrap=True, highlight=False, markup=False, max_lines=5000)
        yield Input(placeholder="command (F1 for help) - e.g. step 10, run, "
                                 "continue, press MENU, serial ID?\\n ...",
                    id="cmdline")
        yield Footer()

    def on_mount(self):
        self.title = "Tek 2230 Emulator"
        self.dbg = Debugger(self.args, output=self._sink_from_worker)
        self._append_log("Tek 2230 TUI debugger. Type a command below "
                          "or press F1 for the full list.")
        self.refresh_registers()
        self.query_one(Input).focus()

    # ---- log/registers rendering -----------------------------------

    def _append_log(self, text):
        """Only ever called on the app's own thread/event loop."""
        self.query_one("#log", RichLog).write(Text.from_ansi(text))

    def _sink_from_worker(self, text):
        """`Debugger`'s `output` callable - always invoked from inside
        a worker thread in this app (see `_run_command`), since every
        command, including instant ones, is dispatched through a
        worker for a uniform threading model. `call_from_thread` would
        raise if called from the app's own thread, so this function
        must never be used as a direct, non-worker call site."""
        self.call_from_thread(self._append_log, text)

    def refresh_registers(self):
        s = self.dbg.snapshot()
        r = s["regs"]
        text = Text()
        text.append(f"instrs: {s['count']}\n", style="bold")
        text.append(f"CS:IP:  {s['cs']:04X}:{s['ip']:04X}\n")
        text.append(f"phys:   0x{s['phys']:06X}\n")
        text.append(f"FLAGS:  {s['flags']}\n\n")
        text.append(f"AX={r['AX']:04X}   BX={r['BX']:04X}\n")
        text.append(f"CX={r['CX']:04X}   DX={r['DX']:04X}\n")
        text.append(f"SI={r['SI']:04X}   DI={r['DI']:04X}\n")
        text.append(f"BP={r['BP']:04X}   SP={r['SP']:04X}\n")
        text.append(f"DS={r['DS']:04X}   ES={r['ES']:04X}   SS={r['SS']:04X}\n\n")
        text.append("next:\n", style="bold")
        text.append(f"  {s['instruction']}\n\n")
        text.append("front panel:\n", style="bold")
        text.append(f"  {s['front_panel']}\n\n")
        text.append("uart:\n", style="bold")
        text.append(f"  {s['uart']}\n")
        self.query_one("#registers", Static).update(text)
        if s["stop_reason"]:
            self._append_log(f"[stopped: {s['stop_reason']}]")

    # ---- command dispatch, always off the main thread ---------------

    def _run_command(self, line):
        """Runs in a worker thread so a long `run`/`continue` never
        freezes the UI. Every command goes through this same path
        (not just step/run) for one uniform threading model - trivial
        commands just finish almost instantly inside the worker."""
        try:
            for out_line in dispatch_command(self.dbg, line):
                self.call_from_thread(self._append_log, out_line)
        except QuitRequested:
            self.call_from_thread(self.exit)
            return
        except ValueError as e:
            self.call_from_thread(self._append_log, str(e))
        self.call_from_thread(self.refresh_registers)

    def on_input_submitted(self, event: Input.Submitted):
        line = event.value.strip()
        event.input.value = ""
        if not line:
            return
        self._append_log(f"(tek2230) {line}")
        self.run_worker(lambda: self._run_command(line), thread=True, exclusive=True)

    # ---- key-bound shortcuts for the most common actions ------------

    def action_do_step(self):
        self.run_worker(lambda: self._run_command("step"), thread=True, exclusive=True)

    def action_do_run(self):
        self.run_worker(lambda: self._run_command("run"), thread=True, exclusive=True)

    def action_do_continue(self):
        self.run_worker(lambda: self._run_command("continue"), thread=True, exclusive=True)

    def action_toggle_trace(self):
        cmd = "trace off" if self.dbg and self.dbg.trace else "trace on"
        self.run_worker(lambda: self._run_command(cmd), thread=True, exclusive=True)

    def action_show_help(self):
        for line in HELP.splitlines():
            self._append_log(line)

    def action_quit_app(self):
        self.exit()


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
                          "project's real physical test units")
    ap.add_argument("--continue-length", type=int, default=25000000,
                     help="instruction count `continue`/`c` runs for "
                          "when called with no explicit argument "
                          "(default 25000000)")
    args = ap.parse_args()

    app = Tek2230App(args)
    app.run()


if __name__ == "__main__":
    main()
