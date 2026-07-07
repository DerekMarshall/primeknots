# CLAUDE.md

## What this project is

Arithmetic topology, executable. We verify named theorems of number theory — quadratic reciprocity as linking symmetry, Rédei reciprocity, the Rédei–Reichardt 4-rank theorem, the CKKPPY arithmetic Chern–Simons path-integral identity, the Weil explicit formula — in from-scratch C++, with visualizations emitted as JSON and rendered by static HTML viewers.

The mathematical spec lives in `docs/RESEARCH.md`. The engineering spec lives in `docs/ARCHITECTURE.md`. Read the relevant stage of RESEARCH.md before writing stage code. RESEARCH.md is a paraphrase; the papers it cites are normative.

## Non-negotiables

1. **Never fit conventions to expected answers.** If `[13, 61, 937]` comes out `+1`, the bug is in the code or in our reading of the normalization — go to Stevenhagen [S22] / Corsman [C07] and fix the reading. Flipping a sign until the anchor value appears is falsification of the experiment and defeats the entire purpose of the repo. This is the prime directive; it applies with maximum force to Stage 2 normalization and Stage 4 cohomological pinning.
2. **Dual implementation before trust.** Every mathematically load-bearing function has two independent implementations (different algorithms, not refactors of each other) or an external oracle check, wired into `verify/`. A result computed one way is a rumor.
3. **Oracles referee, never replace.** PARI/GP and LMFDB confirm our numbers; they are not called from `src/`. Oracle tests skip cleanly (with a visible SKIP, not silent pass) when the oracle is absent.
4. **Stage gates.** Do not build on a stage whose verification suite isn't green. Stage N code may depend only on stages ≤ N.
5. **Flag, don't smooth.** If a definition in RESEARCH.md is ambiguous or seems to conflict with a paper, stop and record the discrepancy in `docs/notes/` — do not pick the interpretation that makes tests pass. Items marked [PIN TO SOURCE] require a written paper-notation → code-identifier mapping in `docs/notes/` before the code is considered done.
6. **No fabricated citations.** Never add a reference, arXiv ID, or theorem attribution without verifying it. Unverified pointers go in notes marked UNVERIFIED.

## Build, test, run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release   # add -DWITH_GMP=ON when a stage needs it
cmake --build build -j
ctest --test-dir build --output-on-failure   # all verification suites
ctest --test-dir build -L stage1             # one stage (labels: stage0..stage6)
ctest --test-dir build -L oracle             # oracle-refereed tests only
./build/bin/at emit --stage 1 --out viz/data/   # emit JSON for the viewers
python3 -m http.server -d viz                    # view at localhost:8000
```

C++20. Core is dependency-free (u128 built-ins). GMP only behind `src/core/bigint.h`, only where RESEARCH.md says values exceed 128 bits (large-discriminant Stage 3, parts of Stage 4). No other third-party libraries in `src/`. Test framework: doctest (vendored, header-only). Viewers: static HTML + D3 from CDN, no build step, consume JSON only.

## Repo layout

```
docs/           RESEARCH.md, ARCHITECTURE.md, notes/ (pinning logs, discrepancies), schemas/ (JSON)
src/core/       u128 arithmetic, modpow, Miller–Rabin, gcd/CRT, bigint facade
src/symbols/    Legendre (Euler + reciprocity twins), Jacobi, Tonelli–Shanks, n-th power residue
src/linking/    F2Matrix (bit-packed), LinkingMatrix, rank, stats
src/redei/      conic solver (Cremona–Rusin + Holzer brute twin), normalization, Rédei symbol
src/classgroup/ quadratic forms, reduction, composition, group structure, Rédei matrix, ranks
src/cs/         S-unit character enumeration, CS phase sum (LHS), Gauss-sum closed form (RHS)
src/zeta/       Riemann–Siegel, explicit-formula reconstruction, suspension-flow toy zeta
src/dw/         S3 homomorphism counting (stretch)
src/emit/       JSON writers (schemas in docs/schemas/)
verify/         per-stage verification suites — the point of the repo
oracle/         PARI/GP scripts, LMFDB fetch/cache scripts, adapters
viz/            index.html + per-stage viewers, viz/data/ (emitted JSON, gitignored)
data/           Odlyzko zero tables, LMFDB extracts (cached, checksummed)
```

## Verification vocabulary

Every suite in `verify/` is built from these patterns; name tests accordingly:

- `twin_*` — two independent implementations agree (e.g. `twin_legendre_euler_vs_reciprocity`).
- `theorem_*` — a named theorem holds over an exhaustive/sampled range (e.g. `theorem_quadratic_reciprocity`, `theorem_redei_reciprocity_s3`, `theorem_redei_reichardt_4rank`, `theorem_ckkppy_partition_identity`).
- `anchor_*` — a specific published value (e.g. `anchor_redei_13_61_937_is_minus_1`, `anchor_zeros_match_odlyzko`).
- `invariance_*` — result independent of arbitrary choices (e.g. `invariance_redei_solution_choice`, `invariance_sqrt_branch`).
- `oracle_*` — PARI/LMFDB referee checks.

A stage's definition of done: all five categories present where applicable, green, plus JSON emitted and rendered by its viewer. Property-test ranges and seeds are fixed in the suite (reproducible), with an opt-in `--extended` mode for larger sweeps.

## Working style

- Small commits, one mathematical object per commit where possible; commit messages name the object or theorem, not the file.
- Performance matters after correctness: get the twin implementations agreeing, then optimize one of them (keep the naive twin as the referee — never optimize both).
- When mathematical uncertainty arises mid-task and can't be resolved from the cited sources in-repo, write the question into `docs/notes/open-questions.md` with full context and stop that thread rather than guessing.
- Visualizations follow the JSON contract in ARCHITECTURE.md; never inline data into viewer HTML.
