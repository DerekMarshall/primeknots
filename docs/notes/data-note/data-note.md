<!-- DATA NOTE. Gate OPEN (Derek's word, 2026-07-17). §1–§6 and the Abstract are drafted and
     refereed section-by-section (rule R4); the note is finalized pending Derek's full
     Checkpoint-B read (nothing merges before it).

     VENUE (reconciled, not silent): the base prompt said "~8pp arXiv math.NT"; the venue
     amendment (2026-07-17) SUPERSEDES the venue → REPO PUBLICATION rendered on the GitHub
     Pages site (same pipeline as docs/deck/), PDF as a build product, Markdown source (not
     LaTeX-first). KEPT unchanged: the ~8pp discipline, the §1–§6 section plan, and rules
     R1–R5. Dropped: arXiv-only packaging (subject class, abstract-form limits). See README.md.

     Numbers/quotes are from cited sources or emitted artifacts only (rules 6/8; R3). Every
     nontrivial sentence carries a `claim:Nx-y` marker (an HTML comment) with a row in CLAIMS-N.md. -->

# A pre-registered replication of the Sawin–Sutherland height-ordered murmuration

**Author:** Derek Marshall (ORCID [0009-0001-8101-3908](https://orcid.org/0009-0001-8101-3908)). <!-- authorship block — Derek's decision: sole author, title final -->

**Venue:** repo publication (GitHub Pages), PDF build product. <!-- venue block — Derek's decision -->

**Methods disclosure.** The research design, the mathematical decisions, the pre-registrations,
and all review are the author's. The C++ implementation, the documentation drafts, and the
execution of the analyses were done by an AI assistant (Anthropic's Claude) under the author's
direction, following the repository's working rules: dual independent implementations, oracles
as referees only, decision rules committed before the data they judge, no constant taken from
memory. The author verified the artifacts and is responsible for this record.

---

## Abstract

We re-derive the Sawin–Sutherland height-ordered elliptic-curve murmuration from scratch and probe
it against their conjectured Bessel-$J_1$ density over a four-rung height ladder,
$X = 10^4~(\approx 2^{13.3}),\, 2^{16},\, 2^{17},\, 2^{18}$. An initial comparison showed an apparent
persistent trough displacement of $\approx 0.0825$ against a density target of $0.805$. A
pre-registered audit round traced that displacement to a transcription error in our reading of the
density's defining equation: the two local-factor products were transposed and one exponent mis-set,
which had mislocated the conjectured trough. We log the correction as erratum #28. Corrected, the
three shape invariants fixed in advance (the positive hump, the first post-hump zero, and the trough)
agree with the density within the a-priori tolerance $\tau = 0.06$, the largest residual
$\approx 0.012$ at the trough. Two pre-registered follow-ups bound candidate mechanisms: excising the
analytic-rank-2 subpopulation does not move the trough, and root-number-imbalance leakage is
$\le 0.5\%$ at the reliable rungs. Every result is numerical over a stated finite range, twinned or
oracle-refereed, and reproducible across compilers and platforms; we claim replication and
constraint, not proof.

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
  D(u) &= 2\pi\sqrt{u} \sum_{\substack{q\in\mathbb{N}\\ \text{squarefree}}} \sum_{m\in\mathbb{N}}
    \frac{\mu(\gcd(m,q))}{q\,m\,\varphi\!\left(q/\gcd(m,q)\right)} \\
  &\hphantom{{}={}} \times\, J_1\!\left(4\pi\sqrt{u}\,m/q\right)
    \prod_{p\mid q} \hat{\ell}_{p,2v_p(m)}
    \prod_{\substack{p\mid m\\ p\nmid q}} \ell_{p,2v_p(m)},
  \end{aligned}
  \tag{2}
  $$

  where $u = p/N$. Here $J_1$ is the Bessel function of the first kind, $v_p$ the $p$-adic
  valuation, $\mu$ the Möbius function, and $\varphi$ Euler's totient; the local factors
  $\ell_{p,\nu}$ (their Lemma 3, positive $\nu$) and $\hat{\ell}_{p,\nu}$ (their Lemma 4,
  even $\nu \ge 0$) carry the paired products — $\hat{\ell}$ over $p \mid q$ and $\ell$ over
  $p \mid m,\, p \nmid q$, both at exponent $2v_p(m)$. An earlier reading of this line
  transposed the two products and mis-set the $p\mid q$ exponent; the correction (erratum
  #28) is the subject of §6. <!-- claim:N1-2 -->
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
zero crossing after it, and the trough (§3). <!-- claim:N1-6 --> Two pre-registered follow-ups then
bound candidate mechanisms for the residual departure of the empirical curve from $D(u)$ (§4).

**What this note does not test.** It makes no asymptotic ($X \to \infty$) claim. Our computable
range, $X \le 2^{18}$, sits at the bottom of the window over which [SS25] observe the bias and
expect it to decay, naive height $2^{16}$ to $2^{28}$, reached only through sub-linear point
counting. Every finite-range statement below concerns that range, not a limit; the note makes no
persistence claim (§3). <!-- claim:N1-7 --> The
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

*Empirical: a binned estimator of [SS25] statistic (1).* For a prime-ratio window $(C_1, C_2]$
(half-open, left-open and right-closed, as in [SS25] p.2 — see §5 and erratum #30),
[SS25]'s statistic (1) is

$$
\mathbb{E}_{\{E\,:\,H(E)\le X\}}\!\left[
  \frac{\log\!\left(N(E)^{(C_1+C_2)/2}\right)}{N(E)}
  \sum_{\substack{p/N(E)\in(C_1,C_2]\\ p\ \text{prime}}}
  \epsilon(E)\,a_p(E)
\right]. \tag{1}
$$

The two displayed equations keep [SS25]'s numbers, (1) and (2); the binned estimator $\hat{D}$
below is ours and is left unnumbered.

We evaluate it as a density on $(0,1]$: partition into 40 bins of width $\Delta u = 0.025$, take each
bin $b = (b\,\Delta u,\, (b+1)\Delta u]$ as the window (so its midpoint
$(C_1+C_2)/2 = u_{\mathrm{mid}} := (b+\tfrac12)\,\Delta u$), and divide by $\Delta u$:

$$
\hat{D}(u_b) = \frac{1}{\Delta u\,\lvert\mathrm{fam}\rvert}
  \sum_{(E,p)\,:\,p/N(E)\,\in\,\mathrm{bin}_b}
  \left(u_{\mathrm{mid}}\,\frac{\ln N(E)}{N(E)}\right)\epsilon(E)\,a_p(E),
$$

over good primes $p > 3$ with $p \nmid (4A^3 + 27B^2)$ and $p \le N(E)$ (so $u \le 1$). The
$u_{\mathrm{mid}}$ factor is eq (1)'s $(C_1+C_2)/2$ normalization exponent evaluated at the bin
midpoint, not a per-pair weight, so $\hat{D}$ is a binned density estimator of statistic (1), one bin
per window. <!-- claim:N2-3 --> For this family the reduced short model is $p$-minimal at every
$p > 3$, so $p \nmid \Delta(\text{model}) \iff p \nmid N(E)$ (good reduction), and this exact
good-prime test keeps $a_p$'s domain computed-only. <!-- claim:N2-4 -->

*Conjectured: [SS25] eq. (2) / Conjecture 1.* [SS25] conjecture that the $X \to \infty$ limit of
(1) equals $\int D(u)\,du$, where $D(u)$ is the Bessel-$J_1$ density transcribed verbatim in §1.
eq (1) is a definition we evaluate; eq (2)'s equality to the limit is the *conjecture*. $\hat{D}$ is
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
*generated* constants (never typed), truncated at $q, m \le 2000$. <!-- claim:N2-12 --> Its reading
of eq (2) is transcribed from the [SS25] PDF, not from an internal quote — the standing rule that a
transcription check terminates at the source artifact, adopted after erratum #28 (§6). <!-- claim:N2-16 -->
The local factors carry level-1 Hecke eigenvalue sums
$\sum_f a_p(f) = \operatorname{Tr}(T_p \mid S_{\nu+2})$, which we compute by a from-scratch
Eichler–Selberg trace reusing the M3 Hurwitz class numbers, so no oracle-provenance constant enters
the density's core (rule 3); the trace is anchored on the published $\operatorname{Tr}(T_p\mid S_{12}) = \tau(p)$.
The density is checked at two levels: term-level agreement on hand-picked structural survivors (the
$m = 32$ term through $\operatorname{Tr}T_2$, the $p = 3$ branch through $m = 3^5$, and
$\gcd(m,q) > 1$ cases exercising the Möbius factor), and curve-level agreement of the whole density.
The C++ evaluator and an independent PARI/GP oracle built from the same Lemmas are two codings of
one reading, so their agreement alone does not certify that reading; the external referee is the
authors' published figure, whose trough the corrected density matches at $u \approx 0.87$ — the
reconciliation recorded as erratum #28 (§6). <!-- claim:N2-17 --> Because
every value is built from IEEE operations, generated constants, and the in-house Bessel, the
emitted curve is byte-portable across compilers and platforms, a property we verify (§5). <!-- claim:N2-13 -->

---

## 3. Results

**The ladder.** Over four height cutoffs the three shape invariants fixed in advance — the positive hump,
the first post-hump zero, and the trough — agree with the conjectured density $D(u)$ within the
a-priori tolerance $\tau = 0.06$. The invariants are stable across the ladder: the binned hump holds
at $u = 0.4625$ and the binned trough at $u = 0.8875$ at every rung, and the first zero stays near
$u \approx 0.671$. All values are from the committed `data/m5/ss_x{65536,131072,262144}.txt` runs and
the M4 run `data/m4/ss_empirical.txt`:

| $X$ | $\lvert\mathrm{fam}\rvert$ | hump u | first zero u | trough u | trough depth |
|--:|--:|--:|--:|--:|--:|
| $10^4$ | 1048 | 0.4625 | 0.672894 | 0.8875 | $-3.47$ |
| $2^{16}$ | 5042 | 0.4625 | 0.670328 | 0.8875 | $-3.72$ |
| $2^{17}$ | 9014 | 0.4625 | 0.673202 | 0.8875 | $-3.65$ |
| $2^{18}$ | 15936 | 0.4625 | 0.671945 | 0.8875 | $-3.58$ |

(Fig. 1 overlays all four empirical rungs on the conjectured $D(u)$ — each rung a distinct marker —
and is regenerated from these committed runs by `figures/make_figures.py`.) <!-- claim:N3-1 -->

**Both estimators, at the verdict rung.** The bin width $\Delta u = 0.025$ sets the resolution of
the binned argmin/argmax. A parabolic interpolation through the extremal bin and its two neighbours
(linear for the zero crossing) gives a sub-bin estimate, and a bootstrap over the curves — the
exchangeable unit, $N_{\text{boot}} = 2000$, seed $20260718$ committed with the study — gives a 95%
confidence interval. At the top rung $X = 2^{18}$, per invariant, against the corrected targets:

| invariant | target | binned u | interpolated u | 95% CI | deviation |
|:--|--:|--:|--:|--:|--:|
| hump | 0.465 | 0.4625 | 0.47064 | $[0.46699,\ 0.47696]$ | $+0.0056$ |
| first zero | 0.671 | 0.671945 | 0.67194 | $[0.66567,\ 0.67715]$ | $+0.0009$ |
| trough | 0.870 | 0.8875 | 0.88216 | $[0.87611,\ 0.88567]$ | $+0.0122$ |

The deviation is the interpolated position minus the target; the residual statements below ride the
interpolated column. <!-- claim:N3-2 -->

**Uniform reading.** All three invariants agree with $D(u)$ within $\tau = 0.06$. The first zero
sits on target: its CI $[0.666,\ 0.677]$ contains $0.671$. The hump and trough carry small resolved
residuals — $\approx 0.006$ and $\approx 0.012$ — whose confidence intervals exclude their targets
marginally (hump: lower bound $0.467$, just above $0.465$) and clearly (trough: lower bound $0.876$,
above $0.870$); these are resolved offsets, not open deviations, and both sit far inside $\tau$. The
largest residual, at the trough, is $\approx 0.012$, about a fifth of the tolerance. <!-- claim:N3-3 -->

**The pre-registered verdict, superseded.** Before the ladder was extended, PR-1 committed a
single decision rule for the trough (`dd6beb0`; the $2^{18}$ Rung-3 clause `4a17ebe` committed
*before the $2^{17}$ rung was read*). With $d(X) = \lvert \mathrm{trough}_u(X) - 0.805\rvert$ and
$\Delta u = 0.025$, it declared the displacement finite-$X$ if $d$ recovered by $\ge \Delta u$ at
both ladder steps and persistent if $d$ stayed flat. The rule was applied verbatim: $d = 0.0825$ at
$2^{16}$, $2^{17}$, and $2^{18}$, flat, with no recovery, so it returned H0 (persistent), read at
`8f64ba1`. The audit round then found the target it gated on to be corrupt: $0.805$ was our
mislocated density trough, not [SS25]'s, the eq (2) transcription error of erratum #28 (§6). The
verdict is therefore void as pronounced: the rule was committed and followed honestly, but the target
it gated on was wrong. We do not re-run it against the corrected target $0.870$, a number it was
never registered against. The corrected comparison above is descriptive, and claims no persistence
verdict. <!-- claim:N3-4 -->

**Supporting observation (not a gate).** The trough *depth* eases across the three extension rungs
($-3.72 \to -3.65 \to -3.58$ at $2^{16} \to 2^{17} \to 2^{18}$) while its position holds; the $10^4$
depth ($-3.47$) is shallower than $2^{16}$, so this is a three-rung easing, not a monotone four-rung
trend. It has the sign a large-$X$ amplitude decay would carry, but the bootstrap depth intervals
overlap heavily across the three rungs, so the easing is a point-estimate trend within sampling
noise, not a bootstrap-resolved one. We log it as an observation. <!-- claim:N3-5 -->

## 4. Mechanism constraints

Two pre-registered follow-ups were designed to probe candidate mechanisms for the trough deficit:
its *position* (PR-2) and the *amplitude* departure at the trough bin (PR-3). After the eq (2)
correction the position residual is the small resolved offset of §3, and the amplitude departure is
itself modest: the empirical $S(u^*) = -3.58$ against the corrected $D(u^*) = -2.94$ at $2^{18}$ is a
departure $\approx -0.64$ in density units, of the same order as [SS25]'s own per-bin discrepancy at
this height (§6). Both follow-ups stand as bounds; neither subpopulation accounts for the residual.

**PR-2: analytic-rank split (measurement stands; coded verdict superseded).** Pre-registered
`f7415a4` (the contrast threshold fixed before any curve was split) $\to$ read `876999f`. Excising
the $738$ analytic-rank-2 curves leaves the trough *position* unchanged — the leave-out trough sits
at $u = 0.8875$, identical to the full family, and holds on the geometric-holdout conductor annulus
$(10^4, 2^{16}]$; the position is not sensitive to the rank-2 subpopulation. That measurement stands.
The pre-committed *recovery gate* that read it, however, asked whether the trough recovered toward
$0.805$: it gated on the corrupt target, so its verdict is superseded with the rest of the $0.805$
chain (§3, erratum #28), while the leave-out measurement it was computed from is untouched. A
secondary contrast — the rank-2 curves are markedly more negative on the descending branch (mean gap
$-1.73$, past the committed threshold $-0.668$) — is a real value-space effect, significant but too
dilute to move the position; retained as a measurement. <!-- claim:N4-1 --> *Wachs clause (verbatim):*
this design does not distinguish rank per se from BSD invariants (Tamagawa product, \|Ш\|, real
period) correlated with rank; "carried by" is a statement about the subpopulation, not the
mechanism. <!-- claim:N4-4 -->

**PR-3: root-number-imbalance leakage (bounded out).** Pre-registered `21060a0` $\to$ read `0d21b62`
(the $2^{18}$ row appended with the Rung-3 run, `8f64ba1`; the leakage table regenerated against the
corrected density by `at ss-leakage`). With $\delta = (n_+ - n_-)/\lvert\mathrm{fam}\rvert$ the
root-number imbalance, $U$ the unsigned bias, leakage $L = \delta\cdot U$, the trough-bin amplitude
departure $\mathrm{dep} = S(u^*) - D(u^*)$, and $f = L/\mathrm{dep}$:

| $X$ | $\delta$ | leakage $L(u^*)$ | departure $\mathrm{dep}(u^*)$ | $f = L/\mathrm{dep}$ | sign vs. departure |
|--:|--:|--:|--:|--:|:--|
| $10^4$ | $-0.0534$ | $-0.0184$ | $-0.531$ | $+0.035$ | same |
| $2^{16}$ | $-0.0115$ | $+0.0037$ | $-0.776$ | $-0.005$ | opposite |
| $2^{17}$ | $-0.0078$ | $+0.0025$ | $-0.713$ | $-0.005$ | opposite |
| $2^{18}$ | $-0.0103$ | $+0.0032$ | $-0.641$ | $-0.005$ | opposite |

The leakage fraction is $\le 0.5\%$ and opposite-signed (points *against* the departure) at the three
reliable rungs, and $+3.5\%$ at $10^4$ (the smallest family, largest imbalance, no bootstrap); it
does not grow with $X$. Leakage of Sutherland's parity-independent bias through the measured
imbalance is bounded out as a material contributor: $f \approx 1$ would require the unsigned bias
about two orders of magnitude larger than measured. A citable null; a bound, not an explanation. <!-- claim:N4-2 -->
*Sutherland-bias attribution (verbatim):* the parity-independent unsigned bias, the named
congruence classes, and its cancellation in the signed average are Sutherland's (Sept-2023 talk);
PR-3 tests only the leakage *relationship*, which is PR-3's hypothesis, not Sutherland's claim. <!-- claim:N4-5 -->

**Congruence stratification: not run.** PR-3 fixed in advance the condition under which its
more expensive follow-up — a congruence-stratified analysis of the same primes — would be
unnecessary: the leakage fraction $f$ small, opposite-signed at the reliable rungs, and with no
structured residual at the named congruence classes. The result above meets that condition, so the
stratified pass was not run, and no separate pre-registration for it was written. <!-- claim:N4-3 -->

The small residual of §3 is left unexplained by either constrained mechanism — neither the rank-2
subpopulation (position) nor root-number leakage (amplitude) accounts for it. What could carry it is
future work (§6).

## 5. Reproducibility

**A byte-identical published curve.** The emitted density JSON is byte-for-byte identical across
compilers and platforms. The density evaluator and its emitter (`src/murm/ss_density.cpp`,
`src/emit/emit_sawin_sutherland.cpp`) are compiled with fused-multiply-add contraction disabled
(`-ffp-contract=off`), so their IEEE operations round the same way under GCC and clang. A freshness
check re-emits the snapshot at the current commit and requires byte-equality with the committed
file, on CI's GCC as well as a local build. The comparison first normalizes one field — the
`generated_by` provenance stamp, a `git describe` string that legitimately varies by checkout — and
then requires every remaining byte to match exactly; "byte-identical" below means identical after
that single-field normalization. <!-- claim:N5-1 -->

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
each value to twelve significant figures (`std::setprecision(12)`, `src/emit/emit_sawin_sutherland.cpp:29`),
roughly three orders of magnitude coarser than the drift at the density's order-one scale, so a
difference this small falls below the last written figure except at a rounding boundary, which the
freshness check would catch. <!-- claim:N5-3 -->

**Errata and inputs.** Corrections are logged as numbered errata entries in the research spec and
the pinning notes, not edited away. One convention correction post-dates the committed grids:
[SS25] eq (1) sums over the half-open prime interval $(C_1N, C_2N]$, and our binning had used the
opposite (left-closed) convention (erratum #30). The committed runs pre-date the fix; the only pairs
it touches are the two-to-three conductor-$40$ curves per rung whose good primes fall on a bin's
right edge — all at low $u$, enumerated in #30 — so no number this note reports changes, and a
from-scratch regeneration under the corrected convention would differ at exactly those pairs and
nowhere else (the diff is documented, not latent; the recompute is deferred as heavy and without
scientific effect). <!-- claim:N5-6 --> Everything the note's numbers depend on is committed, including
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
ctest --test-dir build -L m5 -LE heavy                # M5 suites, ~7 min on a laptop (excludes the heavy 2^18 gate)
./build/bin/at emit --stage m5      --out viz/data/   # empirical-vs-D(u) overlay JSON
./build/bin/at emit --stage m5split --out viz/data/   # rank-split overlay JSON
python3 -m pip install matplotlib                     # figure prerequisite (use a venv; not needed for the suites)
python3 docs/notes/data-note/figures/make_figures.py  # Fig. 1 and the rank-split figure
```

**The heavy gate, run separately.** The `-LE heavy` above excludes one long check,
`m5gate.twin_m0b_bruteforce_x18_tailweighted`, which recomputes on the order of $2.8\times10^7$
$a_p$ to twin the $2^{18}$ tail-weighted sample against the frozen referee. It runs about one to two
hours on the 48-core compute box (not on a laptop, not in CI) and is invoked explicitly with
`ctest --test-dir build -L heavy`.

**Regenerating the inputs, and the robustness grid.** The large $a_p$ caches regenerate with
`at ap-cache --X <H> --cache <ne_cache> --out <bin> --checkpoint <ckpt>` (hours on the compute box
for $2^{17}$/$2^{18}$; SHA-256 and byte counts pinned in `docs/notes/libm-partial-diff-spec.md`),
and the run/partials files with `at ss-run`. The referee-round robustness studies are runnable from
the committed data with no recompute: the density truncation scan (`at ss-density-scan`, seconds per
cutoff, about nine seconds at $B=2000$) and the bootstrap/interpolation study
(`python3 docs/notes/referee_b2b3.py`, under a second), both documented in
`docs/notes/referee-round-2026-07.md`.

The cross-platform `a_p` sample is reproduced by
`./build/bin/at ap-sample --X 131072 --cache data/m5/ne_cache_x131072.txt`; a single-platform run
suffices to verify the committed sample against its pinned SHA-256, and it is the byte-identity of
two such single-platform runs (laptop and compute box) that supplies the cross-platform evidence.
The pin lives in the reproducibility spec. <!-- claim:N5-5 -->

## 6. Discussion

**The corrected comparison in context.** Agreement with the conjectured density at the heights we
can reach is not automatic even for [SS25] themselves. Their Table 4 reports the mean per-bin
discrepancy $\lvert\text{empirical} - \text{density}\rvert$ of their own curve as $1.17$, $0.91$,
$0.73$ at $2^{16}$, $2^{17}$, $2^{18}$, decaying to $\approx 0.10$ only by $2^{28}$, twelve doublings
above our top rung. Our rungs sit at the bottom of that window, where the finite-$X$ curve is still
far from the limit in amplitude. At these heights the three shape-invariant positions land on
the corrected density within $\tau$ (§3), while the trough-bin amplitude departure we measure
($\approx 0.64$ at $2^{18}$, §4) is of the same order as [SS25]'s own per-bin figure. We read this as
positional agreement with an amplitude that has not yet converged, not as a discrepancy of the
replication. <!-- claim:N6-1 -->

**The eq (2) transcription error (erratum #28).** An earlier version of this note reported a
persistent trough displacement of $\approx 0.0825$ against a density target of $0.805$; that target
was our error, not the data's. Our reading of eq (2) had transposed the two local-factor products
(placing $\hat{\ell}$ on the primes $p \mid m,\, p \nmid q$ and $\ell$ on $p \mid q$) and mis-set the
$p \mid q$ exponent to $v_p(m)$ rather than $2v_p(m)$, moving the density trough from its true
position near $0.87$ to a spurious $0.805$. The error was shared. It lived in a pinning note, in the
C++ density evaluator built from that note, and in an independent PARI/GP oracle built from the same
quote: three internally consistent implementations encoding one reading, so their mutual agreement
certified nothing. What caught it was external: the authors' published density figure troughs near
$0.87$, and a blind re-transcription from the paper against that figure recovered the correct
products. The correction and its propagation across the code, data, and prose are erratum #28 and the
reframe registry it indexes. The standing rule it produced is that a transcription check terminates
at the source artifact, never an internal quote. <!-- claim:N6-2 -->

**Direction of the residuals.** The two non-zero position residuals of §3 are both rightward: the
hump at $+0.006$ and the trough at $+0.012$ (interpolated, empirical above target), the first zero
essentially on target. A single small horizontal dilation of the empirical curve relative to $D(u)$
would produce exactly this, one deformation rather than two independent offsets; it is the natural
finite-height reading and sharpens the one follow-up that remains. <!-- claim:N6-3 -->

**PR-4, retired.** An earlier draft named a pre-registered split by BSD invariant at fixed rank
(PR-4) as the route to whatever carried the trough deficit; the invariants in view (the Tamagawa
product, the analytic order of Ш, the real period) are ones Wachs shows modulate murmuration profiles
at fixed rank. With the deficit resolved to the small residual above, that motivation is gone, and
PR-4 is retired. <!-- claim:N6-4 -->

**PR-5, re-motivated.** The live question is whether the coherent rightward shift decays as $X$
grows; the amplitude easing of §3 and the positional dilation here are both consistent with the
expected finite-height convergence. PR-5 is a decision rule on that trajectory (position and depth
together), to be committed before any rung is added beyond $2^{18}$. It is the pre-registered gate
for going further, and has not been run. <!-- claim:N6-5 -->

**A scale caveat with a documented precedent.** Our computable range, $X \le 2^{18}$, is small and
finite. Sutherland's September 2023 talk gives a precedent for reading such a range cautiously. One
formulation of BSD makes the Mestre–Nagao sum
$\lim_{x\to\infty} \frac{1}{\log x} \sum_{p\le x} a_p(E)\,\frac{\log p}{p} = -r + \tfrac12$
(conditional: Kim–Murty 2023 show that if this limit exists, it equals that value and the Riemann
hypothesis holds for $L(E,s)$), and curves of large rank necessarily have large conductor. In
Sutherland's rank-stratified plot of that sum, the ordering by rank emerges only at very large
conductor. A picture that looks settled at the conductors we can reach need not be the asymptotic
one. <!-- claim:N6-6 -->

**Register, restated once.** The claim of this note is replication and constraint over the finite
range $X \le 2^{18}$: the three shape invariants agree with the conjectured density within $\tau$,
with a small resolved residual ($\approx 0.012$, largest at the trough), and two candidate mechanisms
are bounded out. It is not a claim about the $X \to \infty$ limit [SS25] describe, and nothing in this
note bears on that limit or proves anything. <!-- claim:N6-7 -->

---

<!-- All sections (§1–§6 + Abstract) drafted and reframed per erratum #28 (referee C, 2026-07-19);
     numbers from the corrected density (targets 0.465/0.671/0.870) and the committed runs. Finalized
     pending Derek's full Checkpoint-B read; nothing merges before it. See docs/notes/reframe-registry.md. -->
