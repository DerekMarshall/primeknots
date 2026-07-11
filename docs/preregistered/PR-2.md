# PR-2 — Is the height-murmuration tail deficit carried by analytic-rank-2 curves?

**Status: PRE-REGISTERED (committed per the PR-2 gate verdict, 2026-07-11, with the six
gate amendments folded in). Mode: M5 research (RESEARCH-M §7). Runs only AFTER PR-1's 2¹⁶
rung (sequencing below). The document's own commit hash is its timestamp; no confirmation
run may precede it, and no pre-declared element below may change without a logged
deviation postscript.**

---

## Hypothesis (from [SS25], cited — not from our exploration)

[SS25] name their own open question for the downward bias (p.4, verbatim):
> "A possible explanation for (1) is related to elliptic curves of rank ≥ 2. Curves of
> larger rank have been observed since work of Birch and Swinnerton-Dyer [4] to have
> smaller a_p values on average. Since, conjecturally, more curves have rank 2 than any
> other rank > 1, and, conjecturally, curves with rank 2 have root number +1 … this could
> push the average of a_p times the root number downwards."

M4 found the tail deficit as a displaced trough (u≈0.8875, dev 0.0825) + a downward bias
growing in u on the descending branch (the monotone interpolated zero-drift, C3). This is
a candidate for the SS mechanism.

> **H1:** removing the analytic-rank-2 subpopulation materially reduces the tail deficit
> (the trough recovers toward 0.805) — the deficit is carried by analytic-rank-2 curves.
> **H0:** it does not — the deficit is not explained by analytic-rank-2 over-representation
> at this X.

## Amendment 1 — the split variable is ANALYTIC rank (MW rank never asserted)

[SS25]'s "rank" is BSD-loose (they invoke it "conjecturally"; no formal analytic-vs-MW
definition, p.1/p.4). We **pin the classifier to the analytic rank** — the order of
vanishing of L(E,s) at s=1 — via **PARI `ellanalyticrank(E, prec)`** at a precision
recorded in the cache header. Provenance: **oracle, analytic rank** — a new numerically-
determined sub-class (RESEARCH-M §2 amendment 2026-07-11), distinct from the exact,
unconditional N and ε. **Mandatory parity cross-check:** analytic-rank parity must equal
the cached ε's parity for *every* curve (ε=(−1)^{ord} is the functional-equation sign, a
theorem — so a mismatch is a numerical mis-determination, not a conjecture); a nonzero
mismatch count **aborts** the run. **Every claim branch below says "analytic-rank-2"**;
equality with Mordell–Weil-rank-2 is BSD and is **never invoked**.

Subpopulations: S₀, S₁, S₂, S≥₃ by analytic rank; the leave-out set F∖S₂.

## Exact statistic per subpopulation (statistic UNCHANGED)

The M4 statistic (1), `murm::ss_empirical`, restricted to a subpopulation S ⊆ family:
D̂_S(u) = (1/(Δu·|S|)) Σ_{(E,p): E∈S, p/N∈b} (u_mid·ln N/N)·ε·a_p, **same extractor
(`extract_shape`), same Δu=0.025, same τ=0.06 — none touched.** a_p reused (not
recomputed) from PR-1's 2¹⁶ run; the split only *filters* which curves enter the average.

## Amendment 2 — power: primary (well-powered) vs secondary (f₂-limited); sequencing

- **PRIMARY — leave-out trough recovery (u-space, well-powered, runs at 2¹⁶).**
  Compute D̂_{F∖S₂} and its trough. |F∖S₂| ≈ (1−f₂)·|F| ≈ full family, so its sampling
  noise ≈ the M4 confirmation's — **well-powered**. Decision on this axis (below).
- **SECONDARY — subpopulation contrast (value-space, f₂-limited).** Compare the
  descending-branch (u∈[0.7,0.9]) mean of D̂_{S₂} vs D̂_{S₀}. |S₂|=f₂·|F| is small, so its
  tolerance is inflated by √(1/f₂). **Power is pre-computed from f₂** (below); if the
  predicted resolvable effect at 2¹⁶ is smaller than the tolerance, the **third
  (inconclusive) branch is pre-declared to fire** — reported, no claim.
- **Sequencing (the cache design).** PR-1's 2¹⁶ rung runs FIRST and **persists the
  per-curve bin partials** (`ss_empirical` already computes them per curve) keyed by
  (A,B). PR-2 then adds an **analytic-rank column** to that cache (a `at rank-cache`
  generator, `ellanalyticrank` per (A,B)) and **re-aggregates the partials over each
  subpopulation — NO a_p recomputation** (the expensive step is done once, in PR-1). This
  is the "cache designed for PR-2 to add the rank column" requirement.

## Amendment 3 — holdout: the annulus (10⁴, 2¹⁶], plus the virgin split dimension

**Geometric holdout = the height annulus (10⁴, 2¹⁶]** — curves with 10⁴ < H(E) ≤ 2¹⁶:
**|F(2¹⁶)| − |F(10⁴)| = 5042 − 1048 = 3994 curves**, none touched by any exploration
(L1–L3 all used H ≤ 10⁴). **And the split dimension itself is virgin:** the analytic rank
has *never been computed* on any curve in this project — so the rank-split is a fresh
holdout dimension *regardless of X*. The primary/secondary tests are evaluated on the
**full family H≤2¹⁶** as the main population and **cross-checked on the annulus alone**
(labeled secondary), so a result cannot be an artifact of reusing the ≤10⁴ curves that
seeded the hypothesis. *(The earlier modular-half reservation is dropped.)*

## Amendment 4 — thresholds per axis (cite m4-pinning §R0c "Two axes, kept separate")

- **u-space (primary, recovery):** H1 on this axis iff the leave-out trough
  |trough_u(F∖S₂) − 0.805| ≤ **τ = 0.06** *and* is at least one bin (Δu=0.025) closer to
  0.805 than the full-family trough (0.0825) — i.e. the deficit measurably recovers when
  S₂ is removed. Bin-quantization floor as in PR-1.
- **value-space (secondary, contrast):** the S₂-vs-S₀ descending-branch mean gap must
  exceed the R0 sampling tolerance **σ_R0(u)·√(1/f₂)** (the m4-pinning §R0c σ(u) model,
  |S| = f₂·|F|). The gap sign must be *downward* (S₂ more negative), per the SS mechanism.
- These are two axes (location vs value); they are **not** combined — a value-space
  contrast cannot substitute for the u-space recovery and vice versa (the §R0c discipline).

## Amendment 5 — M0b dependence recorded

For X ≥ 2¹⁸ (and to strengthen the f₂-limited secondary), the O(p) a_p is the blocker;
the **M0b fast-a_p stage** (RESEARCH-M §8, deferred) is the prerequisite. PR-2 runs at
2¹⁶ on PR-1's persisted a_p; it does not itself require M0b, but records M0b as the path
to a higher-power rerun.

## Looks the drafting generated: ZERO

This draft computes no statistic. The hypothesis is [SS25]'s (cited) applied to the M4
tail (L2, already logged); no rank has been computed. **No new LOOKS entry.** f₂ (the
analytic-rank-2 fraction under height ordering) is itself a look — measured as
confirmation step 1 and logged then, or read from [SS25]'s cited source [2] for
2¹⁶–2²⁸ (value **UNVERIFIED** until read). The power pre-computation (Amendment 2/4) is a
function of f₂ and is resolved when f₂ is fixed, before the contrast is evaluated.

## Analysis-code binding

The a_p statistic is `murm/ss_empirical.cpp`, sha256
`b87ebd1e188e15dc2d3a1c2a54e5300dc3c7ff3d4adef3e77d5328b9ff526cde` (unchanged from M4/PR-1;
reused, not recomputed). The NEW code — the `ellanalyticrank` classifier + the
subpopulation re-aggregation — will be hash-bound in this doc's postscript when built,
before the confirmation runs. A `prereg_rank_split` test loads THIS doc, verifies both
hashes + the parity cross-check (zero mismatches) + f₂, applies the per-axis thresholds,
and records the branch.

## Claim licensed per branch (analytic-rank-2; including null and underpowered)

- **H1 (deficit carried by analytic-rank-2):** leave-out trough recovers within τ (and,
  if resolvable, the S₂ contrast is downward beyond tolerance) → *"empirical evidence, on a
  from-scratch height-ordered family we control end to end, that the murmuration tail
  deficit at H≤2¹⁶ is carried by the **analytic-rank-2** subpopulation — a citable data
  point on the open question [SS25] pose (p.4)."* Pre-registered finding; not a proof; MW
  rank not asserted.
- **H0 (null):** removing S₂ leaves the trough displaced (recovery < one bin) →
  *"the tail deficit at H≤2¹⁶ is **not** explained by analytic-rank-2 over-representation"*
  — a citable null constraining the proposed mechanism.
- **Inconclusive (pre-declared, f₂-limited):** the primary is well-powered so it resolves;
  but if the primary is ambiguous AND the secondary contrast is below the √(1/f₂)-inflated
  tolerance at 2¹⁶ → *"underpowered at feasible X; resolvable only at larger X via the M0b
  fast-a_p stage"* — no claim.

## Postscript (analysis-code hash, f₂, results — appended only after the run)

*(empty — no rank has been computed; PR-1's 2¹⁶ rung has not run as of this commit)*
