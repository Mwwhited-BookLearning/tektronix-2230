"""
High-level pass: classify every branch target in the proven disassembly
as either local control flow (a loop/goto inside one function) or a
jump that crosses from one function's address range into another's -
the latter being "fractional function" candidates: code that's really
a distinct, shared subroutine but was only ever labeled L_XXXXX
(a bare jump target) because nothing calls it with CALL/LCALL.

Usage: python analyze_loops_vs_functions.py [sysrom_3532_3633.lst]

Method:
  - Walk the .lst top to bottom, tracking "current function" = the
    most recently seen SUB_/ENTRY-kind label. Every instruction until
    the next such label is considered to lexically belong to that
    function (a reasonable approximation given the listing is address-
    sorted and proven functions occupy contiguous address ranges).
  - For every branch/call instruction, resolve its target's physical
    address via the symbols table (label names embed their own
    address, e.g. L_E5D67 -> 0xE5D67; functional names are resolved
    via the reverse address<->name map already in .symbols.json).
  - For every LOC-kind target (a plain L_XXXXX branch target, not a
    real SUB_ call target), collect the set of source addresses/
    functions that jump to it, and classify:
      * loop        - only reached by backward jumps (source addr >
                       target addr) from within its OWN owning
                       function - classic for/while/do-while.
      * goto        - only reached by forward jumps from within its
                       own owning function - if/else, switch, early
                       exit.
      * mixed       - both forward and backward edges, still all
                       local to one function.
      * cross-fn    - reached (via plain JMP/Jcc, not CALL) from a
                       DIFFERENT function's address range than the one
                       that lexically owns the target address - this
                       is the interesting bucket: a label masquerading
                       as local control flow that's actually shared
                       code, a strong "should probably be recognized
                       as its own function" signal.
"""
import re
import sys
import json
from collections import defaultdict

LST_PATH = sys.argv[1] if len(sys.argv) > 1 else "sysrom_3532_3633.lst"
SYMBOLS_PATH = "sysrom_3532_3633.symbols.json"

BRANCH_MNEMONICS = {
    "jmp", "je", "jne", "jz", "jnz", "jg", "jge", "jl", "jle",
    "ja", "jae", "jb", "jbe", "jc", "jnc", "jo", "jno",
    "js", "jns", "jp", "jnp", "jpe", "jpo", "jcxz",
    "loop", "loope", "loopne", "loopz", "loopnz",
}
CALL_MNEMONICS = {"call", "lcall"}

LABEL_LINE_RE = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*):\s*$")
INSN_LINE_RE = re.compile(
    r"^\s+\S+:[0-9A-Fa-f]+\s+\[([0-9A-Fa-f]+)\]\s+[0-9a-fA-F]+\s+(\S+)\s*(.*)$"
)
# operand token that names a label/function, e.g. "jne L_9795A  ; 0x..."
TARGET_NAME_RE = re.compile(r"^(\S+)")


def load_symbols():
    with open(SYMBOLS_PATH) as f:
        syms = json.load(f)
    addr_to_name = {}
    name_to_addr = {}
    addr_to_kind = {}
    for key, v in syms.items():
        if not isinstance(v, dict):
            continue
        addr = int(key, 16)
        name = v.get("name")
        addr_to_name[addr] = name
        addr_to_kind[addr] = v.get("kind")
        if name:
            name_to_addr[name] = addr
    return addr_to_name, name_to_addr, addr_to_kind


def main():
    addr_to_name, name_to_addr, addr_to_kind = load_symbols()

    # edges[target_addr] = list of (source_addr, source_function_addr, mnemonic)
    edges = defaultdict(list)
    # function_ranges: sorted list of (start_addr) for sub/entry-kind labels,
    # used to find "current function" for an arbitrary address.
    func_starts = sorted(
        addr for addr, kind in addr_to_kind.items() if kind in ("sub", "entry")
    )

    def owning_function(addr):
        # last func_start <= addr
        lo, hi = 0, len(func_starts) - 1
        best = None
        while lo <= hi:
            mid = (lo + hi) // 2
            if func_starts[mid] <= addr:
                best = func_starts[mid]
                lo = mid + 1
            else:
                hi = mid - 1
        return best

    current_function = None
    total_insns = 0
    with open(LST_PATH, encoding="utf-8") as f:
        for line in f:
            line = line.rstrip("\n")
            if not line.strip():
                continue
            m = LABEL_LINE_RE.match(line)
            if m:
                name = m.group(1)
                addr = name_to_addr.get(name)
                if addr is not None and addr_to_kind.get(addr) in ("sub", "entry"):
                    current_function = addr
                continue
            m = INSN_LINE_RE.match(line)
            if not m:
                continue
            total_insns += 1
            src_addr = int(m.group(1), 16)
            mnemonic = m.group(2).lower()
            operand = m.group(3).strip()
            if mnemonic not in BRANCH_MNEMONICS and mnemonic not in CALL_MNEMONICS:
                continue
            tm = TARGET_NAME_RE.match(operand)
            if not tm:
                continue
            target_name = tm.group(1)
            target_addr = name_to_addr.get(target_name)
            if target_addr is None:
                continue
            src_fn = owning_function(src_addr)
            edges[target_addr].append((src_addr, src_fn, mnemonic))

    # Classify every LOC-kind target with at least one incoming branch edge
    results = []
    for target_addr, srcs in edges.items():
        kind = addr_to_kind.get(target_addr)
        if kind != "loc":
            continue  # only interested in bare L_ labels here
        target_owner = owning_function(target_addr)
        branch_srcs = [(s, sf, mn) for s, sf, mn in srcs if mn in BRANCH_MNEMONICS]
        if not branch_srcs:
            continue
        distinct_fns = {sf for _, sf, _ in branch_srcs}
        cross = {sf for sf in distinct_fns if sf != target_owner}
        has_backward = any(s > target_addr for s, _, _ in branch_srcs)
        has_forward = any(s < target_addr for s, _, _ in branch_srcs)
        if cross:
            category = "cross-fn"
        elif has_backward and has_forward:
            category = "mixed"
        elif has_backward:
            category = "loop"
        else:
            category = "goto"
        max_dist = max(abs(s - target_addr) for s, _, _ in branch_srcs)
        results.append({
            "target_addr": target_addr,
            "target_name": addr_to_name.get(target_addr),
            "target_owner": target_owner,
            "target_owner_name": addr_to_name.get(target_owner) if target_owner else None,
            "category": category,
            "n_edges": len(branch_srcs),
            "cross_fns": sorted(cross),
            "cross_fn_names": sorted({addr_to_name.get(a) for a in cross}),
            "max_dist": max_dist,
        })

    cats = defaultdict(int)
    for r in results:
        cats[r["category"]] += 1

    print(f"Parsed {total_insns} instructions, {len(edges)} distinct branch/call targets")
    print(f"LOC-kind targets classified: {sum(cats.values())}")
    for c in ("loop", "goto", "mixed", "cross-fn"):
        print(f"  {c:10s}: {cats.get(c, 0)}")

    print()
    print("=== cross-fn candidates (jumped to from a DIFFERENT function's range) ===")
    cross = [r for r in results if r["category"] == "cross-fn"]
    cross.sort(key=lambda r: -len(r["cross_fns"]))
    for r in cross:
        print(f"{r['target_name']} (0x{r['target_addr']:X}) "
              f"owner={r['target_owner_name']} "
              f"crossed_from={r['cross_fn_names']} edges={r['n_edges']} "
              f"max_dist=0x{r['max_dist']:X}")

    print()
    print(f"Total cross-fn candidates: {len(cross)}")

    # Long-jump pass: regardless of the owning-function heuristic above
    # (which can be fragile), a branch/jmp whose displacement is large
    # (bigger than a typical local loop/if-else span) is itself a signal
    # that the target is really a separate, shared routine - a tail-jump
    # to common code rather than a loop back-edge. This is a more
    # assumption-free signal than the owning-function bucketing.
    LONG_JUMP_THRESHOLD = 0x400
    print()
    print(f"=== long-jump candidates (plain JMP/Jcc, displacement >= 0x{LONG_JUMP_THRESHOLD:X}) ===")
    long_jump_targets = defaultdict(list)
    for target_addr, srcs in edges.items():
        for s, sf, mn in srcs:
            if mn not in BRANCH_MNEMONICS:
                continue
            if mn == "jmp" or mn.startswith("j") or mn.startswith("loop"):
                dist = abs(target_addr - s)
                if dist >= LONG_JUMP_THRESHOLD:
                    long_jump_targets[target_addr].append((s, sf, mn, dist))

    lj_rows = []
    for target_addr, hits in long_jump_targets.items():
        kind = addr_to_kind.get(target_addr)
        name = addr_to_name.get(target_addr)
        max_dist = max(h[3] for h in hits)
        n_sources = len({h[0] for h in hits})
        lj_rows.append((max_dist, n_sources, target_addr, name, kind, hits))
    lj_rows.sort(key=lambda r: -r[0])
    for max_dist, n_sources, target_addr, name, kind, hits in lj_rows:
        already_named = kind == "sub" and not name.startswith("SUB_")
        tag = "" if kind == "sub" else " <-- LOC, not a recognized function"
        print(f"0x{target_addr:X} {name} kind={kind}{tag} "
              f"n_sources={n_sources} max_dist=0x{max_dist:X}")

    print()
    print(f"Total long-jump targets: {len(lj_rows)}")


if __name__ == "__main__":
    main()
