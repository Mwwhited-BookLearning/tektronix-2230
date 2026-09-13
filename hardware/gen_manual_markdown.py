"""
Generate markdown transcriptions of the operator's and service manual
PDFs, extracting page images for pages that look like figures/schematics
rather than typeset text (heuristic: page has less than TEXT_THRESHOLD
characters of OCR text - typeset prose pages typically run 1500-4000
chars, while schematic/photo/foldout pages usually fall well under 500).

Both manuals are OCR'd scans (Artek Media) - every page is fundamentally
one big scanned raster with an OCR text layer on top, so there's no way
to cleanly pull out "just the figure" from a text page; this script's
heuristic decides, per whole page, whether to keep the OCR text or keep
the rendered image instead. Not perfect - some genuinely figure-heavy
pages have enough component-label text to clear the threshold, and some
sparse text pages (e.g. a short table) will get imaged when text would
have done. Good enough for a first pass; adjust TEXT_THRESHOLD and rerun
if a specific page comes out wrong.

Usage: python gen_manual_markdown.py
Requires: pip install pymupdf
"""
import fitz  # pymupdf
from pathlib import Path

TEXT_THRESHOLD = 500
RENDER_DPI = 150
JPEG_QUALITY = 80

MANUALS = [
    ("2230 .pdf", "2230 Service Manual", "manuals/2230_service"),
    ("070-4998-02.pdf", "2230 Operators Manual", "manuals/2230_operators"),
]

HERE = Path(__file__).parent


def process(pdf_name, title, out_subdir):
    pdf_path = HERE / pdf_name
    out_dir = HERE / out_subdir
    img_dir = out_dir / "images"
    img_dir.mkdir(parents=True, exist_ok=True)

    doc = fitz.open(str(pdf_path))
    lines = [f"# {title}\n"]
    lines.append(
        f"Transcribed from `hardware/{pdf_name}` (OCR'd scan, Artek Media) "
        f"via `hardware/gen_manual_markdown.py`. Pages with under "
        f"{TEXT_THRESHOLD} characters of OCR text are rendered as images "
        f"instead of transcribed (heuristic for \"probably a figure/"
        f"schematic/photo page\", not perfect - see the script's docstring). "
        f"OCR errors (e.g. \"tlon\" for \"tion\") are left as-is.\n"
    )

    n_pages = doc.page_count
    n_images = 0
    for i in range(n_pages):
        page = doc[i]
        text = page.get_text()
        lines.append(f"\n---\n\n## Page {i + 1} of {n_pages}\n")
        if len(text.strip()) < TEXT_THRESHOLD:
            pix = page.get_pixmap(dpi=RENDER_DPI)
            img_name = f"page{i + 1:04d}.jpg"
            pix.save(str(img_dir / img_name), jpg_quality=JPEG_QUALITY)
            n_images += 1
            lines.append(f"![Page {i + 1}](images/{img_name})\n")
            if text.strip():
                lines.append(
                    "\n<details><summary>OCR text on this page</summary>\n\n"
                    f"```\n{text.strip()}\n```\n\n</details>\n"
                )
        else:
            lines.append(text.strip() + "\n")

    (out_dir / "README.md").write_text("\n".join(lines), encoding="utf-8")
    print(f"{pdf_name}: {n_pages} pages, {n_images} rendered as images -> {out_dir}")


if __name__ == "__main__":
    for pdf_name, title, out_subdir in MANUALS:
        process(pdf_name, title, out_subdir)
