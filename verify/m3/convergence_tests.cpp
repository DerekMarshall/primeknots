// Stage M3 (completion) — the empirical murmuration convergence (m3-pinning §P6 /
// completion). The empirical short-interval statistic (Theorem 1) is assembled from
// the class-number trace formula over square-free levels and normalized by the
// newform dimension; it converges to M₂(y) with kinks (teeth) at y = n²/4.
//
// COMMITTED DESIGN (frozen before this confirmation run; m3-pinning §completion):
//   scale        X = 1800, window Y = ⌊X^0.70⌋ = 190 (Theorem-1 short interval),
//                per-prime sampling y = P/X ∈ (0, 2.35], 113 square-free levels.
//   detector     smooth half-width 0.025, resample grid 0.01 over [0.08, 2.33],
//                positive-Δ² local maxima above 0.30·max, min separation 0.20.
//   teeth        targets {0.25, 1.0, 2.25}; tolerance 0.12 (dominated by the y=9/4
//                window smear (9/4)(Y/X) ≈ 0.25, so a detected corner sits within
//                ~half that of the onset — a physical resolution, not a fitted knob).
//   convergence  L2 = RMS(emp − M₂) over the grid, reported at X ∈ {600,1000,1400,1800}.
//
//   theorem_zub_empirical_teeth       — PASS/FAIL: each tooth matched within tol.
//   theorem_zub_empirical_convergence — REPORTED empiric: L2 decreasing with X.
#include "doctest/doctest.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include "harness.h"
#include "mform/hurwitz.h"
#include "murm/zub_empirical.h"

using at::core::i64;
using at::core::u64;
using namespace at::murm;

namespace {
const KinkDetector kCommittedDetector{0.08, 2.33, 0.01, 0.025, 0.30, 0.20};
double nearest(const std::vector<double>& xs, double t) {
    double best = 1e9;
    for (double x : xs) best = std::min(best, std::abs(x - t));
    return best;
}
}  // namespace

TEST_CASE("theorem_zub_empirical_teeth") {
    ZubEmpirical e = zub_empirical_short(1800.0, 190, 2.35);
    const SmoothCurve s = smooth_empirical(e, kCommittedDetector);
    const std::vector<double> teeth = detect_teeth(s, kCommittedDetector);

    const double targets[3] = {0.25, 1.0, 2.25};
    const double tol = 0.12;
    int matched = 0;
    for (double g : targets) {
        const double d = nearest(teeth, g);
        CHECK(d <= tol);
        matched += (d <= tol) ? 1 : 0;
    }
    // The detector also fires on M₂'s intrinsic (smooth) oscillation maxima at high y;
    // the finite empirical resolution cannot separate those from the √-cusp teeth.
    // Reported, not asserted away — the discriminating claim is that {1/4,1,9/4} are
    // each present (a wrong y-convention would move them off n²/4).
    std::string list;
    for (double t : teeth) list += std::to_string(t).substr(0, 5) + " ";
    MESSAGE("theorem_zub_empirical_teeth: X=1800 Y=190 levels=" << e.n_levels
            << " points=" << e.points.size() << " L2=" << e.l2
            << "; detected kinks {" << list << "}; teeth {0.25,1.0,2.25} matched at dist "
            << nearest(teeth, 0.25) << ", " << nearest(teeth, 1.0) << ", "
            << nearest(teeth, 2.25) << " (tol " << tol << "); remaining detections are "
            "M₂ oscillation maxima");
    REQUIRE(matched == 3);
}

TEST_CASE("theorem_zub_empirical_convergence") {
    // One sieve (sized for the largest X) serves every scale.
    const i64 Pmax = static_cast<i64>(std::floor(2.35 * 1800.0));
    const at::mform::HurwitzTable H(4 * Pmax * (1800 + 190));
    struct S { double X; i64 Y; };
    const std::vector<S> grid = {{600, 88}, {1000, 126}, {1400, 159}, {1800, 190}};
    std::vector<double> l2s;
    for (const S& g : grid) {
        ZubEmpirical e = zub_empirical_with_table(g.X, g.Y, 2.35, H);
        l2s.push_back(e.l2);
    }
    // Reported empiric: the theorem is X→∞; at finite X the RMS distance to M₂ should
    // decrease as X grows. Strictly monotone here (gaps ≫ float noise).
    bool decreasing = true;
    for (std::size_t i = 1; i < l2s.size(); ++i)
        if (!(l2s[i] < l2s[i - 1])) decreasing = false;
    MESSAGE("theorem_zub_empirical_convergence: L2(RMS emp−M₂) = "
            << l2s[0] << " (X=600) → " << l2s[1] << " (1000) → " << l2s[2]
            << " (1400) → " << l2s[3] << " (1800) — decreasing with X");
    CHECK(decreasing);
    REQUIRE(l2s.back() < l2s.front());
}
