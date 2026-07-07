#pragma once
#include "core/int_types.h"
#include "harness.h"
#include "linking/linking_matrix.h"

// Shared Stage 1 fixture. The linking matrix is expensive to build, so it is
// built once per process (lazy static) and shared by the read-only suites.
namespace at::verify::stage1 {

using at::core::u64;

// Prime bound for the default run and the --extended run. The expected prime
// counts are π(x; 4, 1) obtained from PARI/GP (NOT our own sieve), so the count
// assertion is refereed by an outside source (per the Stage 0 precedent).
inline u64 bound() { return at::verify::g_extended ? 1'000'000ULL : 50'000ULL; }
inline u64 expected_primes() {
    return at::verify::g_extended ? 39'175ULL   // π(1e6; 4,1)
                                  : 2'549ULL;    // π(5e4; 4,1)
}

inline const at::linking::LinkingMatrix& shared() {
    static const at::linking::LinkingMatrix m =
        at::linking::LinkingMatrix::build(bound());
    return m;
}

// Deterministic 64-bit LCG (fixed seed, no rand()) for reproducible sampling.
inline u64 next_sample(u64& state) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    return state >> 1;
}

}  // namespace at::verify::stage1
