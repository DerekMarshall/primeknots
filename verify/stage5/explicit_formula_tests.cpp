// Stage 5 — the von Mangoldt explicit formula: spectral reconstruction converges
// to the midpoint-normalized ψ₀ (RESEARCH.md §7.2, rider R3, open-questions R6,
// docs/notes/stage5-pinning.md §3).
#include "doctest/doctest.h"

#include <cmath>
#include <vector>

#include "harness.h"
#include "zeta/explicit_formula.h"
#include "zeta/zero_finder.h"

using namespace at::zeta;

TEST_CASE("anchor_psi_midpoint_at_prime_power") {
    // At x=8=2³, ψ jumps by Λ(8)=log2; the reference is the MIDPOINT
    // ψ(8)−log2/2 = 6.386828…, not ψ(8)=6.733402… (the R6 erratum).
    CHECK(std::fabs(von_mangoldt(8.0) - std::log(2.0)) < 1e-12);
    CHECK(std::fabs(psi_chebyshev(8.0) - 6.733401891) < 1e-6);
    double mid = psi0_reference(8.0);
    CHECK(std::fabs(mid - (psi_chebyshev(8.0) - 0.5 * std::log(2.0))) < 1e-12);
    CHECK(std::fabs(mid - 6.386828302) < 1e-6);
    // non-prime-power integer (6=2·3) and non-integer: ψ₀ = ψ, no midpoint shift.
    CHECK(von_mangoldt(6.0) == 0.0);
    CHECK(std::fabs(psi0_reference(10.5) - psi_chebyshev(10.5)) < 1e-15);
}

TEST_CASE("theorem_explicit_formula_convergence") {
    // Spectral reconstruction converges to ψ₀: error decreases with the zero
    // count M (envelope ~ √x/γ_M) at non-prime-power x, and at a prime power it
    // approaches the jump MIDPOINT — NOT ψ(x) itself (spec-bug guard).
    double tmax = at::verify::g_extended ? 4000.0 : 2000.0;
    ZeroFindResult r = find_zeros(tmax);
    const auto& G = r.zeros;
    REQUIRE(G.size() > 500);
    const double gamma_top = G.back();

    // (a) non-prime-power: coarse decrease + envelope bound.
    for (double x : {10.5, 20.3, 27.7}) {
        double e_small = std::fabs(psi_reconstruct(x, G, 20) - psi0_reference(x));
        double e_big = std::fabs(psi_reconstruct(x, G, G.size()) - psi0_reference(x));
        double env = std::sqrt(x) / gamma_top;
        MESSAGE("x=" << x << " err(20)=" << e_small << " err(all)=" << e_big
                << " envelope √x/γ_M=" << env);
        CHECK(e_big < e_small);        // more zeros ⇒ smaller error (coarse trend)
        CHECK(e_big < 60.0 * env);     // within the truncation envelope
    }

    // (b) prime powers: converge to the midpoint, and stay far from ψ(x) itself.
    for (double x : {8.0, 9.0, 25.0}) {
        double to_mid = std::fabs(psi_reconstruct(x, G, G.size()) - psi0_reference(x));
        double to_psi = std::fabs(psi_reconstruct(x, G, G.size()) - psi_chebyshev(x));
        double half_lambda = 0.5 * von_mangoldt(x);
        MESSAGE("x=" << x << " |approx−midpoint|=" << to_mid
                << " |approx−ψ(x)|=" << to_psi << " (Λ/2=" << half_lambda << ")");
        CHECK(to_mid < 0.15);                 // approaches the midpoint
        CHECK(to_psi > 0.5 * half_lambda);    // NOT converging to ψ(x) — R6 guard
    }
}
