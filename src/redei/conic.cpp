#include "redei/conic.h"

#include <cmath>

#include "symbols/legendre.h"
#include "symbols/tonelli_shanks.h"

namespace at::redei {

using namespace at::core;

namespace {

u64 isqrt_u64(u64 n) {
    if (n == 0) return 0;
    u64 x = static_cast<u64>(std::sqrt(static_cast<double>(n)));
    while (x > 0 && x * x > n) --x;
    while ((x + 1) * (x + 1) <= n) ++x;
    return x;
}

i128 iabs(i128 v) { return v < 0 ? -v : v; }

i128 gcd_i128(i128 a, i128 b) {
    a = iabs(a);
    b = iabs(b);
    while (b) {
        i128 t = a % b;
        a = b;
        b = t;
    }
    return a;
}

// Reduce to a primitive vector with the first nonzero coordinate positive, so
// two solutions that are scalar multiples (or negatives) compare equal.
ConicSolution normalize(i128 x, i128 y, i128 z) {
    i128 g = gcd_i128(gcd_i128(x, y), z);
    if (g == 0) return {0, 0, 0};
    x /= g;
    y /= g;
    z /= g;
    i128 lead = x != 0 ? x : (y != 0 ? y : z);
    if (lead < 0) {
        x = -x;
        y = -y;
        z = -z;
    }
    return {x, y, z};
}

bool same(const ConicSolution& p, const ConicSolution& q) {
    return p.x == q.x && p.y == q.y && p.z == q.z;
}

}  // namespace

bool on_conic(const ConicSolution& s, u64 a, u64 b) {
    if (s.x == 0 && s.y == 0 && s.z == 0) return false;
    return static_cast<i128>(s.x) * s.x - static_cast<i128>(a) * s.y * s.y -
               static_cast<i128>(b) * s.z * s.z ==
           0;
}

namespace {

// One primitive base solution of x² = a y² + b z² via the constrained search:
// x ≡ ±s·z (mod a) with s² ≡ b (mod a). Holzer bounds |z| ≤ √a, |x| ≤ √(ab).
bool fast_base(u64 a, u64 b, ConicSolution& out) {
    if (a == 1) { out = normalize(1, 1, 0); return true; }
    if (b == 1) { out = normalize(1, 0, 1); return true; }
    // Solubility for coprime squarefree a,b (primes here): (a/b)=(b/a)=1.
    if (at::symbols::legendre_euler(b % a, a) != 1) return false;
    if (at::symbols::legendre_euler(a % b, b) != 1) return false;

    auto s_opt = at::symbols::sqrt_mod(b % a, a);
    if (!s_opt) return false;
    u64 s = *s_opt;

    u64 ab = static_cast<u64>(static_cast<u128>(a) * b);
    u64 Zmax = isqrt_u64(a);
    u64 Xmax = isqrt_u64(ab);
    for (u64 z = 1; z <= Zmax; ++z) {
        for (u64 sgn : {s, a - s}) {
            u64 x0 = static_cast<u64>((static_cast<u128>(sgn) * (z % a)) % a);
            for (u64 x = x0; x <= Xmax; x += a) {
                if (x == 0) continue;
                u128 xx = static_cast<u128>(x) * x;
                u128 bzz = static_cast<u128>(b) * z * z;
                if (xx < bzz) continue;
                u128 val = xx - bzz;  // = a·y²
                if (val % a != 0) continue;
                u64 y2 = static_cast<u64>(val / a);
                u64 y = isqrt_u64(y2);
                if (static_cast<u128>(y) * y == y2) {
                    out = normalize(static_cast<i128>(x), static_cast<i128>(y),
                                    static_cast<i128>(z));
                    return true;
                }
            }
        }
    }
    return false;  // should not happen when the QR conditions hold
}

}  // namespace

std::vector<ConicSolution> solve_conic(u64 a, u64 b, int count) {
    std::vector<ConicSolution> out;
    ConicSolution base;
    if (!fast_base(a, b, base)) return out;

    auto push_unique = [&](const ConicSolution& s) {
        if (!on_conic(s, a, b)) return;
        for (const auto& e : out)
            if (same(e, s)) return;
        out.push_back(s);
    };
    push_unique(base);

    // Additional solutions: second intersection of the line P + λV with the
    // conic. With β = B(P,V) = x_P x_V − a y_P y_V − b z_P z_V and q = Q(V), the
    // point W = q·P − 2β·V lies on the conic (Q(W)=0). Varying V over small
    // integer directions yields distinct primitive points.
    const ConicSolution P = base;
    for (i128 vx = -3; vx <= 3 && static_cast<int>(out.size()) < count; ++vx) {
        for (i128 vy = -3; vy <= 3 && static_cast<int>(out.size()) < count; ++vy) {
            for (i128 vz = -3; vz <= 3 && static_cast<int>(out.size()) < count; ++vz) {
                if (vx == 0 && vy == 0 && vz == 0) continue;
                i128 beta = P.x * vx - static_cast<i128>(a) * P.y * vy -
                            static_cast<i128>(b) * P.z * vz;
                i128 q = vx * vx - static_cast<i128>(a) * vy * vy -
                         static_cast<i128>(b) * vz * vz;
                if (q == 0) continue;
                i128 wx = q * P.x - 2 * beta * vx;
                i128 wy = q * P.y - 2 * beta * vy;
                i128 wz = q * P.z - 2 * beta * vz;
                if (wx == 0 && wy == 0 && wz == 0) continue;
                push_unique(normalize(wx, wy, wz));
            }
        }
    }
    if (static_cast<int>(out.size()) > count) out.resize(count);
    return out;
}

std::vector<ConicSolution> solve_conic_holzer(u64 a, u64 b) {
    std::vector<ConicSolution> out;
    auto push_unique = [&](const ConicSolution& s) {
        if (!on_conic(s, a, b)) return;
        for (const auto& e : out)
            if (same(e, s)) return;
        out.push_back(s);
    };
    u64 Ymax = isqrt_u64(b);
    u64 Zmax = isqrt_u64(a);
    for (u64 y = 0; y <= Ymax; ++y) {
        for (u64 z = 0; z <= Zmax; ++z) {
            if (y == 0 && z == 0) continue;
            u128 val = static_cast<u128>(a) * y * y + static_cast<u128>(b) * z * z;
            u64 x = isqrt_u64(static_cast<u64>(val));
            if (static_cast<u128>(x) * x == val) {
                push_unique(normalize(static_cast<i128>(x), static_cast<i128>(y),
                                      static_cast<i128>(z)));
                push_unique(normalize(static_cast<i128>(x), static_cast<i128>(y),
                                      -static_cast<i128>(z)));
            }
        }
    }
    return out;
}

}  // namespace at::redei
