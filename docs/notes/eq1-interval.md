# eq (1) interval — half-open (C₁N, C₂N], and the p = C₂N audit

Referee C, 2026-07-19. [SS25] eq (1) sums over primes `p ∈ (C₁N(E), C₂N(E)]` — the interval is
**half-open: left-open, right-closed** (p.2). Our empirical binning (`ss_empirical.cpp`) used
`b = floor(u/Δu)` — **left-closed, right-open** `[C₁N, C₂N)`, the opposite convention. Corrected to
the right-closed integer bin (exact, no FP boundary fragility): the bin `b` with
`b·N < NB·p ≤ (b+1)·N`.

## Where it bites — the p = C₂N audit

A prime lands on a bin's right edge (p = C₂N, i.e. `u = p/N` a multiple of `Δu = 1/40`) iff
`N | 40p`; for good primes (p ∤ N, p > 3) that means **N | 40**. The only such conductor present in
the committed family is **N = 40**:

| grid | curves with N = 40 (all their good primes are p = C₂N) | total |
|---|---|---|
| 10⁴ (`ne_cache.txt`) | 2 | 1048 |
| 2¹⁶ | 3 | 5042 |
| 2¹⁷ | 3 | 9014 |
| 2¹⁸ | 3 | 15936 |

Each N = 40 curve has only good primes `p ≤ N = 40`, i.e. `p ∈ {7,11,13,17,19,23,29,31,37}`
(≈9 primes), all in **low-u bins (u = p/40 ∈ [0.175, 0.925])** — nowhere near the trough (u ≈ 0.87
region is empty for these curves, since p ≤ 40 ⇒ u ≤ 1 and the trough bin needs p ≈ 0.87·N = 35, so
only p = 37 lands high, in one curve's worth). The convention shifts each such prime's contribution
by exactly one bin.

## Impact — negligible; committed data unchanged; regen deferred

- **Shape / verdict / the note's reported numbers: unaffected.** 2–3 curves out of 5042–15936, a
  one-bin shift of ~9 low-u primes each, cannot move the argmin/argmax bins (hump 0.465, zero 0.671,
  trough 0.870 all hold) — the affected primes are low-u, the trough is high-u.
- **No green-gate breaks.** The committed run/partials files are frozen INPUTS; no ctest or the
  freshness check re-runs the statistic's binning (they read the committed curves). The fix changes
  only *future* `ss-run` output.
- **Regen deferred (flagged).** Regenerating the runs to match the corrected convention is a
  compute-box `ss-run` (a_p over the family) — heavy, for a negligible low-u change. The committed
  runs remain old-convention; this note is the provenance record. Derek's call whether to spend the
  box time; the note's scientific numbers do not depend on it.
