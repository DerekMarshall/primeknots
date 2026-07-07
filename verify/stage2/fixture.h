#pragma once
#include <vector>

#include "core/int_types.h"
#include "core/primes.h"
#include "harness.h"

// Stage 2 test helpers. Phase 1 uses only convention-free conic machinery.
namespace at::verify::stage2 {

using at::core::u64;

// Primes ≡ 1 (mod 4) up to bound, via the (twin-verified) is_prime.
inline std::vector<u64> primes_1mod4_up_to(u64 bound) {
    std::vector<u64> ps;
    for (u64 n = 5; n <= bound; n += 4)
        if (at::core::is_prime(n)) ps.push_back(n);
    return ps;
}

// Bound for the conic twin sweep (brute force is O(√(ab)), so keep it modest).
// Prime count is pinned to π(x; 4,1) from gp (Stage 0/1 precedent).
inline u64 twin_bound() { return at::verify::g_extended ? 5'000ULL : 1'000ULL; }
inline u64 twin_expected_primes() {
    return at::verify::g_extended ? 329ULL   // π(5000; 4,1) from gp
                                  : 80ULL;   // π(1000; 4,1) from gp
}

inline u64 next_sample(u64& state) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    return state >> 1;
}

}  // namespace at::verify::stage2
