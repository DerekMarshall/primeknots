#pragma once
#include <span>

#include "core/int_types.h"

namespace at::cs {

// Right-hand side (closed form) of the Stage 4 mod-2 arithmetic
// Dijkgraaf–Witten invariant.
//
// Input `primes`: a strictly ascending list of r distinct primes, each
// ≡ 1 (mod 4).
//
// Let lk2(p_i, p_j) ∈ {0,1} be the mod-2 linking number, derived from the
// Legendre symbol (p_i / p_j): 0 when the symbol is +1, 1 when it is −1.
// Let R_i = ( sum_{j != i} lk2(p_i, p_j) ) mod 2 be the i-th row sum.
//
// The invariant is Z = 2^{r−1} when every row sum R_i is even, and Z = 0
// otherwise. (Assumes r ≤ 20 so 2^{r−1} fits in a long.)
long rhs_partition(std::span<const at::core::u64> primes);

}  // namespace at::cs
