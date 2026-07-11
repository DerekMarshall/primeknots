#include "murm/height_family.h"

#include <cmath>
#include <vector>

namespace at::murm {

namespace {
i64 iabs(i64 x) { return x < 0 ? -x : x; }

// Box half-widths: largest |A| with 4|A|³ ≤ X, largest |B| with 27B² ≤ X. Computed
// via cbrt/sqrt then integer-corrected (float boundary safety).
i64 a_max(i64 X) {
    i64 a = static_cast<i64>(std::cbrt(X / 4.0));
    while (4 * (a + 1) * (a + 1) * (a + 1) <= X) ++a;
    while (a > 0 && 4 * a * a * a > X) --a;
    return a;
}
i64 b_max(i64 X) {
    i64 b = static_cast<i64>(std::sqrt(X / 27.0));
    while (27 * (b + 1) * (b + 1) <= X) ++b;
    while (b > 0 && 27 * b * b > X) --b;
    return b;
}

// Reduced ⟺ no prime p with p⁴|A AND p⁶|B. (A=0 ⇒ p⁴|A for every p, so reduced iff
// no p⁶|B; B=0 symmetric. The origin is not reduced — and is singular anyway.)
bool is_reduced(i64 A, i64 B) {
    const i64 a = iabs(A), b = iabs(B);
    if (a == 0 && b == 0) return false;
    if (a == 0) {                                  // every p⁴ | 0
        for (i64 p = 2; p * p * p * p * p * p <= b; ++p)
            if (b % (p * p * p * p * p * p) == 0) return false;
        return true;
    }
    for (i64 p = 2; p * p * p * p <= a; ++p) {
        const i64 p4 = p * p * p * p;
        if (a % p4 == 0 && b % (p4 * p * p) == 0) return false;   // b==0 ⇒ 0%p⁶==0 ⇒ not reduced
    }
    return true;
}
}  // namespace

i64 naive_height(i64 A, i64 B) {
    const i64 a = iabs(A);
    const i64 h1 = 4 * a * a * a;
    const i64 h2 = 27 * B * B;
    return h1 > h2 ? h1 : h2;
}

std::vector<HeightCurve> height_family(i64 X) {
    std::vector<HeightCurve> out;
    const i64 Am = a_max(X), Bm = b_max(X);
    for (i64 A = -Am; A <= Am; ++A)
        for (i64 B = -Bm; B <= Bm; ++B) {
            if (4 * A * A * A + 27 * B * B == 0) continue;   // singular
            if (naive_height(A, B) > X) continue;            // box corner beyond H
            if (!is_reduced(A, B)) continue;
            out.push_back({A, B});
        }
    return out;
}

i64 height_family_count_sieve(i64 X) {
    const i64 Am = a_max(X), Bm = b_max(X);
    const i64 W = 2 * Bm + 1;                        // B index width
    // marked[(A+Am)*W + (B+Bm)] = non-reduced
    std::vector<char> marked(static_cast<std::size_t>((2 * Am + 1) * W), 0);
    auto idx = [&](i64 A, i64 B) { return static_cast<std::size_t>((A + Am) * W + (B + Bm)); };
    // Mark every (A,B) with p⁴|A and p⁶|B non-reduced. A prime p can contribute a
    // nonzero A only if p⁴≤Am, a nonzero B only if p⁶≤Bm; beyond both, only the origin
    // qualifies (already marked by p=2). A=0/B=0 are multiples of any p⁴/p⁶, so they
    // are swept naturally.
    for (i64 p = 2; p * p * p * p <= Am || p * p * p * p * p * p <= Bm; ++p) {
        bool prime = p > 1;                          // primality (tiny range)
        for (i64 d = 2; d * d <= p; ++d) if (p % d == 0) { prime = false; break; }
        if (!prime) continue;
        const i64 p4 = p * p * p * p, p6 = p4 * p * p;
        // A ranges over multiples of p⁴ in [−Am,Am]; B over multiples of p⁶ in [−Bm,Bm].
        for (i64 A = -(Am / p4) * p4; A <= Am; A += p4)
            for (i64 B = -(Bm / p6) * p6; B <= Bm; B += p6)
                marked[idx(A, B)] = 1;
    }
    i64 count = 0;
    for (i64 A = -Am; A <= Am; ++A)
        for (i64 B = -Bm; B <= Bm; ++B) {
            if (4 * A * A * A + 27 * B * B == 0) continue;   // singular
            if (naive_height(A, B) > X) continue;
            if (marked[idx(A, B)]) continue;                 // non-reduced
            ++count;
        }
    return count;
}

}  // namespace at::murm
