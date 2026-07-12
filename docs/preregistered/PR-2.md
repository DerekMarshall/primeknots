# PR-2 — Is the height-murmuration tail deficit carried by analytic-rank-2 curves?

**Status: PRE-REGISTERED (committed per the PR-2 gate verdict, 2026-07-11, with the six
gate amendments folded in). Mode: M5 research (RESEARCH-M §7). Runs only AFTER PR-1's 2¹⁶
rung (sequencing below). The document's own commit hash is its timestamp; no confirmation
run may precede it, and no pre-declared element below may change without a logged
deviation postscript.**

---

## Step 0 preamble amendments (2026-07-12, before f2 — non-gated elements, logged)

Per the orchestrator's PR-2 STEP 0, three preamble amendments were made BEFORE any f2 /
split curve was computed (legal for non-gated elements; the gated elements — split
variable, statistic, extractor, τ, holdout, per-axis thresholds, decision rule — are
untouched): **0a** the SS25 rank passage pinned VERBATIM (below) with the hypothesis
reconciled against it; **0b** a two-paper literature pass (§"Literature pass"); **0c** the
`docs/papers/README` M-ladder + 2026-preprint hygiene rider. This log is the amendment
record required by the doc's own change-discipline.

## Hypothesis (from [SS25], cited — not from our exploration)

**0a — VERBATIM PIN.** [SS25] p.3–4, the downward-bias paragraph and the rank paragraph
that follows it, word-for-word from `docs/papers/sawin-sutherland-2504.12295.pdf` (the "…"
elision in every prior version of this doc — and in the referee's gloss — is now removed;
the two caveats it hid are load-bearing):

> "An interesting feature is that part of the discrepancy between the purple and green dots
> comes from a persistent downward bias, where (1) is slightly less than its predicted
> limiting value. As part of the proof of Theorem 2, we prove a variant statement without
> the limit as P goes to ∞. Graphs of the left-hand side and right-hand side of this
> statement do not show the downward bias, suggesting it is genuinely a property of the
> primes. This downward bias seems to decay as X goes to ∞, suggesting it will go away in
> the limit, though it may decay more slowly than other sources of discrepancy between (1)
> and the right-hand side of (2).
>
> A possible explanation for (1) is related to elliptic curves of rank ≥ 2. Curves of
> larger rank have been observed since work of Birch and Swinnerton-Dyer [4] to have
> smaller a_p values on average. Since, conjecturally, more curves have rank 2 than any
> other rank > 1, and, conjecturally, curves with rank 2 have root number +1, with both
> conjectures empirically checked in a range that includes our dataset, this could push the
> average of a_p times the root number downwards. (On the other hand, it is not obvious
> that this effect is not already accounted for by Theorem 2.) However, while the
> proportion of curves with naive height ≤ X that have rank 2 is expected to decrease to 0
> as X → ∞, it actually increases for the range of X we consider (from 2¹⁶ to 2²⁸), as can
> be seen in [2]. Hence, to use curves of rank ≥ 2 to explain the downward bias, one would
> have to explain why the bias decreases over this range, rather than increasing as one
> might expect."

**Reconciliation of our hypothesis against the verbatim** (the prior "possibly rank-2
related" phrasings — ours and the referee's — were gloss; the actual statement is a hedged
PROPOSED explanation carrying two explicit open problems):

1. **SS25 write "rank ≥ 2" / "rank 2" UNQUALIFIED** (analytic vs Mordell–Weil), invoking
   both sub-claims "conjecturally." Amendment 1's pinning to the **analytic rank** is our
   deliberate, unconditional-classifier operationalization of their informal "rank"; we
   never assert MW rank (that equality is BSD). This is consistent with — not a
   contradiction of — the source, which simply does not distinguish.
2. **Caveat (a), verbatim: "it is not obvious that this effect is not already accounted for
   by Theorem 2."** So even a positive PR-2 result (deficit carried by analytic-rank-2)
   does **not** establish rank as an *independent* cause — it may be folded into the proven
   density variant. Our claim wording is therefore "**carried by**" (a descriptive
   statement about which curves the deficit lives on), never "**caused by**."
3. **Caveat (b), the increase/decrease puzzle, verbatim:** the rank-2 *proportion*
   **increases** over 2¹⁶–2²⁸ (per [SS25] ref. [2]) while the bias is expected to *decay* —
   so a rank-2 explanation must explain why the bias decreases despite the fraction rising.
   **Our Rung-1 finding sharpens exactly here:** at X=2¹⁶ (the BOTTOM of SS25's range) the
   trough deficit is **flat/persistent** (does not decay) — the direction consistent with a
   rising rank-2 proportion. PR-2 asks, at this one anchor point, whether the
   analytic-rank-2 subpopulation is where that persistent deficit lives.

M4 found the tail deficit as a displaced trough (u≈0.8875, dev 0.0825) + a downward bias
growing in u on the descending branch (the C3 zero-drift); PR-1 Rung 1 found it PERSISTENT
at 2¹⁶. This is a candidate for the SS mechanism, tested under the two caveats above.

> **H1:** removing the analytic-rank-2 subpopulation materially reduces the tail deficit
> (the trough recovers toward 0.805) — the deficit is **carried by** analytic-rank-2 curves
> (a descriptive claim about the carrier subpopulation; NOT a causal/independent-effect
> claim, per caveat (a) and the §"Literature pass" confound).
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

## Literature pass (step 0b — two-fetch minimum, 2026-07-12)

Both papers below are **March 2026 preprints, past this project's assistant knowledge
cutoff (Jan 2026)** — their content was **fetched from arXiv and verified**, not recalled
(rule 6). Same author (Dane Wachs). Deliverable: what each changes for PR-2's hypothesis,
thresholds, or novelty — *including "nothing," where that is the finding*.

**[Wachs-BSD] D. Wachs, "BSD Invariants and Murmurations of Elliptic Curves,"
arXiv:2603.04604 (4 Mar 2026).** Cremona dataset (3,064,705 curves, conductor ≤ 499,998),
**conductor-ordered**. Findings: (i) BSD invariants (real period, Tamagawa product,
analytic |Ш|, regulator, torsion) do **not** themselves murmur in sliding conductor
windows; (ii) **within a fixed rank**, curves stratified by Tamagawa product / analytic
|Ш| / real period show **significantly different murmuration profiles** (p < 0.001 vs
permutation null, scale-invariant across conductor ranges); (iii) the |Ш| modulation
**survives controlling for L(1), real period, and conductor** — a **pure mean shift** in
the Frobenius-trace distribution (variance/skew/kurtosis unchanged), **concentrating at
small primes**; (iv) curves with |Ш| ≥ 4 have displaced low-lying L-zeros, tied to the
modulation via the explicit formula.
- **Hypothesis / CONFOUND (material):** the analytic-rank-2 subpopulation differs from
  rank-0 not only in rank but in its Tamagawa/|Ш|/period distribution, and [Wachs-BSD]
  shows those independently modulate the murmuration *within fixed rank*. So a positive
  PR-2 (deficit carried by analytic-rank-2) is **consistent with the carrier being a
  rank-correlated |Ш|/Tamagawa effect that PR-2 neither computes nor controls.** This is a
  named, published instance of caveat (a) (§Hypothesis) and it **hardens our wording to
  "carried by," never "caused by"**; PR-2 does not disentangle rank from its correlated BSD
  invariants.
- **Thresholds:** **no change.** (Note only: [Wachs-BSD]'s mean shift concentrates at
  *small* primes ≈ small u, whereas our value-space contrast sits on the descending branch
  u∈[0.7,0.9] ≈ *large* p/N — different u-region, so not obviously the same effect;
  recorded, not a threshold change.)
- **Novelty:** a rank **leave-out on the HEIGHT-ordered SS25 family** remains novel relative
  to [Wachs-BSD] (which is conductor-ordered and stratifies *within* fixed rank). But the
  novelty claim is now **narrower and cites [Wachs-BSD]**: PR-2 establishes *where the
  deficit lives* (a subpopulation statement), not rank causation, precisely because
  finer-than-rank BSD structure is documented.

**[Wachs-FF] D. Wachs, "Murmurations of Elliptic Curves over Function Fields,"
arXiv:2603.13802 (14 Mar 2026).** Function-field F_q(t) family (E_D: y²=x³+x+D(t), D monic
squarefree deg 5; 534,745 curves over q=7,11,13), rank-0 vs rank-1. |Ш| modulation is a
**composition effect** (|Ш| = L(1/q) exactly there; different |Ш| strata are different
mixtures of cyclotomic L-polynomial types → different mean traces), with an exact
reweighting identity.
- **Changes for PR-2: nothing direct.** Different base field, and rank-0/1 (not the ℚ
  height-ordered rank-≥2 deficit we test). Recorded as **context**: in a setting where the
  mechanism is exactly computable it confirms |Ш| modulation of murmurations is real and can
  be a pure mixture/composition effect — thematically reinforcing the [Wachs-BSD] confound,
  but bearing on neither our thresholds nor our specific hypothesis.

**Net:** the pass **narrows the novelty statement** (cite [Wachs-BSD]; no rank-causation
claim) and **adds one named confound** (within-rank BSD modulation). It changes **no gated
element** (split variable, statistic, extractor, τ, holdout, per-axis thresholds, decision
rule all stand). The claim branches below are updated to cite it.

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
  deficit at H≤2¹⁶ is **carried by** the **analytic-rank-2** subpopulation — a citable data
  point on the open question [SS25] pose (p.4)."* Pre-registered finding; not a proof; MW
  rank not asserted. **Novelty/limitation (step 0b):** novel as a rank leave-out on the
  HEIGHT-ordered family (cf. [Wachs-BSD], conductor-ordered / within-fixed-rank). It is a
  **carrier** claim, **not causal**: per SS25 caveat (a) and [Wachs-BSD]'s within-rank
  Tamagawa/|Ш| modulation, the analytic-rank-2 stratum differs in correlated BSD invariants
  that PR-2 does not control, so this does **not** attribute the deficit to rank itself.
- **H0 (null):** removing S₂ leaves the trough displaced (recovery < one bin) →
  *"the tail deficit at H≤2¹⁶ is **not** explained by analytic-rank-2 over-representation"*
  — a citable null constraining the proposed mechanism.
- **Inconclusive (pre-declared, f₂-limited):** the primary is well-powered so it resolves;
  but if the primary is ambiguous AND the secondary contrast is below the √(1/f₂)-inflated
  tolerance at 2¹⁶ → *"underpowered at feasible X; resolvable only at larger X via the M0b
  fast-a_p stage"* — no claim.

## Postscript (analysis-code hash, f₂, results — appended after each step)

### Step 1 — f₂ measured (2026-07-12)

**Analysis-code binding (classifier).** The analytic-rank classifier is the `at rank-cache`
subcommand (`app/at.cpp`) → PARI `ellanalyticrank(ellinit([0,0,0,A,B]))` at **prec 38**,
**PARI/GP [2, 17, 4]**; the committed-cache reader/writer is `src/murm/rank_cache.cpp`,
**sha256 = `24be7320c215b12f5437d0a8bf732cc867b9a81dbdd14abaf79b6dea49c71c77`**. The rank cache
`data/m5/rank_cache_x65536.txt` carries **generator_hash 787e95…** = sha256(height_family.cpp)
— the SAME (A,B) key generator as the N/ε cache, so the rank column is bound to the identical
5042-curve family (no a_p was recomputed; PR-1's persisted partials are untouched). The
**subpopulation re-aggregation code (step 3) will be hash-bound here when built.**

**Parity cross-check — PASSED, 0 mismatches / 5042.** Analytic-rank parity equals the cached
ε parity for every curve (ε=(−1)^r is the functional-equation sign, a theorem). The run did
**not** abort. Internal consistency: even ranks r0+r2 = 1754+738 = **2492** = the ε=+1 count;
odd r1+r3 = 2525+25 = **2550** = the ε=−1 count.

**f₂ = 738 / 5042 = 0.14637** (the pre-declared look, LOOKS L4). Analytic-rank distribution
over the full H≤2¹⁶ family: **r0 = 1754, r1 = 2525, r2 = 738, r3 = 25** (Σ = 5042). Sanity vs
[SS25]: rank 2 dominates every rank > 1 (738 ≫ 25), matching their premise ("more curves have
rank 2 than any other rank > 1"). Committed: `data/m5/rank_cache_x65536.txt`.

*Next: step 2 — resolve the value-space contrast threshold as the committed function of f₂
(σ_R0(u)·√(1/f₂)), IN WRITING, before any split curve is computed.*
