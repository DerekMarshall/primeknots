#include "murm/dirichlet_murmuration.h"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <thread>

#include "core/constants.h"
#include "core/primes.h"
#include "symbols/dirichlet.h"

namespace at::murm {

using at::core::is_prime;
using at::core::kTwoPi;   // generated (oracle/gen_constants.py), never typed
using at::core::u64;
using at::symbols::char_parity;
using at::symbols::chi_value;
using at::symbols::gauss_sum;
using at::symbols::index_table;
using at::symbols::primitive_root;

namespace {
u64 next_prime(u64 n) {
    if (n < 2) return 2;
    while (!is_prime(n)) ++n;
    return n;
}

// The statistic over prime conductors N ∈ [lo,hi], scaled by `prefactor`. χ(⌈yX⌉_p)
// is precomputed per y (the prime depends only on y,X, not N); τ(χ) is computed once
// per (N,χ). Accumulation order is fixed (N asc, k asc) → deterministic.
DirichletMurm compute(double X, u64 lo, u64 hi, double prefactor,
                      const std::string& interval, double param,
                      const std::vector<double>& ys) {
    DirichletMurm r;
    r.X = X;
    r.interval = interval;
    r.param = param;
    r.ys = ys;
    const std::size_t Y = ys.size();

    std::vector<u64> P(Y);
    for (std::size_t i = 0; i < Y; ++i)
        P[i] = next_prime(static_cast<u64>(std::ceil(ys[i] * X)));

    std::vector<u64> Ns;
    for (u64 N = lo; N <= hi; ++N) if (is_prime(N)) Ns.push_back(N);
    r.n_prime_conductors = Ns.size();

    // Per-conductor partial sums (independent per N); reduced below in N-order so the
    // result is deterministic regardless of thread scheduling.
    std::vector<std::vector<std::complex<double>>> pe(Ns.size(), std::vector<std::complex<double>>(Y, {0.0, 0.0}));
    std::vector<std::vector<std::complex<double>>> po(Ns.size(), std::vector<std::complex<double>>(Y, {0.0, 0.0}));
    std::vector<u64> ce(Ns.size(), 0), co(Ns.size(), 0);

    auto work = [&](std::size_t j) {
        const u64 N = Ns[j], order = N - 1;
        const auto ind = index_table(N, primitive_root(N));
        for (u64 k = 1; k < order; ++k) {                 // primitive characters
            const std::complex<double> tau = gauss_sum(k, N, order, ind);
            const bool even = (char_parity(k) == 1);
            (even ? ce[j] : co[j]) += 1;
            for (std::size_t i = 0; i < Y; ++i) {
                const std::complex<double> term = chi_value(k, order, ind, P[i]) / tau;
                (even ? pe[j][i] : po[j][i]) += term;
            }
        }
    };

    const unsigned hw = std::max(2u, std::thread::hardware_concurrency());
    std::atomic<std::size_t> next{0};
    auto worker = [&] { std::size_t j; while ((j = next.fetch_add(1)) < Ns.size()) work(j); };
    std::vector<std::thread> pool;
    for (unsigned t = 0; t < hw; ++t) pool.emplace_back(worker);
    for (std::thread& th : pool) th.join();

    r.even.assign(Y, {0.0, 0.0});
    r.odd.assign(Y, {0.0, 0.0});
    for (std::size_t j = 0; j < Ns.size(); ++j) {         // reduce in N-order
        r.chars_even += ce[j];
        r.chars_odd += co[j];
        for (std::size_t i = 0; i < Y; ++i) { r.even[i] += pe[j][i]; r.odd[i] += po[j][i]; }
    }
    for (std::size_t i = 0; i < Y; ++i) { r.even[i] *= prefactor; r.odd[i] *= prefactor; }
    return r;
}

// ∫₁^c f(2πy/x) dx by composite Simpson (smooth integrand; n even).
double integral(double y, double c, bool use_cos, int n = 20000) {
    auto fx = [&](double x) {
        const double t = kTwoPi * y / x;
        return use_cos ? std::cos(t) : std::sin(t);
    };
    const double h = (c - 1.0) / n;
    double s = fx(1.0) + fx(c);
    for (int i = 1; i < n; ++i) s += (i % 2 ? 4.0 : 2.0) * fx(1.0 + i * h);
    return s * h / 3.0;
}
}  // namespace

DirichletMurm murm_sharp(double X, double delta, const std::vector<double>& ys) {
    const u64 lo = static_cast<u64>(std::ceil(X));
    const u64 hi = static_cast<u64>(std::floor(X + std::pow(X, delta)));
    return compute(X, lo, hi, std::log(X) / std::pow(X, delta), "sharp", delta, ys);
}

DirichletMurm murm_geometric(double X, double c, const std::vector<double>& ys) {
    const u64 lo = static_cast<u64>(std::ceil(X));
    const u64 hi = static_cast<u64>(std::floor(c * X));
    return compute(X, lo, hi, std::log(X) / X, "geometric", c, ys);
}

std::complex<double> density_sharp_even(double y) { return {std::cos(kTwoPi * y), 0.0}; }
std::complex<double> density_sharp_odd(double y) { return {0.0, -std::sin(kTwoPi * y)}; }
std::complex<double> density_geom_even(double y, double c) { return {integral(y, c, true), 0.0}; }
std::complex<double> density_geom_odd(double y, double c) { return {0.0, -integral(y, c, false)}; }

}  // namespace at::murm
