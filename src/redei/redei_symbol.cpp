#include "redei/redei_symbol.h"

#include <cassert>
#include <vector>

#include "core/primes.h"
#include "redei/omega_mod4.h"
#include "symbols/legendre.h"
#include "symbols/tonelli_shanks.h"

namespace at::redei {

using namespace at::core;

namespace {
// (2/p) for odd p: +1 iff p ≡ ±1 (mod 8).
int two_over(u64 p) { return (p % 8 == 1 || p % 8 == 7) ? 1 : -1; }
}  // namespace

int redei_value_from_solution(u64 p1, u64 p3, u64 s, i128 x, i128 y) {
    i128 P = static_cast<i128>(p3);
    i128 arg = ((x % P) + (y % P) * static_cast<i128>(s)) % P;
    arg = (arg + P) % P;
    if (arg == 0) return 0;  // degenerate branch (R1): (x+y s) ≡ 0 (mod p3)
    int leg = at::symbols::legendre_euler(static_cast<u64>(arg), p3);  // ±1
    int w = two_adic_class(x, y, p1);
    return (w == 1) ? leg * two_over(p3) : leg;
}

int redei_symbol(u64 p1, u64 p2, u64 p3) {
    // EC1 — distinct primes ≡ 1 (mod 4).
    assert(at::core::is_prime(p1) && at::core::is_prime(p2) && at::core::is_prime(p3));
    assert(p1 % 4 == 1 && p2 % 4 == 1 && p3 % 4 == 1);
    assert(p1 != p2 && p1 != p3 && p2 != p3);
    // EC2 — pairwise unlinked.
    assert(at::symbols::legendre_euler(p1, p2) == 1);
    assert(at::symbols::legendre_euler(p1, p3) == 1);
    assert(at::symbols::legendre_euler(p2, p3) == 1);

    // p3 splits in Q(√p1) since (p1/p3)=1; s = √p1 mod p3.
    auto s_opt = at::symbols::sqrt_mod(p1 % p3, p3);
    assert(s_opt.has_value());
    u64 s = *s_opt;

    // R1: use a primitive solution with p3 ∤ z (both Legendre branches nonzero).
    // Request progressively more solutions until a non-degenerate one appears.
    for (int need = 8; need <= 1024; need *= 2) {
        std::vector<ConicSolution> sols = solve_conic(p1, p2, need);
        for (const ConicSolution& c : sols) {
            if (c.z % static_cast<i128>(p3) != 0) {
                int v = redei_value_from_solution(p1, p3, s, c.x, c.y);
                assert(v != 0);
                return v;
            }
        }
        if (static_cast<int>(sols.size()) < need) break;  // no more distinct sols
    }
    assert(false && "no primitive conic solution with p3 ∤ z found");
    return 0;
}

}  // namespace at::redei
