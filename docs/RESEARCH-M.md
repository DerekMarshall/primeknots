# RESEARCH-M.md — Murmurations: Background, Ladder, and Protocol

Arithmetic statistics as an executable project, continuing the primeknots harness into territory where some rungs have no anchor. Murmurations: when L-function coefficients a_p are averaged over families ordered by an invariant (conductor, height), the averages trace coherent oscillating curves in p/N, correlated with root number. Discovered by machine-learning experiments on LMFDB data (He–Lee–Oliver–Pozdnyakov, 2022); since proven for several families, open for the original one.

## 0. Epistemic rules and scope

Rules 1–4 of RESEARCH.md §0 apply verbatim (map-not-source; "verified numerically" defined; [CONJECTURAL] flagged; [PIN TO SOURCE] means no improvisation). Additions for this project:

5. **Two modes, never blurred.** *Replication mode* (Stages M0–M4): published results, hard or soft anchors, the primeknots discipline unchanged. *Research mode* (Stage M5): open questions, **no anchors exist**, and the failure mode changes from convention-fitting to **pattern-fitting** — finding structure because enough statistics were computed that something had to look structured. Research-mode findings exist only under the pre-registration protocol (§7). Exploration is unrestricted and cheap; *claims* are expensive.
6. **Provenance labeling.** Some inputs are oracle-supplied data rather than computed (Cremona ranks, root numbers, conductors — see §2). Every emitted column carries `provenance: computed | oracle`, and no `theorem_` test may have its conclusion depend on an oracle-provenance column unless the test's stated purpose is exactly that cross-check.
7. **Scope, stated up front.** Stages M0–M4 verify known mathematics (2022–2025). The research mode's realistic best case is a clean, pre-registered empirical fact a number theorist could cite as data. No proofs are claimed anywhere in this project. n = 1 caveats from the primeknots method notes carry over.

## 1. The object

For a family 𝓕 of L-functions ordered by conductor N (or height), the murmuration is the behavior of averages like (1/|𝓕∩[N₁,N₂]|) Σ_{f} a_p(f) — or the same sum weighted by root number ε_f — as a function of p, for p up to ~N. Empirically (and in the proven cases, exactly) the average is asymptotically a function of the ratio **y = p/N** alone: scale invariance. The shape correlates with root number / rank parity: the rank-0-heavy and rank-1-heavy averages oscillate in antiphase. Proven cases to date all proceed by a trace formula or summation formula, averaged in the non-fixed family parameter; the original case (elliptic curves by conductor) remains open **[CONJECTURAL]**. Survey anchor for the field's state: Zubrilina [Z25] intro; Sawin–Sutherland [SS25] intro (which states precisely what is proven vs. observed); Sarnak's 2023 letter [Sar23] for the root-number framing.

## 2. Stage M0 — a_p machinery and data provenance

**Objects.** Elliptic curves E/ℚ in (short or long) Weierstrass form; the trace of Frobenius a_p = p + 1 − #E(𝔽_p) at good primes.

**Twins.**
- Naive frozen referee: direct point count / character sum. For p > 3 and a short model y² = x³ + ax + b: a_p = −Σ_{x mod p} ((x³+ax+b)/p), each Legendre symbol by the existing Stage-0 `legendre_euler`. Frozen after review.
- Fast path: per-prime batching — for fixed p build the quadratic-residue table once, then O(p) table lookups per curve; and/or Shanks–Mestre baby-step giant-step point counting for large p. Fast path evolves; referee doesn't.
- Oracle: PARI `ellap`, batched, sampled across the full (curve, p) grid.

**[PIN TO SOURCE] items (M0 pinning log, before code):**
- **Sign and normalization of a_p** — the classic off-by-one (point at infinity) and sign errors are silent and global. Pin against quoted values for a named small-conductor curve (e.g., 11a1) taken verbatim from Cremona's tables/LMFDB into the log — not from memory, not from this document.
- **Model reduction**: long → short Weierstrass transformation validity (p > 3), and minimality — a_p is an isogeny/isomorphism invariant at good p; `invariance_weierstrass_model` (same curve, different models, same a_p) is the structural guard.
- **Bad primes**: for M1 replication, follow what the pinned source (HLOP §2) actually uses at p | N — pin their convention (a_p ∈ {0, ±1} from the local factor) and take those values from the dataset, provenance-labeled, rather than implementing Tate's algorithm. **Tate at p = 2, 3 is this project's Rédei normalization and is explicitly deferred** to an optional later stage (M0b) with its own two-phase gate; do not implement it casually in M0.

**Data.** Cremona's ecdata (all E/ℚ with N ≤ 500,000; https://johncremona.github.io/ecdata/) is the primary dataset — canonical, downloadable in bulk, versioned. Pin the release tag and sha256 in the log. Fields consumed as **oracle-provenance**: conductor, rank, root number, bad-prime a_p, minimal model. Fields we compute: good-prime a_p (the point of M0). LMFDB bulk extracts fill in beyond Cremona's range if ever needed, cached per the primeknots data/ precedent. Papers are fetched by script into a gitignored docs/papers/ — the PDF-history lesson is a standing rule now, not a cleanup.

**Amendment — the oracle-provenance INPUT-DATA class (M4/C3, 2026-07-10).** Rule 6's provenance labeling covers *pinned-dataset* oracle inputs (ecdata: rank, conductor, root number, bad-prime a_p — a sha256-fixed release). Stage M4 introduces a genuinely new sub-class that this rule now names explicitly: a **live oracle (PARI) as a supplier of input data** the computed statistic *consumes*, because the height-ordered family runs outside any pinned dataset's conductor range (m4-pinning §P4). Concretely M4 takes each curve's **conductor N(E)** (`ellglobalred`/`ellinit`) and **root number ε(E)** (`ellrootno`) from PARI as input, then computes the a_p and the average from scratch. This is legitimate but carries a **residual risk the labeling must surface**: N and ε are *trusted*, not independently verified by us, so a systematic oracle error propagates silently into the statistic — unlike a *referee* oracle, whose disagreement would be caught. Conditions on using this class (M4 adopts all): (i) the field is `provenance: oracle` in every emitted column and the emitted `params` states the conditionality; (ii) a **dual-oracle overlap twin** exists — over the full curves that lie in BOTH the height family and Cremona's range, PARI's N and ε must match the pinned ecdata exactly, with a certified overlap count (this is the independent check that the live oracle is not lying); (iii) the from-scratch alternative (Tate's algorithm for N; local root-number formulas for ε) is recorded as a **deferred upgrade** (M0b-class), to be built if the residual risk ever needs retiring. The actual murmuration quantity (a_p) stays computed-provenance regardless.

**Amendment — the ANALYTIC-RANK oracle class (M5/PR-2, 2026-07-11).** PR-2 (rank-split
tail analysis) needs each curve's *rank*. [SS25] use "rank" in the BSD sense — loosely,
invoking it "conjecturally" (p.4: *"curves of larger rank … smaller a_p … conjecturally,
curves with rank 2 have root number +1"*); the paper gives no formal analytic-vs-MW
distinction. This project **pins the classifier to the ANALYTIC rank** — the order of
vanishing of L(E,s) at s=1 — and **never asserts the Mordell–Weil (algebraic) rank**
(which would be BSD-conditional). Concretely: the class is **analytic-rank**, obtained
from **PARI `ellanalyticrank(E)`** at a **named precision** (recorded in the cache
header), a numerically-determined oracle input (a new sub-class: unlike N and ε — exact,
unconditional PARI computations — the analytic rank is a *numerical* order-of-vanishing
determination that can be precision-limited). Conditions (PR-2 adopts all): (i)
`provenance: oracle (analytic rank, ellanalyticrank, prec=…)` on the column, params state
the numerical-determination conditionality; (ii) **mandatory parity cross-check** — the
analytic-rank parity must equal the cached ε's parity for **every** curve (ε=(−1)^{ord} is
the functional-equation sign, a *theorem*, so a mismatch flags a numerical
mis-determination, not a conjecture); a nonzero mismatch count aborts. (iii) every claim
branch says **"analytic-rank-2"**, never "rank 2" unqualified; the equality with MW-rank-2
is BSD and is *not* invoked. The a_p and the statistic stay computed-provenance.

## 3. Stage M1 — replicate the discovery [soft anchor]

Reproduce HLOP [HLOP22]: average a_p over curves of given rank (and separately, given root number) in fixed conductor ranges, plotted against p. **[PIN TO SOURCE]:** the exact conductor ranges, rank classes, prime ranges, and averaging conventions from the paper's §3 (dataset construction) — the figures are the anchor, so the family definition must match theirs exactly, quoted into the log.

The anchor is soft (published figures, plus any tabulated values the paper gives). The testable invariants, each a `theorem_`-style sweep with certified family counts:
- oscillation present; antiphase between even/odd rank classes;
- **scale collapse**: curves from disjoint conductor ranges [N, 2N] produce averages that collapse onto one curve in y = p/N (quantify: interpolated L² distance between range-pair curves below a stated tolerance — tolerance chosen and justified in the log *before* the run);
- robustness split: rank-classes vs root-number-classes give the parity-consistent relationship the paper describes.

Deliverables: the averaging pipeline (family filter → weighted average → binned curve in y) with every family count REQUIREd against an independently computed count from the dataset; `murmuration_curve.json` emission; viewer panel with HLOP-range presets.

## 4. Stage M2 — Dirichlet characters [hard anchor, fully from scratch]

Lee–Oliver–Pozdnyakov [LOP23] (IMRN 2025): murmurations for Dirichlet characters — provable by Gauss sums / Poisson summation, and computable here with **zero external data**: characters, conductors, and the averaged quantity all from scratch on top of Stage-0 machinery. **[PIN TO SOURCE]:** the family (which characters, primitive/parity classes, ordering), the averaged statistic, and the closed-form density from the paper, quoted with equation numbers. Then `theorem_murmuration_dirichlet`: empirical family averages converge to the published density over a certified sweep, with the convergence rate recorded as an empiric. This is the first rung where a murmuration density is verified against an exact formula, and it hardens the averaging pipeline on a family where everything is checkable.

## 5. Stage M3 — Zubrilina [hard anchor, the crown replication]

Zubrilina [Z25] (Invent. Math. 241 (2025) 627–680): for weight-2 newforms of squarefree level, root-number-weighted coefficient averages converge to an explicit murmuration density in y = p/N. Her proof runs through the **Eichler–Selberg trace formula**, whose class-number ingredients (Hurwitz class numbers) are computable by the primeknots Stage-3 binary-quadratic-forms machinery — the deepest code reuse in the project.

Plan: implement Tr T_p on the relevant newform spaces via Eichler–Selberg/Hijikata **[PIN TO SOURCE — hardest pinning job in the ladder; primeknots-Stage-4-grade convention risk in the local terms, elliptic/hyperbolic/parabolic contributions, old-to-new descent, and the root-number weighting]**; twin the traces against PARI (`mfcoefs`) and LMFDB samples before any averaging; then verify convergence of the averaged traces to the published density function, including its stated discontinuity structure. Two-phase gate mandatory: pinning log reviewed against the paper before the trace-formula TU is written. Anchor values: individual traces cross-checked against LMFDB dimensions/eigenvalue data; the density itself evaluated from the paper's formula in an independently authored TU (the Stage-4 separate-sessions protocol applies: empirical side and formula side never see each other's code).

## 6. Stage M4 — Sawin–Sutherland height ordering [replication: conjectured density + a proven variant]

**[CORRECTED — ERRATA #24; the earlier wording "proves an explicit murmuration density … via Voronoi-type summation / a proven anchor" was a spec-author overclaim.]** [SS25] gives an **explicit CONJECTURED** murmuration density (their **Conjecture 1**, an explicit Bessel-function sum) for elliptic curves ordered by **naive height** (not conductor), with the average taken over p/N in a fixed interval. What [SS25] **proves** (via Voronoi summation) is their **Theorem 2** — a *variant*: the smooth-weighted sum over n with no small prime factors, in a limit, equals the corresponding density integral. The prime-sum murmuration density itself is conjectural (m4-pinning §C2). Replication: build the height-ordered family from Weierstrass coefficients directly (the *ordering* is oracle-free from (A,B); but the *statistic* needs each curve's conductor N and root number ε — outside Cremona's range, so oracle-supplied, m4-pinning §P4/C3), average per their statistic (1), and demonstrate **empirical agreement with the conjectured density (Conjecture 1)** — an empirical replication of a conjecture, not a proof-check. Optionally, an empirical check of the proven Theorem 2 variant if computable at our scale. **[PIN TO SOURCE]:** their exact height definition, family weighting, and density formula (done: m4-pinning §P1–P5). This rung matters doubly: it is the last replication rung, *and* it establishes the ordering axis (conductor vs height) as a controlled, tested pipeline parameter — precisely the axis the research mode probes.

**Outcome (2026-07-11) — PARTIAL agreement, one open deviation (m4-pinning "confirmation run" §).** At the pre-named X_confirm=10⁴ (a-priori tolerance τ=0.06, derived not fit — the earlier 0.08 was quarantined post-hoc, ERRATA #26): the **hump** (dev 0.0125) and the **first zero-crossing** (dev 0.028) agree within τ — the murmuration and its defining oscillation replicate. The **trough** is an **OPEN DEVIATION** (dev 0.083 > τ): it is flat across the committed X-ladder {4000…10⁴} (so *not* demonstrated to be the [SS25] finite-X downward bias by this range), and the pre-registered windowed[0.7,0.9] trough equals the global one (so *not* a far-tail-argmin artifact). Emitted `claim_class` = "partial agreement with Conjecture 1; trough an open deviation." Not a proof; not full agreement — an honestly-flagged finding, faithful to what the computation produced.

## 7. Research mode — Stage M5 [OPEN — pre-registration mandatory]

Candidate questions, each requiring its own PR document before any confirmation run:
- **Q1 (elliptic vs modular density):** quantify the relationship between the empirical conductor-ordered elliptic-curve murmuration (M1 pipeline) and Zubrilina's weight-2 density (M3 formula side). Elliptic curves are a thin subfamily of weight-2 forms; whether/how the densities match under the appropriate normalization is, per [SS25]'s introduction, not settled by existing theorems. Output: a quantified, pre-registered match/mismatch statement with stated statistic and holdout.
- **Q2 (ordering dependence):** with M1 (conductor) and M4 (height) pipelines both trusted, characterize how the empirical density transforms between orderings on the *same* underlying curve population, against the proven height-side benchmark.
- **Q3 (one-level density / ratios):** Cowan [C24] connects murmurations to the Ratios Conjecture and one-level density. The primeknots Stage-5 zero machinery (Riemann–Siegel, Turing-certified counts) is the relevant instrument; scope any Q3 experiment tightly, as L-function zero computation for elliptic curves is new infrastructure.

**The pre-registration protocol (first-class, replaces anchors):**
- `docs/preregistered/PR-<n>.md`, committed **before** the confirmation run, containing: hypothesis; exact statistic (formula, binning, weighting); exact family definition (every parameter); exploration dataset used to form the hypothesis; **confirmation holdout** — a disjoint, named data region (e.g., a conductor range never touched in exploration) fixed in the document; decision rule and threshold with justification; the commit hash of the analysis code to be run. The PR doc's own commit hash is the timestamp.
- `prereg_` test category (new, sixth): a confirmation run bound to a PR document — the test loads the PR doc, verifies the code hash matches, runs the declared statistic on the declared holdout, and records PASS/FAIL against the declared threshold. A failed confirmation is a deliverable (recorded in the PR doc's postscript), never a license to adjust the threshold.
- **Looks ledger:** `docs/preregistered/LOOKS.md` — every statistic *computed* in exploration is logged (one line: date, statistic, family, outcome-in-one-clause). The denominator of looks is what makes any surprise interpretable. Exploration without logging is the pattern-fitting failure mode in its quiet form.
- Register rule: exploration outputs are labeled *observed (exploratory)*; only a passed `prereg_` confirmation may be labeled *finding (pre-registered)*; nothing in this project is labeled *proved* except cited literature.

**Commit-or-it-didn't-happen (all modes, not only M5).** Any pre-declared statistical design element — a **tolerance, a null control, a decision threshold** — is committed to git **before the run it governs**, in **replication mode too** (M1–M4), not just research mode. Uncommitted pre-registration is unenforceable. ERRATA #19 proved it: an M1 null was changed after the results were seen, and the git record could not corroborate the timing *in either direction* — precisely because the original design was never committed. The remedy is mechanical: the design lives in a committed file (the stage's pinning log and/or the test constants) before the confirmation run executes; a later change is a **logged deviation** (a postscript recording original → observed → replacement → timing), never a silent edit. The M2 convergence test is the first exercise of this rule.

## 8. Order of battle and reuse map

M0 → M1 → M2 → M3 → M4 → M5, gated as always. Reuse: `symbols/` (Legendre, everywhere), `classgroup/` (Hurwitz class numbers for M3), `zeta/` (Q3 only), the harness, CI, freshness, and the five test categories plus `prereg_`. Expected new load: data-scale engineering (M1's grid is ~10¹¹ Legendre evaluations naively; per-prime batching and the a_p cache in ARCHITECTURE-M §4 are the plan, and performance is a named workstream).

**Deferred stages (named, not built):**
- **M0b — fast a_p (sub-linear point counting).** The M0 a_p is O(p) per prime
  (`ap_charsum` referee, `ap_fast` table path). This caps the height-family statistic:
  PR-1's honest runtime estimate makes **X = 2¹⁸ infeasible (~22 days)** at O(p), and
  [SS25] reach 2²⁸ only via Sutherland's sub-linear counting. M0b = baby-step–giant-step
  (O(p^{1/2})) and/or Schoof–Elkies–Atkin / smalljac-class (O(p^{1/4})) a_p, with its own
  pinning + twin against the frozen `ap_charsum`. Motivated by, and blocking, the upper
  rungs of PR-1 and any X ≥ 2¹⁸ work. Not built.
- **M0b (conductor/root-number from scratch) — the Tate twin** (already noted in the §2
  M4/C3 amendment): Tate's algorithm for N + local root-number formulas for ε, to retire
  the oracle-input residual risk on N, ε. Not built.

## 9. References (arXiv IDs verified 2026-07-09 against live records)

- **[HLOP22]** Y.-H. He, K.-H. Lee, T. Oliver, A. Pozdnyakov, *Murmurations of elliptic curves*, arXiv:2204.10140; Experimental Mathematics 34 (2025), 528–540. **M1 normative.**
- **[LOP23]** K.-H. Lee, T. Oliver, A. Pozdnyakov, *Murmurations of Dirichlet characters*, arXiv:2307.00256; IMRN 2025, rnae277. **M2 normative.**
- **[Z25]** N. Zubrilina, *Murmurations*, arXiv:2310.07681; Inventiones Mathematicae 241 (2025), 627–680. **M3 normative.**
- **[SS25]** W. Sawin, A. V. Sutherland, *Murmurations for elliptic curves ordered by height*, arXiv:2504.12295. **M4 normative.**
- **[C24]** A. Cowan, *Murmurations and the ratios conjecture*, arXiv:2408.12723. Q3 context.
- **[BBLL23]** J. Bober, A. Booker, M. Lee, D. Lowry-Duda, *Murmurations of modular forms in the weight aspect*, arXiv:2310.07624. Context: the weight-aspect proven case.
- **[Sar23]** P. Sarnak, letter to Sutherland and Zubrilina on murmurations and root numbers, Aug 2023, publications.ias.edu/sarnak/paper/2726. Framing.
- **[Cre]** J. Cremona, ecdata (elliptic curve database, N ≤ 500,000), https://johncremona.github.io/ecdata/ — release tag + sha256 pinned in the M0 log. **Primary dataset.**
- Trace formula sources for M3: Hijikata, J. Math. Soc. Japan 26 (1974) (explicit Γ₀(N) traces); the exact statement used must be quoted from whichever source the M3 pinning adopts, with Zubrilina's own setup as the arbiter.
- Maass-form murmurations (Booker–Lee–Lowry-Duda–Seymour-Howell–Zubrilina) exist as a further proven case; ID to be pinned from the literature if that rung is ever added — not cited by ID here because it was not verified in this pass.
- Recent activity indicating the area remains live (2025–26 preprints on function fields and BSD-invariant murmurations) — context only, nothing here depends on them.
