#pragma once
#include <vector>

#include "core/int_types.h"
#include "ell/curve.h"

// Shared M0 fixture: a curated curve sample and a small sieve. Models quoted from
// allcurves.00000-09999 @ 25cec5ec… (ranks 0–3: 37a1 r1, 389a1 r2, 5077a1 r3 —
// a_p is rank-independent, the spread just exercises the referee uniformly).
namespace at::verify::m0 {

using at::core::u64;

inline std::vector<at::ell::Curve> sample_curves() {
    return {
        {0, -1, 1, -10, -20},  // 11a1
        {1, 0, 1, 4, -6},      // 14a1
        {1, 1, 1, -10, -10},   // 15a1
        {1, -1, 1, -1, -14},   // 17a1
        {0, 1, 1, -9, -15},    // 19a1
        {0, 0, 1, -1, 0},      // 37a1  (rank 1)
        {0, 1, 1, -2, 0},      // 389a1 (rank 2)
        {0, 0, 1, -7, 6},      // 5077a1 (rank 3)
    };
}

// Primes in [max(lo,2), hi], ascending (small Eratosthenes sieve).
inline std::vector<u64> primes_in(u64 lo, u64 hi) {
    std::vector<bool> comp(hi + 1, false);
    for (u64 i = 2; i * i <= hi; ++i)
        if (!comp[i])
            for (u64 j = i * i; j <= hi; j += i) comp[j] = true;
    std::vector<u64> ps;
    for (u64 n = (lo < 2 ? 2 : lo); n <= hi; ++n)
        if (!comp[n]) ps.push_back(n);
    return ps;
}

}  // namespace at::verify::m0
