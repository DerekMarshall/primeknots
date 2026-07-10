#include "mform/hurwitz.h"

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

}  // namespace at::mform
