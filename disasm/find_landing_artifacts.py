"""
Systematic landing-artifact detector.

Several already-documented anomalies (SUB_E90A5, SUB_E92B0, SUB_F6F4A,
SUB_F1254, the SUB_EAC86/SUB_E99DF/0x88729 chain) share one shape: a
genuine, unambiguous compiled CALL/LCALL/JMP/LJMP instruction whose
literal encoded target lands 1-4 bytes *before* a real, independently
recognized function start (either proven-reachable or heuristically
matched by its own push-bp signature) - the byte(s) in between get
consumed by whatever coincidentally-valid instruction starts at the
"short" target address, rather than reaching the intended prologue.

This scans every "sub"-kind label (i.e. every real CALL/LCALL target
in the proven+heuristic label set) and checks whether address+1
through address+4 is ALSO a recognized label - if so, flags it as a
landing-artifact candidate, so the whole cluster can be found at once
instead of one at a time by hand.
"""
import gen_disasm_x86 as g
import gen_disasm_mainrom_heuristic as mh
import gen_disasm_2998 as g2998


def main():
    mainrom_entries = mh.build_entry_points()
    _, comm_entries = g2998.build_entry_points()
    all_entries = g.ENTRY_POINTS + mainrom_entries + comm_entries
    chips, visited, labels = g.main(None, all_entries)

    kind_by_addr = {addr: lab["kind"] for addr, lab in labels.items()}
    call_targets = {addr for addr, k in kind_by_addr.items() if k == "sub"}

    # Any instruction-start address reached by ANY path (fallthrough or
    # otherwise) - a much broader "is there real, independently-decoded
    # code starting here" check than just recognized labels.
    any_insn_start = set(visited.keys())

    candidates = []
    for t in sorted(call_targets):
        if t not in visited:
            continue  # target itself never got decoded at all (rare)
        t_size = visited[t]["size"]
        for delta in (1, 2, 3, 4):
            if delta >= t_size:
                continue  # not an overlap - just T's normal next instruction
            real = t + delta
            if real in any_insn_start:
                candidates.append((t, delta, real))
                break

    print(f"{len(call_targets)} call targets checked, {len(candidates)} landing-artifact candidates found")
    print()
    for t, delta, real in candidates:
        t_name = labels.get(t, {}).get("name") or visited[t].get("mnem")
        real_lab = labels.get(real, {})
        real_name = real_lab.get("name", "(unlabeled, fallthrough-reached)")
        real_kind = real_lab.get("kind", "insn")
        t_insn = f"{visited[t]['mnem']} {visited[t].get('op', '')}"
        print(f"0x{t:06X} [{t_insn}] is {delta} byte(s) short of 0x{real:06X} ({real_name}, kind={real_kind})")


if __name__ == "__main__":
    main()
