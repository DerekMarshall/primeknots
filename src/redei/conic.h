#pragma once
#include <vector>

#include "core/int_types.h"

// Rational-conic solving for the Rédei symbol (RESEARCH.md §4, step 1). This is
// convention-free mathematics: finding primitive integer points on
// x² − a·y² − b·z² = 0. No normalization choices live here.
namespace at::redei {

struct ConicSolution {
    at::core::i128 x, y, z;
};

// x² − a·y² − b·z² = 0 satisfied, and (x,y,z) not all zero.
bool on_conic(const ConicSolution& s, at::core::u64 a, at::core::u64 b);

// Fast path: up to `count` essentially-different PRIMITIVE solutions
// (gcd(x,y,z)=1, first nonzero coordinate > 0), found via a base solution
// (local-square-root-constrained search, in the spirit of Cremona–Rusin [CR03])
// plus the conic's line parametrization. Empty vector if insoluble.
// Precondition: a, b are each 1 or an odd prime, coprime (Stage 2: distinct
// primes ≡ 1 mod 4). Solubility ⇔ (a/b) = (b/a) = 1.
std::vector<ConicSolution> solve_conic(at::core::u64 a, at::core::u64 b,
                                       int count = 1);

// Referee twin (FROZEN): exhaustive Holzer-bounded brute force. Searches all
// |y| ≤ √b, |z| ≤ √a and keeps primitive solutions with a perfect-square x.
// Obviously correct; O(√(ab)). Guaranteed non-empty iff the conic is soluble
// (Holzer's theorem bounds a solution inside this box).
std::vector<ConicSolution> solve_conic_holzer(at::core::u64 a, at::core::u64 b);

}  // namespace at::redei
