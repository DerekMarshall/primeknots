# Stage dependency direction (ARCHITECTURE.md §1): core -> symbols -> ...
# Only core/ and symbols/ exist at Stage 0.

add_library(at_core STATIC
  core/modpow.cpp
  core/primes.cpp)
target_include_directories(at_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(at_symbols STATIC
  symbols/legendre.cpp
  symbols/jacobi.cpp
  symbols/tonelli_shanks.cpp)
target_include_directories(at_symbols PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(at_symbols PUBLIC at_core)

add_library(at_linking STATIC
  linking/f2matrix.cpp
  linking/linking_matrix.cpp)
target_include_directories(at_linking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(at_linking PUBLIC at_symbols Threads::Threads)

# Provenance baked into emitted JSON ("generated_by"). Computed at configure
# time; re-run cmake to refresh. Falls back to "unknown" outside a git checkout.
execute_process(
  COMMAND git -C ${CMAKE_SOURCE_DIR} describe --always --dirty --tags
  OUTPUT_VARIABLE AT_GIT_DESCRIBE OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET RESULT_VARIABLE AT_GIT_RC)
if(NOT AT_GIT_RC EQUAL 0)
  set(AT_GIT_DESCRIBE "unknown")
endif()

add_library(at_emit STATIC emit/emit_linking.cpp)
target_include_directories(at_emit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(at_emit PUBLIC at_linking)
target_compile_definitions(at_emit PRIVATE AT_GENERATED_BY="${AT_GIT_DESCRIBE}")
