#pragma once
#include "core/int_types.h"

namespace at::symbols {

// Legendre symbol (a / p) for an odd prime p, returning +1, -1, or 0
// (0 exactly when p | a). Two genuinely independent implementations
// (RESEARCH.md §2, "Dual implementation (mandatory)"):

// (a) Euler's criterion: (a/p) ≡ a^((p-1)/2) (mod p), via modpow.
int legendre_euler(at::core::u64 a, at::core::u64 p);

// (b) The reciprocity recursion, specialized to a prime modulus via the
//     Jacobi symbol. Uses no modular exponentiation — a different algorithm,
//     not a refactor of (a).
int legendre_recip(at::core::u64 a, at::core::u64 p);

}  // namespace at::symbols
