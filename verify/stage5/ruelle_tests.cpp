// Stage 5 — the model system: Ruelle zeta of a subshift, Euler product over
// closed orbits vs 1/det(I−L_s) (RESEARCH.md §7.3, rider R4,
// docs/notes/stage5-pinning.md §4).
#include "doctest/doctest.h"

#include <cmath>
#include <vector>

#include "harness.h"
#include "zeta/ruelle.h"

using namespace at::zeta;

TEST_CASE("twin_ruelle_orbit_vs_determinant") {
    // Orbit side (exp Σ tr(L_sⁿ)/n) vs determinant side (1/det(I−L_s)) agree over
    // a grid of s above the abscissa, for both the constant- and log-roof shifts.
    for (const SFT& m : {golden_mean_sft(), golden_mean_log_roof()}) {
        double a = abscissa_of_convergence(m);
        double maxd = 0;
        int n = 0;
        for (double s = a + 0.3; s <= a + 3.0; s += 0.1) {
            double d = std::fabs(ruelle_zeta_orbit(m, s, 200) -
                                 ruelle_zeta_determinant(m, s));
            maxd = std::max(maxd, d);
            ++n;
        }
        MESSAGE("roof[0]=" << m.roof[0] << " abscissa=" << a << " checked " << n
                << " points; max |orbit − det| = " << maxd);
        CHECK(maxd < 1e-9);
    }
}

TEST_CASE("theorem_bowen_lanford_orbit_counts") {
    // Fix(σⁿ) = tr(Aⁿ) = number of closed length-n paths. For the golden-mean
    // shift these are the Lucas numbers (independent recurrence referee).
    SFT gm = golden_mean_sft();
    long long l0 = 2, l1 = 1;  // L_0=2, L_1=1
    for (int n = 1; n <= 15; ++n) {
        long long Ln = (n == 1) ? 1 : (l0 + l1);  // L_n = L_{n-1}+L_{n-2}
        if (n >= 2) { l0 = l1; l1 = Ln; }
        long long got = closed_orbit_count(gm, n);
        CHECK(got == Ln);
    }
}

TEST_CASE("anchor_golden_mean_pole") {
    // Golden-mean const-roof closed form ζ(s)=1/(1−z−z²), z=e^{−s}: pole at
    // z=1/φ ⇔ s=log φ=0.48121 (abscissa). Determinant == closed form; ζ blows up
    // approaching the pole.
    SFT gm = golden_mean_sft();
    CHECK(std::fabs(abscissa_of_convergence(gm) - std::log(1.6180339887498949)) < 1e-4);
    // determinant == analytic closed form away from the pole
    for (double s : {0.7, 1.0, 1.7, 2.5}) {
        CHECK(std::fabs(ruelle_zeta_determinant(gm, s) - golden_mean_closed_form(s)) < 1e-12);
    }
    // pole: just above the abscissa the value is huge
    double near = golden_mean_closed_form(0.4813);
    MESSAGE("ζ(logφ+ε) = " << near);
    CHECK(near > 100.0);
}
