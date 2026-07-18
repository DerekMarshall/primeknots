#!/usr/bin/env python3
"""Regenerate the data-note figures from committed artifacts (draft rule R3).

Every figure in the note is traceable to this script + committed data, never hand-produced.
Series are distinguished by line DASH and marker SHAPE, never colour alone (grayscale- and
colour-blind-safe): D(u) is a dashed line; each empirical series carries a distinct marker.

PROVENANCE CHAIN (all steps reproducible from the repo, no live oracle):
  Fig 1 (figA_ss_overlay) — the four-rung ladder: the four empirical rung curves are read
    straight from the committed run files data/m4/ss_empirical.txt (10^4) and
    data/m5/ss_x{65536,131072,262144}.txt (2^16/2^17/2^18) — their `curve` sections, the same
    committed artifacts the note's §3 table cites. The conjectured density D(u) is read from
    the emitted 2^18 overlay ss_x_extension_murmuration.json (in-house Bessel-J1, byte-portable).
  Fig B (figB_rank_split) — from ss_rank_split_murmuration.json (the m5split emit).
  Regenerate the emitted viz/data JSON first:
    ./build/bin/at emit --stage m5      --out viz/data/   (-> ss_x_extension_murmuration.json)
    ./build/bin/at emit --stage m5split --out viz/data/   (-> ss_rank_split_murmuration.json)

SKIP-clean (project rule 3 discipline): if matplotlib is absent or an input file is missing,
this exits 77 (a visible SKIP), never a silent pass or a hard failure.
"""
import json
import os
import sys

SKIP = 77
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", "..", ".."))
VIZ = os.path.join(REPO, "viz", "data")
OUT = HERE  # write figures next to this script (docs/notes/data-note/figures/)

EXT_JSON = os.path.join(VIZ, "ss_x_extension_murmuration.json")   # 2^18 overlay: D(u) + curve
SPLIT_JSON = os.path.join(VIZ, "ss_rank_split_murmuration.json")

# Fig 1 shows the FROZEN TROUGH — the note's claim — so it overlays all four empirical rungs.
# Their binned curves are read straight from the committed run files (the same artifacts the
# note's §3 table cites), each with a distinct marker SHAPE (grayscale/colour-blind-safe).
RUNS = [
    (r"$10^4$",   "o", os.path.join(REPO, "data", "m4", "ss_empirical.txt")),
    (r"$2^{16}$", "s", os.path.join(REPO, "data", "m5", "ss_x65536.txt")),
    (r"$2^{17}$", "^", os.path.join(REPO, "data", "m5", "ss_x131072.txt")),
    (r"$2^{18}$", "D", os.path.join(REPO, "data", "m5", "ss_x262144.txt")),
]


def _run_curve(path):
    """Parse a committed ss-run text file's `curve u_mid density count` section."""
    if not os.path.exists(path):
        sys.stderr.write(f"SKIP: {os.path.relpath(path, REPO)} not found.\n")
        sys.exit(SKIP)
    us, vs = [], []
    with open(path) as f:
        for line in f:
            if line.startswith("curve "):
                _, u, d = line.split()[:3]
                us.append(float(u)); vs.append(float(d))
    return us, vs


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
    """Fig 1 — the four-rung empirical ladder vs conjectured D(u). The trough stays frozen at
    u=0.8875 across 10^4..2^18 (the note's H0 claim), so all four rungs are overlaid; D(u) is
    a dashed line, each rung a distinct marker SHAPE (grayscale/colour-blind-safe), and the
    dotted vertical marks the frozen trough position."""
    du, dv = _series(_load(EXT_JSON)["density"], "d")   # conjectured D(u): emitted, byte-portable
    fig, ax = plt.subplots(figsize=(6.5, 3.9))
    ax.axhline(0, lw=0.6, color="0.6")
    ax.axvline(0.8875, lw=0.7, ls=":", color="0.55")
    ax.plot(du, dv, ls="--", lw=1.5, color="0.1", label="conjectured D(u) [SS25 Conj. 1]")
    for lab, mk, path in RUNS:
        u, v = _run_curve(path)
        ax.plot(u, v, ls="-", lw=0.8, marker=mk, ms=3.2, mfc="none",
                label=f"empirical, X={lab}")
    ax.set_xlabel("u = p / N(E)")
    ax.set_ylabel("density")
    ax.legend(fontsize=7.5, frameon=False, ncol=2, loc="lower left")
    ax.set_title("Height murmuration: four-rung empirical ladder vs conjectured D(u)", fontsize=9)
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
    # Distinct marker SHAPE per series, never colour alone (grayscale/colour-blind-safe).
    markers = {"full": "o", "leaveout": "s", "s2": "^", "s0": "D"}
    fig, ax = plt.subplots(figsize=(6.5, 3.6))
    ax.axhline(0, lw=0.6, color="0.6")
    ax.plot(du, dv, ls="--", lw=1.4, color="0.15", label="conjectured D(u)")
    for k, lab in labels.items():
        if k in j["curves"]:
            u, v = _series(j["curves"][k], "d")
            ax.plot(u, v, ls="-", lw=0.8, marker=markers[k], ms=3.5, mfc="none",
                    markevery=2, label=lab)
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
