# M1 pinning log — replicate the HLOP murmuration [soft anchor]

Two-phase gate for Stage M1 (RESEARCH-M §3). **Phase 1 (this document): pinning
only** — the family, statistic, ranges, and conventions are pinned against the
fetched HLOP PDF (`docs/papers/hlop-2204.10140.pdf` = arXiv:2204.10140), quoted
verbatim, before any averaging pipeline, emitter, or viewer is written. **STOP at
the pinning review.** RESEARCH-M.md is a paraphrase; HLOP is normative.

Anchor is *soft*: the published figures (Figure 1) plus the tabulated family counts
(Example 1). §3's bad-prime a_q conversion (m0-pinning §3) is load-bearing here.

---

## P1 — the exact statistic

**Quoted, HLOP §1, eq. (1.1):**
> "f_r(n) = (1 / #E_r[N₁,N₂]) · Σ_{E ∈ E_r[N₁,N₂]} a_{pₙ}(E), where N₁ < N₂ ∈ ℤ_{>0},
> and E_r[N₁,N₂] is the set of (representatives for the isogeny classes of) elliptic
> curves over ℚ with rank r and conductor in range [N₁,N₂]."

**Averaging — arithmetic mean, unweighted (quoted, §3.2):**
> "By averaging (arithmetic mean), we construct a single value f_r(n), representative
> of the set of values {a_{pₙ}(E) : E ∈ E_r[N₁,N₂]}."

So the statistic is the **unweighted arithmetic mean** of a_{pₙ} over the family; no
root-number or other weighting in Figure 1. One term per isogeny class (§3.1/§3.2:
"our datasets actually have only one representative curve for each isogeny class").

**As a function of the prime (quoted, §1, eq. (1.2), used in §4.5):**
> "g_r(p) = (1 / #E_r[N₁,N₂]) · Σ_{E ∈ E_r[N₁,N₂]} a_p(E)."

**Bad primes enter the sum (load-bearing, cross-reference m0-pinning §3).** The sum
runs over a_{pₙ}(E) for the n-th prime pₙ with **no exclusion of pₙ | N_E**; a_p(E)
= p + 1 − #E(𝔽_p) at good p (HLOP §2), and at bad p is the standard 0/±1 (HLOP §2.3:
good `L_p = 1 − a_p T + pT²`, bad the degree-≤1 analogue). Since a_p is an isogeny
invariant, the per-class value is well-defined. When pₙ | N_E, the term is obtained
from ecdata's Atkin–Lehner sign via the pinned conversion `ell::ap_from_atkin_lehner`
(m0-pinning §3c: multiplicative a_q = −w_q, additive a_q = 0). An unconverted sign
would bias exactly the curves whose bad prime lands at index n — the M0-guarded
failure mode, here made live at family scale.

## P2 — family classes: rank (not root number)

Figure 1 classes by **rank** r ∈ {0,1,2} (E_r = rank-r isogeny classes, eq. 1.1).
Root number is *related* (HLOP §4.1: the parity conjecture ties rank parity to the
root-number sign, "a theorem for curves of rank r ∈ {0,1}"), but Figure 1 itself is
rank-classed. Root-number-*weighted* averages are the framing of the later proven
cases (Zubrilina → M3, Sawin–Sutherland → M4), not of the HLOP replication.

**Provenance:** rank is read from ecdata (`allcurves` column R) = **oracle**, and is
isogeny-invariant. No `theorem_` conclusion may rest on it except the stated family
definition. *Caveat:* the ecdata format doc / README carry no explicit rank-
certification caveat; Cremona's ranks over N ≤ 500,000 are the standard rigorously-
determined values and are cited as such (oracle). If a later range introduces
conditional ranks, that must be re-pinned (none in the M1 range).

## P3 — conductor and prime ranges (the figures to replicate)

**Quoted, Figure 1 caption:**
> "(Top) Plots of the functions f₀(n) (blue) and f₁(n) (red) for 1 ≤ n ≤ 1000 and
> [N₁,N₂] = [7500, 10000]. (Bottom) Plots of the functions f₀(n) (blue) and f₂(n)
> (green) for 1 ≤ n ≤ 1000 and [N₁,N₂] = [5000, 10000]."

**Quoted, Example 1:** "We have p₁₀₀₀ = 7919." So the prime range is the **first 1000
primes, p₁ = 2 … p₁₀₀₀ = 7919** (indices 1 ≤ n ≤ 1000).

- Conductor ranges: **[7500,10000]** (r ∈ {0,1}) and **[5000,10000]** (r ∈ {0,2}),
  **inclusive** — see P5 (the published counts reproduce exactly under N₁ ≤ N ≤ N₂).
- **Data slice extended (standing note / P3).** The ranges reach N = 10000, beyond
  the M0 slice (N < 10000). The fetch was extended to add the `10000-19999` ecdata
  chunk so [5000,10000] is fully covered; `data/cremona/MANIFEST.json` now pins 4
  files at commit `25cec5ec…`:

  | file | sha256 (16) | bytes |
  |---|---|---|
  | allcurves.00000-09999 | `259f3846329395b3` | 2146401 |
  | allcurves.10000-19999 | `8cb6f95c0a42e8ae` | 2416177 |
  | aplist.00000-09999 | `a1cc11fb2c53b58b` | 3809559 |
  | aplist.10000-19999 | `e637c3f3899eaa05` | 4402395 |

  The slice follows the paper's ranges, not the reverse (m0-pinning standing note).

## P4 — binning, and the scale-collapse test design

**Binning: none in Figure 1.** HLOP plot (n, f_r(n)) — one point per prime index n
(equivalently g_r(p) per prime p, eq. 1.2). No binning, no smoothing; the M1
`murmuration_curve` reproduces the per-prime averages directly.

**Scale invariance** is deferred by HLOP to [S22]/[HLOPS] (§1: "there is a certain
invariance to the oscillations as the intervals are scaled to include larger
conductors [S22]"). So the scale-collapse is a test *I* design (a `theorem_`-style
invariant, not an HLOP figure). Design, fixed **before any confirmation run**:

- **Disjoint ranges:** [2500,5000] vs [5000,10000] (a factor-2 scaling, both within
  the pinned slice; disjoint conductor sets), rank class r = 0.
- **Common axis:** y = p/N₂ for each range (or y = p/N with N the range midpoint —
  the exact choice pinned here as **y = p/N₂**, matching the "p up to ~N" framing).
- **Interpolation:** each range's g₀(p) is linearly interpolated onto a shared grid
  of y-values (the overlap of the two ranges' y-supports), sampled at a fixed step.
- **Distance:** L² / RMS difference D = sqrt( mean_y (gᴬ(y) − gᴮ(y))² ) over the
  shared grid.
- **Tolerance, with justification, pinned now (no shopping — the M-side Rule 1):**
  the per-point mean has sampling variance Var(mean) ≈ p / |E_r| (HLOP §3.2:
  std(a_p) ~ √p). Two range curves differ, from noise alone, by an RMS floor whose
  variance is the sum of the two families' — the **two-family form**, F² =
  mean_y( p_A(y)/|E_A| + p_B(y)/|E_B| ), p_X(y) = y·N2_X. (This is the exact
  instantiation of the √2·RMS(√p/√|E|) heuristic in the earlier pin; they coincide
  for equal families. Recorded as a pre-run refinement, not a loosening.) **Tolerance
  T = 3·F**; the run PASSES iff D < T. T is computed from the manifest-pinned data
  *before* the run; D ≥ T is a recorded deliverable, never a reason to raise T.
- **Floor honesty (rider R1a).** F assumes the a_p are **independent** across the
  family with variance ~p. That is an assumption, and **anticonservative** for
  detection: the murmuration IS a cross-family *correlation*, and positive
  correlation would inflate the true Var(mean) above p/|E_r|, making the real F
  larger and the reported ratio (D/F) an *upper*-biased optimism. So a passing
  collapse is necessary-not-sufficient; the achieved D/F ratio is reported as an
  **empiric (observed)** per rank (R1b), never as a proof.
- **Null control (rider R1c), designed before the run.** The same statistic on a
  wrong-label pairing must FAIL: `scale_collapse(A_r, B_{r'})` with r' an **antiphase
  (opposite-parity) rank** (r0↔r1, r2↔r1; even = {0,2}, odd = {1}). Because even/odd
  murmurations are antiphase, a genuine collapse of A_r onto B_r must NOT collapse
  A_r onto B_{r'}: **REQUIRE null D/F ≥ 3** where the test has power (r0, r1). A
  collapse statistic that passes the scrambled pairing measures nothing; this is
  demonstrated-firing applied to a statistic. (Empirically robust — the antiphase
  null clears 3 at every prime count tested, unlike a within-curve reversal.)
- **Per-class verdicts (rider R2), no blended pass.** Each rank is judged with its
  OWN floor; |E_2| ≈ 1380 (vs |E_0| ≈ 8536) makes the r=2 floor several × looser, so
  its null may not clear 3. That panel is reported "consistent, low power" (real
  D < T but null not rejected) — stated, not averaged away. REQUIRE null-fail only
  for r0, r1.

Antiphase / parity-consistency invariant (`theorem_antiphase`). The murmuration sign
tracks rank **parity** (root number): ranks 0,1 are opposite parity, ranks 0,2 the
same. Pinned thresholds (before the run): Pearson correlation of the per-prime means
over the shared prime grid **corr(f₀,f₁) < −0.5** (antiphase) and **corr(f₀,f₂) > +0.5**
(in phase). Both are modest bars a genuine relationship clears and noise would not.

**Phase-2 record (achieved, reported as empirics — R1b).** At n_primes = 300:
family counts 4328/5194/8536/1380 (exact); collapse ratio D/F = 0.95 (r0), 1.07 (r1),
0.80 (r2), all < 3 → pass; antiphase-null D/F = 4.89 (r0), 4.68 (r1) → ≥ 3, rejected
(power), 2.82 (r2) → not rejected (**low power**, |E₂| small — R2, reported not
hidden); corr(f₀,f₁) = −0.84, corr(f₀,f₂) = +0.70. The a_p come from the M0 machinery
(computed good primes, §3-converted bad primes). These ratios use the independent-a_p
floor F (R1a) and are necessary-not-sufficient evidence, not proof.

## P5 — family-count certification plan (and it already reproduces)

Each family count #E_r[N₁,N₂] the pipeline uses must be **REQUIREd against an
independent count** from the manifest-pinned dataset (not retyped): the pipeline's
count of rank-r isogeny classes in [N₁,N₂] == an independent enumeration of the same.

**Already verified now (data check, not pipeline code):** counting isogeny-class
representatives (`allcurves` NUM==1, rank-invariant column R) over the pinned files
reproduces **all four** published counts exactly, under inclusive [N₁,N₂]:

| count | HLOP (Example 1) | from pinned ecdata |
|---|---|---|
| #E₀[7500,10000] | 4328 | **4328** ✓ |
| #E₁[7500,10000] | 5194 | **5194** ✓ |
| #E₀[5000,10000] | 8536 | **8536** ✓ |
| #E₂[5000,10000] | 1380 | **1380** ✓ |

This confirms the family definition (rank-r isogeny classes, inclusive range) and the
data slice. In Phase 2 the pipeline recomputes these and REQUIREs equality with both
the published values and the independent enumeration; a mismatch is a deliverable.

---

## Phase-2 boundary (gated)

No averaging pipeline, no `murmuration_curve` emitter, no viewer until this pinning
review is signed off. The a_p per (curve, prime) come from the M0 machinery
(computed good primes; §3-converted bad primes), refereed as in M0. Branch
`murmurations`, local commits only.
