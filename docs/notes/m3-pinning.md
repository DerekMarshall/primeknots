# M3 pinning log — Zubrilina replication [hard anchor, the crown]

Two-phase gate for Stage M3 (RESEARCH-M §5). **Phase 1 (this document): pinning
only**, quoted verbatim from the fetched Zubrilina PDF (`docs/papers/zubrilina-2310.07681.pdf`
= arXiv:2310.07681, *Murmurations*, Invent. Math. 241 (2025) 627–680). Equations
**read from the rendered PDF pages** (the "extraction-garble" hazard — `pdftotext`
mangled the Euler products in Theorem 1 and the trace formula; both were re-read from
the page images). **The only Phase-1 code is P4 (Hurwitz class numbers).** STOP at the
pinning review — which is expected to be **adversarial** on P1/P2/P3; convention risks
are flagged inline as **[RISK]** for that referee.

Target: **weight k = 2**, trivial character, squarefree level. M3's snapshot is
computed from scratch (Hurwitz class numbers → the trace formula → averaged), zero
external eigenvalue data; it is the ARCHITECTURE-M §9 repo-reproducible default.

---

## P1 — the trace formula, term by term

**Source, quoted (Z25 p.6):** "**Theorem (Skoruppa–Zagier ([12], section 2, formulas
(5) and (7))).** For N square-free and a prime P ∤ N,

    Σ_{f∈H^new(N,k)} √P·λ_f(P)·ε(f)
        = H₁(−4PN)/2  +  (−1)^{k/2−1} · Σ_{1≤r≤2√(P/N)} U_{k−2}(r√N/(2√P))·H₁(r²N²−4PN)
          −  δ_{k=2}(P+1)."

(First derived by Yamauchi [14], p.6; Z25 uses the Skoruppa–Zagier form.) λ_f(P) :=
a_f(P)/P^{(k−1)/2}, so for **k = 2** √P·λ_f(P) = a_f(P), giving the M3 form:

    Σ_{f∈H^new(N,2)} a_f(P)·ε(f) = H₁(−4PN)/2 + Σ_{1≤r≤2√(P/N)} H₁(r²N²−4PN) − (P+1).

(k=2: (−1)^{k/2−1}=(−1)⁰=1, U_{k−2}=U₀=1, δ_{k=2}=1.) Terms and their sanity checks:

- **Principal term** `H₁(−4PN)/2`. Discriminant −4PN. Sanity: 4PN ≡ 0 (mod 4), a valid
  Hurwitz argument; `mform::hurwitz(4PN)` (P4).
- **Elliptic terms** `Σ_{1≤r≤2√(P/N)} U_{k−2}(r√N/(2√P))·H₁(r²N²−4PN)`. The range
  1 ≤ r ≤ 2√(P/N) is exactly where r²N² − 4PN < 0 (⟺ r²N < 4P), i.e. the t²−4p<0
  elliptic range; the argument magnitude is N(4P − r²N) > 0. Sanity: each is a Hurwitz
  value of a negative discriminant; the U_{k−2} is **1 for k=2** so the k=2 elliptic
  sum is Σ_r H₁(N(4P − r²N)). **[RISK]** for k>2: whether U_{k−2}(r√N/(2√P)) sits
  INSIDE the r-sum (the argument depends on r, so it must) — confirmed by the argument
  dependence, but flagged for the layout.
- **Parabolic / dimension term** `−δ_{k=2}(P+1)`: present only for k=2, absent for k>2.
  Sanity: the whole RHS must be an **integer** (the LHS is Σ of integers a_f(P)·ε(f));
  the rational H₁ terms must combine with −(P+1) to an integer — a mechanical Phase-2
  check (REQUIRE assembled-RHS ∈ ℤ), independent of any oracle.
- **Level structure:** Γ₀(N), **trivial character**, weight-k newforms H^new(N,k).

**Squarefree rewrite, quoted (Z25 eq. (1), p.6):** for N square-free and P ∤ 2N,

    Σ_{f∈H^new(N,k)} √P·λ_f(P)·ε(f)
        = h(−4PN)/2 + h(−PN)/2 − δ_{k=2}P
          + (−1)^{k/2−1} Σ_{1≤r≤2√(P/N)} U_{k−2}(r√N/(2√P)) Σ_{d²|r²N−4P} h(N(r²N−4P)/d²) + O(1)

where h is the Gauss class number, via H₁(−d) = Σ_{f²|d} h(−d/f²) + O(1) (P4). The
squarefree simplification (only square factors 1,4 of 4PN survive; the d=2 term
vanishes) is quoted in full on p.6. **[RISK]** the O(1) error terms — the trace formula
(H₁ form) is EXACT; the h-rewrite carries O(1) errors (from H₁ vs h, disappearing
unless d ∈ 3·□ ∪ 4·□). M3 should use the **exact H₁ form** (no O(1)), reserving the
h-rewrite for reading her class-number-formula analysis. Recorded as a decision.

## P2 — old-to-new descent

Zubrilina's trace formula is **already for newforms**: it is the trace of an operator
on `S^new(N,k)` (Skoruppa–Zagier's newform trace formula), so there is **no separate
S₂ → S₂^new Möbius descent** to implement — the descent is subsumed by using the
newform trace formula on squarefree level via W_N. **[RISK]** verify (adversarial) that
Skoruppa–Zagier ([12] §2 (5),(7)) is genuinely the *newform* trace (not S(N,k) requiring
descent); Z25 states H^new and cites it as such (p.6, l.49/262). **Independent anchor
(no eigenvalues):** dim S₂^new(N) for squarefree N is checkable against LMFDB dimension
data — the normalization denominator Σ_{N} Σ_{f∈H^new(N,2)} 1 = Σ_N dim S₂^new(N). Pin
the squarefree dim formula and REQUIRE it against LMFDB dimensions as its own test
(Phase 2), independent of any eigenvalue.

## P3 — the root-number weighting (the second, hidden, trace)

**Quoted (Z25 §2, p.5, l.264–267):** "(−1)^{k/2}·ε(f) is equal to the eigenvalue of f
under [the Atkin–Lehner operator W_N]"; the trace is of "the operator (−1)^{k/2}·T_p ∘ W_N
on S^new(N,k)". So the ε-weighting is **not a separate sum** — it enters at the trace
level through W_N: Tr((−1)^{k/2} T_p W_N) = Σ_f (−1)^{k/2}·λ_f(P)·(W_N-eigenvalue) =
Σ_f (−1)^{k/2}·λ_f(P)·(−1)^{k/2}·ε(f) = Σ_f λ_f(P)·ε(f). **[RISK — the sign-convention
hiding spot]** the exact relation `W_N-eigenvalue = (−1)^{k/2}·ε(f)`; a sign error here
flips ε and inverts the entire murmuration. Independent sanity: for k=2 the W_N
eigenvalue equals ε(f) (since (−1)^{k/2}=−1... **check**: (−1)^{k/2}=(−1)¹=−1 for k=2, so
W_N-eig = −ε(f); the (−1)^{k/2} in the operator cancels it → +ε(f) in the trace). This
double-sign is EXACTLY the convention to referee against the literature; flagged.

## P4 — Hurwitz class numbers  [DONE — the one Phase-1 code]

**Definition, quoted (Z25 p.6):** "H₁(−d) is the number of equivalence classes with
respect to SL₂(ℤ) of positive-definite binary quadratic forms of discriminant −d
weighed by the number of automorphisms (i.e., with forms corresponding to multiples of
x²+y² and x²+xy+y² counted with multiplicities 1/2 and 1/3, accordingly). H₁ can be
expressed in terms of the Gauss class number h via H₁(−d) = Σ_{f∈ℕ: f²|d} h(−d/f²) + O(1),
with the error term disappearing if d ≠ 3·□, 4·□."

Implemented `src/mform/hurwitz`:
- **anchors (convention-carriers, from source):** H(0) = −1/12, H(3) = 1/3, H(4) = 1/2,
  plus standard H(7)=1, H(12)=4/3, H(23)=3, H(100)=5/2, and H(n)=0 for n ≡ 1,2 (mod 4).
- **twin:** `hurwitz_by_forms` (direct weighted count of ALL reduced positive-definite
  forms) vs `hurwitz_by_decomposition` (Σ_{f²|n} weighted primitive class numbers) —
  agree over n ∈ [0, 3000].
- **oracle:** vs PARI `qfbhclassno(n)` (renamed from `hclassno`) over n ∈ [0, 4000] — all
  4001 match, exact rationals.
- **reuse note:** shares the binary-quadratic-forms *mathematics* with Stage 3 (the 1801
  machinery), but the code is separate — Stage 3 reduces INDEFINITE forms (D>0), Hurwitz
  needs POSITIVE-DEFINITE reduction (D<0), added here.
- **[perf, Phase 2]** the trace formula needs H₁(4PN) with 4PN up to ~10⁷–10⁸; the O(√n)
  per-value referee is too slow at scale — Phase 2 adds a fast path (Σ_{f²|n} h with a
  fast h, or a sieve of H over a range), twinned against this referee. Not M0-grade
  frozen yet; flagged.

## P5 — the density, its variable, and the discontinuities (the anchor's teeth)

**Variable (quoted, Z25 Theorem 1, p.2):** "Let **y := P/X**." (NOT P/N — pinned
explicitly; X is the interval scale.) **Density (quoted, Theorem 1, p.2), for k=2:**

    M_k(y) = (α(−1)^{k/2−1}/(k−1)) Σ_{1≤r≤2√y} ν(r)·√(4y−r²)·U_{k−2}(r/(2√y))
             + (β/(k−1))·√y − γ·δ_{k=2}·y

    ⇒  M₂(y) = α·Σ_{1≤r≤2√y} ν(r)·√(4y−r²)  +  β·√y  −  γ·y

with the Euler-product constants (verbatim from the page, Σ□ = square-free sum):

    α = 2π · Π_p (1 − p − 2p² + p⁴)/(p⁴ − 2p² + p)
    β = 2π · Π_p (−1 + p² + p³)/(p(−1 + p + p²))
    γ = 12  · Π_p p(1+p)/(−1 + p + p²)
    ν(r) = Π_{p|r} (1 + p²/(p⁴ − 2p² − p + 1))
    U_n(cos θ) = sin((n+1)θ)/sin θ   (Chebyshev U)

**[RISK]** the three Euler products — their exact numerators/denominators AND their
convergence (each per-prime factor → 1; α,β,γ are specific constants). To be evaluated
in an **independently authored TU** (the Stage-4 separate-sessions protocol; formula side
never sees the empirical side), as truncated products with a convergence check.

**Discontinuities (quoted, Z25 p.4):** "[the M_k(y)] are oscillating, continuous, with
**derivative discontinuities at n²/4 for n ∈ ℕ**. At the origin, [growth rate ~√y] and
are all positive … At infinity, … growth rate of y^{1/4}." The discontinuities are the
anchor's teeth: they sit where 2√y crosses an integer r (the r-sum's upper limit), i.e.
**y = n²/4 (n = 1,2,3 → y = 0.25, 1.0, 2.25)**. A convention error in y shifts them, so
**REQUIRE the empirical derivative-discontinuity locations at y = n²/4** as its own test
(Phase 2), separate from any L² distance.

(Z25 Theorem 3, p.4, gives an alternative Bessel-J closed form for M_k; kept as a
secondary cross-check of the finite-r-sum M₂ above.)

## P6 — the statistic, the family, and the pre-registered design

- **Family:** square-free levels N, weight k=2, trivial character, Γ₀(N) newforms.
- **Statistic (Theorem 1, short interval):** the ε-weighted normalized average
  [Σ□_{N∈[X,X+Y]} Σ_f a_f(P)ε(f)] / [Σ□_{N∈[X,X+Y]} Σ_f 1] → M₂(y), y=P/X. Empirical side
  = the **trace formula evaluated per N** (from Hurwitz class numbers, P4/P1), summed
  over square-free N, normalized by Σ dim S₂^new(N).
- **Also (Theorem 2, p.3, dyadic):** [Σ□_{N∈[X,cX]}…]/[Σ□…] = (2/(c−1)) ∫₁^c u·M_k(y/u) du
  + o(1). The dyadic interval (more levels → less noise) is the intended primary
  replication; the short interval is the direct-density check.
- **Pre-registered design (commit-before-run, RESEARCH-M §7).** The Phase-2 confirmation
  will be committed BEFORE it runs. Fixed here structurally; the numeric tolerances are
  finalized from *bounded exploration* and committed in the Phase-2 record (as M2 did):
  1. **TWIN first (before any averaging):** assembled Tr T_p (from the P1 formula) vs
     PARI `mfcoefs` / LMFDB eigenvalue sums Σ_f a_f(P)ε(f) over a REQUIREd (N,P) sample
     of square-free N. This is the non-circular check — the class-number formula vs
     actual newform eigenvalues.
  2. **Assembled-RHS ∈ ℤ** per (N,P) (the H₁-terms + −(P+1) must be integral).
  3. **Convergence** of the averaged trace to M₂(y): dyadic (Theorem 2) primary, short
     (Theorem 1) secondary, over an X-grid, tolerance + null committed pre-run.
  4. **Discontinuity-location test:** empirical derivative kinks at y = n²/4 (P5).
  5. **dim S₂^new(N)** vs LMFDB (P2), eigenvalue-independent.
  6. **Separate-TU protocol** (Stage 4): the empirical-average side and the M₂-formula
     side are authored in separate sessions, neither edited after first comparison.

---

## R5 — the H₁(0) boundary cannot occur for the family

The r-sum boundary r² = 4P/N (where the disc r²N²−4PN = 0 and H₁(0) = −1/12 would
enter) requires 2√(P/N) ∈ ℤ, i.e. 4P/N a perfect square. For square-free N ≥ 3 and
prime P ∤ N: 4P/N ∈ ℤ needs N | 4P, and gcd(N,P)=1 ⇒ N | 4 ⇒ N ∈ {1,2} (4 is not
square-free) — excluded. So 2√(P/N) is never integral for the family; r_max =
⌊2√(P/N)⌋ < 2√(P/N) strictly, hence r²N²−4PN < 0 strictly for every term and H₁(0)
never enters. (`trace.cpp` sums 1 ≤ r ≤ ⌊2√(P/N)⌋; boundary handling not needed.)

## Phase 2 — pinning review PASSED; trace formula validated (R1/R2/R3)

Referee hand-adjudicated P1 at N=11, P∈{2,3,5} against M0's independent a_P — the
referee-verified cells (quoted): **P=2 → −2, P=3 → −1, P=5 → 1**. `src/mform/trace`
assembles the P1 formula; the ladder adjudicates itself:
- **R1 `anchor_trace_dim1`** (N∈{11,14,15}, dim 1, ε=+1): assembled trace == a_P(E)·ε,
  a_P from M0 (`ell::ap_charsum`/`ap_enumerate`), 40 cells.
- **R2 `anchor_trace_mixed_sign`** (N=37: 37a ε=−1, 37b ε=+1): trace == a(37a)·(−1) +
  a(37b)·(+1) — the ε=−1 branch of the §P3 sign chain exercised (the mod-8 lesson).
- **R3 `anchor_trace_old_part`**: N=33 (dim S₂=3: 2 old from level 11, 1 new) → trace
  == newform 33a only (old part excluded); N=22 (dim S₂=2, all old) → trace == 0. The
  old-part-vanishing lemma tested mechanically, not trusted.
- `theorem_trace_is_integral`: assembled RHS ∈ ℤ over 246 (square-free N, P) pairs.
Root numbers quoted from PARI `ellrootno`: 11a/14a/15a/33a = +1, 37a = −1, 37b = +1.

**Density side done (`src/murm/zub_density`, separate TU):** M₂(y) = α·Σν(r)√(4y−r²)
+ β√y − γy with the verbatim Euler products; `theorem_zub_density_teeth` — the **R4
kink locator** (second-difference spike, grid h=0.004, tol 0.01, committed pre-run)
finds the derivative discontinuities at **0.248, 1.000, 2.248 ≈ {1/4, 1, 9/4}**;
`theorem_zub_density_features` — positive root-number bias near 0 (M₂(0.05)>0) and
oscillation (6 sign changes). The formula side is authored independently of the
trace-assembly side (separate-TU protocol); neither edited after this comparison.

**Remaining Phase 2 (gated by session budget, pre-registered):** the dimension-
normalized **empirical convergence** — [Σ□_N Tr T_P|S₂^new(N)]/[Σ□_N dim S₂^new(N)]
(assembled from `mform::trace`, needs a `dim S₂^new` genus formula + a fast H₁ at
scale, P4-perf note) → M₂(y), applying the R4 locator to the EMPIRICAL curve (its
teeth at n²/4) — and the emitter + viewer + freshness. The convergence tolerance is
committed in that step before it runs. The high-risk mathematics (the trace formula
conventions R1/R2/R3, the density formula + its teeth R4/P5) is validated; the
remainder averages already-validated components. No M4 stubs. Branch `murmurations`,
local only.
