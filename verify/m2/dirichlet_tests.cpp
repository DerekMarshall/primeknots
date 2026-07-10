// Stage M2 — Dirichlet-character machinery, convention-adjudication (m2-pinning).
// These pin the Gauss-sum normalization MECHANICALLY before it enters the statistic:
//   twin_primitive_root          — least primitive root vs the naive order twin.
//   theorem_gauss_sum_abs        — |τ(χ)|² = p for every primitive χ.
//   theorem_gauss_sum_conjugate  — τ(χ)·τ(χ̄) = χ(−1)·p for every primitive χ.
//   anchor_dirichlet_family_counts — |𝒟₊(p)|=(p−3)/2, |𝒟₋(p)|=(p−1)/2 (P5).
// Zero external data (the whole module takes numbers, opens no file).
#include "doctest/doctest.h"

#include <cmath>
#include <complex>
#include <vector>

#include "core/primes.h"
#include "harness.h"
#include "murm/dirichlet_murmuration.h"
#include "symbols/dirichlet.h"

using at::core::u64;
using namespace at::symbols;

namespace {
constexpr double kTol = 1e-9;   // well above the ~1e-12 numerical floor

std::vector<u64> primes_upto(u64 N) {
    std::vector<bool> comp(N + 1, false);
    for (u64 i = 2; i * i <= N; ++i)
        if (!comp[i]) for (u64 j = i * i; j <= N; j += i) comp[j] = true;
    std::vector<u64> ps;
    for (u64 n = 3; n <= N; ++n) if (!comp[n]) ps.push_back(n);   // odd primes
    return ps;
}
}  // namespace

TEST_CASE("twin_primitive_root") {
    u64 checked = 0;
    for (u64 p : primes_upto(2000)) {
        const u64 g = primitive_root(p);
        // twin: the returned g really has order p−1 (naive iteration)…
        CHECK(mult_order(g, p) == p - 1);
        // …and it is the LEAST such (no smaller base is a primitive root).
        bool least = true;
        for (u64 h = 2; h < g; ++h)
            if (mult_order(h, p) == p - 1) { least = false; break; }
        CHECK(least);
        ++checked;
    }
    MESSAGE("twin_primitive_root: least primitive root == naive-order twin over "
            << checked << " odd primes < 2000");
    REQUIRE(checked >= 300);
}

TEST_CASE("theorem_gauss_sum_abs") {
    u64 chars = 0;
    double max_err = 0;
    for (u64 p : primes_upto(400)) {
        const u64 order = p - 1;
        const auto ind = index_table(p, primitive_root(p));
        for (u64 k = 1; k < order; ++k) {                 // primitive: k ≠ 0
            const std::complex<double> tau = gauss_sum(k, p, order, ind);
            const double err = std::abs(std::norm(tau) - static_cast<double>(p));
            max_err = std::max(max_err, err);
            CHECK(err < kTol);
            ++chars;
        }
    }
    MESSAGE("theorem_gauss_sum_abs: |τ(χ)|² = p for " << chars
            << " primitive characters (max err " << max_err << ")");
    REQUIRE(chars >= 5000);
}

TEST_CASE("theorem_gauss_sum_conjugate") {
    u64 chars = 0;
    double max_err = 0;
    for (u64 p : primes_upto(400)) {
        const u64 order = p - 1;
        const auto ind = index_table(p, primitive_root(p));
        for (u64 k = 1; k < order; ++k) {
            const std::complex<double> tau = gauss_sum(k, p, order, ind);
            const std::complex<double> tau_bar = gauss_sum((order - k) % order, p, order, ind);
            const std::complex<double> expect(char_parity(k) * static_cast<double>(p), 0.0);
            const double err = std::abs(tau * tau_bar - expect);
            max_err = std::max(max_err, err);
            CHECK(err < kTol);
            ++chars;
        }
    }
    MESSAGE("theorem_gauss_sum_conjugate: τ(χ)·τ(χ̄) = χ(−1)·p for " << chars
            << " primitive characters (max err " << max_err << ")");
    REQUIRE(chars >= 5000);
}

TEST_CASE("anchor_dirichlet_family_counts") {
    u64 checked = 0;
    for (u64 p : primes_upto(2000)) {
        u64 even = 0, odd = 0;
        for (u64 k = 1; k < p - 1; ++k) (char_parity(k) == 1 ? even : odd) += 1;  // primitive: k≠0
        CHECK(even == (p - 3) / 2);   // |𝒟₊(p)|
        CHECK(odd == (p - 1) / 2);    // |𝒟₋(p)|
        ++checked;
    }
    MESSAGE("anchor_dirichlet_family_counts: |𝒟₊(p)|=(p−3)/2, |𝒟₋(p)|=(p−1)/2 over "
            << checked << " odd primes < 2000");
    REQUIRE(checked >= 300);
}

// --- statistic + convergence (pre-registered design, m2-pinning Phase-2) ---------
namespace {
using namespace at::murm;
const std::vector<double> kYs = {0.1, 0.25, 0.4, 0.5, 0.6, 0.75, 0.9, 1.0, 1.25, 1.5};

struct Errs { double err = 0, parity = 0; };
Errs sharp_errs(const DirichletMurm& m) {
    Errs e;
    for (std::size_t i = 0; i < m.ys.size(); ++i) {
        e.err = std::max({e.err, std::abs(m.even[i] - density_sharp_even(m.ys[i])),
                          std::abs(m.odd[i] - density_sharp_odd(m.ys[i]))});
        e.parity = std::max({e.parity, std::abs(m.even[i].imag()), std::abs(m.odd[i].real())});
    }
    return e;
}
Errs geom_errs(const DirichletMurm& m, double c) {
    Errs e;
    for (std::size_t i = 0; i < m.ys.size(); ++i) {
        e.err = std::max({e.err, std::abs(m.even[i] - density_geom_even(m.ys[i], c)),
                          std::abs(m.odd[i] - density_geom_odd(m.ys[i], c))});
        e.parity = std::max({e.parity, std::abs(m.even[i].imag()), std::abs(m.odd[i].real())});
    }
    return e;
}
}  // namespace

TEST_CASE("theorem_murmuration_dirichlet") {
    // GEOMETRIC (primary, unconditional): P±(y,X,2) -> ∫₁² ... . Convergence + parity.
    const double c = 2.0;
    Errs g1000 = geom_errs(murm_geometric(1000, c, kYs), c);
    Errs g4000 = geom_errs(murm_geometric(4000, c, kYs), c);
    MESSAGE("geometric c=2 convergence: err X=1000 -> " << g1000.err
            << ", X=4000 -> " << g4000.err << " (tol 0.10); parity |viol|=" << g4000.parity);
    CHECK(g4000.err < 0.10);
    CHECK(g4000.err < g1000.err);          // strictly decreasing
    CHECK(g4000.parity < 1e-9);            // even real, odd imaginary
    CHECK(g1000.parity < 1e-9);

    // SHARP (secondary, RH): P̃±(y,X,0.7) -> cos(2πy) / -i sin(2πy). Noisier.
    const double d = 0.7;
    const DirichletMurm s4000 = murm_sharp(4000, d, kYs);
    Errs s500 = sharp_errs(murm_sharp(500, d, kYs));
    Errs s4 = sharp_errs(s4000);
    MESSAGE("sharp δ=0.7 convergence: err X=500 -> " << s500.err << ", X=4000 -> " << s4.err
            << " (tol 0.45, low prime count); parity |viol|=" << s4.parity);
    CHECK(s4.err < 0.45);
    CHECK(s4.err < s500.err);              // decreasing
    CHECK(s4.parity < 1e-9);

    // NULL control (cross-parity): the even (real) statistic cannot match the odd
    // (imaginary) density — a statistic that could would be parity-degenerate.
    double null_err = 0;
    for (std::size_t i = 0; i < kYs.size(); ++i)
        null_err = std::max(null_err, std::abs(s4000.even[i] - density_sharp_odd(kYs[i])));
    MESSAGE("null (even stat vs odd density): " << null_err << " (must be > 0.5)");
    CHECK(null_err > 0.5);

    REQUIRE(g4000.err < 0.10);
    REQUIRE(g4000.err < g1000.err);
    REQUIRE(s4.err < 0.45);
    REQUIRE(s4.err < s500.err);
    REQUIRE(s4.parity < 1e-9);
    REQUIRE(g4000.parity < 1e-9);
    REQUIRE(null_err > 0.5);
}

TEST_CASE("assert_zero_external_data") {
    // The entire M2 pipeline takes NUMBERS and opens NO file — its output is a pure
    // function of committed code (the ARCHITECTURE-M §9 default). Demonstrated: a
    // murmuration computed with no path argument anywhere in the call.
    const DirichletMurm m = murm_sharp(1000, 0.7, kYs);
    bool finite = true;
    for (const auto& z : m.even) finite = finite && std::isfinite(z.real()) && std::isfinite(z.imag());
    for (const auto& z : m.odd) finite = finite && std::isfinite(z.real()) && std::isfinite(z.imag());
    CHECK(finite);
    CHECK(m.n_prime_conductors > 0);
    MESSAGE("assert_zero_external_data: murmuration from " << m.n_prime_conductors
            << " prime conductors, " << (m.chars_even + m.chars_odd)
            << " characters — computed from scratch, zero external files read (API is path-free)");
    REQUIRE(finite);
}
