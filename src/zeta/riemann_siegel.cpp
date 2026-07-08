#include "zeta/riemann_siegel.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>

namespace at::zeta {
namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr double kTwoPi = 2.0 * kPi;

// Below this height, and inside the band where the Riemann-Siegel remainder
// function Ψ is near its removable singularity (|cos 2πp| small), evaluate Z
// through Euler-Maclaurin instead. See stage5-pinning.md §1.
constexpr double kEMSwitch = 250.0;
constexpr double kSingCosMin = 0.10;

// ---- Riemann-Siegel theta ------------------------------------------------

// Complex log-Γ via Lanczos (g=7, n=9); accurate for Re(z) > 0.
std::complex<double> clgamma(std::complex<double> z) {
    static const double c[9] = {
        0.99999999999980993, 676.5203681218851, -1259.1392167224028,
        771.32342877765313, -176.61502916214059, 12.507343278686905,
        -0.13857109526572012, 9.9843695780195716e-6, 1.5056327351493116e-7};
    z -= 1.0;
    std::complex<double> x = c[0];
    for (int i = 1; i < 9; ++i) x += c[i] / (z + static_cast<double>(i));
    std::complex<double> tt = z + 7.5;
    return 0.5 * std::log(kTwoPi) + (z + 0.5) * std::log(tt) - tt + std::log(x);
}

// ---- Truncated-Taylor jet (AD) for Ψ and its derivatives -----------------
// Coefficients a[k] are the Taylor coefficients about the evaluation point, so
// the k-th derivative is k!·a[k]. Order 10 covers Ψ⁽⁹⁾ (needed by C_3).

constexpr int kOrd = 10;
struct Jet {
    std::array<double, kOrd> a{};
};

Jet jet_var(double p) {
    Jet j;
    j.a[0] = p;
    j.a[1] = 1.0;
    return j;
}
Jet operator*(const Jet& u, const Jet& v) {
    Jet r;
    for (int n = 0; n < kOrd; ++n) {
        double s = 0.0;
        for (int k = 0; k <= n; ++k) s += u.a[k] * v.a[n - k];
        r.a[n] = s;
    }
    return r;
}
Jet scale(const Jet& u, double c) {
    Jet r;
    for (int n = 0; n < kOrd; ++n) r.a[n] = c * u.a[n];
    return r;
}
// cos of a jet (fills its sin partner too).
void jet_cos_sin(const Jet& u, Jet& c, Jet& s) {
    c.a[0] = std::cos(u.a[0]);
    s.a[0] = std::sin(u.a[0]);
    for (int n = 1; n < kOrd; ++n) {
        double cs = 0.0, ss = 0.0;
        for (int k = 1; k <= n; ++k) {
            cs += k * u.a[k] * s.a[n - k];
            ss += k * u.a[k] * c.a[n - k];
        }
        c.a[n] = -cs / n;
        s.a[n] = ss / n;
    }
}
Jet jet_div(const Jet& u, const Jet& v) {
    Jet r;
    for (int n = 0; n < kOrd; ++n) {
        double s = u.a[n];
        for (int k = 1; k <= n; ++k) s -= v.a[k] * r.a[n - k];
        r.a[n] = s / v.a[0];
    }
    return r;
}

// Ψ(p) = cos(2π(p²−p−1/16)) / cos(2πp), as a jet about p (⇒ all derivatives).
Jet psi_jet(double p) {
    Jet jp = jet_var(p);
    Jet p2 = jp * jp;
    Jet arg1 = scale(p2, kTwoPi);                       // 2π p²
    for (int n = 0; n < kOrd; ++n) arg1.a[n] -= kTwoPi * jp.a[n];  // − 2π p
    arg1.a[0] += kTwoPi * (-1.0 / 16.0);                // − 2π/16
    Jet num, num_s;
    jet_cos_sin(arg1, num, num_s);
    Jet arg2 = scale(jp, kTwoPi);                       // 2π p
    Jet den, den_s;
    jet_cos_sin(arg2, den, den_s);
    return jet_div(num, den);
}

// ---- Euler-Maclaurin ζ(1/2+it) -------------------------------------------

Cx em_zeta(double t) {
    const double sigma = 0.5;
    const int M = static_cast<int>(std::ceil(t)) + 8;  // EM needs M ≳ t
    std::complex<double> s(sigma, t);
    std::complex<double> z(0.0, 0.0);
    for (int n = 1; n < M; ++n) {
        // n^{-s} = n^{-σ} (cos(t ln n) − i sin(t ln n))
        double ln = std::log(static_cast<double>(n));
        double mag = std::pow(static_cast<double>(n), -sigma);
        z += std::complex<double>(mag * std::cos(t * ln), -mag * std::sin(t * ln));
    }
    double lnM = std::log(static_cast<double>(M));
    double magM = std::pow(static_cast<double>(M), -sigma);
    std::complex<double> Mminus_s(magM * std::cos(t * lnM), -magM * std::sin(t * lnM));
    z += 0.5 * Mminus_s;                       // M^{-s}/2
    z += Mminus_s * static_cast<double>(M) / (s - 1.0);  // M^{1-s}/(s-1)

    // Bernoulli corrections: Σ B_{2k}/(2k)! · (s)_{2k-1} · M^{-s-2k+1}
    static const double B2k[6] = {1.0 / 6, -1.0 / 30, 1.0 / 42,
                                  -1.0 / 30, 5.0 / 66, -691.0 / 2730};
    static const double fact2k[6] = {2, 24, 720, 40320, 3628800, 479001600};
    std::complex<double> Minv2 = 1.0 / (std::complex<double>(M, 0.0) *
                                        std::complex<double>(M, 0.0));
    std::complex<double> term = Mminus_s / static_cast<double>(M);  // M^{-s-1}
    std::complex<double> poch(1.0, 0.0);
    for (int k = 1; k <= 6; ++k) {
        // poch(s, 2k-1) = s(s+1)...(s+2k-2); extend by two factors each step.
        int base = 2 * k - 2;  // previous poch had (2k-3) factors; add (s+2k-3),(s+2k-2)
        if (k == 1) {
            poch = s;  // (s)_1
        } else {
            poch *= (s + static_cast<double>(base - 1));  // s+2k-3
            poch *= (s + static_cast<double>(base));       // s+2k-2
        }
        z += (B2k[k - 1] / fact2k[k - 1]) * poch * term;
        term *= Minv2;  // M^{-2}: advances M^{-s-2k+1} → M^{-s-2k-1}
    }
    return {z.real(), z.imag()};
}

}  // namespace

// Riemann-Siegel theta, asymptotic (Stirling) series; valid for t ≥ 14.
double theta(double t) {
    const double h = t / kTwoPi;
    double s = (t / 2.0) * std::log(h) - t / 2.0 - kPi / 8.0;
    const double it = 1.0 / t, it2 = it * it;
    // 1/(48t) + 7/(5760 t³) + 31/(80640 t⁵) + 127/(430080 t⁷)
    s += it * (1.0 / 48.0
         + it2 * (7.0 / 5760.0
         + it2 * (31.0 / 80640.0
         + it2 * (127.0 / 430080.0))));
    return s;
}

double theta_lgamma(double t) {
    // Im log Γ(1/4 + it/2) − (t/2) log π. (Principal branch — the twin test
    // compares e^{iθ}, which is branch-independent, not θ itself.)
    std::complex<double> lg = clgamma(std::complex<double>(0.25, t / 2.0));
    return lg.imag() - (t / 2.0) * std::log(kPi);
}

Cx zeta_half_plus_it(double t) { return em_zeta(t); }

double Z_euler_maclaurin(double t) {
    Cx z = em_zeta(t);
    double th = theta(t);
    return std::cos(th) * z.re - std::sin(th) * z.im;  // Re(e^{iθ} ζ)
}

double Z_riemann_siegel(double t) {
    const double th = theta(t);
    const double tau = t / kTwoPi;
    const double sq = std::sqrt(tau);
    const int N = static_cast<int>(std::floor(sq));
    // Main sum: 2 Σ_{n=1}^{N} cos(θ − t ln n)/√n
    double main = 0.0;
    for (int n = 1; n <= N; ++n) {
        double ln = std::log(static_cast<double>(n));
        main += std::cos(th - t * ln) / std::sqrt(static_cast<double>(n));
    }
    main *= 2.0;
    // Remainder: (−1)^{N−1} τ^{−1/4} Σ_{k=0}^{3} C_k(p) s^k,  s = τ^{−1/2}
    const double p = sq - N;
    Jet Ps = psi_jet(p);
    // k-th derivative of Ψ = k!·(Taylor coefficient a[k]).
    static const double kFact[kOrd] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880};
    auto D = [&](int k) { return Ps.a[k] * kFact[k]; };
    const double pi2 = kPi * kPi, pi4 = pi2 * pi2, pi6 = pi4 * pi2;
    double C0 = D(0);
    double C1 = -D(3) / (96.0 * pi2);
    double C2 = D(2) / (64.0 * pi2) + D(6) / (18432.0 * pi4);
    double C3 = -D(1) / (64.0 * pi2) - D(5) / (3840.0 * pi4)
                - D(9) / (5308416.0 * pi6);
    const double s = 1.0 / sq;  // τ^{−1/2}
    double rem = C0 + C1 * s + C2 * s * s + C3 * s * s * s;
    rem *= std::pow(tau, -0.25);
    if (((N - 1) & 1) != 0) rem = -rem;  // (−1)^{N−1}
    return main + rem;
}

bool uses_euler_maclaurin(double t) {
    if (t < kEMSwitch) return true;
    const double sq = std::sqrt(t / kTwoPi);
    const double p = sq - std::floor(sq);
    return std::fabs(std::cos(kTwoPi * p)) < kSingCosMin;
}

double Z(double t) {
    return uses_euler_maclaurin(t) ? Z_euler_maclaurin(t) : Z_riemann_siegel(t);
}

double gram_point(long n) {
    // Seed: solve leading asymptotic (x)(ln x − 1) = n + 7/8 with x = t/2π.
    double rhs = static_cast<double>(n) + 0.875;
    double x = std::max(2.5, rhs);
    for (int i = 0; i < 60; ++i) {
        double lx = std::log(x);
        double f = x * (lx - 1.0) - rhs;
        double fp = lx;  // d/dx [x ln x − x] = ln x
        if (std::fabs(fp) < 1e-15) break;
        double step = f / fp;
        x -= step;
        if (x < 1e-3) x = 1e-3;
        if (std::fabs(step) < 1e-14) break;
    }
    double t = kTwoPi * x;
    // Newton on the full θ: θ'(t) ≈ ½ log(t/2π).
    double target = static_cast<double>(n) * kPi;
    for (int i = 0; i < 80; ++i) {
        double f = theta(t) - target;
        double fp = 0.5 * std::log(t / kTwoPi);
        if (std::fabs(fp) < 1e-15) break;
        double step = f / fp;
        t -= step;
        if (t < 1.0) t = 1.0;
        if (std::fabs(step) < 1e-13) break;
    }
    return t;
}

}  // namespace at::zeta
