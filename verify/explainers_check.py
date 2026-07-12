#!/usr/bin/env python3
"""explainer-claim checker (ctest label `explainers`). The explainer suite
(docs/explainers/) is a documentation artifact; it gets validated like the deck
(see verify/deck_check.py, docs/deck/CLAIMS.md — the precedent).

STRUCTURAL check only, both directions, over docs/explainers/E*.md vs CLAIMS-E.md:
  (1) every `<!-- claim:ID -->` marker in a prose file has a row in CLAIMS-E.md;
  (2) every CLAIMS-E.md row is cited by some marker (no dead ledger rows).

It does NOT verify content ACCURACY — that each row's claim faithfully reflects its
source as an understatement/special-case and never an inversion (register rule 1). That
is a referee-level read, exactly as docs/deck/CLAIMS.md prose attribution is refereed
(deck_check.py header, ERRATA #16). Fails (exit 1) on any unsourced marker or dead row;
77 (SKIP) if the files are missing.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXPL = ROOT / "docs" / "explainers"
CLAIMS = EXPL / "CLAIMS-E.md"

MARKER = re.compile(r"<!--\s*claim:(E[0-9]+-[0-9]+)\s*-->")
ROW = re.compile(r"^\|\s*(E[0-9]+-[0-9]+)\s*\|", re.M)


def main() -> int:
    if not EXPL.is_dir() or not CLAIMS.exists():
        print("[SKIP] explainers_check: docs/explainers/ or CLAIMS-E.md missing")
        return 77

    prose_files = sorted(p for p in EXPL.glob("E*.md"))
    markers = {}                      # id -> list of files it appears in
    for f in prose_files:
        for cid in MARKER.findall(f.read_text()):
            markers.setdefault(cid, []).append(f.name)
    marker_ids = set(markers)

    rows = set(ROW.findall(CLAIMS.read_text()))

    if not marker_ids:
        print("FAIL: no <!-- claim:ID --> markers found in docs/explainers/E*.md")
        return 1

    unsourced = sorted(marker_ids - rows)   # marker with no ledger row
    dead = sorted(rows - marker_ids)        # ledger row cited by nothing

    print(f"explainer files: {len(prose_files)}  |  claim markers: {len(marker_ids)}  "
          f"|  CLAIMS-E rows: {len(rows)}")

    ok = True
    if unsourced:
        print(f"\nFAIL: {len(unsourced)} claim marker(s) lack a CLAIMS-E.md row: "
              + ", ".join(unsourced))
        ok = False
    if dead:
        print(f"\nFAIL: {len(dead)} CLAIMS-E.md row(s) cited by no marker: "
              + ", ".join(dead))
        ok = False
    if not ok:
        print("\nFix: add the missing ledger row(s), or remove the dead row(s) / marker(s). "
              "Content accuracy (rule 1) is a referee read, not checked here.")
        return 1

    print(f"\nOK: all {len(marker_ids)} explainer claim markers are sourced in CLAIMS-E.md, "
          "and every ledger row is cited (structural; content accuracy referee-owned)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
