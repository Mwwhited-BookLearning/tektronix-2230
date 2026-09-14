"""
Extract the comm ROM's (160-2998-14.bin) RS-232/GPIB command-keyword
tables: the length-prefixed argument/value keyword string table, the
length-prefixed top-level command header string table, and the
6-byte-per-entry dispatch/index table that resolves a numeric command
ID to a far pointer landing on one of the header table's entries.

See `docs/comm-rom/command-keyword-table.md` for the full writeup and
`docs/comm-rom/rs232-live-session-2026-09-14.md` for how this was
originally found (live command/response testing, then confirmed by
reading the raw ROM bytes).

Table boundaries and the metadata-byte-skip heuristic were determined
by inspection this session - see the writeup for the reasoning. Usage:

    python disasm/decode_comm_keyword_table.py
"""
import json

ROM_PATH = "binary/160-2998-14.bin"
PHYS_BASE = 0x80000

# Table boundaries (file offsets into 160-2998-14.bin).
ARG_TABLE_START = 0x8A58   # length byte for "ABOrt"
ARG_TABLE_END = 0x8D1C
DISPATCH_TABLE_START = 0x8D3E
DISPATCH_TABLE_END = 0x8DE6
HDR_TABLE_START = 0x8DE6   # length byte for "ACQuisition"
HDR_TABLE_END = 0x8F1E

KEYWORD_CHARS = set("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789./:")


def _try_entry(raw, p, end, min_len=2):
    if p >= end:
        return None
    length = raw[p]
    if length < min_len or length > 12 or p + 1 + length > end:
        return None
    chunk = raw[p + 1:p + 1 + length]
    try:
        text = chunk.decode("ascii")
    except UnicodeDecodeError:
        return None
    if not all(c in KEYWORD_CHARS for c in text):
        return None
    return (p, length, text)


def parse_length_prefixed_strings(raw, start, end):
    """Walk [len_byte][ascii text][N metadata bytes] entries.

    The metadata-byte count varies per entry (0-3 bytes observed, not a
    fixed width - e.g. CH1/CH2 have none, most single-value keywords
    have 1, some like CURS2 have 3). This tries the smallest skip
    (0..3 bytes) that lands on another plausible entry - real entries
    here are never a single bare letter, which is what rules out most
    metadata-byte coincidences (see the writeup for a worked example:
    without this, "CURS2" + a metadata byte that happens to equal 0x44
    ('D') gets misread as a spurious 1-character keyword "D").
    """
    out = []
    p = start
    while p < end:
        entry = _try_entry(raw, p, end)
        if entry is None:
            break
        out.append(entry)
        text_end = entry[0] + 1 + entry[1]
        found = None
        for skip in (0, 1, 2, 3):
            if _try_entry(raw, text_end + skip, end) is not None:
                found = text_end + skip
                break
        if found is None:
            break
        p = found
    return out, p


def parse_dispatch_table(raw, start, end):
    """Each 6-byte record is [id_byte][0xFF marker][2-byte LE offset]
    [2-byte segment, always 0xDE,0x90]. The far pointer (segment*16 +
    offset) resolves through the confirmed 0x90000-0x97FFF address-
    decode alias back to a real file offset via `file_off = 0x8DE0 +
    offset` - landing exactly on a header-table entry's length byte for
    every non-zero id. id=0x00 entries all point at a shared 0x00
    "empty string" sentinel (file offset 0x8F1C).
    """
    records = []
    for i in range(0, end - start, 6):
        r = raw[start + i:start + i + 6]
        if len(r) < 6 or r[1] != 0xFF or r[4] != 0xDE or r[5] != 0x90:
            continue
        idx = r[0]
        offset = r[2] | (r[3] << 8)
        file_off = 0x8DE0 + offset
        if idx == 0:
            target = None
        else:
            length = raw[file_off]
            target = raw[file_off + 1:file_off + 1 + length].decode("ascii", errors="replace")
        records.append({"id": idx, "rec_offset": offset, "file_offset": file_off, "target": target})
    return records


def main():
    raw = open(ROM_PATH, "rb").read()

    arg_entries, arg_end = parse_length_prefixed_strings(raw, ARG_TABLE_START, ARG_TABLE_END)
    dispatch_records = parse_dispatch_table(raw, DISPATCH_TABLE_START, DISPATCH_TABLE_END)
    hdr_entries, hdr_end = parse_length_prefixed_strings(raw, HDR_TABLE_START, HDR_TABLE_END)

    result = {
        "argument_keywords": [
            {"phys_addr": f"0x{PHYS_BASE + off:06x}", "len": length, "text": text}
            for off, length, text in arg_entries
        ],
        "dispatch_table": [
            {**rec, "phys_addr": f"0x{PHYS_BASE + rec['file_offset']:06x}"}
            for rec in dispatch_records
        ],
        "header_keywords": [
            {"phys_addr": f"0x{PHYS_BASE + off:06x}", "len": length, "text": text}
            for off, length, text in hdr_entries
        ],
    }

    out_path = "disasm/comm_keyword_tables.json"
    with open(out_path, "w") as f:
        json.dump(result, f, indent=2)

    print(f"argument-keyword table: {len(arg_entries)} entries (0x8A59-0x{arg_end:04x})")
    print(f"dispatch table: {len(dispatch_records)} records (0x{DISPATCH_TABLE_START:04x}-0x{DISPATCH_TABLE_END:04x})")
    print(f"header-keyword table: {len(hdr_entries)} entries (0x8DE7-0x{hdr_end:04x})")
    print(f"wrote {out_path}")


if __name__ == "__main__":
    main()
