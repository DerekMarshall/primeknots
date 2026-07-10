# ARCHITECTURE-M.md — Delta for the Murmurations Stages

Extends ARCHITECTURE.md; everything there stands (twins, layering, JSON contract, freshness, oracle discipline). This document specifies only what is new. Read with RESEARCH-M.md.

## 1. Modules and layering

```
src/ell/     curves (Weierstrass models, reduction mod p), a_p twins
             (charsum referee frozen; table-batched fast path; optional
             Shanks–Mestre), model-invariance helpers
src/mform/   Eichler–Selberg/Hijikata traces (M3) — depends on
             classgroup/ (Hurwitz class numbers) and symbols/
src/murm/    family definitions, ordering axes (conductor | height),
             weighted averaging, binning in y = p/N, density-formula
             evaluators (M2/M3/M4 formula-side TUs live here,
             authored per the separate-sessions protocol)
src/stats/   prereg runner (loads PR docs, verifies code hash, runs
             declared statistic on declared holdout), looks-ledger
             tooling, tolerance/threshold utilities
data/cremona/  fetched by script, never committed: release tag +
             sha256 pinned in the M0 log; loader validates checksum
```

Dependency direction: `ell → {symbols, core}`; `mform → {classgroup, symbols, core}`; `murm → {ell, mform}`; `stats → murm`. No module below `murm` knows about families or averaging.

## 2. Provenance as a schema field

Every emitted dataset column and every in-memory field consumed from ecdata/LMFDB carries `provenance: computed | oracle`. Loader enforces it (unknown columns rejected). Rule 6 of RESEARCH-M §0 is mechanically checkable: a `theorem_` test declares which columns it reads, and CI greps that declaration against provenance. Rank, root number, conductor, bad-prime a_p: **oracle**. Good-prime a_p: **computed** (and spot-refereed by `oracle_ellap`).

## 3. New test category

`prereg_` joins the five. Contract: the test names its PR document; fails if (a) the PR doc's recorded analysis-code hash does not match HEAD's relevant TUs, (b) the holdout region overlaps any region named in the PR doc's exploration section, or (c) the declared statistic on the declared holdout violates the declared threshold. Outcome (either way) is appended to the PR doc as a postscript in the same commit. There is no mechanism for re-running a failed prereg with adjusted parameters under the same PR number — that is a new PR document, which cites the failed one.

## 4. The a_p cache (a new artifact class)

The M1 grid (~10⁵ curves × primes to ~10⁴⁺) makes recomputation the dominant cost; a_p values are cached in a binary, versioned format: header = {format version, generator code hash, ecdata release sha, prime bound, curve-set descriptor}, then packed int16 values (|a_p| ≤ 2√p bounds fit). Rules: the cache is an *artifact with provenance*, invalidated by generator-hash mismatch (loader refuses stale caches — no silent reuse); it is never committed (fetch/regenerate script + checksum manifest, per the Odlyzko precedent); the freshness principle applies — any test consuming the cache asserts the header hash matches HEAD's generator. Twin discipline unchanged: the cache stores fast-path output; `twin_ap_charsum_vs_table` recomputes a REQUIREd sample with the frozen referee on every run, and `oracle_ellap` samples independently.

## 5. Performance notes

Per-prime batching is the intended shape: fix p, build the QR table (O(p)), then stream all curves (O(p) per curve via x-loop, or Shanks–Mestre when p is large enough to win). Embarrassingly parallel over primes; bit-identical determinism required and tested (`invariance_parallel_vs_serial`, as Stage 1). Budget expectation: M1 default grid in core-minutes, extended grid overnight — record wall times in the extended-run notes as before. Do not optimize the referee.

## 6. Schemas and viewers

- `murmuration_curve/1`: {family: {ordering, invariant ranges, class (rank|root_number), weighting, counts (REQUIRE-certified)}, points: [(p, y, avg)], provenance block, params envelope as always}.
- `density_compare/1`: {empirical curve ref, formula id + paper eq, evaluation grid, distance statistic + value, tolerance + its justification ref}.
- `prereg_result/1`: {pr id, code hash, holdout descriptor, statistic value, threshold, verdict}.
Viewers: `viz/murmurations.html` (family curves with range presets, scale-collapse overlay), `viz/density.html` (empirical vs formula, residuals). Same house rules: JSON-only, params visible, committed snapshot governed by the freshness guard.

## 7. Stage acceptance (delta table)

| Stage | Green means |
|---|---|
| M0 | pinning log (a_p sign anchor quoted from source; model invariance; bad-prime convention; ecdata release pinned); twin_ap referee vs fast path over REQUIREd grid sample; oracle_ellap; invariance_weierstrass_model; cache round-trip + stale-hash rejection test |
| M1 | family counts REQUIRE-certified against dataset; HLOP ranges pinned + reproduced; scale-collapse statistic under pre-stated tolerance; antiphase invariant; emission + viewer |
| M2 | family + density pinned with eq numbers; theorem_murmuration_dirichlet convergence over certified sweep; zero external data used (asserted) |
| M3 | trace pinning reviewed at gate BEFORE trace TU; twin traces vs PARI/LMFDB samples; separate-TU formula side; convergence to [Z25] density incl. discontinuity structure; Hurwitz reuse from classgroup/ |
| M4 | height definition + density pinned; ordering axis a tested parameter; convergence to [SS25] density |
| M5 | per-PR: prereg_ contract of §3 in full; LOOKS.md current (CI check: every exploration emission has a ledger line) |

## 8. Failure modes this delta is built against

Named so the tests can target them: sign/off-by-one in a_p (global, silent → source-quoted anchor + model invariance); family-definition drift from the paper (soft anchors make this invisible → definitions quoted, counts certified); cache staleness (→ generator-hash refusal); pattern-fitting (→ prereg contract, holdout disjointness check, looks ledger); tolerance shopping (→ tolerances justified in logs before runs, changes to a tolerance are log events); oracle-provenance leakage into computed claims (→ §2 declaration + CI grep).

## 9. Snapshot reproducibility (amendment to the ERRATA #18 principle)

ERRATA #18 established: the committed `viz/data` snapshot must be reproducible from
the repo alone, and the freshness guard byte-checks it in CI. Some M-stages compute
their snapshot from **external data**, which cannot be inlined. This amends the
principle into two named artifact classes:

- **Repo-reproducible (the default).** No external data; the snapshot is a pure
  function of committed code. Freshness byte-checks it from the repo in CI, exactly
  per ERRATA #18. **M2 is the clean case** — Dirichlet-character murmurations use
  *zero external data* (asserted in the suite), so their snapshot is fully
  repo-reproducible; this is the default every stage should aim for.

- **External-data-derived.** The snapshot is computed from an external dataset
  (M1: Cremona ecdata). Policy, in order of preference:
  1. **If the dataset's license permits redistribution** (M1: ecdata is Artistic
     License 2.0), commit a **compact derived extract** — exactly the parsed rows
     the stage consumes — git-tracked, header-attributed, and sha-linked in its log
     to the pinned upstream release. This *restores* the repo-reproducible property:
     freshness/validate/tests read the extract and run in CI. The raw upstream slices
     stay gitignored (fetched, checksum-pinned) and are needed only to regenerate the
     extract (`at ecdata-extract`). **M1 takes this path.**
  2. **If redistribution is not permitted**, the snapshot is a *SKIP-class* artifact:
     freshness/validate/tests SKIP cleanly (exit 77) when the data is absent, with the
     sha256-pinned fetch manifest as the reproducibility anchor. **Residual risk,
     stated explicitly:** CI cannot detect drift for a SKIP-class snapshot — only a
     local run with the data present can. This is a real gap, accepted only when
     redistribution is barred; it is not the default and must be named in the stage's
     log, never left implicit.
