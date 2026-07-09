#include "ell/curve.h"

#include <stdexcept>
#include <string>

namespace at::ell {

using at::core::u64;

// Silverman III.1, b- and c-invariants (exact, 128-bit).
i128 b2(const Curve& E) { return (i128)E.a1 * E.a1 + 4 * (i128)E.a2; }
i128 b4(const Curve& E) { return 2 * (i128)E.a4 + (i128)E.a1 * E.a3; }
i128 b6(const Curve& E) { return (i128)E.a3 * E.a3 + 4 * (i128)E.a6; }

static i128 b8(const Curve& E) {
    return (i128)E.a1 * E.a1 * E.a6 + 4 * (i128)E.a2 * E.a6
         - (i128)E.a1 * E.a3 * E.a4 + (i128)E.a2 * E.a3 * E.a3
         - (i128)E.a4 * E.a4;
}

i128 c4(const Curve& E) {
    i128 B2 = b2(E);
    return B2 * B2 - 24 * b4(E);
}

i128 c6(const Curve& E) {
    i128 B2 = b2(E);
    return -B2 * B2 * B2 + 36 * B2 * b4(E) - 216 * b6(E);
}

i128 discriminant(const Curve& E) {
    i128 B2 = b2(E), B4 = b4(E), B6 = b6(E), B8 = b8(E);
    return -B2 * B2 * B8 - 8 * B4 * B4 * B4 - 27 * B6 * B6 + 9 * B2 * B4 * B6;
}

// (x,y) ↦ (x + r, y + s·x + t), i.e. the u = 1 admissible change (Silverman
// III.1). Always integral; the same curve, a different model.
Curve translate(const Curve& E, i64 r, i64 s, i64 t) {
    Curve F;
    F.a1 = E.a1 + 2 * s;
    F.a2 = E.a2 - s * E.a1 + 3 * r - s * s;
    F.a3 = E.a3 + r * E.a1 + 2 * t;
    F.a4 = E.a4 - s * E.a3 + 2 * r * E.a2 - (t + r * s) * E.a1 + 3 * r * r - 2 * s * t;
    F.a6 = E.a6 + r * E.a4 + r * r * E.a2 + r * r * r - t * E.a3 - t * t - r * t * E.a1;
    return F;
}

// a_i ↦ u^i·a_i (the (x,y) ↦ (x/u², y/u³) scaling): c4 ↦ u⁴c4, c6 ↦ u⁶c6, so the
// short model scales by u and a_p is unchanged at good p ∤ u (m0-pinning §2).
Curve scale(const Curve& E, i64 u) {
    i64 u2 = u * u, u3 = u2 * u, u4 = u2 * u2, u6 = u3 * u3;
    return Curve{E.a1 * u, E.a2 * u2, E.a3 * u3, E.a4 * u4, E.a6 * u6};
}

static i128 iabs128(i128 v) { return v < 0 ? -v : v; }
static i128 gcd128(i128 a, i128 b) {
    a = iabs128(a);
    b = iabs128(b);
    while (b) { i128 t = a % b; a = b; b = t; }
    return a;
}

void assert_minimal(const Curve& E, u64 conductor_N) {
    const i128 d = iabs128(discriminant(E));
    if (d == 0)
        throw std::runtime_error("assert_minimal: singular model (Δ = 0)");
    // Strip every prime shared with N; a residue > 1 is a prime dividing Δ(model)
    // but not N — the model is non-minimal there (silent-wrong-a_p hazard, §5/C1).
    i128 r = d;
    const i128 n = static_cast<i128>(conductor_N);
    for (i128 g; (g = gcd128(r, n)) > 1;) r /= g;
    if (r != 1)
        throw std::runtime_error(
            "assert_minimal: non-minimal model — a prime divides Δ(model) but not the "
            "conductor N; point-counting would count a singular reduction and return a "
            "wrong a_p (use the minimal model)");
}

}  // namespace at::ell
