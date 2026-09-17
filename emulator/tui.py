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
from textual.containers import Horizontal, Vertical
from textual.widgets import Header, Footer, Static, RichLog, Input

from debugger_core import Debugger, HELP, QuitRequested, dispatch_command


class Tek2230App(App):
    CSS = """
    #main { height: 1fr; }
    #side {
        width: 44;
    }
    #registers {
        border: solid $accent;
        padding: 1 2;
        height: auto;
        max-height: 60%;
    }
    #outgoing {
        border: solid $accent;
        border-title-align: center;
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
        self._busy = False
        self._outgoing_buffer = []

    def compose(self) -> ComposeResult:
        yield Header(show_clock=True)
        with Horizontal(id="main"):
            with Vertical(id="side"):
                yield Static(id="registers")
                yield RichLog(id="outgoing", wrap=True, highlight=False, markup=False, max_lines=5000)
            yield RichLog(id="log", wrap=True, highlight=False, markup=False, max_lines=5000)
        yield Input(placeholder="command (F1 for help) - e.g. step 10, run, "
                                 "continue, press MENU, serial ID?\\n ...",
                    id="cmdline")
        yield Footer()

    def on_mount(self):
        self.title = "Tek 2230 Emulator"
        self.query_one("#outgoing", RichLog).border_title = "outgoing serial (UART TX)"
        self.dbg = Debugger(self.args, output=self._sink_from_worker,
                             on_tx=self._on_tx_from_worker)
        self._append_log("Tek 2230 TUI debugger. Type a command below "
                          "or press F1 for the full list.")
        self.refresh_registers()
        self.query_one(Input).focus()

    # ---- log/registers rendering -----------------------------------

    def _append_log(self, text):
        """Only ever called on the app's own thread/event loop."""
        self.query_one("#log", RichLog).write(Text.from_ansi(text))

    def _on_tx_from_worker(self, byte):
        """`Debugger`'s `on_tx` callback - invoked from inside a worker
        thread (see `_run_command`) every time the UART transmits a
        byte, live, not just retrievable on request via `outgoing`."""
        self.call_from_thread(self._append_outgoing_byte, byte)

    def _append_outgoing_byte(self, byte):
        """Buffers into complete lines before writing, the same way
        `io_stubs.DiagnosticTextCapture` does for the main log - a
        write per byte would put one character per line in the panel
        instead of readable text."""
        ch = chr(byte) if 32 <= byte < 127 else None
        if byte in (0, 13, 10) or ch is None:
            if self._outgoing_buffer:
                self.query_one("#outgoing", RichLog).write("".join(self._outgoing_buffer))
                self._outgoing_buffer = []
        else:
            self._outgoing_buffer.append(ch)

    def _set_busy(self, busy):
        """Only ever called on the app's own thread/event loop (either
        directly, or via `call_from_thread` from a worker's `finally`).
        Disables the input and shows a clear "running" indicator instead
        of the app just appearing frozen during a long `run`/`continue` -
        Unicorn's `emu_start` is a long synchronous C call with no safe
        way to be interrupted from another thread, so a second command
        must never be allowed to start (and try to cancel the first)
        while one is already in flight. See `_launch` for the launch
        side of this guard."""
        self._busy = busy
        self.query_one(Input).disabled = busy
        self.sub_title = "running... (input disabled until it stops)" if busy else ""

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
        commands just finish almost instantly inside the worker. The
        `finally` always clears the busy flag, however the command
        ended, so a crash mid-command can't leave the input
        permanently disabled."""
        try:
            for out_line in dispatch_command(self.dbg, line):
                self.call_from_thread(self._append_log, out_line)
        except QuitRequested:
            self.call_from_thread(self.exit)
            return
        except ValueError as e:
            self.call_from_thread(self._append_log, str(e))
        finally:
            self.call_from_thread(self._set_busy, False)
        self.call_from_thread(self.refresh_registers)

    def _launch(self, line):
        """The only place that starts a command worker - refuses to
        start a second one while one is already running instead of
        Textual's `exclusive=True` (which *cancels* the in-flight
        worker to make room for the new one). That's unsafe here:
        `dbg.run()` is a long synchronous Unicorn C call with no clean
        way to be interrupted from another thread, so canceling it
        mid-flight can crash the interpreter hard enough to skip
        Textual's own terminal cleanup (confirmed 2026-09-16 - pressing
        F4 while a previous `continue` was still running left the
        terminal stuck in raw SGR mouse-tracking mode, printing mouse-
        move escape codes as literal text after the app died)."""
        if self._busy:
            self._append_log("(still running the previous command - wait for it to finish)")
            return
        self._set_busy(True)
        self.run_worker(lambda: self._run_command(line), thread=True)

    def on_input_submitted(self, event: Input.Submitted):
        line = event.value.strip()
        event.input.value = ""
        if not line:
            return
        self._append_log(f"(tek2230) {line}")
        self._launch(line)

    # ---- key-bound shortcuts for the most common actions ------------

    def action_do_step(self):
        self._launch("step")

    def action_do_run(self):
        self._launch("run")

    def action_do_continue(self):
        self._launch("continue")

    def action_toggle_trace(self):
        cmd = "trace off" if self.dbg and self.dbg.trace else "trace on"
        self._launch(cmd)

    def action_show_help(self):
        for line in HELP.splitlines():
            self._append_log(line)

    def action_quit_app(self):
        if self._busy:
            # A worker thread stuck inside a long emu_start() call can't
            # be safely killed either (same reason `_launch` refuses a
            # second command) - Python's default thread-pool executor
            # would just block process exit waiting for it instead.
            # Clearer to say so than to hang with no explanation.
            self._append_log("(still running - can't quit mid-run; wait "
                              "for it to finish, or close the terminal "
                              "window if you need to force it)")
            return
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
