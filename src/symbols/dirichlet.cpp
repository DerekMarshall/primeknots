#include "symbols/dirichlet.h"

#include <cmath>

#include "core/modpow.h"

namespace at::symbols {

using at::core::modpow;
using at::core::u128;

namespace {
const double kTwoPi = 6.28318530717958647692528676655900577;

// e(t) = exp(2πi t).
std::complex<double> e(double t) {
    return std::complex<double>(std::cos(kTwoPi * t), std::sin(kTwoPi * t));
}

std::vector<u64> prime_factors(u64 n) {
    std::vector<u64> f;
    for (u64 d = 2; d * d <= n; ++d)
        if (n % d == 0) { f.push_back(d); while (n % d == 0) n /= d; }
    if (n > 1) f.push_back(n);
    return f;
}
}  // namespace

u64 mult_order(u64 g, u64 p) {
    u64 order = 1, cur = g % p;
    while (cur != 1 && order <= p) {
        cur = static_cast<u64>(static_cast<u128>(cur) * g % p);
        ++order;
    }
    return order;
}

u64 primitive_root(u64 p) {
    if (p == 2) return 1;
    const u64 phi = p - 1;
    const std::vector<u64> qs = prime_factors(phi);
    for (u64 g = 2; g < p; ++g) {
        bool ok = true;
        for (u64 q : qs)
            if (modpow(g, phi / q, p) == 1) { ok = false; break; }
        if (ok) return g;
    }
    return 0;  // unreachable for a prime p
}

std::vector<u64> index_table(u64 p, u64 g) {
    std::vector<u64> ind(p, 0);
    u64 cur = 1;
    for (u64 k = 0; k < p - 1; ++k) {
        ind[cur] = k;
        cur = static_cast<u64>(static_cast<u128>(cur) * g % p);
    }
    return ind;
}

std::complex<double> chi_value(u64 k, u64 order, const std::vector<u64>& ind, u64 a) {
    const u64 p = ind.size();
    a %= p;
    if (a == 0) return {0.0, 0.0};
    const u64 exp = static_cast<u64>(static_cast<u128>(k) * ind[a] % order);
    return e(static_cast<double>(exp) / static_cast<double>(order));
}

std::complex<double> gauss_sum(u64 k, u64 p, u64 order, const std::vector<u64>& ind) {
    std::complex<double> s(0.0, 0.0);
    for (u64 a = 1; a < p; ++a)
        s += chi_value(k, order, ind, a) * e(static_cast<double>(a) / static_cast<double>(p));
    return s;
}

}  // namespace at::symbols
