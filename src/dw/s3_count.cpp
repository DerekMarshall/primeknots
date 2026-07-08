#include "dw/s3_count.h"

#include <numeric>
#include <stdexcept>

namespace at::dw {

Rational rational(long long num, long long den) {
    if (den == 0) throw std::runtime_error("dw: zero denominator");
    if (den < 0) { num = -num; den = -den; }
    long long g = std::gcd(num < 0 ? -num : num, den);
    if (g == 0) g = 1;
    return {num / g, den / g};
}

Rational add(Rational a, Rational b) {
    // a.num/a.den + b.num/b.den — small values, no overflow in our range.
    return rational(a.num * b.den + b.num * a.den, a.den * b.den);
}

bool equal(Rational a, Rational b) {
    Rational x = rational(a.num, a.den), y = rational(b.num, b.den);
    return x.num == y.num && x.den == y.den;
}

namespace {
long long pow_ll(long long base, int e) {
    long long r = 1;
    for (int i = 0; i < e; ++i) r *= base;
    return r;
}
}  // namespace

long long trivial_term() { return 1; }
long long c2_term(int t) { return 3 * (pow_ll(2, t) - 1); }
long long c3_term(int k) { return pow_ll(3, k) - 1; }
long long s3_term(long long c) { return 6 * c; }

long long hom_count(int t, int k, long long c) {
    return trivial_term() + c2_term(t) + c3_term(k) + s3_term(c);
}

Rational z_dw(int t, int k, long long c) {
    return rational(hom_count(t, k, c), 6);
}

Rational mass_formula_rhs(int t, int k, long long c) {
    // 1/6 + (2^t − 1)/2 + (3^k − 1)/6 + c  — assembled independently of z_dw.
    Rational r = rational(1, 6);
    r = add(r, rational(pow_ll(2, t) - 1, 2));
    r = add(r, rational(pow_ll(3, k) - 1, 6));
    r = add(r, rational(c, 1));
    return r;
}

int cyclic_k(const std::vector<at::core::u64>& S) {
    int k = 0;
    for (at::core::u64 p : S)
        if (p % 12 == 1) ++k;
    return k;
}

}  // namespace at::dw
