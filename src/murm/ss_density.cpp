#include "murm/ss_density.h"

#include <cmath>   // only std::sqrt (IEEE, portable) — NOT std::pow (libm, not portable)
#include <cstddef>
#include <vector>

#include "core/bessel.h"
#include "core/constants.h"

namespace at::murm {

using at::core::kPi;

namespace {
long long gcd_(long long a, long long b) { while (b) { long long t = a % b; a = b; b = t; } return a; }

int v_p(long long m, long long p) { int v = 0; while (m % p == 0) { m /= p; ++v; } return v; }

bool squarefree(long long n) {
    for (long long p = 2; p * p <= n; ++p) if (n % (p * p) == 0) return false;
    return true;
}
long long euler_phi(long long n) {
    long long r = n;
    for (long long p = 2; p * p <= n; ++p)
        if (n % p == 0) { while (n % p == 0) n /= p; r -= r / p; }
    if (n > 1) r -= r / n;
    return r;
}
int mobius(long long n) {
    if (n == 1) return 1;
    int mu = 1;
    for (long long p = 2; p * p <= n; ++p)
        if (n % p == 0) { n /= p; if (n % p == 0) return 0; mu = -mu; }
    if (n > 1) mu = -mu;
    return mu;
}
std::vector<long long> prime_factors(long long n) {
    std::vector<long long> ps;
    for (long long p = 2; p * p <= n; ++p)
        if (n % p == 0) { ps.push_back(p); while (n % p == 0) n /= p; }
    if (n > 1) ps.push_back(n);
    return ps;
}
// base^e for integer e, via IEEE multiply/divide only (deterministic; no libm pow).
double dpow(double base, int e) {
    double r = 1.0;
    const int a = e < 0 ? -e : e;
    for (int i = 0; i < a; ++i) r *= base;
    return e < 0 ? 1.0 / r : r;
}
}  // namespace

double cheb_U(int nu, double x) {
    if (nu < 0) return 0.0;
    double um1 = 1.0;        // U_0
    if (nu == 0) return um1;
    double u = 2.0 * x;      // U_1
    for (int k = 2; k <= nu; ++k) { const double t = 2.0 * x * u - um1; um1 = u; u = t; }
    return u;
}

long long level1_hecke_trace(int k, long long /*p*/) {
    // Exact 0 where the space is zero-dimensional: odd weight, or even weight < 12.
    // Weight ≥12 eigenvalue sums are TRUNCATED (returns 0) — the terms needing them are
    // sparse and high-m (m divisible by p⁵); their bounded contribution is folded into
    // the density-evaluation tolerance (m4-pinning §P3/R1). Faithful upgrade: a level-1
    // Eichler–Selberg trace reusing M3's Hurwitz class numbers.
    if (k < 12 || (k % 2) != 0) return 0;
    return 0;   // TRUNCATED (documented)
}

double ss_ell_hat(long long p, int nu) {
    // ℓ̂_{p,ν} (Def 6 / Lemma 4), ν even ≥ 0. Integer powers via dpow (no libm).
    const double pd = static_cast<double>(p);
    if (p == 3) {
        const double d = 1.0 - dpow(3.0, -10);
        if (nu == 0) return (2.0 / 3 + 4.0 * dpow(3.0, -11)) / d;
        if (nu == 2) return -(3.0 - dpow(3.0, -7) + 16.0 * dpow(3.0, -11)) / (2.0 * d);
        const double s3 = std::sqrt(3.0);
        const double inner = dpow(3.0, nu / 2) *
              (cheb_U(nu, 3.0 / (2 * s3)) + 2.0 * cheb_U(nu, 0.0)
               + dpow(3.0, -9) * (cheb_U(nu, 2.0 / (2 * s3)) + cheb_U(nu, 1.0 / (2 * s3))))
              - dpow(3.0, -8);
        return (2.0 / 9) / d * inner;
    }
    if (p == 2) {
        const double d = 1.0 - dpow(2.0, -10);
        if (nu == 0) return dpow(2.0, -9) / d;
        if (nu == 2) return -(4.0 - dpow(2.0, -6) + 3.0 * dpow(2.0, -10)) / d;
        const double S = static_cast<double>(level1_hecke_trace(nu + 2, 2));
        return -(1.0 / (dpow(2.0, 10) - 1)) * (3.0 + S);
    }
    const double d = 1.0 - dpow(pd, -10);   // p > 3
    if (nu == 0) return (1.0 - 1.0 / pd) / d;
    if (nu == 2) return -(pd - 1.0 / pd + 1.0 / (pd * pd) - dpow(pd, -8)) / (d * (pd - 1));
    const double S = static_cast<double>(level1_hecke_trace(nu + 2, p));
    return -((1.0 / pd - 1.0 / (pd * pd)) / d) * (pd + 1 + S);
}

double ss_ell(long long p, int nu) {
    // ℓ_{p,ν} (Def 5 / Lemma 3), ν ≥ 0. Eigenvalue sum vanishes for weight ν+2 < 12.
    const double pd = static_cast<double>(p);
    if (p == 3) {
        const double s3 = std::sqrt(3.0);
        const double cheb = cheb_U(nu, 3.0 / (2 * s3)) + 4.0 * cheb_U(nu, 0.0)
                          + cheb_U(nu, -3.0 / (2 * s3));
        const double d = 1.0 - dpow(3.0, -10);
        const double S = static_cast<double>(level1_hecke_trace(nu + 2, 3));
        return dpow(3.0, nu / 2 - 2) * cheb - ((dpow(3.0, -10) - dpow(3.0, -11)) / d) * S;
    }
    if (p == 2) {
        const double S = static_cast<double>(level1_hecke_trace(nu + 2, 2));
        return -(dpow(2.0, -10) / (1.0 - dpow(2.0, -10))) * S;
    }
    const double S = static_cast<double>(level1_hecke_trace(nu + 2, p));   // p > 3
    return -((1.0 / pd - 1.0 / (pd * pd)) / (1.0 - dpow(pd, -10))) * S;
}

double ss_density(double u, long long m_bound, long long q_bound) {
    const double su = std::sqrt(u);
    double sum = 0.0;
    for (long long q = 1; q <= q_bound; ++q) {
        if (!squarefree(q)) continue;
        const std::vector<long long> qp = prime_factors(q);
        for (long long m = 1; m <= m_bound; ++m) {
            const long long g = gcd_(m, q);
            const int mu = mobius(g);
            if (mu == 0) continue;
            double prod = 1.0;
            bool zero = false;
            for (long long p : qp) {                       // ∏_{p|q} ℓ_{p,v_p(m)}
                const double e = ss_ell(p, v_p(m, p));
                if (e == 0.0) { zero = true; break; }
                prod *= e;
            }
            if (zero) continue;
            for (long long p : prime_factors(m)) {         // ∏_{p|m,p∤q} ℓ̂_{p,2v_p(m)}
                if (q % p == 0) continue;
                prod *= ss_ell_hat(p, 2 * v_p(m, p));
            }
            const double coef = static_cast<double>(mu) /
                (static_cast<double>(q) * static_cast<double>(m)
                 * static_cast<double>(euler_phi(q / g)));
            sum += coef * prod *
                   at::core::bessel_j1(4.0 * kPi * su * static_cast<double>(m) / static_cast<double>(q));
        }
    }
    return 2.0 * kPi * su * sum;
}

SSShape ss_shape(long long B, double du) {
    // Sample the curve once, then extract globally (two-pass: the zero crossing must
    // be the one AFTER the GLOBAL hump, not the early low-u wiggle).
    std::vector<double> us, ds;
    for (double u = du; u <= 1.0 + 1e-9; u += du) { us.push_back(u); ds.push_back(ss_density(u, B, B)); }
    SSShape s{0, -1e18, -1, 0, 1e18};
    for (std::size_t i = 0; i < us.size(); ++i) {
        if (ds[i] > s.hump_v) { s.hump_v = ds[i]; s.hump_u = us[i]; }
        if (ds[i] < s.trough_v) { s.trough_v = ds[i]; s.trough_u = us[i]; }
    }
    for (std::size_t i = 1; i < us.size(); ++i)   // first + → − crossing past the hump
        if (us[i] > s.hump_u && ds[i - 1] > 0 && ds[i] <= 0) {
            s.zero_u = us[i - 1] + (us[i] - us[i - 1]) * ds[i - 1] / (ds[i - 1] - ds[i]);
            break;
        }
    return s;
}

}  // namespace at::murm
