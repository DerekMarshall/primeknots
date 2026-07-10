# Stage dependency direction (ARCHITECTURE.md §1): core -> symbols -> ...
# Only core/ and symbols/ exist at Stage 0.

add_library(at_core STATIC
  core/modpow.cpp
  core/primes.cpp)
target_include_directories(at_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(at_symbols STATIC
  symbols/legendre.cpp
  symbols/jacobi.cpp
  symbols/tonelli_shanks.cpp
  symbols/dirichlet.cpp)
target_include_directories(at_symbols PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(at_symbols PUBLIC at_core)

# Stage M0 (murmurations) — elliptic curves, a_p referees (charsum p>3 +
# enumeration for p∈{2,3}), the fast path, and the versioned a_p cache.
# ell -> {symbols, core} (ARCHITECTURE-M §1). No family/averaging concepts here.
add_library(at_ell STATIC
  ell/curve.cpp
  ell/ap.cpp
  ell/ap_fast.cpp
  ell/ap_cache.cpp
  ell/ecdata.cpp)
target_include_directories(at_ell PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(at_ell PUBLIC at_symbols Threads::Threads)

# Stage M3 (murmurations) — modular-form trace-formula ingredients. mform -> core
# (Hurwitz class numbers); the Eichler–Selberg traces (Phase 2) will add classgroup.
add_library(at_mform STATIC
  mform/hurwitz.cpp
  mform/trace.cpp
  mform/dimension.cpp)
target_include_directories(at_mform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Threads: the Hurwitz sieve (fast path) fills disjoint output ranges in parallel.
target_link_libraries(at_mform PUBLIC at_core Threads::Threads)

# Stage M1 (murmurations) — family definition + per-prime averaging.
# murm -> ell (ARCHITECTURE-M §1). No ordering/density concepts beyond HLOP's
# conductor family.
add_library(at_murm STATIC
  murm/murmuration.cpp
  murm/dirichlet_murmuration.cpp
  murm/zub_density.cpp
  murm/zub_empirical.cpp)
target_include_directories(at_murm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# murm -> {ell, mform}: M1/M2 averaging uses ell (a_p); M3's empirical convergence
# assembles the newform trace formula (mform) over the family (ARCHITECTURE-M §1).
target_link_libraries(at_murm PUBLIC at_ell at_mform)

# a_p cache generator hash (ARCHITECTURE-M §4): sha256 of the fast-path generator
# source. A change to ell/ap_fast.cpp changes this, so the loader refuses caches
# written by an older generator. Computed at configure time (re-configure to
# refresh — the CMakeLists change on any src edit already triggers that here).
file(SHA256 ${CMAKE_CURRENT_SOURCE_DIR}/ell/ap_fast.cpp AT_AP_GENERATOR_HASH)
set_source_files_properties(ell/ap_cache.cpp PROPERTIES
  COMPILE_DEFINITIONS AP_GENERATOR_HASH="${AT_AP_GENERATOR_HASH}")

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

# Stage 6 — untwisted S₃ Dijkgraaf-Witten invariant of Spec ℤ[1/S]: assemble
# |Hom| and Z_DW (exact rational) from t, k (our machinery) + refereed c.
add_library(at_dw STATIC dw/s3_count.cpp)
target_include_directories(at_dw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(at_dw PUBLIC at_core)

add_library(at_emit STATIC
  emit/emit_linking.cpp
  emit/emit_borromean.cpp
  emit/emit_classgroups.cpp
  emit/emit_cs.cpp
  emit/emit_zeta.cpp
  emit/emit_dw.cpp
  emit/emit_murmuration.cpp
  emit/emit_dirichlet.cpp)
target_include_directories(at_emit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(at_emit PUBLIC at_linking at_redei at_classgroup at_cs at_zeta at_dw at_murm)
