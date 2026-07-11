# PR-1 — Is the M4 trough open-deviation a finite-X effect? (X-extension)

**Status: PRE-REGISTERED (committed before any confirmation rung runs). Mode: M5
research (RESEARCH-M §7). This is `prereg_`'s first real-stakes exercise.**

The document's own commit hash is its timestamp. No rung of the confirmation may run
before this file is committed; a later change to any pre-declared element below is a
logged deviation (postscript: original → observed → replacement → timing), never a
silent edit.

---

## Hypothesis

M4 found the empirical height murmuration's **trough** displaced to u ≈ 0.8875 (target
0.805, deviation 0.0825 > the a-priori τ=0.06), **flat** across the ladder
X∈{4000,6000,8000,10⁴} — verdict OPEN DEVIATION (m4-pinning "confirmation run" §). [SS25]
(p.3–4, quoted in m4-pinning) report a persistent downward bias that "seems to decay as X
goes to ∞." Their tested range is naive height 2¹⁶–2²⁸; the M4 ladder (≤10⁴ ≈ 2¹³·³) sits
**below** it.

> **H1 (finite-X):** the trough deviation is a finite-X effect; extending X into [SS25]'s
> regime (≥ 2¹⁶) makes the trough deviation **decrease** toward 0 (the trough migrates
> left toward 0.805).
>
> **H0 / alternative (persistent):** the deviation does **not** decrease over the
> extension — a persistent effect, to be escalated to PR-2 (rank-split tail analysis).

This is a directional test of a claim [SS25] make about their own data, on a family and
statistic we control end to end.

## Exact statistic (UNCHANGED from M4 — nothing tuned)

The M4 empirical statistic (1), `murm::ss_empirical`, run verbatim:
- ε(E)-weighted a_p(E) averaged over the height family, binned by **u = p/N(E)** over
  (0,1], bin width **Δu = 0.025**;
- per-bin D̂(u_b) = (1/(Δu·|fam|)) Σ_{(E,p): p/N∈b} (u_mid·ln N(E)/N(E))·ε(E)·a_p(E);
- a_p **computed from scratch** (`ell::ap_fast`, twinned vs the frozen `ell::ap_charsum`);
  good primes p>3 with p∤(4A³+27B²);
- N, ε **oracle-provenance input** (PARI `ellglobalred`/`ellrootno`), R3-certified via the
  dual-oracle overlap twin;
- shape via the committed **two-pass extractor** (`extract_shape`): global hump, first
  +→− crossing past the hump, global trough. **Same extractor, same τ, same Δu — none
  touched.** The extension changes **only X**.

**Analysis-code hash (binding):** the statistic is `murm/ss_empirical.cpp`,
sha256 = `b87ebd1e188e15dc2d3a1c2a54e5300dc3c7ff3d4adef3e77d5328b9ff526cde`
(= `SS_GENERATOR_HASH`, the committed `data/m4/ss_empirical.txt` header at HEAD `ce5f974`).
The confirmation refuses to run against any other hash (the `read_ss_run`/generator-hash
mechanism). If the statistic source changes before the run, this PR is void and must be
re-committed against the new hash.

## Exact family definition

E_{A,B}: y²=x³+Ax+B, integers A,B **reduced** (no prime p with p⁴|A ∧ p⁶|B),
**nonsingular** (4A³+27B²≠0), **naive height** H(E)=max(4|A|³,27B²) ≤ X. Enumerated by
`murm::height_family` (direct filter, twinned vs the inclusion–exclusion sieve, R4). N, ε
per curve from `at ne-cache` (committed cache, generator-hash-refused).

## Exploration dataset used to form the hypothesis

LOOKS L1 (the quarantined X=3000 peek), L2 (the M4 ladder X∈{4000,6000,8000,10⁴}), L3
(the windowed-trough discriminant). The hypothesis is formed from L2 + the [SS25] quote —
**not** from any data at X ≥ 2¹⁶.

## Confirmation holdout (disjoint, named)

**X ∈ {2¹⁶ = 65536, 2¹⁷ = 131072, 2¹⁸ = 262144}** — the extension rungs. Every one is
**disjoint** from the exploration (all of L1–L3 used X ≤ 10⁴ < 2¹⁶). No statistic has been
computed at any X ≥ 2¹⁶. These rungs are the holdout; the M4 ladder ≤10⁴ is the anchor
end of the trend, not part of the holdout decision.

## Decision rule and threshold (fixed here, before any rung runs)

The trough is **bin-quantized** (Δu=0.025; the global argmin lands on a bin center), so a
"decrease" is only real if it exceeds one bin. Let d(X) = |trough_u(X) − 0.805|.

> **VERDICT "consistent with SS finite-X bias" (H1)** iff, across the feasible extension
> rungs (in increasing X), d(X) is **non-increasing** AND the largest feasible rung has
> **d ≤ 0.0575** — i.e. the trough has moved at least **one full bin (Δu=0.025)** toward
> the target relative to X=10⁴ (where d=0.0825, trough_u=0.8875). One bin is the
> quantization floor: a smaller move is indistinguishable from bin noise.
>
> **VERDICT "persistent deviation" (H0)** otherwise (flat within quantization, or moving
> away). Consequence: **escalate to PR-2** (rank-split tail analysis). This is a
> deliverable, not a threshold to renegotiate.

**Supporting empiric (reported, NOT the gate):** the zero-crossing deviation is
*interpolated* (sub-bin) and was monotone-**increasing** over the M4 ladder
(0.0226→0.0279). Under H1 it should turn over and decrease at large X; under H0 it keeps
climbing. It is more sensitive than the quantized trough but is recorded as supporting
evidence only — the pass/fail gate is the trough rule above (the same discipline as M3:
teeth gate, L2 supporting).

## Partial-ladder clause — what ONE rung licenses (amended 2026-07-11, before Rung 1 data; R1)

The VERDICT language above ("consistent with SS finite-X bias" / "persistent deviation")
is a statement about **d(X) across the feasible ladder** — it needs ≥ 2 extension rungs
(a *trend*), because "non-increasing" and "moved ≥ 1 bin at the largest rung" are
ladder properties, not single-point ones. The first rung (2¹⁶) runs alone, on an
overnight budget; this clause fixes — **before any 2¹⁶ number exists** — exactly what
that single rung may and may not license, so no post-hoc reading can inflate one point
into the verdict. Let d(X) = |trough_u(X) − 0.805|; the X=10⁴ anchor is d=0.0825
(trough_u=0.8875).

> **A single rung is EVIDENCE-GRADE only, never verdict-grade.** Reading the {10⁴, 2¹⁶}
> pair yields exactly one of two *directional interim readings* — each says "keep going,"
> in a different direction — and **neither is the H1/H0 verdict:**
>
> - **Reading A — "consistent with finite-X, escalate":** d(2¹⁶) ≤ 0.0575 (trough moved
>   ≥ one full bin, Δu=0.025, toward 0.805) **and** d(2¹⁶) < d(10⁴). Licenses:
>   **run the next rung (2¹⁷)** to see whether the trend continues. Does **NOT** license
>   the H1 ("finite-X bias") verdict — one downward step is not a decaying trend; the
>   verdict waits for the ladder.
> - **Reading B — "consistent with persistent, proceed to PR-2":** anything else (flat
>   within the Δu quantization floor, or d increased). Licenses: **begin PR-2** (the
>   rank-split tail analysis), which is the H0 escalation path and does not itself need
>   H1's ladder. Does **NOT** license the H0 ("persistent deviation") verdict as
>   *final* — a fuller ladder (adding 2¹⁷ when budget allows) still gets appended, and
>   the verdict is pronounced there.
>
> **The asymmetry is deliberate and pre-committed:** one rung can send us *sideways to
> PR-2* (Reading B — the rank-split is worth doing on 2¹⁶-persistence evidence alone,
> and PR-2 has its own pre-registered gate), but one rung can **never** send us to a
> *finite-X "done"* (Reading A must be confirmed up the X-ladder first). This encodes
> that the burden for declaring the deviation *explained-away-as-finite-X* is strictly
> higher than the burden for opening the mechanistic follow-up.

The `prereg_ss_x_extension` test records **which interim reading fires** and pins the
observed d(2¹⁶); it does **not** emit a verdict from one rung. A FAIL-to-recover
(Reading B) is a deliverable that *advances* the program (to PR-2), exactly as the
committed decision rule intends — it is never a threshold to renegotiate.

## Runtime estimate per rung (stated up front — the ladder is NOT silently shrunk)

Calibration: X=10⁴ ran in **250 s** (1048 curves, 12 threads). Cost model
t ≈ 250·(|fam|/1048)·(X/10⁴)²·ln(10⁴)/ln(X) (per-curve a_p work ∝ N²/ln N with N∝X;
|fam| exact from the oracle-free sieve):

| rung | |fam| (sieve) | est. runtime | feasibility (current O(p) `ap_charsum`/`ap_fast`) |
|------|---------------|--------------|-----------------------------------------------------|
| 2¹⁶ = 65536 | 5042 | ≈ 11.9 h | **feasible** — an overnight background run |
| 2¹⁷ = 131072 | 9014 | ≈ 3.3 d | **costly but possible** — a multi-day committed run |
| 2¹⁸ = 262144 | 15936 | ≈ 22 d | **INFEASIBLE** with the O(p) charsum a_p |

**2¹⁸ is declared infeasible at current infrastructure, not silently dropped.** The
blocker is named: a_p is O(p) per prime (`ap_fast`, table charsum); [SS25] reach 2²⁸ only
via Sutherland's sub-linear point counting (smalljac / Schoof–Elkies–Atkin). Reaching
2¹⁸+ here requires an **M0b faster-a_p stage** (its own pinning + twins) — recorded as a
prerequisite, out of PR-1 scope.

**Execution plan (budget-gated):**
- **Rung 2¹⁶** runs first (overnight). If the decision rule already resolves at
  {10⁴, 2¹⁶} + 2¹⁷ (a clear monotone drop ≥1 bin, or a clear flat), report and stop.
- **Rung 2¹⁷** runs if budget allows and 2¹⁶ is ambiguous.
- **Rung 2¹⁸** is **blocked** pending the M0b a_p upgrade; the doc records this so a
  two-rung ladder is understood as *infrastructure-limited*, not a shrunk design. If only
  {10⁴, 2¹⁶, 2¹⁷} run, the verdict is taken over those rungs, with the 2¹⁸ gap stated.

## Confirmation mechanics (when it runs)

`at ss-run --X <rung>` per rung (each writes its own committed run file / cache),
extractor + τ + Δu unchanged; a `prereg_ss_x_extension` test loads THIS doc, checks the
analysis-code hash matches, applies the decision rule (and, for a lone rung, the
partial-ladder clause) to the committed extension shapes, and records the interim
reading / verdict in a postscript here. A FAIL-to-recover is a deliverable; the threshold
is never adjusted post-hoc.

**Execution discipline for Rung 1, pre-committed (R2, R3 — before any 2¹⁶ number):**

- **R2 — one cache, two consumers.** The 2¹⁶ run builds the a_p work and the N/ε cache
  **once** over the **full 5042-curve family**, with the standard committed-artifact
  headers: the statistic `generator_hash` (SS), **provenance: oracle** on the N/ε column
  (PARI `ellglobalred`/`ellrootno`, R3-class dual-oracle-overlap certified — the M4
  `oracle_dual_overlap_NE` twin), and the thread count (a_p summation order is
  thread-count-fixed; recorded for reproducibility). The **per-curve bin partials are
  persisted keyed by (A,B)** so PR-2 attaches only an **analytic-rank column** and
  re-aggregates subpopulations with **no a_p recomputation** (PR-2 Amendment 2). PR-2's
  mandatory **parity cross-check** (analytic-rank parity == cached ε parity; ε=(−1)^ord
  is the functional-equation sign, a theorem, so a mismatch is a numerical
  mis-determination) runs over **all 5042 curves** (not a sample); **any nonzero mismatch
  aborts and is itself a deliverable.** None of this touches the statistic, extractor, τ,
  holdout, or decision rule; it is emit-side I/O in the non-hash-bound TUs
  (`app/at.cpp`, `murm/ss_run_io.cpp`), so `SS_GENERATOR_HASH` is unchanged.

- **R3 — checkpoint without peeking.** The ~12 h run writes chunked intermediate partials
  to a **checkpoint file for crash-safety only** (resume skips completed (A,B) curves).
  Chunking by curves is **byte-identical** to a single pass at fixed thread count (each
  prime sits at a maxN-independent index → same thread → same accumulation order; locked
  by the `twin_ss_partials_chunked_vs_single` test). **Shape statistics are computed
  exactly ONCE, at completion, by the committed `extract_shape`.** Intermediate dumps are
  **quarantined from all analysis** — no mid-run inspection of any trough/zero/hump
  (the R0 lesson, scheduled edition: a confirmation is defined by the single completion
  computation, not by anything glimpsed while it ran).

## Postscript (results — appended only after the confirmation rungs run)

*(empty — no extension rung has run as of commit of this document)*
