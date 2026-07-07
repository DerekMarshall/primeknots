#pragma once
#include "core/int_types.h"

namespace at::symbols {

// Jacobi symbol (a / n) for odd n >= 1, returning +1, -1, or 0.
// Computed by the binary reciprocity recursion — no modular exponentiation,
// which is what makes it algorithmically independent of the Euler-criterion
// Legendre twin. Value is 0 exactly when gcd(a, n) > 1.
int jacobi(at::core::u64 a, at::core::u64 n);

}  // namespace at::symbols
