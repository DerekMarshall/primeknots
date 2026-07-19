#include "murm/ss_density.h"

#include <cmath>   // only std::sqrt (IEEE, portable) — NOT std::pow (libm, not portable)
#include <cstddef>
#include <vector>

#include "core/bessel.h"
#include "core/constants.h"
#include "mform/hurwitz.h"   // M3 Hurwitz class numbers, for the Eichler–Selberg trace (M4→M3 allowed)

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

namespace {
// dim_ℂ S_k(SL₂ℤ), even k ≥ 0 (level-1 closed form): 0 for k<12 or k=14; then the
// standard ⌊k/12⌋ (or ⌊k/12⌋−1 when k≡2 mod 12). Used only to shortcut Tr = 0.
long long dim_Sk_level1(int k) {
    if (k < 4 || (k % 2) != 0) return 0;
    const long long d = (k % 12 == 2) ? (k / 12 - 1) : (k / 12);
    return d < 0 ? 0 : d;
}
// P_k(t,n): coefficient of x^{k−2} in 1/(1 − t·x + n·x²); recurrence c_j = t·c_{j−1} − n·c_{j−2}
// (c_0=1, c_1=t), P_k = c_{k−2}. Exact integer. For k even, P_k(−t,n) = P_k(t,n).
long long gegenbauer_P(int k, long long t, long long n) {
    if (k <= 2) return 1;              // P_2 = c_0 = 1
    long long cprev = 1, ccur = t;     // c_0, c_1
    for (int j = 2; j <= k - 2; ++j) { const long long cj = t * ccur - n * cprev; cprev = ccur; ccur = cj; }
    return ccur;                       // c_{k−2}
}
// 12·H(D) as an exact integer (Hurwitz denominators divide 12).
long long twelve_hurwitz(long long D) {
    const at::mform::Frac h = at::mform::hurwitz(D);
    return h.num * (12 / h.den);
}
}  // namespace

long long level1_hecke_trace(int k, long long n) {
    // Tr(T_n | S_k(SL₂ℤ)) via the level-1 Eichler–Selberg trace formula, reusing M3's
    // Hurwitz class numbers (m4-pinning §P3 upgrade; supersedes the earlier weight-≥12
    // truncation, whose bound was certified only against the corrupted eq (2) products):
    //   Tr = −½ Σ_{t²≤4n} P_k(t,n)·H(4n−t²) − ½ Σ_{d|n} min(d,n/d)^{k−1}.
    // Exact integer arithmetic scaled by 12 (H's denominator). Tr is provably integral.
    // `n` is the Hecke index (a prime p in the local factors); 0 where S_k = 0.
    if (dim_Sk_level1(k) == 0) return 0;
    long long T12 = 0;                                  // 12·Σ_t P_k(t,n)·H(4n−t²)
    for (long long t = 0; t * t <= 4 * n; ++t) {
        const long long contrib = gegenbauer_P(k, t, n) * twelve_hurwitz(4 * n - t * t);
        T12 += (t == 0) ? contrib : 2 * contrib;        // ±t give equal terms (k even)
    }
    long long dsum = 0;                                 // Σ_{d|n} min(d,n/d)^{k−1}
    for (long long d = 1; d <= n; ++d)
        if (n % d == 0) {
            const long long mn = (d < n / d) ? d : n / d;
            long long pw = 1;
            for (int i = 0; i < k - 1; ++i) pw *= mn;
            dsum += pw;
        }
    const long long numer = T12 + 12 * dsum;            // Tr = −(T12 + 12·dsum)/24
    return -(numer / 24);
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

double ss_density_term(double u, long long q, long long m) {
    // One (q,m) summand of eq (2), WITHOUT the outer 2π√u. The single source of truth
    // for the corrected product assignment (ℓ̂ on p|q, ℓ on p|m,p∤q, both exponent
    // 2v_p(m)); ss_density sums it and the term-level anchors exercise it directly.
    if (!squarefree(q)) return 0.0;
    const long long g = gcd_(m, q);
    const int mu = mobius(g);
    if (mu == 0) return 0.0;
    double prod = 1.0;
    for (long long p : prime_factors(q))               // ∏_{p|q} ℓ̂_{p,2v_p(m)}
        prod *= ss_ell_hat(p, 2 * v_p(m, p));
    for (long long p : prime_factors(m)) {             // ∏_{p|m,p∤q} ℓ_{p,2v_p(m)}
        if (q % p == 0) continue;
        const double e = ss_ell(p, 2 * v_p(m, p));
        if (e == 0.0) return 0.0;                       // ℓ vanishes for p>3, 2v_p(m)<10
        prod *= e;
    }
    const double coef = static_cast<double>(mu) /
        (static_cast<double>(q) * static_cast<double>(m) * static_cast<double>(euler_phi(q / g)));
    return coef * prod * at::core::bessel_j1(4.0 * kPi * std::sqrt(u) * static_cast<double>(m)
                                             / static_cast<double>(q));
}

double ss_density(double u, long long m_bound, long long q_bound) {
    double sum = 0.0;
    for (long long q = 1; q <= q_bound; ++q) {
        if (!squarefree(q)) continue;
        for (long long m = 1; m <= m_bound; ++m) sum += ss_density_term(u, q, m);
    }
    return 2.0 * kPi * std::sqrt(u) * sum;
}

SSShape extract_shape(const std::vector<double>& us, const std::vector<double>& ds) {
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

SSShape ss_shape(long long B, double du) {
    // Sample the curve once, then extract globally (two-pass: the zero crossing must
    // be the one AFTER the GLOBAL hump, not the early low-u wiggle).
    std::vector<double> us, ds;
    for (double u = du; u <= 1.0 + 1e-9; u += du) { us.push_back(u); ds.push_back(ss_density(u, B, B)); }
    return extract_shape(us, ds);
}

}  // namespace at::murm
