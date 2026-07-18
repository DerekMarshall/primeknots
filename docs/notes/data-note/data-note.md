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

# A pre-registered replication of the Sawin–Sutherland height-ordered murmuration

**Author:** Derek Marshall (ORCID [0009-0001-8101-3908](https://orcid.org/0009-0001-8101-3908)). <!-- authorship block — Derek's decision; co-authorship/affiliation PLACEHOLDER; title still [PLACEHOLDER] -->

**Venue:** repo publication (GitHub Pages), PDF build product. <!-- venue block — Derek's decision -->

> **Methods disclosure.** The research design, the mathematical decisions, the pre-registrations,
> and all review are the author's. The C++ implementation, the documentation drafts, and the
> execution of the analyses were done by an AI assistant (Anthropic's Claude) under the author's
> direction, following the repository's working rules: dual independent implementations, oracles
> as referees only, decision rules committed before the data they judge, no constant taken from
> memory. The author verified the artifacts and is responsible for this record.

---

## Abstract

We re-derive the Sawin–Sutherland height-ordered elliptic-curve murmuration from scratch and probe
it against their conjectured Bessel-`J₁` density. Over a four-rung height ladder,
`X = 10⁴, 2¹⁶, 2¹⁷, 2¹⁸`, two calibrated shape invariants (the positive hump and the first
post-hump zero) agree with the conjectured density within an a-priori tolerance, while the trough
sits at a fixed displacement that exceeds the tolerance at every rung. A pre-registered decision
rule returns H0: the trough deviation is persistent across the ladder, a finite-range statement
over `X ≤ 2¹⁸` and not a claim about the large-`X` limit. Two pre-registered follow-ups constrain
candidate mechanisms: excising the analytic-rank-2 subpopulation does not move the trough, so the
deficit is not carried by rank-2 over-representation, and leakage of the parity-independent
unsigned bias through the measured root-number imbalance is bounded to `|f| ≤ 2%` with unstable
sign. Neither explains the deficit, which we leave open. Every result is numerical over a stated
finite range, twinned or oracle-refereed, and cross-checked for cross-platform reproducibility; the
note proves nothing.

---

## 1. Introduction

Murmurations are an oscillating correlation between the Frobenius traces `a_p(E)` of a family of
elliptic curves and the prime `p`. He–Lee–Oliver–Pozdnyakov discovered them in 2022; the
phenomenon has since been observed in several arithmetic settings, and proved in the first of them,
Zubrilina's weight-2 density. <!-- claim:N1-1 --> The companion explainer (E1) gives a fuller
account.

Sawin–Sutherland [SS25] order the family by naive height rather than by conductor and propose an
explicit candidate for its limiting density. Two of their statements must be kept apart; the
distinction fixes what this note claims. The first is a conjecture, the second a theorem.

- **Conjecture 1 (the density).** For `0 < C₁ < C₂`, [SS25] conjecture that the
  `X → ∞` limit of their statistic (1) equals `∫_{C₁}^{C₂} D(u) du`, where the murmuration
  density is the Bessel-`J₁` double sum of their equation (2):

  ```
  D(u) = 2π√u · Σ_{q square-free} Σ_{m≥1}
             [ μ(gcd(m,q)) / (q · m · φ(q/gcd(m,q))) ] · J₁(4π√u · m/q)
             · ∏_{p|m, p∤q} ℓ̂_{p, 2v_p(m)} · ∏_{p|q} ℓ_{p, v_p(m)},      u = p/N,
  ```

  Here `J₁` is the Bessel function of the first kind, `v_p` the `p`-adic valuation, `μ` the Möbius
  function, and `φ` Euler's totient; the local factors `ℓ_{p,ν}` and `ℓ̂_{p,ν}` are those of their
  Lemmas 3 and 4. <!-- claim:N1-2 -->
- **Theorem 2 (the proven variant).** [SS25] prove a variant in which the prime sum is replaced by
  a sum over integers with no prime factor `≤ P`, against a smooth weight; the same integrand
  appears, and the `P → ∞` limit is established. This smoothed variant motivates Conjecture 1
  without being the prime-sum murmuration. <!-- claim:N1-3 -->

The explicit density of the height-ordered prime-sum murmuration is therefore conjectural. The
proven Theorem 2 is a rigorous backdrop, not the object our empirical curve is compared against. <!-- claim:N1-4 -->
This note replicates and probes Conjecture 1; it proves nothing. Throughout, "verified" means
numerically, over a stated finite range, cross-checked by two independent implementations, by a
published anchor value, or by an external oracle. It never means a formal proof. <!-- claim:N1-5 -->

**What this note tests.** We compute statistic (1) from scratch over the height family up to
`X = 10⁴, 2¹⁶, 2¹⁷, 2¹⁸`, bin it by `u = p/N(E)`, and compare the empirical density to the
conjectured `D(u)` through three shape invariants fixed in advance: the positive hump, the first
zero crossing after it, and the trough (§3). <!-- claim:N1-6 --> Where the empirical curve departs
from `D(u)`, two pre-registered follow-ups constrain candidate mechanisms (§4).

**What this note does not test.** It makes no asymptotic (`X → ∞`) claim. Our computable range,
`X ≤ 2¹⁸`, sits at the bottom of the window over which [SS25] observe the bias and expect it to
decay, naive height `2¹⁶` to `2²⁸`, reached only through sub-linear point counting. Every
"persistent" statement below concerns that finite range, not a limit. <!-- claim:N1-7 --> The
discussion (§6) gives a documented precedent in which a small-conductor picture does not survive
to the asymptotic one; that is why the distinction is not pedantic.

---

## 2. Methods

**Family and ordering.** We use the short Weierstrass family `E_{A,B}: y² = x³ + Ax + B` with
integers `A, B`, *reduced* (no prime `p` with `p⁴|A` and `p⁶|B`, the unique minimal
representative of each isomorphism class) and *nonsingular* (`4A³ + 27B² ≠ 0`), ordered by the
naive height `H(E) := max(4|A|³, 27B²)`, quoted verbatim from [SS25] p.1. <!-- claim:N2-1 --> The
family `{E_{A,B} : H(E) ≤ X}` is enumerated directly over the box `|A| ≤ (X/4)^{1/3}`,
`|B| ≤ (X/27)^{1/2}`; its cardinality is certified by a second, independent sieve algorithm
required to agree with the direct count at every cutoff. <!-- claim:N2-2 -->

**The two objects, side by side.** The note compares one *computed* quantity with one
*conjectured* one. The distinction (§1) governs what the note claims.

*Empirical: a binned estimator of [SS25] statistic (1).* For a prime-ratio window `(C₁, C₂)`,
[SS25]'s statistic (1) is

```
E_{E: H(E)≤X} [ ( log N(E)^{(C₁+C₂)/2} / N(E) ) · Σ_{p prime, p/N(E)∈(C₁,C₂)} ε(E)·a_p(E) ].
```

We evaluate it as a density on (0,1]: partition into 40 bins of width `Δu = 0.025`, take each
bin `b = (bΔu, (b+1)Δu]` as the window (so its midpoint `(C₁+C₂)/2 = u_mid := (b+½)·Δu`), and
divide by `Δu`:

```
D̂(u_b) = (1 / (Δu · |fam|)) · Σ_{(E,p): p/N(E)∈bin_b} (u_mid · ln N(E) / N(E)) · ε(E) · a_p(E),
```

over good primes `p > 3` with `p ∤ (4A³ + 27B²)` and `p ≤ N(E)` (so `u ≤ 1`). The `u_mid` factor is
eq (1)'s `(C₁+C₂)/2` normalization exponent evaluated at the bin midpoint, not a per-pair weight,
so `D̂` is a binned density estimator of statistic (1), one bin per
window. <!-- claim:N2-3 --> For this family the reduced short model is `p`-minimal at every `p > 3`,
so `p ∤ Δ(model) ⟺ p ∤ N(E)` (good reduction), and this exact good-prime test keeps `a_p`'s domain
computed-only. <!-- claim:N2-4 -->

*Conjectured: [SS25] eq. (2) / Conjecture 1.* [SS25] conjecture that the `X→∞` limit of (1)
equals `∫ D(u) du`, where `D(u)` is the Bessel-`J₁` density transcribed verbatim in §1. eq (1)
is a definition we evaluate; eq (2)'s equality to the limit is the *conjecture*. `D̂` is compared
to `D`; the note reports empirical agreement, never a proof (§1). <!-- claim:N2-15 -->

**Provenance: computed vs. oracle.** The Frobenius trace `a_p = p + 1 − #E(𝔽_p)` is computed
from scratch. The conductor `N(E)` and root number `ε(E)` are oracle-provenance input
supplied by PARI/GP, trusted input *to* the statistic and not a referee *of* our numbers. Every
emitted column is labelled with its provenance. <!-- claim:N2-5 --> The oracle `N/ε` are
certified before use by a dual-oracle overlap check against LMFDB/Cremona data. <!-- claim:N2-6 -->

**Twin / anchor / oracle architecture.** Each function the result depends on has two
independent implementations (a different algorithm, never a refactor of itself) or an external
oracle check. `a_p` is computed by a fast quadratic-residue-table character sum (`ap_fast`)
twinned against a frozen modular-exponentiation referee (`ap_charsum`); the two agree exactly on
a certified sample. <!-- claim:N2-7 --> Published values are pinned as anchor tests; oracles
(PARI, LMFDB, Odlyzko) referee and are never called from the computational core; an absent
oracle skips visibly rather than passing silently. <!-- claim:N2-8 --> The full method is
described in the companion method explainer (E3). <!-- claim:N2-9 -->

<!-- x17 gate RESOLVED 2026-07-17 (twin_m0b_vs_charsum_x17 GREEN, commit fbe51a5): the sentence
     below, drafted-and-gated per R2, is now live prose. Recorded non-silently (R5). -->
`a_p` is computed by three independent algorithms: the frozen `ap_charsum` referee,
the fast QR-table `ap_fast`, and Shanks–Mestre point counting. They agree exactly over the full
2¹⁶ and 2¹⁷ `a_p` grids (112 M and 385 M values, 0 mismatches), at about 145× less CPU aggregated
over the 2¹⁶ grid (O(p^{1/4}) point counting vs. O(p) character sum). At 2¹⁸ the equality is
verified on a tail-weighted 27.9 M-value sample against the O(p) referee and on a PARI `ellap`
spot (204 pairs, p up to 6.9 M), the pre-registered production-capability gate. This is a
cross-algorithm check (Shanks–Mestre vs. the charsum referee). Since `a_p` is a
platform-independent integer, it is verified same-platform by design, not as a two-platform
claim. <!-- claim:N2-14 -->

**Pre-registration.** Every decision rule, tolerance, and threshold is committed to version
control *before* the data it judges exists (commit-before-run), cited here as (pre-registered →
read) commit pairs. PR-1, the ladder decision rule and finite-`X`/persistent fork: pre-registered
`dd6beb0`, with the 2¹⁸ Rung-3 clause `4a17ebe` committed *before the 2¹⁷ rung was read*, read
`8f64ba1` (verdict H0, ≤2¹⁸). PR-2, the analytic-rank contrast threshold: pre-registered `f7415a4`
*before any split curve*, read `876999f` (null). PR-3, the leakage estimator: pre-registered
`21060a0`, read `0d21b62`. <!-- claim:N2-10 --> Every exploratory statistic looked at is recorded
in a looks ledger. <!-- claim:N2-11 -->

**The conjectured density.** `D(u)` is evaluated independently of the empirical statistic (a
separate translation unit, authored without reference to the empirical side) as the Bessel-`J₁`
double sum of Conjecture 1 with the local factors of [SS25] Lemmas 3–4, an in-house `J₁`, and
*generated* constants (never typed), truncated at `q, m ≤ 2000`. <!-- claim:N2-12 --> Because
every value is built from IEEE operations, generated constants, and the in-house Bessel, the
emitted curve is byte-portable across compilers and platforms, a property we verify (§5). <!-- claim:N2-13 -->

---

## 3. Results

**The ladder.** Over four height cutoffs the two calibrated shape invariants (the positive hump
and the first post-hump zero) agree with the conjectured density `D(u)` within the a-priori
tolerance `τ = 0.06`, while the trough sits at `u = 0.8875` at every rung, a displacement
`d = |0.8875 − 0.805| = 0.0825` that is frozen across the ladder. All values are from the committed
`data/m5/ss_x{65536,131072,262144}.txt` runs and the M4 run `data/m4/ss_empirical.txt`:

| X | \|fam\| | hump u (dev) | first zero u (dev) | trough u (dev) | trough v |
|---|---|---|---|---|---|
| 10⁴ | 1048 | 0.4625 (0.0125) | 0.672894 (0.0279) | 0.8875 (0.0825) | −3.47 |
| 2¹⁶ | 5042 | 0.4625 (0.0125) | 0.670328 (0.0253) | 0.8875 (0.0825) | −3.72 |
| 2¹⁷ | 9014 | 0.4625 (0.0125) | 0.673202 (0.0282) | 0.8875 (0.0825) | −3.65 |
| 2¹⁸ | 15936 | 0.4625 (0.0125) | 0.671945 (0.0269) | 0.8875 (0.0825) | −3.58 |

Hump dev 0.0125 and first-zero dev ≤ 0.0282 are both within `τ = 0.06` (agreement with `D(u)`).
The trough dev 0.0825 exceeds `τ` at every rung, a persistent open deviation. <!-- claim:N3-1 --><!-- claim:N3-2 -->
(Fig. 1, the empirical-vs-`D(u)` overlay, is regenerated from these committed runs by
`figures/make_figures.py`.)

**Verdict: H0 (persistent, ≤ 2¹⁸).** The decision rule was pre-registered before the data (PR-1,
`dd6beb0`; the 2¹⁸ Rung-3 clause `4a17ebe` committed *before the 2¹⁷ rung was read*). With
`d(X) = |trough_u − 0.805|` and `Δu = 0.025`, the Rung-3 clause reads, verbatim:

> **H1 (finite-X, ≤2¹⁸)** iff the trough recovers monotonically by ≥ Δu at **both** steps:
> d(2¹⁷) ≤ d(2¹⁶) − Δu **and** d(2¹⁸) ≤ d(2¹⁷) − Δu … **H0 (persistent, ≤2¹⁸)** iff the trough
> stays flat across the ladder (no ≥ Δu recovery at either step).

Observed `d = 0.0825` at 2¹⁶, 2¹⁷, and 2¹⁸: flat, with no recovery at either step, hence H0
(persistent, ≤ 2¹⁸), read at `8f64ba1`. <!-- claim:N3-3 --> This is the first rung at which the
full-ladder verdict PR-1's R0 clause reserved could be pronounced. It is a finite-range statement
(≤ 2¹⁸), explicitly not the `X→∞` verdict [SS25] describe. 2¹⁸ is the bottom of their 2¹⁶–2²⁸
decay window.

**Supporting register (not the gate).** (i) The first-zero series `0.672894 → 0.670328 → 0.673202
→ 0.671945` is flat near about 0.671, non-monotone, converging to neither the 0.645 target nor
away, direction only. <!-- claim:N3-4 --> (ii) The trough *depth* eases monotonically across the
three extension rungs `−3.715 → −3.652 → −3.580` even as its *position* holds at 0.8875. The 10⁴
depth (−3.47) is shallower than 2¹⁶, so this easing is a three-rung observation, not a monotone
four-rung trend. It is a hint of amplitude decay consistent with the expected large-X behaviour,
but the committed rule gates on position, so it does not move the verdict; logged as an
observation, not a claim. <!-- claim:N3-5 -->

## 4. Mechanism constraints

Two pre-registered follow-ups constrain candidate mechanisms for the trough deviation. Both are
bounds/nulls; neither explains it.

**PR-2: analytic-rank split (null).** Pre-registered `f7415a4` (the contrast threshold fixed
before any curve was split) → read `876999f`. Excising the analytic-rank-2 subpopulation leaves
the trough position unchanged. The pre-committed recovery gate (leave-out trough moves < 1
bin, on the full family and on a virgin conductor annulus) is not met, so the deficit is not
carried by rank-2 over-representation. A secondary contrast (rank-2 curves carry an enhanced
downward bias) is significant but too dilute to move the location gate; the two gate outcomes are
coherent. <!-- claim:N4-1 --> *Wachs clause (verbatim):* this design does not distinguish rank per
se from BSD invariants (Tamagawa product, \|Ш\|, real period) correlated with rank; "carried by"
is a statement about the subpopulation, not the mechanism. <!-- claim:N4-4 -->

**PR-3: root-number-imbalance leakage (bounded out).** Pre-registered `21060a0` → read `0d21b62`
(the 2¹⁸ row appended with the Rung-3 run, `8f64ba1`). With `δ = (n₊−n₋)/|fam|` the root-number
imbalance, `U` the unsigned bias, leakage `L = δ·U`, and `f = L / departure` at the trough:

| X | δ | leakage L(u\*) | f = L/departure | sign vs. deficit |
|---|---|---|---|---|
| 10⁴ | −0.0534 | −0.0184 | +0.0046 | same |
| 2¹⁶ | −0.0115 | +0.0037 | −0.0009 | opposite |
| 2¹⁷ | −0.0078 | +0.0025 | −0.0006 | opposite |
| 2¹⁸ | −0.0103 | +0.0032 | −0.0008 | opposite |

`|f| ≤ 2%` at every rung, it is opposite-signed (points *against* the deficit) at the three
reliable rungs, and it does not grow with X. Leakage of Sutherland's parity-independent bias
through the measured imbalance is bounded out as a material contributor (`f ≈ 1` would require the
unsigned bias about 3 orders larger than measured). A citable null; a bound, not an
explanation. <!-- claim:N4-2 -->
*Sutherland-bias attribution (verbatim):* the parity-independent unsigned bias, the named
congruence classes, and its cancellation in the signed average are Sutherland's (Sept-2023 talk);
PR-3 tests only the leakage *relationship*, which is PR-3's hypothesis, not Sutherland's claim. <!-- claim:N4-5 -->

**D1 (congruence stratification): died, unrun.** Per PR-3's pre-committed trigger, D2 bounding
the mechanism out (small `f`, wrong sign, no structured residual at the named classes) closes the
prime-class mechanism without the expensive stratified pass. D1's own pre-registration was never
written (hypotheses die in public). <!-- claim:N4-3 -->

The tail deficit remains unexplained by either constrained mechanism, stated as such and not
smoothed. What could carry it is future work (§6).

## 5. Reproducibility

**A byte-identical published curve.** The emitted density JSON is byte-for-byte identical across
compilers and platforms. The density evaluator and its emitter (`src/murm/ss_density.cpp`,
`src/emit/emit_sawin_sutherland.cpp`) are compiled with fused-multiply-add contraction disabled
(`-ffp-contract=off`), so their IEEE operations round the same way under GCC and clang. A freshness
check re-emits the snapshot at the current commit and requires byte-equality with the committed
file, on CI's GCC as well as a local build. <!-- claim:N5-1 -->

**Integer `a_p` is platform-independent.** A pre-registered, tail-weighted sample of 79,268
(curve, prime) pairs, with primes up to about 4.15 million, was computed by the frozen referee
`ap_charsum` on the laptop (`g++-16`, macOS) and on the compute box (`clang 21.1.8`, FreeBSD 15.1).
The two outputs are byte-identical, with zero mismatches. This sample corroborates on the 2¹⁷
family; the primary evidence that `a_p` is platform-independent is the exact agreement of the three
`a_p` algorithms over the full 2¹⁶ and 2¹⁷ grids together with the byte-identical emit above. <!-- claim:N5-2 -->

**The raw partials are the deliberate exception (the contrast).** Contraction is off for the
density path but left at the compiler default for the empirical partial-sum generator
(`src/murm/ss_empirical.cpp` is outside the `-ffp-contract=off` list). So the raw per-curve partial
sums drift across toolchains while the emitted density does not. Measured on the shared 8640-curve
2¹⁷ set, the drift has maximum size `1.887 × 10⁻¹⁵`, the last bit of a value near one. It does not
reach the published curve, and byte-identity of the emitted density is established empirically by
the freshness check above, not by rounding alone. The precision gap explains it: the emitter writes
each value to twelve significant figures (`std::setprecision(12)`, `src/emit/emit_sawin_sutherland.cpp:28`),
roughly three orders of magnitude coarser than the drift at the density's order-one scale, so a
difference this small falls below the last written figure except at a rounding boundary, which the
freshness check would catch. <!-- claim:N5-3 -->

**Errata and inputs.** Corrections are logged as numbered errata entries in the research spec and
the pinning notes, not edited away. Everything the note's numbers depend on is committed, including
the from-scratch code, the oracle `N`/`ε` caches, the per-curve partials, and the `at ap-sample`
subcommand. The one input that cannot be regenerated byte-for-byte is the abandoned laptop 2¹⁷
partials checkpoint, since it is itself the float-drift artifact; it is force-committed through a
scoped `.gitignore` exception (`data/m5/ss_partials_x131072.txt.ckpt`), not left to regeneration.
The large `a_p` caches (regenerable by `at ap-cache`) and the cross-platform sample outputs
(regenerable by `at ap-sample`) are gitignored and pinned by SHA-256 in the reproducibility spec
(`docs/notes/libm-partial-diff-spec.md`). <!-- claim:N5-4 -->

**How to cite and verify.** *(Release tag and DOI are placeholders, minted after the author's full
review.)* Repository: <https://github.com/DerekMarshall/primeknots>, release tag
`[PLACEHOLDER: v1.0.0]`, at its tagged commit. Zenodo DOI `[PLACEHOLDER]`. To reproduce the results
this note reports, from the tagged commit:

```
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
ctest --test-dir build -L m5                          # the M5 verification suites
./build/bin/at emit --stage m5      --out viz/data/   # empirical-vs-D(u) overlay JSON
./build/bin/at emit --stage m5split --out viz/data/   # rank-split overlay JSON
python3 docs/notes/data-note/figures/make_figures.py  # Fig. 1 and the rank-split figure
```

The cross-platform `a_p` sample is reproduced by
`./build/bin/at ap-sample --X 131072 --cache data/m5/ne_cache_x131072.txt` on two toolchains; its
output SHA-256 is pinned in the reproducibility spec. <!-- claim:N5-5 -->

## 6. Discussion

**What could carry the tail deficit.** Neither constrained mechanism (§4) explains the trough
deficit; both are bounds or nulls. A candidate the design has not isolated is the family of BSD
invariants correlated with analytic rank (the Tamagawa product, the analytic order of Ш, and the
real period), which Wachs shows modulate murmuration profiles at fixed rank. Separating those
invariants from rank is a pre-registered follow-up (PR-4, a split by BSD invariant at fixed rank),
named here as future work and not claimed. <!-- claim:N6-1 -->

**A scale caveat with a documented precedent.** Our computable range, `X ≤ 2¹⁸`, is small and
finite. Sutherland's September 2023 talk gives a precedent for reading such a range cautiously. One
formulation of BSD makes the Mestre–Nagao sum
`lim_{x→∞} (1/log x) Σ_{p≤x} a_p(E) log p / p = −r + 1/2` (conditional: Kim–Murty 2023 show that if
this limit exists, it equals that value and the Riemann hypothesis holds for `L(E,s)`), and
curves of large rank necessarily have large conductor. In Sutherland's rank-stratified plot of that
sum, the ordering by rank emerges only at very large conductor. A picture that looks settled at the
conductors we can reach need not be the asymptotic one. <!-- claim:N6-2 -->

**A gate for going further.** Across the extension rungs the trough depth eases while its position
holds (§3). Whether that easing is the amplitude decay the large-`X` picture predicts is not
something our position-gated rule tests. A pre-registered amplitude-trajectory gate (PR-5), a
decision rule on the trough's depth trajectory fixed before the data, is the condition under which
rungs beyond 2¹⁸ would be added. It is named here, not run. <!-- claim:N6-3 -->

**Register, restated once.** The verdict of §3 is H0 over the finite range `X ≤ 2¹⁸`: the trough
displacement is persistent there. It is not a claim about the `X → ∞` limit [SS25] describe, and
nothing in this note bears on that limit. <!-- claim:N6-4 -->

---

<!-- Deliverable boundary: §1 + §2 are drafted; §3–§6 are stubs. STOP for review (R4). -->
