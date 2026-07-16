<!-- M0b PINNING — PHASE 1 (PINNING ONLY). No implementation exists or is authorized by this
     doc. This is the source-pinning artifact that RESEARCH-M §8 requires before M0b code is
     written ("its own pinning + twins"). It STOPS at a review gate (§9). Every threshold /
     constant is quoted from a NAMED, LOCATED source (rules 6 & 8); nothing is from memory, and
     anything not eyes-on-verified is marked UNVERIFIED. -->

# M0b pinning — fast a_p via Shanks–Mestre point counting (PHASE 1: PINNING ONLY)

**Status: PHASE 1 COMPLETE — reviewed 2026-07-15, amendments R1–R4 incorporated; Phase 2
(implementation) AUTHORIZED (§9).** M0b is the deferred
faster-a_p stage named in `docs/RESEARCH-M.md` §8; PR-1's Rung-3 clause and any X ≥ 2¹⁸ work
motivate building it now. This document pins the algorithm, its load-bearing threshold, the
small-p fallback, the edge cases, and the verification contract **to cited sources** — so that
implementation (Phase 2) is a transcription of a reviewed spec, not improvisation. It follows the
two-phase gate `docs/RESEARCH-M.md` §8 mandates ("M0b = … with its own pinning + twins").

## 0. Frame (the prime directive, applied to an optimization)

- **a_p is a uniquely-determined integer.** M0b computes the *same* value as the frozen referee
  `ell::ap_charsum`, only faster. So M0b changes **nothing mathematical** — no statistic, no
  normalization, no convention, no pre-registered threshold. It is pure infrastructure **plus a
  new, genuinely independent twin** (point counting is a different algorithm from the two
  character-sum paths we have — `ap_charsum` modpow referee and `ap_fast` QR-table). The
  dual-implementation non-negotiable (CLAUDE.md rule 2) actively *wants* this third method.
- **Referee stays frozen.** `ap_charsum` is the referee and is never optimized to match M0b
  (RESEARCH-M §M0, ARCHITECTURE-M §5). M0b is checked *against* it, one direction only.
- **Ceiling, stated up front.** Shanks–Mestre gets us off the O(p) floor (≈2¹⁸–2²⁰-class reach);
  it does **not** reach [SS25]'s 2²⁸, which needs the SEA / average-polynomial-time tier
  ([Schoof85] and beyond). M0b Phase 1 pins **BSGS/Shanks–Mestre only**; SEA is a separate future
  question, not pinned here.

## 1. Sources (located; verification status marked per rule 6)

- **[S-notes] — PRIMARY, VERIFIED (eyes-on the rendered PDF, 2026-07-15).** A. V. Sutherland,
  *18.783 Elliptic Curves*, Spring 2022, **Lecture #7 "Point counting"**
  (`math.mit.edu/classes/18.783/2022/LectureNotes7.pdf`). Sutherland is the [SS25] co-author, so
  this is ideal provenance for the method [SS25] themselves use. Read directly: §7.2 (Hasse), §7.3
  (our QR-table method), §7.4 (order of a point via interval scan / BSGS), §7.5 (group exponent
  λ, Thm 7.6), §7.6 (quadratic twist), §7.7 (**Theorem 7.7, Mestre — the p>229 threshold**).
- **[Schoof85] — VERIFIED bibliographically (located via AMS/MathComp record; full text not
  read).** R. Schoof, "Elliptic Curves over Finite Fields and the Computation of Square Roots
  Mod p," *Mathematics of Computation* **44**(170), 1985, 483–494, doi:10.1090/S0025-5718-1985-0777280-6.
  [S-notes] cites this as where Mestre's theorem was **published** (its ref [4]) and as Schoof's
  point-counting algorithm.
- **[CS10] — VERIFIED bibliographically (located via arXiv + JTNB; abstract read, full text not
  read → improvement claim marked accordingly).** J. E. Cremona and A. V. Sutherland, "On a
  theorem of Mestre and Schoof," *J. Théor. Nombres Bordeaux* **22**(2), 2010, 353–358,
  arXiv:**0901.0120**. [S-notes] cites this (its ref [2]) as the prime-power generalization. Per
  the located abstract, it **improves the threshold to q>49 (all finite fields) and q>29 (prime
  fields)** — sharper than the p>229 in Thm 7.7.
- **[Cohen] — CANDIDATE, UNVERIFIED.** H. Cohen, *A Course in Computational Algebraic Number
  Theory*, GTM 138, §7.4.3 (Shanks–Mestre), for an independent write-up of the algorithm. Named as
  a candidate cross-reference; **not located/verified** — do not cite a specific algorithm number
  until checked.

## 2. Paper-notation → code-identifier mapping [PIN TO SOURCE]

| source notation | meaning | intended code identifier |
|---|---|---|
| `#E(F_p)` | order of the group of F_p-points (incl. ∞) | `curve_order` (to compute) |
| `t := tr π_E`, `#E = p+1−t` [S-notes §7.2] | Frobenius trace | **`a_p` (this is our a_p — exact same object as `ap_charsum`)** |
| `H(p) = [(√p−1)², (√p+1)²]` [S-notes §7.2] | Hasse interval, width 4√p | `hasse_lo`, `hasse_hi` |
| `λ(G) = lcm{\|α\|}` [S-notes Def 7.5] | group exponent | `group_exponent` |
| `Ẽ`, `#E+#Ẽ = 2p+2` [S-notes §7.6] | quadratic twist + twin relation | `twist_curve`, used only to disambiguate |
| Thm 7.7 threshold `p > 229` [S-notes §7.7] | twist-trick uniqueness bound | `MESTRE_THRESHOLD` (a **tested parameter**, §5) |

## 3. The algorithm (pinned outline — NO code here)

Short Weierstrass model `y² = x³ + Ax + B` over F_p (`p > 3`), the same `(A,B) = (−27c₄, −54c₆)`
reduction `ap_fast` already uses — **M0b MUST use the identical model** so the twin is an exact
integer comparison, not an isomorphism-class argument.

1. **Bound.** `#E(F_p) ∈ H(p) = [(√p−1)², (√p+1)²]` (Hasse, [S-notes §7.2, Thm 7.3]).
2. **Order of a point (fast).** Pick `P ∈ E(F_p)`; find a multiple of `|P|` in `H(p)` by
   baby-step giant-step over the interval — **O(p^{1/4})** group operations ([S-notes §7.4], the
   improvement over the O(√p) linear interval scan; see §8). Accumulate the exponent `λ` from a
   few random points ([S-notes Def 7.5, Thm 7.6]: two random points determine λ with probability
   `> 6/π²`).
3. **Disambiguate (the crux — §4).** If the multiple of `λ(E)` in `H(p)` is **unique**,
   `#E(F_p)` is determined. If not, use the quadratic twist `Ẽ` and `#E + #Ẽ = 2p+2`
   ([S-notes §7.6]); Theorem 7.7 (§4) guarantees one of the two is unique for `p` above threshold.
4. **Trace.** `a_p = p + 1 − #E(F_p)`.

### 3a. Determinism and the randomness pin [R1]

M0b is the **repo's first randomized algorithm** (point selection; twist non-residue choice), so it
needs an explicit determinism pin the character-sum paths never required.

- **Output determinism is safe by mathematics.** a_p is a uniquely-determined **integer** — the
  *same value whatever points were sampled*. So the parallel-vs-serial and cross-platform
  byte-identity guarantees hold for M0b's **outputs** unconditionally, exactly as for `ap_fast`.
  State this explicitly: no derandomization is needed for output correctness or reproducibility.
- **Sampling is DERANDOMIZED anyway (pinned).** Point selection follows a **deterministic sequence
  seeded by a canonical hash of (A, B, p)**; the twist uses the **smallest non-residue mod p** (as
  in the A2 adjudication, s=3 for p=233). No RNG, no wall-clock seed, no thread-order dependence.
- **Reason (not cosmetic):** a mismatch must **replay bit-identically on both platforms** — failure
  reproducibility. Derandomizing keeps the parallel-vs-serial and cross-platform guarantees
  trivially true for **diagnostics and intermediate state** (which point failed, in which order),
  not only for the final integer output — so a bug surfaced at 2¹⁸ reproduces identically on the
  laptop referee.

## 4. Order-ambiguity and the twist trick (the load-bearing pin) [PIN TO SOURCE]

**Problem.** The exponent λ(E(F_p)) can have **more than one** multiple in H(p) when λ ≤ 4√p, so a
point's order alone need not pin `#E(F_p)` ([S-notes §7.5–7.6]).

**Resolution — Theorem 7.7 (Mestre), VERBATIM from [S-notes §7.7, p.6]:**

> "Let p > 229 be prime, and let E/F_p be an elliptic curve with quadratic twist Ẽ/F_p. At least
> one of the integers λ(E(F_p)) and λ(Ẽ(F_p)) has a unique multiple in the Hasse interval
> H(p) = [(√p−1)², (√p+1)²]."

[S-notes] attributes this to **Mestre, published by Schoof [Schoof85]**, and notes a prime-power
generalization in **[CS10]**. The proof's own words: *"if neither M nor N has a unique multiple in
H(p), then p < 17413. An exhaustive computer search for p < 17413 finds that in fact we must have
p ≤ 229."* So **229 is not a heuristic — it is the exact exhaustive-search bound.**

**Threshold to pin (a decision for review, §9):**
- **Conservative (default):** `MESTRE_THRESHOLD = 229`, i.e. Thm 7.7 as stated. Below it, fall
  back (§5). Costs nothing — small primes are trivial for the O(p) referee.
- **Sharp (optional):** [CS10] improves prime fields to `q > 29`. Adopting 29 shrinks the fallback
  band but relies on [CS10] full text (currently abstract-only here → **UNVERIFIED for the exact
  29 bound**; verify before adopting).

Either way the boundary is a **tested parameter** (§5), validated by the twin — **never a magic
constant baked past review.**

### 4a. Termination and the loop invariant [R2]

The accumulated exponent λ′ (the lcm of the orders of the points sampled so far) only **divides**
the true exponent λ, so early iterations may see **several** Hasse-interval candidates even when
Theorem 7.7's uniqueness holds for the *full* λ — A2 (§6) makes this concrete: λ(E)=52 admits
{208, 260} until the twist is brought in.

**Loop invariant (pinned):** each iteration extends λ′ on **both** E and its twist Ẽ, then checks
whether **either** λ′ now has a unique multiple in H(p).
- **Correctness / termination [correctness register]:** for p > 229, Theorem 7.7 guarantees the full
  λ of E *or* Ẽ has a unique multiple, so the loop **terminates** once λ′ reaches that λ on the
  winning side. This does **not** depend on how many points happened to be lucky.
- **Expected cost [performance register — labelled as such, per R2]:** Theorem 7.6 (two random
  points fix λ with probability > 6/π²) bounds the **expected** iteration count at O(1). This is a
  *performance* fact, **not** a correctness one — even a pathological sampling sequence still
  terminates by the Theorem 7.7 argument; it merely takes more iterations.

## 5. Small-p fallback [PIN TO SOURCE]

- `p ≤ MESTRE_THRESHOLD` → compute `a_p` with the **frozen `ap_charsum`** (O(p), and p is tiny, so
  this is free). M0b's fast path handles only `p > MESTRE_THRESHOLD`.
- The boundary is a **tested parameter**, not a constant: the twin (§7) runs across a band
  straddling the threshold, so an off-by-one or an over-tight threshold is caught as a mismatch.
  Default 229 ([S-notes Thm 7.7]); tightening to [CS10] 29 is a reviewed change, not an edit.
- **p ∈ {2, 3}**: out of M0b scope entirely — bad-prime a_p is oracle-provenance
  (RESEARCH-M §M0 / HLOP convention), and the short model needs p > 3.

## 6. Edge cases (enumerated; each anchor's expected value PARI-adjudicated, recorded) [PIN — R4]

Per R4 / the §C3 PARI-adjudication precedent: **no anchor carries a from-memory expected value** —
each is source-quoted or adjudicated by PARI `ellap`/`ellgroup` and **recorded here** (gp 2.17.4),
so the Phase-2 tests read the expected value from this log, not from a coder's recollection (which
would reintroduce rule 8 through the test suite). Hasse interval used below: **H(233) = [204, 264]**.

- **A1 — supersingular, a_p = 0.** E: y²=x³+1 (A=0, B=1, j=0), p=233 (233 ≡ 2 mod 3 ⇒ supersingular).
  `ellap = 0`, `#E = 234`, `ellgroup = [234]` (cyclic). *M0b must return exactly 0.*
- **A2 — non-cyclic group + the uniqueness ambiguity (the Thm 7.7 anchor).** E: y²=x³−x (A=−1,
  B=0, j=1728), p=233 (≡ 1 mod 4 ⇒ ordinary). `ellgroup = [52, 4]` (non-cyclic), so the exponent
  λ(E)=52 **under**-counts #E=208. Multiples of 52 in H(233): **{208, 260} — TWO**, so E alone is
  ambiguous. Smallest non-residue mod 233 is s=3; the twist y²=x³−9x has `ellgroup = [130, 2]`,
  λ(Ẽ)=130, whose only Hasse multiple is **{260} — UNIQUE** ⇒ #Ẽ=260 ⇒ #E = 2·233+2−260 = 208 ⇒
  `ellap = 26`. *M0b must return 26 via the twist, not stall on E's ambiguity (see §4a).*
- **A3 — bad reduction, deferred (not point-counted).** E: y²=x³+x+2 (disc = −1792 = −2⁸·7).
  At p=7: `ellap = −1`; at p=2: `ellap = 0` — the local factor (oracle/referee provenance, HLOP
  convention). *M0b must DEFER, never attempt a count at p | disc.*
- **A4 — threshold boundary (the fast/fallback switch).** E: y²=x³+x+1 (disc = −496 = −2⁴·31,
  good at 227/229/233). `ellap(227)=0`, `ellap(229)=−2`, `ellap(233)=−3`. With MESTRE_THRESHOLD=229:
  227 and 229 route to the **charsum fallback**, 233 to the **fast path** — all three must match.
- **A5 — ordinary j=0 (no special-case bug; pairs with A1).** E: y²=x³+1, p=241 (241 ≡ 1 mod 3 ⇒
  ordinary): `ellap = 14`, `ellgroup = [114, 2]`. Same curve as A1, SS-vs-ordinary flipped by p mod 3.
- **A6 — model minimality.** M0b must reduce with the SAME `(A,B) = (−27c₄, −54c₆)` map as
  `ap_fast`; a non-minimal integer model must give the identical a_p. *(No new gp value — a
  same-input/same-output check against the referee, covered exhaustively by the §7 full-cache twins.)*

**Adjudication provenance:** `gp -q` (PARI/GP **2.17.4**), via `ellinit([A,B])`, `ellap(E,p)`,
`ellgroup(E,p)`, and `kronecker(s,p)` for the twist non-residue. Re-runnable verbatim from this line.

## 7. Verification contract [PIN — the Phase-2 acceptance gate]

M0b is accepted only when ALL of the following are green (a_p is an **integer** → these are
**exact-equality** checks, not tolerances; a single mismatch is a bug and aborts):

1. **`twin_m0b_vs_charsum_x16`** — M0b reproduces the **checksummed 2¹⁶ a_p cache in full**: every
   curve, every prime, exact integer match.
2. **`twin_m0b_vs_charsum_x17`** — same, against the **checksummed 2¹⁷ a_p cache in full**.
   *(1 & 2 are the strongest possible check — millions of (curve, prime) a_p values already computed
   by the frozen referee, reproduced bit-for-bit.)* **[R3 provenance]** these a_p caches are
   **local, gitignored artifacts** (ARCHITECTURE-M §4), **checksummed and generator-hash-verified** —
   the twin verifies the cache header (the generator hash of the `ap_charsum`/`ap_fast` source)
   before consuming, refusing a stale cache. They are **not committed** repo artifacts; the
   distinction is the **ERRATA #18** lesson (reproducible-local vs committed), so this contract says
   *checksummed* caches, never *committed* ones.
3. **`twin_m0b_bruteforce_x18_tailweighted`** — at 2¹⁸ (no committed full referee), a
   **stratified sample weighted toward the high-conductor tail** (where M0b's speedup, and thus any
   large-p bug, concentrates) recomputed with the O(p) referee and matched exactly.
4. **`oracle_ellap_m0b_spot`** — a spot sample checked against **PARI `ellap`** (independent
   oracle); SKIPs cleanly (visible SKIP) when `gp` is absent.

Two-phase gate (RESEARCH-M §8): **Phase 1 = this doc, reviewed. Phase 2 = implementation**, which
begins only after the review sign-off and produces the twins above before M0b is trusted or used
for any rung.

## 8. Complexity claims (pinned) — and a RESEARCH-M discrepancy FLAGGED (rule 5)

- Referee `ap_charsum` / `ap_fast`: **O(p)** per prime — this is exactly [S-notes §7.3]'s
  QR-table method ("precompute a table of quadratic residues … O(q M(log q))").
- Linear Hasse-interval scan (add P across the interval): **O(√p)** group ops ([S-notes §7.4],
  "4√q additions suffice").
- **BSGS over the interval: O(p^{1/4})** ([S-notes §7.4], the stated improvement over the linear
  scan) — **this is M0b's target complexity.**
- SEA / average-polynomial-time: **O(polylog p)** ([Schoof85] and successors) — the 2²⁸ tier,
  **out of M0b scope.**

**FLAG (rule 5 — do not smooth):** `docs/RESEARCH-M.md` §8 writes *"baby-step–giant-step
(O(p^{1/2}))"*. Per [S-notes §7.4], **O(p^{1/2}) is the *linear interval scan*, not BSGS**; true
BSGS over the width-4√p interval is **O(p^{1/4})**. Recommend at review: adopt **O(p^{1/4})** as
M0b's target and correct the RESEARCH-M §8 note (an ERRATA-class edit), rather than inherit the
looser bound.

## 9. Review gate — PASSED (2026-07-15); Phase 2 authorized

Reviewed 2026-07-15; amendments **R1** (randomness pin, §3a), **R2** (termination/loop invariant,
§4a), **R3** (checksummed-not-committed provenance, §7), **R4** (PARI-adjudicated anchors, §6)
incorporated. **Phase 2 (implementation) is AUTHORIZED.** Resolutions of the Phase-1 open questions:

1. **Threshold — RESOLVED:** pin `MESTRE_THRESHOLD = 229` ([S-notes Thm 7.7], eyes-on-verified) as
   the default; the fallback band below it is free. Tightening to [CS10]'s `p > 29` is deferred and
   gated on verifying [CS10] full text — not adopted now.
2. **Complexity note — RESOLVED:** the RESEARCH-M §8 `O(p^{1/2})→O(p^{1/4})` correction (§8) is
   approved as an ERRATA-class edit, to be made when Phase 2 lands (so RESEARCH-M and the M0b code
   agree on the target complexity).
3. **[Cohen]/[CS10] full text — RESOLVED:** proceed on [S-notes] alone (sufficient for the algorithm
   and the verified 229 threshold); [CS10] stays abstract-level unless/until the sharper bound (Q1)
   is pursued.
4. **Scope — RESOLVED:** BSGS/Shanks–Mestre + Mestre twist only; SEA / average-polynomial-time is a
   separate future stage, explicitly out of M0b scope.

**Phase 2 acceptance = the §7 contract green** (full-cache twins at 2¹⁶/2¹⁷, tail-weighted
brute-force sample at 2¹⁸, `oracle_ellap` spot), with the §6 anchors as unit tests reading their
expected values from this log.
