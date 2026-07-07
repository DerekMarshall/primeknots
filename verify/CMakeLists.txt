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

# Oracle referee: reports SKIP (exit 77) when gp is absent, PASS/FAIL otherwise.
add_test(NAME stage0.oracle_kronecker
         COMMAND verify_stage0 --test-case=oracle_kronecker)
set_tests_properties(stage0.oracle_kronecker PROPERTIES
  LABELS "stage0;oracle"
  SKIP_RETURN_CODE 77)

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
