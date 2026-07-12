# LOOKS ledger — every statistic computed in exploration

RESEARCH-M §7. One line per *statistic computed* in exploration (date, statistic,
family, outcome-in-one-clause). The denominator of looks is what makes any surprise
interpretable; exploration without logging is the quiet form of the pattern-fitting
failure mode. A `prereg_` confirmation is NOT a look — it is a bound, pre-registered
run recorded in its PR doc. Register: exploration outputs are *observed (exploratory)*;
only a passed `prereg_` may be *finding (pre-registered)*.

| # | date | statistic | family / region | outcome (one clause) |
|---|------|-----------|-----------------|----------------------|
| L1 | 2026-07-10 | statistic (1): ε·a_p averaged, binned by u=p/N, two-pass shape | height family X=3000 (396 curves), live-PARI N/ε | **UNOFFICIAL confirmation-class PEEK** (ERRATA #26, quarantined): shape reproduced — hump 0.463 / zero 0.67 / trough 0.812; sets NO tolerance |
| L2 | 2026-07-11 | statistic (1), committed extractor, Δu=0.025 | height ladder X∈{4000,6000,8000,10⁴}, cached N/ε | M4 confirmation (m4-pinning): hump+zero within a-priori τ=0.06; **trough OPEN DEVIATION** (flat ≈0.887); zero-dev monotone-increasing (0.0226→0.0279) |
| L3 | 2026-07-11 | windowed[0.7,0.9] trough (same two-pass extractor) | X=10⁴ confirm curve | localization discriminant: windowed == global (0.8875) ⇒ displacement real, tail-argmin hypothesis **refuted** |
| L4 | 2026-07-12 | analytic rank (PARI `ellanalyticrank`, prec 38) per curve; f₂ = analytic-rank-2 fraction | height family X=2¹⁶ (all 5042), rank cache | **PR-2 step 1** (pre-declared look, sets the step-2 threshold): **f₂ = 738/5042 = 0.14637**; dist r0=1754 r1=2525 r2=738 r3=25; analytic-rank parity vs ε **0 mismatches / 5042** (functional-equation-sign theorem check) |

Notes:
- L1 is the peek reclassified in ERRATA #26 — logged here for the denominator, not as
  evidence. L2/L3 are the M4 confirmation (its own committed record in m4-pinning §"the
  confirmation run"); logged here because they are the exploration that forms PR-1's
  hypothesis.
- PR-1's hypothesis (the trough deviation is a finite-X effect decaying at larger X) is
  formed from L2 + [SS25]'s "the bias seems to decay as X→∞"; its confirmation runs on a
  disjoint holdout (X ≥ 2¹⁶, never touched by L1–L3).
