# The `at` CLI entry point (docs/notes/open-questions.md R2). Dispatches the
# per-stage JSON emitters; Stage 1 is the first wired. AT_GIT_FALLBACK is the
# configure-time git describe, used only when runtime git is unavailable.
add_executable(at at.cpp)
# at_emit for the stage emitters; at_murm + at_oracle for the `ne-cache` generator
# (the height family + the committed N/ε cache, filled from PARI — the one place the
# CLI touches the oracle; src/ stays oracle-free, rule 3).
target_link_libraries(at PRIVATE at_emit at_murm at_oracle)
target_compile_definitions(at PRIVATE AT_GIT_FALLBACK="${AT_GIT_DESCRIBE}")
