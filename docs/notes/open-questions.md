# Open questions & resolved decisions

Per CLAUDE.md rule 5: ambiguities and doc/spec discrepancies are recorded here
rather than silently resolved in code. This file has two parts — currently-open
questions (things awaiting a source or a decision) and a log of resolved
decisions (with their rationale, so later sessions don't relitigate them).

## Open questions

*(none at Stage 0)*

## Resolved decisions

### R1 — Location of `RESEARCH.md` and `ARCHITECTURE.md`  (resolved 2026-07-07)

**Discrepancy.** `CLAUDE.md`, `README.md`, and `ARCHITECTURE.md` all reference
the specs as `docs/RESEARCH.md` and `docs/ARCHITECTURE.md`, and the repo-layout
section (in CLAUDE.md) places `RESEARCH.md`, `ARCHITECTURE.md`, `notes/`, and
`schemas/` under `docs/`. But both files were initially committed at the repo
root.

**Resolution.** `git mv` both files into `docs/` to match the spec's own
references. `README.md` stays at the repo root (standard). User approved
2026-07-07. Layout authority is **CLAUDE.md** (§Repo layout); acceptance-criteria
authority is **ARCHITECTURE.md §7** — confirmed by the user (the session prompt's
phrase "ARCHITECTURE.md §repo layout" was a misattribution).

### R2 — Home of the `at` CLI entry point  (resolved 2026-07-07)

**Gap.** ARCHITECTURE.md §2 says JSON emitters live in `src/emit/`, and the
build docs invoke `./build/bin/at emit ...`. But the Stage 0 scope forbids
anything in `src/` beyond `core/` and `symbols/`, and per-stage emitters
(`linking_matrix.json` is Stage 1, etc.) would be ahead-of-stage scaffolding.
The spec does not say where the executable's `main()` lives.

**Resolution.** The `at` executable's entry point lives in `app/at.cpp` (a
top-level `app/` directory for the CLI, kept separate from the `src/` libraries).
For Stage 0 it is a **stub**: it parses `emit --stage <N> --out <dir>` and reports
that no emitters are wired yet (Stage 0 emits no visualization data). `src/emit/`
and the real per-stage emitters are authored by the stages that produce data —
not now.

### R3 — Vendored doctest location  (resolved 2026-07-07)

CLAUDE.md requires doctest be vendored (header-only). It lives at
`third_party/doctest/doctest.h`, pinned to release **v2.4.12**. The CMake ctest
integration module (`doctest.cmake`) is intentionally **not** vendored: Stage 0
registers ctest tests manually via `add_test` with doctest `--test-case` filters,
which gives explicit control over per-test labels (`stage0`, `oracle`) and over
the clean-SKIP exit-code contract (see R4). No other third-party code is added.

### R4 — Runtime SKIP semantics for oracle tests  (resolved 2026-07-07)

CLAUDE.md rule 3 / ARCHITECTURE §4: a missing oracle must SKIP *visibly*, never
a silent pass and never a failure. doctest has no native runtime-skip that ctest
observes, so:

- The verify binary uses a **custom doctest `main`** (`DOCTEST_CONFIG_IMPLEMENT`).
  An oracle test that finds no `gp` prints a visible `[SKIP] ...` line and sets a
  global skip flag instead of asserting.
- The `main` maps results to exit codes deterministically: **any assertion
  failure → 1** (never the skip code); **clean skip with no failures → 77**;
  **all-pass → 0**. ctest is told `SKIP_RETURN_CODE 77` for oracle tests, so a
  missing oracle shows as `Skipped`, a real bug shows as `Failed`, and the two
  can never be confused.
- Because `gp` is installed on the dev machine, the SKIP path never triggers
  naturally. It is exercised by a dedicated test (`oracle_clean_skip_when_gp_absent`)
  that runs the oracle suite with `gp` scrubbed from `PATH` and asserts exit 77 +
  a visible `[SKIP]` line. Clean-skip is a spec behavior, so it is tested like any
  other. This test itself always PASSES (it verifies the mechanism); it is not
  itself skipped.

### R5 — `anchor_` and `invariance_` categories at Stage 0  (noted 2026-07-07)

Not a discrepancy — recorded so their absence isn't read as an omission. Of the
five verification categories, Stage 0 exercises `twin_`, `theorem_`, and
`oracle_`. `anchor_` (a specific published constant like `[13,61,937]`) and
`invariance_` (independence from an arbitrary internal choice) have no natural
subject at the primitives layer: there is no published magic constant to pin, and
the primitives (modpow, Miller–Rabin, Legendre/Jacobi, Tonelli–Shanks) expose no
arbitrary convention choice whose independence needs asserting. Both categories
first become load-bearing at Stage 2 (Rédei symbol). They are omitted at Stage 0
deliberately, not forgotten.
