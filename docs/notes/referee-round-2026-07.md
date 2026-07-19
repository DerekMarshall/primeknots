# Referee-response round — 2026-07 (findings log)

Structured response to the referee round of 2026-07-18 (Derek). Blocks A, B (B1/B2/B3),
D. **Nothing here edits the data-note narrative** — this is a findings log; the prose
reframe is Block C (Derek issues separately, after A+B land). All numbers are reported
**as measured**, adverse ones included. B specs are committed *before* their runs
(pre-registration, RESEARCH-M §7 / ERRATA #19, #26).

Source of truth for SS25 numbers: `docs/papers/sawin-sutherland-2504.12295.pdf`
(arXiv:2504.12295v2), transcribed from the paper (rule 8 — not from memory).

---

## Block A — where SS25's own data sits at matched X (gates C)

**Question.** At the bottom of SS25's 2¹⁶–2²⁸ window, matched to our rungs, where does
*their* empirical curve sit relative to the conjectured density (2), and relative to our
trough at u = 0.8875 and our "0.805 target"?

### A.1 Same object confirmed (family-size cross-check)

SS25 Table 4 (p.45) lists `#H(X)`, the count of curves with H(E) ≤ X, alongside their
LHS/RHS comparison. At our three rungs these are **identical** to ours:

| X | SS25 `#H(X)` | our `\|fam\|` |
|---|---|---|
| 2¹⁶ | 5042 | 5042 |
| 2¹⁷ | 9014 | 9014 |
| 2¹⁸ | 15936 | 15936 |

Same naive height H(E) = max(4|A|³, 27|B|²), same statistic (1), same u = p/N ∈ (0,1]
convention (their C₁ = j/2000). We are computing the same LHS object — the comparison is
apples-to-apples.

### A.2 SS25's own empirical-vs-density discrepancy, per X (Table 4, P = ∞ column)

Table 4's P = ∞ column is the mean over bins of |LHS(j, X) − density(2)|, i.e. how far
SS25's *own* empirical curve is from the conjectured density at each height X:

| X | mean \|empirical − density\| (SS25) |
|---|---|
| **2¹⁶** | **1.1693** |
| **2¹⁷** | **0.9110** |
| **2¹⁸** | **0.7252** |
| 2¹⁹ | 0.5751 |
| 2²⁰ | 0.4597 |
| 2²¹ | 0.3731 |
| 2²² | 0.3067 |
| 2²³ | 0.2548 |
| 2²⁴ | 0.2139 |
| 2²⁵ | 0.1769 |
| 2²⁶ | 0.1443 |
| 2²⁷ | 0.1205 |
| 2²⁸ | 0.1013 |

**At our rungs (2¹⁶–2¹⁸) SS25's own curve is 0.73–1.17 (mean, per bin) away from the
density.** The discrepancy decays monotonically but slowly, reaching agreement (~0.10)
only at **2²⁸** — twelve doublings above our top rung. In the range we can reach, SS25
themselves do **not** see the density; the finite-X curve is still far from D(u).

### A.3 SS25 explicitly document the finite-X effect

Verbatim, p.3–4:

> "part of the discrepancy … comes from a persistent downward bias, where (1) is slightly
> less than its predicted limiting value. … This downward bias seems to decay as X → ∞,
> suggesting it will go away in the limit, though **it may decay more slowly than other
> sources of discrepancy** between (1) and the right-hand side of (2)."

They float rank-≥2 over-representation as a candidate mechanism (p.4), noting it is not
obviously accounted for by their Theorem 2. So the persistent-at-finite-X, slow-decaying
gap between empirical and density is a phenomenon the *authors* report, not an artifact
unique to our replication.

### A.4 Trough *position*: SS25 publish none per X; the 0.805 target is truncation-bound

SS25 do **not** tabulate an empirical trough *position* at any X. Their only published
curves are at X = 2²⁸ (Figs 1, 5) and 2³⁰ (Fig 4). A visual read of Fig 1 / Fig 5 (X =
2²⁸): both the empirical (purple) trough and the B = 10⁵ density (green) trough sit near
**u ≈ 0.85–0.90 — not 0.805** (eyeball, ±~0.05; flagged for the rigorous B1 test).

This points at our comparison target. Our density trough "0.805" is extracted from our
own D(u) evaluation, truncated to q, m ≤ B with **B ∈ [200, 5000]** (per
`emit_sawin_sutherland.cpp:32`; ≈ 2⁸–2¹²·³). SS25 use **B = 10⁵** and their Table 1 (p.43)
shows the B→2B change in the density is ~0.06 at B = 2¹⁰ and only drops below the 0.025
pixel resolution at **B ≥ 2¹⁵** (their stated cutoff choice). **Our density was truncated
an order of magnitude below SS25's converged regime**, so 0.805 may be an under-truncated
trough *position*, not the true density trough.

### A.5 Consequences for the ladder (setup for B and C)

1. The d = 0.0825 trough displacement is a comparison of our empirical trough (0.8875)
   against a density trough (0.805) computed at B ≤ 5000. **B1 tests directly whether
   that 0.805 moves as the cutoff grows** — if the density trough drifts toward ~0.88 as
   B increases, the "displacement" shrinks or vanishes (it was our-density-vs-true-density,
   not empirical-vs-density).
2. Independently, our rungs sit where SS25's own mean discrepancy is 0.73–1.17; agreement
   arrives only at 2²⁸. Any "persistent at ≤ 2¹⁸" statement is consistent with SS25's
   documented slow finite-X convergence — the ladder is far below the height where the
   density becomes visible.
3. Our trough *depth* easing (−3.72 → −3.65 → −3.58 across 2¹⁶–2¹⁸) has the same sign as
   SS25's decaying downward bias. (Depth CIs: Block B3. Whether depth converges toward the
   density-trough depth needs the B1 density value at higher B.)

**A does not itself rewrite any claim** — it establishes that (i) we compute SS25's object,
(ii) SS25's own curve is 0.7–1.2 off the density at our heights, (iii) 0.805 is a
truncation-bound quantity B1 must re-measure. These gate the C reframe.
