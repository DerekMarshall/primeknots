#pragma once
#include <algorithm>
#include <vector>

#include "core/int_types.h"
#include "core/primes.h"
#include "harness.h"

// Stage 3 test helpers. Phase 1 uses the convention-free forms layer only.
namespace at::verify::stage3 {

using at::core::i128;
using at::core::u64;

inline u64 pool_bound() { return at::verify::g_extended ? 200ULL : 100ULL; }
inline i128 disc_bound() { return at::verify::g_extended ? 2'000'000 : 200'000; }
inline i128 expected_disc_count() {
    return at::verify::g_extended ? 1394 : 208;  // pinned (computed independently)
}

// Our restricted family: squarefree products of 1..3 distinct primes ≡ 1 (mod 4)
// (t = 1,2,3) with D ≤ disc_bound. Deterministic, ascending, deduplicated.
inline std::vector<i128> family_discriminants() {
    std::vector<u64> P;
    for (u64 n = 5; n <= pool_bound(); n += 4)
        if (at::core::is_prime(n)) P.push_back(n);
    std::vector<i128> D;
    const i128 B = disc_bound();
    for (std::size_t i = 0; i < P.size(); ++i) {
        if ((i128)P[i] <= B) D.push_back(P[i]);
        for (std::size_t j = i + 1; j < P.size(); ++j) {
            i128 d2 = (i128)P[i] * P[j];
            if (d2 <= B) D.push_back(d2);
            for (std::size_t k = j + 1; k < P.size(); ++k) {
                i128 d3 = (i128)P[i] * P[j] * P[k];
                if (d3 <= B) D.push_back(d3);
            }
        }
    }
    std::sort(D.begin(), D.end());
    D.erase(std::unique(D.begin(), D.end()), D.end());
    return D;
}

}  // namespace at::verify::stage3
