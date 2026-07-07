#include "symbols/tonelli_shanks.h"

#include "core/modpow.h"

namespace at::symbols {

using at::core::modmul;
using at::core::modpow;
using at::core::u64;

std::optional<u64> sqrt_mod(u64 a, u64 p) {
    // Precondition: p is an odd prime.
    a %= p;
    if (a == 0) return u64{0};

    // Non-residue detection (Euler criterion). This is the guard that keeps the
    // algorithm from looping forever searching for a root that does not exist.
    if (modpow(a, (p - 1) / 2, p) != 1) return std::nullopt;

    // Fast path for p ≡ 3 (mod 4): r = a^((p+1)/4).
    if (p % 4 == 3) return modpow(a, (p + 1) / 4, p);

    // General Tonelli–Shanks. Write p - 1 = q * 2^s with q odd.
    u64 q = p - 1;
    int s = 0;
    while ((q & 1) == 0) { q >>= 1; ++s; }

    // Find a quadratic non-residue z (bounded search; ~half of residues qualify).
    u64 z = 2;
    while (modpow(z, (p - 1) / 2, p) != p - 1) ++z;

    u64 c = modpow(z, q, p);
    u64 x = modpow(a, (q + 1) / 2, p);
    u64 t = modpow(a, q, p);
    int m = s;

    while (t != 1) {
        // Least i in (0, m) with t^(2^i) == 1.
        int i = 0;
        u64 tt = t;
        while (tt != 1) { tt = modmul(tt, tt, p); ++i; }

        u64 b = c;
        for (int j = 0; j < m - i - 1; ++j) b = modmul(b, b, p);
        u64 b2 = modmul(b, b, p);
        x = modmul(x, b, p);
        t = modmul(t, b2, p);
        c = b2;
        m = i;
    }
    return x;
}

}  // namespace at::symbols
