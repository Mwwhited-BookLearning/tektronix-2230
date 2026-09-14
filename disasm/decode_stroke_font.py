"""
Decode a candidate ROM byte region into SVG "glyphs" using the exact
stroke bit-packing formula traced from draw_readout_char (0xE3854) and
plot_readout_point (0xE3930) in 160-3633 - see disasm/NOTES.md
"Attempted: locating the stroke-font glyph table" for the full
disassembly trace this is based on.

Confirmed formula, per stroke byte B (0x00 terminates the glyph and is
never itself plotted):
    coarse   = (B & 0x70) >> 4   # 0-7  -> Y position (traced as
                                  #        plot_readout_point's [bp+6])
    fine     = B & 0x0F          # 0-15 -> X position (traced as
                                  #        plot_readout_point's [bp+8])
    pen_down = bool(B & 0x80)    # bit7 SET selects the character's
                                  # own pen_default (nonzero -> draws a
                                  # line); CLEAR forces attr=0 (move
                                  # only, no visible line)

Positions are absolute within an 16 (X, 0-15) x 8 (Y, 0-7) cell, not
relative deltas - draw_readout_char re-derives X/Y fresh from every
stroke byte, it does not accumulate offsets. The very first stroke of
each glyph is always treated as a pen-up move (there is no prior point
within the glyph to draw from); every later stroke's line/move choice
follows its own pen_down bit.

This tool takes any byte range from the ROM and renders it this way -
it does NOT know or assume the range is really the font table. Point
it at a candidate region and inspect the output SVG for real
letterforms vs. generic noise. See the CLI --scan mode for a rough
automated search across a whole ROM chip for regions that look
statistically more "font-shaped" (moderate glyph count, moderate
average glyph length) than others.
"""
import argparse
from pathlib import Path

import gen_disasm_x86 as g

CELL_W = 16 * 12   # 16 X positions, 12px each
CELL_H = 8 * 12    # 8 Y positions, 12px each
PAD = 10
COLS = 16


def read_chip_bytes(chip_name):
    chips = g.load_chips()
    c = chips[chip_name]
    return c["buf"], c["base"]


def split_glyphs(data: bytes):
    """Split a byte run into 0x00-terminated glyph candidates."""
    glyphs = []
    cur = []
    for b in data:
        if b == 0:
            if cur:
                glyphs.append(cur)
                cur = []
        else:
            cur.append(b)
    if cur:
        glyphs.append(cur)
    return glyphs


def glyph_points(glyph_bytes):
    """Decode one glyph's stroke bytes into (x, y, pen_down) points."""
    points = []
    for i, b in enumerate(glyph_bytes):
        coarse = (b & 0x70) >> 4
        fine = b & 0x0F
        pen_down = bool(b & 0x80) and i > 0  # first stroke is always a move
        points.append((fine, coarse, pen_down))
    return points


def points_to_svg_path(points):
    if not points:
        return ""
    cmds = []
    for i, (x, y, pen_down) in enumerate(points):
        # flip Y so "up" in the data reads as up on screen
        py = 7 - y
        if i == 0 or not pen_down:
            cmds.append(f"M {x * 12 + 6} {py * 12 + 6}")
        else:
            cmds.append(f"L {x * 12 + 6} {py * 12 + 6}")
    return " ".join(cmds)


def render_svg(glyphs, title, out_path, start_addr=None):
    rows = (len(glyphs) + COLS - 1) // COLS
    width = COLS * (CELL_W + PAD) + PAD
    height = rows * (CELL_H + PAD * 2 + 14) + PAD

    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" '
        f'height="{height}" viewBox="0 0 {width} {height}">',
        f'<rect width="100%" height="100%" fill="white"/>',
        f'<text x="{PAD}" y="14" font-family="monospace" font-size="12">'
        f'{title} - {len(glyphs)} glyphs</text>',
    ]

    for idx, gl in enumerate(glyphs):
        col = idx % COLS
        row = idx // COLS
        cx = PAD + col * (CELL_W + PAD)
        cy = 20 + row * (CELL_H + PAD * 2 + 14)
        addr_label = f"+{idx}" if start_addr is None else f"0x{start_addr + idx:X}"
        parts.append(
            f'<rect x="{cx}" y="{cy}" width="{CELL_W}" height="{CELL_H}" '
            f'fill="none" stroke="lightgray"/>'
        )
        parts.append(
            f'<text x="{cx}" y="{cy - 2}" font-family="monospace" '
            f'font-size="8">{addr_label} ({len(gl)}B)</text>'
        )
        pts = glyph_points(gl)
        path = points_to_svg_path(pts)
        if path:
            parts.append(
                f'<path d="{path}" fill="none" stroke="black" '
                f'stroke-width="1.5" transform="translate({cx},{cy})"/>'
            )

    parts.append("</svg>")
    Path(out_path).write_text("\n".join(parts), encoding="utf-8")
    print(f"wrote {out_path} ({len(glyphs)} glyphs, {rows} rows)")


def scan_candidates(chip_name, min_len=4, max_len=60, window=6000, step=2000):
    """Rough heuristic search: slide a window across the chip and score
    each by how "font-table-like" its glyph-length distribution looks
    (target ~80-140 glyphs, avg length in a plausible stroke-count
    range for simple alphanumerics). Kept for reference - superseded by
    scan_pointer_table below once the real 2-level structure (a 128-
    entry far-pointer array at [0x1DB0]'s value, each entry pointing to
    that character's own separately-located stroke bytes) was traced
    precisely - a flat contiguous-glyph-run search doesn't match that
    shape, and 0x00 is too common in ordinary code to discriminate on
    fragment count/length alone."""
    buf, base = read_chip_bytes(chip_name)
    best = []
    for off in range(0, len(buf) - window, step):
        chunk = buf[off:off + window]
        glyphs = split_glyphs(chunk)
        lens = [len(x) for x in glyphs if min_len <= len(x) <= max_len]
        if not (60 <= len(lens) <= 160):
            continue
        avg = sum(lens) / len(lens)
        if not (6 <= avg <= 30):
            continue
        score = -abs(len(lens) - 100) - abs(avg - 15) * 2
        best.append((score, base + off, len(lens), avg))
    best.sort(reverse=True)
    print(f"{chip_name}: {len(best)} windows passed the shape filter")
    for score, addr, n, avg in best[:15]:
        print(f"  0x{addr:06X}  glyphs~{n}  avglen~{avg:.1f}  score={score:.1f}")


ALL_CHIP_RANGES = [
    ("3633", 0xE0000, 0x10000),
    ("3532", 0xF0000, 0x10000),
    ("2998", 0x80000, 0x10000),
]


def _plausible_rom_addr(seg, off):
    phys = (seg << 4) + off
    for _, base, size in ALL_CHIP_RANGES:
        if base <= phys < base + size:
            return phys
    return None


def _stroke_run_score(buf, base, phys, max_len=60):
    """At a candidate stroke-data address, read forward until a 0x00
    terminator (or max_len) and score how "stroke-byte-like" the run
    looks.

    CORRECTED: every byte 0x01-0xFF is a *syntactically* legal stroke
    byte under this encoding (1 pen bit + 3 coarse bits + 4 fine bits
    = all 8 bits accounted for, no reserved combinations) - there is
    no invalid bit pattern to check for, only 0x00 is special (the
    terminator). An earlier version of this function penalized bit
    0x08 as "an invalid coarse bit," but 0x08 is bit 3, which is part
    of the *fine* nibble (mask 0x0F, bits 0-3) - a fine value of 8-15
    legitimately sets it. That bug rejected half of all valid fine
    values and likely contributed to the "zero candidates found" result
    from the original scan. Score is now just run length (a very short
    run, e.g. immediately hitting 0x00, scores low - not because it's
    "invalid" but because it's too little evidence either way)."""
    off = phys - base
    if off < 0 or off >= len(buf):
        return -1, 0
    run = []
    for i in range(max_len):
        if off + i >= len(buf):
            break
        b = buf[off + i]
        if b == 0:
            break
        run.append(b)
    return len(run), len(run)


def scan_pointer_table(chip_name, step=2, sample_chars=(0x30, 0x31, 0x41, 0x48, 0x4f, 0x61),
                        min_hits=None, min_run=2):
    """Search for the REAL shape: a 128-entry (offset, segment) far-
    pointer array where most sampled entries point to a physically
    plausible ROM address that itself looks like a short, real stroke-
    byte run. Only checks a handful of representative character
    indices per candidate base for speed - good enough to shortlist
    candidates for a full render.

    Sample chars deliberately avoid space (0x20) - a real space glyph
    plausibly has zero strokes (an immediate 0x00 terminator), which
    would always fail a "must have a run" check regardless of whether
    the table is real. Uses '0','1','A','H','O','a' instead - all
    printable characters overwhelmingly likely to have visible strokes.
    min_hits defaults to requiring ALL sample chars to resolve, but can
    be loosened (e.g. len(sample_chars)-1) to tolerate one degenerate
    entry."""
    if min_hits is None:
        min_hits = len(sample_chars)
    buf, base = read_chip_bytes(chip_name)
    results = []
    for table_off in range(0, len(buf) - 512, step):
        total_score = 0
        hits = 0
        for ch in sample_chars:
            idx = (ch & 0x7F) * 4
            entry_off = table_off + idx
            if entry_off + 4 > len(buf):
                break
            far_off = buf[entry_off] | (buf[entry_off + 1] << 8)
            far_seg = buf[entry_off + 2] | (buf[entry_off + 3] << 8)
            phys = _plausible_rom_addr(far_seg, far_off)
            if phys is None:
                continue
            s, length = _stroke_run_score(buf, base, phys)
            if s >= min_run:
                hits += 1
                total_score += s
        if hits >= min_hits:
            results.append((total_score, base + table_off, hits))
    results.sort(reverse=True)
    print(f"{chip_name}: {len(results)} table-base candidates with >={min_hits}/"
          f"{len(sample_chars)} sampled chars resolving to plausible stroke runs")
    for score, addr, hits in results[:20]:
        print(f"  0x{addr:06X}  score={score}  hits={hits}")
    return results


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--chip", default="3633", help="chip name (3633/3532/2998)")
    ap.add_argument("--start", type=lambda s: int(s, 0), help="physical start address")
    ap.add_argument("--end", type=lambda s: int(s, 0), help="physical end address")
    ap.add_argument("--out", default="stroke_candidate.svg")
    ap.add_argument("--scan", action="store_true",
                     help="scan the whole chip for font-table-shaped regions instead (superseded, see --scan-pointers)")
    ap.add_argument("--scan-pointers", action="store_true",
                     help="scan for the real 2-level pointer-table shape instead")
    ap.add_argument("--min-hits", type=int, default=None,
                     help="--scan-pointers: how many sample chars must resolve (default: all)")
    ap.add_argument("--min-run", type=int, default=2,
                     help="--scan-pointers: minimum stroke-run length to count as a hit")
    args = ap.parse_args()

    if args.scan:
        scan_candidates(args.chip)
        return

    if args.scan_pointers:
        scan_pointer_table(args.chip, min_hits=args.min_hits, min_run=args.min_run)
        return

    if args.start is None or args.end is None:
        # default to the already-investigated (and ruled out) candidate
        args.start, args.end = 0xEA5E6, 0xEB131
        args.chip = "3633"

    buf, base = read_chip_bytes(args.chip)
    data = buf[args.start - base:args.end - base]
    glyphs = split_glyphs(data)
    render_svg(glyphs, f"{args.chip} 0x{args.start:X}-0x{args.end:X}",
               args.out, start_addr=args.start)


if __name__ == "__main__":
    main()
