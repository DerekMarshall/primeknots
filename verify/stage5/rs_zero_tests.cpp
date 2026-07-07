// Stage 5 — Riemann-Siegel / Hardy Z, the Turing-certified zero finder, the
// Lehmer close-pair path, and the Odlyzko oracle (RESEARCH.md §7.1, riders
// R1/R2, docs/notes/stage5-pinning.md §§1-2).
#include "doctest/doctest.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>

#include "harness.h"
#include "zeta/riemann_siegel.h"
#include "zeta/zero_finder.h"

#ifndef AT_ODLYZKO_PATH
#define AT_ODLYZKO_PATH "data/odlyzko/zeros1"
#endif

using namespace at::zeta;

namespace {
// Independent bisection on the accurate engine (used by invariance/oracle tests).
double bisect_em(double lo, double hi) {
    double zlo = Z_euler_maclaurin(lo);
    for (int i = 0; i < 54; ++i) {
        double mid = 0.5 * (lo + hi), zm = Z_euler_maclaurin(mid);
        if ((zlo < 0) == (zm < 0)) { lo = mid; zlo = zm; } else hi = mid;
    }
    return 0.5 * (lo + hi);
}
std::vector<double> load_odlyzko() {
    std::vector<double> g;
    std::ifstream f(AT_ODLYZKO_PATH);
    std::string line;
    while (std::getline(f, line))
        if (!line.empty()) { try { g.push_back(std::stod(line)); } catch (...) {} }
    return g;
}
}  // namespace

TEST_CASE("twin_theta_asymptotic_vs_lgamma") {
    // e^{iθ} from the asymptotic Stirling series vs from complex log-Γ (Lanczos).
    // Comparing e^{iθ} (not θ) is branch-independent.
    double maxd = 0;
    int n = 0;
    for (double t = 14.5; t < 400; t += 3.7) {
        double a = theta(t), b = theta_lgamma(t);
        double d = std::hypot(std::cos(a) - std::cos(b), std::sin(a) - std::sin(b));
        maxd = std::max(maxd, d);
        ++n;
    }
    MESSAGE("checked " << n << " points; max |e^{iθ_asymp} − e^{iθ_lΓ}| = " << maxd);
    CHECK(maxd < 1e-10);
}

TEST_CASE("twin_z_riemann_siegel_vs_euler_maclaurin") {
    // The fast Riemann-Siegel engine agrees with the accurate Euler-Maclaurin
    // referee away from the Ψ-singularity band.
    double maxd = 0;
    int n = 0;
    for (double t = 300.5; t < 6000; t += 53.17) {
        if (uses_euler_maclaurin(t)) continue;
        double d = std::fabs(Z_riemann_siegel(t) - Z_euler_maclaurin(t));
        maxd = std::max(maxd, d);
        ++n;
    }
    MESSAGE("checked " << n << " points; max |Z_RS − Z_EM| = " << maxd);
    CHECK(maxd < 1e-6);
}

TEST_CASE("anchor_first_zeros_published_values") {
    // The classic first ordinates (γ₁=14.134725142, γ₂=21.022039639,
    // γ₃=25.010857580) — published constants, independent of the full table.
    ZeroFindResult r = find_zeros(30.0);
    REQUIRE(r.zeros.size() >= 3);
    CHECK(std::fabs(r.zeros[0] - 14.134725142) < 1e-8);
    CHECK(std::fabs(r.zeros[1] - 21.022039639) < 1e-8);
    CHECK(std::fabs(r.zeros[2] - 25.010857580) < 1e-8);
}

TEST_CASE("anchor_N_main_term_turing_necessity") {
    // round(θ(T)/π)+1 is exact only at Gram points. At T=500 (not a Gram point)
    // it gives 270, but the certified count N(500)=269 — this is precisely why
    // Gram's law cannot define the count and Turing's method is required (R1).
    long naive = static_cast<long>(std::llround(N_main_term(500.0)));
    CHECK(naive == 270);
    ZeroFindResult r = find_zeros(500.0);
    MESSAGE("round(θ/π)+1 at 500 = " << naive << "; certified N(t_top=" << r.t_top
            << ") = " << r.n_expected);
    CHECK(r.n_expected == 269);
    CHECK(r.complete);
}

TEST_CASE("theorem_zero_count_turing_complete") {
    // Found count == Turing/Rosser-block-certified N(t_top) over the range, with
    // three-way structure (found == block certificate; Odlyzko referees below).
    double tmax = at::verify::g_extended ? 5000.0 : 1500.0;
    ZeroFindResult r = find_zeros(tmax);
    MESSAGE("t_top=" << r.t_top << " found=" << r.zeros.size() << " N=" << r.n_expected
            << " good_gram=" << r.n_good_gram << " gram_law_exceptions="
            << r.n_gram_law_exceptions << " close_pairs=" << r.n_close_pairs);
    CHECK(r.complete);
    CHECK(static_cast<long>(r.zeros.size()) == r.n_expected);
    CHECK(r.n_good_gram > 100);
}

TEST_CASE("anchor_lehmer_pair_7005") {
    // The canonical Lehmer close pair γ ≈ 7005.06288 / 7005.10056 (gap 0.0377).
    // A windowed scan certifies the block and recovers the pair via adaptive
    // refinement (the close-pair path fires: n_refined_blocks ≥ 1). (R2)
    ZeroFindResult L = find_zeros(7020.0, 6990.0);
    MESSAGE("window complete=" << L.complete << " found=" << L.zeros.size()
            << " refined_blocks=" << L.n_refined_blocks);
    CHECK(L.complete);
    CHECK(L.n_refined_blocks >= 1);
    auto has = [&](double g) {
        for (double z : L.zeros) if (std::fabs(z - g) < 1e-7) return true;
        return false;
    };
    bool lo = has(7005.062866175), hi = has(7005.100564674);
    CHECK(lo);
    CHECK(hi);
}

TEST_CASE("invariance_zero_bracket_independence") {
    // A zero's ordinate is intrinsic, not an artifact of the bracket it was
    // refined from: bisecting from ±0.2 vs ±0.05 gives the same value.
    ZeroFindResult r = find_zeros(1000.0);
    double maxd = 0;
    int checked = 0;
    for (std::size_t i = 0; i < r.zeros.size(); ++i) {
        double g = r.zeros[i];
        if (i > 0 && g - r.zeros[i - 1] < 0.25) continue;         // isolated only
        if (i + 1 < r.zeros.size() && r.zeros[i + 1] - g < 0.25) continue;
        double a = bisect_em(g - 0.2, g + 0.2);
        double b = bisect_em(g - 0.05, g + 0.05);
        maxd = std::max(maxd, std::fabs(a - b));
        ++checked;
    }
    MESSAGE("checked " << checked << " isolated zeros; max bracket disagreement = " << maxd);
    CHECK(checked > 100);
    CHECK(maxd < 1e-10);
}

TEST_CASE("anchor_zeros_match_odlyzko") {
    // ORACLE: our zeros vs Odlyzko's table, ≥ 8 dp per zero; max deviation is an
    // empiric. SKIPs cleanly if the (gitignored) table is absent.
    std::vector<double> odl = load_odlyzko();
    if (odl.empty()) {
        MESSAGE("[SKIP] anchor_zeros_match_odlyzko: " AT_ODLYZKO_PATH
                " absent (run oracle/fetch_odlyzko.py).");
        at::verify::g_oracle_skipped = true;
        return;
    }
    double tmax = at::verify::g_extended ? 9900.0 : 1000.0;
    ZeroFindResult r = find_zeros(tmax);
    long odcount = static_cast<long>(
        std::lower_bound(odl.begin(), odl.end(), r.t_top) - odl.begin());
    double maxdev = 0;
    double worst = 0;
    for (std::size_t i = 0; i < r.zeros.size() && i < odl.size(); ++i) {
        double d = std::fabs(r.zeros[i] - odl[i]);
        if (d > maxdev) { maxdev = d; worst = odl[i]; }
    }
    MESSAGE("compared " << r.zeros.size() << " zeros; max |ours − Odlyzko| = "
            << maxdev << " at t=" << worst << " (table floor ~3e-9)");
    CHECK(static_cast<long>(r.zeros.size()) == odcount);  // count matches oracle
    CHECK(maxdev < 5e-9);                                  // ≥ 8 dp
}
