# Stage 5 pinning — Riemann–Siegel, Turing certification, explicit formula, Ruelle twin

**Scope.** Light pinning, per the Stage 5 authorization: the traps here are
*numerical*, not conventional. There is no sign/normalization ambiguity of the
kind that dominated Stages 2 and 4 — the objects (ζ zeros, ψ, a dynamical zeta)
have fixed classical definitions. What this document pins is (a) the exact
formulas we implement, with their source; (b) the completeness discipline
(rider R1 — Turing's method, not Gram's law); (c) the explicit-formula
normalization and the jump-midpoint erratum (rider R3); (d) the fully-constructed
model whose Ruelle zeta twins Euler-product against determinant (rider R4).

**Binding correctness gate.** Per CLAUDE.md non-negotiables #1 and #3, the
operational referee for the zero computation is the **Odlyzko table** (oracle,
`data/odlyzko/`, ≥ 8 dp). Where a coefficient or section number below is stated
from memory of Edwards [E74] (a book, not committable to `docs/papers/`), it is
marked **UNVERIFIED** and must be transcribed from a physical copy before the
attribution is considered pinned. A coefficient that is wrong makes the Odlyzko
anchor fail; the fix is then to the *reading* (correct the coefficient from the
source), never to fudge the comparison. Chapter-level topic attributions to
Edwards are standard and are given without the UNVERIFIED tag; equation- and
section-number precision carries it.

---

## 1. Riemann–Siegel: θ, Z, and the remainder  (Edwards [E74], Ch. 7)

**Riemann–Siegel theta.** With Γ the gamma function,

    θ(t) = arg Γ(1/4 + i t/2) − (t/2)·log π
         = Im log Γ(1/4 + i t/2) − (t/2)·log π          [exact; used directly]

Asymptotic (used only as a Newton seed for Gram points, not in Z):

    θ(t) ≈ (t/2) log(t/2π) − t/2 − π/8 + 1/(48 t) + 7/(5760 t³) + …

Grounding: θ has its first Gram point (θ(g₀)=0) at g₀ = 17.8455995…, reproduced
by our formula (PARI, 2026-07-07).

**Hardy Z-function.** Z(t) = e^{iθ(t)} ζ(1/2 + i t) is real; its real sign changes
are the ordinates γ of the nontrivial zeros ρ = 1/2 + iγ (RH-agnostic: Z detects
zeros *on* the critical line, which is all our range contains and all Odlyzko
lists).

**Riemann–Siegel formula.** Let τ = t/(2π), N = ⌊√τ⌋, p = √τ − N ∈ [0,1). Then

    Z(t) = 2 Σ_{n=1}^{N} n^{−1/2} cos(θ(t) − t·log n)
           + (−1)^{N−1} τ^{−1/4} Σ_{k=0}^{K} C_k(p) τ^{−k/2} + O(τ^{−(K+1)/2−1/4})

The first term is the **main sum**; the second is the remainder with correction
terms C_k. We pin:

    C_0(p) = Ψ(p) = cos(2π(p² − p − 1/16)) / cos(2π p)            [exact, certain]

The higher C_k (k ≥ 1) are polynomials in derivatives of Ψ; C_1 has the form
−Ψ‴(p)/(96 π²) **[UNVERIFIED — Edwards §7.4 / Gabcke coefficients]**. The
implementation carries C_0..C_K with K chosen so the Odlyzko anchor holds to
≥ 8 dp across the tested range (t ≲ 9878 for the first 10⁴ zeros: τ ≲ 1572,
each term ~ τ^{−1/2} ≈ 0.025 smaller, so K = 3 is the working target). **The C_k
values are validated by, not asserted ahead of, the Odlyzko cross-check.**

**Gram points.** g_n solves θ(g_n) = nπ, n ≥ −1; computed by Newton from the
asymptotic seed. Gram's law (one zero per Gram interval, Z(g_n) sign = (−1)^n)
is a **heuristic**; per rider R1 Gram points may *seed* the scan but never define
the count.

**Twin / oracle.** Our Z(t) sign-change zeros are the verified twin; Odlyzko is
the accuracy oracle (`anchor_zeros_match_odlyzko`, ≥ 8 dp, max-deviation reported
as an empiric). No second in-house algorithm for the zeros themselves — the
external oracle *is* the second computation (CLAUDE.md #2, "or an external oracle
check"). The completeness of the set is a separate in-house certificate (§2).

---

## 2. Completeness: N(T) via Turing's method  (rider R1; Edwards [E74], Ch. 8)

**Why not sign-change counting alone.** A fine grid can step over a close pair
(both zeros between two samples → no net sign change → two zeros missed). The
Lehmer pair near t ≈ 7005.06 (γ = 7005.062866175 and 7005.100564674, gap 0.0377;
Odlyzko lines 6709–6710) is the canonical example and gets an explicit test
(rider R2).

**Riemann–von Mangoldt.**

    N(T) = θ(T)/π + 1 + S(T),   S(T) = (1/π)·arg ζ(1/2 + i T)   (continuous from 2)

The naive integer estimate round(θ(T)/π) + 1 assumes S(T) ≈ 0 and **fails**:
grounding witness (PARI, 2026-07-07) at T = 500 gives round(θ/π)+1 = 270 but the
true count is **269** (S(500) ≈ −1). At T = 100 and T = 280 it happens to agree
(29, 126). So the count must be *certified*, not rounded.

**Turing's method (pinned as the completeness certificate).** S(T) averages to 0
and its integral is bounded: for 2πe ≤ T₁ < T₂,

    |∫_{T₁}^{T₂} S(t) dt| ≤ 2.30 + 0.128·log(T₂/2π)     [Lehman 1970 bound; **UNVERIFIED constants**]

Evaluated at a run of Gram points, this bound is < 1 over a modest block, and
since N is integer-valued the bound pins N(g_n) exactly from θ(g_n)/π plus the
sign pattern of Z at the Gram points in the block (a "Turing-good" endpoint is a
Gram point where the block sum forces the integer). **Requirement (R1):** on every
scanned window the number of zeros our finder locates must equal the Turing-certified
N(T); and independently must equal the Odlyzko count. Three-way agreement
(finder = Turing = Odlyzko) or the window is a deliverable with a full witness
(T, found list, N(T), Odlyzko slice), thread stops — not reconciled by loosening
the grid.

**Close-pair handling.** Where finder-count < Turing-count in a window, the scan
adaptively refines (bisection subdivision) in the deficit sub-intervals until the
counts reconcile or a genuine discrepancy is certified. This is what exercises the
Lehmer path.

---

## 3. Explicit formula: normalization + jump-midpoint erratum  (rider R3)

**Formula (RESEARCH.md §7.2), von Mangoldt:**

    ψ₀(x) = x − Σ_ρ x^ρ/ρ − log 2π − ½ log(1 − x⁻²)

- `x` — the main term (pole of ζ at s=1).
- `Σ_ρ x^ρ/ρ` — sum over nontrivial zeros ρ = ½ ± iγ. **Conjugate pairs are
  combined analytically:** x^ρ/ρ + x^ρ̄/ρ̄ = 2·Re(x^ρ/ρ), and with ρ = ½+iγ,
  x^ρ/ρ = x^{1/2} e^{iγ log x}/(½+iγ). We compute 2·Re once per pair — **never**
  sum ρ and ρ̄ as separate floating-point terms (rider R3: catastrophic
  cancellation of the imaginary parts otherwise).
- `− log 2π` — the s=0 / trivial constant.
- `− ½ log(1 − x⁻²)` — the sum over trivial zeros (−2,−4,…).

**Erratum against RESEARCH.md §7.2 (see open-questions R6).** The series converges
to the **normalized** ψ₀(x) = ½(ψ(x⁻) + ψ(x⁺)), not to ψ(x). ψ(x) = Σ_{n≤x} Λ(n)
is right-continuous; at a prime power x = p^k it jumps by Λ(x) = log p, so

    ψ₀(x) = ψ(x) − Λ(x)/2   at prime powers,   ψ₀(x) = ψ(x)   elsewhere.

Witness (PARI, x=8=2³): ψ(8⁻)=6.04025471, ψ(8⁺)=6.73340189, midpoint 6.38682830 =
ψ(8⁺) − log2/2. **The reconstruction target and the error metric use ψ₀.**

**Convergence tests (R3).**
- *Non-prime-power x* (e.g. x = 10, 100.5): partial sum over the first M computed
  zeros → ψ₀(x) = ψ(x); max-error decreases as M grows (documented curve).
- *Prime-power x* (e.g. x = 8, 9, 32): partial sum → the **midpoint** ψ(x) − Λ(x)/2.
  Asserting convergence to ψ(x) here would be a spec bug (this is the erratum).
  Truncated sums Gibbs-overshoot at the jump, so the prime-power assertion uses a
  tolerance that tightens with M rather than a fixed bound, and is stated about
  the symmetric-partial-sum limit.

**Twin.** Orbit side = ψ(x) computed directly as Σ_{p^k ≤ x} log p (from Stage 0
primality). Spectral side = the zero-sum reconstruction. Their agreement (at the
midpoint) *is* the explicit formula — the two sides of Deninger's duality.

**Animation.** Frames indexed by zero count M; each added zero contributes one
wave 2 Re(x^ρ/ρ). Emit per the `psi_reconstruction` schema (frames of (x, ψ₀-
approx) arrays + the direct ψ staircase + per-frame max-error).

---

## 4. Model system: Ruelle zeta of a subshift, Euler product = determinant  (rider R4)

**Construction (fully explicit; Parry–Pollicott [PP90]).** Take a subshift of
finite type (SFT) on a finite alphabet with 0–1 transition matrix A (adjacency
matrix of a directed graph). The **golden-mean shift** is the base model:

    A = [[1,1],[1,0]]   (forbids the word "11"; tr Aⁿ = Lucas number L_n)

Periodic points: Fix(σⁿ) = tr(Aⁿ) = number of closed length-n paths. The
Artin–Mazur / Bowen–Lanford dynamical zeta of the shift is

    ζ_σ(z) = exp( Σ_{n≥1} (tr Aⁿ / n) zⁿ ) = 1 / det(I − zA)            [Bowen–Lanford]

and in Euler-product form over **primitive** closed orbits γ (period ℓ(γ)):

    ζ_σ(z) = Π_γ (1 − z^{ℓ(γ)})^{−1}

**Suspension flow / Ruelle zeta.** With a roof (ceiling) function r: alphabet → ℝ₊,
the suspension flow's closed orbits have length ℓ(γ) = Σ_{i∈γ} r(a_i), and the
Ruelle zeta is

    ζ_flow(s) = Π_γ (1 − e^{−s·ℓ(γ)})^{−1} = 1 / det(I − L_s),   L_s = diag(e^{−s r})·A

- Constant roof r ≡ 1 ⇒ z = e^{−s}, ζ_flow(s) = 1/det(I − e^{−s}A) = 1/(1 − z − z²)
  for the golden mean (poles at z = 1/φ; abscissa of convergence s = log φ).
- **Log-roof** r = (log 2, log 3) ⇒ orbit lengths are sums of logs — deliberately
  echoing the arithmetic "length log p" — and ζ_flow(s) = 1/det(I − L_s) with
  L_s = diag(2^{−s}, 3^{−s})·A. This is the panel shown beside ψ.

**Twin (R4).**
- *Orbit side:* ζ_N(s) = exp( Σ_{n=1}^{N} tr(L_s^n)/n ) truncated (periodic-orbit /
  trace side), and independently the primitive-orbit product Π_{ℓ(γ)≤·}(1 − e^{−sℓ(γ)})^{−1}.
- *Determinant side:* 1/det(I − L_s), a genuine matrix determinant (algorithm-
  independent from the orbit sum).
- Twinned over a grid of s in the region of convergence (s > log φ for the golden
  mean); assert orbit → determinant as N grows, error decreasing.

**Contrast label (required in the viewer).** This subshift is a system where the
orbits genuinely exist and the Euler-product = determinant identity *is* geometry
(closed paths ↔ spectrum of A). The arithmetic case (§3) has the same identity
shape — Euler product over primes / sum over orbits ↔ spectral determinant / sum
over zeros — but **no such dynamical system is constructed**: Deninger's foliated
flow is conjectural [D98], [D02]. The panel sits beside the ψ animation labeled:
*this subshift — fully constructed; Deninger's system — not constructed; same
identity shape.*

---

## 5. Object / anchor map and verification plan

| Object | Code | Anchor / referee |
|---|---|---|
| θ(t) | `riemann_siegel::theta` | g₀ = 17.8455995… (θ(g₀)=0) |
| Z(t) | `riemann_siegel::Z` | sign changes at Odlyzko γ |
| zeros γ_n | `zero_finder::zeros_upto` | Odlyzko ≥ 8 dp; γ₁=14.134725142, γ₁₀₀₀₀=9877.782654004 |
| N(T) | `zero_finder::N_turing` | round-fails-at-500 witness (270 vs 269); = Odlyzko count |
| Lehmer pair | close-pair test | γ ∈ {7005.062866175, 7005.100564674}, gap 0.0377 |
| ψ direct | `explicit_formula::psi` | ψ(8⁺)=6.73340189 |
| ψ₀ midpoint | reconstruction limit | 6.38682830 at x=8 |
| ζ_flow twin | `ruelle::*` | golden mean: 1/(1−z−z²), poles 1/φ |

**Verification categories.** twin_ (Ruelle Euler-product vs determinant; ψ orbit
vs spectral), theorem_ (explicit-formula convergence over sampled x; Bowen–Lanford
tr Aⁿ = closed-orbit count), anchor_ (Odlyzko zeros, Lehmer pair, ψ(8) midpoint,
golden-mean poles), invariance_ (zero set independent of grid step / Gram seeding;
ζ_flow independent of orbit-enumeration order), oracle_ (Odlyzko table; visible
SKIP if `data/odlyzko/zeros1` absent — `oracle/fetch_odlyzko.py`). Case counts
certified per precedent.

---

## 6. Recorded R2 result — full-coverage Odlyzko comparison  (recorded 2026-07-07)

Per rider R2 the accuracy claim must be reported at full coverage, not just the
suite default. Stating what was tested:

- **Oracle table:** `data/odlyzko/zeros1`, **100 000** zeros to 9 dp (γ₁ =
  14.134725142 … γ₁₀₀₀₀ = 9877.782654004). Full coverage is available; nothing
  was unavailable.
- **Default suite run** (`anchor_zeros_match_odlyzko`, t_max = 1000): 648 zeros,
  max |ours − Odlyzko| = **4.99e-10**, count matches the oracle.
- **Full-coverage run** (`--extended`, t_max = 9900, covers the first ~10⁴ zeros):
  **10 025 zeros compared**, our count == the Odlyzko count below t_top,
  max |ours − Odlyzko| = **2.91e-9 at t ≈ 5764.96**. Passes the ≥ 8 dp bar
  (< 5e-9) for every zero.
- **On the 2.91e-9 residual:** it is the *Odlyzko table's* own last-digit floor,
  not our error. At t ≈ 5765 an independent PARI `lfunzeros` high-precision value
  (5764.96355671509) agrees with ours to < 1e-9, while Odlyzko's tabulated
  5764.963556718 sits 2.9e-9 away. So the deviation is the reference's precision,
  confirmed by a third computation — the achievable bar is the table's 9 dp.

R2 is closed: ≥ 8 dp per zero over the first 10⁴, max deviation and count reported.
