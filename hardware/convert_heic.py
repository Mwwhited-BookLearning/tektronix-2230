#!/usr/bin/env python
"""
Convert iOS-style .heic photos to JPEG, scaled to 25% of the real
photo's dimensions at ~85% quality.

Deliberately does NOT pass ffmpeg a manual `-map`/stream selection.
iOS HEIC photos are HEIF "grid" images: the real photo is stored as a
grid of many small tile streams (e.g. 48x 512x512 tiles marked
"dependent") plus separate depth/matte auxiliary streams (pix_fmt
"gray") that can be as large as, or larger than, one tile. Trying to
manually pick "the largest stream" (an earlier version of this script)
grabs one of those auxiliary streams instead of the real photo, giving
a black/near-black image. ffmpeg's own default HEIF demuxer already
knows how to find and reassemble the correct primary grid image
(yielding the true full resolution, e.g. 4032x3024) - just let it.

Usage:
    python convert_heic.py [directory]

Defaults to this script's own directory (hardware/). Requires ffmpeg
on PATH.
"""
import subprocess
import sys
from pathlib import Path

QUALITY_PERCENT = 85
SCALE_FACTOR = 0.25


def quality_to_qscale(percent):
    # ffmpeg's mjpeg -q:v is 2 (best) .. 31 (worst), not a percent.
    # Linear approximation: 100% -> ~2, 0% -> ~31.
    qscale = 2 + (100 - percent) / 100 * 29
    return max(2, min(31, round(qscale)))


def convert_one(heic_path, out_path):
    qscale = quality_to_qscale(QUALITY_PERCENT)
    # Two passes: ffmpeg's HEIF grid-reconstruction runs as an internal
    # complex filtergraph, which can't have a simple -vf scale filter
    # attached in the same command (errors "Simple and complex
    # filtering cannot be used together"). So extract the full-res
    # image first with no filter, then scale that in a second pass.
    full_res = out_path.with_name(out_path.stem + "._fullres.jpg")
    subprocess.run(
        ["ffmpeg", "-y", "-i", str(heic_path), "-update", "1", str(full_res)],
        capture_output=True, text=True, check=True,
    )
    try:
        subprocess.run(
            ["ffmpeg", "-y", "-i", str(full_res),
             "-vf", f"scale=iw*{SCALE_FACTOR}:ih*{SCALE_FACTOR}",
             "-q:v", str(qscale),
             str(out_path)],
            capture_output=True, text=True, check=True,
        )
    finally:
        full_res.unlink(missing_ok=True)
    print(f"{heic_path.name} -> {out_path.name}")


def main():
    directory = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(__file__).parent
    heic_files = sorted(directory.glob("*.heic")) + sorted(directory.glob("*.HEIC"))
    if not heic_files:
        print(f"No .heic files found in {directory}")
        return
    for heic_path in heic_files:
        out_path = heic_path.with_suffix(".jpg")
        try:
            convert_one(heic_path, out_path)
        except subprocess.CalledProcessError as e:
            print(f"FAILED: {heic_path.name}: {e.stderr[-500:]}")


if __name__ == "__main__":
    main()
