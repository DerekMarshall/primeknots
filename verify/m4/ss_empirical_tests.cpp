// Stage M4 — the height-murmuration confirmation (m4-pinning "confirmation run" §).
// Reads the committed empirical run (data/m4/ss_empirical.txt), and:
//   theorem_ss_height_murmuration — at X_confirm=10⁴, the two agreeing committed shape
//     invariants (hump, first zero-crossing) are within the a-priori τ=0.06 (§R0c), and
//     the trough is asserted to be the OPEN DEVIATION the fork resolved to (dev > τ,
//     flat over the ladder, windowed==global so not a tail artifact) — the miss is
//     PINNED, not hidden. claim_class = "partial agreement, open deviation".
//   twin_ap_fast_vs_charsum_m4 — the statistic's a_p engine (ell::ap_fast) equals the
//     frozen referee (ell::ap_charsum) over the M4 large-prime regime (rule 2).
#include "doctest/doctest.h"

#include <cmath>
#include <stdexcept>
#include <vector>

#include "ell/ap.h"
#include "ell/ap_fast.h"
#include "ell/curve.h"
#include "harness.h"
#include "murm/height_family.h"
#include "murm/ss_empirical.h"

using at::core::i64;
using at::core::u64;
using namespace at::murm;

#ifndef AT_M4_DATA_DIR
#define AT_M4_DATA_DIR "data/m4"
#endif

TEST_CASE("theorem_ss_height_murmuration") {
    const std::string path = std::string(AT_M4_DATA_DIR) + "/ss_empirical.txt";
    SSRun run;
    try {
        run = read_ss_run(path);
    } catch (const std::exception& e) {
        MESSAGE("[SKIP] theorem_ss_height_murmuration: run unreadable/stale (" << e.what() << ").");
        at::verify::g_oracle_skipped = true;
        return;
    }

    const double tol = run.tol;   // a-priori τ = 0.06 (§R0c)
    const SSShape& s = run.confirm.shape;
    const double dev_hump = std::abs(s.hump_u - run.r2_hump);
    const double dev_zero = std::abs(s.zero_u - run.r2_zero);
    const double dev_trough = std::abs(s.trough_u - run.r2_trough);

    MESSAGE("X_confirm=" << run.X_confirm << " |fam|=" << run.confirm.n_curves
            << "  hump u=" << s.hump_u << " (dev " << dev_hump << ")"
            << "  zero u=" << s.zero_u << " (dev " << dev_zero << ")"
            << "  trough u=" << s.trough_u << " (dev " << dev_trough << ")  τ=" << tol);

    // The two agreeing invariants — the murmuration's defining oscillation.
    CHECK(dev_hump < tol);
    CHECK(dev_zero < tol);

    // The trough is the OPEN DEVIATION (fork verdict): asserted present, not hidden.
    // A future change that brought it within τ would (correctly) fail this and force the
    // finding to be re-examined — the miss is pinned as a fact.
    CHECK(dev_trough > tol);

    // Localization discriminant (pre-registered window u∈[0.7,0.9], same extractor):
    // windowed == global ⇒ the displacement is real, not a far-tail-argmin artifact.
    const double win_trough = windowed_trough_u(run.confirm, 0.7, 0.9);
    MESSAGE("localization: global trough u=" << s.trough_u
            << ", windowed[0.7,0.9] trough u=" << win_trough
            << " (equal ⇒ not a tail artifact)");
    CHECK(std::abs(win_trough - s.trough_u) < 0.03);   // both land in the same displaced bin

    // Convergence empiric: the trough deviation is FLAT (does not decay) across the
    // ladder — the recorded basis for the OPEN-DEVIATION verdict. Reported; and the
    // hump/zero stay within τ at every scale (assert).
    for (const SSScaleShape& sc : run.shapes) {
        const double dh = std::abs(sc.shape.hump_u - run.r2_hump);
        const double dz = std::abs(sc.shape.zero_u - run.r2_zero);
        const double dt = std::abs(sc.shape.trough_u - run.r2_trough);
        MESSAGE("  ladder X=" << sc.X << " |fam|=" << sc.n_curves
                << "  dev(hump)=" << dh << " dev(zero)=" << dz << " dev(trough)=" << dt);
        CHECK(dh < tol);
        CHECK(dz < tol);
    }

    REQUIRE(dev_hump < tol);
    REQUIRE(dev_zero < tol);
    REQUIRE(dev_trough > tol);   // the deviation is the finding
}

TEST_CASE("twin_ap_fast_vs_charsum_m4") {
    // Rule 2 over the M4 regime: the RUN's a_p (ap_fast, table) equals the frozen
    // referee (ap_charsum, modpow) — including primes far past M0's range (u=p/N ~ 1
    // for large-conductor curves). A few family curves × primes up to ~2·10⁵.
    const std::vector<HeightCurve> fam = height_family(500);   // a small, fast sample set
    const std::vector<i64> primes = {5, 101, 1009, 10007, 100003, 200003};
    int checked = 0, mism = 0;
    for (const HeightCurve& c : fam) {
        const i64 disc = 4 * c.A * c.A * c.A + 27 * c.B * c.B;
        const at::ell::Curve E{0, 0, 0, c.A, c.B};
        for (i64 p : primes) {
            if (disc % p == 0) continue;   // bad reduction: a_p domain excludes it
            const at::ell::QRTable qr = at::ell::build_qr_table(static_cast<u64>(p));
            const int fast = at::ell::ap_fast(E, qr);
            const int ref = at::ell::ap_charsum(E, static_cast<u64>(p));
            if (fast != ref) ++mism;
            ++checked;
        }
    }
    MESSAGE("twin ap_fast vs ap_charsum (M4 regime): " << checked << " (curve,prime) pairs, "
            << mism << " mismatches");
    CHECK(checked > 100);
    CHECK(mism == 0);
    REQUIRE(mism == 0);
}
