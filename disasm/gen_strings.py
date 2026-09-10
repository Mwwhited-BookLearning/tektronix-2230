#!/usr/bin/env python
"""
Extract every printable ASCII string (>=4 chars) from each of the three
"latest" ROM binaries, classify its encoding (length-prefixed vs
null-terminated - see STRINGS.md), and write one readable JSON file per
ROM under disasm/strings_<rom>.json.

This is the machine-readable companion to STRINGS.md (which is the
curated, categorized, human-readable version of the same data) - run
this again and re-review STRINGS.md if a ROM binary ever changes.
"""
import json
import re

STRING_RE = re.compile(rb"[ -~]{4,}")

ROMS = {
    "160-3633": "../binary/160-3633-14.bin",
    "160-3532": "../binary/160-3532-14.bin",
    "160-2998": "../binary/160-2998-14.bin",
}


def looks_real(text, length_prefixed):
    """Filter out short same-letter-shuffle runs that are almost
    certainly coincidental printable bytes inside code/data, not
    actual text."""
    if len(text) >= 5 or " " in text or any(c.islower() for c in text):
        return True
    if length_prefixed:
        return True
    return not (len(set(text)) <= 3 and len(text) <= 6)


def extract(path):
    data = open(path, "rb").read()
    entries = []
    for m in STRING_RE.finditer(data):
        start, end = m.start(), m.end()
        text = data[start:end].decode("ascii")
        len_byte = data[start - 1] if start > 0 else None
        after_byte = data[end] if end < len(data) else None
        length_prefixed = (len_byte == len(text))
        null_terminated = (after_byte == 0)
        if not looks_real(text, length_prefixed):
            continue
        entries.append({
            "offset": f"0x{start:04x}",
            "text": text,
            "length_prefixed": length_prefixed,
            "null_terminated": null_terminated,
        })
    return entries


def main():
    for name, path in ROMS.items():
        entries = extract(path)
        out_path = f"strings_{name}.json"
        with open(out_path, "w") as f:
            json.dump(entries, f, indent=2)
            f.write("\n")
        lp = sum(1 for e in entries if e["length_prefixed"])
        nt = sum(1 for e in entries if e["null_terminated"])
        print(f"{out_path}: {len(entries)} strings "
              f"({lp} length-prefixed, {nt} null-terminated)")


if __name__ == "__main__":
    main()
