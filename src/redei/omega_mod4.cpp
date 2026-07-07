#include "redei/omega_mod4.h"

namespace at::redei {

using at::core::i128;
using at::core::u64;

namespace {
int mod4(i128 v) { return static_cast<int>(((v % 4) + 4) % 4); }
}  // namespace

std::array<bool, 16> square_classes_mod4(u64 p1) {
    int m = static_cast<int>(((p1 - 1) / 4) % 4);
    std::array<bool, 16> sq{};  // all false
    for (int a = 0; a < 4; ++a) {
        for (int b = 0; b < 4; ++b) {
            // (a + bω)² = (a² + b²m) + (2ab + b²)ω,  using ω² = ω + m.
            int A = ((a * a + b * b * m) % 4 + 4) % 4;
            int B = ((2 * a * b + b * b) % 4 + 4) % 4;
            sq[4 * A + B] = true;
        }
    }
    return sq;
}

std::array<bool, 16> square_classes_mod4_via_mul(u64 p1) {
    int m = static_cast<int>(((p1 - 1) / 4) % 4);
    // Generic O/4 multiply: (a1+b1ω)(a2+b2ω) = (a1a2 + b1b2 m) + (a1b2+a2b1+b1b2)ω.
    auto mul = [m](int a1, int b1, int a2, int b2, int& A, int& B) {
        A = ((a1 * a2 + b1 * b2 * m) % 4 + 4) % 4;
        B = ((a1 * b2 + a2 * b1 + b1 * b2) % 4 + 4) % 4;
    };
    std::array<bool, 16> sq{};
    for (int a = 0; a < 4; ++a) {
        for (int b = 0; b < 4; ++b) {
            int A, B;
            mul(a, b, a, b, A, B);
            sq[4 * A + B] = true;
        }
    }
    return sq;
}

bool is_square_mod4(i128 A, i128 B, u64 p1) {
    std::array<bool, 16> sq = square_classes_mod4(p1);
    return sq[4 * mod4(A) + mod4(B)];
}

int two_adic_class(i128 x, i128 y, u64 p1) {
    // β = x + y√p1 = (x−y) + (2y)ω  in O = Z[ω].
    i128 A = x - y;
    i128 B = 2 * y;
    int w = 0;
    // Peel factors of 2 (each is a non-square 2-adically → flips w) to a 2-unit.
    while (A % 2 == 0 && B % 2 == 0) {
        A /= 2;
        B /= 2;
        w ^= 1;
    }
    // 2-unit β': unramified ⇔ β'≡□ mod 4. Unique twist t∈{±1,±2} (S22 Prop 7.3):
    // t∈{±1} (no value change) iff β' or −β' is a square mod 4; else t∈{±2}.
    if (!is_square_mod4(A, B, p1) && !is_square_mod4(-A, -B, p1)) {
        w ^= 1;
    }
    return w;
}

}  // namespace at::redei
