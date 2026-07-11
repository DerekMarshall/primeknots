#include "core/bessel.h"

#include <cmath>   // only std::sqrt (IEEE correctly-rounded, portable) — no sin/cos/etc.

#include "core/constants.h"   // kPi, kTwoOverPi, kHalfPiHi/Lo — GENERATED, never typed

namespace at::core {

namespace {

// --- in-house sin/cos (IEEE ops only; no libm) --------------------------------
// Argument reduction mod π/2 with the generated two-part (Cody–Waite) split so the
// reduced value keeps precision for |x| up to ~1e5 (the density's J₁-argument range).
// Cores are Taylor series on |r| ≤ π/4 (~1e-17). This need not match libm bit-for-bit
// — only be accurate (twin vs PARI besselj) and deterministic (IEEE ops).
double sin_core(double r) {   // |r| ≤ π/4
    const double z = r * r;
    return r * (1.0 + z * (-1.0 / 6 + z * (1.0 / 120 + z * (-1.0 / 5040
             + z * (1.0 / 362880 + z * (-1.0 / 39916800))))));
}
double cos_core(double r) {   // |r| ≤ π/4
    const double z = r * r;
    return 1.0 + z * (-0.5 + z * (1.0 / 24 + z * (-1.0 / 720 + z * (1.0 / 40320
             + z * (-1.0 / 3628800 + z * (1.0 / 479001600))))));
}
// reduce x to r ∈ [−π/4, π/4] and quadrant n = round(x/(π/2)) mod 4 (n ≥ 0).
double reduce(double x, int& n) {
    double k = x * kTwoOverPi;
    // round-half-to-nearest via truncation of k+0.5 (k ≥ 0 here; x is used |·|)
    long long kk = static_cast<long long>(k + 0.5);
    double kf = static_cast<double>(kk);
    double r = (x - kf * kHalfPiHi) - kf * kHalfPiLo;   // Cody–Waite
    n = static_cast<int>(kk & 3);
    return r;
}
double sin_ieee(double x) {   // x ≥ 0 (callers pass reduced positive angles)
    int n; const double r = reduce(x, n);
    switch (n) {
        case 0: return sin_core(r);
        case 1: return cos_core(r);
        case 2: return -sin_core(r);
        default: return -cos_core(r);
    }
}
double cos_ieee(double x) {
    int n; const double r = reduce(x, n);
    switch (n) {
        case 0: return cos_core(r);
        case 1: return -sin_core(r);
        case 2: return -cos_core(r);
        default: return sin_core(r);
    }
}

// --- J₁ ------------------------------------------------------------------------
constexpr double kSwitch = 15.0;   // series ≤ 15 (~1e-11), asymptotic above

double j1_series(double ax) {
    // J₁(x) = Σ_{k≥0} (−1)^k / (k!(k+1)!) · (x/2)^{2k+1}
    const double half = 0.5 * ax;
    const double x2 = half * half;
    double term = half, sum = half;   // k = 0
    for (int k = 1; k < 80; ++k) {
        term *= -x2 / (static_cast<double>(k) * (k + 1));
        sum += term;
        if (term < 1e-20 && term > -1e-20) break;
    }
    return sum;
}
double j1_asymptotic(double ax) {
    // J₁(x) ~ √(2/(πx)) [ P·cos(ω) − Q·sin(ω) ],  ω = x − 3π/4 (DLMF 10.17.3, ν=1).
    // P = Σ_k (−1)^k a_{2k} x^{−2k}, Q = Σ_k (−1)^k a_{2k+1} x^{−(2k+1)}, with the
    // coefficients GENERATED in-code from the DLMF 10.17.4 recurrence (4ν²=4):
    //   a_j = a_{j−1}·(4 − (2j−1)²)/(8j),  a_0 = 1
    // — never typed as decimals (CLAUDE.md #7). The series is asymptotic (divergent);
    // truncate at the optimal (smallest) term, giving ~1e-12 for x ≥ 15.
    const double xi = 1.0 / ax;
    double P = 1.0, Q = 0.0;   // k = 0 terms: P += a_0 = 1; Q has no k=0 (odd index ≥ 1)
    double a = 1.0, powj = 1.0, prev_mag = 1e308;
    for (int j = 1; j <= 30; ++j) {
        a *= (4.0 - (2.0 * j - 1.0) * (2.0 * j - 1.0)) / (8.0 * j);
        powj *= xi;                                    // x^{−j}
        const double term = a * powj;
        const double mag = term < 0 ? -term : term;
        if (mag >= prev_mag) break;                    // asymptotic optimal truncation
        prev_mag = mag;
        const double s = ((j / 2) & 1) ? -1.0 : 1.0;   // (−1)^{⌊j/2⌋}
        if (j % 2 == 0) P += s * term; else Q += s * term;
    }
    const double omega = ax - 0.75 * kPi;
    const double amp = std::sqrt(2.0 / (kPi * ax));
    return amp * (P * cos_ieee(omega) - Q * sin_ieee(omega));
}
}  // namespace

double bessel_j1(double x) {
    if (x == 0.0) return 0.0;
    const double sign = x < 0 ? -1.0 : 1.0;   // J₁ is odd
    const double ax = x < 0 ? -x : x;
    return sign * (ax <= kSwitch ? j1_series(ax) : j1_asymptotic(ax));
}

}  // namespace at::core
