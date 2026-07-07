# Stage dependency direction (ARCHITECTURE.md §1): core -> symbols -> ...
# Only core/ and symbols/ exist at Stage 0.

add_library(at_core STATIC
  core/modpow.cpp
  core/primes.cpp)
target_include_directories(at_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
