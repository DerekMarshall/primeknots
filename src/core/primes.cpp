#include "core/primes.h"

#include "core/modpow.h"

namespace at::core {

namespace {
// The 12-witness set that is deterministic for all n < 3.317e24.
constexpr u64 kWitnesses[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
}  // namespace

bool is_prime(u64 n) {
    if (n < 2) return false;
    for (u64 p : kWitnesses) {
        if (n % p == 0) return n == p;  // small prime, or divisible by one
    }
    // n is odd and coprime to every witness, so n > 37.
    u64 d = n - 1;
    int s = 0;
    while ((d & 1) == 0) { d >>= 1; ++s; }

    for (u64 a : kWitnesses) {
        u64 x = modpow(a, d, n);
        if (x == 1 || x == n - 1) continue;
        bool composite = true;
        for (int r = 1; r < s; ++r) {
            x = modmul(x, x, n);
            if (x == n - 1) { composite = false; break; }
        }
        if (composite) return false;
    }
    return true;
}

}  // namespace at::core
