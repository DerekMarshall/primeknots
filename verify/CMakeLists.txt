# Stage 0 verification binary (one binary per stage, ARCHITECTURE.md §3).
add_executable(verify_stage0
  verify_main.cpp
  stage0/core_tests.cpp
  stage0/symbols_tests.cpp
  stage0/oracle_tests.cpp)
target_include_directories(verify_stage0 PRIVATE
  ${CMAKE_SOURCE_DIR}/third_party   # doctest/doctest.h
  ${CMAKE_CURRENT_SOURCE_DIR})      # harness.h, stage0/util.h
target_link_libraries(verify_stage0 PRIVATE at_symbols at_oracle)  # at_symbols pulls at_core

# --- ctest registration ----------------------------------------------------
# Registered manually (not via doctest_discover_tests) so each test case gets
# its ctest label and the oracle clean-SKIP exit-code contract is explicit.
# See docs/notes/open-questions.md R4.

# Non-oracle Stage 0 cases: labelled `stage0`.
set(STAGE0_CASES
  twin_modmul_montgomery_vs_direct
  twin_is_prime_vs_sieve
  twin_legendre_euler_vs_reciprocity
  twin_jacobi_vs_legendre_factorization
  twin_sqrt_mod_vs_legendre
  theorem_quadratic_reciprocity
  theorem_supplement_first_minus1
  theorem_supplement_second_two)
foreach(case ${STAGE0_CASES})
  add_test(NAME stage0.${case}
           COMMAND verify_stage0 --test-case=${case})
  set_tests_properties(stage0.${case} PROPERTIES LABELS "stage0")
endforeach()

# Oracle referees: report SKIP (exit 77) when gp is absent, PASS/FAIL otherwise.
# RESOURCE_LOCK "gp" serializes gp-spawning tests so a burst of simultaneous gp
# process launches under `ctest -j` can't contend (an observed startup flake).
foreach(ocase oracle_kronecker oracle_modpow)
  add_test(NAME stage0.${ocase}
           COMMAND verify_stage0 --test-case=${ocase})
  set_tests_properties(stage0.${ocase} PROPERTIES
    LABELS "stage0;oracle"
    SKIP_RETURN_CODE 77
    RESOURCE_LOCK "gp")
endforeach()

# Clean-SKIP contract test: run the oracle suite with gp scrubbed from PATH and
# assert a *visible* SKIP (exit 77 + "[SKIP]" line), never a silent pass or a
# failure. This test itself always PASSES — it verifies the mechanism.
add_test(NAME stage0.oracle_clean_skip_when_gp_absent
         COMMAND ${CMAKE_COMMAND}
           -DORACLE_BIN=$<TARGET_FILE:verify_stage0>
           -DEMPTY_DIR=${CMAKE_CURRENT_BINARY_DIR}/no_gp_path
           -P ${CMAKE_CURRENT_SOURCE_DIR}/oracle_skip_check.cmake)
set_tests_properties(stage0.oracle_clean_skip_when_gp_absent PROPERTIES
  LABELS "stage0;oracle")

# --- Stage 1 ---------------------------------------------------------------
add_executable(verify_stage1
  verify_main.cpp
  stage1/linking_tests.cpp
  stage1/f2matrix_tests.cpp
  stage1/oracle_tests.cpp)
target_include_directories(verify_stage1 PRIVATE
  ${CMAKE_SOURCE_DIR}/third_party   # doctest/doctest.h
  ${CMAKE_CURRENT_SOURCE_DIR})      # harness.h, stage1/fixture.h
target_link_libraries(verify_stage1 PRIVATE at_linking at_oracle)

set(STAGE1_CASES
  theorem_linking_vertex_set
  theorem_quadratic_reciprocity_at_scale
  theorem_matrix_symmetry_bitrows
  twin_linking_euler_vs_reciprocity
  invariance_build_parallel_vs_serial
  twin_f2rank_packed_vs_naive
  twin_submatrix_vs_direct_lookup)
foreach(case ${STAGE1_CASES})
  add_test(NAME stage1.${case} COMMAND verify_stage1 --test-case=${case})
  set_tests_properties(stage1.${case} PROPERTIES LABELS "stage1")
endforeach()

add_test(NAME stage1.oracle_kronecker_linking
         COMMAND verify_stage1 --test-case=oracle_kronecker_linking)
set_tests_properties(stage1.oracle_kronecker_linking PROPERTIES
  LABELS "stage1;oracle"
  SKIP_RETURN_CODE 77
  RESOURCE_LOCK "gp")

# validate-json: emit Stage 1 JSON and validate every file against its schema.
# Prefers the project .venv interpreter (which has jsonschema); otherwise falls
# back to system python3 and SKIPs (77) if jsonschema is unavailable.
if(EXISTS ${CMAKE_SOURCE_DIR}/.venv/bin/python3)
  set(AT_VALIDATE_PY ${CMAKE_SOURCE_DIR}/.venv/bin/python3)
else()
  find_program(AT_VALIDATE_PY NAMES python3 python)
endif()
if(AT_VALIDATE_PY)
  add_test(NAME stage1.validate_json
           COMMAND ${AT_VALIDATE_PY} ${CMAKE_CURRENT_SOURCE_DIR}/validate_json.py
             --at $<TARGET_FILE:at>
             --schema-dir ${CMAKE_SOURCE_DIR}/docs/schemas
             --out ${CMAKE_CURRENT_BINARY_DIR}/emit_check)
  set_tests_properties(stage1.validate_json PROPERTIES
    LABELS "stage1;validate-json"
    SKIP_RETURN_CODE 77)
endif()

# --- Stage 2 (PHASE 1: convention-free conic solver only) ------------------
add_executable(verify_stage2
  verify_main.cpp
  stage2/conic_tests.cpp
  stage2/omega_tests.cpp
  stage2/redei_tests.cpp
  stage2/oracle_tests.cpp)
target_include_directories(verify_stage2 PRIVATE
  ${CMAKE_SOURCE_DIR}/third_party
  ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(verify_stage2 PRIVATE at_redei at_oracle)

set(STAGE2_CASES
  twin_conic_fast_vs_holzer
  theorem_conic_multiple_distinct_solutions
  anchor_conic_13_61_has_primitive_solution
  twin_omega_square_classes_mod4
  invariance_redei_solution_choice
  invariance_sqrt_branch
  theorem_redei_degenerate_resolve
  anchor_redei_13_61_937_is_minus_1
  theorem_redei_reciprocity_s3)
foreach(case ${STAGE2_CASES})
  add_test(NAME stage2.${case} COMMAND verify_stage2 --test-case=${case})
  set_tests_properties(stage2.${case} PROPERTIES LABELS "stage2")
endforeach()

add_test(NAME stage2.oracle_conic_qfsolve
         COMMAND verify_stage2 --test-case=oracle_conic_qfsolve)
set_tests_properties(stage2.oracle_conic_qfsolve PROPERTIES
  LABELS "stage2;oracle"
  SKIP_RETURN_CODE 77
  RESOURCE_LOCK "gp")

# --- Stage 3 (PHASE 1: convention-free forms layer only) -------------------
add_executable(verify_stage3
  verify_main.cpp
  stage3/qform_tests.cpp
  stage3/redei_reichardt_tests.cpp
  stage3/oracle_tests.cpp)
target_include_directories(verify_stage3 PRIVATE
  ${CMAKE_SOURCE_DIR}/third_party
  ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(verify_stage3 PRIVATE at_classgroup at_oracle)

set(STAGE3_CASES
  twin_classno_cycles_vs_composition
  theorem_reduce_is_reduced_and_equivalent
  theorem_compose_group_axioms
  theorem_genus_2rank
  theorem_redei_matrix_rows_sum_zero
  theorem_redei_reichardt_4rank)
foreach(case ${STAGE3_CASES})
  add_test(NAME stage3.${case} COMMAND verify_stage3 --test-case=${case})
  set_tests_properties(stage3.${case} PROPERTIES LABELS "stage3")
endforeach()

foreach(ocase oracle_narrow_class_number oracle_narrow_structure_match)
  add_test(NAME stage3.${ocase} COMMAND verify_stage3 --test-case=${ocase})
  set_tests_properties(stage3.${ocase} PROPERTIES
    LABELS "stage3;oracle"
    SKIP_RETURN_CODE 77
    RESOURCE_LOCK "gp")
endforeach()

# --- Stage 4 (arithmetic Chern-Simons / DW, Hirano mod-2 route) -------------
add_executable(verify_stage4
  verify_main.cpp
  stage4/invariance_tests.cpp
  stage4/comparison_tests.cpp)
target_include_directories(verify_stage4 PRIVATE
  ${CMAKE_SOURCE_DIR}/third_party
  ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(verify_stage4 PRIVATE at_cs at_classgroup)

set(STAGE4_CASES
  invariance_s_ordering
  invariance_lhs_character_basis
  anchor_ckkppy_hirano_examples
  theorem_ckkppy_partition_identity)
foreach(case ${STAGE4_CASES})
  add_test(NAME stage4.${case} COMMAND verify_stage4 --test-case=${case})
  set_tests_properties(stage4.${case} PROPERTIES LABELS "stage4")
endforeach()

# --- Stage 5 (zeta zeros, explicit formula, Ruelle-zeta model twin) ---------
add_executable(verify_stage5
  verify_main.cpp
  stage5/rs_zero_tests.cpp
  stage5/explicit_formula_tests.cpp
  stage5/ruelle_tests.cpp)
target_include_directories(verify_stage5 PRIVATE
  ${CMAKE_SOURCE_DIR}/third_party
  ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(verify_stage5 PRIVATE at_zeta)
# The Odlyzko table is read by the oracle test only; its absence ⇒ clean SKIP.
target_compile_definitions(verify_stage5 PRIVATE
  AT_ODLYZKO_PATH="${CMAKE_SOURCE_DIR}/data/odlyzko/zeros1")

set(STAGE5_CASES
  twin_theta_asymptotic_vs_lgamma
  twin_z_riemann_siegel_vs_euler_maclaurin
  twin_ruelle_orbit_vs_determinant
  theorem_zero_count_turing_complete
  theorem_explicit_formula_convergence
  theorem_bowen_lanford_orbit_counts
  anchor_first_zeros_published_values
  anchor_N_main_term_turing_necessity
  anchor_lehmer_pair_7005
  anchor_psi_midpoint_at_prime_power
  anchor_golden_mean_pole
  invariance_zero_bracket_independence)
foreach(case ${STAGE5_CASES})
  add_test(NAME stage5.${case} COMMAND verify_stage5 --test-case=${case})
  set_tests_properties(stage5.${case} PROPERTIES LABELS "stage5")
endforeach()

# Odlyzko oracle: reads only the local cached table (no subprocess), SKIPs (77)
# with a visible message when the table is absent. No gp ⇒ no RESOURCE_LOCK.
add_test(NAME stage5.anchor_zeros_match_odlyzko
         COMMAND verify_stage5 --test-case=anchor_zeros_match_odlyzko)
set_tests_properties(stage5.anchor_zeros_match_odlyzko PROPERTIES
  LABELS "stage5;oracle"
  SKIP_RETURN_CODE 77)

# --- Stage 6 (untwisted S₃ Dijkgraaf-Witten counting vs cubic fields) --------
add_executable(verify_stage6
  verify_main.cpp
  stage6/dw_tests.cpp
  stage6/oracle_tests.cpp)
target_include_directories(verify_stage6 PRIVATE
  ${CMAKE_SOURCE_DIR}/third_party
  ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(verify_stage6 PRIVATE at_dw)
target_compile_definitions(verify_stage6 PRIVATE
  AT_CUBIC_CACHE="${CMAKE_SOURCE_DIR}/data/cubic/s3_counts.txt"
  AT_BELABAS_CACHE="${CMAKE_SOURCE_DIR}/data/belabas/cubic_s3.txt")

# All stage6 verify cases read committed caches only (no gp/binary at test time),
# so they run everywhere including CI. twin_cubic_count_belabas_vs_pari compares
# the two independent committed referee counts (Belabas `cubic` vs PARI nflist).
set(STAGE6_CASES
  twin_cubic_count_belabas_vs_pari
  invariance_s_ordering
  anchor_dw_c_zero_and_nonzero
  theorem_dw_mass_formula
  theorem_signature_all_totally_real
  theorem_dw_s3_decomposition)
foreach(case ${STAGE6_CASES})
  add_test(NAME stage6.${case} COMMAND verify_stage6 --test-case=${case})
  set_tests_properties(stage6.${case} PROPERTIES LABELS "stage6")
endforeach()

# PARI referee regen: re-run the nflist dual-strategy enumeration and diff the
# committed cache (guards staleness). SKIPs (77) if gp absent. RESOURCE_LOCK "gp".
if(AT_VALIDATE_PY)
  add_test(NAME stage6.oracle_pari_cubic_regen
           COMMAND ${AT_VALIDATE_PY} ${CMAKE_SOURCE_DIR}/oracle/compute_cubic_counts.py --check)
  set_tests_properties(stage6.oracle_pari_cubic_regen PROPERTIES
    LABELS "stage6;oracle"
    SKIP_RETURN_CODE 77
    RESOURCE_LOCK "gp"
    TIMEOUT 180          # backstop: fail fast if nflist ever hangs, never block the suite
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

  # Belabas referee regen: rebuild `cubic` and re-diff its committed cache. SKIPs
  # (77) if the binary/libpari is absent (e.g. in CI). Provides the independent
  # cross-check of the Belabas cache the twin compares against.
  add_test(NAME stage6.oracle_belabas_cubic_regen
           COMMAND ${AT_VALIDATE_PY} ${CMAKE_SOURCE_DIR}/oracle/belabas_cubic.py --check)
  set_tests_properties(stage6.oracle_belabas_cubic_regen PROPERTIES
    LABELS "stage6;oracle"
    SKIP_RETURN_CODE 77
    TIMEOUT 180
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

# --- M0 (murmurations: a_p machinery, convention-free) ----------------------
add_executable(verify_m0
  verify_main.cpp
  m0/ap_tests.cpp
  m0/oracle_tests.cpp)
target_include_directories(verify_m0 PRIVATE
  ${CMAKE_SOURCE_DIR}/third_party
  ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(verify_m0 PRIVATE at_ell at_oracle)

set(M0_CASES
  anchor_ap_11a1
  anchor_ap_bad_prime
  invariance_weierstrass_model)
foreach(case ${M0_CASES})
  add_test(NAME m0.${case} COMMAND verify_m0 --test-case=${case})
  set_tests_properties(m0.${case} PROPERTIES LABELS "m0")
endforeach()

# Oracle: our a_p refereed by PARI ellap. SKIPs (77) if gp absent; RESOURCE_LOCK
# serializes gp spawns under `ctest -j` (as elsewhere).
add_test(NAME m0.oracle_ellap COMMAND verify_m0 --test-case=oracle_ellap)
set_tests_properties(m0.oracle_ellap PROPERTIES
  LABELS "m0;oracle"
  SKIP_RETURN_CODE 77
  RESOURCE_LOCK "gp")

# Clean-SKIP contract for M0's oracle (per Stage 0 precedent): gp absent must give
# a visible [SKIP] + exit 77, never a silent pass. This test always PASSES.
add_test(NAME m0.oracle_clean_skip_when_gp_absent
         COMMAND ${CMAKE_COMMAND}
           -DORACLE_BIN=$<TARGET_FILE:verify_m0>
           -DTESTCASE=oracle_ellap
           -DEMPTY_DIR=${CMAKE_CURRENT_BINARY_DIR}/no_gp_path_m0
           -P ${CMAKE_CURRENT_SOURCE_DIR}/oracle_skip_check.cmake)
set_tests_properties(m0.oracle_clean_skip_when_gp_absent PROPERTIES LABELS "m0;oracle")

# --- deck: the explainer deck is an emitted artifact, validated like one -------
# Every data-claim="ID" in docs/deck/index.html must have a CLAIMS.md row.
if(AT_VALIDATE_PY)
  add_test(NAME deck.claims_sourced
           COMMAND ${AT_VALIDATE_PY} ${CMAKE_SOURCE_DIR}/verify/deck_check.py)
  set_tests_properties(deck.claims_sourced PROPERTIES
    LABELS "deck"
    SKIP_RETURN_CODE 77
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

# --- freshness: the committed viz/data snapshot must equal `at emit` at HEAD ----
# Closes the "synced manually" decay mode (ARCHITECTURE §5): CI verifies exactly
# what GitHub Pages serves. Byte-identical standard; generated_by normalized.
if(AT_VALIDATE_PY)
  add_test(NAME freshness.snapshot_matches_emit
           COMMAND ${AT_VALIDATE_PY} ${CMAKE_SOURCE_DIR}/verify/freshness_check.py
             --at $<TARGET_FILE:at>)
  set_tests_properties(freshness.snapshot_matches_emit PROPERTIES
    LABELS "freshness"
    SKIP_RETURN_CODE 77
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
