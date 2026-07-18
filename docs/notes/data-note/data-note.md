<!-- DATA NOTE — DRAFT. Gate OPEN (Derek's word, 2026-07-17). Deliverable of this block:
     §1 Introduction + §2 Methods drafted in full; §3–§6 are stubs marked [PENDING]; STOP for
     section-by-section review (rule R4) before drafting §3 onward.

     VENUE (reconciled, not silent): the base prompt said "~8pp arXiv math.NT"; the venue
     amendment (2026-07-17) SUPERSEDES the venue → REPO PUBLICATION rendered on the GitHub
     Pages site (same pipeline as docs/deck/), PDF as a build product, Markdown source (not
     LaTeX-first). KEPT unchanged: the ~8pp discipline, the §1–§6 section plan, and rules
     R1–R5. Dropped: arXiv-only packaging (subject class, abstract-form limits). See README.md.

     Numbers/quotes are from cited sources or emitted artifacts only (rules 6/8; R3). Every
     nontrivial sentence carries a `claim:Nx-y` marker (an HTML comment) with a row in CLAIMS-N.md. -->

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
it against their conjectured Bessel-$J_1$ density. Over a four-rung height ladder,
$X = 10^4, 2^{16}, 2^{17}, 2^{18}$, two calibrated shape invariants (the positive hump and the first
post-hump zero) agree with the conjectured density within an a-priori tolerance, while the trough
sits at a fixed displacement that exceeds the tolerance at every rung. A pre-registered decision
rule returns H0: the trough deviation is persistent across the ladder, a finite-range statement
over $X \le 2^{18}$ and not a claim about the large-$X$ limit. Two pre-registered follow-ups
constrain candidate mechanisms: excising the analytic-rank-2 subpopulation does not move the
trough, so the deficit is not carried by rank-2 over-representation, and leakage of the
parity-independent unsigned bias through the measured root-number imbalance is bounded to
$\lvert f\rvert \le 2\%$ with unstable sign. Neither explains the deficit. Every result is numerical over a stated finite range, twinned
or oracle-refereed, and cross-checked for cross-platform reproducibility. We claim replication and
constraint, not proof; the deviation is left open.

---

## 1. Introduction

Murmurations are an oscillating correlation between the Frobenius traces $a_p(E)$ of a family of
elliptic curves and the prime $p$. He–Lee–Oliver–Pozdnyakov discovered them in 2022; the
phenomenon has since been observed in several arithmetic settings, and proved in the first of them,
Zubrilina's weight-2 density. <!-- claim:N1-1 --> The companion explainer (E1) gives a fuller
account.

Sawin–Sutherland [SS25] order the family by naive height rather than by conductor and propose an
explicit candidate for its limiting density. Two of their statements must be kept apart; the
distinction fixes what this note claims. The first is a conjecture, the second a theorem.

- **Conjecture 1 (the density).** For $0 < C_1 < C_2$, [SS25] conjecture that the
  $X \to \infty$ limit of their statistic (1) equals $\int_{C_1}^{C_2} D(u)\,du$, where the
  murmuration density is the Bessel-$J_1$ double sum of their equation (2):

  $$
  \begin{aligned}
  D(u) &= 2\pi\sqrt{u} \sum_{\substack{q\,\in\,\mathbb{N}\\ \text{squarefree}}} \sum_{m\,\in\,\mathbb{N}}
    \frac{\mu(\gcd(m,q))}{q\,m\,\varphi\!\left(q/\gcd(m,q)\right)} \\
  &\hphantom{{}={}} \times\, J_1\!\left(4\pi\sqrt{u}\,m/q\right)
    \prod_{\substack{p\,\mid\,m\\ p\,\nmid\,q}} \hat{\ell}_{p,\,2v_p(m)}
    \prod_{p\,\mid\,q} \ell_{p,\,v_p(m)},
    \qquad u = p/N.
  \end{aligned}
  $$

  Here $J_1$ is the Bessel function of the first kind, $v_p$ the $p$-adic valuation, $\mu$ the
  Möbius function, and $\varphi$ Euler's totient; the local factors $\ell_{p,\nu}$ and
  $\hat\ell_{p,\nu}$ are those of their Lemmas 3 and 4. <!-- claim:N1-2 -->
- **Theorem 2 (the proven variant).** [SS25] prove a variant in which the prime sum is replaced by
  a sum over integers with no prime factor $\le P$, against a smooth weight; the same integrand
  appears, and the $P \to \infty$ limit is established. This smoothed variant motivates Conjecture 1
  without being the prime-sum murmuration. <!-- claim:N1-3 -->

The explicit density of the height-ordered prime-sum murmuration is therefore conjectural. The
proven Theorem 2 is a rigorous backdrop, not the object our empirical curve is compared against. <!-- claim:N1-4 -->
This note replicates and probes Conjecture 1; it proves nothing. Throughout, "verified" means
numerically, over a stated finite range, cross-checked by two independent implementations, by a
published anchor value, or by an external oracle. It never means a formal proof. <!-- claim:N1-5 -->

**What this note tests.** We compute statistic (1) from scratch over the height family up to
$X = 10^4, 2^{16}, 2^{17}, 2^{18}$, bin it by $u = p/N(E)$, and compare the empirical density to the
conjectured $D(u)$ through three shape invariants fixed in advance: the positive hump, the first
zero crossing after it, and the trough (§3). <!-- claim:N1-6 --> Where the empirical curve departs
from $D(u)$, two pre-registered follow-ups constrain candidate mechanisms (§4).

**What this note does not test.** It makes no asymptotic ($X \to \infty$) claim. Our computable
range, $X \le 2^{18}$, sits at the bottom of the window over which [SS25] observe the bias and
expect it to decay, naive height $2^{16}$ to $2^{28}$, reached only through sub-linear point
counting. Every "persistent" statement below concerns that finite range, not a limit. <!-- claim:N1-7 --> The
discussion (§6) gives a documented precedent in which a small-conductor picture does not survive
to the asymptotic one; that is why the distinction is not pedantic.

---

## 2. Methods

**Family and ordering.** We use the short Weierstrass family $E_{A,B}: y^2 = x^3 + Ax + B$ with
integers $A, B$, *reduced* (no prime $p$ with $p^4 \mid A$ and $p^6 \mid B$, the unique minimal
representative of each isomorphism class) and *nonsingular* ($4A^3 + 27B^2 \ne 0$), ordered by the
naive height $H(E) := \max(4\lvert A\rvert^3, 27\lvert B\rvert^2)$, quoted verbatim from [SS25] p.1.
<!-- claim:N2-1 --> The family $\{E_{A,B} : H(E) \le X\}$ is enumerated directly over the box
$\lvert A\rvert \le (X/4)^{1/3}$, $\lvert B\rvert \le (X/27)^{1/2}$; its cardinality is certified by
a second, independent sieve algorithm required to agree with the direct count at every cutoff. <!-- claim:N2-2 -->

**The two objects, side by side.** The note compares one *computed* quantity with one
*conjectured* one. The distinction (§1) governs what the note claims.

*Empirical: a binned estimator of [SS25] statistic (1).* For a prime-ratio window $(C_1, C_2)$,
[SS25]'s statistic (1) is

$$
\mathbb{E}_{\{E\,:\,H(E)\le X\}}\!\left[
  \frac{\log\!\left(N(E)^{(C_1+C_2)/2}\right)}{N(E)}
  \sum_{\substack{p\,\in\,(C_1 N(E),\,C_2 N(E))\\ p\ \text{prime}}}
  \epsilon(E)\,a_p(E)
\right].
$$

We evaluate it as a density on $(0,1]$: partition into 40 bins of width $\Delta u = 0.025$, take each
bin $b = (b\,\Delta u,\, (b+1)\Delta u]$ as the window (so its midpoint
$(C_1+C_2)/2 = u_{\mathrm{mid}} := (b+\tfrac12)\,\Delta u$), and divide by $\Delta u$:

$$
\hat D(u_b) = \frac{1}{\Delta u\,\lvert\mathrm{fam}\rvert}
  \sum_{(E,p)\,:\,p/N(E)\,\in\,\mathrm{bin}_b}
  \left(u_{\mathrm{mid}}\,\frac{\ln N(E)}{N(E)}\right)\epsilon(E)\,a_p(E),
$$

over good primes $p > 3$ with $p \nmid (4A^3 + 27B^2)$ and $p \le N(E)$ (so $u \le 1$). The
$u_{\mathrm{mid}}$ factor is eq (1)'s $(C_1+C_2)/2$ normalization exponent evaluated at the bin
midpoint, not a per-pair weight, so $\hat D$ is a binned density estimator of statistic (1), one bin
per window. <!-- claim:N2-3 --> For this family the reduced short model is $p$-minimal at every
$p > 3$, so $p \nmid \Delta(\text{model}) \iff p \nmid N(E)$ (good reduction), and this exact
good-prime test keeps $a_p$'s domain computed-only. <!-- claim:N2-4 -->

*Conjectured: [SS25] eq. (2) / Conjecture 1.* [SS25] conjecture that the $X \to \infty$ limit of
(1) equals $\int D(u)\,du$, where $D(u)$ is the Bessel-$J_1$ density transcribed verbatim in §1.
eq (1) is a definition we evaluate; eq (2)'s equality to the limit is the *conjecture*. $\hat D$ is
compared to $D$; the note reports empirical agreement, never a proof (§1). <!-- claim:N2-15 -->

**Provenance: computed vs. oracle.** The Frobenius trace $a_p = p + 1 - \#E(\mathbb{F}_p)$ is
computed from scratch. The conductor $N(E)$ and root number $\epsilon(E)$ are oracle-provenance
input supplied by PARI/GP, trusted input *to* the statistic and not a referee *of* our numbers.
Every emitted column is labelled with its provenance. <!-- claim:N2-5 --> The oracle $N/\epsilon$
are certified before use by a dual-oracle overlap check against LMFDB/Cremona data. <!-- claim:N2-6 -->

**Twin / anchor / oracle architecture.** Each function the result depends on has two
independent implementations (a different algorithm, never a refactor of itself) or an external
oracle check. $a_p$ is computed by a fast quadratic-residue-table character sum (`ap_fast`)
twinned against a frozen modular-exponentiation referee (`ap_charsum`); the two agree exactly on
a certified sample. <!-- claim:N2-7 --> Published values are pinned as anchor tests; oracles
(PARI, LMFDB, Odlyzko) referee and are never called from the computational core; an absent
oracle skips visibly rather than passing silently. <!-- claim:N2-8 --> The full method is
described in the companion method explainer (E3). <!-- claim:N2-9 -->

<!-- x17 gate RESOLVED 2026-07-17 (twin_m0b_vs_charsum_x17 GREEN, commit fbe51a5): the sentence
     below, drafted-and-gated per R2, is now live prose. Recorded non-silently (R5). -->
$a_p$ is computed by three independent algorithms: the frozen `ap_charsum` referee,
the fast QR-table `ap_fast`, and Shanks–Mestre point counting. They agree exactly over the full
$2^{16}$ and $2^{17}$ $a_p$ grids (112 M and 385 M values, 0 mismatches), at about 145× less CPU
aggregated over the $2^{16}$ grid ($O(p^{1/4})$ point counting vs. $O(p)$ character sum). At
$2^{18}$ the equality is verified on a tail-weighted 27.9 M-value sample against the $O(p)$ referee
and on a PARI `ellap` spot (204 pairs, $p$ up to 6.9 M), the pre-registered production-capability
gate. This is a cross-algorithm check (Shanks–Mestre vs. the charsum referee). Since $a_p$ is a
platform-independent integer, it is verified same-platform by design, not as a two-platform
claim. <!-- claim:N2-14 -->

**Pre-registration.** Every decision rule, tolerance, and threshold is committed to version
control *before* the data it judges exists, cited here as (pre-registered
$\to$ read) commit pairs. PR-1, the ladder decision rule and finite-$X$/persistent fork: pre-registered
`dd6beb0`, with the $2^{18}$ Rung-3 clause `4a17ebe` committed *before the $2^{17}$ rung was read*,
read `8f64ba1` (verdict H0, $\le 2^{18}$). PR-2, the analytic-rank contrast threshold: pre-registered `f7415a4`
*before any split curve*, read `876999f` (null). PR-3, the leakage estimator: pre-registered
`21060a0`, read `0d21b62`. <!-- claim:N2-10 --> Every exploratory statistic looked at is recorded
in a looks ledger. <!-- claim:N2-11 -->

**The conjectured density.** $D(u)$ is evaluated independently of the empirical statistic (a
separate translation unit, authored without reference to the empirical side) as the Bessel-$J_1$
double sum of Conjecture 1 with the local factors of [SS25] Lemmas 3–4, an in-house $J_1$, and
*generated* constants (never typed), truncated at $q, m \le 2000$. <!-- claim:N2-12 --> Because
every value is built from IEEE operations, generated constants, and the in-house Bessel, the
emitted curve is byte-portable across compilers and platforms, a property we verify (§5). <!-- claim:N2-13 -->

---

## 3. Results

**The ladder.** Over four height cutoffs the two calibrated shape invariants (the positive hump
and the first post-hump zero) agree with the conjectured density $D(u)$ within the a-priori
tolerance $\tau = 0.06$, while the trough sits at $u = 0.8875$ at every rung, a displacement
$d = \lvert 0.8875 - 0.805\rvert = 0.0825$ that is frozen across the ladder. All values are from the
committed `data/m5/ss_x{65536,131072,262144}.txt` runs and the M4 run `data/m4/ss_empirical.txt`:

| $X$ | $\lvert\mathrm{fam}\rvert$ | hump u (dev) | first zero u (dev) | trough u (dev) | trough v |
|---|---|---|---|---|---|
| $10^4$ | 1048 | 0.4625 (0.0125) | 0.672894 (0.0279) | 0.8875 (0.0825) | $-3.47$ |
| $2^{16}$ | 5042 | 0.4625 (0.0125) | 0.670328 (0.0253) | 0.8875 (0.0825) | $-3.72$ |
| $2^{17}$ | 9014 | 0.4625 (0.0125) | 0.673202 (0.0282) | 0.8875 (0.0825) | $-3.65$ |
| $2^{18}$ | 15936 | 0.4625 (0.0125) | 0.671945 (0.0269) | 0.8875 (0.0825) | $-3.58$ |

Hump dev 0.0125 and first-zero dev $\le 0.0282$ are both within $\tau = 0.06$ (agreement with
$D(u)$). The trough dev 0.0825 exceeds $\tau$ at every rung, a persistent open deviation. <!-- claim:N3-1 --><!-- claim:N3-2 -->
(Fig. 1, the empirical-vs-$D(u)$ overlay, is regenerated from these committed runs by
`figures/make_figures.py`.)

**Verdict: H0 (persistent, $\le 2^{18}$).** The decision rule was pre-registered before the data
(PR-1, `dd6beb0`; the $2^{18}$ Rung-3 clause `4a17ebe` committed *before the $2^{17}$ rung was
read*). With $d(X) = \lvert \mathrm{trough}_u - 0.805\rvert$ and $\Delta u = 0.025$, the Rung-3
clause reads, verbatim:

> **H1 (finite-$X$, $\le 2^{18}$)** iff the trough recovers monotonically by $\ge \Delta u$ at
> **both** steps: $d(2^{17}) \le d(2^{16}) - \Delta u$ **and** $d(2^{18}) \le d(2^{17}) - \Delta u$
> … **H0 (persistent, $\le 2^{18}$)** iff the trough stays flat across the ladder (no
> $\ge \Delta u$ recovery at either step).

Observed $d = 0.0825$ at $2^{16}, 2^{17}$, and $2^{18}$: flat, with no recovery at either step,
hence H0 (persistent, $\le 2^{18}$), read at `8f64ba1`. <!-- claim:N3-3 --> This is the first rung
at which the pre-registered full-ladder verdict could be pronounced. It is a finite-range statement
($\le 2^{18}$), explicitly not the $X \to \infty$ verdict [SS25] describe.
$2^{18}$ is the bottom of their $2^{16}$–$2^{28}$ decay window.

**Supporting observations (not the gate).** (i) The first-zero series
$0.672894 \to 0.670328 \to 0.673202 \to 0.671945$ is flat near about 0.671, non-monotone,
converging to neither the 0.645 target nor away, direction only. <!-- claim:N3-4 --> (ii) The
trough *depth* eases monotonically across the three extension rungs $-3.715 \to -3.652 \to -3.580$
even as its *position* holds at 0.8875. The $10^4$ depth ($-3.47$) is shallower than $2^{16}$, so
this easing is a three-rung observation, not a monotone four-rung trend. It is a hint of amplitude
decay consistent with the expected large-$X$ behaviour, but the committed rule gates on position,
so it does not move the verdict; logged as an observation, not a claim. <!-- claim:N3-5 -->

## 4. Mechanism constraints

Two pre-registered follow-ups constrain candidate mechanisms for the trough deviation. Both are
bounds/nulls; neither explains it.

**PR-2: analytic-rank split (null).** Pre-registered `f7415a4` (the contrast threshold fixed
before any curve was split) $\to$ read `876999f`. Excising the analytic-rank-2 subpopulation leaves
the trough position unchanged. The pre-committed recovery gate (leave-out trough moves < 1
bin, on the full family and on a virgin conductor annulus) is not met, so the deficit is not
carried by rank-2 over-representation. A secondary contrast (rank-2 curves carry an enhanced
downward bias) is significant but too dilute to move the location gate; the two gate outcomes are
coherent. <!-- claim:N4-1 --> *Wachs clause (verbatim):* this design does not distinguish rank per
se from BSD invariants (Tamagawa product, \|Ш\|, real period) correlated with rank; "carried by"
is a statement about the subpopulation, not the mechanism. <!-- claim:N4-4 -->

**PR-3: root-number-imbalance leakage (bounded out).** Pre-registered `21060a0` $\to$ read `0d21b62`
(the $2^{18}$ row appended with the Rung-3 run, `8f64ba1`). With
$\delta = (n_+ - n_-)/\lvert\mathrm{fam}\rvert$ the root-number imbalance, $U$ the unsigned bias,
leakage $L = \delta\cdot U$, and $f = L/\text{departure}$ at the trough:

| $X$ | $\delta$ | leakage $L(u^*)$ | $f = L/\text{departure}$ | sign vs. deficit |
|---|---|---|---|---|
| $10^4$ | $-0.0534$ | $-0.0184$ | $+0.0046$ | same |
| $2^{16}$ | $-0.0115$ | $+0.0037$ | $-0.0009$ | opposite |
| $2^{17}$ | $-0.0078$ | $+0.0025$ | $-0.0006$ | opposite |
| $2^{18}$ | $-0.0103$ | $+0.0032$ | $-0.0008$ | opposite |

$\lvert f\rvert \le 2\%$ at every rung, it is opposite-signed (points *against* the deficit) at the
three reliable rungs, and it does not grow with $X$. Leakage of Sutherland's parity-independent
bias through the measured imbalance is bounded out as a material contributor ($f \approx 1$ would
require the unsigned bias about 3 orders larger than measured). A citable null; a bound, not an
explanation. <!-- claim:N4-2 -->
*Sutherland-bias attribution (verbatim):* the parity-independent unsigned bias, the named
congruence classes, and its cancellation in the signed average are Sutherland's (Sept-2023 talk);
PR-3 tests only the leakage *relationship*, which is PR-3's hypothesis, not Sutherland's claim. <!-- claim:N4-5 -->

**Congruence stratification: not run.** PR-3 fixed in advance the condition under which its
more expensive follow-up — a congruence-stratified analysis of the same primes — would be
unnecessary: the leakage fraction $f$ small, opposite-signed, and with no structured residual at
the named congruence classes. The result above meets that condition, so the stratified pass was not
run, and no separate pre-registration for it was written. <!-- claim:N4-3 -->

The tail deficit remains unexplained by either constrained mechanism. What could carry it is future
work (§6).

## 5. Reproducibility

**A byte-identical published curve.** The emitted density JSON is byte-for-byte identical across
compilers and platforms. The density evaluator and its emitter (`src/murm/ss_density.cpp`,
`src/emit/emit_sawin_sutherland.cpp`) are compiled with fused-multiply-add contraction disabled
(`-ffp-contract=off`), so their IEEE operations round the same way under GCC and clang. A freshness
check re-emits the snapshot at the current commit and requires byte-equality with the committed
file, on CI's GCC as well as a local build. <!-- claim:N5-1 -->

**Integer $a_p$ is platform-independent.** A pre-registered, tail-weighted sample of 79,268
(curve, prime) pairs, with primes up to about 4.15 million, was computed by the frozen referee
`ap_charsum` on the laptop (`g++-16`, macOS) and on the compute box (`clang 21.1.8`, FreeBSD 15.1).
The two outputs are byte-identical, with zero mismatches. This sample corroborates on the $2^{17}$
family; the primary evidence that $a_p$ is platform-independent is the exact agreement of the three
$a_p$ algorithms over the full $2^{16}$ and $2^{17}$ grids together with the byte-identical emit
above. <!-- claim:N5-2 -->

**The raw partials are the deliberate exception (the contrast).** Contraction is off for the
density path but left at the compiler default for the empirical partial-sum generator
(`src/murm/ss_empirical.cpp` is outside the `-ffp-contract=off` list). So the raw per-curve partial
sums drift across toolchains while the emitted density does not. Measured on the shared 8640-curve
$2^{17}$ set, the drift has maximum size $1.887 \times 10^{-15}$, the last bit of a value near one. It does not
reach the published curve, and byte-identity of the emitted density is established empirically by
the freshness check above, not by rounding alone. The precision gap explains it: the emitter writes
each value to twelve significant figures (`std::setprecision(12)`, `src/emit/emit_sawin_sutherland.cpp:28`),
roughly three orders of magnitude coarser than the drift at the density's order-one scale, so a
difference this small falls below the last written figure except at a rounding boundary, which the
freshness check would catch. <!-- claim:N5-3 -->

**Errata and inputs.** Corrections are logged as numbered errata entries in the research spec and
the pinning notes, not edited away. Everything the note's numbers depend on is committed, including
the from-scratch code, the oracle $N$/$\epsilon$ caches, the per-curve partials, and the
`at ap-sample` subcommand. The one input that cannot be regenerated byte-for-byte is the abandoned
laptop $2^{17}$ partials checkpoint, since it is itself the float-drift artifact; it is
force-committed through a scoped `.gitignore` exception (`data/m5/ss_partials_x131072.txt.ckpt`),
not left to regeneration. The large $a_p$ caches (regenerable by `at ap-cache`) and the
cross-platform sample outputs
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

**A scale caveat with a documented precedent.** Our computable range, $X \le 2^{18}$, is small and
finite. Sutherland's September 2023 talk gives a precedent for reading such a range cautiously. One
formulation of BSD makes the Mestre–Nagao sum
$\lim_{x\to\infty} \frac{1}{\log x} \sum_{p\le x} a_p(E)\,\frac{\log p}{p} = -r + \tfrac12$
(conditional: Kim–Murty 2023 show that if this limit exists, it equals that value and the Riemann
hypothesis holds for $L(E,s)$), and curves of large rank necessarily have large conductor. In
Sutherland's rank-stratified plot of that sum, the ordering by rank emerges only at very large
conductor. A picture that looks settled at the conductors we can reach need not be the asymptotic
one. <!-- claim:N6-2 -->

**A gate for going further.** Across the extension rungs the trough depth eases while its position
holds (§3). Whether that easing is the amplitude decay the large-$X$ picture predicts is not
something our position-gated rule tests. A pre-registered amplitude-trajectory gate (PR-5), a
decision rule on the trough's depth trajectory fixed before the data, is the condition under which
rungs beyond $2^{18}$ would be added. It is named here, not run. <!-- claim:N6-3 -->

**Register, restated once.** The verdict of §3 is H0 over the finite range $X \le 2^{18}$: the
trough displacement is persistent there. It is not a claim about the $X \to \infty$ limit [SS25]
describe, and nothing in this note bears on that limit. <!-- claim:N6-4 -->

---

<!-- Deliverable boundary: §1 + §2 are drafted; §3–§6 are stubs. STOP for review (R4). -->
