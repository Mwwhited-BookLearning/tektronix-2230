"""
Request an HPGL plot from a real Tek 2230 over RS-232, save the raw
HPGL stream, and render it to SVG for visual inspection.

Uses the RS-232 command language documented in
`hardware/manuals/2230_programming/README.md` (Table 7-26, Display
Commands): `PLOt FORmat:HPGl` selects HP-GL output, `PLOt STArt` begins
the plot. Per that same table, while a plot is in progress the
instrument ignores every command/query except `PLOt ABOrt` - so this
tool doesn't poll for status, it just reads raw bytes until the line
goes quiet for `--idle-timeout` seconds (or `--max-wait` is hit as a
safety net), on the theory that "no more bytes arriving" means the
plot finished.

Default serial settings (1200 8N1, CR terminator) match the baud rate
confirmed live-working on 2026-09-14 - see `disasm/NOTES.md`'s
"RESOLVED, 2026-09-14" section. 9600 was found to be unreliable on
this hardware/cable combination; override with --baud if your own
setup differs.

The HPGL-to-SVG renderer is a small, pragmatic subset interpreter (PU/
PD/PA/PR/SP/IN/DF) - it does NOT implement HPGL's SC/IP scaling
commands (which need to know the plotter's true P1/P2 corner points in
raw device units, which aren't known for this instrument's output).
Instead it auto-fits an SVG viewBox to the actual bounding box of all
plotted points, which is enough to visually validate/inspect a plot
without needing to reverse-engineer exact device units. SC/IP tokens
are parsed (so they don't break anything) but their arguments are
ignored for coordinate purposes.

Usage:
    # Capture a live plot from the scope and render it
    python plot_hpgl_to_svg.py --port COM3 --out plots/waveform1

    # Just re-render a previously-captured .hpgl file
    python plot_hpgl_to_svg.py --from-file plots/waveform1.hpgl --out plots/waveform1

Outputs `<out>.hpgl` (raw capture, skipped in --from-file mode) and
`<out>.svg`.
"""
import argparse
import re
import sys
import time


# ---------------------------------------------------------------------------
# Serial capture
# ---------------------------------------------------------------------------

def capture_hpgl_plot(port, baud, graticule, speed, idle_timeout, max_wait):
    import serial

    parity_map = {"N": serial.PARITY_NONE, "E": serial.PARITY_EVEN, "O": serial.PARITY_ODD}
    ser = serial.Serial(port=port, baudrate=baud, bytesize=8,
                         parity=parity_map["N"], stopbits=1, timeout=0.3)
    try:
        ser.reset_input_buffer()
        # If a previous run was cut off mid-plot (e.g. --max-wait was hit
        # before the scope finished), it's still "busy" and ignores every
        # command but PLOt ABOrt - clear that state unconditionally first.
        ser.write(b"PLOt ABOrt\r")
        time.sleep(0.5)
        ser.read(500)

        ser.write(b"PLOt FORmat:HPGl\r")
        time.sleep(0.3)
        ser.read(500)  # discard any command-echo/status noise before the plot

        if graticule is not None:
            ser.write(("PLOt GRAt:%s\r" % ("ON" if graticule else "OFF")).encode("ascii"))
            time.sleep(0.3)
            ser.read(500)

        if speed is not None:
            ser.write(("PLOt SPEed:%d\r" % speed).encode("ascii"))
            time.sleep(0.3)
            ser.read(500)

        print("Sending PLOt STArt ...", file=sys.stderr)
        ser.write(b"PLOt STArt\r")

        data = bytearray()
        start = time.time()
        last_byte_time = time.time()
        while True:
            chunk = ser.read(1024)
            now = time.time()
            if chunk:
                data.extend(chunk)
                last_byte_time = now
                print(f"\r{len(data)} bytes captured...", end="", file=sys.stderr)
            if data and (now - last_byte_time) > idle_timeout:
                break
            if (now - start) > max_wait:
                print("\nHit --max-wait before the line went idle - stopping anyway.",
                      file=sys.stderr)
                break
        print(file=sys.stderr)
        return bytes(data)
    except KeyboardInterrupt:
        print("\nInterrupted - sending PLOt ABOrt to leave the scope in a clean state.",
              file=sys.stderr)
        try:
            ser.write(b"PLOt ABOrt\r")
        except Exception:
            pass
        raise
    finally:
        ser.close()


# ---------------------------------------------------------------------------
# HPGL parsing
# ---------------------------------------------------------------------------

_STMT_RE = re.compile(r"^([A-Za-z]{2})\s*(.*)$", re.DOTALL)


def parse_hpgl_statements(text):
    """Yield (CMD, [float, ...]) for each semicolon-separated statement."""
    text = text.replace("\r", "").replace("\n", "")
    for stmt in text.split(";"):
        stmt = stmt.strip()
        if not stmt:
            continue
        m = _STMT_RE.match(stmt)
        if not m:
            continue
        cmd = m.group(1).upper()
        argstr = m.group(2).strip()
        nums = []
        if argstr:
            for part in re.split(r"[,\s]+", argstr):
                if not part:
                    continue
                try:
                    nums.append(float(part))
                except ValueError:
                    pass
        yield cmd, nums


def hpgl_to_polylines(text):
    """
    Interpret a pragmatic subset of HPGL, returning a list of
    (pen_number, [(x, y), ...]) polylines in raw plotter units (no
    SC/IP scaling applied - see module docstring).
    """
    x, y = 0.0, 0.0
    pen_down = False
    relative = False
    pen = 1
    polylines = []
    current = None

    def move_to(nx, ny, draw):
        nonlocal current
        if draw:
            if current is None:
                current = [(x, y)]
                polylines.append((pen, current))
            current.append((nx, ny))
        else:
            current = None

    for cmd, nums in parse_hpgl_statements(text):
        if cmd in ("IN", "DF"):
            x, y = 0.0, 0.0
            pen_down = False
            relative = False
            current = None
        elif cmd == "SP":
            pen = int(nums[0]) if nums else 1
            current = None
        elif cmd == "PA":
            relative = False
            for i in range(0, len(nums) - 1, 2):
                x, y = nums[i], nums[i + 1]
                move_to(x, y, draw=pen_down)
        elif cmd == "PR":
            relative = True
            for i in range(0, len(nums) - 1, 2):
                x, y = x + nums[i], y + nums[i + 1]
                move_to(x, y, draw=pen_down)
        elif cmd == "PU":
            pen_down = False
            if not nums:
                current = None
            else:
                for i in range(0, len(nums) - 1, 2):
                    if relative:
                        x, y = x + nums[i], y + nums[i + 1]
                    else:
                        x, y = nums[i], nums[i + 1]
                    move_to(x, y, draw=False)
        elif cmd == "PD":
            pen_down = True
            if nums:
                for i in range(0, len(nums) - 1, 2):
                    if relative:
                        nx, ny = x + nums[i], y + nums[i + 1]
                    else:
                        nx, ny = nums[i], nums[i + 1]
                    move_to(nx, ny, draw=True)
                    x, y = nx, ny
        # SC, IP, VS, PT, and anything else: no geometric effect here.

    return polylines


# ---------------------------------------------------------------------------
# SVG rendering
# ---------------------------------------------------------------------------

_PEN_COLORS = ["#000000", "#1f6feb", "#d62728", "#2ca02c", "#9467bd", "#ff7f0e"]


def polylines_to_svg(polylines, title="HPGL plot", margin=20, target_size=800):
    all_points = [pt for _, pts in polylines for pt in pts]
    if not all_points:
        xs = ys = [0, 1]
    else:
        xs = [p[0] for p in all_points]
        ys = [p[1] for p in all_points]
    xmin, xmax = min(xs), max(xs)
    ymin, ymax = min(ys), max(ys)
    w = max(xmax - xmin, 1e-6)
    h = max(ymax - ymin, 1e-6)
    scale = target_size / max(w, h)
    view_w = w * scale + 2 * margin
    view_h = h * scale + 2 * margin

    def tx(px):
        return (px - xmin) * scale + margin

    def ty(py):
        # HPGL/plotter Y increases "up"; SVG Y increases down - flip.
        return (ymax - py) * scale + margin

    paths = []
    for pen, pts in polylines:
        if len(pts) < 2:
            continue
        color = _PEN_COLORS[(pen - 1) % len(_PEN_COLORS)]
        d = "M " + " L ".join(f"{tx(px):.2f} {ty(py):.2f}" for px, py in pts)
        paths.append(f'<path d="{d}" stroke="{color}" fill="none" stroke-width="1"/>')

    svg = f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {view_w:.1f} {view_h:.1f}">
<title>{title}</title>
<rect x="0" y="0" width="{view_w:.1f}" height="{view_h:.1f}" fill="white"/>
{chr(10).join(paths)}
</svg>
'''
    return svg


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                  formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--port", default="COM3")
    ap.add_argument("--baud", type=int, default=1200,
                     help="default 1200 - see module docstring for why 9600 is avoided")
    ap.add_argument("--graticule", choices=["on", "off"], default=None,
                     help="set PLOt GRAt before starting; omit to leave the scope's current setting")
    ap.add_argument("--speed", type=int, default=None, help="PLOt SPEed 1-10")
    ap.add_argument("--idle-timeout", type=float, default=3.0,
                     help="seconds of silence that mean the plot is done")
    ap.add_argument("--max-wait", type=float, default=120.0,
                     help="hard cap on total capture time")
    ap.add_argument("--from-file", help="skip serial capture, render an existing .hpgl file instead")
    ap.add_argument("--out", required=True,
                     help="output path stem - writes <out>.hpgl (if captured live) and <out>.svg")
    args = ap.parse_args()

    if args.from_file:
        with open(args.from_file, "rb") as f:
            raw = f.read()
    else:
        graticule = {"on": True, "off": False, None: None}[args.graticule]
        raw = capture_hpgl_plot(args.port, args.baud, graticule, args.speed,
                                 args.idle_timeout, args.max_wait)
        hpgl_path = args.out + ".hpgl"
        with open(hpgl_path, "wb") as f:
            f.write(raw)
        print(f"Saved raw HPGL: {hpgl_path} ({len(raw)} bytes)", file=sys.stderr)

    text = raw.decode("ascii", errors="replace")
    polylines = hpgl_to_polylines(text)
    total_points = sum(len(pts) for _, pts in polylines)
    print(f"Parsed {len(polylines)} polyline(s), {total_points} points total", file=sys.stderr)

    svg = polylines_to_svg(polylines, title=args.out)
    svg_path = args.out + ".svg"
    with open(svg_path, "w", encoding="utf-8") as f:
        f.write(svg)
    print(f"Saved SVG: {svg_path}", file=sys.stderr)


if __name__ == "__main__":
    main()
