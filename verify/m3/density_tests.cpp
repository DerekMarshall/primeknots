// Stage M3 Phase 2 — the Zubrilina density M₂(y), formula side (m3-pinning §P5).
//   theorem_zub_density_teeth   — derivative discontinuities at y = n²/4 (rider R4:
//                                 the locator + tolerance + expected {1/4,1,9/4}).
//   theorem_zub_density_features — positive root-number bias near the origin, and
//                                 the density oscillates (Z25 p.4).
#include "doctest/doctest.h"

#include <cmath>
#include <vector>

#include "harness.h"
#include "murm/zub_density.h"

using namespace at::murm;

TEST_CASE("theorem_zub_density_teeth") {
    // R4 (committed pre-run): grid step h = 0.004 over [0.02, 2.6], second-difference
    // spike locator; expected onsets 1/4, 1, 9/4; tolerance 0.01 (~2.5 grid steps).
    const std::vector<double> kinks = density_kink_locations(0.02, 2.6, 0.004, 50000);
    const double expected[3] = {0.25, 1.0, 2.25};
    const double tol = 0.01;
    int matched = 0;
    for (double e : expected) {
        bool found = false;
        for (double k : kinks) if (std::abs(k - e) < tol) found = true;
        CHECK(found);
        matched += found ? 1 : 0;
    }
    MESSAGE("density teeth (derivative discontinuities) located at "
            << (kinks.size() > 0 ? kinks[0] : -1) << ", "
            << (kinks.size() > 1 ? kinks[1] : -1) << ", "
            << (kinks.size() > 2 ? kinks[2] : -1) << " ≈ {1/4, 1, 9/4} (R4/P5)");
    REQUIRE(matched == 3);
}

TEST_CASE("theorem_zub_density_features") {
    // Z25 p.4: "positive root number bias for small P" — M₂(y) > 0 near the origin;
    // and the density oscillates (changes sign over the plotted range).
    CHECK(zub_density_m2(0.05, 50000) > 0);
    CHECK(zub_density_m2(0.15, 50000) > 0);
    int sign_changes = 0;
    double prev = zub_density_m2(0.05, 50000);
    for (double y = 0.10; y <= 2.5; y += 0.05) {
        const double cur = zub_density_m2(y, 50000);
        if ((prev < 0) != (cur < 0)) ++sign_changes;
        prev = cur;
    }
    MESSAGE("density: M₂(0.05)=" << zub_density_m2(0.05, 50000) << " > 0 (positive bias); "
            << sign_changes << " sign changes over [0.05,2.5] (oscillating)");
    CHECK(sign_changes >= 3);
    REQUIRE(zub_density_m2(0.05, 50000) > 0);
    REQUIRE(sign_changes >= 3);
}
