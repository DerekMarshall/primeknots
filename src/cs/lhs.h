#pragma once
#include <span>
#include "core/int_types.h"

namespace at::cs {

// LEFT-HAND SIDE of the Stage 4 mod-2 arithmetic Dijkgraaf-Witten invariant.
//
// Brute-force character phase sum. Input `primes` is a strictly ascending
// list of r distinct primes, each == 1 (mod 4).
//
// Let lk2(p_i, p_j) in {0,1} be the mod-2 linking number (0 if the Legendre
// symbol (p_i/p_j) is +1, else 1). The character group is Hom(T+, Z/2) where
// T+ = { x in (Z/2)^r : sum_i x_i = 0 }, of dimension r-1. For a character rho
// the Chern-Simons phase is CS(rho) = sum_{i<j} rho(e+_{ij}) * lk2(p_i, p_j)
// (mod 2), and the invariant is Z = sum over rho of (-1)^{CS(rho)}.
long lhs_partition(std::span<const at::core::u64> primes);

}  // namespace at::cs
