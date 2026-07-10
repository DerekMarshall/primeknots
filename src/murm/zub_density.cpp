#include "murm/zub_density.h"

#include <cmath>

namespace at::murm {

namespace {
const double kPi = 3.14159265358979323846264338327950288;

std::vector<long long> primes_below(unsigned B) {
    std::vector<bool> comp(B, false);
    std::vector<long long> ps;
    for (unsigned i = 2; i < B; ++i) {
        if (!comp[i]) { ps.push_back(i); for (unsigned j = 2 * i; j < B; j += i) comp[j] = true; }
    }
    return ps;
}

// The Euler-product constants (Z25 Theorem 1, verbatim), truncated to primes < B.
struct Constants { double alpha, beta, gamma; std::vector<long long> primes; };
Constants constants(unsigned B) {
    Constants c;
    c.primes = primes_below(B);
    double a = 1, b = 1, g = 1;
    for (long long p : c.primes) {
        const double p2 = p * p, p3 = p2 * p, p4 = p2 * p2;
        a *= (1 - p - 2 * p2 + p4) / (p4 - 2 * p2 + p);
        b *= (-1 + p2 + p3) / (p * (-1 + p + p2));
        g *= (p * (1 + p)) / (-1 + p + p2);
    }
    c.alpha = 2 * kPi * a;
    c.beta = 2 * kPi * b;
    c.gamma = 12 * g;
    return c;
}

// ν(r) = Π_{p|r} (1 + p²/(p⁴ − 2p² − p + 1)); one factor per distinct prime of r.
double factor_nu(double p) {
    const double p2 = p * p, p4 = p2 * p2;
    return 1 + p2 / (p4 - 2 * p2 - p + 1);
}
double nu(long long r, const std::vector<long long>& primes) {
    double v = 1;
    for (long long p : primes) {
        if (p * p > r) break;
        if (r % p == 0) { v *= factor_nu(static_cast<double>(p)); while (r % p == 0) r /= p; }
    }
    if (r > 1) v *= factor_nu(static_cast<double>(r));   // leftover prime factor > √r
    return v;
}
}  // namespace

double zub_density_m2(double y, unsigned prime_bound) {
    const Constants c = constants(prime_bound);
    double sum = 0;
    const long long rmax = static_cast<long long>(std::floor(2 * std::sqrt(y)));
    for (long long r = 1; r <= rmax; ++r)
        sum += nu(r, c.primes) * std::sqrt(4 * y - r * r);
    return c.alpha * sum + c.beta * std::sqrt(y) - c.gamma * y;
}

std::vector<double> density_kink_locations(double lo, double hi, double h, unsigned prime_bound) {
    const int n = static_cast<int>((hi - lo) / h) + 1;
    std::vector<double> M(n);
    for (int i = 0; i < n; ++i) M[i] = zub_density_m2(lo + i * h, prime_bound);
    // Second difference; onsets show a spike. Report local maxima of |Δ²M| above a
    // threshold scaled to the curve (the vertical tangents dominate the smooth part).
    std::vector<double> d2(n, 0.0);
    double maxd2 = 0;
    for (int i = 1; i < n - 1; ++i) { d2[i] = std::abs(M[i + 1] - 2 * M[i] + M[i - 1]); maxd2 = std::max(maxd2, d2[i]); }
    std::vector<double> kinks;
    const double thresh = 0.25 * maxd2;
    for (int i = 2; i < n - 2; ++i)
        if (d2[i] > thresh && d2[i] >= d2[i - 1] && d2[i] > d2[i + 1]) kinks.push_back(lo + i * h);
    return kinks;
}

}  // namespace at::murm
