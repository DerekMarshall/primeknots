# Extended run — `ctest --extended` across all stages

Full witness of the opt-in extended sweep (larger ranges/seeds than the
default suite), run by invoking each `verify_stageN --extended --duration=true`
directly (ctest cannot forward `--extended` to test commands). Case counts and
per-test wall times are pulled verbatim from doctest output — not retyped.

- date: 2026-07-08
- machine: Apple clang, Release build
- **result: all 7 stages exit 0 — zero failures.**


## stage0

| test | duration | cases (from MESSAGE) |
|---|---|---|
| `twin_modmul_montgomery_vs_direct` | 6.873215 s | cases: 5004800 (modpow twin comparisons) |
| `twin_is_prime_vs_sieve` | 2.172902 s | cases: 20000001 integers in [0, 20000000]; primes found: 1270607 |
| `oracle_kronecker` | 0.063906 s | oracle: using gp at /opt/homebrew/bin/gp ; cases: 500 Jacobi values matched PARI/GP kronecker() ; cases: 108 Legendre values matched PARI/GP kronecker() |
| `oracle_modpow` | 0.019407 s | cases: 363 modpow values matched PARI/GP Mod(a,m)^e |
| `twin_legendre_euler_vs_reciprocity` | 6.907544 s | cases: 53165634 symbol agreements over all 664579 primes < 10000000 (exhaustive) |
| `twin_jacobi_vs_legendre_factorization` | 0.555041 s | cases: 3899814 over 100001 odd moduli n ≤ 200001 |
| `theorem_quadratic_reciprocity` | 0.777501 s | cases: 5260146 odd prime pairs; 1296855 with both ≡ 1 (mod 4) checked symmetric |
| `theorem_supplement_first_minus1` | 0.097230 s | cases: 664578 odd primes < 10000000 |
| `theorem_supplement_second_two` | 0.095257 s | cases: 664578 odd primes < 10000000 |
| `twin_sqrt_mod_vs_legendre` | 0.779664 s | cases: 2408735 over 41538 primes < 500000 (1347831 roots verified, 1060904 non-residues detected) |

**test cases:       10 |       10 passed | 0 failed | 0 skipped** · **assertions: 95127168 | 95127168 passed | 0 failed |**

wall time: **19 s**

## stage1

| test | duration | cases (from MESSAGE) |
|---|---|---|
| `twin_f2rank_packed_vs_naive` | 0.140719 s | cases: 50000 random F2 matrices; rank(packed) == rank(naive) |
| `twin_submatrix_vs_direct_lookup` | 0.232106 s | cases: 10370859 submatrix entries over 20000 random extractions |
| `theorem_linking_vertex_set` | 13.906967 s | cases: 39175 primes ≡ 1 (mod 4) ≤ 1000000 (== π(x;4,1) from gp) |
| `theorem_quadratic_reciprocity_at_scale` | 13.708228 s | cases: 767320725 symmetric pairs over 39175 primes ≤ 1000000 (reciprocity at scale) |
| `theorem_matrix_symmetry_bitrows` | 17.729959 s | cases: 39175 bit-rows vs columns; M == Mᵀ over 39175×39175 (1534680625 bits, no sampling) |
| `twin_linking_euler_vs_reciprocity` | 0.211732 s | cases: 2039800 entries recomputed with the reciprocity twin (200-prime block exhaustive + 2000000 sampled) |
| `invariance_build_parallel_vs_serial` | 7.817871 s | cases: 8977 primes; serial == parallel(hw) == parallel(8), bit-identical |
| `oracle_kronecker_linking` | 0.084695 s | cases: 20000 linking entries matched PARI/GP kronecker() over 39175 primes |

**test cases:         8 |         8 passed | 0 failed | 0 skipped** · **assertions: 780057279 | 780057279 passed | 0 failed |**

wall time: **54 s**

## stage2

| test | duration | cases (from MESSAGE) |
|---|---|---|
| `twin_conic_fast_vs_holzer` | 0.210221 s | cases: 53956 prime pairs (26608 soluble, 27348 insoluble); fast/brute/Legendre agree |
| `theorem_conic_multiple_distinct_solutions` | 0.055722 s | cases: 26608 soluble pairs, 133040 distinct primitive solutions total |
| `anchor_conic_13_61_has_primitive_solution` | 0.000007 s | cases: 1 — (13,61) conic primitive solution verified |
| `twin_omega_square_classes_mod4` | 0.000291 s | cases: 329 primes; closed-form vs generic-mul square classes agree |
| `oracle_conic_qfsolve` | 0.186980 s | cases: 4000 conics refereed by gp qfsolve (1966 soluble); solubility + gp solution validity confirmed |
| `invariance_redei_solution_choice` | 7.596672 s | cases: 28097929 solution comparisons over 702788 triples; 352361 with p3≡5 (mod 8); 352361 of those saw ≥2 distinct 2-adic classes (non-vacuous) |
| `invariance_sqrt_branch` | 3.487780 s | cases: 8429665 (±s) branch comparisons; both roots agree |
| `theorem_redei_degenerate_resolve` | 35.814168 s | cases: 56337 primitive solutions with p3\|z exercised the R1 guard |
| `anchor_redei_13_61_937_is_minus_1` | 0.000011 s | cases: 1 — [13,61,937] = -1 |
| `theorem_redei_reciprocity_s3` | 9.888178 s | cases: 702788 valid triples × 6 permutations = 4216728 symbol evaluations; 2169320 permutations with a ≡5 (mod 8) third argument (N4-sensitive) |

**test cases:       10 |       10 passed | 0 failed | 0 skipped** · **assertions: 88054417 | 88054417 passed | 0 failed |**

wall time: **57 s**

## stage3

| test | duration | cases (from MESSAGE) |
|---|---|---|
| `oracle_narrow_class_number` | 6.138901 s | cases: 1394 narrow class numbers matched PARI/GP bnfnarrow |
| `oracle_narrow_structure_match` | 1.074651 s | cases: 249 narrow class-group structures matched bnfnarrow.cyc |
| `twin_classno_cycles_vs_composition` | 7.713028 s | cases: 1394 discriminants; h⁺ by cycle-count == h⁺ by composition |
| `theorem_reduce_is_reduced_and_equivalent` | 4.734198 s | cases: 155490 forms reduced; each reduced & class-preserving |
| `theorem_compose_group_axioms` | 3.001512 s | cases: 6645 elements; 183429 associativity triples; 1373 discriminants with >1 class |
| `theorem_genus_2rank` | 4.621786 s | cases: 1986 discriminants; 2-rank(Cl⁺) == t−1 (Gauss) |
| `theorem_redei_matrix_rows_sum_zero` | 0.000471 s | cases: 1986 Rédei matrices; every row sums to 0 |
| `theorem_redei_reichardt_4rank` | 4.773778 s | cases: 1986 discriminants; 4-rank strata r4=0:1085 r4=1:776 r4≥2:125 |

**test cases:      8 |      8 passed | 0 failed | 0 skipped** · **assertions: 882969 | 882969 passed | 0 failed |**

wall time: **33 s**

## stage4

| test | duration | cases (from MESSAGE) |
|---|---|---|
| `anchor_ckkppy_hirano_examples` | 0.000043 s | cases: 3 anchors — Z_c(√65)=0, Z_c(√(5·29·37))=0, Z_c(√(5·13·73))=4 |
| `theorem_ckkppy_partition_identity` | 0.071431 s | cases: 4810 gated tuples (LHS==RHS); 2715 excluded by pell_neg (R3); strata even[ae=280,ao=563,mx=3078] odd[ae=201,ao=0,mx=688] |
| `invariance_s_ordering` | 0.068197 s | cases: 14430 permutations; lhs and rhs both S-ordering invariant |
| `invariance_lhs_character_basis` | 0.067193 s | cases: 4810 tuples; lhs == independent character-sum enumeration |

**test cases:     4 |     4 passed | 0 failed | 0 skipped** · **assertions: 38500 | 38500 passed | 0 failed |**

wall time: **0 s**

## stage5

| test | duration | cases (from MESSAGE) |
|---|---|---|
| `anchor_psi_midpoint_at_prime_power` | 0.000120 s | — |
| `theorem_explicit_formula_convergence` | 5.142028 s | — |
| `twin_theta_asymptotic_vs_lgamma` | 0.000020 s | — |
| `twin_z_riemann_siegel_vs_euler_maclaurin` | 0.003849 s | — |
| `anchor_first_zeros_published_values` | 0.000074 s | — |
| `anchor_N_main_term_turing_necessity` | 0.056527 s | — |
| `theorem_zero_count_turing_complete` | 8.237004 s | — |
| `anchor_lehmer_pair_7005` | 0.159605 s | — |
| `invariance_zero_bracket_independence` | 0.770807 s | — |
| `anchor_zeros_match_odlyzko` | 35.069584 s | — |
| `twin_ruelle_orbit_vs_determinant` | 0.001480 s | — |
| `theorem_bowen_lanford_orbit_counts` | 0.000013 s | — |
| `anchor_golden_mean_pole` | 0.000239 s | — |

**test cases: 13 | 13 passed | 0 failed | 0 skipped** · **assertions: 62 | 62 passed | 0 failed |**

wall time: **50 s**

## stage6

| test | duration | cases (from MESSAGE) |
|---|---|---|
| `invariance_s_ordering` | 0.000410 s | — |
| `anchor_dw_c_zero_and_nonzero` | 0.000048 s | — |
| `theorem_dw_mass_formula` | 0.000053 s | — |
| `theorem_signature_all_totally_real` | 0.000042 s | — |
| `theorem_dw_s3_decomposition` | 0.000036 s | — |
| `twin_cubic_count_belabas_vs_pari` | 0.000230 s | — |

**test cases:   6 |   6 passed | 0 failed | 0 skipped** · **assertions: 242 | 242 passed | 0 failed |**

wall time: **0 s**
