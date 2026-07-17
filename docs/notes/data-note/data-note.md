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
since established in several arithmetic settings. <!-- claim:N1-1 --> A gentle account of the
phenomenon and its history is in the companion explainer (E1).

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
`X = 10⁴, 2¹⁶, 2¹⁷`, bin it by `u = p/N(E)`, and compare the empirical density to the conjectured
`D(u)` through shape invariants fixed in advance — the positive hump, the first zero crossing
after it, and the trough (§3). <!-- claim:N1-6 --> Where the empirical curve departs from `D(u)`,
two pre-registered follow-ups constrain candidate mechanisms (§4).

**What we do not test.** We make **no asymptotic (`X → ∞`) claim.** Our computable range sits at
the *very bottom* of the window over which [SS25] observe the bias and expect it to decay (naive
height `2¹⁶–2²⁸`); every "persistent" statement below is a statement about our finite range, not
about a limit. <!-- claim:N1-7 --> Why that distinction is not pedantic — a documented precedent
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

**The statistic.** Binning by `u = p/N(E)` on `(0,1]` at width `Δu = 0.025` (40 bins), the
empirical density is [SS25]'s statistic (1),

```
D̂(u_b) = (1 / (Δu · |fam|)) · Σ_{(E,p): p/N(E) ∈ bin_b} (u_mid · ln N(E) / N(E)) · ε(E) · a_p(E),
```

summed over good primes `p > 3` with `p ∤ (4A³ + 27B²)` and `p ≤ N(E)` (so `u ≤ 1`). <!-- claim:N2-3 -->
For this family the reduced short model is `p`-minimal at every `p > 3`, so `p ∤ Δ(model) ⟺ p ∤ N(E)`
(good reduction) — the model discriminant is the exact good-prime test, keeping `a_p`'s domain
computed-only. <!-- claim:N2-4 -->

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

<!-- GATED sentence (R2) — appears ONLY if the 2¹⁷ full twin is green at publication time;
     [PENDING x17]. Drafted and gated here, same pattern as the E3 amendment:
     "In fact `a_p` is computed by three independent algorithms — the frozen `ap_charsum`
     referee, the fast QR-table `ap_fast`, and Shanks–Mestre point counting — agreeing exactly
     over the full 2¹⁶ `a_p` grid (112 M values) at roughly 145× less CPU (O(p^{1/4}) point
     counting vs. O(p) character sum); the cross-platform 2¹⁷ extension is the completeness
     gate." (source: E3-2b; m0b-pinning.md §7; test twin_m0b_vs_charsum_x16, and x17 [PENDING]) -->

**Pre-registration.** Every decision rule, tolerance, and threshold is committed to version
control *before* the data it judges exists (commit-before-run). Three pre-registrations govern
this note: **PR-1** fixed the ladder decision rule and its finite-`X`-vs-persistent interpretation
fork before the larger scales were read (commit `fa4e35a`); **PR-2** fixed the analytic-rank
contrast threshold before the split was computed (commit `100a13f`); **PR-3** pinned the
root-number-imbalance leakage estimator before any of its numbers were computed (pre-registration
`21060a0`, measurement `0d21b62`). <!-- claim:N2-10 --> Every exploratory statistic looked at is
recorded in a looks ledger. <!-- claim:N2-11 -->

**The conjectured density.** `D(u)` is evaluated independently of the empirical statistic — a
separate translation unit, authored without reference to the empirical side — as the Bessel-`J₁`
double sum of Conjecture 1 with the local factors of [SS25] Lemmas 3–4, an in-house `J₁`, and
*generated* (never typed) constants, truncated at `q, m ≤ 2000`. <!-- claim:N2-12 --> Because
every value is built from IEEE operations plus generated constants plus the in-house Bessel, the
emitted curve is byte-portable across compilers and platforms — a property we verify (§5). <!-- claim:N2-13 -->

---

## 3. Results *(stub — [PENDING], drafted after §1+§2 review)*

<!-- Section plan (do not draft yet): the ladder table (X = 10⁴ / 2¹⁶ / 2¹⁷, trough displacement
     dev 0.0825 FROZEN across all three rungs); hump + first-zero agreement within the a-priori
     tolerance τ = 0.06; the four-curve overlay figure (Fig 1, from figures/make_figures.py);
     the sub-bin zero-crossing series reported in the SUPPORTING register only. Numbers from the
     committed ss_x*.txt runs and the emitted JSON only (R3). -->

## 4. Mechanism constraints *(stub — [PENDING])*

<!-- PR-2 null (analytic-rank-2 over-representation does NOT carry the trough displacement) with
     its pre-committed recovery gate quoted; PR-3 bound (root-number-imbalance leakage: |f| ≤ 2%,
     sign-unstable, ~3 orders short of explaining the deficit) with its §0d envelope quoted; D1
     (congruence stratification) recorded DIED-UNRUN per its pre-committed trigger. Each carries
     the Wachs confound clause and the Sutherland-bias attribution verbatim. -->

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
