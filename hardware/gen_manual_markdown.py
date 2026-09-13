"""
Generate structured markdown transcriptions of the operator's and
service manual PDFs, using the PDF's own per-line font-size data (from
the OCR text layer) to detect real headings rather than dumping flat
per-page text. Also extracts page images for pages that look like
figures/schematics rather than typeset text (heuristic: page has less
than TEXT_THRESHOLD characters of body text after cleanup).

Both manuals are OCR'd scans (Artek Media) - every page is fundamentally
one big scanned raster with an invisible OCR text layer on top matching
the original layout, so there's no way to cleanly pull out "just the
figure" from a text page, and no real vector/structural markup exists -
only line-by-line text with a font size per span. This script uses that
size data as the best available structure signal:

- Lines whose max span size is a large multiple of the page's dominant
  ("body") text size become `##`/`###` headings.
- Decorative/noise artifacts (the recurring "Scans by ARTEK MEDIA"
  watermark, single-symbol "lines" like a stray "@" or triangle glyph)
  are filtered before heading classification, since the OCR layer
  sometimes assigns them huge point sizes.
- Remaining body lines are grouped into paragraphs by the PDF's own
  text "block" structure (each block's lines are joined with spaces
  instead of hard newlines) so paragraphs reflow naturally instead of
  breaking at every original line.
- Bare page-folio lines (e.g. "3-1", "1-15") are dropped.

This is a heuristic best-effort structuring pass over an OCR'd scan,
not a faithful re-typesetting - tables in particular don't reconstruct
as markdown tables (OCR column alignment isn't reliable enough), they
just read as body paragraphs. Good enough for search/reference; treat
`images/pageNNNN.jpg` as the source of truth for anything ambiguous.

Usage: python gen_manual_markdown.py
Requires: pip install pymupdf
"""
import re
import fitz  # pymupdf
from pathlib import Path

TEXT_THRESHOLD = 500
RENDER_DPI = 150
JPEG_QUALITY = 80

# A line's max span size must be at least this multiple of the page's
# dominant body-text size to be treated as a heading.
H2_RATIO = 1.7
H3_RATIO = 1.25

NOISE_LINE_RE = re.compile(r"scans?\s+by\s+artek", re.IGNORECASE)
FOLIO_LINE_RE = re.compile(r"^\s*\d{1,3}-\d{1,3}\s*$")

MANUALS = [
    ("2230 .pdf", "2230 Service Manual", "manuals/2230_service"),
    ("070-4998-02.pdf", "2230 Operators Manual", "manuals/2230_operators"),
]

HERE = Path(__file__).parent


def dominant_size(doc):
    """Most-represented (by character count) font size across the doc."""
    totals = {}
    for page in doc:
        for block in page.get_text("dict")["blocks"]:
            for line in block.get("lines", []):
                for span in line["spans"]:
                    size = round(span["size"], 1)
                    totals[size] = totals.get(size, 0) + len(span["text"])
    return max(totals, key=totals.get) if totals else 9.5


def clean_line(text):
    text = text.strip()
    text = re.sub(r"\s+", " ", text)
    return text


def page_to_markdown(page, body_size):
    """Render one page's text as markdown lines, using block/line/span
    structure for paragraph grouping and heading detection."""
    out = []
    d = page.get_text("dict")
    for block in d["blocks"]:
        if "lines" not in block:
            continue
        para_words = []
        for line in block["lines"]:
            raw = "".join(s["text"] for s in line["spans"])
            text = clean_line(raw)
            if not text:
                continue
            if NOISE_LINE_RE.search(text):
                continue
            if FOLIO_LINE_RE.match(text):
                continue
            max_size = max((s["size"] for s in line["spans"]), default=0)
            is_heading_candidate = max_size >= body_size * H3_RATIO
            # Guard against decorative/symbol-only "headings" (stray
            # large glyphs the OCR layer sometimes mis-sizes).
            alnum_count = sum(c.isalnum() for c in text)
            if is_heading_candidate and alnum_count >= 4:
                # Flush any accumulated body paragraph first.
                if para_words:
                    out.append(" ".join(para_words))
                    out.append("")
                    para_words = []
                level = "##" if max_size >= body_size * H2_RATIO else "###"
                out.append(f"{level} {text}")
                out.append("")
            else:
                para_words.append(text)
        if para_words:
            out.append(" ".join(para_words))
            out.append("")
    return "\n".join(out).strip()


def process(pdf_name, title, out_subdir):
    pdf_path = HERE / pdf_name
    out_dir = HERE / out_subdir
    img_dir = out_dir / "images"
    img_dir.mkdir(parents=True, exist_ok=True)

    doc = fitz.open(str(pdf_path))
    body_size = dominant_size(doc)

    lines = [f"# {title}\n"]
    lines.append(
        f"Transcribed from `hardware/{pdf_name}` (OCR'd scan, Artek Media) "
        f"via `hardware/gen_manual_markdown.py`. Headings are detected from "
        f"the OCR layer's own font-size data (body text ≈ {body_size}pt); "
        f"pages with under {TEXT_THRESHOLD} characters of body text are "
        f"rendered as images instead of transcribed (heuristic for "
        f"\"probably a figure/schematic/photo page\", not perfect - see the "
        f"script's docstring). Tables read as plain paragraphs, not "
        f"reconstructed markdown tables - OCR column alignment isn't "
        f"reliable enough for that. OCR errors (e.g. \"tlon\" for \"tion\") "
        f"are left as-is.\n"
    )

    n_pages = doc.page_count
    n_images = 0
    for i in range(n_pages):
        page = doc[i]
        md = page_to_markdown(page, body_size)
        lines.append(f"\n<!-- page {i + 1} of {n_pages} -->\n")
        if len(md) < TEXT_THRESHOLD:
            pix = page.get_pixmap(dpi=RENDER_DPI)
            img_name = f"page{i + 1:04d}.jpg"
            pix.save(str(img_dir / img_name), jpg_quality=JPEG_QUALITY)
            n_images += 1
            lines.append(f"![Page {i + 1}](images/{img_name})\n")
            if md:
                lines.append(
                    "\n<details><summary>OCR text on this page</summary>\n\n"
                    f"{md}\n\n</details>\n"
                )
        else:
            lines.append(md + "\n")

    (out_dir / "README.md").write_text("\n".join(lines), encoding="utf-8")
    print(f"{pdf_name}: {n_pages} pages, {n_images} rendered as images, "
          f"body size {body_size}pt -> {out_dir}")


if __name__ == "__main__":
    for pdf_name, title, out_subdir in MANUALS:
        process(pdf_name, title, out_subdir)
