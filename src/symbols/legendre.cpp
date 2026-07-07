#include "symbols/legendre.h"

#include "core/modpow.h"
#include "symbols/jacobi.h"

namespace at::symbols {

using at::core::u64;

int legendre_euler(u64 a, u64 p) {
    // Precondition: p is an odd prime.
    a %= p;
    if (a == 0) return 0;
    u64 r = at::core::modpow(a, (p - 1) / 2, p);
    // For a prime modulus the criterion yields exactly 1 or p-1 (== -1).
    if (r == 1) return 1;
    if (r == p - 1) return -1;
    return 0;  // unreachable for a genuine odd prime p
}

int legendre_recip(u64 a, u64 p) {
    // For an odd prime p, the Jacobi symbol coincides with the Legendre symbol,
    // including the p | a case (jacobi returns 0 there). The Jacobi routine
    // reaches this answer through the binary reciprocity recursion, wholly
    // independently of the Euler-criterion path above.
    return jacobi(a % p, p);
}

}  // namespace at::symbols
