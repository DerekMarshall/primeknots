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

**Recommendation for the gate (Derek to rule):** (a) with the §2 amendment + the
overlap spot-check + (if cheap) a from-scratch Tate conductor twin. This keeps the
actual murmuration quantity (a_p) from-scratch while treating N, ε as oracle-supplied
metadata — the M1-continuous reading. But this is the **named decision** of M4; I do
not resolve it unilaterally.

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

Still local (push-on-green per the M-branch authorization; no merge to main). No M5
stubs. Design fixed here, numeric tolerances committed before the confirmation run:
1. **a_p from-scratch** (M0) over the height family; **N, ε per the P4 ruling**.
2. **Family counts** certified (P5, two enumerations agree) before any averaging.
3. **Empirical statistic (1)** assembled: ε-weighted a_p averaged over p/N ∈ (C₁,C₂),
   over height-ordered curves; **separate-TU** from the D(u) formula side (P3).
4. **Shape-invariant test** (P3) — committed peak/zero/trough locations + tolerances
   (no √-cusp teeth exist); the reported empiric is the L² / pointwise distance to D,
   decreasing with X. The persistent **downward bias** SS note (p.3–4, (1) sits below
   the RHS, decaying in X, possibly rank-2 related) is reported, not fitted away.
5. **Emitter + viewer + freshness**, byte-portable (the J₁ cross-libm hazard, P3);
   schema; validate-json. Snapshot committed with the emitter.
