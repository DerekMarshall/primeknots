# M0 pinning log — a_p machinery and data provenance

Two-phase gate for Stage M0 (RESEARCH-M §2, ARCHITECTURE-M §7). Convention-free
code and the frozen referee do not exist until the [PIN TO SOURCE] items below are
resolved against **sources**, not against RESEARCH-M.md (a paraphrase) or memory.

**Status.** Phase 0 (sources + data) — §0, complete. Phase 1 (§1–§4, pins + frozen
referee + tests) — complete, pinning review PASSED. Phase 2 (§5 small-prime path +
fast path + a_p cache + parallel bit-identity) — authorized; §5 is the R1 mini-pin.
The charsum referee (§1) and `ap_enumerate` (§5) are frozen referees; the fast path
is checked against them.

---

## §0 — Data provenance (Phase 0, complete)

### Cremona ecdata — the pin

- source: https://github.com/JohnCremona/ecdata (releases carry no downloadable
  assets; the data lives in versioned repo dirs, so the pin is a git commit and
  per-file sha256 is exact).
- tag **`2026-04-22`** = commit **`25cec5ecfec8b9f016eb1631ac633194c2bed39f`** (lightweight tag).
- format spec cited below is `docs/file-format.txt` at that commit.
- fetch command: `python3 oracle/fetch_ecdata.py` · fetch date: 2026-07-09.
- checksum record (also in `data/cremona/MANIFEST.json`, validated by the loader):

  | file | sha256 | bytes |
  |---|---|---|
  | `allcurves.00000-09999` | `259f3846329395b371e8079c77a6f1097adaebc98a054974573e241416efa968` | 2146401 |
  | `aplist.00000-09999` | `a1cc11fb2c53b58bcf03600cd67d8d9133a14381f1df9dac7b096c20699eca65` | 3809559 |

M0 fetches the conductor-`< 10000` slice (contains the `11a1` anchor plus ample
sampling); M1 extends `RANGES` in the fetch script to the full N ≤ 500,000 set.

### ecdata file formats (quoted from `docs/file-format.txt` @ pinned commit)

- **allcurves**: `ID AI R T` — one line per curve. `ID = N CLASS NUM`; `AI` =
  "Reduced minimal Weierstrass coefficients in format `[a1,a2,a3,a4,a6]`"; `R` =
  rank; `T` = torsion order. Example: `11 a 1 [0,-1,1,-10,-20] 0 5`.
- **aplist**: `N CLASSCODE AP25 BQ` — one line per **isogeny class**. Per the doc:
  > "AP: `a2 a3 a5 a7 a11 (etc.)` where each ap is the p'th eigenvalue of the
  > newform (integers, separated by whitespace). When p divides N this is the W_p
  > eigenvalue, shown just as `+` or `-`; else the T_p eigenvalue."
  > "AP25: AP for all primes<100 (the first 25 primes)."

  So at **good** p the entry is the T_p eigenvalue = a_p (trace of Frobenius); at
  **bad** p | N it is the Atkin–Lehner W_p sign (`+`/`-`), *not* a_p. This is the
  source for the M0 anchor's good-prime values, and it correctly walls off the
  deferred bad-prime handling.

### Papers (fetched by `oracle/fetch_papers_m.py`; title-page verified; gitignored)

All five arXiv PDFs fetched and title-page verified (manifest:
`docs/papers/MANIFEST-M.json`): [HLOP22] 2204.10140, [LOP23] 2307.00256,
[Z25] 2310.07681, [SS25] 2504.12295, [C24] 2408.12723. Note [C24]'s true title is
"Murmurations and ratios conjecture**s**" (plural), vs RESEARCH-M.md's singular —
arXiv is authoritative. The Sarnak letter [Sar23] is **framing only** (non-normative
for M0); its IAS landing page is Cloudflare-guarded (403 to automated tools) and no
stable direct-PDF path is published, so it is recorded SKIP-with-manual-instructions,
not fetched. None of M0's code depends on it.

---

## §1 — a_p sign and normalization  [PIN TO SOURCE — resolved]

**Definition (quoted, HLOP `docs/papers/hlop-2204.10140.pdf` §1):** "where
a_p(E) = p + 1 − #E(F_p), and #E(F_p) is the number of points of E over F_p."

**Referee derivation (not assumed).** For p > 3 and a short model y² = x³ + Ax + B,
each x contributes 1 + χ_p(f(x)) affine points (χ_p = Legendre symbol; χ_p(0)=0 ⇒
the single point y=0), plus the point at infinity:

    #E(𝔽_p) = 1 + Σ_x (1 + χ_p(f(x))) = p + 1 + Σ_x χ_p(x³+Ax+B).

Hence, with **no free sign**,

    a_p = p + 1 − #E(𝔽_p) = − Σ_{x mod p} ( (x³+Ax+B) / p ).

This is `ell::ap_charsum` (frozen). Legendre via the existing Stage-0
`symbols::legendre_euler`. The short model comes from the long invariants
c4, c6 (Silverman III.1): E ≅ y² = x³ − 27·c4·x − 54·c6 over ℤ[1/6], an isomorphism
defined at every p > 3, so a_p is preserved at good p > 3. All arithmetic is done
mod p (coefficients reduced first), so there is no overflow and no large-curve
dependence. p = 2, 3 are **out of the referee's scope** (the short model needs 6
invertible); the referee claims only good p > 3.

**Anchor (`anchor_ap_11a1`), source-quoted.** Curve `11a1`.
- Model — `data/cremona/allcurves.00000-09999` @ commit `25cec5ec…`, line `11 a 1`:
  `[a1,a2,a3,a4,a6] = [0,-1,1,-10,-20]`, rank 0, torsion 5.
- Good-prime a_p — `data/cremona/aplist.00000-09999` @ pin, line `11 a` (AP25 = a_p
  for the first 25 primes; p=11 is the bad prime, shown as the W₁₁ sign `-`, and is
  excluded here). The p > 3 good-prime T_p eigenvalues (= a_p):

  | p | 5 | 7 | 13 | 17 | 19 | 23 | 29 | 31 | 37 | 41 | 43 | 47 | 53 | 59 | 61 | 67 | 71 | 73 | 79 | 83 | 89 | 97 |
  |---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
  | a_p | 1 | -2 | 4 | -2 | 0 | -1 | 0 | 7 | 3 | -8 | -6 | 8 | -6 | 5 | 12 | -7 | -3 | 4 | -10 | -6 | 15 | -7 |

  22 values (the primes 5..97 minus the bad prime 11). Cross-referenced LMFDB curve
  `11.a` (Cremona `11a`). Worked check: at p=5, c4=496, c6=20008 ⇒ A≡3, B≡3 (mod 5),
  Σ_x χ₅(x³+3x+3) = −1, a_5 = −(−1) = 1 ✓ (PARI `ellap([0,-1,1,-10,-20],5)=1`).

## §2 — Model reduction and invariance  [PIN TO SOURCE — resolved]

long → short reduction is valid for p > 3 (6 invertible mod p). a_p is an
**isomorphism invariant** at good p (isomorphic models over ℚ, hence over ℚ_p for
good p, have the same reduction and the same trace of Frobenius).

**`invariance_weierstrass_model` design.** Three models of the *same* curve `11a1`:
- A = minimal `[0,-1,1,-10,-20]`.
- B = an integral translation, `translate(A; r,s,t)` with u=1 (change of variable
  (x,y) ↦ (x+r, y+sx+t); Silverman III.1 with u=1 — always integral).
- C = an integral scaling, `scale(A; u=2)` = `[u·a1, u²·a2, u³·a3, u⁴·a4, u⁶·a6]`
  (change (x,y) ↦ (x/u², y/u³); c4 ↦ u⁴c4, c6 ↦ u⁶c6, same a_p at good p ∤ u).
Assert `ap_charsum(A,p) = ap_charsum(B,p) = ap_charsum(C,p)` over all 22 good
p > 3 < 100 (all coprime to u=2). Model choice is an arbitrary convention; a
sign/normalization bug that survived the anchor by luck would break invariance.

## §3 — Bad primes  [PIN TO SOURCE — resolved; three pins per referee review]

The ecdata bad-prime column holds an **Atkin–Lehner sign**, and an A–L sign is
**not** an a_q — the conversion is sign-bearing and reduction-type-dependent.
Failure mode guarded: if HLOP's average touches a bad prime, an unconverted sign
injects a systematic error into exactly the curves whose bad prime lands in the
plotted range — silent, global, invisible to every good-prime test.

**3a — Does HLOP's statistic include p | N? (quoted, HLOP §1, eq. (1.1)):**

    f_r(n) = (1 / #E_r[N₁,N₂]) · Σ_{E ∈ E_r[N₁,N₂]} a_{pₙ}(E),

over the ascending prime sequence p₁=2, p₂=3, p₃=5, … with **no exclusion of
p | N**; the a_p sequence is taken "Using [LMFDB, Elliptic curves over Q]" (§1,
Example 1), which supplies a_p at bad primes as the standard 0/±1. **⇒ bad-prime
a_p is in the averaged statistic**, so the conversion below is load-bearing for M1.

**3b — ecdata column → a_q conversion (quoted, `docs/file-format.txt` @ pin):**

    "AP: a2 a3 a5 a7 a11 (etc.) where each ap is the p'th eigenvalue of the
     newform … When p divides N this is the W_p eigenvalue, shown just as '+'
     or '-'; else the T_p eigenvalue."

The doc identifies the stored bad-prime quantity as the **W_p (Atkin–Lehner)
eigenvalue** but gives **no explicit a_q formula** — it is ambiguous on the
conversion, so 3c adjudicates. *Hypothesis to test* (referee's recollection, NOT
taken as the answer): a_q = −w_q for multiplicative reduction (q ∥ N), a_q = 0 for
additive (q² | N). Reduction type is read from the conductor: `ord_q(N) = 1`
⇔ multiplicative, `ord_q(N) ≥ 2` ⇔ additive.

**3c — Mechanical adjudication (`anchor_ap_bad_prime`; PARI settles it):** for named
curves with a small bad prime, apply the conversion to ecdata's A–L sign and check
against PARI `ellap` (definitive, regardless of doc quality). Models + A–L signs
from ecdata @ pin; `ellap` from `gp` (recorded verbatim):

  | curve | model | q | ord_q(N) | type | ecdata A–L w_q | −w_q / 0 | PARI a_q |
  |---|---|---|---|---|---|---|---|
  | 11a1 | [0,-1,1,-10,-20] | 11 | 1 | mult | `-` (−1) | +1 | **+1** |
  | 14a1 | [1,0,1,4,-6] | 2 | 1 | mult | `+` (+1) | −1 | **−1** |
  | 14a1 | [1,0,1,4,-6] | 7 | 1 | mult | `-` (−1) | +1 | **+1** |
  | 27a1 | [0,0,1,0,-7] | 3 | 3 | add | `-` (−1) | 0 | **0** |
  | 32a1 | [0,0,0,4,0] | 2 | 5 | add | `-` (−1) | 0 | **0** |

  (`gp`: ellap(ellinit(model), q); ord_q(N) = valuation(ellglobalred(E)[1], q).)

**Resolution — hypothesis CONFIRMED.** Pinned conversion `ell::ap_from_atkin_lehner`:
`ord_q(N)==1 ⇒ a_q = −w_q` (multiplicative); `ord_q(N)≥2 ⇒ a_q = 0` (additive —
the A–L sign is present but a_q vanishes; the 27a1/32a1 rows prove the naive
"convert the sign" is wrong). **Independently corroborated by a second source**,
SS25 §1.2 (quoted in §5): a_q = +1 (split mult.), −1 (non-split mult.), 0 (additive)
— matching the PARI adjudication (the A–L sign w_q = −a_q for multiplicative q, so
−w_q recovers the split/non-split value). **Tate's algorithm at p = 2, 3 remains DEFERRED to
M0b** (its own gate): here q-reduction-type comes from ord_q(N) and the a_q value
from the oracle A–L sign, never from computing the local model.

## §4 — ecdata column map (provenance) + M1 standing note

| file | column | meaning | provenance |
|---|---|---|---|
| allcurves | N | conductor | **oracle** |
| allcurves | CLASS, NUM | isogeny-class label + curve index | oracle (labels) |
| allcurves | AI = [a1..a6] | reduced minimal Weierstrass model | **oracle** |
| allcurves | R | rank | **oracle** |
| allcurves | T | torsion order | **oracle** |
| aplist | AP25 (good p) | T_p eigenvalue = a_p | referee source / **computed** by `src/ell` |
| aplist | AP25 (bad p\|N) | W_p Atkin–Lehner sign → a_q via §3 | **oracle** |
| aplist | BQ | A–L data for bad primes q > 100 | **oracle** |

Good-prime a_p is the one **computed** quantity (the point of M0); everything else
consumed from ecdata is oracle-provenance. No `theorem_` may draw its conclusion
from an oracle column except a stated cross-check (RESEARCH-M §2 / ARCH-M §2).

**M1 standing note (binds the M1 gate, recorded here so the M1 session inherits
it):** *At M1 the replicated conductor ranges come from HLOP's paper, and the data
slice follows them — never the reverse. If the pinned ranges exceed the current
N < 10,000 slice, extending the fetch (new manifest, new shas) is the required
move; selecting among the paper's ranges by what is already downloaded would be
family-definition drift.*

## §5 — Small-prime a_2, a_3  [PIN TO SOURCE — resolved; rider R1]

The frozen charsum referee (§1) is **p > 3** (the short model needs 6 invertible).
But §3a establishes the M1 statistic sums a_p over **all** primes, so good-prime
a_2 and a_3 need their own computed path. `ell::ap_enumerate`: direct point count
on the **long** Weierstrass model mod p (valid for any good p, used at p ∈ {2,3}),

    #E(𝔽_p) = 1  +  #{ (x,y) ∈ 𝔽_p² : y² + a1·x·y + a3·y ≡ x³ + a2·x² + a4·x + a6 } ,
    a_p = p + 1 − #E(𝔽_p),

with the point at infinity counted **explicitly** as the leading `1`.

**a_p formula (quoted, SS25 §1.2, `docs/papers/sawin-sutherland-2504.12295.pdf`):**
"For p prime, a_p(E) is equal to p + 1 − |E(𝔽_p)| if E has good reduction at p,
equal to 1 if E has split multiplicative reduction at p, equal to −1 if E has
non-split multiplicative reduction at p, and equal to 0 if E has additive reduction
at p." (Also the §3 corroboration.)

**Good-reduction criterion (correction C1 — minimal-model-only).** Good reduction
of E at p is **p ∤ Δ_minimal ⟺ p ∤ N** (N the conductor, oracle-provenance). This is
NOT "p ∤ Δ(model)" in general: under a rescaling (x,y) ↦ (u²x, u³y) the discriminant
scales Δ ↦ u¹²·Δ, so a non-minimal model carries extra primes in Δ that do not
divide N. The reduced *model* over 𝔽_p is nonsingular ⟺ p ∤ Δ(model) (Δ is by
definition the obstruction to nonsingularity; standard, Silverman AEC Ch. III.1 —
*rule-6 note:* not fetched, so stated as the standard fact and adjudicated against
PARI below, not attributed by a verbatim quote I cannot verify), and this coincides
with good reduction **only when the model is p-minimal**.

**The hazard (state it plainly).** Point-counting a_p on a **non-p-minimal** model
counts a *singular* reduction and returns a **wrong a_p silently** — e.g. `scale(11a1,
2)` reduces to y² = x³ (mod 2), giving a_2 = 0 instead of the true −2. Guard: the ell
API takes minimality as a **checked precondition**, `ell::assert_minimal(E, N)`,
which throws when a prime divides Δ(model) but not N (i.e. asserts p ∤ Δ(model) ⟺
p ∤ N for all p). Every curve entering the a_p path must pass it; the M1 ecdata
loader calls it per curve. Demonstrated firing (both the refusal and the wrong-a_p
it prevents) in `invariance_weierstrass_model`, to the cache-refusal standard.

**Current M0 results are unaffected:** every input used is an ecdata *minimal* model
(the anchors, the sample curves, the grid), so p ∤ Δ(model) ⟺ p ∤ N held throughout;
C1 hardens the precondition against a future non-minimal input, it does not change
any computed value. Operational adjudication of the criterion on the minimal models:

  | curve | model | Δ(model) | 2∤Δ | 3∤Δ | N | agrees w/ p∤N |
  |---|---|---|---|---|---|---|
  | 11a1 | [0,-1,1,-10,-20] | −11⁵ = −161051 | ✓ | ✓ | 11 | ✓ (good at 2,3) |
  | 14a1 | [1,0,1,4,-6] | −2⁶·7³ = −21952 | ✗ | ✓ | 14 | ✓ (bad at 2) |
  | 27a1 | [0,0,1,0,-7] | −3⁹ = −19683 | ✓ | ✗ | 27 | ✓ (bad at 3) |

  (`gp`: `E.disc`, `ellglobalred(E)[1]`.) p ∤ Δ(model) ⟺ p ∤ N in every row.

**Referee structure — Option B (stated).** ONE enumeration implementation
(`ell::ap_enumerate`), twinned against **`oracle_ellap` extended to p ∈ {2,3}**
across a REQUIREd curve sample. PARI `ellap` computes a_2, a_3 correctly for every
reduction type and is genuinely independent; a second hand-rolled enumeration would
share the same point-count skeleton (twin-independence principle), so PARI is the
right outside witness.

**Anchor (`anchor_ap_small_primes`), source-quoted.** `11a1` (good at 2, 3 since
N=11): a_2 = **−2**, a_3 = **−1** — `aplist.00000-09999` line `11 a`, first two
AP25 tokens. Enumeration hand-check: mod 2, y²+y = x³+x² ⇒ 4 affine + 1 ∞ = 5 ⇒
a_2 = 3−5 = −2 ✓; mod 3, 4 affine + 1 ∞ = 5 ⇒ a_3 = 4−5 = −1 ✓.

**Failure mode guarded.** An off-by-one at infinity at p=2 shifts every curve's a_2
by 1, silently biasing the leftmost bins of every murmuration curve M1 will draw.
The explicit `+1`, the anchor, and `oracle_ellap` at p ∈ {2,3} pin it.

## Performance — Phase 2 wall-time (a_p fast path)

Per-prime QR-table batching (`ell::ap_fast_grid`), measured on this machine
(Darwin, ~10 cores), grid = 1000 curves × 1227 primes (p ≤ 10⁴) = 1,227,000 cells:

| mode | time | throughput |
|---|---|---|
| parallel | 5.52 s | 2.24 × 10⁵ a_p/s |
| serial | 54.7 s | 2.24 × 10⁴ a_p/s |

Speedup ≈ 9.9×; parallel and serial outputs **bit-identical** (also asserted by
`invariance_parallel_vs_serial`). Extrapolating to M1's ~10⁵-curve grid over the
same prime range is ~10² × this ⇒ core-minutes parallel, consistent with the
ARCHITECTURE-M §5 budget ("M1 default grid in core-minutes, extended overnight").
The frozen referee (`ap_charsum`, per-symbol modpow) is ~10× slower per cell than
the table path and is never used in the grid — only as the twin referee.

## Standing decisions (project-wide)

**Repo name — DECIDED permanently (Derek, explicit; rule 7 satisfied).** The
repository remains `primeknots` **permanently**, as an homage to the project's
origin (arithmetic topology, primes-as-knots), **not** a scope claim; M-stages and
any future stage families live under it. **No M3 revisit** — this supersedes the
earlier "staged to M3" note at Derek's explicit word (C2). Deferred consequence, for
whichever session next touches public-facing text: a one-line README note that the
repo's scope outgrew its name deliberately, so a reader arriving for murmurations
data isn't confused by the title. No action now.
