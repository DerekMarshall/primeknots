# ARCHITECTURE.md — Design and Engineering Spec

Companion to `docs/RESEARCH.md` (mathematics) and `CLAUDE.md` (operating rules). This document covers module design, core types, the verification harness, oracle integration, the JSON/visualization contract, and per-stage acceptance criteria.

## 1. Design principles

1. **Verification is the product.** `src/` exists so that `verify/` has something to check. Design decisions optimize for independent recomputation and refereeing, not for framework elegance.
2. **Twin asymmetry.** Each load-bearing computation has a fast implementation and a naive referee twin. The naive twin is frozen after review: it stays simple, obviously-correct, and unoptimized forever. Only the fast path evolves.
3. **Layering by stage.** Dependency direction is strictly: `core → symbols → linking → {redei, classgroup} → cs`, with `zeta` and `dw` depending only on `core`/`symbols`. Enforced by include-what-you-use discipline and a CI grep; no upward or lateral includes.
4. **Data out, pixels elsewhere.** C++ emits JSON conforming to `docs/schemas/`; viewers are static HTML consuming that JSON. No plotting libraries in C++, no computation in JavaScript beyond layout.

## 2. Core types

```cpp
// src/core — no dependencies
using u64  = std::uint64_t;
using u128 = unsigned __int128;          // all intermediate products go through u128
u64  modpow(u64 base, u64 exp, u64 mod); // Montgomery multiplication internally
bool is_prime(u64 n);                    // deterministic Miller–Rabin, fixed witness set
struct Bigint;                           // src/core/bigint.h — GMP facade, compiled only with WITH_GMP;
                                         // core algorithms are templated on the integer type where reuse is needed

// src/symbols
int legendre_euler(u64 a, u64 p);        // Euler criterion twin
int legendre_recip(u64 a, u64 p);        // reciprocity-recursion twin (via Jacobi)
int jacobi(u64 a, u64 n);
std::optional<u64> sqrt_mod(u64 a, u64 p);   // Tonelli–Shanks; nullopt on non-residue (tested both ways)
int power_residue(/* n-th power residue symbol; Stage 4 odd-p extension */);

// src/linking
class F2Matrix {                         // bit-packed rows (std::vector<u64>), row ops on words
  size_t rank() const;                   // Gaussian elimination over F2
  F2Matrix submatrix(std::span<const size_t> idx) const;
};
struct LinkingMatrix {                   // symmetric F2Matrix + prime labels
  std::vector<u64> primes;               // ascending, all ≡ 1 (mod 4)
  F2Matrix lk;
  static LinkingMatrix build(u64 prime_bound);   // twin symbols cross-checked during build
};

// src/redei
struct ConicSolution { i128 x, y, z; };  // primitive solution of x² − a·y² − b·z² = 0
std::vector<ConicSolution> solve_conic(u64 a, u64 b, int count);
  // Cremona–Rusin fast path; brute force under Holzer bound as referee twin;
  // count > 1 solutions feed invariance_* tests
int redei_symbol(u64 p1, u64 p2, u64 p3);        // ±1; preconditions asserted, normalization per [S22]/[C07]

// src/classgroup
struct QForm { i128 a, b, c; };          // primitive, discriminant D
QForm reduce(QForm f);
QForm compose(QForm f, QForm g);         // Gauss/Shanks composition
struct ClassGroup {                      // narrow class group of discriminant D
  std::vector<u64> invariants;           // Smith normal form
  size_t rank2() const; size_t rank4() const;
  static ClassGroup compute(i128 D);     // via form enumeration + composition
};
F2Matrix redei_matrix(std::span<const u64> primes, const LinkingMatrix& lk);
size_t   fourrank_from_linking(std::span<const u64> primes, const LinkingMatrix& lk); // t − 1 − rank(R)

// src/cs
struct PrimeSet { std::vector<u64> primes; };    // the ramification set S
std::complex<double> cs_partition_lhs(const PrimeSet&, /* sources */);  // brute-force phase sum over characters
std::complex<double> cs_partition_rhs(const PrimeSet&, const LinkingMatrix&, /* sources */); // closed-form Gauss sum
// The two functions live in separate TUs, are written against the paper independently,
// and share nothing except core/symbols. See docs/notes/stage4-pinning.md protocol.

// src/zeta
std::vector<double> zeta_zeros(size_t n);        // Riemann–Siegel; each zero bracketed by sign change of Z(t)
double psi_direct(double x);                     // Chebyshev ψ by sieve
double psi_from_zeros(double x, size_t n_zeros); // explicit-formula partial sum
struct SuspensionFlow { /* subshift adjacency + roof function */ };
std::complex<double> ruelle_zeta_product(const SuspensionFlow&, std::complex<double> s, double len_cut);
std::complex<double> ruelle_zeta_det(const SuspensionFlow&, std::complex<double> s);   // determinant twin
```

`VerificationResult { std::string name; enum {PASS, FAIL, SKIP} status; u64 cases; std::vector<std::string> failures; }` is the uniform return of every suite entry; the runner aggregates and also emits `viz/data/verification.json` so the dashboard viewer shows repo-wide status.

## 3. Verification harness (`verify/`)

- doctest-based, one binary per stage (`verify_stage0` … `verify_stage6`), registered in ctest with labels `stageN`, plus label `oracle` on referee tests.
- Test taxonomy and naming per CLAUDE.md: `twin_`, `theorem_`, `anchor_`, `invariance_`, `oracle_`.
- Deterministic: fixed seeds, fixed default ranges. `--extended` flag scales ranges ~100× for overnight sweeps; extended failures are still failures.
- Exhaustive where cheap (all prime pairs to 10⁶ for reciprocity), property-sampled where not (Rédei triples, CS prime sets), with the sampled sets logged into the JSON output so a failure is replayable.

## 4. Oracle integration (`oracle/`)

- **PARI/GP**: invoked as a subprocess (`gp -q script.gp`), never linked. Adapters in `oracle/pari.{h,cpp}` serialize one request batch per invocation (thousands of checks per process spawn, not one). Scripts in `oracle/gp/*.gp` are plain-text, reviewable **PARI/GP** (the `\\`-comment language of `gp`), e.g. `kronecker.gp`, `cubic_count.gp` — *not* Gnuplot; GitHub Linguist maps the `.gp` extension to Gnuplot, so `.gitattributes` marks them `linguist-detectable=false` to keep the language stats honest. Used for: `kronecker` (Stage 0/1), `quadclassunit`/`bnfnarrow` (Stage 3), cubic-field enumeration `nflist` (Stage 6). Belabas's standalone `cubic` enumerator is a separate LMFDB-independent referee (Stage 6; `oracle/build_belabas.sh`).
- **LMFDB**: fetched by `oracle/fetch_lmfdb.py` into `data/` with recorded query, date, and SHA-256; tests read only the local cache. Used for: cubic field counts (Stage 6), sanity metadata (Stage 5).
- **Odlyzko tables**: mirrored into `data/odlyzko/` with checksums; `anchor_zeros_match_odlyzko` compares to ≥ 8 decimals.
- Missing oracle ⇒ SKIP with visible message; CI treats SKIP of oracle tests as warning, SKIP of anything else as failure.

## 5. JSON / visualization contract

All schemas versioned in `docs/schemas/*.schema.json`; every emitted file carries `{"schema": "<name>/<version>", "generated_by": "<git describe>", "params": {...}}`. Emitters live in `src/emit/`; a `validate-json` ctest target checks every emitted file against its schema.

| File | Producer stage | Viewer | Content |
|---|---|---|---|
| `linking_matrix.json` | 1 | `viz/linking.html` | primes, bit-rows (base64), density stats |
| `linking_graph.json` | 1 | `viz/linking.html` | nodes/edges for force layout (thresholded prime range) |
| `borromean.json` | 2 | `viz/borromean.html` | verified triples, symbol values, invariance-test provenance |
| `classgroups.json` | 3 | `viz/classgroups.html` | per-D: invariants, 2-rank, 4-rank (both computations), match flag |
| `cs_partition.json` | 4 | `viz/cs.html` | per prime set: LHS, RHS as complex pairs, |LHS−RHS|, linking-det, phase class |
| `zeros.json`, `psi_reconstruction.json` | 5 | `viz/explicit_formula.html` | zeros (+ Odlyzko delta), ψ(x) frames per zero count for the animation |
| `dyn_zeta.json` | 5 | `viz/explicit_formula.html` | model-flow product vs. determinant values |
| `dw_s3.json` | 6 | `viz/dw.html` | per prime set S: t, k, c, image decomposition of \|Hom\|, Z_DW (exact rational), N_S₃ vs t |
| `verification.json` | all | `viz/index.html` | dashboard: suite status per stage |

Viewers: plain HTML + D3 v7 from CDN, one file per viewer, no bundler. Shared style in `viz/common.css`. Every viewer renders from JSON only and shows the JSON's `params` block so a screenshot is self-documenting.

**Published-site data (GitHub Pages).** `viz/data/*.json` is a **committed snapshot** (deliberately *un-gitignored*), not a deploy-time emit: GitHub Pages serves the repo from `main` at root, so the live explainer deck (`docs/deck/index.html`) embeds the viewers by iframe (`../../viz/*.html`) and they fetch this committed JSON — a reader of the public site gets real data with no build step. The snapshot is regenerated by `./build/bin/at emit --stage <N> --out viz/data` at the parameters declared in each file's `params` block (linking `--bound 500`, borromean `--bound 200`, classgroups `--bound 20000`, cs `--bound 2000000`, zeros `--bound 500`, dw `--cubic-cache data/cubic/s3_counts.txt`). Files are small text (largest ~132 KB, none > 1 MB; publication-audit.md).

**Freshness contract (enforced, not manual).** The committed snapshot **must be byte-identical to `at emit` output at HEAD**, at the parameters in its own `params` blocks. This is enforced by the `freshness`-labelled ctest (`verify/freshness_check.py`), which runs on every push in CI — it re-emits every stage into a temp dir and byte-compares. Emit is deterministic (a tested property, `invariance_build_parallel_vs_serial`), so byte-identical is the bar; the one nondeterministic field, `generated_by` (a runtime `git describe`), is normalized structurally (checked present + well-formed, then blanked before the compare) — no other field is excused, and there is no fuzzy payload comparison. **Drift is a build failure; the fix is re-emitting and committing (or fixing the emitter), never editing the JSON by hand.** Hand-editing the snapshot JSON is this mechanism's Rule 1 violation. (This retired the earlier "synced manually" mode, whose failure was: change an emitter, forget to re-emit, and the public deck renders stale data while the suite stays green.)

## 6. Performance notes

- Stage 1 matrix to prime bound 10⁶ (~39k primes ≡ 1 mod 4): ~7.6×10⁸ symbol evaluations for the full matrix — fine single-threaded with Montgomery modpow, embarrassingly parallel if wanted (row-block `std::async`; determinism preserved since entries are pure).
- F2 rank on bit-packed rows is word-parallel; 39k×39k rank is seconds. Submatrix ranks (Stage 3, thousands of discriminants) dominate — batch them.
- Stage 2 conic solutions: Cremona–Rusin is near-instant; the Holzer brute twin is O(√(p₁p₂)) and only run inside `twin_` tests at moderate sizes.
- Stage 4 LHS is exponential in |S| by design (that's what makes RHS a theorem); cap |S| ≈ 12–14 and document the wall.
- Stage 5 Riemann–Siegel to the first ~10⁴ zeros with the standard correction terms is double-precision territory; verify precision against Odlyzko before trusting Z(t) signs near close zero pairs (Lehmer-like regions get an explicit test).

## 7. Stage acceptance criteria

| Stage | Green means |
|---|---|
| 0 | `twin_legendre` exhaustive to 10⁶; `theorem_quadratic_reciprocity` + supplements exhaustive; Tonelli–Shanks residue/non-residue tests; `oracle_kronecker` sample |
| 1 | matrix build with twin cross-check; symmetry sweep; density/stat report emitted; linking viewer renders |
| 2 | `anchor_redei_13_61_937_is_minus_1`; `invariance_redei_solution_choice` + `invariance_sqrt_branch`; `theorem_redei_reciprocity_s3` over all in-range valid triples; Borromean scan table emitted; pinning log exists in `docs/notes/` |
| 3 | class groups match PARI over sweep; `theorem_genus_2rank`; `theorem_redei_reichardt_4rank` (form-side vs linking-side) over ≥ 10³ discriminants; scatter viewer shows the diagonal |
| 4 | pinning log complete (paper→code map for every object); `theorem_ckkppy_partition_identity` at p=2 across sampled prime sets, then odd p; LHS/RHS built in separate TUs with disjoint authorship sessions; phase-plot viewer |
| 5 | zeros match Odlyzko ≥ 8 dp; ψ reconstruction error decreases as documented; model-flow product=determinant twin; animation renders |
| 6 | S₃ counts match LMFDB cubic tables over the tested conductor range |

## 8. Risks and mitigations

- **Rédei normalization (Stage 2)** — highest mathematical risk. Mitigation: implement directly from [S22]/[C07] with the pinning log; the invariance tests are designed to catch normalization errors structurally rather than by anchor luck.
- **Stage 4 pinning** — highest effort risk; the cohomology-to-enumeration translation may take longer than all coding combined. Mitigation: treat the pinning log as the deliverable; LHS and RHS pinned in separate sessions; do p=2 completely before touching odd p.
- **Silent convention-fitting** — the failure mode that invalidates everything. Mitigation: CLAUDE.md rule 1; anchor tests are necessary but never sufficient — a stage is green only with its invariance and theorem sweeps.
- **Floating point in Stage 5** — Z(t) sign errors near close zeros. Mitigation: interval-style double checks (recompute with perturbed t), Odlyzko referee, explicit Lehmer-pair test.
