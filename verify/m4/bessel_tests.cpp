// Stage M4 (C4) — from-scratch Bessel J₁, the density's transcendental ingredient
// built in-house (IEEE ops only) so no emitted snapshot byte depends on libm
// (m4-pinning §C4 / §P3; [[cross-compiler-emit-determinism]]).
//   anchor_bessel_j1        — published J₁ values + oddness + the first zero.
//   twin_bessel_j1_vs_pari  — J₁ vs PARI besselj(1,·) over a sample spanning the
//                             series/asymptotic switchover and up to the density's
//                             large-argument range. Correctness referee (not bit-exact).
#include "doctest/doctest.h"

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

#include "core/bessel.h"
#include "core/constants.h"
#include "harness.h"
#include "oracle/pari.h"

using at::core::bessel_j1;

TEST_CASE("anchor_generated_constants") {
    // The high-precision identities (hi+lo reconstructs π/2 to <1e-30; hi = nearest
    // double to π/2) are asserted in the generator and re-checked by ctest
    // `constants.regen_matches`. Here: the in-code double values are self-consistent.
    using namespace at::core;
    CHECK(kHalfPiHi == kHalfPi);                 // hi is the nearest double to π/2
    CHECK(kHalfPiHi + kHalfPiLo == kHalfPi);     // lo is a sub-ulp residual
    CHECK(kTwoPi == 2.0 * kPi);                  // doubling is exact
    CHECK(kTwoPi == 4.0 * kHalfPi);
    CHECK(kPi > 3.14159265);                     // (doctest can't decompose &&; split)
    CHECK(kPi < 3.14159266);
    CHECK(kTwoOverPi > 0.63661977);
    CHECK(kTwoOverPi < 0.63661978);
    MESSAGE("anchor_generated_constants: kPi/kTwoPi/kHalfPi/kTwoOverPi + hi/lo split "
            "self-consistent (high-precision identities in oracle/gen_constants.py)");
}

TEST_CASE("anchor_bessel_j1") {
    // Standard tabulated values (DLMF / A&S table 9.1).
    CHECK(bessel_j1(0.0) == 0.0);
    CHECK(std::abs(bessel_j1(1.0) - 0.4400505857449335) < 1e-12);
    CHECK(std::abs(bessel_j1(2.0) - 0.5767248077568734) < 1e-12);
    CHECK(std::abs(bessel_j1(10.0) - 0.04347274616886144) < 1e-11);   // asymptotic side
    // oddness J₁(−x) = −J₁(x)
    CHECK(std::abs(bessel_j1(-1.0) + 0.4400505857449335) < 1e-12);
    CHECK(std::abs(bessel_j1(-3.5) + bessel_j1(3.5)) < 1e-15);
    // first positive zero j_{1,1} ≈ 3.831705970207512
    CHECK(std::abs(bessel_j1(3.831705970207512)) < 1e-9);
    MESSAGE("anchor_bessel_j1: J₁(1)=0.44005…, J₁(2)=0.57672…, J₁(10)=0.04347…, "
            "odd, first zero 3.8317… — all matched");
}

TEST_CASE("twin_bessel_j1_vs_pari") {
    std::optional<std::string> gp = oracle::find_gp();
    if (!gp) {
        MESSAGE("[SKIP] twin_bessel_j1_vs_pari: PARI/GP `gp` not found on PATH.");
        at::verify::g_oracle_skipped = true;
        return;
    }
    MESSAGE("oracle: using gp at " << *gp);
    // Sample across the series range, the switchover seam (12.0), the asymptotic range,
    // and out to the density's large-argument regime (J₁(4π√u·m/q), m up to ~10⁵).
    const std::vector<double> xs = {
        0.1, 0.5, 1.0, 2.0, 3.5, 5.0, 8.0, 11.5, 11.9, 12.0, 12.1, 13.0,
        15.0, 20.0, 37.5, 50.0, 100.0, 250.0, 1000.0, 4321.0, 12345.678,
        50000.0, 100000.0, 125663.7};
    std::ostringstream script;
    script.setf(std::ios::fixed);
    script.precision(4);
    // `print(...)` (not a bare expression): gp with a script-file argument only emits
    // output for print(), unlike interactive/stdin mode (the run_gp invocation, C4).
    for (double x : xs) script << "print(besselj(1," << x << "))\n";
    const std::string out = oracle::run_gp(*gp, script.str());

    std::istringstream lines(out);
    std::string line;
    double maxdev = 0;
    unsigned checked = 0;
    for (double x : xs) {
        REQUIRE(std::getline(lines, line));
        const double pari = std::stod(line);
        const double ours = bessel_j1(x);
        const double dev = std::abs(ours - pari);
        maxdev = std::max(maxdev, dev);
        CHECK(dev < 1e-8);
        ++checked;
    }
    MESSAGE("twin_bessel_j1_vs_pari: " << checked << " points vs PARI besselj(1,·) "
            "(series/asymptotic, x up to 1.26e5), max deviation " << maxdev);
    REQUIRE(checked == xs.size());
}
