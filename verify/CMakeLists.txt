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
foreach(ocase oracle_kronecker oracle_modpow)
  add_test(NAME stage0.${ocase}
           COMMAND verify_stage0 --test-case=${ocase})
  set_tests_properties(stage0.${ocase} PROPERTIES
    LABELS "stage0;oracle"
    SKIP_RETURN_CODE 77)
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
  SKIP_RETURN_CODE 77)

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
  SKIP_RETURN_CODE 77)
