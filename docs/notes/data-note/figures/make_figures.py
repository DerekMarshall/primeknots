#!/usr/bin/env python3
"""Regenerate the data-note figures from committed artifacts (draft rule R3).

STATUS: STUB / DRAFT. Structurally complete and runnable, but print styling (fonts,
sizes, colour-blind-safe palette, final figure numbering) is [PENDING] until §3/§4 land.
It is checked in NOW so that every figure in the note is traceable to a script + committed
data, never hand-produced.

PROVENANCE CHAIN (all steps reproducible from the repo, no live oracle):
    committed  data/m5/ss_x{65536,131072}.txt   (the ss-run: a_p computed, N/eps oracle-input)
      └─ ./build/bin/at emit --stage m5     --out viz/data/   ->  sawin_sutherland_murmuration.json
      └─ ./build/bin/at emit --stage m5split --out viz/data/  ->  ss_rank_split_murmuration.json
           └─ this script                                     ->  figures/*.pdf
viz/data/ is gitignored (emitted, not committed); regenerate it with the two `at emit`
commands above before running this script. The conjectured density D(u) in each JSON is the
in-house Bessel-J1 formula (byte-portable); the empirical series are the committed statistic.

SKIP-clean (project rule 3 discipline): if matplotlib is absent or the emitted JSON has not
been regenerated, this exits 77 (a visible SKIP), never a silent pass or a hard failure.
"""
import json
import os
import sys

SKIP = 77
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", "..", ".."))
VIZ = os.path.join(REPO, "viz", "data")
OUT = HERE  # write figures next to this script (docs/notes/data-note/figures/)

SS_JSON = os.path.join(VIZ, "sawin_sutherland_murmuration.json")
SPLIT_JSON = os.path.join(VIZ, "ss_rank_split_murmuration.json")


def _load(path):
    if not os.path.exists(path):
        sys.stderr.write(
            f"SKIP: {os.path.relpath(path, REPO)} not found — regenerate with "
            f"`at emit` (see this script's header).\n")
        sys.exit(SKIP)
    with open(path) as f:
        return json.load(f)


def _series(items, key="d"):
    return [it["u"] for it in items], [it[key] for it in items]


def fig_ss_overlay(plt):
    """Fig A — empirical statistic (1) vs conjectured density D(u), at X_confirm."""
    j = _load(SS_JSON)
    eu, ev = _series(j["empirical"], "emp")   # empirical D-hat
    du, dv = _series(j["density"], "d")        # conjectured D(u)
    X = j["params"].get("X_confirm", "?")
    fig, ax = plt.subplots(figsize=(6.5, 3.6))
    ax.axhline(0, lw=0.6, color="0.6")
    ax.plot(du, dv, lw=1.4, label="conjectured D(u) [SS25 Conj. 1]")
    ax.plot(eu, ev, marker="o", ms=2.5, lw=1.0, label=f"empirical statistic (1), X={X}")
    ax.set_xlabel("u = p / N(E)")
    ax.set_ylabel("density")
    ax.legend(fontsize=8, frameon=False)
    ax.set_title("[DRAFT] Height murmuration: empirical vs conjectured", fontsize=9)
    fig.tight_layout()
    out = os.path.join(OUT, "figA_ss_overlay.pdf")
    fig.savefig(out)
    return out


def fig_rank_split(plt):
    """Fig B — four-curve overlay: full / leave-out(F\\S2) / S2 / S0 + D(u) (PR-2)."""
    j = _load(SPLIT_JSON)
    du, dv = _series(j["density"], "d")
    labels = {"full": "full family", "leaveout": "F \\ S2 (leave-out)",
              "s2": "S2 (analytic rank 2)", "s0": "S0 (analytic rank 0)"}
    fig, ax = plt.subplots(figsize=(6.5, 3.6))
    ax.axhline(0, lw=0.6, color="0.6")
    ax.plot(du, dv, lw=1.4, color="0.3", label="conjectured D(u)")
    for k, lab in labels.items():
        if k in j["curves"]:
            u, v = _series(j["curves"][k], "d")
            ax.plot(u, v, lw=1.0, marker=".", ms=2, label=lab)
    ax.set_xlabel("u = p / N(E)")
    ax.set_ylabel("density")
    ax.legend(fontsize=8, frameon=False)
    ax.set_title("[DRAFT] Analytic-rank split overlay (PR-2)", fontsize=9)
    fig.tight_layout()
    out = os.path.join(OUT, "figB_rank_split_overlay.pdf")
    fig.savefig(out)
    return out


def main():
    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except Exception as e:  # noqa: BLE001
        sys.stderr.write(f"SKIP: matplotlib unavailable ({e}); figures not regenerated.\n")
        sys.exit(SKIP)

    made = [fig_ss_overlay(plt), fig_rank_split(plt)]
    for p in made:
        sys.stderr.write(f"wrote {os.path.relpath(p, REPO)}\n")
    # [PENDING] final print styling + figure numbering, fixed when §3/§4 are drafted.


if __name__ == "__main__":
    main()
