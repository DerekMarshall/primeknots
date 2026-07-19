# Referee-C reframe — canonical wording + location registry

The eq-(2) transcription fix (ERRATA #28) changes the density trough target 0.805 → 0.870
and dissolves the "persistent trough displacement." This file is the **single draft** of the
reframe wording; every location below is corrected to match it, so a future correction has a
list, not a grep. Numbers are from the corrected density (ss_density, ERRATA #28) and the
uniform B2/B3 run (`referee_b2b3.py`, seed 20260718).

## Canonical numbers

- **Corrected density targets** (stable over B∈[500,2000], `ss-density-scan`): hump **0.465**,
  zero **0.671**, trough **0.870**. (Superseded targets, from the swapped products: 0.475 /
  0.645 / 0.805.)
- **Empirical, 2¹⁸ verdict rung** — binned / interpolated (B2) / bootstrap 95% CI (B3):
  hump 0.4625 / 0.47064 / [0.46699, 0.47696]; zero (crossing) 0.671945 / 0.67194 /
  [0.66567, 0.67715]; trough 0.8875 / 0.88216 / [0.87611, 0.88567].
- **Deviations (interpolated column — residual claims ride this):** hump 0.0056, zero 0.0009,
  trough 0.0122. All ≪ **τ = 0.06**.

## Canonical statements (propagate verbatim in substance)

**S1 — uniform CI language (§3, JSON `within_tol`/claim_class).** All three invariants agree
within the a-priori tolerance τ = 0.06. The **zero sits on target** (CI [0.666, 0.677] contains
0.671). The **hump and trough carry small resolved residuals** (≈0.006 and ≈0.012; their CIs
marginally and clearly exclude their targets respectively) — resolved, not open; both ≪ τ.

**S2 — claim_class (emitter JSON, note §3, E1).** From-scratch replication of [SS25] Conjecture 1
over the PR-1 four-rung ladder X∈{10⁴,2¹⁶,2¹⁷,2¹⁸}. An initial comparison showed an apparent
persistent trough displacement (dev ≈0.0825 from a 0.805 target); a pre-registered audit round
traced it to a transcription error in our reading of eq (2) — the two products swapped and the
wrong exponent — **ERRATA #28**. Corrected, all three shape invariants agree within τ at the
bottom of the authors' 2¹⁶–2²⁸ window (zero on target; hump/trough small resolved residuals
≈0.006/≈0.012), at heights where the authors' own empirical-density discrepancy is an order of
magnitude larger (Table 4: mean 1.17/0.91/0.73 at 2¹⁶/2¹⁷/2¹⁸). Replication and constraint, not
proof (rule 7).

**S3 — PR-1 verdict SUPERSEDED (JSON `verdict`, note §3 historical paragraph).** PR-1's committed
Rung-3 clause gated |trough_u − 0.805| against a target ERRATA #28 invalidates. Rule committed
(dd6beb0, 4a17ebe), rule followed, H0 pronounced (8f64ba1) — then the target was found corrupted.
The verdict is **void as pronounced**; it is **not re-run against 0.870**, which it was never
registered against. Chain cited to ERRATA #28/#29.

**S4 — rightward-deformation observation (note §6, observation not claim).** Both non-zero
residuals point **rightward** (empirical > target): hump +0.006, trough +0.012. This is
consistent with a single small finite-height deformation — a horizontal dilation — rather than
two independent facts; it is the deformation-fit / PR-5 territory.

**S5 — PR-5 re-motivation (note §6).** PR-5 is retained and sharpened: the natural question is
now whether the coherent rightward shift **decays with X**, under a decision rule committed
before any rung beyond 2¹⁸.

## Location registry — every artifact carrying the verdict prose

Corrected in the reframe pass; keep this list current if the wording moves again.

| # | Location | What it carried | Reframe |
|---|---|---|---|
| 1 | `src/emit/emit_sawin_sutherland.cpp` `emit_m4` (claim_class, downward_bias_note, shape.refuted_hypothesis, convergence) | X=10⁴ "OPEN DEVIATION" | S2 + within-τ |
| 2 | `emit_sawin_sutherland.cpp` `emit_m5_extension` (claim_class, ladder_verdict, verdict logic + field, shape.refuted_hypothesis, convergence) | four-rung **H0** | S2 + **S3** (drop verdict computation) |
| 3 | `emit_sawin_sutherland.cpp` `emit_m5_rank_split` (claim_class, `target=0.805`, `targets` line) | PR-2 vs 0.805 | target→0.870; measurement stands; S3 |
| 4 | `app/at.cpp:362` (ss-run r2 default) | `0.475 0.645 0.805` | `0.465 0.671 0.870` |
| 5 | `app/at.cpp:999` (ss-leakage `target_trough`) | `0.805` | `0.870` |
| 6 | `src/murm/rank_split.{cpp:61,h:16-17}` | "recovers toward 0.805" H1 string | superseded; measurement (leave-out trough unchanged) stands |
| 7 | run-file `# r2` headers: `data/m4/ss_empirical.txt`, `data/m5/ss_x{65536,131072,262144}.txt` | `0.475 0.645 0.805` | `0.465 0.671 0.870` (empirical `curve` rows untouched) |
| 8 | `docs/schemas/ss_x_extension_murmuration.schema.json` | `verdict` enum {H0,H1,ambiguous} | add `superseded` |
| 9 | `viz/data/ss_x_extension_murmuration.json`, `sawin_sutherland_murmuration.json`, `ss_rank_split_murmuration.json` | emitted verdict/targets | re-emit from the corrected emitter |
| 10 | `docs/explainers/E1-murmurations.md` (E1-32/33/36), `E3-method.md` (E3-11/16), `CLAIMS-E.md` rows | "persists"/"open deviation"/"peek overturned to deviation" | S2 + own erratum row |
| 11 | `docs/preregistered/PR-1.md`, `PR-2.md`, `PR-3.md`; `docs/notes/m4-pinning.md` §P3/R0/R2 | the deviation diagnosis + 0.805 targets | historical + SUPERSEDED postscripts, cited to #28 |
| 12 | `docs/notes/data-note/data-note.md` §3/§4/§6/abstract + `CLAIMS-N.md` | trough displacement / H0 | note prose per the C spec (S1–S5) |
