<!-- PR-3 — COMMITTED 2026-07-17 (Derek's resequence). PR-1 Rung 2 (X=2^17) landed at commit
     fa4e35a (STRENGTHENS PERSISTENT), releasing the hold. The D2 estimator (§"D2
     operationalization") is pinned BELOW before any of its numbers are computed; the
     postscript records the measured outcome. Verbatim pins are from cited sources only
     (rules 4/6/8). -->

# PR-3 — Is the height-murmuration deficit root-number-imbalance leakage of Sutherland's parity-independent bias?

**Status: COMMITTED (2026-07-17). PR-1 Rung 2 (X=2¹⁷) landed (fa4e35a). Mode: M5 research
(RESEARCH-M §7). The pre-registration content is the §0d expectation envelope and the pinned
D2 estimator (§"D2 operationalization"), both fixed before D2's numbers are read; the D2 run
and postscript follow, authorized by Derek's 2026-07-17 resequence.**

---

## Step 0 — verbatim pins (from cited sources only)

**0a — Sutherland, murmurations talk (Sept 2023), "Bias cancellation" (slides 11–12), VERBATIM**
(source `docs/papers/Sutherland_Slides.pdf`, gitignored; slides drive.google.com/file/d/17VR0FPH8kNTL3nPXKrY4zu4m6C2Ml_9G,
video youtube.com/watch?v=AFupZY06FPY):

> "There is a negative bias in ā_p that is parity-independent and disappears in m̄_p. This is
> especially noticeable at primes p ≡ 1 mod 24 and p ≡ □ mod 5, 7."

with the definitions pinned on slides 9–10: a blue/red/purple dot at (p, ā_p or m̃_p) is
"the average of **a_p** or **m_p := w(E)·a_p** over even/odd or all E/ℚ with N_E ∈ (M, 2M)."
So **ā_p** is the *unsigned* average of the Frobenius trace over the family, and **m̄_p** is
the *root-number-weighted* ("signed") average — the murmuration statistic. The named classes
are congruence conditions on p; the arithmetic reason (labelled interpretation, **not** a
slide quote) is the **divisibility of p−1**: p ≡ 1 mod 24 ⟺ 24 | (p−1), and p ≡ □ mod 5, 7
are quadratic-residue conditions tied to the multiplicative structure mod those primes.

Slides 6–7 pin the rank/`L`-function context: the Mestre–Nagao / BSD sum
`lim_{x→∞} (1/log x) Σ_{p≤x} a_p(E) log p / p = −r + ½` (Kim–Murty 2023, conditional), the
rank-stratified version of which converges to its asymptotic ordering only at very large
conductor (plotted to ~10¹⁷) — the scale caveat PR-1 already carries.

**0b — [SS25] p.3–4, VERBATIM** — the persistent downward bias in the *signed* statistic and
the rank remark, already pinned in `docs/preregistered/PR-2.md` §Hypothesis (not re-quoted
here; that pin is the reference).

**0c — Relationship hypothesis (LABELLED AS HYPOTHESIS, not a claim of either source).**
Sutherland's bias lives in the **unsigned** average ā_p (and *cancels* in the balanced signed
average m̄_p). [SS25]'s deficit lives in the **signed** statistic. PR-3's hypothesis is that
these are the *same* arithmetic bias, coupled into the signed statistic by a **root-number
population imbalance** — i.e. the cancellation Sutherland observes is only exact when the
even/odd (w=±1) populations are equal, and a real imbalance leaks a residue of the unsigned
bias into the signed statistic. Neither source asserts this link; PR-3 tests it.

**0d — Plausibility envelope (PRE-DATA expectation, stated before D2 is computed).** From the
committed 2¹⁶ ne_cache split (n₊=2492, n₋=2550), the root-number imbalance is
**δ = −58/5042 = −0.0115**. The observed tail deficit is **O(0.5–1) density units**. If the
deficit were *pure* leakage (deficit = δ·β), it would require **β ≈ 45–90** — but β is a bias
in the *plain average* of a_p, expected **O(0.1–1)**; **β ≈ 45–90 is implausible by ~2 orders
of magnitude.** So PR-3 does **not** test leakage as *the* explanation; it tests it as a
**PARTIAL contributor**, with the pre-data expectation that it accounts for a **small fraction
f ≪ 1** of the deficit. **A BOUND on f is the expected deliverable; a full explanation
(f ≈ 1) would be the surprise** — and would itself be a red flag (an anomalously large β).
This envelope is fixed here, before the measurement; D2 reports the measured f against it.

## Mechanism — "team-imbalance leakage"

Split the family (in a conductor/height bin) into F₊ (w=+1, n₊ curves) and F₋ (w=−1, n₋).
Writing ā_p^± for the unsigned average over each side and β(p) for the *parity-independent*
component (Sutherland: β(p) < 0, concentrated at the named classes), the signed statistic is

```
    m̄_p = (1/|F|)[ n₊·ā_p^+ − n₋·ā_p^− ]
        = δ · β(p)  +  (murmuration signal),     δ := (n₊ − n₋)/|F|.
```

The parity-independent part β cancels **only** when δ = 0. A nonzero root-number imbalance δ
leaves a residual **δ·β(p)** — a slowly-varying offset in the signed statistic. Against a
density D(u) whose amplitude **decays** toward u→1, a roughly-constant offset is *relatively*
largest where D(u) is smallest — the descending branch — so it reads as a **tail-dominant
displacement**: exactly the shape of the observed trough deficit. **The sign and magnitude of
δ·β are to be MEASURED (D2), not assumed** — if they do not match the deficit in sign, the
mechanism is refuted (a deliverable).

*Already-visible ingredient (not yet a look):* the committed ne_caches show a small
root-number imbalance — 2¹⁶: n₊=2492, n₋=2550 (δ≈−0.0115); 2¹⁷: n₊=4472, n₋=4542 (δ≈−0.0078)
— so δ ≠ 0, the necessary condition for any leakage.

## Discriminants

**D2 — quantitative leakage: THE SOLE COMMITTED DISCRIMINANT (feasible from committed caches,
NO new a_p).** All three inputs are recoverable without recomputing a_p:
- **δ per rung** from the committed ne_cache root-number counts;
- **β per rung** (the unsigned bias) from the committed partials by **dividing ε back out**:
  the partials store `num = (prefactor)·ε·a_p` per curve; `num/ε` recovers the unsigned
  `(prefactor)·a_p`, whose family average is ā_p — no a_p recompute (ε is in the ne_cache);
- **the deficit per rung** from the committed signed partials (the descending-branch offset
  below D(u) already emitted).

Test — a **direct per-rung table, NOT a fit.** The primary D2 output is the per-rung ratio
**f_rung = (δ·β)_rung / deficit_rung** (with its sign) tabulated across the rungs — because the
ladder rungs are **NESTED** families (H ≤ X is cumulative), so per-rung observations are
**strongly correlated** and a regression across them has far fewer effective degrees of freedom
than points; a five-point nested table must not be dressed up as inference. **If a slope is
drawn at all it is purely descriptive, the nesting caveat rides it, and the ANNULUS increments
(H ∈ (X_{k−1}, X_k], the only quasi-independent pieces of the ladder) are reported alongside.**
At the magnitudes §0d predicts (f ≪ 1) the conclusion is the same either way. The estimator is
pinned exactly in §"D2 operationalization" below, and its epistemic status is stated honestly
there: **D2 is a pre-committed plausibility bound, not a blind-holdout confirmation** — R2
(fa4e35a) already publicly read the 2¹⁷ trough, and every D2 input (δ, S, U, D) is a
deterministic re-aggregation of committed caches. The pre-registered content is §0d's envelope,
fixed *before* U and f are computed; the measurement fits nothing.

Branch language (pre-framed per §0d): the deliverable reads **"leakage measured at [δ·β = x],
sign [correct/wrong], fraction of deficit f = [value]"** — a **bound** (f ≪ 1) is the expected
outcome; a correct sign with small f *confirms leakage as a minor partial contributor*; a
wrong sign *refutes* it; f ≈ 1 would be the surprise flagged in §0d.

### D2 operationalization — the estimator, PINNED before measurement (2026-07-17)

All quantities are in the **emitted density units** (the units of the `curve … density` column
of the committed `ss_x*.txt` run). For a rung X, let Fam(X) = {E : H(E) ≤ X}, |fam| = |Fam(X)|,
and n± = #{E ∈ Fam : ε = ±1}, all from the committed ne_cache (H recomputed as
max(4|A|³, 27B²), the exact good-prime key). Let num[c][b] be the committed per-curve bin
partials (num = Σ_p (u_mid·lnN/N)·ε·a_p), Δu the bin width, and b\* the trough bin (the global
trough of S from the shared two-pass extractor — i.e. PR-1's trough_u).

- **δ(X) = (n₊ − n₋)/|fam|.**  [dimensionless — the root-number imbalance]
- **Signed density  S(u_b) = (Δu·|fam|)⁻¹ Σ_{c∈Fam} num[c][b].**  Reproduces the emitted D̂ —
  a free consistency twin against the committed `ss_x*.txt` density column (any mismatch voids
  the reading).
- **Unsigned density  U(u_b) = (Δu·|fam|)⁻¹ Σ_{c∈Fam} ε(c)·num[c][b].**  (ε divided back out:
  since ε = ±1, num/ε = ε·num — no a_p recompute; the density-weighted analogue of Sutherland's
  parity-independent ā_p.)
- **Conjectured density  D(u_b) = ss_density(u_b, 2000, 2000)** — the committed formula-side
  Bessel-J₁ density, X-independent, at the same truncation bound the emitter uses (kDensB).
- **Departure  Δ(u_b) = S(u_b) − D(u_b)** — observed signed density minus the conjectured
  limit; the gap the mechanism must explain (negative on the descending branch, where the
  empirical dips below D). §0d's "deficit O(0.5–1) density units" is |Δ| on that branch.
- **Leakage  L(u_b) = δ(X)·U(u_b)** — the residual the imbalance leaks, from the exact identity
  S = δ·U under (i) parity-independence of the unsigned bias and (ii) an ε-independent weight
  distribution. Both are the *mechanism's* hypotheses, not assumed true: L is the **prediction**,
  Δ the **observation**.
- **Leakage fraction  f(X) = L(u\*)/Δ(u\*)** at the trough bin b\*.  **f ≈ 1** ⟺ leakage explains
  the whole departure (the §0d SURPRISE, demanding β-scrutiny); **f ≪ 1** ⟺ minor partial
  contributor (the §0d EXPECTED outcome); **f < 0** ⟺ leakage points the wrong way (REFUTED).

Reported per rung: the scalar row (δ, U(u\*), L(u\*), S(u\*), D(u\*), Δ(u\*), f, sign L vs sign Δ);
the descending-branch profile L(u) vs Δ(u) for u > zero_u; and the **annulus increments**
(H ∈ (X_{k−1}, X_k], the only quasi-independent ladder pieces) alongside the nested cumulative
rows. Computed by the read-only `at ss-leakage` subcommand over the committed partials +
ne_cache; it recomputes a_p for nothing.

**Epistemic status (honest, rule 5): D2 is NOT a blind-holdout confirmation.** R2 (fa4e35a)
already publicly read the 2¹⁷ trough, and every D2 input (δ, S, U, D) is a deterministic
re-aggregation of already-committed caches — nothing here is unseen. D2's pre-registered
content is the **§0d expectation envelope** (f ≪ 1; β ≈ 45–90 would be needed for f ≈ 1, ~2
orders beyond a plausible unsigned bias), committed *before* U and f are computed; the
measurement **fits nothing** (δ, U, D are each fixed by committed data; f is derived, with no
free parameter to tune). Its evidential weight is therefore that of a **pre-committed
plausibility bound**, not a blind confirmation — recorded here, carrying that caveat.

**D1 — congruence stratification: CONTINGENT EXTENSION, UNRUN.** The intended test — is the
tail deficit *concentrated* in the named classes (p≡1 mod 24, p≡□ mod 5,7) versus uniform
across prime classes? — needs the deficit **resolved by prime class**, which the committed
partials do **not** provide (they aggregate over primes into u-bins; no per-prime a_p is
retained). D1 is therefore **not runnable from committed data** and requires a partials-v2
format (sub-binning by u-bin × prime-class) — a change to the hash-bound `ss_empirical.cpp`,
hence its **own pre-registration + an a_p re-run** (or the deferred M0b fast-a_p stage).

**Trigger (pre-committed):** D1 is opened **only if D2 leaves a residual with structure** —
i.e. the leakage fraction f does not close the deficit (expected per §0d) *and* the residual
shows prime-class dependence worth resolving. **If instead D2 bounds the mechanism out** (small
f, correct-or-wrong sign, no structured residual pointing at the named classes), then the
congruence-stratification mechanism is **recorded as DIED in this doc** (the
hypotheses-die-in-public pattern): the referee's prime-class mechanism is closed by the D2
bound, without ever running the expensive stratified pass. D1 stays unrun and its own prereg
is written only if the trigger fires.

## Provenance of the 2¹⁷ rung (run-engineering note)

D2's 2¹⁷ rung is the 2¹⁷ `ss-run`, computed by the **conductor-ascending chunked build**
(commit `e8396cb`): curves are processed cheapest-first for reaper-resilience and lower
table-build overhead. Byte-identity of that build to the canonical order was verified at
**scratch scale** (the X=3000 family, chunk 64, `pp3000_sorted.txt` == `pp3000_clean.txt`
bit-for-bit); the **full-scale proof is R2's consistency twin** — the 2¹⁷ run's ≤2¹⁶ rows
reproduce the committed M4/2¹⁶ artifacts exactly. **That twin PASSED (R2, commit fa4e35a);**
the 2¹⁷ partials are therefore certified for D2. (D2 does not depend on the 2¹⁷ trough being
"unseen" — R2 already read it; see the epistemic-status note under D2 operationalization.)

## Every branch carries (verbatim, as committed)
- **Wachs clause:** *this design does not distinguish rank per se from BSD invariants
  correlated with rank (Wachs 2603.04604); "carried by" is a statement about the
  subpopulation, not the mechanism.*
- **Sutherland-bias attribution:** the parity-independent unsigned bias, the named congruence
  classes, and the cancellation-in-the-signed-average are Sutherland's (Sept-2023 talk); PR-3
  tests only the *leakage relationship*, which is PR-3's hypothesis, not Sutherland's claim.

## LOOKS
D2 is recorded **here** (not as a LOOKS entry) as a pre-committed plausibility bound — with the
honest caveat above that none of its inputs is a blind holdout: R2 already read the 2¹⁷ trough,
and δ/S/U/D are deterministic re-aggregations of committed caches. No separate exploratory look
precedes it; the §0d envelope is the pre-registered content, fixed before U and f are computed.

## Claim licensed per branch (all Wachs- and Sutherland-attribution-bound; f = leakage fraction)
- **Leakage as a minor partial contributor (the EXPECTED outcome, §0d):** correct sign,
  small **f ≪ 1** → *"leakage of Sutherland's parity-independent unsigned bias, through the
  measured root-number imbalance δ≈−0.0115, accounts for a bounded fraction f=[x] of the
  height-murmuration tail deficit at H≤2¹⁷ — a minor partial contributor; the bulk of the
  deficit is unexplained by this mechanism."* A bound, not an explanation; not a rank claim.
- **Leakage refuted (null):** wrong sign, or f consistent with 0 → *"root-number-imbalance
  leakage does not contribute to the tail deficit at this scale"* — a citable null; the
  congruence mechanism (D1) is then recorded as died (above).
- **Full explanation (the SURPRISE, §0d):** correct sign with **f ≈ 1** → NOT taken at face
  value; it would require β ≈ 45–90, ~2 orders beyond a plausible unsigned bias, so it is
  reported as an anomaly demanding scrutiny (a measurement of β, a check of δ) before any claim.
- **D1 (contingent, unrun):** evaluated only if D2 leaves a structured residual (trigger above).

## Postscript — D2 measured (2026-07-17), commit 21060a0 pinned the estimator first

Run: `at ss-leakage` (read-only, no a_p recompute) over the committed partials + ne_caches,
D = `ss_density(u, 2000, 2000)`. Both files passed the free **S-twin** (my aggregated S vs the
committed `ss_x*.txt` density column: max|ΔS| = 4.9×10⁻⁶ = the 6-sig-fig print floor). A free
**cross-platform twin** fell out too: the ≤2¹⁶ ladder rows aggregated from the FreeBSD-computed
2¹⁷ partials are identical (to printed precision) to those from the laptop 2¹⁶ partials.

**Cumulative per-rung table** (all densities in emitted units; b\* = trough bin, u\*≈0.8875):

| X | δ | U(u\*) | L(u\*)=δ·U | S(u\*) | D(u\*) | dep\*=S−D | f=L/dep | sign |
|---|---|---|---|---|---|---|---|---|
| 4000 | −0.06897 | +0.800 | −0.05518 | −3.642 | +0.515 | −4.158 | **+0.0133** | same |
| 6000 | −0.05422 | −1.299 | +0.07044 | −3.302 | +0.161 | −3.463 | **−0.0203** | OPP |
| 8000 | −0.05058 | +0.653 | −0.03302 | −3.483 | +0.515 | −3.998 | **+0.0083** | same |
| 10000 | −0.05344 | +0.345 | −0.01842 | −3.471 | +0.515 | −3.986 | **+0.0046** | same |
| 2¹⁶ | −0.01150 | −0.326 | +0.00375 | −3.715 | +0.515 | −4.230 | **−0.0009** | OPP |
| 2¹⁷ | −0.00777 | −0.327 | +0.00254 | −3.652 | +0.515 | −4.168 | **−0.0006** | OPP |
| 2¹⁸ | −0.01029 | −0.312 | +0.00321 | −3.580 | +0.515 | −4.095 | **−0.0008** | OPP |

*(The 2¹⁸ row was appended 2026-07-17 with the PR-1 Rung-3 M0b production run — the D2 estimator
was already pinned commit 21060a0, so δ was committed before the a_p; the re-aggregated 2¹⁶/2¹⁷
rows reproduce this table's committed values exactly, a free consistency check.)*

Annulus increments (quasi-independent shells) tell the same story: f ∈ {+0.013, 0.000,
+0.001, −0.020, −0.0001, −0.0002, **−0.0010**}; the newest shell **(2¹⁷,2¹⁸]** (6922 curves,
δ=−0.01358) gives L\*=+0.00396 vs dep\*=−4.001 ⇒ f=−0.0010, OPP — in line with all the others. The
(4000,6000] shell has n₊=n₋ exactly ⇒ δ=0 ⇒ L=0 (the mechanism's necessary condition, cleanly
visible). The descending-branch profile (u>zero_u) has |L(u)| ≤ 0.0054 across the *entire* branch
at 2¹⁸ while |dep(u)| ranges 0.05–4.1 — L is negligible everywhere, with no structure.

### Verdict — leakage **bounded out** (the §0d EXPECTED outcome, sharper than expected)

- **f ≪ 1 at every rung**: |f| ≤ 0.021 (cumulative) and ≤ 0.020 (annulus), across the **full
  four-rung ladder to 2¹⁸**. At the three largest, least-noisy rungs (2¹⁶, 2¹⁷, 2¹⁸) f = −0.0009,
  −0.0006, −0.0008 — the leakage accounts for < 0.1% of the trough departure, and does not grow
  with X.
- **The absolute leakage is tiny and denominator-robust**: |L(u\*)| ≤ 0.07 density units (and
  ≤ 0.003 at 2¹⁶/2¹⁷), against a trough departure ≈ 4 and a signed trough depth ≈ 3.7. Even
  divided by §0d's smaller O(0.5–1) "deficit," f ≈ 0.005 at 2¹⁶ — still ≪ 1. The conclusion
  does not depend on the denominator choice, because the *numerator* is negligible.
- **The sign is UNSTABLE** (cumulative: same, OPP, same, same, OPP, OPP, OPP), because U(u\*) — the
  unsigned bias *at the trough bin* — itself flips sign across rungs (+0.80, −1.30, +0.65,
  +0.34, −0.33, −0.33, −0.31). At the three most reliable rungs the leakage points *against* the
  deficit.
  A material leakage would be a stable, same-sign offset; this is noise-like scatter about zero.
- **§0d's implausibility argument confirmed, decisively**: f ≈ 1 would need |U(u\*)| ≈
  |dep\*|/|δ| ≈ 368 at 2¹⁶; measured |U(u\*)| = 0.33 — short by ~3 orders of magnitude (§0d
  predicted ~2 from the β≈45–90 heuristic; the direct measurement is even more decisive).

**Claim licensed (per the pre-committed branch language, Wachs- and Sutherland-attribution-bound):**
*root-number-imbalance leakage of Sutherland's parity-independent unsigned bias does NOT
materially contribute to the height-murmuration tail deficit at H ≤ 2¹⁷ — the leakage fraction
is |f| ≤ 2% with unstable sign (< 0.1% and pointing against the deficit at 2¹⁶/2¹⁷), a citable
null.* This is a **bound**, not an explanation; not a rank claim. **The tail deficit (PR-1's
persistent trough displacement) remains unexplained by this mechanism.**

**D1 (congruence stratification) — recorded as DIED** (the pre-committed trigger, hypotheses-die-
in-public): D2 bounds the mechanism out (small f, sign-unstable/wrong-sign at the reliable rungs,
no structured descending-branch residual pointing at the named classes p≡1 mod 24, p≡□ mod 5,7).
Per the trigger, the referee's prime-class mechanism is closed by the D2 bound **without** running
the expensive stratified pass. D1's own pre-registration is NOT written; the partials-v2
(u-bin × prime-class) re-run is not undertaken.

*Caveat (rule 5, honest): the single-bin f mixes the trough displacement with the trough depth,
because the conjectured trough (u≈0.805) and the empirical trough (u≈0.8875) are misaligned in u
(hence D(u\*)>0 while S(u\*)<0). The descending-branch profile is the fuller picture and carries
the same verdict (L negligible across the whole branch). And D2 is a pre-committed plausibility
bound, not a blind-holdout confirmation — see the epistemic-status note above.*

---

## POSTSCRIPT — LEAKAGE BOUND REGENERATED (2026-07-19, ERRATA #28)

The D2 table above took the trough-bin departure `dep = S(u*) − D(u*)` against the **corrupted**
density (its trough at 0.805, so D(u*) > 0 at u* = 0.8875, dep ≈ 4). With the eq (2) correction
(**ERRATA #28**, corrected trough 0.870), the conjectured density at u* = 0.8875 is now
**D(u*) = −2.94** — deeply negative, close to the empirical S(u*) = −3.58 — so the departure shrinks
to **dep ≈ −0.64 at 2¹⁸** (−0.53 / −0.78 / −0.71 / −0.64 at 10⁴/2¹⁶/2¹⁷/2¹⁸). Regenerated by
`at ss-leakage` (corrected `target_trough`):

- The leakage numerator **L = δ·U is unchanged** (density-independent).
- The fraction **f = L/dep** is now **≤ 0.5% and opposite-signed at the three reliable rungs**
  (−0.005 / −0.005 / −0.005 at 2¹⁶/2¹⁷/2¹⁸) and **+3.5% (same-signed) at 10⁴** (smallest family,
  largest |δ|, no bootstrap). Previously ≤ 2% — the larger dep had divided it down.
- `f ≈ 1` would require `|U(u*)| ≈ |dep|/|δ| ≈ 67` at 2¹⁶ against a measured 0.33 — short by
  **~2 orders** (the prior note said ~3, against the larger dep).

**The bound holds**: leakage is immaterial (f ≪ 1), a citable null. **VOID:** the caveat directly
above — that "the single-bin f mixes displacement with depth because the conjectured trough (0.805)
and the empirical trough (0.8875) are misaligned, hence D(u*) > 0 while S(u*) < 0" — the corrected
trough (0.870) is *aligned* with the empirical, and D(u*) is now negative; the misalignment it
described was the #28 artifact. The descending-branch profile (leakage negligible across the whole
branch) carries the same verdict. Data note §4 (PR-3) has the regenerated table. See
`docs/notes/reframe-registry.md`.
