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
from textual.containers import Horizontal, Vertical, Grid
from textual.widgets import Header, Footer, Static, RichLog, Input, Checkbox, Select

from debugger_core import Debugger, HELP, QuitRequested, dispatch_command
from io_stubs import InteractiveFrontPanel

# The 2 active-low buttons form one mutually-exclusive 3-state group
# (real hardware: pressing one physically releases the other) - shown
# as a single dropdown instead of 2 independent checkboxes that could
# otherwise represent an impossible state (both pressed at once).
HORIZONTAL_MODE_BUTTONS = ("A_ONLY", "B_ONLY")
# Every other button is a genuine independent momentary toggle.
CHECKBOX_BUTTONS = [name for name in InteractiveFrontPanel.BUTTONS
                    if name not in HORIZONTAL_MODE_BUTTONS]


class Tek2230App(App):
    CSS = """
    #root { height: 1fr; }
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
    #incoming {
        border: solid $accent;
        border-title-align: center;
        height: 4;
        padding: 0 1;
    }
    #outgoing {
        border: solid $accent;
        border-title-align: center;
    }
    #log {
        border: solid $accent;
        border-title-align: center;
    }
    #front-panel {
        border: solid $accent;
        border-title-align: center;
        height: 30%;
        overflow-y: auto;
        grid-size: 8;
        grid-gutter: 0 2;
        padding: 1 1;
    }
    #horizontal-mode {
        column-span: 8;
        margin-bottom: 1;
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
        # `Input` has no built-in binding for these (checked directly:
        # its own BINDINGS list has left/right/home/end/delete/etc. but
        # nothing for up/down), so adding them here doesn't shadow any
        # existing behavior - they only fire when nothing more specific
        # (e.g. the front-panel Select's own up/down) claims the key
        # first, i.e. exactly when the command Input has focus.
        ("up", "history_prev", "Prev command"),
        ("down", "history_next", "Next command"),
    ]

    def __init__(self, args):
        super().__init__()
        self.args = args
        self._command_history = []
        self._history_index = None  # None = not currently navigating
        self._history_draft = ""    # unsent text saved when history nav starts
        self.dbg = None
        self._busy = False
        self._outgoing_buffer = []
        self._suppress_panel_events = False  # see _sync_front_panel_widgets

    def compose(self) -> ComposeResult:
        yield Header(show_clock=True)
        with Vertical(id="root"):
            with Horizontal(id="main"):
                with Vertical(id="side"):
                    yield Static(id="registers")
                    yield Static(id="incoming")
                    yield RichLog(id="outgoing", wrap=True, highlight=False, markup=False, max_lines=5000)
                yield RichLog(id="log", wrap=True, highlight=False, markup=False, max_lines=5000)
            with Grid(id="front-panel"):
                yield Select(
                    [("A ONLY", "A_ONLY"), ("BOTH", "BOTH"), ("B ONLY", "B_ONLY")],
                    id="horizontal-mode", allow_blank=False, value="A_ONLY")
                for name in CHECKBOX_BUTTONS:
                    yield Checkbox(name, id=f"btn-{name}")
        yield Input(placeholder="command (F1 for help) - e.g. step 10, run, "
                                 "continue, press MENU, serial ID?\\n ...",
                    id="cmdline")
        yield Footer()

    def on_mount(self):
        self.title = "Tek 2230 Emulator"
        self.query_one("#registers", Static).border_title = "registers"
        self.query_one("#incoming", Static).border_title = "incoming serial (UART RX)"
        self.query_one("#front-panel", Grid).border_title = "front panel"
        self.query_one("#outgoing", RichLog).border_title = "outgoing serial (UART TX)"
        self.query_one("#log", RichLog).border_title = "log"
        self.dbg = Debugger(self.args, output=self._sink_from_worker,
                             on_tx=self._on_tx_from_worker,
                             on_progress=self._on_progress_from_worker)
        self._append_log("Tek 2230 TUI debugger. Type a command below "
                          "or press F1 for the full list.")
        self._sync_front_panel_widgets()
        self.refresh_registers()
        self.query_one(Input).focus()

    # ---- front-panel checkbox/dropdown controls ----------------------

    def _sync_front_panel_widgets(self):
        """Sets each checkbox/the dropdown to match `dbg.front_panel`'s
        actual current state - called once at startup so the controls
        reflect the real idle baseline (`VARIABLES.md`'s live-hardware-
        confirmed values) instead of defaulting to all-unchecked, which
        would misrepresent buttons that are genuinely active at idle.
        Only a startup sync, not continuous - a button pressed/released
        via a typed `press`/`release` command won't retroactively move
        its checkbox; the checkboxes are the intended way to drive the
        panel interactively."""
        self._suppress_panel_events = True
        try:
            fp = self.dbg.front_panel
            for name in CHECKBOX_BUTTONS:
                reg, bit, active_low = InteractiveFrontPanel.BUTTONS[name]
                value = fp.swb2 if reg == "SWB2" else fp.swb1
                bit_set = bool(value & (1 << bit))
                pressed = (not bit_set) if active_low else bit_set
                self.query_one(f"#btn-{name}", Checkbox).value = pressed
            a_bit = bool(fp.swb1 & 1)       # A_ONLY, active-low
            b_bit = bool(fp.swb1 & (1 << 6))  # B_ONLY, active-low
            a_pressed, b_pressed = not a_bit, not b_bit
            mode = "A_ONLY" if a_pressed else "B_ONLY" if b_pressed else "BOTH"
            self.query_one("#horizontal-mode", Select).value = mode
        finally:
            self._suppress_panel_events = False

    def on_checkbox_changed(self, event: Checkbox.Changed):
        if self._suppress_panel_events or self.dbg is None:
            return
        name = event.checkbox.id.removeprefix("btn-")
        self.dbg.front_panel.set_button(name, event.value)
        self._append_log(f"{'pressed' if event.value else 'released'} {name} - "
                          f"{self.dbg.front_panel.status()}")
        self.refresh_registers()

    def on_select_changed(self, event: Select.Changed):
        if self._suppress_panel_events or self.dbg is None or event.select.id != "horizontal-mode":
            return
        mode = event.value
        self.dbg.front_panel.set_button("A_ONLY", mode == "A_ONLY")
        self.dbg.front_panel.set_button("B_ONLY", mode == "B_ONLY")
        self._append_log(f"HORIZONTAL MODE -> {mode} - {self.dbg.front_panel.status()}")
        self.refresh_registers()

    def refresh_incoming(self):
        text = self.dbg.uart.incoming_text()
        self.query_one("#incoming", Static).update(text if text else "(RX queue empty)")

    # ---- log/registers rendering -----------------------------------

    def _append_log(self, text):
        """Only ever called on the app's own thread/event loop."""
        self.query_one("#log", RichLog).write(Text.from_ansi(text))

    def _on_tx_from_worker(self, byte):
        """`Debugger`'s `on_tx` callback - invoked from inside a worker
        thread (see `_run_command`) every time the UART transmits a
        byte, live, not just retrievable on request via `outgoing`."""
        self.call_from_thread(self._append_outgoing_byte, byte)

    def _on_progress_from_worker(self):
        """`Debugger`'s `on_progress` callback - fires periodically
        (wall-clock throttled inside `Debugger`, not here) during any
        step/run/continue, so the registers panel updates live while a
        long run is still in flight instead of only once it stops."""
        self.call_from_thread(self.refresh_registers)

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
        side of this guard.

        **Real bug, found and fixed 2026-09-16**: disabling a focused
        widget in Textual also strips its focus, and it doesn't
        automatically come back when re-enabled - so every command
        after the first one silently went nowhere (Enter was reaching
        no focused widget at all, not the `Input`), confirmed via a
        headless test showing `app.focused` was `None` after the first
        command completed. Fixed by explicitly refocusing the input
        each time it's re-enabled."""
        self._busy = busy
        inp = self.query_one(Input)
        inp.disabled = busy
        self.sub_title = "running... (input disabled until it stops)" if busy else ""
        if not busy:
            inp.focus()

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
        text.append(f"  {s['uart']}\n\n")
        text.append("interrupts:\n", style="bold")
        iv = s["interrupts"]
        text.append(f"  IF={int(iv['if_flag'])}\n")
        text.append(f"  mask latch: 0D={iv['mask_0D_dr']:02X} 1D={iv['mask_1D_tbre']:02X} "
                     f"2D={iv['mask_2D']:02X} 3D={iv['mask_3D_diag']:02X}\n")
        text.append(f"  UART enable: RxEN={int(iv['uart_rxen'])} TxEN={int(iv['uart_txen'])}\n")
        text.append(f"  UART pins:   RxRDY={int(iv['uart_rxrdy'])} TxRDY={int(iv['uart_txrdy'])}\n")
        text.append(f"  UART status: RX_READY={int(iv['uart_rx_ready_bit'])} "
                     f"TX_READY={int(iv['uart_tx_ready_bit'])} "
                     f"TX_EMPTY={int(iv['uart_tx_empty_bit'])}\n")
        if iv["int2_fired"] is not None:
            text.append(f"  INT2: {iv['int2_fired']} fired, {iv['int2_skipped']} skipped\n")
        self.query_one("#registers", Static).update(text)
        self.refresh_incoming()
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
        # Don't record a duplicate of the immediately-preceding command
        # (matches typical shell history behavior - retyping the same
        # command repeatedly shouldn't fill history with N copies of it).
        if not self._command_history or self._command_history[-1] != line:
            self._command_history.append(line)
        self._history_index = None
        self._history_draft = ""
        self._append_log(f"(tek2230) {line}")
        self._launch(line)

    def action_history_prev(self):
        """Up arrow - step to an older command, same convention as a
        shell history (bash/zsh): the first press saves whatever was
        already typed (so Down can restore it later), then walks
        backward from the most recent command."""
        inp = self.query_one(Input)
        if not inp.has_focus or not self._command_history:
            return
        if self._history_index is None:
            self._history_draft = inp.value
            self._history_index = len(self._command_history)
        if self._history_index > 0:
            self._history_index -= 1
            inp.value = self._command_history[self._history_index]
            inp.cursor_position = len(inp.value)

    def action_history_next(self):
        """Down arrow - step to a newer command, or back to whatever
        was being typed before history navigation started once past
        the newest entry."""
        inp = self.query_one(Input)
        if not inp.has_focus or self._history_index is None:
            return
        self._history_index += 1
        if self._history_index >= len(self._command_history):
            inp.value = self._history_draft
            self._history_index = None
        else:
            inp.value = self._command_history[self._history_index]
        inp.cursor_position = len(inp.value)

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
