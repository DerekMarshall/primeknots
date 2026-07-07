#pragma once
#include <vector>

#include "core/int_types.h"

// Small header-only helpers shared by the Stage 0 suites: an Eratosthenes sieve
// (also the frozen referee twin for is_prime) and a deterministic sampler
// (fixed seed, no rand() — reproducibility per ARCHITECTURE.md §3).
namespace at::verify::stage0 {

using at::core::u64;

// Boolean "is composite" table for [0, N]. comp[k] == false iff k is prime
// (with comp[0] = comp[1] = true).
inline std::vector<bool> composite_sieve(u64 N) {
    std::vector<bool> comp(N + 1, false);
    comp[0] = true;
    if (N >= 1) comp[1] = true;
    for (u64 i = 2; i * i <= N; ++i) {
        if (!comp[i]) {
            for (u64 j = i * i; j <= N; j += i) comp[j] = true;
        }
    }
    return comp;
}

// Primes in [2, N], ascending.
inline std::vector<u64> primes_up_to(u64 N) {
    std::vector<bool> comp = composite_sieve(N);
    std::vector<u64> ps;
    for (u64 n = 2; n <= N; ++n) {
        if (!comp[n]) ps.push_back(n);
    }
    return ps;
}

// Deterministic 64-bit LCG (Knuth MMIX constants). Seed carried by reference.
inline u64 next_sample(u64& state) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    return state >> 1;  // drop the low bit (weakest in an LCG)
}

}  // namespace at::verify::stage0
