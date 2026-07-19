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

## Impact — shape unchanged; a code/data coherence defect corrected pre-release

- **Shape / verdict / the note's reported numbers: unaffected.** 2–3 curves out of 5042–15936, a
  one-bin shift of ~9 low-u primes each, cannot move the argmin/argmax bins (hump 0.465, zero 0.671,
  trough 0.870 all hold) — the affected primes are low-u, the trough is high-u.
- **CORRECTION (referee C, clean-clone gate — ERRATA #31).** An earlier version of this note claimed
  "no ctest re-runs the statistic's binning." That was **false**: `twin_ss_provider_fast_vs_m0b`
  (test 123) re-bins live from the committed `ne_cache` under both a_p providers, and the ap-cache
  reproduce test re-bins too. The fix was applied to `ss_empirical.cpp` only and its byte-copy
  `ss_empirical_m0b.cpp` was missed, so the two providers drifted at the conductor-40 edge primes and
  test 123 went red on the pre-merge clean-clone gate. Both loops (and the reproduce test) are now
  unified on the right-closed bin, and `SS_GENERATOR_HASH` is widened to cover the copy (#31).
- **Regen reversed from deferred → executed this release.** Leaving the code corrected but the runs
  stale meant HEAD could no longer regenerate the committed runs byte-identically and the stamped
  `SS_GENERATOR_HASH` no longer matched — a §5-level defect, not a negligible deferral. The
  runs/partials are regenerated on the compute box under the corrected binning in this release
  series; the flat prediction (only conductor-40 pairs move; every shape invariant and §3/§4 number
  unchanged) is confirmed with that regen.
