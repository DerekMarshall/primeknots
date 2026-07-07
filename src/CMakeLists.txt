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
