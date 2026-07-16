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

### Two completed rungs — what {2¹⁶, 2¹⁷} license (amended 2026-07-12, before Rung 2 data; R0)

With **two** extension rungs in hand (2¹⁶ and 2¹⁷, over the 10⁴ anchor), the reading gains a
**direction** but is still **evidence-grade, not the verdict**. Let d(X)=|trough_u(X)−0.805|;
d(2¹⁶)=0.0825 (Rung 1, flat vs the anchor).

> - **"Strengthens finite-X"** iff the trough dev at 2¹⁷ drops **≥ one bin** relative to 2¹⁶
>   — d(2¹⁷) ≤ d(2¹⁶) − Δu = **0.0575**. (The sub-bin zero-crossing series is reported
>   alongside as supporting direction, never as the gate.)
> - **"Strengthens persistent"** iff d(2¹⁷) is **flat or rising** vs 2¹⁶ (within the Δu
>   quantization floor).
>
> **The committed H1/H0 verdict language still requires the full ladder** — two rungs
> sharpen the direction, they do not pronounce it.
>
> **Standing scale caveat, riding EVERY branch:** our feasible range (≤ 2¹⁷) sits at the
> **very bottom** of [SS25]'s observed-decay window **2¹⁶–2²⁸**; 2¹⁸ is infeasible without
> M0b (§Runtime). So a two-rung reading — in either direction — is **range-limited** and does
> **not** settle the X→∞ behaviour [SS25] describe. "Strengthens finite-X" is not
> "finite-X confirmed"; "strengthens persistent" is not "persistent proven." The verdict
> remains explicitly conditional on a range we cannot yet reach.

### Three completed rungs — what {2¹⁶, 2¹⁷, 2¹⁸} license (amended 2026-07-15, before Rung 2 data; Rung-3 / R0-ext)

**Pre-registration status.** Committed while the 2¹⁷ rung is still *running and unread* (the FreeBSD
canonical run is churning as this is written) and before any 2¹⁸ data exists. The 2¹⁸ threshold below
is therefore expressed **relative to the as-yet-unread d(2¹⁷)** — never a typed absolute — so no
rung's reading rule can be contaminated by a rung's result. This extends the R0 two-rung clause by one
rung; the prime directive (a threshold is never renegotiated post-hoc) governs it identically.

**Why 2¹⁸ is now reachable (reconciliation with §Runtime).** §Runtime declared 2¹⁸ "INFEASIBLE
(~22 d) with the O(p) charsum a_p, blocked pending M0b." That blocker is **infrastructure, not
mathematics**: a_p(E,p) is a uniquely-determined **integer**, identical whichever algorithm computes
it. The ~22 d estimate is retired either by **brute-forcing the embarrassingly-parallel O(p) a_p on a
48-core host** (~4 d; the box measured **5.6×** over the 12-thread calibration — 2¹⁶ in 2.1 h vs the
11.9 h estimate) **or** by the **M0b faster-a_p stage** (Shanks–Mestre; usable once its twin against
the frozen `ap_charsum` passes). The rung's data is **provenance-clean regardless of the compute
path**, and if M0b computes it, M0b's exact-match twin on the checksummed 2¹⁶/2¹⁷ a_p caches must pass first.
Rung 3 is thus a *hardware/algorithm* escalation — it does **not** touch the statistic, extractor, τ,
holdout, or decision rule.

Let d(X)=|trough_u(X)−0.805|; d(2¹⁶)=0.0825 (Rung 1, committed); Δu=0.025; d(2¹⁷) is the holdout.

> - **Rung-3 step reading — "strengthens finite-X":** iff d(2¹⁸) ≤ d(2¹⁷) − Δu (one further bin of
>   trough recovery beyond the 2¹⁷ rung). **"Strengthens persistent":** iff d(2¹⁸) is flat or rising
>   vs d(2¹⁷) within the Δu quantization floor. (The 4-point sub-bin zero-crossing series over the
>   anchor {2¹⁶, 2¹⁷, 2¹⁸} is reported alongside as supporting direction, **never** the gate.)
>
> - **Finite-range verdict (the full feasible ladder).** Three rungs over the 10⁴ anchor is the
>   **fullest ladder this infrastructure can build** — the ladder R0 reserved the H1/H0 verdict for.
>   With it in hand the verdict may be **pronounced as a finite-range statement (over ≤ 2¹⁸)**:
>     - **H1 (finite-X, ≤ 2¹⁸)** iff the trough recovers monotonically by ≥ Δu at **both** steps:
>       d(2¹⁷) ≤ d(2¹⁶) − Δu **and** d(2¹⁸) ≤ d(2¹⁷) − Δu — two consecutive bin-recoveries, a genuine
>       decaying trend, not a single step.
>     - **H0 (persistent, ≤ 2¹⁸)** iff the trough stays flat across the ladder (no ≥ Δu recovery at
>       either step) — Rung 1's Reading-B trajectory extended.
>     - **Ambiguous** otherwise (one step recovers, the other does not): reported as-is, no verdict.
>
> **Standing scale caveat, riding EVERY branch (unchanged — and it still bites at 2¹⁸).** Even 2¹⁸
> sits at the **very bottom** of [SS25]'s observed-decay window **2¹⁶–2²⁸** — ~2¹⁰× below their top,
> which they reach only via sub-linear point counting. Reaching 2¹⁸ by cores or by M0b/Shanks–Mestre
> does **not** change this: a "finite-range H1/H0 verdict (≤ 2¹⁸)" is **explicitly not** the X→∞
> verdict [SS25] describe. Every reading above stays conditional on a range we still cannot reach.

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

**[2026-07-15 update — infrastructure changed, estimate retired.]** A 48-core host retires the
~22 d wall by brute force (~4 d; measured 5.6× over this 12-thread calibration), so 2¹⁸ is no
longer infrastructure-blocked — see the Rung-3 clause above for the pre-registered reading and
the provenance argument (a_p is an integer, path-independent). The **M0b algorithmic upgrade
remains the right long-term route** (it, not cores, is how [SS25] reach 2²⁸) and is now in
Phase-1 pinning (`docs/notes/m0b-pinning.md`); the hardware route substitutes cores for it
without touching a_p. This estimate table is left as the original honest calibration.

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

### Rung 1 — X = 2¹⁶ = 65536 (ran 2026-07-11 → 2026-07-12)

**Outcome: Reading B — "consistent with persistent, proceed to PR-2" (evidence-grade).**
The gate (the Δu-quantized trough) did **not move one bin** over a 6.5× increase in X.

| X | \|fam\| | hump u (dev) | zero u (dev) | trough u (dev) |
|---|--------|--------------|--------------|----------------|
| 4000 | 522 | 0.4625 (0.0125) | 0.667642 (0.022642) | 0.8875 (0.0825) |
| 6000 | 664 | 0.4625 (0.0125) | 0.669584 (0.024584) | 0.9125 (0.1075) |
| 8000 | 870 | 0.4625 (0.0125) | 0.672637 (0.027637) | 0.8875 (0.0825) |
| 10000 | 1048 | 0.4625 (0.0125) | 0.672894 (0.027894) | 0.8875 (0.0825) |
| **65536** | **5042** | **0.4625 (0.0125)** | **0.670328 (0.025328)** | **0.8875 (0.0825)** |

**Decision rule / single-rung clause applied VERBATIM.** d(10⁴)=0.0825; d(2¹⁶)=**0.0825**
(trough_u=0.8875 at both — identical bin). The one-bin-recovery threshold is d ≤ 0.0575;
d(2¹⁶)=0.0825 does **not** meet it and is **not** below d(10⁴). Reading A does not fire.
**Reading B fires: consistent with persistent, proceed to PR-2.** This is EVIDENCE-GRADE
only — it licenses beginning the rank-split (PR-2), it does **not** pronounce the H1/H0
verdict, which requires the full ladder (2¹⁷ remains for verdict-strengthening; a spend
decision, below). The threshold was **not** adjusted; a FAIL-to-recover advancing the
program to PR-2 is exactly what the committed rule intends.

**The two agreeing invariants still agree at 2¹⁶.** hump dev 0.0125 < τ and zero dev
0.025328 < τ — the murmuration and its primary oscillation replicate at the larger scale.

**Consistency twin (a strong internal cross-check).** Aggregating the SAME 5042-curve
partials at H ≤ {4000,6000,8000,10⁴} reproduces the frozen M4 committed run **exactly** —
same \|fam\| (522/664/870/1048) and same shape fields to full precision (`hump_v`, `zero_u`,
`trough_v`, all). The 5042-curve pipeline reproduces M4; the extension is not a
re-parameterised artifact. (Pinned in `prereg_ss_x_extension`.)

**Supporting empiric — the interpolated zero-crossing (NOT the gate).** Over the M4 ladder
the zero dev rose monotonically (0.022642 → 0.027894); at 2¹⁶ it **retreated** to 0.025328,
**reversing direction**. Under H1 that is the finite-X-consistent sign — but it is the
*supporting* empiric, sub-bin and more sensitive than the quantized trough, and the
committed gate is the trough. Caveat stated: the trough's bin quantization (Δu=0.025) means
a sub-bin trough drift could be masked; the zero-crossing (interpolated) is where such
drift would show first, and it moved only 0.0026 — well inside plausible sampling jitter.
**Recorded as a first-class empiric; it does not override the trough gate**, so the verdict
stays Reading B. (Whether the *trough* is finite-X-biased below one bin is precisely what
2¹⁷ + the value-space analysis would resolve; open.)

**Crash + resume event (part of the run record).** The run was killed by an environment-level
reap at **2048/5042 curves** (~2.09 h in, mid-chunk-5). The R3 crash-safety checkpoint held:
a clean `complete=0` partials dump (no truncation, tmp+rename), from which the run **resumed
byte-identically** (chunk size dropped 512→256 to checkpoint more often; thread count held at
12, which the compat check enforces for byte-identity with the already-done curves) and
completed the remaining 2994 curves. The final partials are exactly what a single clean pass
would have produced (the property `twin_ss_partials_chunked_vs_single` locks, validated live
in the resume).

**Realized wall time per stage** (12 threads, this machine):
- N/ε oracle cache (5042 curves, PARI): ~0.1 s.
- **a_p statistic (the heavy stage): ~10.2 h** (7541 s to 2048 curves + 29207 s resume for
  2994) — **heavily back-loaded**: the high-conductor tail dominates (per-chunk cost rose
  from ~1000 s early to ~6000 s for the last chunks; max N = 2,037,232 = 31× X).
- aggregate + two-pass shape + emit: < 1 s (from the committed partials, no a_p recompute).

**Revised runtime estimates from the measured scaling** (power-law fit through the two
12-thread points (10⁴, 250 s) and (2¹⁶, 36748 s): total ∝ X^2.65 — below the a-priori
model's ~X², so PR-1's original figures were ~17% pessimistic):
- **2¹⁷ ≈ 2.7 d** (was 3.3 d a-priori) — *costly but possible*, a multi-day committed run.
- **2¹⁸ ≈ 17 d** (was 22.3 d) — still **INFEASIBLE** at current infrastructure; the **M0b
  fast-a_p stage remains motivated** (no change: 2¹⁸ stays out of reach without it).

**Analysis-code hash unchanged** (`b87ebd…` = `SS_GENERATOR_HASH`); `prereg_ss_x_extension`
PASSES (records Reading B, pins d(2¹⁶)=0.0825 and the M4 consistency). Committed artifacts:
`data/m5/ne_cache_x65536.txt`, `data/m5/ss_x65536.txt`, `data/m5/ss_partials_x65536.txt`
(the a_p work, persisted for PR-2 — no recompute), `viz/data/ss_x_extension_murmuration.json`.
