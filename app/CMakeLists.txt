# The `at` CLI entry point (docs/notes/open-questions.md R2). Dispatches the
# per-stage JSON emitters; Stage 1 is the first wired. AT_GIT_FALLBACK is the
# configure-time git describe, used only when runtime git is unavailable.
add_executable(at at.cpp)
target_link_libraries(at PRIVATE at_emit)
target_compile_definitions(at PRIVATE AT_GIT_FALLBACK="${AT_GIT_DESCRIBE}")
