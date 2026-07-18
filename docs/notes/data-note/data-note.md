<!-- DATA NOTE — DRAFT. Gate OPEN (Derek's word, 2026-07-17). Deliverable of this block:
     §1 Introduction + §2 Methods drafted in full; §3–§6 are stubs marked [PENDING]; STOP for
     section-by-section review (rule R4) before drafting §3 onward.

     VENUE (reconciled, not silent): the base prompt said "~8pp arXiv math.NT"; the venue
     amendment (2026-07-17) SUPERSEDES the venue → REPO PUBLICATION rendered on the GitHub
     Pages site (same pipeline as docs/deck/), PDF as a build product, Markdown source (not
     LaTeX-first). KEPT unchanged: the ~8pp discipline, the §1–§6 section plan, and rules
     R1–R5. Dropped: arXiv-only packaging (subject class, abstract-form limits). See README.md.

     Numbers/quotes are from cited sources or emitted artifacts only (rules 6/8; R3). Every
     nontrivial sentence carries a <!-- claim:Nx-y --> marker with a row in CLAIMS-N.md. -->

# [TITLE — PLACEHOLDER, Derek's decision]

<!-- Working title (draft, replace): "Height-ordered elliptic-curve murmurations: a
     from-scratch replication of Sawin–Sutherland Conjecture 1 and two pre-registered
     mechanism constraints." -->

**Author:** Derek Marshall. <!-- authorship block — Derek's decision; co-authorship/affiliation/ORCID PLACEHOLDER -->

**Venue:** repo publication (GitHub Pages), PDF build product. <!-- venue block — Derek's decision -->

> **Methods disclosure (human–AI collaboration) — [DRAFT FOR DEREK'S EDIT, in your register].**
> The research design, the mathematical and methodological decisions, the pre-registration
> commitments, and every review were the author's. Implementation of the from-scratch C++,
> drafting of documentation, and execution of the analyses were carried out by an AI assistant
> (Anthropic's Claude) working under the author's direction and under the project's standing
> discipline — dual independent implementations before trust, oracles as referees only,
> commit-before-run pre-registration, no convention fitted to an expected answer, and no
> numerical constant taken from memory. The author verified the artifacts and controls this
> record; responsibility for it is the author's. *(Rewrite freely; this is a faithful factual
> draft, not a fixed form.)*

---

## Abstract *(stub — [PENDING], written last)*

<!-- One paragraph, drafted after §3–§6 land: we re-derive the Sawin–Sutherland height-ordered
     murmuration from scratch; confirm hump + first zero within the a-priori tolerance; report
     the trough as an openly-flagged, finite-range open deviation persistent across a 10⁴→2¹⁷
     ladder; and constrain two candidate mechanisms (analytic-rank split, null; root-number-
     imbalance leakage, bounded to |f|≤2% with unstable sign). No asymptotic or proof claim. -->

---

## 1. Introduction

Murmurations are an unexpected oscillating correlation between the Frobenius traces `a_p(E)` of
a family of elliptic curves and the prime `p`, discovered by He–Lee–Oliver–Pozdnyakov (2022) and
since observed across several arithmetic settings — and proved in the first of them (Zubrilina's
weight-2 density). <!-- claim:N1-1 --> A gentle account of the phenomenon and its history is in
the companion explainer (E1).

Sawin–Sutherland [SS25] study the version in which the family is ordered by **naive height**
rather than by conductor, and propose an explicit candidate for its limiting density. Two of
their statements must be held apart, because the distinction is load-bearing for what this note
does and does not claim:

- **Conjecture 1 (the density — CONJECTURAL).** For `0 < C₁ < C₂`, [SS25] conjecture that the
  `X → ∞` limit of their statistic (1) equals `∫_{C₁}^{C₂} D(u) du`, where the murmuration
  density is the Bessel-`J₁` double sum of their equation (2),

  ```
  D(u) = 2π√u · Σ_{q square-free} Σ_{m≥1}
             [ μ(gcd(m,q)) / (q · m · φ(q/gcd(m,q))) ] · J₁(4π√u · m/q)
             · ∏_{p|m, p∤q} ℓ̂_{p, 2v_p(m)} · ∏_{p|q} ℓ_{p, v_p(m)},      u = p/N,
  ```

  with `J₁` the Bessel function of the first kind, `v_p` the `p`-adic valuation, `μ` Möbius,
  `φ` Euler, and the local factors `ℓ_{p,ν}`, `ℓ̂_{p,ν}` given verbatim in their Lemmas 3–4. <!-- claim:N1-2 -->
- **Theorem 2 (the proven variant).** [SS25] *prove* a variant in which the prime sum is
  replaced by a sum over integers with no prime factor `≤ P` against a smooth weight; the same
  integrand appears and the `P → ∞` limit is established. This smoothed / no-small-factor
  variant *motivates* Conjecture 1 but is not the prime-sum murmuration itself. <!-- claim:N1-3 -->

The explicit density of the height-ordered **prime-sum** murmuration is therefore
**conjectural**; the proven Theorem 2 is a rigorous backdrop, not the object our empirical curve
is compared against. <!-- claim:N1-4 --> This note **replicates and probes Conjecture 1** — it
proves nothing. Throughout, "verified" means *numerically, over a stated finite range,
cross-checked by two independent implementations, by a published anchor value, or by an external
oracle* — never a formal proof. <!-- claim:N1-5 -->

**What we test.** We compute statistic (1) from scratch over the height family up to
`X = 10⁴, 2¹⁶, 2¹⁷, 2¹⁸`, bin it by `u = p/N(E)`, and compare the empirical density to the
conjectured `D(u)` through shape invariants fixed in advance — the positive hump, the first zero
crossing after it, and the trough (§3). <!-- claim:N1-6 --> Where the empirical curve departs from
`D(u)`, two pre-registered follow-ups constrain candidate mechanisms (§4).

**What we do not test.** We make **no asymptotic (`X → ∞`) claim.** Our computable range (`≤ 2¹⁸`)
sits at the *very bottom* of the window over which [SS25] observe the bias and expect it to decay
(naive height `2¹⁶–2²⁸`, reached only via sub-linear point counting); every "persistent" statement
below is a statement about our finite range, not about a limit. <!-- claim:N1-7 --> Why that distinction is not pedantic — a documented precedent
in which a small-conductor picture does not survive to the asymptotic one — is set out with the
results and the discussion (§3, §6).

---

## 2. Methods

**Family and ordering.** We use the short Weierstrass family `E_{A,B}: y² = x³ + Ax + B` with
integers `A, B`, **reduced** (no prime `p` with `p⁴|A` and `p⁶|B` — the unique minimal
representative of each isomorphism class) and **nonsingular** (`4A³ + 27B² ≠ 0`), ordered by the
naive height `H(E) := max(4|A|³, 27B²)`, quoted verbatim from [SS25] p.1. <!-- claim:N2-1 --> The
family `{E_{A,B} : H(E) ≤ X}` is enumerated directly over the box `|A| ≤ (X/4)^{1/3}`,
`|B| ≤ (X/27)^{1/2}`; its cardinality is certified by a second, independent sieve algorithm
required to agree with the direct count at every cutoff. <!-- claim:N2-2 -->

**The two objects, side by side.** The note pits one *computed* quantity against one *conjectured*
one; the distinction (§1) is load-bearing.

*Empirical — a binned estimator of [SS25] statistic (1).* For a prime-ratio window `(C₁, C₂)`,
[SS25]'s statistic (1) is

```
E_{E: H(E)≤X} [ ( log N(E)^{(C₁+C₂)/2} / N(E) ) · Σ_{p prime, p/N(E)∈(C₁,C₂)} ε(E)·a_p(E) ].
```

We evaluate it as a **density on (0,1]**: partition into 40 bins of width `Δu = 0.025`, take each
bin `b = (bΔu, (b+1)Δu]` as the window (so its midpoint `(C₁+C₂)/2 = u_mid := (b+½)·Δu`), and
divide by `Δu`:

```
D̂(u_b) = (1 / (Δu · |fam|)) · Σ_{(E,p): p/N(E)∈bin_b} (u_mid · ln N(E) / N(E)) · ε(E) · a_p(E),
```

over good primes `p > 3` with `p ∤ (4A³ + 27B²)` and `p ≤ N(E)` (so `u ≤ 1`). The `u_mid` factor is
**exactly eq (1)'s `(C₁+C₂)/2` normalization exponent at the bin midpoint** — not a per-pair weight
and not ad hoc — so `D̂` is a faithful **binned density estimator** of statistic (1), one bin per
window. <!-- claim:N2-3 --> For this family the reduced short model is `p`-minimal at every `p > 3`,
so `p ∤ Δ(model) ⟺ p ∤ N(E)` (good reduction), and this exact good-prime test keeps `a_p`'s domain
computed-only. <!-- claim:N2-4 -->

*Conjectured — [SS25] eq. (2) / Conjecture 1.* [SS25] conjecture that the `X→∞` limit of (1)
equals `∫ D(u) du`, where `D(u)` is the Bessel-`J₁` density transcribed verbatim in §1. **eq (1)
is a definition we evaluate; eq (2)'s equality to the limit is the CONJECTURE.** `D̂` is compared
to `D`; the note reports empirical agreement, never a proof (§1). <!-- claim:N1-2 -->

**Provenance — computed vs. oracle.** The Frobenius trace `a_p = p + 1 − #E(𝔽_p)` is **computed
from scratch**; the conductor `N(E)` and root number `ε(E)` are **oracle-provenance input**
supplied by PARI/GP — trusted input *to* the statistic, not a referee *of* our numbers — and
every emitted column is labelled with its provenance. <!-- claim:N2-5 --> The oracle `N/ε` are
certified before use by a dual-oracle overlap check against LMFDB/Cremona data. <!-- claim:N2-6 -->

**Twin / anchor / oracle architecture.** Every mathematically load-bearing function has two
independent implementations (a different algorithm, never a refactor of itself) or an external
oracle check. `a_p` is computed by a fast quadratic-residue-table character sum (`ap_fast`)
twinned against a frozen modular-exponentiation referee (`ap_charsum`); the two agree exactly on
a certified sample. <!-- claim:N2-7 --> Published values are pinned as anchor tests; oracles
(PARI, LMFDB, Odlyzko) referee and are never called from the computational core; an absent
oracle skips visibly rather than passing silently. <!-- claim:N2-8 --> The full method is
described in the companion method explainer (E3). <!-- claim:N2-9 -->

<!-- x17 gate RESOLVED 2026-07-17 (twin_m0b_vs_charsum_x17 GREEN, commit fbe51a5): the sentence
     below, drafted-and-gated per R2, is now live prose. Recorded non-silently (R5). -->
In fact `a_p` is computed by **three** independent algorithms — the frozen `ap_charsum` referee,
the fast QR-table `ap_fast`, and Shanks–Mestre point counting — agreeing **exactly** over the full
2¹⁶ and 2¹⁷ `a_p` grids (112 M and 385 M values, 0 mismatches) at ~145× less CPU (O(p^{1/4}) point
counting vs. O(p) character sum); at 2¹⁸ the equality is verified on a tail-weighted 27.9 M-value
sample against the O(p) referee **and** on a PARI `ellap` spot (204 pairs, p up to 6.9 M) — the
pre-registered production-capability gate. This is a **cross-algorithm** check (Shanks–Mestre vs.
the charsum referee); `a_p` is a platform-independent integer, so it is verified same-platform by
design — not a two-platform claim. <!-- claim:N2-14 -->

**Pre-registration.** Every decision rule, tolerance, and threshold is committed to version
control *before* the data it judges exists (commit-before-run), cited here as **(pre-registered →
read)** commit pairs: **PR-1** ladder decision rule + finite-`X`/persistent fork — pre-registered
`dd6beb0`, with the 2¹⁸ Rung-3 clause `4a17ebe` committed *before the 2¹⁷ rung was read* → read
`8f64ba1` (verdict H0, ≤2¹⁸); **PR-2** analytic-rank contrast threshold pre-registered `f7415a4`
*before any split curve* → read `876999f` (null); **PR-3** leakage estimator pre-registered
`21060a0` → read `0d21b62`. <!-- claim:N2-10 --> Every exploratory statistic looked at is recorded
in a looks ledger. <!-- claim:N2-11 -->

**The conjectured density.** `D(u)` is evaluated independently of the empirical statistic — a
separate translation unit, authored without reference to the empirical side — as the Bessel-`J₁`
double sum of Conjecture 1 with the local factors of [SS25] Lemmas 3–4, an in-house `J₁`, and
*generated* (never typed) constants, truncated at `q, m ≤ 2000`. <!-- claim:N2-12 --> Because
every value is built from IEEE operations plus generated constants plus the in-house Bessel, the
emitted curve is byte-portable across compilers and platforms — a property we verify (§5). <!-- claim:N2-13 -->

---

## 3. Results

**The ladder.** Over four height cutoffs the two calibrated shape invariants — the positive hump
and the first post-hump zero — agree with the conjectured density `D(u)` within the a-priori
tolerance `τ = 0.06`, while the trough sits at `u = 0.8875` at **every** rung, a displacement
`d = |0.8875 − 0.805| = 0.0825` that is **frozen** across the ladder (all values from the committed
`data/m5/ss_x{65536,131072,262144}.txt` runs and the M4 run `data/m4/ss_empirical.txt`):

| X | \|fam\| | hump u (dev) | first zero u (dev) | trough u (dev) | trough v |
|---|---|---|---|---|---|
| 10⁴ | 1048 | 0.4625 (0.0125) | 0.672894 (0.0279) | 0.8875 (**0.0825**) | −3.47 |
| 2¹⁶ | 5042 | 0.4625 (0.0125) | 0.670328 (0.0253) | 0.8875 (**0.0825**) | −3.72 |
| 2¹⁷ | 9014 | 0.4625 (0.0125) | 0.673202 (0.0282) | 0.8875 (**0.0825**) | −3.65 |
| 2¹⁸ | 15936 | 0.4625 (0.0125) | 0.671945 (0.0269) | 0.8875 (**0.0825**) | −3.58 |

Hump dev 0.0125 and first-zero dev ≤ 0.0282 are both **within `τ = 0.06`** (agreement with `D(u)`);
the trough dev 0.0825 **exceeds `τ`** at every rung — a persistent open deviation. <!-- claim:N3-1 --><!-- claim:N3-2 -->
(Fig. 1, the empirical-vs-`D(u)` overlay, is regenerated from these committed runs by
`figures/make_figures.py`.)

**Verdict — H0 (persistent, ≤ 2¹⁸).** The decision rule was pre-registered before the data (PR-1,
`dd6beb0`; the 2¹⁸ Rung-3 clause `4a17ebe` committed *before the 2¹⁷ rung was read*). With
`d(X) = |trough_u − 0.805|` and `Δu = 0.025`, the Rung-3 clause reads, **verbatim**:

> **H1 (finite-X, ≤2¹⁸)** iff the trough recovers monotonically by ≥ Δu at **both** steps:
> d(2¹⁷) ≤ d(2¹⁶) − Δu **and** d(2¹⁸) ≤ d(2¹⁷) − Δu … **H0 (persistent, ≤2¹⁸)** iff the trough
> stays flat across the ladder (no ≥ Δu recovery at either step).

Observed `d = 0.0825` at 2¹⁶, 2¹⁷, and 2¹⁸ — flat, no recovery at either step ⇒ **H0 (persistent,
≤ 2¹⁸)**, read at `8f64ba1`. <!-- claim:N3-3 --> This is the first rung at which the full-ladder
verdict PR-1's R0 clause reserved could be pronounced. It is a **finite-range** statement (≤ 2¹⁸),
explicitly **not** the `X→∞` verdict [SS25] describe — 2¹⁸ is the very bottom of their 2¹⁶–2²⁸
decay window.

**Supporting register (NOT the gate).** (i) The first-zero series `0.672894 → 0.670328 → 0.673202
→ 0.671945` is flat near ~0.671, non-monotone, converging to neither the 0.645 target nor away —
direction only. <!-- claim:N3-4 --> (ii) The trough **depth** eases monotonically across the three
extension rungs `−3.715 → −3.652 → −3.580` even as its **position** holds at 0.8875 — a hint of
amplitude decay consistent with the expected large-X behaviour, but the committed rule gates on
**position**, so it does not move the verdict; logged as an observation, not a claim. <!-- claim:N3-5 -->

## 4. Mechanism constraints

Two pre-registered follow-ups constrain candidate mechanisms for the trough deviation. Both are
bounds/nulls; neither explains it.

**PR-2 — analytic-rank split (null).** Pre-registered `f7415a4` (the contrast threshold fixed
before any curve was split) → read `876999f`. Excising the analytic-rank-2 subpopulation leaves
the trough **position** unchanged — the pre-committed recovery gate (leave-out trough moves < 1
bin, on the full family and on a virgin conductor annulus) is not met — so **the deficit is NOT
carried by rank-2 over-representation.** A secondary contrast (rank-2 curves carry an enhanced
downward bias) is significant but too dilute to move the location gate; the two gate outcomes are
coherent. <!-- claim:N4-1 --> *Wachs clause (verbatim):* this design does not distinguish rank per
se from BSD invariants (Tamagawa product, \|Ш\|, real period) correlated with rank; "carried by"
is a statement about the subpopulation, not the mechanism. <!-- claim:N4-4 -->

**PR-3 — root-number-imbalance leakage (bounded out).** Pre-registered `21060a0` → read `0d21b62`
(the 2¹⁸ row appended with the Rung-3 run, `8f64ba1`). With `δ = (n₊−n₋)/|fam|` the root-number
imbalance, `U` the unsigned bias, leakage `L = δ·U`, and `f = L / departure` at the trough:

| X | δ | leakage L(u\*) | f = L/departure | sign vs. deficit |
|---|---|---|---|---|
| 10⁴ | −0.0534 | −0.0184 | +0.0046 | same |
| 2¹⁶ | −0.0115 | +0.0037 | −0.0009 | opposite |
| 2¹⁷ | −0.0078 | +0.0025 | −0.0006 | opposite |
| 2¹⁸ | −0.0103 | +0.0032 | −0.0008 | opposite |

**`|f| ≤ 2%` at every rung, opposite-signed (points *against* the deficit) at the three reliable
rungs, and it does not grow with X** — leakage of Sutherland's parity-independent bias through the
measured imbalance is bounded out as a material contributor (`f ≈ 1` would require the unsigned
bias ~3 orders larger than measured). A citable null; a bound, not an explanation. <!-- claim:N4-2 -->
*Sutherland-bias attribution (verbatim):* the parity-independent unsigned bias, the named
congruence classes, and its cancellation in the signed average are Sutherland's (Sept-2023 talk);
PR-3 tests only the leakage *relationship*, which is PR-3's hypothesis, not Sutherland's claim. <!-- claim:N4-5 -->

**D1 (congruence stratification) — DIED, unrun.** Per PR-3's pre-committed trigger, D2 bounding
the mechanism out (small `f`, wrong sign, no structured residual at the named classes) closes the
prime-class mechanism *without* the expensive stratified pass; D1's own pre-registration was never
written (hypotheses-die-in-public). <!-- claim:N4-3 -->

**The tail deficit remains unexplained by either constrained mechanism** — stated as such, not
smoothed. What could carry it is future work (§6).

## 5. Reproducibility *(stub — [PENDING])*

<!-- Cross-platform byte-identity (macOS referee vs FreeBSD/clang independent re-run); the libm
     partial-diff paragraph [PENDING item-4 diff]; the errata ledger; byte-portable emit +
     freshness check.
     HOW TO CITE / HOW TO VERIFY block (venue amendment): repository URL, release tag
     [PLACEHOLDER], HEAD commit, and a one-command reproduction pointer (cmake build + ctest +
     `at emit` + this note's figure script). Zenodo DOI [PLACEHOLDER — minted after Derek's full
     review, per Derek 2026-07-17]. -->

## 6. Discussion *(stub — [PENDING])*

<!-- What could carry the trough displacement: Wachs-adjacent BSD invariants (Tamagawa product,
     |Ш|, real period) correlated with rank — named as future work (PR-4), not claimed. The
     scale caveat with its documented precedent (Sutherland Sept-2023 slide 7: the rank-stratified
     Mestre–Nagao/BSD average attains its asymptotic ordering only at very large conductor, so a
     settled-looking small-conductor picture need not be the asymptotic one). Explicit restatement
     of the finite-range register: persistent OVER OUR RANGE, no X→∞ claim. -->

---

<!-- Deliverable boundary: §1 + §2 are drafted; §3–§6 are stubs. STOP for review (R4). -->
