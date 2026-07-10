# M2 pinning log — Dirichlet-character murmurations [hard anchor, from scratch]

Two-phase gate for Stage M2 (RESEARCH-M §4). **Phase 1 (this document): pinning
only**, quoted verbatim from the fetched LOP PDF (`docs/papers/lop-2307.00256.pdf`
= arXiv:2307.00256, Lee–Oliver–Pozdnyakov, *Murmurations of Dirichlet characters*,
IMRN 2025). **STOP at the pinning review** — no averaging pipeline, no emitter, no
viewer until sign-off. Equations read directly from the PDF pages (not pdftotext,
which garbled eq. (1.3)).

This is the **hard-anchor, zero-external-data** rung: the family, the statistic, and
the density are all computed from scratch (characters, Gauss sums, primes) and the
target is an **exact closed form**, not a soft figure. M2's snapshot is therefore the
**repo-reproducible default** of ARCHITECTURE-M §9.

M2 targets LOP's **first** family (Theorem 1.1 — complex characters normalized by
Gauss sums). The second family (Theorem 1.2 — real characters χ₈d, eqs (1.5)–(1.7),
via Möbius/Polya–Vinogradov) is harder and **out of M2's initial scope** (a possible
M2b).

---

## P1 — the family, parity classes, and ordering

**Quoted (LOP §1, p.1):** "we let 𝒟₊(N) (resp. 𝒟₋(N)) denote the set of primitive
even (resp. odd) Dirichlet characters mod N."

- **Parity classes:** even = χ(−1) = +1 (𝒟₊); odd = χ(−1) = −1 (𝒟₋). Characters are
  **primitive** (conductor exactly N).
- **Ordering:** by **conductor N**, restricted to **N prime**, over conductor
  intervals — either the *geometric* interval N ∈ [X, cX] (eq. 1.1) or the *sharp*
  interval N ∈ [X, X+X^δ] (eq. 1.2). The murmuration is a function of y ∈ ℝ_{>0},
  the scale variable (prime ≈ yX).

## P2 — the exact statistic  (LOP eqs. (1.1), (1.2), verbatim)

Let ⌈x⌉_p = the smallest prime ≥ x, and τ(χ) = the Gauss sum of χ. For c ∈ ℝ_{>1},
δ ∈ (0,1), y ∈ ℝ_{>0}:

    (1.1)   P±(y, X, c)  = (log X / X)   · Σ_{N∈[X,cX], N prime}   Σ_{χ∈𝒟±(N)} χ(⌈yX⌉_p) / τ(χ)

    (1.2)   P̃±(y, X, δ) = (log X / X^δ) · Σ_{N∈[X,X+X^δ], N prime} Σ_{χ∈𝒟±(N)} χ(⌈yX⌉_p) / τ(χ)

The averaged quantity is **χ(p)/τ(χ)**. Quoted justification (LOP §1, p.1): "χ(p)/τ(χ)
is the Fourier coefficient of χ̄ when expanded in terms of additive characters (see,
e.g., [IK04, equation (3.12)]), and so this is a natural analogue of the modular form
case." The log X/X (resp. log X/X^δ) factor is the prime-count normalization of the
interval; eq. (1.2) is conditional on RH (so [X, X+X^δ] contains primes for δ > ½).
τ(χ) = Σ_{a mod N} χ(a) e(a/N) is the standard Gauss sum (computed from scratch).

## P3 — the closed-form density  (LOP Theorem 1.1, eqs. (1.3), (1.4), verbatim)

**Theorem 1.1.** Fix y ∈ ℝ_{>0}.

- **Geometric interval (unconditional), eq. (1.3):** if c ∈ ℝ_{>1},

      lim_{X→∞} P±(y, X, c) = ∫₁^c cos(2πy/x) dx     if +   (even)
                            = −i ∫₁^c sin(2πy/x) dx  if −   (odd)

  (Read from the PDF: there is **no prefactor** on the integral — pdftotext had
  suggested a spurious `1/c`; the source shows a bare ∫₁^c. Flagged and corrected.)

- **Sharp interval (assuming RH, δ ∈ (½,1)), eq. (1.4):**

      lim_{X→∞} P̃±(y, X, δ) = cos(2πy)       if +   (even)
                             = −i sin(2πy)    if −   (odd)

**`theorem_murmuration_dirichlet` target (M2 Phase 2).** Empirical family averages
converge to these exact densities as X grows:
- **PRIMARY** (cleanest, unambiguous): P̃±(y,X,δ) → cos(2πy) (even) / −i sin(2πy)
  (odd), eq. (1.4). Verified on a y-grid at increasing X; convergence is the assertion.
- **SECONDARY** (unconditional, no RH): P±(y,X,c) → ∫₁^c cos(2πy/x) dx (even) /
  −i ∫₁^c sin(2πy/x) dx (odd), eq. (1.3) — the integral evaluated in an independently
  authored TU (Stage-4 separate-sessions spirit: empirical side and closed-form side
  never share code).
The **convergence rate** (how fast the empirical average approaches the density as X
grows) is recorded as an **empiric (observed)**, not asserted as a theorem
(RESEARCH-M §4). Tolerance for "converged" fixed before the run, with justification
(to be pinned in the Phase-2 record; anti-shopping clause applies).

## P4 — zero external data (the ARCHITECTURE-M §9 default)

Everything is computed **from scratch**; nothing is read from ecdata, LMFDB, or any
oracle table:
- primes and ⌈yX⌉_p — Stage-0 machinery (sieve / Miller–Rabin);
- Dirichlet characters mod a prime p — via a **primitive root** g mod p (NEW to M2):
  χ_k(g) = e(k/(p−1)), χ_k(g^j) = e(kj/(p−1)); χ_k even ⟺ k even, χ_k primitive ⟺
  k ≠ 0 (for prime conductor);
- Gauss sums τ(χ) = Σ_a χ(a) e(a/N) — NEW, in complex double (or exact roots of unity);
- the averaged quantity χ(p)/τ(χ) — complex arithmetic.
**Assert in the suite: zero external data consumed** (M2 reads no `data/` file). Its
snapshot is a pure function of committed code → **fully repo-reproducible**, freshness-
byte-checked from the repo alone in CI (the §9 default; contrast M1's committed
extract). Reuse: `symbols/` (Legendre only where a quadratic character helps),
`core/` (primes, modpow). Primitive-root finding and Gauss sums are the new load.

## P5 — certified family counts

For a **prime** conductor p, the character group is cyclic of order p−1; χ_k(−1) =
(−1)^k, and every non-principal character is primitive (conductor p). Hence the exact
counts, to be REQUIREd against an independent enumeration:

    |𝒟₊(p)| = (p−3)/2   (primitive even  = even minus the principal χ₀),
    |𝒟₋(p)| = (p−1)/2   (primitive odd),
    total primitive = p−2 = (p−1) − 1.

Each family count the pipeline uses (per prime conductor, and summed over the interval)
is REQUIREd against this closed form AND an independent character enumeration — the
certified-count discipline (RESEARCH-M §4), the M2 analogue of M1's `anchor_family_counts`.

---

## Phase-2 boundary (gated)

No character/Gauss-sum/averaging code, no `theorem_murmuration_dirichlet`, no emitter,
no viewer until this pinning review is signed off. Branch `murmurations`, local only.
Two-phase gate: this log is reviewed against LOP before the pipeline TU is written.
