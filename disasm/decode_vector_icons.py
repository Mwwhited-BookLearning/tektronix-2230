"""
Render a candidate ROM byte region as vector icon shapes, using the
encoding traced from the 160-3633 physical 0xAE64-0xB061 region (see
docs/decode-anomalies/unknown-data-deep-dive-2026-09-15.md, finding 3):

    Each shape is a sequence of 2-byte (x, y) points. A point with BOTH
    bytes >= 0x80 is a "pen-up move to (x&0x7F, y&0x7F)" - it starts a
    new shape and is not itself drawn as a line. A point with either
    byte < 0x80 is a "pen-down line to (x, y)" - drawn as a line from
    the previous point.

    This is the same pen-bit convention already confirmed for
    draw_readout_char's stroke font (bit 7 = pen state), just spread
    across a full byte per axis (7 usable bits: 0-127) instead of a
    packed nibble (0-15 X, 0-7 Y) - consistent with a small table of a
    few large, detailed shapes rather than 128 compact glyphs.

    Verified against the default region: the first closed shape's own
    moveto header lands exactly on its own last traced point (a real
    closed loop, not a coincidence), and the big 40-point circle shape
    reappears later in the table as an exact cyclic rotation of the
    same point list.

This tool does NOT know the region is really icon data - point it at
any candidate range and inspect the rendered SVGs to judge for
yourself. Default region is the one candidate this project has traced
evidence for; nothing here assumes it's correct beyond that.

Usage:
    python decode_vector_icons.py
        (renders the default candidate region to ./vector_icons/)
    python decode_vector_icons.py --chip 3633 --start 0xEAE64 --end 0xEB061 --out-dir out/
    python decode_vector_icons.py --no-flip-y   (disable the Y-flip - see below)

Output, per run:
    - overlay.svg    - every shape drawn on one canvas at its own raw
                        coordinates, each in a different color, with a
                        legend. This is the one to look at first to
                        check whether the shapes visually compose one
                        coherent icon (e.g. a ring + hub + needle all
                        centered together).
    - catalog.svg    - every shape rendered separately in its own
                        labeled grid cell (address, point count),
                        matching decode_stroke_font.py's catalog style.
    - shape_NN.svg   - each shape as its own standalone file, for
                        close inspection or dropping into another tool.

Y-axis note: plotter/CRT coordinate systems commonly have Y increasing
upward, while SVG has Y increasing downward. This is UNCONFIRMED for
this specific table - default behavior flips Y for a more "natural"
look (matching decode_stroke_font.py's own convention), but pass
--no-flip-y to see it the other way and judge which looks more like a
real icon.
"""
import argparse
from pathlib import Path

import gen_disasm_x86 as g

SCALE = 8      # px per coordinate unit
PAD = 20
CELL = 40 * SCALE + PAD * 2   # generous fixed cell size for the catalog
COLS = 6

# The one candidate this project has actual evidence for - see
# docs/decode-anomalies/unknown-data-deep-dive-2026-09-15.md finding 3.
DEFAULT_CHIP = "3633"
DEFAULT_START = 0xEAE64
DEFAULT_END = 0xEB061

PALETTE = [
    "#1b6ea0", "#a01b1b", "#1ba05a", "#a0731b", "#6a1ba0",
    "#a01b73", "#1ba0a0", "#5a731b", "#731b5a", "#333333",
]


def read_chip_bytes(chip_name):
    chips = g.load_chips()
    c = chips[chip_name]
    return c["buf"], c["base"]


def parse_shapes(data: bytes):
    """Split a byte run into shapes per the pen-bit convention.

    Returns a list of dicts: {"header": (x,y) or None, "points": [...],
    "byte_offset": offset of this shape's own points within `data`}.
    A leading run of points with no preceding header (header=None) is
    included as-is - it's exactly what a real un-headed data prefix
    would look like (e.g. this project's own countdown-ramp prefix
    before the real icon data starts), not filtered out, so you can
    judge it visually too.

    Special case: the literal bytes 0xFF 0xFF (masked coordinate
    (127,127), far outside every observed real coordinate's 0-34
    range) show up repeatedly as a boundary marker with no following
    points in this project's own manual trace - it's a "shape
    ended, no new moveto yet" marker, not a real (127,127) point.
    Treated as header=None rather than a real header so it doesn't
    get misread as part of whatever shape happens to follow it.
    """
    shapes = []
    cur_points = []
    cur_header = None
    cur_start = 0
    i = 0
    n = len(data)
    while i + 1 < n:
        b1, b2 = data[i], data[i + 1]
        if b1 >= 0x80 and b2 >= 0x80:
            shapes.append({
                "header": cur_header,
                "points": cur_points,
                "byte_offset": cur_start,
            })
            cur_points = []
            cur_header = None if (b1, b2) == (0xFF, 0xFF) else (b1 & 0x7F, b2 & 0x7F)
            cur_start = i
            i += 2
        else:
            cur_points.append((b1, b2))
            i += 2
    shapes.append({
        "header": cur_header,
        "points": cur_points,
        "byte_offset": cur_start,
    })
    # drop the bookkeeping "shape" that's just a trailing header with
    # zero points (an artifact of the scan, not a real shape)
    return [s for s in shapes if s["points"]]


def shape_bbox(shape):
    pts = shape["points"]
    if shape["header"]:
        pts = [shape["header"]] + pts
    xs = [p[0] for p in pts]
    ys = [p[1] for p in pts]
    return min(xs), min(ys), max(xs), max(ys)


def shape_path(shape, flip_y=True, y_max=127):
    def fy(y):
        return (y_max - y) if flip_y else y

    pts = shape["points"]
    cmds = []
    if shape["header"] is not None:
        hx, hy = shape["header"]
        cmds.append(f"M {hx * SCALE} {fy(hy) * SCALE}")
        first_is_L = True
    else:
        first_is_L = False
    for i, (x, y) in enumerate(pts):
        cmd = "L" if (i > 0 or first_is_L) else "M"
        cmds.append(f"{cmd} {x * SCALE} {fy(y) * SCALE}")
    return " ".join(cmds)


def render_overlay(shapes, out_path, flip_y=True):
    all_pts = []
    for s in shapes:
        if s["header"]:
            all_pts.append(s["header"])
        all_pts.extend(s["points"])
    if not all_pts:
        print("no points to render")
        return
    xs = [p[0] for p in all_pts]
    ys = [p[1] for p in all_pts]
    y_max = max(ys) if ys else 127

    drawable = [(idx, s) for idx, s in enumerate(shapes)
                if len(s["points"]) >= 2 or s["header"]]

    legend_rows = len(drawable)
    legend_h = legend_rows * 14 + 8
    width = max((max(xs) + 4) * SCALE + PAD * 2, 260)
    height = (y_max + 4) * SCALE + PAD * 2 + 20 + legend_h

    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" '
        f'height="{height}" viewBox="0 0 {width} {height}">',
        '<rect width="100%" height="100%" fill="white"/>',
        f'<text x="{PAD}" y="14" font-family="monospace" font-size="12">'
        f'overlay - {len(shapes)} shapes, all drawn at their own raw '
        f'coordinates (flip_y={flip_y})</text>',
        f'<g transform="translate({PAD},{PAD + 20})">',
    ]
    for idx, s in drawable:
        color = PALETTE[idx % len(PALETTE)]
        path = shape_path(s, flip_y=flip_y, y_max=y_max)
        parts.append(
            f'<path d="{path}" fill="none" stroke="{color}" '
            f'stroke-width="1.2"/>'
        )
        # mark the header/start point
        if s["header"]:
            hx, hy = s["header"]
            fy = (y_max - hy) if flip_y else hy
            parts.append(
                f'<circle cx="{hx*SCALE}" cy="{fy*SCALE}" r="2" fill="{color}"/>'
            )
    parts.append("</g>")

    legend_y0 = height - legend_h
    parts.append(
        f'<text x="{PAD}" y="{legend_y0}" font-family="monospace" '
        f'font-size="10">legend:</text>'
    )
    for row, (idx, s) in enumerate(drawable):
        color = PALETTE[idx % len(PALETTE)]
        y = legend_y0 + 14 + row * 14
        x0, y0, x1, y1 = shape_bbox(s)
        closes = ""
        if s["header"] and s["points"]:
            hx, hy = s["header"]
            lx, ly = s["points"][-1]
            if abs(hx - lx) <= 1 and abs(hy - ly) <= 1:
                closes = " CLOSED"
        parts.append(f'<rect x="{PAD}" y="{y-8}" width="10" height="10" fill="{color}"/>')
        parts.append(
            f'<text x="{PAD+16}" y="{y}" font-family="monospace" font-size="9">'
            f'shape{idx} +0x{s["byte_offset"]:X} {len(s["points"])}pt '
            f'bbox[{x0},{y0}-{x1},{y1}]{closes}</text>'
        )
    parts.append("</svg>")
    Path(out_path).write_text("\n".join(parts), encoding="utf-8")
    print(f"wrote {out_path} ({len(shapes)} shapes overlaid)")


def render_shape_svg(shape, out_path, flip_y=True, label=""):
    x0, y0, x1, y1 = shape_bbox(shape)
    y_max = y1
    width = (x1 - x0 + 2) * SCALE + PAD * 2
    height = (y1 - y0 + 2) * SCALE + PAD * 2 + 16
    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" '
        f'height="{height}" viewBox="0 0 {width} {height}">',
        '<rect width="100%" height="100%" fill="white"/>',
        f'<text x="4" y="12" font-family="monospace" font-size="10">{label}</text>',
        f'<g transform="translate({PAD - x0*SCALE},{PAD + 16})">',
    ]
    path = shape_path(shape, flip_y=flip_y, y_max=y_max)
    parts.append(f'<path d="{path}" fill="none" stroke="black" stroke-width="1.5"/>')
    if shape["header"]:
        hx, hy = shape["header"]
        fy = (y_max - hy) if flip_y else hy
        parts.append(f'<circle cx="{hx*SCALE}" cy="{fy*SCALE}" r="2.5" fill="red"/>')
    parts.append("</g>")
    parts.append("</svg>")
    Path(out_path).write_text("\n".join(parts), encoding="utf-8")


def render_catalog(shapes, title, out_path, flip_y=True):
    rows = (len(shapes) + COLS - 1) // COLS
    width = COLS * (CELL + PAD) + PAD
    height = rows * (CELL + PAD * 2 + 14) + PAD + 20

    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" '
        f'height="{height}" viewBox="0 0 {width} {height}">',
        '<rect width="100%" height="100%" fill="white"/>',
        f'<text x="{PAD}" y="16" font-family="monospace" font-size="12">'
        f'{title} - {len(shapes)} shapes</text>',
    ]
    for idx, s in enumerate(shapes):
        col = idx % COLS
        row = idx // COLS
        cx = PAD + col * (CELL + PAD)
        cy = 24 + row * (CELL + PAD * 2 + 14)
        x0, y0, x1, y1 = shape_bbox(s)
        closes = ""
        if s["header"] and s["points"]:
            hx, hy = s["header"]
            lx, ly = s["points"][-1]
            if abs(hx - lx) <= 1 and abs(hy - ly) <= 1:
                closes = " CLOSED"
        label = (f"shape{idx} +0x{s['byte_offset']:X} "
                 f"{len(s['points'])}pt bbox[{x0},{y0}-{x1},{y1}]{closes}")
        parts.append(
            f'<rect x="{cx}" y="{cy}" width="{CELL}" height="{CELL}" '
            f'fill="none" stroke="lightgray"/>'
        )
        parts.append(
            f'<text x="{cx}" y="{cy - 2}" font-family="monospace" '
            f'font-size="8">{label}</text>'
        )
        if len(s["points"]) >= 1 or s["header"]:
            y_max = y1
            # center the shape in its cell
            sw = (x1 - x0) * SCALE
            sh = (y1 - y0) * SCALE
            ox = cx + (CELL - sw) / 2 - x0 * SCALE
            oy = cy + (CELL - sh) / 2
            path = shape_path(s, flip_y=flip_y, y_max=y_max)
            parts.append(
                f'<path d="{path}" fill="none" stroke="black" '
                f'stroke-width="1.5" transform="translate({ox},{oy})"/>'
            )
            if s["header"]:
                hx, hy = s["header"]
                fy = (y_max - hy) if flip_y else hy
                parts.append(
                    f'<circle cx="{ox+hx*SCALE}" cy="{oy+fy*SCALE}" r="2" fill="red"/>'
                )
    parts.append("</svg>")
    Path(out_path).write_text("\n".join(parts), encoding="utf-8")
    print(f"wrote {out_path} ({len(shapes)} shapes, {rows} rows)")


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                  formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--chip", default=DEFAULT_CHIP, help="chip name (3633/3532/2998)")
    ap.add_argument("--start", type=lambda s: int(s, 0), default=None,
                     help="physical start address")
    ap.add_argument("--end", type=lambda s: int(s, 0), default=None,
                     help="physical end address")
    ap.add_argument("--out-dir", default="vector_icons", help="output directory")
    ap.add_argument("--no-flip-y", action="store_true",
                     help="disable the Y-flip (see module docstring)")
    ap.add_argument("--min-points", type=int, default=1,
                     help="skip shapes with fewer points than this when writing individual files")
    args = ap.parse_args()

    start = args.start if args.start is not None else DEFAULT_START
    end = args.end if args.end is not None else DEFAULT_END
    flip_y = not args.no_flip_y

    buf, base = read_chip_bytes(args.chip)
    data = buf[start - base:end - base]
    shapes = parse_shapes(data)

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    print(f"{args.chip} 0x{start:06X}-0x{end:06X} ({len(data)} bytes): "
          f"{len(shapes)} shapes found")
    for idx, s in enumerate(shapes):
        x0, y0, x1, y1 = shape_bbox(s)
        closes = ""
        if s["header"] and s["points"]:
            hx, hy = s["header"]
            lx, ly = s["points"][-1]
            if abs(hx - lx) <= 1 and abs(hy - ly) <= 1:
                closes = " CLOSED"
        print(f"  shape{idx}: +0x{s['byte_offset']:X}  hdr={s['header']}  "
              f"{len(s['points'])} pts  bbox=[{x0},{y0}]-[{x1},{y1}]{closes}")

    render_overlay(shapes, out_dir / "overlay.svg", flip_y=flip_y)
    render_catalog(shapes, f"{args.chip} 0x{start:X}-0x{end:X}",
                    out_dir / "catalog.svg", flip_y=flip_y)
    for idx, s in enumerate(shapes):
        if len(s["points"]) < args.min_points:
            continue
        label = f"shape{idx} +0x{s['byte_offset']:X} ({len(s['points'])}pt)"
        render_shape_svg(s, out_dir / f"shape_{idx:02d}.svg", flip_y=flip_y, label=label)
    print(f"\nwrote {len(shapes)} individual shape SVGs to {out_dir}/")


if __name__ == "__main__":
    main()


def scan_chip_for_shape_clusters(chip_name, window=2000, step=500, min_closed=3):
    """Slide a window across a whole chip and score each by how many
    genuinely CLOSED shapes (header present, >=4 points, and the
    header lands within 2 units of the shape's own last point) it
    contains in a plausible size range (bbox width/height 4-40 units -
    covers everything actually seen in the one confirmed candidate).
    A real character/icon library should show up as a run of *several*
    such windows in a row (many glyphs back-to-back), not an isolated
    spike - single-window spikes are much more likely coincidental
    byte patterns than a real table this small a scan can even detect
    reliably. This is a coarse survey tool, not a final answer -
    manually render anything it flags with the normal CLI before
    trusting it."""
    buf, base = read_chip_bytes(chip_name)
    results = []
    for off in range(0, len(buf) - window, step):
        chunk = buf[off:off + window]
        shapes = parse_shapes(chunk)
        closed = 0
        for s in shapes:
            if not s["header"] or len(s["points"]) < 4:
                continue
            x0, y0, x1, y1 = shape_bbox(s)
            if not (4 <= (x1 - x0) <= 40 and 4 <= (y1 - y0) <= 40):
                continue
            hx, hy = s["header"]
            lx, ly = s["points"][-1]
            if abs(hx - lx) <= 2 and abs(hy - ly) <= 2:
                closed += 1
        if closed >= min_closed:
            results.append((closed, base + off))
    results.sort(reverse=True)
    print(f"{chip_name}: {len(results)} windows with >= {min_closed} plausible closed shapes "
          f"(window={window}, step={step})")
    for closed, addr in results[:25]:
        print(f"  0x{addr:06X}  closed_shapes={closed}")
    return results
