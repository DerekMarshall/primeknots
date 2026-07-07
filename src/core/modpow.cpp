#include "core/modpow.h"

namespace at::core {

// --- Naive referee twins (frozen) -----------------------------------------

u64 modmul(u64 a, u64 b, u64 m) {
    return static_cast<u64>((static_cast<u128>(a % m) * (b % m)) % m);
}

u64 modpow_naive(u64 base, u64 exp, u64 m) {
    if (m == 1) return 0;
    u64 result = 1;
    u64 b = base % m;
    while (exp) {
        if (exp & 1) result = modmul(result, b, m);
        b = modmul(b, b, m);
        exp >>= 1;
    }
    return result;
}

// --- Fast path: 64-bit Montgomery arithmetic ------------------------------
// Reference: standard REDC construction (Montgomery 1985; Cohen [Coh93] §11.1).
// Only valid for odd modulus; the public modpow routes even moduli to the
// direct twin above.

namespace {
struct Montgomery {
    u64 n;    // odd modulus
    u64 npi;  // -n^{-1} mod 2^64
    u64 one;  // R mod n  = 2^64 mod n  (== Montgomery form of 1)
    u64 r2;   // R^2 mod n            (for converting into Montgomery form)

    explicit Montgomery(u64 mod) : n(mod) {
        // n^{-1} mod 2^64 by Newton iteration. Seed inv=1 is correct mod 2
        // (n is odd); each step doubles the number of correct low bits, so
        // six steps reach 2^64.
        u64 inv = 1;
        for (int i = 0; i < 6; ++i) inv *= 2u - n * inv;
        npi = static_cast<u64>(0) - inv;               // -n^{-1} mod 2^64
        one = static_cast<u64>((static_cast<u128>(1) << 64) % n);  // 2^64 mod n
        r2  = static_cast<u64>((static_cast<u128>(one) * one) % n);  // R^2 mod n
    }

    // REDC: given T < n*R, return T * R^{-1} mod n.
    u64 redc(u128 T) const {
        u64 m = static_cast<u64>(T) * npi;                    // low 64 bits
        u128 t = (T + static_cast<u128>(m) * n) >> 64;
        return t >= n ? static_cast<u64>(t - n) : static_cast<u64>(t);
    }

    u64 mul(u64 a, u64 b) const { return redc(static_cast<u128>(a) * b); }
    u64 to_mont(u64 a) const { return redc(static_cast<u128>(a) * r2); }
    u64 from_mont(u64 a) const { return redc(static_cast<u128>(a)); }
};
}  // namespace

u64 modpow(u64 base, u64 exp, u64 m) {
    if (m == 1) return 0;
    if ((m & 1) == 0) return modpow_naive(base, exp, m);  // Montgomery needs odd m

    Montgomery mont(m);
    u64 result = mont.one;               // 1 in Montgomery form
    u64 b = mont.to_mont(base % m);
    while (exp) {
        if (exp & 1) result = mont.mul(result, b);
        b = mont.mul(b, b);
        exp >>= 1;
    }
    return mont.from_mont(result);
}

}  // namespace at::core
