#include "ell/ap.h"

#include "symbols/legendre.h"

namespace at::ell {

using at::core::i64;
using at::core::u128;

int ap_charsum(const Curve& E, u64 p) {
    // All arithmetic mod p: reduce the coefficients first, then build c4, c6 and
    // (A, B) mod p, so no intermediate overflows for any curve (m0-pinning §1).
    auto rp = [p](i64 v) -> u64 {                       // reduce a signed coeff to [0,p)
        i64 m = v % static_cast<i64>(p);
        return static_cast<u64>(m < 0 ? m + static_cast<i64>(p) : m);
    };
    auto mul = [p](u64 a, u64 b) -> u64 { return static_cast<u64>(static_cast<u128>(a) * b % p); };
    auto add = [p](u64 a, u64 b) -> u64 { return (a + b) % p; };          // a,b ∈ [0,p)
    auto sub = [p](u64 a, u64 b) -> u64 { return (a + p - b) % p; };      // a,b ∈ [0,p)

    const u64 a1 = rp(E.a1), a2 = rp(E.a2), a3 = rp(E.a3), a4 = rp(E.a4), a6 = rp(E.a6);

    const u64 B2 = add(mul(a1, a1), mul(4 % p, a2));
    const u64 B4 = add(mul(2 % p, a4), mul(a1, a3));
    const u64 B6 = add(mul(a3, a3), mul(4 % p, a6));

    const u64 C4 = sub(mul(B2, B2), mul(24 % p, B4));
    const u64 B2cube = mul(mul(B2, B2), B2);
    const u64 C6 = sub(add(sub(0, B2cube), mul(36 % p, mul(B2, B4))), mul(216 % p, B6));

    const u64 A = sub(0, mul(27 % p, C4));   // short model y² = x³ + A x + B
    const u64 B = sub(0, mul(54 % p, C6));

    long long sum = 0;
    for (u64 x = 0; x < p; ++x) {
        const u64 fx = add(add(mul(mul(x, x), x), mul(A, x)), B);
        sum += at::symbols::legendre_euler(fx, p);     // 0 when p | f(x)
    }
    return static_cast<int>(-sum);
}

int ap_enumerate(const Curve& E, u64 p) {
    // Point count on the long model y²+a1xy+a3y = x³+a2x²+a4x+a6 mod p, O(p²).
    // Precondition: p ∤ Δ(model) (good reduction — m0-pinning §5). Any good prime;
    // used for p ∈ {2,3} where the short-model referee does not apply.
    auto rp = [p](i64 v) -> u64 {
        i64 m = v % static_cast<i64>(p);
        return static_cast<u64>(m < 0 ? m + static_cast<i64>(p) : m);
    };
    auto mul = [p](u64 a, u64 b) -> u64 { return static_cast<u64>(static_cast<u128>(a) * b % p); };
    auto add = [p](u64 a, u64 b) -> u64 { return (a + b) % p; };

    const u64 a1 = rp(E.a1), a2 = rp(E.a2), a3 = rp(E.a3), a4 = rp(E.a4), a6 = rp(E.a6);

    u64 count = 1;   // the point at infinity, counted EXPLICITLY (m0-pinning §5)
    for (u64 x = 0; x < p; ++x) {
        const u64 x2 = mul(x, x);
        const u64 rhs = add(add(add(mul(x2, x), mul(a2, x2)), mul(a4, x)), a6);
        for (u64 y = 0; y < p; ++y) {
            const u64 lhs = add(add(mul(y, y), mul(a1, mul(x, y))), mul(a3, y));
            if (lhs == rhs) ++count;
        }
    }
    return static_cast<int>(static_cast<i64>(p) + 1 - static_cast<i64>(count));
}

int ap_from_atkin_lehner(int w_sign, int ord_q_N) {
    if (ord_q_N == 1) return -w_sign;   // multiplicative q ∥ N
    return 0;                           // additive q² | N
}

}  // namespace at::ell
