# CLAIMS ledger — every data-claim in the deck, sourced

One row per `data-claim="ID"` in `docs/deck/index.html`. The `deck` ctest check
(`verify/deck_check.py`) fails if any ID in the built HTML lacks a row here.

**Register discipline** (enforced at build): our results are **"verified
numerically over N cases"**, never "proved" (that word is reserved for literature
theorems, with citation). N is pulled from emitted JSON or test output at build
time — never retyped, never rounded. Empirics are labeled *observed / unasserted*.

Status vocabulary: `verified` (a test asserts it, with N) · `published` (a
literature theorem, cited) · `matches-published` (our number equals a published
value) · `observed` (an empiric, not asserted as a theorem) · `documented`
(a fact recorded in a log, not a numeric test).

_Counts below marked ⟨pull⟩ are confirmed against test output / emitted JSON when
the deck HTML is built; the source (test name / pinning-§ / paper eq) is fixed now._

| id | claim (as it will read on the slide) | status | source |
|---|---|---|---|
| `qr-exhaustive` | Quadratic reciprocity + both supplements hold over all coprime odd pairs to 10⁶ | verified | `stage0.theorem_quadratic_reciprocity`, `…_supplement_first_minus1`, `…_supplement_second_two` (exhaustive to 10⁶); twin `twin_legendre_euler_vs_reciprocity` |
| `linking-symmetry` | The prime linking matrix is symmetric: `(p/q)=(−1)^{lk₂(p,q)}` | verified | `stage1.theorem_matrix_symmetry_bitrows` (full matrix, no sampling); `twin_linking_euler_vs_reciprocity` ⟨pull n_primes from linking_matrix.json⟩ |
| `redei-s3` | The Rédei symbol has full S₃ symmetry (Rédei reciprocity) over every valid triple in range | verified | `stage2.theorem_redei_reciprocity_s3` ⟨pull triple/permutation count from test output⟩ |
| `redei-anchor` | `[13,61,937] = −1` (Stevenhagen's worked value) | matches-published | `stage2.anchor_redei_13_61_937_is_minus_1`; normalization pinned to [S22], stage2-pinning §8 |
| `four-rank` | Rédei–Reichardt: narrow-class-group 4-rank (form side) = F₂-rank of the linking matrix (linking side) | verified | `stage3.theorem_redei_reichardt_4rank` over ≥10³ discriminants ⟨pull N from classgroups.json⟩; oracle `bnfnarrow` |
| `cs-identity` | Mod-2 arithmetic Chern–Simons `Z_c`: brute character sum (LHS) = closed form (RHS) | verified | `stage4.theorem_ckkppy_partition_identity`, 386 gated tuples (default); LHS/RHS in disjoint-session TUs ⟨pull N from cs_partition.json⟩ |
| `zeros-odlyzko` | Our ζ zeros match Odlyzko to ≥ 8 dp over the first 10⁴; max deviation 2.91e-9 (the table's own floor) | verified | `stage5.anchor_zeros_match_odlyzko --extended` (10025 zeros); completeness `theorem_zero_count_turing_complete` ⟨pull max-dev + count from test output⟩ |
| `psi-midpoint` | The explicit-formula series converges to the jump midpoint ψ₀(8)=ψ(8)−log2/2, not ψ(8) | verified | `stage5.anchor_psi_midpoint_at_prime_power`; erratum open-questions R6 |
| `ruelle-twin` | Ruelle zeta: Euler product over closed orbits = 1/det(I−L_s) | verified | `stage5.twin_ruelle_orbit_vs_determinant` (agreement ~1e-15 over an s-grid) |
| `dw-decomposition` | `Z_DW = 1/6 + (2^t−1)/2 + (3^k−1)/6 + c`, exact rational, every instance | verified | `stage6.theorem_dw_mass_formula` (16 sweep instances); `theorem_dw_s3_decomposition` |
| `belabas-twin` | Cubic iso-class count `c` agrees between PARI `nflist` and Belabas's `cubic` | verified | `stage6.twin_cubic_count_belabas_vs_pari` (14 feasible sweep sets; 2 recorded skipped, Dmax too large) |
| `errata-count` | Every party was caught at least once; every catch a computation or a citation | documented | `docs/ERRATA.md` (15 entries; authority-catches: 0) |
| `suite-total` | The full suite is green | verified | `ctest` — 63 tests, 0 failing (10 oracle-refereed) ⟨pull totals from ctest summary at build⟩ |

## Empirics (observed / unasserted — NOT theorems)

These appear on slides labeled *observed*, never "verified" or "proved":

| id | observed value | source |
|---|---|---|
| `obs-borromean-rate` | Borromean-triple rate ≈ 51.6% among in-range valid triples | borromean.json aggregate ⟨pull⟩ |
| `obs-pell-fraction` | negative-Pell-solvable fraction 0.6692 (vs Koymans–Pagano asymptotic 0.5805) | cs_partition.json `n_excluded_pell` / n ⟨pull⟩ |
| `obs-ns3-growth` | N_{S₃} grows with t (0 at t=1 up to 7 at t=4 in the sweep) | dw_s3.json instances ⟨pull⟩ |
