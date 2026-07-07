# The `at` CLI entry point (docs/notes/open-questions.md R2). Dispatches the
# per-stage JSON emitters; Stage 1 is the first wired.
add_executable(at at.cpp)
target_link_libraries(at PRIVATE at_emit)
