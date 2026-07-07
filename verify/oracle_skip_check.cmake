# Clean-SKIP contract check (invoked via `cmake -P`).
#
# Runs the Stage 0 oracle suite with `gp` scrubbed from PATH and asserts the
# spec's behavior (CLAUDE.md rule 3, ARCHITECTURE.md §4): a missing oracle must
# SKIP *visibly* — exit code 77 and a printed "[SKIP]" line — never a silent
# pass (exit 0) and never a failure. Passes iff the contract holds.
#
# Inputs (via -D): ORACLE_BIN (the verify_stage0 binary), EMPTY_DIR (a dir with
# no `gp` on it, used as the entire PATH).

file(MAKE_DIRECTORY "${EMPTY_DIR}")
set(ENV{PATH} "${EMPTY_DIR}")   # PATH containing no gp; binary launched by abs path

execute_process(
  COMMAND "${ORACLE_BIN}" "--test-case=oracle_kronecker"
  RESULT_VARIABLE rc
  OUTPUT_VARIABLE out
  ERROR_VARIABLE err)

message(STATUS "oracle suite output with gp absent:\n${out}${err}")

if(NOT rc EQUAL 77)
  message(FATAL_ERROR
    "clean-SKIP contract violated: expected exit 77 (SKIP) with gp absent, "
    "got ${rc}. A missing oracle must SKIP, not pass (0) or fail.")
endif()

if(NOT out MATCHES "\\[SKIP\\]")
  message(FATAL_ERROR
    "clean-SKIP contract violated: no visible [SKIP] line in the output.")
endif()

message(STATUS "clean-SKIP OK: gp absent -> exit 77 and a visible [SKIP] line.")
