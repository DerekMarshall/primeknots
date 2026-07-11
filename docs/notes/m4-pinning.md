# M4 pinning log — Sawin–Sutherland height ordering [replication, the last rung]

Two-phase gate for Stage M4 (RESEARCH-M). **Phase 1 (this document): PINNING ONLY**,
quoted verbatim from the fetched PDF `docs/papers/sawin-sutherland-2504.12295.pdf`
= arXiv:2504.12295 v2, *Murmurations for elliptic curves ordered by height*, Will
Sawin & Andrew V. Sutherland. Equations **read from the rendered PDF pages** (the
extraction-garble discipline — no `pdftotext`). No code in Phase 1. **STOP at the
pinning review; the P4 root-number-provenance decision is presented, NOT resolved.**

The M3 pattern carries over: hard anchor (this is REPLICATION, failure mode =
convention-fitting), separate-TU density, committed tolerances before the run. The
one genuinely new hazard is **P4** (root numbers/conductors for a family outside
Cremona) — flagged as the crux for the gate.

---

## C2 — CLAIM CLASS: what SS25 PROVES vs what it CONJECTURES  [blocks Phase 2]

**Verbatim (abstract, p.1):** "We consider a variant problem where the elliptic curves
are ordered by naive height, and the pth coefficients are averaged over p/N in a fixed
interval. **We give a conjecture for the murmuration density in this case, as an
explicit but complicated sum of Bessel functions.** This conjecture is motivated by a
theorem about a variant problem where we sum the nth coefficients for n with no small
prime factors against a smooth weight function. We test this conjecture for elliptic
curves of naive height up to 2²⁸ and find good agreement with the data. **The theorem
is proved using the Voronoi summation formula** … This is the first work to give an
explicit formula for the murmuration density of a family of elliptic curves, in any
ordering."

So the register is unambiguous in the source:
- **CONJECTURED — Conjecture 1 (eq. (2)):** the explicit density for the *actual*
  height-ordered elliptic-curve murmuration, statistic (1) (a prime sum). Labeled
  **"Conjecture 1"** in the paper. This is what M4's empirical curve is compared to.
- **PROVEN — Theorem 2 (p.2):** the *variant* — the sum over n with **no prime factors
  ≤ P**, weighted by a smooth W(n/N(E)), in the P→∞ then X→∞ limit, equals
  ∫ W(u)·(the same Bessel-sum integrand) du. Labeled **"Theorem 2"**, proved via Voronoi
  summation. This is NOT the prime-sum murmuration; it is the smooth-weight,
  no-small-factor variant that *motivates* Conjecture 1.

**⇒ The explicit density of the height-ordered EC murmuration is CONJECTURAL, not
proven.** RESEARCH-M §6 overclaimed it ("[SS25] proves an explicit murmuration density
… ordered by naive height", "a proven anchor") — filed as **ERRATA #24** (spec-author
overclaim) and corrected in RESEARCH-M; §6 now reads in the honest register.

**M4 acceptance language (honest register, replacing "verify against their density"):**
M4 demonstrates **empirical agreement between the height-ordered statistic (1) and the
CONJECTURED density (Conjecture 1)** — an empirical replication of a conjecture, not a
proof-check. Optionally (Phase 2, if computable at our scale) an **empirical check of
Theorem 2** — the proven variant — as convergence of the smooth-weight/no-small-factor
average to ∫W·D; that is the only part with a proof behind it, and even there our check
is empirical (a limit statement). Nothing in M4 is labeled *proved* (RESEARCH-M §11:
"No proofs are claimed anywhere in this project"; only cited literature is).

**Emitted-artifact requirement (Phase 2):** `params` MUST carry a `claim_class` field
naming this — e.g. `"empirical agreement with the CONJECTURED density (Z-... Conj 1);
Theorem 2 is the proven variant (not the prime-sum murmuration)"` — so the public JSON
cannot be read as a proof-verification.

---

## P1 — the height and the family

**Family (quoted, p.1):** "For integers A, B, let E_{A,B} be the curve with equation
**y² = x³ + Ax + B**. Every elliptic curve over ℚ can be expressed uniquely as E_{A,B}
for integers A, B such that **no prime p simultaneously satisfies p⁴|A and p⁶|B**."
This uniqueness clause is the minimality / isomorphism-class dedup convention: each
iso class ↔ the unique reduced (A,B) with no p having (p⁴|A ∧ p⁶|B).

**Naive height (quoted, p.1) — pinned verbatim, NOT assumed:**

    H(E) := H(E_{A,B}) := max(4|A|³, 27B²),

"where A and B are uniquely determined by E_{A,B} ≃ E and p⁴∤A or p⁶∤B for all primes
p." (So the height is the max of the two monomials 4|A|³ and 27·B²; note |A| cubed,
B squared, the numeric constants 4 and 27 — the same constants as in the discriminant
Δ(E_{A,B}) = −16(4A³+27B²), but H is the max of the two positive terms, not |Δ|.)

**Δ≠0 / singular handling:** E_{A,B} is an elliptic curve iff nonsingular iff
**4A³ + 27B² ≠ 0**; pairs with 4A³+27B²=0 are excluded from the family.

**Averaging notation (quoted, p.1):** "For S a set, 𝔼_{E∈S}[f(E)] := (1/|S|) Σ_{E∈S} f(E)."

## P2 — the statistic and the variable  [the convention trap]

**Statistic (quoted, eq. (1), p.2):**

    𝔼_{E: H(E)≤X} [ (log(N(E)^{(C₁+C₂)/2}) / N(E)) · Σ_{p∈(C₁N(E), C₂N(E)), p prime} ε(E)·a_p(E) ]

where N(E) is the conductor and ε(E) ∈ {−1,+1} the root number (from the functional
equation, p.1). a_p(E) is the p-th L-function coefficient (= a_p in the M0 sense for
good p).

**THE VARIABLE (the pinned trap):** the family is **ordered by height** (H(E) ≤ X),
but the scaling variable is **u = p / N(E)** — the ratio of the prime to each curve's
**conductor**, NOT p/X. The inner sum runs over primes p ∈ (C₁·N(E), C₂·N(E)), i.e.
**p/N(E) ∈ (C₁, C₂)**. This is Bober's suggestion (p.2, personal communication): "to
sum over p/N … when elliptic curves are not necessarily ordered by conductor." The
height only selects *which curves* enter the average; the correlation variable is
p/N. **This differs from M3's y = P/X** (Zubrilina) — do not conflate. Confirmed by
Figure 1 (p.3): it plots (1) with X=2²⁸ over the 2000 intervals (j/2000, (j+1)/2000]
⊆ (0,1], j=0…1999 — the x-axis is **u = p/N ∈ (0,1]**.

**Normalization:** the prefactor log(N^{(C₁+C₂)/2})/N ≈ divides out the PNT count of
primes in (C₁N, C₂N) (≈ (C₂−C₁)N / log(N^{(C₁+C₂)/2})); the constant (C₂−C₁) is
divided out before averaging over E (p.2). So the per-curve inner quantity is
essentially the ε-weighted average of a_p over primes with p/N ∈ (C₁,C₂).

## P3 — the density, its evaluation, and its (absent √-cusp) teeth

**Conjecture 1 (quoted, eq. (2), p.2), for 0 < C₁ < C₂:** the X→∞ limit of (1) equals

    ∫_{C₁}^{C₂} 2π√u  Σ_{q∈ℕ, q square-free}  Σ_{m∈ℕ}
        [ μ(gcd(m,q)) / ( q·m·φ(q/gcd(m,q)) ) ]  ·  J₁(4π√u·m/q)
        ·  ∏_{p|m, p∤q} ℓ̂_{p, 2v_p(m)}  ·  ∏_{p|q} ℓ_{p, v_p(m)}   du,

where J₁ is the **Bessel function of the first kind**, v_p(m) the p-adic valuation,
μ Möbius, φ Euler. "The integrand in the right-hand side of (2) is the murmuration
density in the sense of [26]." So the **density is D(u) = the integrand** (a function
of u = p/N); Lemma 15 asserts the double sum converges absolutely uniformly on
compact intervals ⇒ D continuous.

**Local factors (quoted, Lemmas 3 & 4, pp.4–5).** ℓ_{p,ν} (Def 5) and ℓ̂_{p,ν} (Def 6)
are:
- ℓ_{p,ν}, p>3:  −((p⁻¹−p⁻²)/(1−p⁻¹⁰)) · Σ_{f∈S₀^{ν+2}(SL₂ℤ) eigenform, a₁(f)=1} a_p(f).
- ℓ̂_{p,0}, p>3:  (1−p⁻¹)/(1−p⁻¹⁰);  ℓ̂_{p,2}, p>3:  −(p−p⁻¹+p⁻²−p⁻⁸)/((1−p⁻¹⁰)(p−1));
  ℓ̂_{p,ν} (ν>2, p>3):  −((p⁻¹−p⁻²)/(1−p⁻¹⁰))·(p+1+Σ_{f} a_p(f)).
- Explicit p=3 and p=2 variants are given in both lemmas (with the Chebyshev U_ν
  terms at p=3). S₀^{k}(SL₂ℤ) = weight-k **level-1** cusp forms; a_p(f) their Hecke
  eigenvalues. **Key simplification:** dim S₀^{ν+2}(SL₂ℤ) = 0 for ν+2 < 12 (i.e.
  ν < 10), so the eigenform sums are **empty for all small ν** and the ℓ/ℓ̂ reduce to
  explicit rational functions of p there — the dominant range. (Level-1 eigenvalues,
  when needed for ν ≥ 10, are computable from scratch / reuse the M3 trace machinery.)

**Evaluation plan (formula side, separate TU):** evaluate D(u) as the truncated double
sum q,m ≤ B (Figure 1 uses **B = 10⁵**), with J₁ from a from-scratch series/asymptotic,
the ℓ/ℓ̂ from the Lemma 3/4 rationals (+ level-1 eigenvalues only for ν≥10), and a
truncation/convergence check (Lemma 15). Authored independently of the empirical side
(Stage-4 separate-TU protocol).

**Teeth — there are NONE of the M3 √-cusp kind.** D(u) is a sum of **Bessel J₁**
terms (analytic), not a finite √(4y−r²) sum, so it has **no derivative-discontinuity
teeth at n²/4**. Per the instruction: the density has no kink structure, so the
calibrated test is **shape invariants**, committed pre-run. Candidate invariants from
Figure 1 (u ∈ (0,1], X=2²⁸): D(0)=0 (the √u factor), a single positive hump (peak near
u≈0.4, value ≈ +4 in the area-normalized plot), a zero crossing near u≈0.6–0.7, and a
negative trough near u≈0.85 (≈ −4); "positive root number bias for small P" is NOT
claimed here as at the M3 origin — the shape is one hump then one trough on (0,1]. The
exact committed invariants (peak/zero/trough locations + tolerances, and/or an L²
band) are finalized from bounded exploration and **committed before the confirmation
run** (commit-before-run, RESEARCH-M §7), as M2/M3 did. **[EMITTER HAZARD, flagged
now]** D(u) uses the transcendental **J₁** → the emitted formula curve carries the
cross-libm freshness risk of [[cross-compiler-emit-determinism]] (worse than M2's
sin/cos); the M4 emitter must be designed for byte-portability up front (precision/
clamp, or accept + let CI referee).

**Theorem 2 (quoted, p.2)** is the proven variant: replaces the prime sum by a sum
over n with no prime factors ≤ P (inverse density ∏_{p≤P}(1−1/p)⁻¹) against a smooth
weight W(n/N(E)); same integrand with the ∏ over p|m,p∤q and p|q and the P→∞ limit.
This is the theorem that motivates Conjecture 1 (the Cramér-model + smooth-weight
heuristics, p.3); the M4 replication targets **Conjecture 1 / eq (1)** (the actual
murmuration), with Theorem 2 as the rigorous backdrop.

## P4 — ROOT-NUMBER (and conductor) PROVENANCE  [the M4 crux — decide at the gate]

The statistic (1) needs, **per curve E_{A,B} in a height-ordered family**: (i) a_p(E)
— already from-scratch via M0 (charsum/enumerate from the model), **no oracle**; (ii)
the **conductor N(E)** — for the prime interval (C₁N, C₂N) and the normalization; and
(iii) the **root number ε(E)** — for the ε(E)·a_p(E) weighting. A height family
(SS test to H ≤ 2²⁸) is enumerated directly from (A,B) and is **not inside Cremona's
conductor range**, so N(E) and ε(E) **cannot come from the pinned M1 extract**.

**What SS compute (quoted/paraphrased):** they test (1) against a dataset of curves of
naive height up to 2²⁸ (Sutherland's computation; ranges 2¹⁶–2²⁸ per [2]), which
requires N(E) and ε(E) for every such curve — obtained by the standard algorithms
(Tate's algorithm for the conductor; local root-number formulas for ε), not from a
table (the family is far too large / outside tables).

**The decision (presented, not resolved):**
- **(a) PARI `ellrootno` (ε) + `ellglobalred`/`ellinit` (N) as ORACLE-PROVENANCE
  INPUT DATA.** Legitimate but a **first**: the oracle stops being only a *referee of*
  our numbers and becomes a *supplier of* input data feeding the computation. The
  provenance rule (CLAUDE.md §2 / RESEARCH-M) needs a **written amendment naming this
  class** and its **residual risk**: N,ε are trusted, not independently verified by us,
  so a systematic oracle error propagates silently into the statistic. *Precedent:* M1
  already took rank + conductor + Atkin–Lehner signs from ecdata (oracle-provenance
  input); (a) is **continuous with M1**, extended to (1) the **root number** (a new
  quantity) and (2) **live PARI** rather than a sha256-pinned dataset. *Mitigants:*
  a_p stays from-scratch; **overlap spot-check** — the low-height ∩ Cremona-range curves
  have N,ε in the pinned extract, REQUIRE agreement there; optionally a **from-scratch
  Tate conductor twin** for partial independence on N.
- **(b) Restrict the family to avoid oracle data.** *Not viable* — ε and N are
  intrinsic to the statistic (the ε-weighting and the p/N variable); there is no
  subfamily of SS's family where they are free without introducing a silent bias.
  Rejected (recorded so the rejection is explicit, not silent).
- **(c) From-scratch ε (and N via Tate) = the deferred Tate mountain.** Local root
  numbers at additive primes (Kobayashi–Halberstadt / Rohrlich tables) + Tate's
  algorithm. Out of M4 scope unless the paper forces it; large surface area, its own
  pinning + twins.

**RESOLVED (C3, Derek's ruling 2026-07-10): option (a) with riders.**
- **§2 amendment** naming the *oracle-provenance input-data* class + its residual risk
  (a live oracle supplying trusted input, not refereeing) — written into RESEARCH-M §2.
- **Dual-oracle overlap twin (Phase 2, REQUIREd):** over every curve in BOTH the height
  family AND Cremona's range, PARI's N and ε must equal the pinned ecdata **exactly**,
  with a **certified overlap count**. This is the independent check that the live oracle
  is not lying; a `twin_`/`oracle_`-class test.
- **Provenance + conditionality in emitted `params`:** N, ε columns labeled
  `provenance: oracle`; params states the input-data conditionality (alongside the
  §C2 `claim_class`).
- **Tate twin = deferred upgrade:** from-scratch conductor (Tate) + local root numbers
  recorded as an M0b-class future stage; not built in M4.
The actual murmuration quantity a_p stays computed-provenance.

## P5 — certified counts

The family size at each height cutoff X: the number of reduced (A,B) with H(E)≤X and
4A³+27B²≠0. H = max(4|A|³,27B²) ≤ X ⟺ |A| ≤ ⌊(X/4)^{1/3}⌋ **and** |B| ≤ ⌊(X/27)^{1/2}⌋;
reduced ⟺ no prime p with (p⁴|A ∧ p⁶|B). **REQUIREd against an independent
enumeration:**
- (i) **direct**: nested loop over (A,B) in the box, apply the reduce filter and the
  Δ≠0 filter, count;
- (ii) **inclusion–exclusion**: |box| − (non-reduced, via Möbius over primes p with
  p⁴|A ∧ p⁶|B) − (singular 4A³+27B²=0). REQUIRE (i) == (ii).
- **cross-check** (if available): SS cite [2] for the proportion of naive-height-≤X
  curves of rank 2 over 2¹⁶–2²⁸; any published height-count at a specific X is a third
  witness. (Marked UNVERIFIED until the exact [2] value is read.)

---

## Phase 2 — PRE-REGISTERED (built only after the pinning review signs off)

Still local until CI-green, then **push on CI-green** (M-branch authorization; CI now
runs on branch pushes, C1); no merge to main. No M5 stubs. Design fixed here, numeric
tolerances committed before the confirmation run. **Prerequisites already cleared before
Phase 2:** C1 (CI branch trigger), C2 (claim class + RESEARCH-M fix + ERRATA #24), C3
(§2 amendment + P4 resolution), **C4 (from-scratch Bessel J₁, twinned vs PARI `besselj`,
so no emitted byte depends on libm)**.
1. **a_p from-scratch** (M0) over the height family; **N, ε oracle-provenance** (PARI),
   with the **dual-oracle overlap twin** (C3): PARI vs pinned ecdata over the full
   height∩Cremona overlap, N and ε exact, certified count — REQUIREd before trusting
   the oracle inputs.
2. **Family counts** certified (P5, two independent enumerations agree) before averaging.
3. **Empirical statistic (1)** assembled: ε-weighted a_p averaged over p/N ∈ (C₁,C₂),
   over height-ordered curves; **separate-TU** from the D(u) formula side (P3), which
   consumes the C4 in-house J₁.
4. **Shape-invariant test** (P3) — committed peak/zero/trough locations + tolerances
   (no √-cusp teeth exist); the reported empiric is the L² / pointwise distance to D,
   decreasing with X. The persistent **downward bias** SS note (p.3–4, (1) sits below
   the RHS, decaying in X, possibly rank-2 related) is reported, not fitted away.
   Optionally the **Theorem 2** empirical convergence (the proven variant, §C2).
5. **Emitter + viewer + freshness**, byte-portable (in-house J₁, no libm bytes);
   `params.claim_class` (§C2) + `provenance:oracle` on N,ε (§C3); schema; validate-json.
   Snapshot committed with the emitter; **acceptance register = "empirical agreement
   with the CONJECTURED density"**, never "verified/proved".

### R1 — the error budget (stated, not implied)

The calibrated test carries its instrument error on the label — in the log and the
emitted `params` — as the chain **instrument ≪ evaluation ≪ decision**:

    J₁ accuracy 6.4e-12  (measured, PARI-besselj-twinned, incl. post-reduction args
                          to 1.26e5; core/bessel + verify/m4)
      ≪ density-evaluation tolerance  (M₂/D(u) truncation of the q,m double sum;
                          measured & COMMITTED with the density TU, item 3)
      ≪ shape-invariant tolerance     (the pass/fail band on hump/zero/trough
                          locations; COMMITTED pre-run with item 4).

Only the first number is fixed today (J₁ is built, C4). The other two are finalized
from bounded exploration and committed **before** the confirmation run they govern
(commit-before-run) — as M2/M3 did.

### Phase-2 progress (this session)

- **R4 DONE — certified family counts** (`murm/height_family`, verify/m4): direct ==
  sieve at 7 cutoffs to 10⁶; anchors 14/166/1048/7130 at X=10²…10⁵ certified by a
  third independent gp enumeration. This is the required-first foundation.
- **FORMULA TU DONE (step 1)** — `murm/ss_density` builds D(u) from the Conjecture-1
  integrand: in-house J₁ + generated constants + Lemma 3/4 local factors (rational +
  Chebyshev U_ν; the level-1 eigenvalue sums vanish for weight <12 and are truncated
  above — bounded, folded into the density-eval tolerance). **It reproduces [SS25]
  Fig. 1**: the numerically-extracted hump/zero/trough and amplitudes fall in the
  figure's band (hump ≈0.4–0.5/≈+4, zero ≈0.65, trough ≈0.8–0.9/≈−4). The eyeballed
  figure values are the **sanity band only, never the test** (transcribed-from-a-picture
  is the constants failure mode, CLAUDE.md rule 8).
- **COMMITTED R2 invariant targets (extracted numerically from D(u), NOT eyeballed;
  verify/m4 `theorem_ss_density_shape`):**
      hump  u = 0.475  (D ≈ +3.96)
      zero  u = 0.645  (first +→− crossing after the hump)
      trough u = 0.805 (D ≈ −3.40)
  Stable across truncation B∈[200,5000] to < the 0.005 scan step. Formula-side test
  tolerance 0.02; the EMPIRICAL-side tolerance (larger, from finite-X noise) is
  committed with step 3/4 before that run.
- **R1 error budget (numbers now fixed for the first two links):** J₁ accuracy
  **6.4e-12** (PARI-twinned) ≪ density-evaluation tolerance **0.056** (max|D₅₀₀−D₅₀₀₀|
  pointwise; B-truncation + eigenvalue-truncation) ≪ shape-invariant tolerance (0.02
  formula-side; empirical-side committed with step 3). `claim_class` = "empirical
  agreement with Conjecture 1 [SS25]" in the emitted `params` AND the viewer caption.
- **R3 DONE (step 2)** — `verify/m4/oracle_dual_overlap_NE`: over the Cremona-range
  overlap [2500,4000] (**5676 classes**), PARI ellglobalred conductor == ecdata N
  (5676/5676, exact) and PARI ellrootno == (−1)^{ecdata rank} (5676/5676, parity). The
  oracle-consumption gate is cleared before the statistic trusts N/ε.
- **STEP-3 EXPLORATION — RECLASSIFIED as an unofficial confirmation-class peek (R0
  below).** ⚠️ The X=3000 run evaluated the *real statistic* against the committed R2
  targets — that is a **confirmation-class computation performed outside the
  pre-registration protocol**, not a design-scoping exploration. The number that was
  written here as "COMMITTED empirical-side shape tolerance = 0.08 (finite-X deviation +
  density-eval 0.056)" was **set after seeing the peek's ≈0.05 residuals** and is
  **QUARANTINED as post-hoc** (ERRATA #26). Its replacement is the *a-priori* tolerance
  derived in **§R0(c)** below. The peek's outcomes are preserved in the exploration
  register §R0(a); they are NOT the confirmation.
- **Remaining (the committed run + emit):** the N/ε ORACLE CACHE (generator + reader
  with generator-hash refusal + `provenance:oracle`, like M0's ap_cache) so the emit is
  repo-reproducible; the statistic in `src/murm` (two-pass shape extraction, confirm vs
  the committed R2 targets at the **§R0(c) derived tolerance τ = 0.06** at **X_confirm =
  10⁴ (§R0b)**, convergence-with-X reported); emitter + `viz/sawin_sutherland.html`
  (claim_class in params AND caption) + freshness + schema. Deviations are deliverables.
  No M5 stubs.

---

## R0 — reclassification of the X=3000 peek  (2026-07-10 adjudication; ERRATA #26)

The last-checkpoint X=3000 run computed the real statistic (1) and compared it to the
committed R2 targets. That is **confirmation-class**, and it was run outside the
pre-registration protocol; the empirical tolerance 0.08 was then written to sit just
above its observed residuals. Treated here per the **ERRATA #19 precedent** (a
pre-declared design set/changed after results were seen). Renaming it "bounded
exploration" does **not** reclassify it — the computation it performed is what counts.

**(a) Exploration register — the peek, logged as what it was.**
> **Unofficial confirmation-class peek**, X=3000, 396 height-ordered curves, live-PARI
> N/ε, bin width Δu=0.025, a_p from M0 (`ell::ap_charsum`), statistic (1) binned by
> u=p/N. Extracted (two-pass): **hump u≈0.463, post-hump zero u≈0.67, trough u≈0.812**.
> Deviations from the R2 targets {0.475, 0.645, 0.805}: {0.012, 0.025, 0.007}, max
> **0.025**. This peek is a sanity signal only; it is **not** an M4 confirmation and its
> residuals set **no** tolerance.

**(b) Official confirmation scale — pre-named NOW, peek-untouched.**
No committed statistic scale existed in the pinned design (P5 certifies *counts* to
10⁶; the statistic had no pinned X). The peek touched only X=3000. Named here, before
any data exists at it:
- **X_confirm = 10⁴** — |fam| = **1048** (P5-certified, direct==sieve==gp); ≥ 10⁴ as
  required; the peek never touched it. Feasibility measured: `ap_charsum` over primes
  ≤ N is 51 ms/curve at N=3e3, 364 ms at N=1e4; the 1048-curve family (mean conductor
  well below the 2X ceiling) is a ~5–20 min run — feasible.
- **Convergence ladder** (all peek-untouched, monotone): X ∈ {4000, 6000, 8000, 10000}.
  The reported empiric is the L²/pointwise distance to D and the R2-location deviations,
  **decreasing in X**; X_confirm=10⁴ is the pass/fail gate.

**(c) A-priori empirical tolerance — derived, not fit.** The 0.08 is quarantined. The
location tolerance is re-derived from the R1 chain + binning + a Hasse/CLT sampling
model, before X_confirm data exists (formula side + family counts only):

  *Sampling-noise model.* The binned empirical density
  D̂_b = (1/(Δu·|fam|)) Σ_{(E,p): p/N∈b} (u_mid·ln N(E)/N(E))·ε(E)·a_p(E). Modelling a_p
  as zero-mean with the Hasse/Sato–Tate variance ⟨a_p²⟩≈p, and n_b ≈ |fam|·Δu·⟨N/ln N⟩
  samples per bin, the bin-mean standard error collapses to

      σ(u) ≈ sqrt( u³ · ln N̄ / (Δu · |fam|) ),   ln N̄ ≤ ln(2X)  (N ≤ |disc| ≤ 2X).

  *Location jitter.* A value error σ shifts an extracted extremum through the
  MACROSCOPIC curvature/slope of D (the binned estimator smooths over Δu, so the
  micro-wiggle of the truncated Bessel sum is averaged out; |D′|,|D″| measured at
  macroscopic spacing h∈{0.05,0.075} from the formula TU, conservative min):
      extremum (D′≈0):  τ = sqrt(2σ/|D″|);   crossing (D′≠0):  τ = σ/|D′|.
  At X_confirm=10⁴, |fam|=1048, Δu=0.025, ln(2X)=9.90:

      pt      u       σ(u)    |D″| or |D′|    τ_samp
      hump    0.475   0.201   |D″|≈866        0.022
      zero    0.645   0.318   |D′|≈27         0.012
      trough  0.805   0.444   |D″|≈965        0.030   ← governing

  *Budget (R1 chain, numbers).* τ_bin = Δu = **0.025** (bin granularity floor);
  τ_dens = **0.005** (R2 locations stable under B∈[200,5000] to < the 0.005 scan step —
  measured, §"COMMITTED R2 targets"); τ_samp = **0.030** (governing, trough). J₁ 6.4e-12
  is negligible below all three. Combined **linearly** (conservative — the three may
  align): **τ = 0.060.** Quadrature (statistical) estimate: 0.040. **Committed
  empirical-side location tolerance τ = 0.06** at X_confirm — tighter than the
  quarantined 0.08, i.e. a *stronger* gate, and derived a priori.

  *Model cross-check against the peek (validation, NOT a fit).* Same model at X=3000
  (|fam|=396): τ_samp = 0.030·√(1048/396) = **0.049**; the peek's actual max deviation
  was 0.025 < 0.049 — it sits *under* the a-priori band. The band was derived from the
  formula side + counts, never from the peek residual.

  *If the confirmation at X_confirm fails τ = 0.06:* that is a deliverable with full
  witness (the empirical curve, per-point deviations, convergence trend) — **not** a
  reason to revisit this derivation. The tolerance is frozen here, before the run.

**(d) ERRATA.** The commit record (`e2dbadc`) introduced the peek outcomes AND the
"COMMITTED … 0.08 (finite-X deviation + density-eval 0.056)" line in the *same* commit,
and the tolerance's own stated derivation references the "finite-X deviation" it was fit
around — so, unlike #19 (where git could not corroborate timing), here the record
**corroborates** that 0.08 postdated the peek. Filed **ERRATA #26** (a post-hoc
tolerance, softened by "bounded exploration" framing; caught by the human referee's R0
directive reading the checkpoint). Remedy: this §R0 (peek → register; a-priori τ; the
0.08 quarantined).

---

## The confirmation run at X_confirm=10⁴ — FORK COMMITTED BEFORE THE LADDER IS READ (2026-07-11)

`at ss-run` produced the empirical statistic at X_confirm=10⁴ (1048 curves, a_p from
scratch via `ell::ap_fast`, N/ε from the R3-certified oracle cache, Δu=0.025, the
committed two-pass extractor). **The confirmation point (already observed):**

    invariant   empirical u   R2 target   |dev|     vs τ=0.06
    hump        0.463         0.475       0.012     ✓
    zero        0.673         0.645       0.028     ✓
    trough      0.888         0.805       0.083     ✗  (exceeds τ)

**2 of 3 committed shape invariants agree within the a-priori τ; the trough misses.**
The τ=0.06 budget (§R0c) covered instrument+binning+*sampling* noise — it did **not**
budget the *systematic* downward bias SS document (a distinct error mode). The empirical
tail stays deeply negative near u→1 where D(u) returns toward 0, flattening the
trough→tail region so the global argmin drifts right into a noisy deep plateau. This is
the miss, reported as a deliverable — **τ and the extractor are NOT touched** (that would
be the #26 sin again).

**THE FORK — committed now, before the convergence ladder's trough trend is read.** The
verdict on the trough miss is decided by a rule fixed in advance, not chosen to fit the
trend (interpreting a convergence post-hoc is the same class as setting τ post-peek):
- **Pre-named ladder** (peek-untouched, §R0b): X ∈ {4000, 6000, 8000, 10000}.
- **Per-scale noise band:** τ_samp(X) = 0.030·√(1048 / |fam(X)|) (the §R0c model at
  scale X; larger at smaller X).
- **DECISION RULE.** The trough miss is attributed to **"finite-X downward bias"**
  ⟺ |trough_u(X) − 0.805| **decreases monotonically across the ladder** (or is
  non-increasing within the per-scale τ_samp band). **Any other pattern — flat,
  non-monotone, or increasing beyond the band — is verdict "UNEXPLAINED — OPEN
  DEVIATION,"** and the emitted `claim_class` reads **"partial agreement, open
  deviation"** rather than "empirical agreement with Conjecture 1." *The caption
  follows the fork, not the hope.* (SS's own note below makes the monotone case genuinely
  uncertain at my X-range — see the quote: the rank-2 proportion **increases** over
  2¹⁶–2²⁸, and my ladder 2¹²–2¹³·³ sits *below* that range, so a decreasing trend is
  not a foregone conclusion.)

**LOCALIZATION DISCRIMINANT — committed before it is computed.** Extract the trough with
the **same two-pass extractor restricted to u ∈ [0.7, 0.9]** (around the target, away
from the biased tail). Report **both** numbers as emitted empirics: the *global-argmin*
trough (the committed R2 test's input, stays ✗) and the *windowed* trough (diagnostic).
**Predicted discriminant:** if the tail bias is the cause, the windowed trough sits near
0.805 while the global sits at 0.888 — demonstrating the diagnosis rather than narrating
it. The windowed number is **diagnostic evidence filed with the deliverable, never a
replacement test**; the committed verdict stays global-extractor ✗.

**SS25 downward-bias passage — quoted verbatim (the diagnosis leans on it, so it is
pinned).** [SS25] p.3 (bottom, after Fig. 1) → p.4 (top):
> "An interesting feature is that part of the discrepancy between the purple and green
> dots comes from a persistent downward bias, where (1) is slightly less than its
> predicted limiting value. As part of the proof of Theorem 2, we prove a variant
> statement without the limit as P goes to ∞. Graphs of the left-hand side and
> right-hand side of this statement do not show the downward bias, suggesting it is
> genuinely a property of the primes. This downward bias seems to decay as X goes to ∞,
> suggesting it will go away in the limit, though it may decay more slowly than other
> sources of discrepancy between (1) and the right-hand side of (2)."

and the rank-2 caveat (p.4), which is why the trend is not assumed:
> "…while the proportion of curves with naive height ≤ X that have rank 2 is expected to
> decrease to 0 as X → ∞, it actually increases for the range of X we consider (from
> 2¹⁶ to 2²⁸)… Hence, to use curves of rank ≥ 2 to explain the downward bias, one would
> have to explain why the bias decreases over this range, rather than increasing as one
> might expect."

**THE ARTIFACT CARRIES THE MISS.** Emitted `params` + viewer caption state, in words:
"2 of 3 committed shape invariants within τ=0.06 at X=10⁴; trough deviation 0.083
exceeds τ, [verdict per the fork]; convergence empirics + windowed-trough diagnostic
attached." The ladder trend and the windowed trough are recorded below **after** this
fork is committed.
