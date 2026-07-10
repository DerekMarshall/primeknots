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

- **Derived extract — repo-reproducibility (R2).** ecdata is under the **Artistic
  License 2.0**, whose preamble states the Package "may be copied, modified,
  distributed, and/or redistributed"; a derived/modified version is redistributable
  with attribution. So the M1 pipeline reads a **committed derived extract** —
  exactly the rows it consumes (label, N, minimal model, rank, bad-prime A–L signs)
  — restoring reproducible-from-repo for CI (freshness/validate) at trivial size,
  instead of the SKIP-when-absent path. Provenance chain:
  - `data/cremona/m1_extract.txt` — git-tracked, sha256
    `6cce329632e095e723bb6a89fd3a4b3bfb50e3d264c05918499bb9f8df85bf97`, 30,366
    isogeny classes, covered range [2500, 10000]. Carries a full license header
    (source, release commit, generating command, Artistic-2.0 notice, derived
    marker) — the manifest satisfies us, the header satisfies the license (R2).
  - derived by `at ecdata-extract --lo 2500 --hi 10000` from the raw slices
    `allcurves.00000-09999`/`.10000-19999` + `aplist.*` at ecdata release
    `2026-04-22 / 25cec5ec` (the sha256s pinned in `data/cremona/MANIFEST.json`).
  - attribution: J. Cremona, *ecdata*, Artistic License 2.0. The extract is a
    "Modified Version" carrying this notice in its header.
  - faithfulness verified: `at emit --stage m1` from the extract is byte-identical
    (payload) to emit from the raw slices. The loader prefers the extract; raw
    slices are only needed to regenerate it. Being git-tracked, the extract's
    integrity is git's (any change shows in the diff) — no separate checksum test.
  - **size, flagged:** 1.4 MB — the largest committed blob, over the project's
    self-imposed 1 MB observation (publication-audit.md). Justified: a 4× reduction
    from the raw slices, license-clean, and the price of R2's repo-reproducibility;
    plain text is required (no zlib in `src/`). This is a documented, not silent,
    exception — the publication audit's blob table is to be updated when the
    `murmurations` branch merges.

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
- **Claim, correctly scoped (rider R1).** The verdict on real data is "**consistent
  with p/N scaling**" (D < T), *not* "proven to scale". Two controls, reported side
  by side:
  - **Reversal null (the scaling-power control, `scale_collapse_null`).** Reverse one
    curve's a_p means in y — a *non-scaling* scramble — and recompute D/F. This is the
    test's **power against a wrong scaling** at these family sizes. It is **limited and
    erratic**: it clears the 3F bar for one rank (r1, D/F ≈ 3.58) but not the others
    (r0 ≈ 2.86, r2 ≈ 1.44) — a within-curve reversal is a weak, shape-dependent
    scrambler, which is exactly why the parity null was added as the reliable control.
    It always exceeds the real D/F (the statistic orders real ≪ scramble, checked), but
    the 3F bar cannot reliably reject it. Honest statement: "consistent with p/N
    scaling; power to distinguish alternative scalings is limited at these family
    sizes — reversal control D/F = [numbers below]."
  - **Parity null (the non-degeneracy control).** `scale_collapse(A_r, B_{r'})` with
    r' an opposite-parity rank (r0↔r1, r2↔r1). This is **NOT a scaling null** — it
    differs from the real comparison in *two* variables (rank AND range). It tests
    that the statistic is not degenerate across parity classes: **REQUIRE D/F ≥ 3**
    where powered (r0, r1). It is robust (clears 3 at every prime count) precisely
    because even/odd murmurations are antiphase.
- **Per-class verdicts (rider R2), no blended pass.** Each rank judged with its OWN
  floor; |E_2| ≈ 1380 (vs |E_0| ≈ 8536) makes the r=2 floor several × looser. r=2 is
  reported "consistent, low power" (real D < T; controls not rejected) — stated, not
  averaged away. REQUIRE the parity-null rejection only for r0, r1.

Antiphase / parity-consistency invariant (`theorem_antiphase`). The murmuration sign
tracks rank **parity** (root number): ranks 0,1 are opposite parity, ranks 0,2 the
same. Pinned thresholds (before the run): Pearson correlation of the per-prime means
over the shared prime grid **corr(f₀,f₁) < −0.5** (antiphase) and **corr(f₀,f₂) > +0.5**
(in phase). Both are modest bars a genuine relationship clears and noise would not.

**Phase-2 record (achieved, reported as empirics — R1b).** At n_primes = 300:
family counts 4328/5194/8536/1380 (exact). Real collapse D/F = 0.95 (r0), 1.07 (r1),
0.80 (r2) — all < 3, consistent with p/N scaling. **Reversal null (scaling power)**
D/F = 2.86 (r0), 3.58 (r1), 1.44 (r2) — clears 3 only for r1, i.e. **limited and
erratic power** to reject a wrong scaling at these family sizes (the statistic orders
real ≪ reversal in every rank, but the 3F bar rejects the reversal only for r1 — a
within-curve reversal is a weak scrambler). **Parity null (non-degeneracy)** D/F =
4.89 (r0), 4.68 (r1) → rejected; 2.82 (r2) → not rejected (low power, |E₂| small — R2).
corr(f₀,f₁) = −0.84 (antiphase), corr(f₀,f₂) = +0.70 (in phase). a_p from the M0
machinery. All ratios use the independent-a_p floor F (R1a) — necessary-not-
sufficient evidence, not proof.

**Deviation postscript (rider R1a/R1d — PR-postscript pattern).**
- *Original design:* a single within-curve **reversal** null, REQUIREd to fail (D/F ≥ 3).
- *Observed behavior:* the reversal null did **not** robustly clear 3 — the r0
  experiment measured ≈ 2.4–2.9 across n_primes ∈ {300,500,700}, and per-rank it is
  erratic (at n_primes=300 it clears 3 for r1 = 3.58 but not r0 = 2.86 or r2 = 1.44).
  A within-curve reversal is a weak, shape-dependent scrambler at these amplitudes.
- *Replacement:* added the **parity** null (opposite-parity rank), which robustly clears
  3 (4.89/4.70/3.75), as the non-degeneracy control; the reversal null was **not deleted**
  but re-scoped to a reported *scaling-power* measurement.
- *Timing (stated plainly, from the record):* the swap happened **after** the real-data
  collapse results were seen — the smoke run showed the real collapse passing *and* the
  reversal null's behavior, which is what prompted adding the parity null. The git record
  cannot corroborate the order: the reversal null was intra-session and never committed
  (commit 0e4cdb4 already carried only the parity null). This is disclosed as a
  post-hoc control change; the real-data collapse verdict (D/F < 3) is independent of
  which null is used, and both nulls are now reported (additions + relabels only, no
  test deletions).

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
