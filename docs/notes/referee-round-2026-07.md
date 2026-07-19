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

---

## Block B — specs (pre-registered; committed *before* any B run)

Descriptive designs only — **no verdict / pass-fail clauses** (the #19/#26 lesson: a
threshold set or changed after seeing results is the failure mode). Each study says what
is measured and reported; interpretation is deferred to Block C. Every result is reported
**as measured**, including any that weakens the ladder. Fixed seeds/params below make the
runs reproducible. Inputs are the committed files only; no a_p recompute.

**Data availability (governs per-rung coverage).** Committed binned curves exist for all
four rungs (`data/m4/ss_empirical.txt` = 10⁴; `data/m5/ss_x{65536,131072,262144}.txt` =
2¹⁶/2¹⁷/2¹⁸). Per-curve partials (`num[c][b]`, ε folded in, NB=40, du=0.025) exist for
**2¹⁶/2¹⁷/2¹⁸ only** (`data/m5/ss_partials_x{65536,131072,262144}.txt`). **10⁴ has no
committed partials**, so any partials-based analysis (B2 grid-offset, B3 bootstrap) covers
2¹⁶–2¹⁸; 10⁴ is reported as unavailable, not skipped silently.

**Aggregation (shared).** The committed density is `density[b] = (Σ_c num[c][b]) / denom`,
a linear mean over curves (`ss_empirical.cpp:107–111`). Linearity ⇒ resampling curves and
re-averaging is valid. **Gate for B2(offset)/B3:** the re-aggregation must reproduce the
committed `density[b]` (hence trough at u = 0.8875) to ≤ 1e-9 before any offset/resample;
if it does not, the run stops and the discrepancy is reported (no analysis on an
unverified reconstruction).

### B1 — density truncation study

Vary the truncation cutoff B (both bounds: q ≤ B squarefree, m ≤ B) in
`at::murm::ss_density` / `ss_shape(B, du)` (the emitter's own formula-side extractor), and
report the density shape as a function of B.

- **Core grid (as specified):** B ∈ {500, 750, 1000, 1250, 1500, 1750, 2000}.
- **Extension (added; motivated by A.4):** B ∈ {3000, 5000, 8000, …} up to the largest B
  that runs in a few minutes with the naive double sum; report the largest B reached.
  *Matching SS25's B = 10⁵ exactly is infeasible here* (naive Σ_{q,m≤B} is ~B² terms per
  u; SS25 use FFT). SS25's Table 1 puts their resolution threshold at B ≥ 2¹⁵ = 32768;
  the extension brackets toward it as far as feasible. This is disclosed, not hidden.
- **Scan:** du = 0.005 on u ∈ (0,1]; the same two-pass `extract_shape` used by the emitter.
- **Reported per B:** trough_u, trough_v (depth), and the first post-hump +→− zero_u.
  No threshold; the table of (B → trough_u, zero_u, trough_v) is the deliverable.

### B2 — interpolated trough + Δu/2 grid-offset, per rung

From the committed data (no recompute):

- **(a) Parabolic interpolation (all four rungs, binned curve).** Around the argmin bin,
  fit a parabola through that bin and its two neighbors; report the vertex u (sub-bin
  trough position) and vertex value. Reported alongside the raw argmin-bin-center u.
- **(b) Δu/2 grid-offset (2¹⁶/2¹⁷/2¹⁸, from partials).** Re-bin each curve's prime
  contributions onto a bin grid whose edges are shifted by Δu/2 = 0.0125, re-aggregate,
  and report the argmin-bin-center trough_u on the shifted grid. *Caveat:* the committed
  partials store per-bin sums `num[c][b]`, not per-prime (A,B,p) data, so a true half-bin
  re-bin requires the primes' u-values. If the committed partials do not carry enough
  resolution to re-bin at Δu/2, this is reported as a limitation and the offset is instead
  estimated by the parabolic-vertex shift; which path was taken is stated explicitly.
- **Reported per rung:** argmin-bin trough_u, parabolic-vertex trough_u, and (where
  available) the Δu/2-offset trough_u. No threshold.

### B3 — bootstrap over curves (the exchangeable unit)

For each rung with partials (2¹⁶, 2¹⁷, 2¹⁸):

- Resample n curves with replacement from the n committed curves (n = 5042 / 9014 / 15936),
  re-aggregate `density[b]`, extract the trough. **N_boot = 2000**, seed = **20260718**
  (committed here). The reproduction gate above runs first.
- **Reported per rung:** for the trough *position* — both the discrete argmin-bin-center
  (bin-quantized at Δu = 0.025) and the parabolic-vertex position; mean and 2.5 / 97.5
  percentile CI for each. For the trough *depth* (trough_v): mean and 2.5 / 97.5
  percentile CI. The bin quantization of the discrete position is stated (its CI is
  coarse by construction; the parabolic CI is the finer read). No threshold.

**Pre-registration marker.** This section is committed before B1/B2/B3 are implemented or
run; the results section below is a later commit. The git history is the audit trail
(the #19/#26 remedy).

---

## Block B — results

All numbers **as measured**. No verdict — interpretation is deferred to Block C. Tooling:
`at ss-density-scan` (B1, committed), `docs/notes/referee_b2b3.py` (B2/B3, committed).

### B1 — density truncation study (`ss-density-scan --du 0.005`)

| B (q,m ≤ B) | hump_u | zero_u | **trough_u** | trough_v |
|---|---|---|---|---|
| 500  | 0.4750 | 0.644767 | **0.805000** | −3.37441 |
| 750  | 0.4750 | 0.644696 | **0.805000** | −3.35437 |
| 1000 | 0.4750 | 0.644705 | **0.805000** | −3.36902 |
| 1250 | 0.4750 | 0.644721 | **0.805000** | −3.36349 |
| 1500 | 0.4750 | 0.644915 | **0.805000** | −3.40450 |
| 1750 | 0.4750 | 0.644927 | **0.805000** | −3.40007 |
| 2000 | 0.4750 | 0.644922 | **0.805000** | −3.40028 |
| 3000 | 0.4750 | 0.644918 | **0.805000** | −3.40574 |
| 5000 | 0.4750 | 0.644901 | **0.805000** | −3.40350 |
| 8000 | 0.4750 | 0.644949 | **0.805000** | −3.38777 |
| 12000| 0.4750 | 0.644953 | **0.805000** | −3.38859 |

**trough_u = 0.805000 exactly at all 11 cutoffs** — a 24× range (2⁹ → 2¹³·⁵), zero movement. hump_u
is constant (0.4750); zero_u is stable to ±0.0002 (0.6447–0.6450); trough_v (depth) wobbles within
[−3.406, −3.354] with no trend. **This refutes the A.4 truncation hypothesis:** the conjectured
density D(u) troughs at 0.805, and that position is truncation-stable — it does not drift toward
~0.88 as B grows. (The extension was capped at B = 12000 ≈ 2¹³·⁵, which took 6.6 s… 6.6 min; the
naive Σ_{q,m≤B} is ~B² per u-point, so SS25's B = 10⁵ / their 2¹⁵ resolution threshold is beyond
the naive method. The 24× range with zero position movement makes further B overwhelmingly likely
to hold 0.805 — a limitation of reach, not an open question of the trend.)

**Consequence:** the empirical trough (~0.88, B2/B3) versus the density trough (0.805, B1) is a
**real displacement, not a truncation artifact.** ~0.077 in u, ≈ 3.3 bins.

### B2 — interpolated trough + grid-offset

Reproduce gate (per rung, from committed partials): `denom` = n_curves·du **exactly** (126.049989
vs 126.05 at 2¹⁶, rel-diff 8.8e-8; likewise 2¹⁷/2¹⁸), confirming the re-aggregation formula. The
residual vs the committed curve is ~5e-6 — **the committed run file's 6-significant-figure print
floor**, not a reconstruction error. *(Disclosure: the pre-registered gate said "≤ 1e-9"; that is
infeasible against a 6-sig-fig text file regardless of correctness — a mis-set tolerance. The
substantive gate — `denom` = n_curves·du exactly, residual at the file's print precision — passes.
The gate constant was corrected in the committed script with this note; no scientific threshold was
moved.)*

| rung | argmin bin u (v) | **parabolic-vertex u (v)** |
|---|---|---|
| 10⁴  | 0.8875 (−3.47052) | **0.88952** (−3.47645) |
| 2¹⁶  | 0.8875 (−3.71505) | **0.88227** (−3.76614) |
| 2¹⁷  | 0.8875 (−3.65241) | **0.88176** (−3.70297) |
| 2¹⁸  | 0.8875 (−3.57994) | **0.88216** (−3.61546) |

Sub-bin empirical trough ≈ **0.882–0.890**, all four rungs. **Δu/2 grid-offset:** the committed
partials store per-bin sums (`num[c][b]`), not per-prime u, so a true half-bin re-bin is not
possible from committed data (pre-registered fallback taken). The displacement from 0.805 is
**3.3 bins ≫ 0.5 bin**, so 0.8875 is not a half-bin grid-quantization artifact. (10⁴ has no
committed partials: parabolic only, no bootstrap.)

### B3 — bootstrap over curves (N_boot = 2000, seed = 20260718)

| rung | trough_u argmin: mean [95% CI] | trough_u parabolic: mean [95% CI] | trough_v depth: mean [95% CI] |
|---|---|---|---|
| 2¹⁶ | 0.89100 [0.88750, **0.98750**] | 0.88617 [0.87530, **0.98750**] | −3.72430 [−4.17405, −3.25738] |
| 2¹⁷ | 0.88929 [0.88750, 0.88750] | 0.88391 [0.87521, 0.88646] | −3.65963 [−4.03699, −3.30830] |
| 2¹⁸ | 0.88730 [0.88750, 0.88750] | 0.88208 [0.87611, 0.88567] | −3.57758 [−3.88600, −3.28405] |

Two findings, reported flat:
1. **Trough position is bootstrap-stable at 2¹⁷ and 2¹⁸** (argmin CI is a single bin, 0.8875;
   parabolic CI ≈ [0.876, 0.886]) — the ~0.88 trough is not a sampling accident there. **At 2¹⁶ it
   is looser:** the argmin's upper CI reaches 0.98750 — under resampling the smaller family (5042
   curves) occasionally puts its global minimum in the far-right tail rather than at ~0.88.
2. **(Adverse to the ladder.) The trough-*depth* CIs overlap heavily** across the three rungs
   ([−4.17, −3.26] / [−4.04, −3.31] / [−3.89, −3.28]). The depth-easing means the note reports as a
   supporting observation (−3.72 → −3.66 → −3.58) sit well inside this bootstrap noise, so that
   easing is **not a curve-bootstrap-resolved monotone trend**. Recorded as measured.

### B — summary of the measured picture (no verdict)

- Density trough (D(u), B1): **0.805, truncation-stable** over 2⁹–2¹³·⁵ → the 0.805 target is real,
  not under-truncated (A.4 refuted).
- Empirical trough (B2 parabolic; B3 CI): **~0.882–0.889, bootstrap-stable at 2¹⁷/2¹⁸**, looser at
  2¹⁶ → the ~0.88 position is real, ≈ 3.3 bins from 0.805, not a grid artifact.
- Trough **depth** easing: **within bootstrap noise** (B3) — not a resolved trend.
- (From A) our rungs sit where SS25's own mean |empirical − density| is 0.73–1.17, converging only
  by 2²⁸.

Interpretation (is the real, bootstrap-stable ~0.077 displacement a finite-X effect consistent with
SS25's documented slow convergence, or something else, and how the depth-easing should be reworded
given B3) is **Block C**.

---

## Block C — GATE (digitize SS25's density): **STOP**

**Task.** Before the reframe prose, digitize SS25's density (green) curve and locate its trough
numerically, to resolve the A.4 eyeball (~0.85–0.90) against B1's stable 0.805. Resolve to ~0.805 →
correction line + proceed; otherwise → **stop and report**.

**Method.** 400-dpi renders of Fig 1 (P=∞, the eq-(2) density at B=10⁵) and Fig 5 (P=2,4,8,16). The
green (density) curve is isolated by colour (`docs/notes/referee_digitize.py`), pixel-x is
self-calibrated to u by the curve's [0,1] span, and the **hump is the calibration check** — our D(u)
hump is 0.475 (B1), so a digitized hump near 0.475 validates the mapping.

**Result — the gate does NOT resolve to 0.805.**

| curve | hump u | trough u |
|---|---|---|
| SS25 Fig 1 (P=∞, B=10⁵) green | 0.466 | **0.871** |
| SS25 Fig 5 P=2 / 4 / 8 / 16 green | 0.466 | **0.872 / 0.868 / 0.872 / 0.872** |
| our `ss_density` (B=2000; B1 to 12000) | 0.475 | **0.807** (0.805 argmin) |

The humps agree (0.466 vs 0.475), so the calibration is sound and this is **not** a global shift. A
z-scored overlay of our density against the digitized Fig 1 green curve: gross-shape correlation
0.64, best agreement in the hump region [0.45,0.65] (corr 0.64), **worst in the trough region
[0.80,0.95] (corr 0.25, mean |Δz| = 1.04)**. The two curves are the same family but **diverge
specifically where the trough sits.** In the Fig 5 P=2 panel the green line and purple dots are
coincident (at X=2²⁸ empirical ≈ density) and the deepest excursion is visibly ~0.87–0.88.

**SS25's density troughs at ~0.87. Ours troughs at 0.805.** This is a real shape discrepancy in the
trough region between our `ss_density` (our reading of Conjecture 1 / eq (2)) and SS25's published
density.

### What this overturns (flag, don't smooth)

- **A.4's eyeball (~0.85–0.90) was right; B1's "A.4 refuted, displacement real" was wrong.** B1's
  *measurements* stand (our D(u) troughs at 0.805, truncation-stable; the empirical troughs at ~0.88,
  B2/B3). What is overturned is the *interpretation* that 0.805 is the correct density target. It is
  **our** density that is the discrepant party — the presumption (rule 1: the paper is normative) is
  that our implementation misreads eq (2) in the tail.
- **The central "trough displacement" is very likely spurious.** SS25's density trough (~0.87) ≈ the
  empirical trough (~0.88). If our density is corrected to ~0.87, the empirical trough **agrees**
  with the density and the note's "open deviation / H0-persistent" story largely dissolves. The
  ~0.0825 displacement was our-density-vs-empirical, with our density mislocated — not
  empirical-vs-truth.

### Candidate causes (not pre-judged — for the debugging task)

The hump is right and the tail is wrong, so a factor that grows toward large u: the local factors
ℓ_{p,ν} / ℓ̂_{p,ν} (Lemmas 3, 4), the **weight ≥ 12 eigenvalue-sum truncation** (`ss_density.cpp`
returns 0 above weight 12), the J₁ argument / √u weight, or a u = p/N convention difference that only
bites in the tail. Rule 1 applies with full force: fix the *reading of eq (2)* against [SS25], never
tune the trough to a target.

### Recommendation (STOP)

No reframe prose is written; the data-note narrative is untouched. Next step is an **M4 density
investigation** — debug `ss_density` against [SS25] Lemmas 3/4 + Conjecture 1 until it reproduces the
published density trough (~0.87), then re-pin, re-emit, and **re-run this entire A/B comparison** on
the corrected density. Only then does a Block-C reframe make sense — and it may be a different
reframe (agreement, not deviation). Reported to Derek for direction.
