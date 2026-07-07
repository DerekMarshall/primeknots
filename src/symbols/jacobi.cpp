#include "symbols/jacobi.h"

#include <utility>

namespace at::symbols {

using at::core::u64;

int jacobi(u64 a, u64 n) {
    // Precondition: n is odd and n >= 1. (For n == 1 the empty product is +1.)
    a %= n;
    int result = 1;
    while (a != 0) {
        // Pull out factors of 2, flipping sign by the second supplementary law:
        // (2/n) = -1 iff n ≡ 3 or 5 (mod 8).
        while ((a & 1) == 0) {
            a >>= 1;
            u64 r = n % 8;
            if (r == 3 || r == 5) result = -result;
        }
        // Reciprocity flip: (a/n)(n/a) = -1 iff a ≡ n ≡ 3 (mod 4).
        std::swap(a, n);
        if (a % 4 == 3 && n % 4 == 3) result = -result;
        a %= n;
    }
    return n == 1 ? result : 0;  // n != 1 here means gcd(a, n) > 1
}

}  // namespace at::symbols
