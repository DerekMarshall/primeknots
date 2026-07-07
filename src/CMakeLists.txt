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

# Stage 2 — conic solving (Phase 1) + Rédei symbol (Phase 2).
add_library(at_redei STATIC
  redei/conic.cpp
  redei/omega_mod4.cpp
  redei/redei_symbol.cpp)
target_include_directories(at_redei PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(at_redei PUBLIC at_symbols)

# Stage 3 — quadratic forms, narrow class group, Rédei matrix (consumes linking).
add_library(at_classgroup STATIC
  classgroup/qform.cpp
  classgroup/class_group.cpp
  classgroup/redei_matrix.cpp)
target_include_directories(at_classgroup PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(at_classgroup PUBLIC at_symbols at_linking)

# Stage 4 — arithmetic Chern-Simons / Dijkgraaf-Witten (Hirano mod-2 route).
# lhs.cpp (brute character sum) and rhs.cpp (closed form) authored in isolation.
add_library(at_cs STATIC cs/lhs.cpp cs/rhs.cpp)
target_include_directories(at_cs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(at_cs PUBLIC at_symbols)

# Stage 5 — zeta zeros (Riemann-Siegel + Euler-Maclaurin twin), Turing-certified
# zero finder, explicit-formula ψ reconstruction, Ruelle-zeta model twin.
add_library(at_zeta STATIC
  zeta/riemann_siegel.cpp
  zeta/zero_finder.cpp
  zeta/explicit_formula.cpp
  zeta/ruelle.cpp)
target_include_directories(at_zeta PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(at_zeta PUBLIC at_core)

add_library(at_emit STATIC
  emit/emit_linking.cpp
  emit/emit_borromean.cpp
  emit/emit_classgroups.cpp
  emit/emit_cs.cpp
  emit/emit_zeta.cpp)
target_include_directories(at_emit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(at_emit PUBLIC at_linking at_redei at_classgroup at_cs at_zeta)
