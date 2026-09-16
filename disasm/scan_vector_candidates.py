"""
Render EVERY unidentified byte region (per find_unknown_data.py's own
coverage computation - proven+heuristic disassembly, known strings,
known data tables all subtracted out) as vector shapes at every
possible starting byte offset, using decode_vector_icons.py's
confirmed pen-bit encoding (bit7 set on both bytes of a point = a
"moveto" header, otherwise a "lineto" point; see that module's
docstring for the full rationale and evidence).

This is a brute-force sliding-window sweep, not a scored/filtered
search: real vector data could start at ANY byte offset within a
region (the 3532 jump table and the 0xAE64 icon table both showed
real structure only becoming visible once the right phase/offset was
found), so this tries every one and renders the result for a human to
scan, rather than trying to algorithmically pick "the best" one.

Output: one SVG per unidentified region, each showing one row per
starting offset (offset 0 at top), so scrolling down one file sweeps
through every possible alignment for that region. Written to
--out-dir (default ./vector_scan/), never reviewed by this script
itself - point a human (or another tool) at the output.

Usage:
    python scan_vector_candidates.py                    # default: all regions >= 8 bytes, offsets 0-31
    python scan_vector_candidates.py --min-size 16 --max-offset 16
    python scan_vector_candidates.py --chip 3633         # just one chip
"""
import argparse
from pathlib import Path

import find_unknown_data as fud
import decode_vector_icons as dvi

SCALE = 3
ROW_H = 40
PAD = 10
LABEL_W = 60


def render_region_sweep(chip, start, end, data, out_path, max_offset):
    """One SVG: one row per starting offset, each row a compact
    catalog of that offset's decoded shapes (no per-shape metadata -
    this is a visual sweep, not an analysis; use decode_vector_icons.py
    directly on a specific offset if a row looks promising)."""
    region = data[start:end]
    n = len(region)
    max_off = min(max_offset, max(n - 4, 1))

    rows = []
    for off in range(max_off):
        sub = region[off:]
        if len(sub) < 4:
            continue
        shapes = dvi.parse_shapes(sub)
        rows.append((off, shapes))

    if not rows:
        return False

    width = 1400
    height = PAD * 2 + len(rows) * (ROW_H + 2) + 20

    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" '
        f'height="{height}" viewBox="0 0 {width} {height}">',
        '<rect width="100%" height="100%" fill="white"/>',
        f'<text x="{PAD}" y="14" font-family="monospace" font-size="11">'
        f'{chip} file 0x{start:05X}-0x{end:05X} ({n} bytes) - one row per '
        f'starting offset, sliding window 0..{max_off-1}</text>',
    ]

    y0 = 26
    for row_idx, (off, shapes) in enumerate(rows):
        ry = y0 + row_idx * (ROW_H + 2)
        parts.append(
            f'<text x="{PAD}" y="{ry+ROW_H/2+3}" font-family="monospace" '
            f'font-size="9">+0x{off:X}</text>'
        )
        parts.append(f'<line x1="{LABEL_W}" y1="{ry}" x2="{width}" y2="{ry}" '
                      f'stroke="#eee"/>')
        cx = LABEL_W + 5
        for s in shapes:
            pts_all = ([s["header"]] if s["header"] else []) + s["points"]
            if len(pts_all) < 2:
                cx += 6
                continue
            xs = [p[0] for p in pts_all]
            ys = [p[1] for p in pts_all]
            x0, x1 = min(xs), max(xs)
            y0b, y1 = min(ys), max(ys)
            w = max(x1 - x0, 1)
            h = max(y1 - y0b, 1)
            if cx + w * SCALE + 8 > width:
                break
            cmds = []
            for i, (px, py) in enumerate(pts_all):
                sx = cx + (px - x0) * SCALE
                sy = ry + ROW_H - 4 - (py - y0b) * SCALE
                cmds.append(f"{'M' if i == 0 else 'L'} {sx} {sy}")
            color = "black" if len(s["points"]) >= 3 else "#bbb"
            parts.append(
                f'<path d="{" ".join(cmds)}" fill="none" stroke="{color}" '
                f'stroke-width="1"/>'
            )
            cx += w * SCALE + 8
    parts.append("</svg>")
    Path(out_path).write_text("\n".join(parts), encoding="utf-8")
    return True


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                  formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--chip", default=None, help="restrict to one chip (3633/3532/2998)")
    ap.add_argument("--min-size", type=int, default=8,
                     help="minimum unidentified region size to include (default 8)")
    ap.add_argument("--max-offset", type=int, default=32,
                     help="max sliding-window starting offset to try per region (default 32)")
    ap.add_argument("--out-dir", default="vector_scan", help="output directory")
    args = ap.parse_args()

    covered = fud.compute_covered_bytes()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    chips = [args.chip] if args.chip else list(fud.ROMS.keys())
    total_written = 0
    for chip in chips:
        info = fud.ROMS[chip]
        data = open(info["bin"], "rb").read()
        str_ranges = fud.load_string_ranges(info["strings"])
        known = set()
        for s, e in fud.KNOWN_DATA_RANGES.get(chip, []):
            known.update(range(s, e))
        identified = covered[chip] | str_ranges | known
        gaps = fud.find_gaps(identified, len(data), args.min_size)

        print(f"{chip}: {len(gaps)} unidentified regions >= {args.min_size} bytes")
        for start, end in gaps:
            out_path = out_dir / f"{chip}_0x{start:05X}-0x{end:05X}.svg"
            ok = render_region_sweep(chip, start, end, data, out_path, args.max_offset)
            if ok:
                total_written += 1
                print(f"  wrote {out_path.name}  ({end-start} bytes)")

    print(f"\n{total_written} SVGs written to {out_dir}/")


if __name__ == "__main__":
    main()
