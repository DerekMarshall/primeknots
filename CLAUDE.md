# CLAUDE.md

## What this project is

Arithmetic topology, executable. We verify named theorems of number theory — quadratic reciprocity as linking symmetry, Rédei reciprocity, the Rédei–Reichardt 4-rank theorem, the CKKPPY arithmetic Chern–Simons path-integral identity, the Weil explicit formula — in from-scratch C++, with visualizations emitted as JSON and rendered by static HTML viewers.

The mathematical spec lives in `docs/RESEARCH.md`. The engineering spec lives in `docs/ARCHITECTURE.md`. Read the relevant stage of RESEARCH.md before writing stage code. RESEARCH.md is a paraphrase; the papers it cites are normative.

A second track, the **M-ladder** (spec `docs/RESEARCH-M.md`, engineering `docs/ARCHITECTURE-M.md`), replicates elliptic-curve *murmurations* — the oscillating `a_p` / root-number correlation discovered in 2022 — from scratch under the same discipline: replication stages M0 (`a_p` machinery + provenance), M1 (the discovery), M2 (Dirichlet characters), M3 (Zubrilina's density), M4 (Sawin–Sutherland height ordering), and research-mode M5, whose pre-registered result is written up as the data note in `docs/notes/data-note/`. Two modes — replication (M0–M4) and research (M5) — are never blurred (RESEARCH-M §0 rule 5); no proofs are claimed anywhere (rule 7).

## Non-negotiables

1. **Never fit conventions to expected answers.** If `[13, 61, 937]` comes out `+1`, the bug is in the code or in our reading of the normalization — go to Stevenhagen [S22] / Corsman [C07] and fix the reading. Flipping a sign until the anchor value appears is falsification of the experiment and defeats the entire purpose of the repo. This is the prime directive; it applies with maximum force to Stage 2 normalization and Stage 4 cohomological pinning.
2. **Dual implementation before trust.** Every mathematically load-bearing function has two independent implementations (different algorithms, not refactors of each other) or an external oracle check, wired into `verify/`. A result computed one way is a rumor.
3. **Oracles referee, never replace.** PARI/GP and LMFDB confirm our numbers; they are not called from `src/`. Oracle tests skip cleanly (with a visible SKIP, not silent pass) when the oracle is absent.
4. **Stage gates.** Do not build on a stage whose verification suite isn't green. Stage N code may depend only on stages ≤ N.
5. **Flag, don't smooth.** If a definition in RESEARCH.md is ambiguous or seems to conflict with a paper, stop and record the discrepancy in `docs/notes/` — do not pick the interpretation that makes tests pass. Items marked [PIN TO SOURCE] require a written paper-notation → code-identifier mapping in `docs/notes/` before the code is considered done.
6. **No fabricated citations.** Never add a reference, arXiv ID, or theorem attribution without verifying it. Unverified pointers go in notes marked UNVERIFIED.
7. **Scope, stated up front.** (= RESEARCH-M §0 rule 7.) The M-stages M0–M4 replicate known mathematics (2022–2025); research mode (M5) produces at most a clean, pre-registered empirical fact. **No proofs are claimed anywhere in this project** — "verified" means numerically, over a range, twinned/refereed. The permanent repo name `primeknots` satisfies this rule (an origin homage, not a scope claim; m0-pinning §282).
8. **No numerical constant is ever typed from memory.** Every floating constant is either **(a)** the true mathematical value transcribed from a *cited source* to ≥ 25 significant digits, source in a comment, letting the compiler round; or **(b)** *generated* — a checked-in script (`oracle/gen_constants.py`, PARI/GP or MPFR-class precision ≥ 50 digits) emits `src/core/constants.h`, and a ctest target (`constants.regen_matches`) regenerates and diffs it (the freshness pattern, applied to constants). **Derived pairs (hi/lo argument-reduction splits) are ALWAYS generated, never typed:** the split is computed at high precision (`hi = round-to-double(x)`, `lo = round-to-double(x − hi)`) and the generator asserts the defining identities (`hi == double(x)`, `hi + lo` reconstructs the source value to ≫ double precision). The reason (ERRATA #25, the CONSTANTS INCIDENT): a constant typed from memory is **invisible to twins** — twins share the constant, so a wrong value corrupts a computation and its twin identically; only an external referee or a high-precision generator can catch it.

## Rule index (canonical)

Two numbered rule-lists exist and their numbers collide. **Citation convention:** a
bare "rule N" (in pinning notes, code comments, commit messages) means the **CLAUDE.md
non-negotiable N** below; the epistemic rules of RESEARCH(.md/-M) §0 are cited
**qualified** ("RESEARCH-M §0 rule N") or in-document. Rule 7 is the one number the two
lists share (both = Scope). The deck's "Rule 1" is CLAUDE.md #1.

| Cite as | File | One-line statement |
|---|---|---|
| rule 1 | CLAUDE.md | Never fit conventions to expected answers (the prime directive) |
| rule 2 | CLAUDE.md | Dual implementation before trust (two independent impls, or an oracle) |
| rule 3 | CLAUDE.md | Oracles referee, never replace; an absent oracle SKIPs visibly |
| rule 4 | CLAUDE.md | Stage gates: build only on green; stage N depends only on stages ≤ N |
| rule 5 | CLAUDE.md | Flag, don't smooth — record discrepancies in docs/notes, don't fit |
| rule 6 | CLAUDE.md | No fabricated citations (unverified pointers marked UNVERIFIED) |
| rule 7 | CLAUDE.md | Scope, stated up front — no proofs claimed (≡ RESEARCH-M §0 rule 7) |
| rule 8 | CLAUDE.md | No numerical constant ever typed from memory (transcribe-cited or generate) |
| RESEARCH.md §0 rule 1 | RESEARCH.md | Map, not source — the cited paper is normative |
| RESEARCH.md §0 rule 2 | RESEARCH.md | "Provable" = numerically verified (twins/oracle), not formal proof |
| RESEARCH.md §0 rule 3 | RESEARCH.md | [CONJECTURAL] = research-open; compute *around* it, not the object |
| RESEARCH.md §0 rule 4 | RESEARCH.md | [PIN TO SOURCE] = don't improvise fiddly conventions |
| RESEARCH-M §0 rule 5 | RESEARCH-M | Two modes (replication M0–M4 / research M5), never blurred |
| RESEARCH-M §0 rule 6 | RESEARCH-M | Provenance labeling: every column `computed \| oracle` |
| RESEARCH-M §0 rule 7 | RESEARCH-M | Scope, stated up front (≡ CLAUDE.md rule 7) |

Collisions to be aware of (same number, different rule): 1, 2, 3, 4, 5, 6 all differ
between the two lists; 7 is aligned; 8 is CLAUDE.md-only. Audit (M4-completion session):
all existing bare citations in docs/notes already follow the convention (e.g.
m0-pinning §221 "rule-6" = CLAUDE.md #6, no-fabricated-citations); RESEARCH-M §0
rule 6 (provenance) is always cited qualified.

## Build, test, run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release   # add -DWITH_GMP=ON when a stage needs it
cmake --build build -j
ctest --test-dir build --output-on-failure   # all verification suites
ctest --test-dir build -L stage1             # one stage (labels: stage0..stage6, m0..m5)
ctest --test-dir build -L oracle             # oracle-refereed tests only
./build/bin/at emit --stage 1 --out viz/data/   # emit JSON for the viewers
python3 -m http.server -d viz                    # view at localhost:8000
bash docs/notes/data-note/render.sh              # render the M5 data note (HTML + PDF)
```

C++20. Core is dependency-free (u128 built-ins). GMP only behind `src/core/bigint.h`, only where RESEARCH.md says values exceed 128 bits (large-discriminant Stage 3, parts of Stage 4). No other third-party libraries in `src/`. Test framework: doctest (vendored, header-only). Viewers: static HTML + D3 from CDN, no build step, consume JSON only.

## Repo layout

```
docs/           RESEARCH.md, ARCHITECTURE.md (+ RESEARCH-M.md/ARCHITECTURE-M.md, the M-ladder), notes/ (pinning logs, discrepancies; data-note/), explainers/, schemas/ (JSON)
src/core/       u128 arithmetic, modpow, Miller–Rabin, gcd/CRT, bigint facade
src/symbols/    Legendre (Euler + reciprocity twins), Jacobi, Tonelli–Shanks, n-th power residue
src/linking/    F2Matrix (bit-packed), LinkingMatrix, rank, stats
src/redei/      conic solver (Cremona–Rusin + Holzer brute twin), normalization, Rédei symbol
src/classgroup/ quadratic forms, reduction, composition, group structure, Rédei matrix, ranks
src/cs/         S-unit character enumeration, CS phase sum (LHS), Gauss-sum closed form (RHS)
src/zeta/       Riemann–Siegel, explicit-formula reconstruction, suspension-flow toy zeta
src/dw/         S3 homomorphism counting (stretch)
src/ell/        M-ladder — elliptic curves: a_p (char-sum referee, O(p) fast, Shanks–Mestre O(p^{1/4})), curve, ecdata, a_p cache
src/mform/      M-ladder — modular-form side: Hurwitz class numbers, dimension formulas, trace formula
src/murm/       M-ladder — murmurations: height family, murmuration statistic, Dirichlet/Zubrilina/Sawin–Sutherland densities, rank split, N/ε + a_p caches, ss-run I/O
src/emit/       JSON writers incl. murmuration/Dirichlet/Zubrilina/Sawin–Sutherland (schemas in docs/schemas/)
verify/         per-stage verification suites (stage0..6, m0..m5) — the point of the repo
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
- `prereg_*` — a pre-registered M5 prediction whose design (tolerance / null / threshold) was committed *before* the confirming run (RESEARCH-M §7).

A stage's definition of done: all five categories present where applicable, green, plus JSON emitted and rendered by its viewer. Property-test ranges and seeds are fixed in the suite (reproducible), with an opt-in `--extended` mode for larger sweeps.

## Working style

- Small commits, one mathematical object per commit where possible; commit messages name the object or theorem, not the file.
- **After any change to `src/`, re-run the affected verification suites (the touched stage's ctest) *before* reporting the commit done** — never push a src edit on the strength of a spot-check. A byte-copy or twin can ride along stale otherwise (ERRATA #31: the eq (1) binning fix shipped without re-running m5, and its `ss_empirical_m0b.cpp` byte-copy stayed on the old convention until the pre-merge clean-clone gate caught the drift).
- Performance matters after correctness: get the twin implementations agreeing, then optimize one of them (keep the naive twin as the referee — never optimize both).
- When mathematical uncertainty arises mid-task and can't be resolved from the cited sources in-repo, write the question into `docs/notes/open-questions.md` with full context and stop that thread rather than guessing.
- Visualizations follow the JSON contract in ARCHITECTURE.md; never inline data into viewer HTML.
