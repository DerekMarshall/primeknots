#!/usr/bin/env python3
"""Referee round B2 + B3 (docs/notes/referee-round-2026-07.md) — READ-ONLY analysis of the
committed empirical curves + per-curve partials. No a_p recompute, no data written.

B2  interpolated shape invariants on the committed binned curve (all three: hump, first
    +→− zero after the hump, trough), per rung. Parabolic 3-bin interpolation for the two
    extrema; linear interpolation for the zero crossing.
B3  bootstrap over curves (the exchangeable unit): resample curves with replacement,
    re-aggregate density[b] = (Σ_c num[c][b]) / (n_curves·du), re-extract all three
    invariants; report 95% percentile CIs. Rungs with partials only (2^16, 2^17, 2^18);
    10^4 has no committed partials.

AMENDED 2026-07-18 (referee C prerequisite): extended from the trough alone to all three
invariants (hump + zero + trough) with the SAME seed discipline, so the §3 table is
uniformly treated. Pre-registered params unchanged: N_boot = 2000, seed = 20260718.

Usage:  .venv/bin/python docs/notes/referee_b2b3.py
"""
import numpy as np

N_BOOT = 2000
SEED = 20260718

RUNGS = [
    ("10^4",  "data/m4/ss_empirical.txt",  None),
    ("2^16",  "data/m5/ss_x65536.txt",     "data/m5/ss_partials_x65536.txt"),
    ("2^17",  "data/m5/ss_x131072.txt",    "data/m5/ss_partials_x131072.txt"),
    ("2^18",  "data/m5/ss_x262144.txt",    "data/m5/ss_partials_x262144.txt"),
]


def read_run(path):
    u, d, du = [], [], None
    with open(path) as f:
        for line in f:
            if line.startswith("# du"):
                du = float(line.split()[2])
            elif line.startswith("curve"):
                t = line.split()
                u.append(float(t[1]))
                d.append(float(t[2]))
    return np.array(u), np.array(d), du


def read_partials(path):
    num, NB = [], None
    with open(path) as f:
        for line in f:
            if line.startswith("# NB"):
                NB = int(line.split()[2])
            elif line.startswith("part"):
                t = line.split()
                num.append([float(x) for x in t[4:4 + NB]])
    return np.array(num), len(num), NB


def parab(u, d, du, b, kind):
    """Sub-bin extremum via a parabola through bin b and its neighbours. kind='max'|'min'.
    Returns the vertex u (falls back to the bin centre at an edge or non-extremum)."""
    if b <= 0 or b >= len(d) - 1:
        return u[b]
    y0, y1, y2 = d[b - 1], d[b], d[b + 1]
    curv = y0 - 2 * y1 + y2
    if (kind == "min" and curv <= 0) or (kind == "max" and curv >= 0):
        return u[b]
    delta = 0.5 * (y0 - y2) / curv
    return u[b] + delta * du


def invariants(u, d, du):
    """Two-pass shape extractor matching the C++ (global hump/trough, then first +→− zero
    after the hump). Returns dict of bin + interpolated positions."""
    hb = int(np.argmax(d))
    tb = int(np.argmin(d))
    hump_bin, hump_par = u[hb], parab(u, d, du, hb, "max")
    trough_bin, trough_par = u[tb], parab(u, d, du, tb, "min")
    zero = None
    for i in range(hb, len(d) - 1):
        if d[i] > 0 >= d[i + 1]:
            zero = u[i] + (u[i + 1] - u[i]) * d[i] / (d[i] - d[i + 1])
            break
    return {"hump_bin": hump_bin, "hump_par": hump_par,
            "zero": zero if zero is not None else float("nan"),
            "trough_bin": trough_bin, "trough_par": trough_par}


def ci(a):
    return np.mean(a), np.percentile(a, 2.5), np.percentile(a, 97.5)


def main():
    print("=" * 84)
    print("Referee round B2 + B3 (all three invariants)  —  seed", SEED, " N_boot", N_BOOT)
    print("=" * 84)

    for label, run_path, part_path in RUNGS:
        u, d, du = read_run(run_path)
        inv = invariants(u, d, du)
        print(f"\n### rung {label}   (du={du}, {len(u)} bins)")
        print(f"B2 hump  : bin {inv['hump_bin']:.4f}  interp {inv['hump_par']:.5f}")
        print(f"B2 zero  : interp {inv['zero']:.5f}")
        print(f"B2 trough: bin {inv['trough_bin']:.4f}  interp {inv['trough_par']:.5f}")

        if part_path is None:
            print("B3 bootstrap: UNAVAILABLE (no committed partials for this rung)")
            continue

        num, C, NB = read_partials(part_path)
        S = num.sum(axis=0)
        with np.errstate(divide="ignore", invalid="ignore"):
            ratios = S / d
        denom = float(np.median(ratios[np.isfinite(ratios)]))
        max_err = float(np.max(np.abs(S / denom - d)))
        print(f"reproduce gate: denom={denom:.6f} (n_curves*du={C * du:.6f}), "
              f"max|repro-committed|={max_err:.2e}  {'OK' if max_err <= 5e-5 else 'FAIL'}")
        if max_err > 5e-5:
            print("  !! reproduction beyond print precision -> B3 not run"); continue

        rng = np.random.default_rng(SEED)
        hp = np.empty(N_BOOT); zr = np.empty(N_BOOT); tp = np.empty(N_BOOT)
        for i in range(N_BOOT):
            db = num[rng.integers(0, C, C)].sum(axis=0) / denom
            iv = invariants(u, db, du)
            hp[i], zr[i], tp[i] = iv["hump_par"], iv["zero"], iv["trough_par"]
        for name, arr in (("hump_par", hp), ("zero", zr), ("trough_par", tp)):
            m, lo, hi = ci(arr[np.isfinite(arr)])
            print(f"B3 {name:9s}: mean={m:.5f}  95% CI [{lo:.5f}, {hi:.5f}]")


if __name__ == "__main__":
    main()
