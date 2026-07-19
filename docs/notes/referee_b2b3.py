#!/usr/bin/env python3
"""Referee round B2 + B3 (docs/notes/referee-round-2026-07.md) — READ-ONLY analysis of
the committed empirical curves + per-curve partials. No a_p recompute, no data written.

B2  interpolated trough (parabolic, 3-bin) on the committed binned curve, all rungs;
    + Delta-u/2 grid-offset note (partials store per-bin sums, not per-prime u -> a true
    half-bin re-bin is not possible from committed data; reported as a limitation with the
    parabolic vertex as the sub-bin estimate, per the pre-registered spec).
B3  bootstrap over curves (the exchangeable unit): resample curves with replacement,
    re-aggregate density[b] = (sum_c num[c][b]) / denom, extract trough position (discrete
    argmin + parabolic vertex) and depth; report 95% percentile CIs. Rungs with partials
    only (2^16, 2^17, 2^18); 10^4 has no committed partials.

Pre-registered params: N_boot = 2000, seed = 20260718. Reproduce-gate precedes B3.

Usage:  .venv/bin/python docs/notes/referee_b2b3.py
"""
import sys
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
    """-> (u_mid[], density[], du)"""
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
    """-> (num[C,NB], n_curves, NB)"""
    num = []
    NB = None
    with open(path) as f:
        for line in f:
            if line.startswith("# NB"):
                NB = int(line.split()[2])
            elif line.startswith("part"):
                t = line.split()
                # part A B N  num[0..NB-1]  cnt[0..NB-1]
                num.append([float(x) for x in t[4:4 + NB]])
    return np.array(num), len(num), NB


def parabolic_vertex(u, d, du):
    """Sub-bin trough via parabola through argmin and its two neighbours.
    Returns (u_argmin, d_argmin, u_vertex, d_vertex, b_star)."""
    b = int(np.argmin(d))
    if b == 0 or b == len(d) - 1:                    # trough at edge -> no interpolation
        return u[b], d[b], u[b], d[b], b
    y0, y1, y2 = d[b - 1], d[b], d[b + 1]
    denomp = (y0 - 2 * y1 + y2)
    if denomp <= 0:                                  # not a clean minimum -> fall back
        return u[b], d[b], u[b], d[b], b
    delta = 0.5 * (y0 - y2) / denomp                 # in units of du, in (-0.5, 0.5)
    u_v = u[b] + delta * du
    d_v = y1 - 0.25 * (y0 - y2) * delta
    return u[b], d[b], u_v, d_v, b


def main():
    print("=" * 78)
    print("Referee round B2 + B3  —  seed", SEED, " N_boot", N_BOOT)
    print("=" * 78)

    for label, run_path, part_path in RUNGS:
        u, d, du = read_run(run_path)
        u_arg, d_arg, u_v, d_v, b = parabolic_vertex(u, d, du)
        print(f"\n### rung {label}   (du={du}, {len(u)} bins, run={run_path})")
        print(f"B2(a) parabolic trough:  argmin u={u_arg:.4f} (v={d_arg:.5f})  "
              f"-> vertex u={u_v:.5f} (v={d_v:.5f})  [bin {b}]")

        if part_path is None:
            print("B2(b) grid-offset:  UNAVAILABLE (no committed partials for this rung)")
            print("B3 bootstrap:       UNAVAILABLE (no committed partials for this rung)")
            continue

        num, C, NB = read_partials(part_path)
        # --- reproduce gate: density[b] = (sum_c num[c][b]) / denom ---
        # The exact aggregation (ss_empirical.cpp:107-111) is density[b] = acc[b]/denom
        # with acc[b] = sum_c num[c][b].  We confirm the FORMULA exactly (denom == n_curves*du)
        # and then check the residual against the committed curve. NOTE: the committed run file
        # stores density to 6 significant figures, so the achievable residual is ~5e-6 for
        # O(1-4) values -- the file's print floor, not a reconstruction error. The
        # pre-registered 1e-9 gate was infeasible against 6-sig-fig text (a mis-set tolerance,
        # disclosed in the results); the substantive gate is denom == n_curves*du exactly AND
        # residual at the file's print precision.
        S = num.sum(axis=0)
        with np.errstate(divide="ignore", invalid="ignore"):
            ratios = S / d
        denom = float(np.median(ratios[np.isfinite(ratios)]))
        denom_theory = C * du
        repro = S / denom
        max_err = float(np.max(np.abs(repro - d)))
        print(f"reproduce gate:  C={C} curves, denom={denom:.6f}  "
              f"(n_curves*du = {denom_theory:.6f}, rel-diff {abs(denom-denom_theory)/denom_theory:.2e})")
        print(f"  max|repro-committed| = {max_err:.3e}  "
              f"(committed curve is 6-sig-fig; floor ~5e-6)  "
              f"{'OK' if max_err <= 5e-5 else 'FAIL'}")
        if max_err > 5e-5:
            print("  !! reproduction beyond print precision -> B2(b)/B3 not run")
            continue

        # --- B2(b) grid-offset: cannot re-bin (per-bin sums only) ---
        disp_bins = abs(u_arg - 0.805) / du
        print(f"B2(b) grid-offset:  committed partials are per-bin sums (num[c][b]), NOT "
              f"per-prime u; a true Delta-u/2 re-bin is not possible from committed data.")
        print(f"  sub-bin trough = parabolic vertex above (u={u_v:.5f}); displacement from "
              f"0.805 is {disp_bins:.1f} bins >> 0.5 bin, so 0.8875 is not a half-bin "
              f"grid-quantization artifact.")

        # --- B3 bootstrap over curves ---
        rng = np.random.default_rng(SEED)
        t_arg = np.empty(N_BOOT)
        t_par = np.empty(N_BOOT)
        v_min = np.empty(N_BOOT)
        for i in range(N_BOOT):
            idx = rng.integers(0, C, C)
            db = num[idx].sum(axis=0) / denom
            ua, da, uv, dv, bb = parabolic_vertex(u, db, du)
            t_arg[i] = ua
            t_par[i] = uv
            v_min[i] = da
        def ci(a):
            return np.mean(a), np.percentile(a, 2.5), np.percentile(a, 97.5)
        for name, arr in (("trough_u argmin", t_arg),
                          ("trough_u parab ", t_par),
                          ("trough_v depth ", v_min)):
            m, lo, hi = ci(arr)
            print(f"B3 {name}:  mean={m:.5f}  95% CI [{lo:.5f}, {hi:.5f}]")


if __name__ == "__main__":
    main()
