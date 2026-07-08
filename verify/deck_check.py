#!/usr/bin/env python3
"""deck-claim checker (ctest label `deck`). The explainer deck is an emitted
artifact; it gets validated like one. Two checks over docs/deck/index.html vs
docs/deck/CLAIMS.md:

  (1) EXISTENCE — every `data-claim="ID"` in the deck has a row in CLAIMS.md.
  (2) NUMBERS — every literal integer of >= 5 digits in a slide's claim badge
      also appears (comma-stripped) in that ID's ledger row, so a slide cannot
      cite a significant count that isn't sourced.

It does NOT verify prose ATTRIBUTION (that a bound/credit is applied to the right
object) — that is a semantic, referee-level check (see CLAIMS.md header and
ERRATA #16). Fails (exit 1) on any missing row or unsourced slide number.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DECK = ROOT / "docs" / "deck" / "index.html"
CLAIMS = ROOT / "docs" / "deck" / "CLAIMS.md"


def strip_commas(s: str) -> str:
    return s.replace(",", "")


def main() -> int:
    if not DECK.exists() or not CLAIMS.exists():
        print(f"[SKIP] deck_check: deck or claims file missing")
        return 77

    html = DECK.read_text()
    md = CLAIMS.read_text()

    deck_ids = sorted(set(re.findall(r'data-claim="([A-Za-z0-9_-]+)"', html)))
    badges = dict(re.findall(
        r'<span class="claim"[^>]*data-claim="([A-Za-z0-9_-]+)">(.*?)</span>',
        html, re.S))
    rows = {}
    for m in re.finditer(r'^\|\s*`([A-Za-z0-9_-]+)`\s*\|.*$', md, re.M):
        rows[m.group(1)] = m.group(0)
    claim_ids = set(rows)

    if not deck_ids:
        print("FAIL: no data-claim attributes found in the deck")
        return 1

    missing_rows = [d for d in deck_ids if d not in claim_ids]
    num_fail = []          # (id, integer-on-slide-absent-from-row)
    for cid in deck_ids:
        if cid not in rows:
            continue
        badge = strip_commas(badges.get(cid, ""))
        rowtext = strip_commas(rows[cid])
        for tok in set(re.findall(r"\d{5,}", badge)):
            if tok not in rowtext:
                num_fail.append((cid, tok))

    print(f"deck data-claims: {len(deck_ids)}  |  CLAIMS.md rows: {len(claim_ids)}")
    for d in deck_ids:
        toks = sorted(set(re.findall(r"\d{5,}", strip_commas(badges.get(d, "")))))
        status = "OK " if d in claim_ids else "MISSING-ROW"
        badnums = [t for (i, t) in num_fail if i == d]
        if badnums:
            status = "NUM-UNSOURCED"
        print(f"  {status:14s} {d}" + (f"  (≥5-digit checked: {toks})" if toks else ""))
    orphans = sorted(claim_ids - set(deck_ids))
    if orphans:
        print("  (CLAIMS rows not used by any slide — allowed: " + ", ".join(orphans) + ")")

    ok = True
    if missing_rows:
        print(f"\nFAIL: {len(missing_rows)} deck claim(s) lack a CLAIMS.md row: "
              + ", ".join(missing_rows)); ok = False
    if num_fail:
        print("\nFAIL: slide integers absent from their ledger row: "
              + "; ".join(f"{i}:{t}" for i, t in num_fail)); ok = False
    if not ok:
        return 1
    print(f"\nOK: all {len(deck_ids)} deck data-claims sourced; all ≥5-digit slide "
          "numbers appear in their ledger rows")
    return 0


if __name__ == "__main__":
    sys.exit(main())
