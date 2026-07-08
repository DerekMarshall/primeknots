# Freshness guard — the committed viz/data snapshot == `at emit` at HEAD

Session note for the `freshness`-labelled ctest (`verify/freshness_check.py`),
added to retire the earlier "synced manually" decay mode. The contract and its
enforcement are in ARCHITECTURE §5; this note records what happened when the guard
first ran.

## The guard fired on its first CI run

The freshness session's spec carried a contingency: adding an ERRATA row was
expected **only if** the initial run found the committed snapshot already stale —
and if it did, that was a finding to record, not to quietly fix. That branch fired.

- **When:** the guard's first CI run (GCC/ubuntu leg), before it had ever run green
  in CI. It failed immediately with
  `zeros.json .aggregate.max_odlyzko_dev: type float≠NoneType`.
- **Which file drifted:** `viz/data/zeros.json` — and only that file. The other
  six snapshot files were byte-identical to `at emit` at HEAD.
- **Since which commit:** `52e14c3`, where the snapshot was committed with the
  Odlyzko oracle present locally. `zeros.json` embedded oracle-dependent fields
  (per-zero `delta`, `max_odlyzko_dev`, `oracle:"odlyzko"`) that are not
  reproducible from the repo alone, because the Odlyzko table is gitignored and so
  absent in CI. The defect had been latent in the published snapshot since `52e14c3`;
  the guard is the first mechanism that could see it, because CI is a repo-only
  checkout — the same *second environment* effect as the missing-headers bug (#15).
- **Resolution:** commit `f16d927` re-emitted `zeros.json` oracle-less
  (`--odlyzko ""` → `oracle:"none"`, no deltas), and the freshness check emits
  stage 5 oracle-less so the published data stays repo-reproducible. The Odlyzko
  cross-check itself is unaffected — it lives in the suite as
  `anchor_zeros_match_odlyzko`, not in the published snapshot.

Recorded as **ERRATA #18** (who: coding agent, at `52e14c3`; caught-by: the
freshness guard's first CI run; stage: infra/CI). This finding was not a reason to
weaken the guard — it is the guard doing exactly what the contract is for.

## Row-count reconciliation

The ledger stands at 18 rows. External accounting that expected 17 was correct as
of the **deck-review session**, which added #16 (Stage-0 coverage conflation) and
#17 (Borromean mis-attribution) and closed at 17. #18 was added later, in the
**CI freshness session** (commit `f16d927`), by the contingency above — so the
count is one session ahead of that accounting, not miscounted.
