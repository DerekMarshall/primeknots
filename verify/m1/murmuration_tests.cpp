// Stage M1 — HLOP murmuration replication (m1-pinning). These read the pinned
// ecdata slices (gitignored); they SKIP cleanly (exit 77) when ecdata is absent,
// exactly as an oracle test does — the dataset is the referee here.
//   anchor_family_counts  — the four published family counts, REQUIREd.
//   theorem_scale_collapse — per-rank collapse in y=p/N; real passes, the antiphase
//                            null fails where there is power (r0,r1); r2 low-power (R2).
//   theorem_antiphase      — rank-parity relationship: f0~f1 antiphase, f0~f2 in phase.
#include "doctest/doctest.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <string>
#include <vector>

#include "ell/ecdata.h"
#include "harness.h"
#include "murm/murmuration.h"

#ifndef AT_ECDATA_DIR
#define AT_ECDATA_DIR "data/cremona"
#endif

using at::core::u64;
using namespace at::murm;

namespace {
constexpr std::size_t NP = 300;   // primes: enough power for the antiphase null (verified)

bool ecdata_present() {
    namespace fs = std::filesystem;
    if (!fs::is_directory(AT_ECDATA_DIR)) return false;
    for (const auto& e : fs::directory_iterator(AT_ECDATA_DIR))
        if (e.path().filename().string().rfind("allcurves.", 0) == 0) return true;
    return false;
}

double pearson(const std::vector<MurmPoint>& A, const std::vector<MurmPoint>& B) {
    const std::size_t n = std::min(A.size(), B.size());
    double ma = 0, mb = 0;
    for (std::size_t i = 0; i < n; ++i) { ma += A[i].avg; mb += B[i].avg; }
    ma /= n; mb /= n;
    double cov = 0, va = 0, vb = 0;
    for (std::size_t i = 0; i < n; ++i) {
        const double da = A[i].avg - ma, db = B[i].avg - mb;
        cov += da * db; va += da * da; vb += db * db;
    }
    return cov / std::sqrt(va * vb);
}
}  // namespace

#define SKIP_IF_NO_ECDATA()                                                        \
    do {                                                                           \
        if (!ecdata_present()) {                                                   \
            MESSAGE("[SKIP] ecdata absent at " AT_ECDATA_DIR                        \
                    " — fetch via oracle/fetch_ecdata.py (data referee unavailable)"); \
            at::verify::g_oracle_skipped = true;                                   \
            return;                                                                \
        }                                                                          \
    } while (0)

TEST_CASE("anchor_family_counts") {
    SKIP_IF_NO_ECDATA();
    const auto all = at::ell::load_ecdata_range(AT_ECDATA_DIR, 5000, 10000);
    const auto e0_75 = filter_family(all, 0, 7500, 10000);
    const auto e1_75 = filter_family(all, 1, 7500, 10000);
    const auto e0_50 = filter_family(all, 0, 5000, 10000);
    const auto e2_50 = filter_family(all, 2, 5000, 10000);
    MESSAGE("family counts: E0[7500,10000]=" << e0_75.size() << " E1=" << e1_75.size()
            << " E0[5000,10000]=" << e0_50.size() << " E2=" << e2_50.size()
            << " (HLOP Example 1: 4328/5194/8536/1380)");
    CHECK(e0_75.size() == 4328);
    CHECK(e1_75.size() == 5194);
    CHECK(e0_50.size() == 8536);
    CHECK(e2_50.size() == 1380);
    REQUIRE(e0_75.size() == 4328);
    REQUIRE(e1_75.size() == 5194);
    REQUIRE(e0_50.size() == 8536);
    REQUIRE(e2_50.size() == 1380);
}

TEST_CASE("theorem_scale_collapse") {
    SKIP_IF_NO_ECDATA();
    const auto all = at::ell::load_ecdata_range(AT_ECDATA_DIR, 2500, 10000);
    const auto a0 = murmuration_curve(all, 0, 2500, 5000, NP), b0 = murmuration_curve(all, 0, 5000, 10000, NP);
    const auto a1 = murmuration_curve(all, 1, 2500, 5000, NP), b1 = murmuration_curve(all, 1, 5000, 10000, NP);
    const auto a2 = murmuration_curve(all, 2, 2500, 5000, NP), b2 = murmuration_curve(all, 2, 5000, 10000, NP);
    const int G = 60;
    // real: same rank; null: antiphase (opposite-parity) rank on the B side (R1c).
    const auto c0 = scale_collapse(a0, b0, G), n0 = scale_collapse(a0, b1, G);
    const auto c1 = scale_collapse(a1, b1, G), n1 = scale_collapse(a1, b0, G);
    const auto c2 = scale_collapse(a2, b2, G), n2 = scale_collapse(a2, b1, G);

    MESSAGE("collapse r0: real ratio=" << c0.ratio << " (pass<3) | antiphase null=" << n0.ratio
            << " (fail>=3) | F=" << c0.floor_F);
    MESSAGE("collapse r1: real ratio=" << c1.ratio << " | antiphase null=" << n1.ratio
            << " | F=" << c1.floor_F);
    MESSAGE("collapse r2: real ratio=" << c2.ratio << " | antiphase null=" << n2.ratio
            << " | F=" << c2.floor_F << "  (LOW POWER: |E_2| small, null not required to fail — R2)");

    // Power cases (r0, r1): the collapse holds AND the null is rejected.
    CHECK(c0.ratio < 3.0);
    CHECK(n0.ratio >= 3.0);
    CHECK(c1.ratio < 3.0);
    CHECK(n1.ratio >= 3.0);
    // r2: consistent (real passes) but low power — the null is NOT required to fail.
    CHECK(c2.ratio < 3.0);
    REQUIRE(c0.ratio < 3.0);
    REQUIRE(n0.ratio >= 3.0);   // a collapse that couldn't reject the antiphase null measures nothing
    REQUIRE(c1.ratio < 3.0);
    REQUIRE(n1.ratio >= 3.0);
    REQUIRE(c2.ratio < 3.0);
}

TEST_CASE("theorem_antiphase") {
    SKIP_IF_NO_ECDATA();
    const auto all = at::ell::load_ecdata_range(AT_ECDATA_DIR, 5000, 10000);
    const auto f0 = murmuration_curve(all, 0, 7500, 10000, NP);
    const auto f1 = murmuration_curve(all, 1, 7500, 10000, NP);
    const auto g0 = murmuration_curve(all, 0, 5000, 10000, NP);
    const auto g2 = murmuration_curve(all, 2, 5000, 10000, NP);
    const double r01 = pearson(f0.points, f1.points);   // rank 0 vs 1 — opposite parity
    const double r02 = pearson(g0.points, g2.points);   // rank 0 vs 2 — same parity
    MESSAGE("rank-parity relationship: corr(f0,f1)=" << r01 << " (antiphase, <-0.5) | corr(f0,f2)="
            << r02 << " (in phase, >0.5)");
    CHECK(r01 < -0.5);   // even vs odd rank oscillate in antiphase
    CHECK(r02 > 0.5);    // even vs even (ranks 0,2) oscillate in phase — parity-consistent
    REQUIRE(r01 < -0.5);
    REQUIRE(r02 > 0.5);
}
