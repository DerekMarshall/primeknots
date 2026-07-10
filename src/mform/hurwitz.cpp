#include "mform/hurwitz.h"

#include <algorithm>
#include <atomic>
#include <thread>

namespace at::mform {

namespace {
i64 gcd_(i64 a, i64 b) {
    a = a < 0 ? -a : a; b = b < 0 ? -b : b;
    while (b) { i64 t = a % b; a = b; b = t; }
    return a;
}

// Weight of a reduced positive-definite form (a,b,c) in the Hurwitz count:
// a multiple of x²+y² = (f,0,f) counts 1/2; of x²+xy+y² = (f,f,f) counts 1/3.
Frac weight(i64 a, i64 b, i64 c) {
    if (b == 0 && a == c) return {1, 2};
    if (a == b && b == c) return {1, 3};
    return {1, 1};
}
}  // namespace

Frac frac(i64 num, i64 den) {
    if (den < 0) { num = -num; den = -den; }
    i64 g = gcd_(num, den);
    if (g == 0) g = 1;
    return {num / g, den / g};
}

Frac add(const Frac& a, const Frac& b) { return frac(a.num * b.den + b.num * a.den, a.den * b.den); }

// (A) Enumerate every reduced positive-definite form of discriminant −n and sum its
// Hurwitz weight. Reduced: −a < b ≤ a ≤ c, with b ≥ 0 when a == c.
Frac hurwitz_by_forms(i64 n) {
    if (n == 0) return {-1, 12};
    if (n % 4 == 1 || n % 4 == 2) return {0, 1};   // −n not a discriminant
    Frac H{0, 1};
    for (i64 a = 1; 3 * a * a <= n; ++a) {
        for (i64 b = -a + 1; b <= a; ++b) {
            const i64 num = b * b + n;
            if (num % (4 * a) != 0) continue;
            const i64 c = num / (4 * a);
            if (c < a) continue;
            if (c == a && b < 0) continue;           // reduced-form tie-break
            H = add(H, weight(a, b, c));
        }
    }
    return H;
}

// (B) H(n) = Σ_{f²|n} hp(n/f²), hp = weighted count of PRIMITIVE reduced forms.
namespace {
Frac primitive_class_number(i64 m) {
    if (m % 4 == 1 || m % 4 == 2) return {0, 1};
    Frac h{0, 1};
    for (i64 a = 1; 3 * a * a <= m; ++a) {
        for (i64 b = -a + 1; b <= a; ++b) {
            const i64 num = b * b + m;
            if (num % (4 * a) != 0) continue;
            const i64 c = num / (4 * a);
            if (c < a) continue;
            if (c == a && b < 0) continue;
            if (gcd_(gcd_(a, b), c) != 1) continue;  // primitive only
            h = add(h, weight(a, b, c));
        }
    }
    return h;
}
}  // namespace

Frac hurwitz_by_decomposition(i64 n) {
    if (n == 0) return {-1, 12};
    if (n % 4 == 1 || n % 4 == 2) return {0, 1};
    Frac H{0, 1};
    for (i64 f = 1; f * f <= n; ++f)
        if (n % (f * f) == 0) H = add(H, primitive_class_number(n / (f * f)));
    return H;
}

Frac hurwitz(i64 n) { return hurwitz_by_forms(n); }

// --- fast path: the 12·H sieve (twinned against hurwitz_by_forms) --------------

namespace {
// 12·weight of a reduced form: generic 12 (weight 1), x²+y² multiple 6 (weight
// 1/2, b=0 & a=c), x²+xy+y² multiple 4 (weight 1/3, a=b=c). Matches weight().
int weight12(i64 a, i64 b, i64 c) {
    if (b == 0 && a == c) return 6;
    if (a == b && b == c) return 4;
    return 12;
}
}  // namespace

namespace {
// Fill 12·H over the OUTPUT range [lo,hi] by enumerating every reduced form (a,b,c)
// whose disc n = 4ac − b² lands in [lo,hi]. Because the range is by n, different
// ranges touch disjoint array cells — so this parallelizes with no locks and no
// merge, and the result is independent of thread order (every form of a given disc
// is enumerated by the one range that owns that disc). Deterministic (freshness).
void fill_range(std::vector<int>& t12, i64 lo, i64 hi) {
    for (i64 a = 1; 3 * a * a <= hi; ++a) {
        for (i64 b = -a + 1; b <= a; ++b) {
            const i64 b2 = b * b;
            const i64 fa = 4 * a;
            i64 c0 = (lo + b2 + fa - 1) / fa;    // ceil((lo+b²)/4a)
            if (c0 < a) c0 = a;                  // reduced: c ≥ a
            const i64 c1 = (hi + b2) / fa;        // floor((hi+b²)/4a)
            for (i64 c = c0; c <= c1; ++c) {
                if (c == a && b < 0) continue;    // reduced-form tie-break
                t12[4 * a * c - b2] += weight12(a, b, c);
            }
        }
    }
}
}  // namespace

HurwitzTable::HurwitzTable(i64 bound) : bound_(bound), t12_(bound + 1, 0) {
    t12_[0] = -1;   // 12·H(0) = −1
    // Partition [1, bound] into chunks pulled by a pool of threads; each thread owns
    // a disjoint output range (fill_range), so no synchronization on the array.
    unsigned hw = std::thread::hardware_concurrency();
    const unsigned nthreads = hw ? (hw > 16 ? 16 : hw) : 1;
    const i64 nchunks = static_cast<i64>(nthreads) * 4;
    const i64 chunk = bound / nchunks + 1;
    std::atomic<i64> next{0};
    auto worker = [&] {
        for (;;) {
            const i64 ci = next.fetch_add(1);
            i64 lo = ci * chunk;
            if (lo > bound) break;
            if (lo < 1) lo = 1;                   // cell 0 is the special H(0)
            const i64 hi = std::min(bound, ci * chunk + chunk - 1);
            fill_range(t12_, lo, hi);
        }
    };
    std::vector<std::thread> pool;
    for (unsigned t = 1; t < nthreads; ++t) pool.emplace_back(worker);
    worker();
    for (auto& th : pool) th.join();
}

Frac HurwitzTable::operator()(i64 n) const { return frac(t12_[n], 12); }

}  // namespace at::mform
