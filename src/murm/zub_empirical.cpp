#include "murm/zub_empirical.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "core/int_types.h"
#include "core/primes.h"
#include "mform/dimension.h"
#include "mform/trace.h"
#include "murm/zub_density.h"

namespace at::murm {

using at::core::i64;
using at::core::u64;

namespace {
bool squarefree(i64 n) {
    for (i64 p = 2; p * p <= n; ++p)
        if (n % (p * p) == 0) return false;
    return true;
}
}  // namespace

ZubEmpirical zub_empirical_with_table(double X, long long Y, double y_max,
                                      const at::mform::HurwitzTable& H,
                                      unsigned dens_prime_bound) {
    ZubEmpirical out;
    out.X = X;
    const i64 Xi = static_cast<i64>(std::llround(X));
    out.win_lo = Xi;
    out.win_hi = Xi + Y;

    // The square-free levels of the window (fixed once; the same set feeds every P,
    // minus the P|N drops handled per prime).
    std::vector<i64> levels;
    for (i64 N = out.win_lo; N <= out.win_hi; ++N)
        if (squarefree(N)) levels.push_back(N);
    out.n_levels = static_cast<int>(levels.size());

    const i64 Pmax = static_cast<i64>(std::floor(y_max * X));
    // The sieve must cover the largest possible argument 4·Pmax·win_hi; fail loudly
    // rather than read out of bounds (a caller-sizing bug, never silent).
    if (H.bound() < 4 * Pmax * out.win_hi)
        throw std::runtime_error("zub_empirical: Hurwitz sieve too small for 4·Pmax·(X+Y)");

    double sq_sum = 0;
    int sq_cnt = 0;
    for (i64 P = 2; P <= Pmax; ++P) {
        if (!at::core::is_prime(static_cast<u64>(P))) continue;
        // Deterministic integer accumulation over the level set (fixed ascending
        // order); P|N levels are invalid for this prime (trace formula needs P∤N),
        // dropped from BOTH sums.
        at::mform::Frac numer{0, 1};
        i64 denom = 0;
        for (i64 N : levels) {
            if (N % P == 0) continue;
            numer = at::mform::add(numer, at::mform::newform_weighted_trace_k2(N, P, H));
            denom += at::mform::dim_s2_new(N);
        }
        if (denom <= 0) continue;
        ZubPoint pt;
        pt.p = static_cast<double>(P);
        pt.y = pt.p / X;
        pt.emp = static_cast<double>(numer.num) /
                 (static_cast<double>(numer.den) * static_cast<double>(denom));
        pt.dens = zub_density_m2(pt.y, dens_prime_bound);
        out.points.push_back(pt);
        sq_sum += (pt.emp - pt.dens) * (pt.emp - pt.dens);
        ++sq_cnt;
        if (4 * P * out.win_hi > out.max_arg) out.max_arg = 4 * P * out.win_hi;
    }
    out.l2 = sq_cnt ? std::sqrt(sq_sum / sq_cnt) : 0.0;
    return out;
}

ZubEmpirical zub_empirical_short(double X, long long Y, double y_max,
                                 unsigned dens_prime_bound) {
    const i64 Xi = static_cast<i64>(std::llround(X));
    const i64 Pmax = static_cast<i64>(std::floor(y_max * X));
    // Size the sieve to the max argument 4·Pmax·(X+Y).
    const at::mform::HurwitzTable H(4 * Pmax * (Xi + Y));
    return zub_empirical_with_table(X, Y, y_max, H, dens_prime_bound);
}

SmoothCurve smooth_empirical(const ZubEmpirical& e, const KinkDetector& kd) {
    SmoothCurve s;
    for (double y = kd.y_lo; y <= kd.y_hi + 1e-9; y += kd.grid) {
        double sum = 0;
        int cnt = 0;
        for (const ZubPoint& p : e.points)
            if (std::abs(p.y - y) <= kd.smooth_hw) { sum += p.emp; ++cnt; }
        if (cnt > 0) { s.y.push_back(y); s.v.push_back(sum / cnt); }
    }
    return s;
}

std::vector<double> detect_teeth(const SmoothCurve& s, const KinkDetector& kd) {
    const int n = static_cast<int>(s.v.size());
    std::vector<double> d2(n, 0.0);
    double maxabs = 0;
    for (int i = 1; i < n - 1; ++i) {
        d2[i] = s.v[i + 1] - 2 * s.v[i] + s.v[i - 1];   // signed (uniform grid)
        maxabs = std::max(maxabs, std::abs(d2[i]));
    }
    const double thresh = kd.threshold_frac * maxabs;
    std::vector<double> teeth;
    for (int i = 1; i < n - 1; ++i) {
        if (d2[i] < thresh) continue;                    // POSITIVE (convex / √-onset)
        if (d2[i] < d2[i - 1] || d2[i] < d2[i + 1]) continue;   // local max
        if (!teeth.empty() && s.y[i] - teeth.back() < kd.min_sep) continue;
        teeth.push_back(s.y[i]);
    }
    return teeth;
}

}  // namespace at::murm
