"""
Tkinter-based full-screen dashboard for the Tek 2230 firmware emulator
- a live registers panel, a scrolling trace/diagnostic/serial log, a
vector-display canvas showing what the firmware is actually plotting
on-screen, and a command input, all built on the same `debugger_core.
Debugger` engine `interactive.py`'s REPL uses.

**Converted from a Textual TUI, user request 2026-09-22**: "can you
replace the TUI with some other GUI such as a webform or winform that
can have a canvas that can display the vector buffer?" - followed by a
factual question about Tkinter's `Canvas` (a native vector/retained-
mode drawing surface, `create_line` et al, plus the full `ttk` widget
toolkit) vs. Pygame (pixel blitter, no built-in widgets), then the
explicit instruction "convert to tkinter". `tkinter` is part of the
Python standard library (no new `requirements.txt` entry, unlike the
`textual` dependency this replaces) - see `tui.bat`.

**Threading model**: same underlying hazard Textual's version already
documented (`emu_start()` is a long synchronous Unicorn C call with no
safe way to be interrupted from another thread, so a long `run`/
`continue` runs on a worker thread while the input stays responsive) -
but Tkinter, unlike Textual, has no `call_from_thread` equivalent:
touching a widget from any thread but the main one is simply
unsupported and corrupts the UI. Every worker thread here only ever
puts a zero-arg callable onto `self._ui_queue`; `_drain_ui_queue`,
scheduled via `root.after` on the main thread, is the only code that
actually calls widget methods on the worker's behalf. `live` mode's
own action-queueing (`Debugger.queue_action`) is separate from this -
that one marshals debugger/engine-state changes onto `run_live`'s own
thread, between bursts; this one marshals UI updates onto Tkinter's
main thread. They compose (see `_run_queued`): a live-mode command's
*output* still has to go through `_ui_queue` to reach the log widget
safely, even though the command itself already reached the right
thread via `queue_action`.

Not a replacement for `interactive.py` - both stay available. Use this
one when a live, always-visible dashboard (and the vector display) is
more useful than a scrollback prompt; use the REPL for quick scripted
sequences (it also works when piped, which this cannot).

Usage:
    python tui.py [--revision 13|14] [--tick-interval N]

Commands typed into the input bar are identical to interactive.py's -
see debugger_core.HELP (F1) for the full list. Common actions also
have direct key bindings (F2-F7, Ctrl+Q, Up/Down history) and toolbar
buttons.
"""
import argparse
import queue
import threading
import tkinter as tk
from tkinter import ttk, scrolledtext

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

MONO_FONT = ("Consolas", 10)

# Vector canvas sizing/scaling. Raw plotted coordinates (`io_stubs.
# VectorDisplay`) live in an unscaled ~0-1023 range (10-bit, the
# firmware's own `*4`-scaled internal value divided back out) - see
# that class's docstring. `SCALE` maps that range onto a fixed-size
# square canvas.
VECTOR_CANVAS_SIZE = 520
VECTOR_COORD_RANGE = 1024
VECTOR_SCALE = VECTOR_CANVAS_SIZE / VECTOR_COORD_RANGE


class Tek2230App:
    def __init__(self, args):
        self.args = args
        self.dbg = None
        self._busy = False
        self._live = False
        self._outgoing_buffer = []
        self._command_history = []
        self._history_index = None
        self._history_draft = ""
        self._vector_last_total = 0  # see VectorDisplay.total / _redraw_vector
        self._vector_size = VECTOR_CANVAS_SIZE  # current on-screen side length (kept square)
        self._vector_scale = VECTOR_SCALE  # self._vector_size / VECTOR_COORD_RANGE

        # See module docstring: workers only ever put() a zero-arg
        # callable here; `_drain_ui_queue` (main-thread only) is what
        # actually runs it.
        self._ui_queue = queue.Queue()

        self.root = tk.Tk()
        self.root.title("Tek 2230 Emulator")
        self.root.geometry("1400x900")
        self.root.protocol("WM_DELETE_WINDOW", self.action_quit_app)

        self._build_ui()
        self._bind_keys()

        self.dbg = Debugger(self.args, output=self._sink_from_worker,
                             on_tx=self._on_tx_from_worker,
                             on_progress=self._on_progress_from_worker)
        self._append_log("Tek 2230 Tkinter debugger. Type a command below, "
                          "press F1 for the full list, or use the toolbar.")
        self._sync_front_panel_widgets()
        self.refresh_registers()
        self.cmd_entry.focus_set()

        self.root.after(50, self._drain_ui_queue)

    # ---- UI construction ---------------------------------------------

    def _build_ui(self):
        self.status_var = tk.StringVar(value="")
        ttk.Label(self.root, textvariable=self.status_var,
                  anchor="w", padding=(6, 2)).pack(side="top", fill="x")

        self._build_toolbar()

        main = ttk.PanedWindow(self.root, orient="horizontal")
        main.pack(side="top", fill="both", expand=True)

        side = ttk.Frame(main)
        main.add(side, weight=1)
        self._build_side_panel(side)

        center = ttk.Frame(main)
        main.add(center, weight=2)
        self._build_vector_canvas(center)

        log_frame = ttk.Frame(main)
        main.add(log_frame, weight=2)
        self._build_log_panel(log_frame)

        controls = ttk.Frame(self.root)
        controls.pack(side="top", fill="x")
        self._build_front_panel(controls)
        self._build_dip_switches(controls)

        cmd_frame = ttk.Frame(self.root)
        cmd_frame.pack(side="bottom", fill="x", padx=4, pady=4)
        ttk.Label(cmd_frame, text="cmd:").pack(side="left")
        self.cmd_entry = ttk.Entry(cmd_frame, font=MONO_FONT)
        self.cmd_entry.pack(side="left", fill="x", expand=True, padx=(4, 0))
        self.cmd_entry.bind("<Return>", self._on_command_submitted)
        self.cmd_entry.bind("<Up>", self._on_history_prev)
        self.cmd_entry.bind("<Down>", self._on_history_next)

    def _build_toolbar(self):
        bar = ttk.Frame(self.root, padding=(4, 2))
        bar.pack(side="top", fill="x")
        buttons = [
            ("Help (F1)", self.action_show_help),
            ("Step (F2)", self.action_do_step),
            ("Run (F3)", self.action_do_run),
            ("Continue (F4)", self.action_do_continue),
            ("Trace (F5)", self.action_toggle_trace),
            ("Live (F6)", self.action_toggle_live),
            ("Reset (F7)", self._do_reset),
            ("Quit (Ctrl+Q)", self.action_quit_app),
        ]
        for label, cmd in buttons:
            ttk.Button(bar, text=label, command=cmd).pack(side="left", padx=2)

    def _build_side_panel(self, parent):
        reg_frame = ttk.LabelFrame(parent, text="registers")
        reg_frame.pack(side="top", fill="both", expand=True, padx=2, pady=2)
        self.registers_text = tk.Text(reg_frame, font=MONO_FONT, height=20,
                                       wrap="none", state="disabled")
        self.registers_text.pack(side="top", fill="both", expand=True)

        in_frame = ttk.LabelFrame(parent, text="incoming serial (UART RX)")
        in_frame.pack(side="top", fill="x", padx=2, pady=2)
        self.incoming_var = tk.StringVar(value="(RX queue empty)")
        ttk.Label(in_frame, textvariable=self.incoming_var,
                  font=MONO_FONT, wraplength=380).pack(side="top", fill="x",
                                                        padx=4, pady=4)

        out_frame = ttk.LabelFrame(parent, text="outgoing serial (UART TX)")
        out_frame.pack(side="top", fill="both", expand=True, padx=2, pady=2)
        self.outgoing_text = scrolledtext.ScrolledText(
            out_frame, font=MONO_FONT, height=10, wrap="word", state="disabled")
        self.outgoing_text.pack(side="top", fill="both", expand=True)

    def _build_vector_canvas(self, parent):
        frame = ttk.LabelFrame(parent, text="vector display")
        frame.pack(side="top", fill="both", expand=True, padx=2, pady=2)
        toolbar = ttk.Frame(frame)
        toolbar.pack(side="top", fill="x")
        ttk.Button(toolbar, text="Clear",
                   command=self._clear_vector).pack(side="left", padx=2, pady=2)
        self.vector_count_var = tk.StringVar(value="0 segment(s)")
        ttk.Label(toolbar, textvariable=self.vector_count_var).pack(side="left", padx=6)
        # Holder frame fills the remaining space below the toolbar; the
        # canvas is packed into it without fill/expand (default anchor
        # "center") so shrinking it to a square via _on_vector_frame_resize
        # keeps it centered in whatever room the pane gives it, instead of
        # stretching non-square.
        holder = ttk.Frame(frame)
        holder.pack(side="top", fill="both", expand=True)
        self._vector_holder = holder
        self.vector_canvas = tk.Canvas(holder, width=VECTOR_CANVAS_SIZE,
                                        height=VECTOR_CANVAS_SIZE, bg="black",
                                        highlightthickness=0)
        self.vector_canvas.pack(side="top", padx=4, pady=4)
        holder.bind("<Configure>", self._on_vector_frame_resize)

    def _on_vector_frame_resize(self, event):
        """Keeps the vector canvas square as the window/pane is resized:
        picks the largest square that fits the holder frame's current
        available space (min of its width/height, minus the canvas's own
        padding), resizes the canvas widget, and forces a full redraw at
        the new scale (see _redraw_vector's `_vector_last_total` reset)."""
        size = max(50, min(event.width, event.height) - 8)
        if size == self._vector_size:
            return
        self._vector_size = size
        self._vector_scale = size / VECTOR_COORD_RANGE
        self.vector_canvas.config(width=size, height=size)
        self.vector_canvas.delete("vec")
        self._vector_last_total = 0
        self._redraw_vector()

    def _build_log_panel(self, parent):
        frame = ttk.LabelFrame(parent, text="log")
        frame.pack(side="top", fill="both", expand=True, padx=2, pady=2)
        self.log_text = scrolledtext.ScrolledText(
            frame, font=MONO_FONT, wrap="word", state="disabled")
        self.log_text.pack(side="top", fill="both", expand=True)

    def _build_front_panel(self, parent):
        frame = ttk.LabelFrame(parent, text="front panel")
        frame.pack(side="left", fill="both", expand=True, padx=2, pady=2)

        self.mode_var = tk.StringVar(value="A_ONLY")
        mode_combo = ttk.Combobox(frame, textvariable=self.mode_var,
                                   values=["A_ONLY", "BOTH", "B_ONLY"],
                                   state="readonly", width=10)
        mode_combo.grid(row=0, column=0, columnspan=4, sticky="w", padx=4, pady=4)
        mode_combo.bind("<<ComboboxSelected>>", self._on_mode_changed)

        self._button_vars = {}
        cols = 4
        for i, name in enumerate(CHECKBOX_BUTTONS):
            var = tk.BooleanVar(value=False)
            self._button_vars[name] = var
            cb = ttk.Checkbutton(frame, text=name, variable=var,
                                  command=lambda n=name: self._on_button_toggle(n))
            row, col = divmod(i, cols)
            cb.grid(row=row + 1, column=col, sticky="w", padx=4, pady=2)

        ttk.Button(frame, text="RESET", command=self._do_reset).grid(
            row=0, column=4, padx=8, pady=4, sticky="e")

    def _build_dip_switches(self, parent):
        frame = ttk.LabelFrame(parent, text="comm option DIP switches (1-10)")
        frame.pack(side="left", fill="both", padx=2, pady=2)
        self._dip_vars = {}
        for n in range(1, 11):
            var = tk.BooleanVar(value=False)
            self._dip_vars[n] = var
            cb = ttk.Checkbutton(frame, text=f"SW{n}", variable=var,
                                  command=lambda i=n: self._on_dip_toggle(i))
            cb.grid(row=0, column=n - 1, sticky="w", padx=4, pady=4)

    def _bind_keys(self):
        self.root.bind("<F1>", lambda e: self.action_show_help())
        self.root.bind("<F2>", lambda e: self.action_do_step())
        self.root.bind("<F3>", lambda e: self.action_do_run())
        self.root.bind("<F4>", lambda e: self.action_do_continue())
        self.root.bind("<F5>", lambda e: self.action_toggle_trace())
        self.root.bind("<F6>", lambda e: self.action_toggle_live())
        self.root.bind("<F7>", lambda e: self._do_reset())
        self.root.bind("<Control-q>", lambda e: self.action_quit_app())

    # ---- thread-safe UI update plumbing -------------------------------

    def _drain_ui_queue(self):
        """Main-thread only, scheduled via `root.after` - the sole point
        where callables enqueued by a worker thread actually touch a
        widget. Reschedules itself unconditionally (even after a quit
        request is in flight) since `root.destroy()` simply stops the
        Tkinter mainloop, which stops this being called again."""
        try:
            while True:
                fn = self._ui_queue.get_nowait()
                fn()
        except queue.Empty:
            pass
        self.root.after(50, self._drain_ui_queue)

    def _post(self, fn):
        self._ui_queue.put(fn)

    def _sink_from_worker(self, text):
        """`Debugger`'s `output` callable - invoked from a worker
        thread."""
        self._post(lambda: self._append_log(text))

    def _on_tx_from_worker(self, byte):
        self._post(lambda: self._append_outgoing_byte(byte))

    def _on_progress_from_worker(self):
        self._post(self.refresh_registers)

    # ---- log/registers/vector rendering --------------------------------

    def _append_log(self, text):
        self.log_text.configure(state="normal")
        self.log_text.insert("end", text + "\n")
        self.log_text.see("end")
        self.log_text.configure(state="disabled")

    def _append_outgoing_byte(self, byte):
        """Buffers into complete lines before writing, same as
        `io_stubs.DiagnosticTextCapture` - a write per byte would put
        one character per line in the panel instead of readable text."""
        ch = chr(byte) if 32 <= byte < 127 else None
        if byte in (0, 13, 10) or ch is None:
            if self._outgoing_buffer:
                self.outgoing_text.configure(state="normal")
                self.outgoing_text.insert("end", "".join(self._outgoing_buffer) + "\n")
                self.outgoing_text.see("end")
                self.outgoing_text.configure(state="disabled")
                self._outgoing_buffer = []
        else:
            self._outgoing_buffer.append(ch)

    def refresh_incoming(self):
        text = self.dbg.uart.incoming_text()
        self.incoming_var.set(text if text else "(RX queue empty)")

    def refresh_registers(self):
        s = self.dbg.snapshot()
        r = s["regs"]
        iv = s["interrupts"]
        lines = [
            f"instrs: {s['count']}",
            f"CS:IP:  {s['cs']:04X}:{s['ip']:04X}",
            f"phys:   0x{s['phys']:06X}",
            f"FLAGS:  {s['flags']}",
            "",
            f"AX={r['AX']:04X}   BX={r['BX']:04X}   CX={r['CX']:04X}   DX={r['DX']:04X}   "
            f"SI={r['SI']:04X}   DI={r['DI']:04X}   BP={r['BP']:04X}   SP={r['SP']:04X}",
            f"DS={r['DS']:04X}   ES={r['ES']:04X}   SS={r['SS']:04X}",
            "",
            "next:",
            f"  {s['instruction']}",
            "",
            "front panel:",
            f"  {s['front_panel']}",
            "",
            "comm option:",
            f"  {self.dbg.dip_switches.status()}",
            "",
            "uart:",
            f"  {s['uart']}",
            f"  {self.dbg.uart.chip_detail()}",
            "",
            "vector display:",
            f"  {self.dbg.vector_display.status()}",
            "",
            "interrupts:",
            f"  IF={int(iv['if_flag'])}   "
            f"mask latch: 0D={iv['mask_0D_dr']:02X} 1D={iv['mask_1D_tbre']:02X} "
            f"2D={iv['mask_2D']:02X} 3D={iv['mask_3D_diag']:02X}",
            f"  UART enable: RxEN={int(iv['uart_rxen'])} TxEN={int(iv['uart_txen'])}   "
            f"pins: RxRDY={int(iv['uart_rxrdy'])} TxRDY={int(iv['uart_txrdy'])}",
            f"  UART status: RX_READY={int(iv['uart_rx_ready_bit'])} "
            f"TX_READY={int(iv['uart_tx_ready_bit'])} "
            f"TX_EMPTY={int(iv['uart_tx_empty_bit'])}",
        ]
        if iv["int2_fired"] is not None:
            lines.append(f"  INT2: {iv['int2_fired']} fired, {iv['int2_skipped']} skipped")
        self.registers_text.configure(state="normal")
        self.registers_text.delete("1.0", "end")
        self.registers_text.insert("1.0", "\n".join(lines))
        self.registers_text.configure(state="disabled")
        self.refresh_incoming()
        self._redraw_vector()
        if s["stop_reason"]:
            self._append_log(f"[stopped: {s['stop_reason']}]")

    def _redraw_vector(self):
        """Incremental: draws only segments captured since the last
        call, tracked via `VectorDisplay.total` (a monotonic counter,
        unlike `len(segments)` which shrinks once the bounded deque
        starts evicting). Falls back to a full clear+redraw of whatever
        the deque currently holds if either (a) `total` went backwards
        (a `vector clear` happened) or (b) more segments were captured
        since the last redraw than the deque can currently hold (some
        were evicted before ever being drawn) - both cases mean
        `self._vector_last_total` no longer lines up with what's
        actually in the deque.

        **Y-axis orientation is an unconfirmed assumption**: flips Y
        (canvas Y increases downward; assumed the firmware's own
        coordinate space increases upward, like a real scope's CRT)
        purely by convention - not verified against real hardware.
        Revisit if a captured trace looks upside-down next to a known
        real waveform."""
        vd = self.dbg.vector_display
        total = vd.total
        if total == 0:
            if self._vector_last_total != 0:
                self.vector_canvas.delete("vec")
                self._vector_last_total = 0
                self.vector_count_var.set("0 segment(s)")
            return
        missing = total - self._vector_last_total
        if missing <= 0:
            return
        segments = list(vd.segments)
        if missing > len(segments):
            self.vector_canvas.delete("vec")
            to_draw = segments
        else:
            to_draw = segments[-missing:]
        scale, size = self._vector_scale, self._vector_size
        for old_x, old_y, x, y in to_draw:
            x0, y0 = old_x * scale, size - old_y * scale
            x1, y1 = x * scale, size - y * scale
            self.vector_canvas.create_line(x0, y0, x1, y1, fill="#33FF66", tags="vec")
        self._vector_last_total = total
        self.vector_count_var.set(f"{len(segments)} segment(s) ({total} captured total)")

    def _clear_vector(self):
        self._launch("vector clear")

    # ---- front-panel checkbox/dropdown controls ------------------------

    def _sync_front_panel_widgets(self):
        """Sets each checkbox/the dropdown to match `dbg.front_panel`'s
        actual current state - called once at startup so the controls
        reflect the real idle baseline (`VARIABLES.md`'s live-hardware-
        confirmed values) instead of defaulting to all-unchecked.
        Unlike the Textual version, no suppress-events flag is needed
        here: `tk.BooleanVar.set()`/`ttk.Combobox`'s `.set()` don't
        invoke a Checkbutton's `command=` or fire `<<ComboboxSelected>>`
        - only real user interaction (or an explicit `.invoke()`) does."""
        fp = self.dbg.front_panel
        for name in CHECKBOX_BUTTONS:
            reg, bit, active_low = InteractiveFrontPanel.BUTTONS[name]
            value = fp.swb2 if reg == "SWB2" else fp.swb1
            bit_set = bool(value & (1 << bit))
            pressed = (not bit_set) if active_low else bit_set
            self._button_vars[name].set(pressed)
        a_bit = bool(fp.swb1 & 1)          # A_ONLY, active-low
        b_bit = bool(fp.swb1 & (1 << 6))   # B_ONLY, active-low
        a_pressed, b_pressed = not a_bit, not b_bit
        self.mode_var.set("A_ONLY" if a_pressed else "B_ONLY" if b_pressed else "BOTH")
        for n in range(1, 11):
            self._dip_vars[n].set(self.dbg.dip_switches.switches[n - 1])

    def _on_dip_toggle(self, number):
        value = self._dip_vars[number].get()
        if self._live:
            self.dbg.queue_action(lambda: self.dbg.dip_switches.set_switch(number, value))
            self._append_log(f"DIP switch {number} -> {'ON' if value else 'OFF'} (queued)")
            return
        self.dbg.dip_switches.set_switch(number, value)
        self._append_log(f"DIP switch {number} -> {'ON' if value else 'OFF'} - "
                          f"{self.dbg.dip_switches.status()}")
        self.refresh_registers()

    def _on_button_toggle(self, name):
        value = self._button_vars[name].get()
        if self._live:
            self.dbg.queue_action(lambda: self.dbg.front_panel.set_button(name, value))
            self._append_log(f"{'pressed' if value else 'released'} {name} (queued)")
            return
        self.dbg.front_panel.set_button(name, value)
        self._append_log(f"{'pressed' if value else 'released'} {name} - "
                          f"{self.dbg.front_panel.status()}")
        self.refresh_registers()

    def _on_mode_changed(self, event=None):
        mode = self.mode_var.get()
        if self._live:
            self.dbg.queue_action(lambda: (
                self.dbg.front_panel.set_button("A_ONLY", mode == "A_ONLY"),
                self.dbg.front_panel.set_button("B_ONLY", mode == "B_ONLY")))
            self._append_log(f"HORIZONTAL MODE -> {mode} (queued)")
            return
        self.dbg.front_panel.set_button("A_ONLY", mode == "A_ONLY")
        self.dbg.front_panel.set_button("B_ONLY", mode == "B_ONLY")
        self._append_log(f"HORIZONTAL MODE -> {mode} - {self.dbg.front_panel.status()}")
        self.refresh_registers()

    # ---- command dispatch, always off the main thread ------------------

    def _dispatch_and_log(self, line):
        """The actual dispatch+output loop, factored out of
        `_run_command` so `_run_queued` (live mode's per-action path)
        can share it. Both call sites run on a worker thread, never the
        main one, so every touch of a widget must go through `_post`."""
        try:
            for out_line in dispatch_command(self.dbg, line):
                self._post(lambda t=out_line: self._append_log(t))
        except QuitRequested:
            self._post(self.root.destroy)
            return False
        except ValueError as e:
            self._post(lambda: self._append_log(str(e)))
        return True

    def _run_command(self, line):
        """Runs in a worker thread so a long `run`/`continue` never
        freezes the UI. Every non-live command goes through this same
        path for one uniform threading model."""
        try:
            self._dispatch_and_log(line)
        finally:
            self._post(lambda: self._set_busy(False))
        self._post(self.refresh_registers)

    def _run_queued(self, line):
        """A command typed (or a button/checkbox toggled) while `live`
        mode is running - dequeued and invoked between bursts by
        `Debugger.run_live` itself on its own worker thread, so this
        never needs the busy flag (live mode's own `self._live` guard
        already keeps a second step/run/continue from starting
        concurrently - see `_launch`)."""
        self._dispatch_and_log(line)

    def _set_busy(self, busy):
        """Main-thread only (either called directly, or via `_post`
        from a worker's `finally`)."""
        self._busy = busy
        self.cmd_entry.configure(state="disabled" if busy else "normal")
        self.status_var.set("running... (input disabled until it stops)" if busy else "")
        if not busy:
            self.cmd_entry.focus_set()

    def _launch(self, line):
        """The only place that starts a command worker for a normal
        (non-live) command - refuses to start a second one while one is
        already running (Unicorn's `emu_start` can't be safely
        interrupted mid-flight from another thread - see module
        docstring). While `live` mode is running, step/run/continue are
        refused outright and every other command is deferred onto
        `run_live`'s own thread via `queue_action` instead."""
        if self._busy:
            self._append_log("(still running the previous command - wait for it to finish)")
            return
        if self._live:
            cmd0 = line.strip().split(None, 1)[0].lower() if line.strip() else ""
            if cmd0 in ("step", "run", "continue", "c", "reset"):
                self._append_log("(live mode is running - 'live off' or F6 stops it first)")
                return
            self.dbg.queue_action(lambda: self._run_queued(line))
            return
        self._set_busy(True)
        threading.Thread(target=self._run_command, args=(line,), daemon=True).start()

    def _start_live(self):
        if self._busy or self._live:
            self._append_log("(already running)")
            return
        self._live = True
        self.status_var.set("LIVE - ticking in background (buttons/serial stay active)")
        threading.Thread(target=self._run_live_loop, daemon=True).start()

    def _run_live_loop(self):
        try:
            self.dbg.run_live()
        finally:
            self._post(self._live_ended)

    def _live_ended(self):
        self._live = False
        self.status_var.set("")
        self.refresh_registers()  # already logs the stop reason, if any
        self._append_log("(live mode stopped)")

    def _stop_live(self):
        if not self._live:
            self._append_log("(live mode isn't running)")
            return
        self.dbg.stop_live()
        self._append_log("(stopping live mode - finishing the current tick burst...)")

    def action_toggle_live(self):
        if self._live:
            self._stop_live()
        else:
            self._start_live()

    def _do_reset(self):
        """Reboots the emulated CPU/memory/stubs back to power-up
        state. Same busy/live guards as `_launch`, but a separate path
        (not routed through `dispatch_command`) so it can also resync
        the front-panel checkboxes/DIP switches to the freshly-recreated
        `front_panel`/`dip_switches` objects afterward. Runs directly on
        the main thread (not a worker) - `Debugger.reset()` is fast and
        synchronous, unlike `run`/`continue`."""
        if self._busy:
            self._append_log("(still running the previous command - wait for it to finish)")
            return
        if self._live:
            self._append_log("(live mode is running - 'live off' or F6 stops it first)")
            return
        try:
            self.dbg.reset()
        except ValueError as e:
            self._append_log(str(e))
            return
        self._append_log("(reset - CPU back at the reset vector)")
        self.vector_canvas.delete("vec")
        self._vector_last_total = 0
        self._sync_front_panel_widgets()
        self.refresh_registers()

    def _on_command_submitted(self, event=None):
        line = self.cmd_entry.get().strip()
        self.cmd_entry.delete(0, "end")
        if not line:
            return
        if not self._command_history or self._command_history[-1] != line:
            self._command_history.append(line)
        self._history_index = None
        self._history_draft = ""
        self._append_log(f"(tek2230) {line}")
        lower = line.lower()
        if lower in ("live", "live on"):
            self._start_live()
            return
        if lower == "live off":
            self._stop_live()
            return
        if lower == "reset":
            self._do_reset()
            return
        self._launch(line)

    def _on_history_prev(self, event=None):
        if not self._command_history:
            return
        if self._history_index is None:
            self._history_draft = self.cmd_entry.get()
            self._history_index = len(self._command_history)
        if self._history_index > 0:
            self._history_index -= 1
            self.cmd_entry.delete(0, "end")
            self.cmd_entry.insert(0, self._command_history[self._history_index])
        return "break"

    def _on_history_next(self, event=None):
        if self._history_index is None:
            return
        self._history_index += 1
        self.cmd_entry.delete(0, "end")
        if self._history_index >= len(self._command_history):
            self.cmd_entry.insert(0, self._history_draft)
            self._history_index = None
        else:
            self.cmd_entry.insert(0, self._command_history[self._history_index])
        return "break"

    # ---- key-bound/toolbar shortcuts for the most common actions -------

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
            # be safely killed - clearer to say so than to hang with no
            # explanation.
            self._append_log("(still running - can't quit mid-run; wait "
                              "for it to finish, or close the window if "
                              "you need to force it)")
            return
        if self._live:
            self._append_log("(live mode is running - press F6 or type "
                              "'live off' to stop it, then quit again)")
            return
        self.root.destroy()

    def run(self):
        self.root.mainloop()


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
