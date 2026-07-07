#include "zeta/explicit_formula.h"

#include <cmath>

#include "core/primes.h"

namespace at::zeta {
namespace {
constexpr double kPi = 3.14159265358979323846;

// If n is a prime power p^k (k≥1) return log p, else 0. Trivial trial division;
// x range here is tiny (test points ≤ a few hundred).
double lambda_int(at::core::u64 n) {
    if (n < 2) return 0.0;
    at::core::u64 p = 0;
    for (at::core::u64 d = 2; d * d <= n; ++d)
        if (n % d == 0) { p = d; break; }   // smallest prime factor
    if (p == 0) p = n;                        // n itself is prime
    at::core::u64 m = n;
    while (m % p == 0) m /= p;
    return (m == 1) ? std::log(static_cast<double>(p)) : 0.0;  // n = p^k
}
}  // namespace

double von_mangoldt(double x) {
    double r = std::llround(x);
    if (std::fabs(x - r) > 1e-9 || r < 2) return 0.0;  // not an integer ≥ 2
    return lambda_int(static_cast<at::core::u64>(r));
}

double psi_chebyshev(double x) {
    double s = 0.0;
    if (x < 2) return 0.0;
    auto X = static_cast<at::core::u64>(std::floor(x + 1e-9));
    for (at::core::u64 p = 2; p <= X; ++p) {
        if (!at::core::is_prime(p)) continue;
        double lp = std::log(static_cast<double>(p));
        for (at::core::u128 pk = p; pk <= X; pk *= p) s += lp;  // p, p², p³, …
    }
    return s;
}

double psi0_reference(double x) {
    return psi_chebyshev(x) - 0.5 * von_mangoldt(x);
}

double psi_reconstruct(double x, const std::vector<double>& gammas, std::size_t M) {
    if (M > gammas.size()) M = gammas.size();
    const double L = std::log(x);
    const double sx = std::sqrt(x);
    double wave = 0.0;
    for (std::size_t n = 0; n < M; ++n) {
        double g = gammas[n];
        // 2 Re(x^ρ/ρ) = 2√x (½cos(γL) + γ sin(γL)) / (¼ + γ²)   [conjugate pair]
        wave += 2.0 * sx * (0.5 * std::cos(g * L) + g * std::sin(g * L))
                / (0.25 + g * g);
    }
    return x - wave - std::log(2.0 * kPi) - 0.5 * std::log(1.0 - 1.0 / (x * x));
}

}  // namespace at::zeta
