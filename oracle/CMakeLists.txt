# PARI/GP oracle adapter. Refereed-by, not depended-on: it is linked into the
# verify binaries, never into src/ libraries (CLAUDE.md rule 3).
add_library(at_oracle STATIC pari.cpp)
# Root include so consumers write "oracle/pari.h".
target_include_directories(at_oracle PUBLIC ${CMAKE_SOURCE_DIR})
