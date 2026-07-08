#!/usr/bin/env python3
"""deck-claim checker (ctest label `deck`). The explainer deck is an emitted
artifact; it gets validated like one. Every `data-claim="ID"` in
docs/deck/index.html must have a matching row in docs/deck/CLAIMS.md — the ledger
that ties each on-slide number to a test name + case count / pinning § / paper eq.

Fails (exit 1) if any deck data-claim lacks a CLAIMS.md row. Exit 0 if all are
sourced. Orphan CLAIMS rows (a row with no slide using it) are reported as a
warning, not a failure.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DECK = ROOT / "docs" / "deck" / "index.html"
CLAIMS = ROOT / "docs" / "deck" / "CLAIMS.md"


def main() -> int:
    if not DECK.exists() or not CLAIMS.exists():
        print(f"[SKIP] deck_check: deck or claims file missing "
              f"({DECK.exists()=}, {CLAIMS.exists()=})")
        return 77

    html = DECK.read_text()
    deck_ids = sorted(set(re.findall(r'data-claim="([A-Za-z0-9_-]+)"', html)))

    md = CLAIMS.read_text()
    # rows begin "| `id` |" — capture the backticked id in the first cell
    claim_ids = set(re.findall(r'^\|\s*`([A-Za-z0-9_-]+)`\s*\|', md, re.M))

    if not deck_ids:
        print("FAIL: no data-claim attributes found in the deck")
        return 1

    missing = [d for d in deck_ids if d not in claim_ids]
    print(f"deck data-claims: {len(deck_ids)}  |  CLAIMS.md rows: {len(claim_ids)}")
    for d in deck_ids:
        print(f"  {'OK ' if d in claim_ids else 'MISSING'}  {d}")
    orphans = sorted(claim_ids - set(deck_ids))
    if orphans:
        print("  (note: CLAIMS rows not used by any slide — allowed: "
              + ", ".join(orphans) + ")")

    if missing:
        print(f"\nFAIL: {len(missing)} deck claim(s) lack a CLAIMS.md row: "
              + ", ".join(missing))
        return 1
    print(f"\nOK: all {len(deck_ids)} deck data-claims are sourced in CLAIMS.md")
    return 0


if __name__ == "__main__":
    sys.exit(main())
