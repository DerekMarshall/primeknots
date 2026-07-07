cmake_minimum_required(VERSION 3.20)
project(arithmetic_topology LANGUAGES CXX)

# Release by default (single-config generators). RESEARCH sweeps want -O2/-O3.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)   # strict C++20; unsigned __int128 is still available

# GMP facade toggle. No stage before Stage 3 needs arbitrary precision, so the
# facade (src/core/bigint.h) is only a seam today. See docs/notes R3-adjacent.
option(WITH_GMP "Build the GMP-backed Bigint facade (unused before Stage 3)" OFF)
if(WITH_GMP)
  message(WARNING
    "WITH_GMP=ON has no effect yet: no stage before Stage 3 uses "
    "src/core/bigint.h. Building it will fail by design.")
endif()

add_compile_options(-Wall -Wextra)

find_package(Threads REQUIRED)

# Configure-time git describe, used only as the *fallback* for emitted-JSON
# provenance. The `at` CLI resolves generated_by at emit time (runtime git) and
# falls back to this when run outside a checkout. Refreshed on re-configure.
execute_process(
  COMMAND git -C ${CMAKE_SOURCE_DIR} describe --always --dirty --tags
  OUTPUT_VARIABLE AT_GIT_DESCRIBE OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET RESULT_VARIABLE AT_GIT_RC)
if(NOT AT_GIT_RC EQUAL 0)
  set(AT_GIT_DESCRIBE "unknown")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

enable_testing()

add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(oracle)
add_subdirectory(verify)
