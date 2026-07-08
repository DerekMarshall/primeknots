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

Counts below are pulled from the extended-run test output (`docs/notes/
extended-run-2026-07-08.md`) and the committed emitted JSON (`viz/data/`) — the
strongest reproducible numbers, not retyped by hand.

**What the `deck` ctest check (`verify/deck_check.py`) verifies — and what it
cannot.** It enforces (1) every `data-claim="ID"` in the deck HTML has a row
here, and (2) every literal integer of ≥ 5 digits appearing in a slide's claim
badge also appears (comma-stripped) in that ID's row — so a slide cannot cite a
significant count absent from its ledger row. It does **not** verify prose
*attribution*: that a coverage bound (e.g. "to 10⁷") is applied to the right
object, or that a value is credited to the right source. Those are semantic and
remain a human/referee responsibility. The demonstrated example is ERRATA #16:
the Stage-0 slide misattributed the 10⁷ bound to the reciprocity check (which was
exhaustive only to 3×10⁴) — every number was real and in the row, so the
existence check passed; the external referee caught the misattribution. Register
discipline is therefore machine-enforced for *numbers present* and referee-
enforced for *numbers correctly attributed*.

| id | claim (as it will read on the slide) | status | source (N pulled from output/JSON) |
|---|---|---|---|
| `qr-exhaustive` | Reciprocity law over all pairs of odd primes < 3×10⁴; both supplements over all odd primes < 10⁷; the Euler-vs-reciprocity twin exhaustive to 10⁷ — three distinct coverages | verified | `stage0.theorem_quadratic_reciprocity` — 5,260,146 pairs to 3×10⁴ (1,296,855 both ≡1 mod4); `…_supplement_first_minus1`/`…_second_two` over 664,578 odd primes < 10⁷; twin `twin_legendre_euler_vs_reciprocity` 53,165,634 agreements to 10⁷ |
| `linking-symmetry` | The prime linking matrix is symmetric: `(p/q)=(−1)^{lk₂(p,q)}` | verified | `stage1.theorem_matrix_symmetry_bitrows` — 39175×39175 over primes ≤10⁶ (1,534,680,625 bits, no sampling); `theorem_quadratic_reciprocity_at_scale` 767,320,725 pairs |
| `redei-s3` | The Rédei symbol has full S₃ symmetry (Rédei reciprocity) over every valid triple in range | verified | `stage2.theorem_redei_reciprocity_s3` — 702,788 valid triples × 6 permutations = 4,216,728 symbol evaluations (2,169,320 N4-sensitive) |
| `redei-anchor` | `[13,61,937] = −1`, the standard Borromean triple (Morishita [M12]) | matches-published | `stage2.anchor_redei_13_61_937_is_minus_1`; value attributed to [M12] (RESEARCH.md §10 "the standard Borromean triple [M12]"; stage2-pinning line 181); *normalization* pinned to [S22]/[C07] |
| `four-rank` | Rédei–Reichardt: narrow-class-group 4-rank (form side) = F₂-rank of the linking matrix (linking side) | verified | `stage3.theorem_redei_reichardt_4rank` — 1,986 discriminants (strata r4=0:1085, r4=1:776, r4≥2:125); oracle `bnfnarrow` |
| `cs-identity` | Mod-2 arithmetic Chern–Simons `Z_c`: brute character sum (LHS) = closed form (RHS) | verified | `stage4.theorem_ckkppy_partition_identity` — 386 gated tuples (default) / 4,810 (extended); LHS/RHS authored in disjoint-session TUs, agreed on first comparison |
| `zeros-odlyzko` | Our ζ zeros match Odlyzko to ≥ 8 dp over the first 10⁴; max deviation 2.91e-9 (the table's own floor, PARI-confirmed) | verified | `stage5.anchor_zeros_match_odlyzko --extended` — 10,025 zeros compared, max \|ours−Odlyzko\| = 2.91129e-9; completeness `theorem_zero_count_turing_complete` |
| `psi-midpoint` | The explicit-formula series converges to the jump midpoint ψ₀(8)=ψ(8)−log2/2, not ψ(8) | verified | `stage5.anchor_psi_midpoint_at_prime_power`; erratum open-questions R6 |
| `ruelle-twin` | Ruelle zeta: Euler product over closed orbits = 1/det(I−L_s) | verified | `stage5.twin_ruelle_orbit_vs_determinant` — agreement ~9e-16 over the s-grid (both golden-mean roofs) |
| `dw-decomposition` | `Z_DW = 1/6 + (2^t−1)/2 + (3^k−1)/6 + c`, exact rational, every instance | verified | `stage6.theorem_dw_mass_formula` — 16 sweep instances (exact rational); `theorem_dw_s3_decomposition` |
| `belabas-twin` | Cubic iso-class count `c` agrees between PARI `nflist` and Belabas's `cubic` | verified | `stage6.twin_cubic_count_belabas_vs_pari` — 14 feasible sweep sets agree; 2 recorded skipped (Dmax too large) |
| `errata-count` | Every party was caught at least once; every catch a computation or a citation | documented | `docs/ERRATA.md` — 18 entries (the slide shows a selection); authority-catches: 0 |
| `suite-total` | The full suite is green | verified | `ctest` — 63 tests, 0 failing (10 oracle-refereed); extended run all 7 stages exit 0 |

## Empirics (observed / unasserted — NOT theorems)

These appear on slides labeled *observed*, never "verified" or "proved":

| id | observed value | source |
|---|---|---|
| `obs-borromean-rate` | Borromean-triple rate 63/122 = 51.6% among in-range valid triples (committed snapshot, bound 200; 51.1% at bound 2000) | borromean.json aggregate |
| `obs-pell-fraction` | negative-Pell-solvable fraction 1308/1965 = 0.666 at disc_bound 2·10⁶ (vs Koymans–Pagano asymptotic 0.5805; convergence is slow) | cs_partition.json `n_included`/(`n_included`+`n_excluded_pell`) |
| `obs-ns3-growth` | N_{S₃} grows with t: 0 at t=1, up to 7 at t=4 in the sweep | dw_s3.json instances |
