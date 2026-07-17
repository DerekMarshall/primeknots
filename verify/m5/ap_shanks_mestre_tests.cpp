// Stage M0b (Phase 2a) — Shanks–Mestre a_p seals.
//
//   anchor_shanks_mestre_pari  — the §6 PARI-adjudicated anchors (gp 2.17.4, recorded in
//     m0b-pinning.md §6): supersingular a_p=0, the non-cyclic [52,4] curve whose λ=52 is
//     AMBIGUOUS ({208,260}) and is resolved only by the twist (A2 — the R2 disambiguation
//     path), the ordinary j=0 pair, and the 227/229/233 threshold straddle (fast/fallback).
//   anchor_shanks_mestre_bad_reduction_refused — a bad prime (p | disc) is REFUSED, demonstrated
//     firing (M0b never point-counts a singular reduction; it never assumes a pre-filtered caller).
//   twin_shanks_mestre_vs_charsum — over good primes p > 229 and several curves, Shanks–Mestre
//     reproduces the frozen ell::ap_charsum EXACTLY (integer equality). The cross-ALGORITHM twin.
#include "doctest/doctest.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "harness.h"
#include "progress.h"               // at::verify::Progress — standing live-progress convention
#include "ell/ap.h"                 // ap_charsum (frozen referee)
#include "ell/ap_cache.h"
#include "ell/ap_shanks_mestre.h"
#include "ell/curve.h"
#include "murm/height_family.h"
#include "murm/ne_cache.h"

using at::core::i64;
using at::core::u64;
using at::ell::Curve;
using at::ell::ap_charsum;
using at::ell::ap_shanks_mestre;

#ifndef AT_M5_DATA_DIR
#define AT_M5_DATA_DIR "data/m5"
#endif

TEST_CASE("anchor_shanks_mestre_pari") {
    // A1 — supersingular j=0 (y²=x³+1), p=233 ≡ 2 mod 3: a_p = 0  [#E=234].  p>229 ⇒ SM path.
    CHECK(ap_shanks_mestre(Curve{0, 0, 0, 0, 1}, 233) == 0);

    // A2 — non-cyclic j=1728 (y²=x³−x), p=233: ellgroup [52,4], λ=52 has TWO Hasse multiples
    // {208,260}; the twist's λ=130 has the unique {260} ⇒ #E=208 ⇒ a_p = 26. Exercises the
    // Mestre twist disambiguation (the whole point of the R2 loop invariant).
    CHECK(ap_shanks_mestre(Curve{0, 0, 0, -1, 0}, 233) == 26);

    // A5 — ordinary j=0 (y²=x³+1), p=241 ≡ 1 mod 3: a_p = 14  [#E=228].
    CHECK(ap_shanks_mestre(Curve{0, 0, 0, 0, 1}, 241) == 14);

    // A4 — threshold straddle on y²=x³+x+1: 227,229 route to the charsum fallback (≤229),
    // 233 to the Shanks–Mestre fast path — all three must match the adjudicated values.
    CHECK(ap_shanks_mestre(Curve{0, 0, 0, 1, 1}, 227) == 0);
    CHECK(ap_shanks_mestre(Curve{0, 0, 0, 1, 1}, 229) == -2);
    CHECK(ap_shanks_mestre(Curve{0, 0, 0, 1, 1}, 233) == -3);
}

TEST_CASE("anchor_shanks_mestre_bad_reduction_refused") {
    // A3 — y²=x³+x+2 has 7 | (4A³+27B²) in the short model: bad reduction. REFUSED (throws),
    // demonstrated firing — regardless of p ≤ 229 (the disc check precedes the fallback).
    CHECK_THROWS_AS((void)ap_shanks_mestre(Curve{0, 0, 0, 1, 2}, 7), std::invalid_argument);
}

TEST_CASE("twin_shanks_mestre_vs_charsum") {
    // Small-range cross-ALGORITHM twin: Shanks–Mestre ≡ the frozen ap_charsum exactly, over
    // good primes p > 229 (the SM fast path) and several curves. Bad (curve,p) pairs (SM refuses)
    // are skipped — the twin is for good reduction. No cache, no 2^18 — Phase 2a scope.
    const std::vector<Curve> curves = {
        {0, 0, 0, -1, 0}, {0, 0, 0, 0, 1}, {0, 0, 0, 1, 1},
        {0, 0, 0, 2, -3}, {0, 0, 0, -4, 4}, {0, 0, 0, 5, 7}, {0, 0, 0, -7, 10},
    };
    // primes in (229, 1200]
    std::vector<u64> primes;
    for (u64 n = 230; n <= 1200; ++n) {
        bool pr = n > 1;
        for (u64 d = 2; d * d <= n; ++d) if (n % d == 0) { pr = false; break; }
        if (pr) primes.push_back(n);
    }
    int checked = 0, mismatches = 0;
    for (const Curve& E : curves) {
        for (u64 p : primes) {
            int sm;
            try {
                sm = ap_shanks_mestre(E, p);
            } catch (const std::exception&) {
                continue;                       // bad reduction at p — skip (twin is good-prime)
            }
            const int ref = ap_charsum(E, p);
            if (sm != ref) ++mismatches;
            ++checked;
        }
    }
    MESSAGE("Shanks-Mestre vs charsum: " << checked << " good (curve,p) pairs checked, "
            << mismatches << " mismatches");
    CHECK(mismatches == 0);
    CHECK(checked > 200);                        // the twin actually ran a broad range
}

TEST_CASE("twin_m0b_vs_charsum_x16") {
    // M0b full-cache twin at 2^16: Shanks-Mestre reproduces the checksummed 2^16 a_p reference
    // cache EXACTLY, every curve × every good prime. Contract (§7.1 + P1 + R1): header-grid
    // identity BEFORE any value compare; COMPLETE required (R1 refusal live); first mismatch
    // ABORTS with a full witness (a mismatch is a deliverable, not a retry); the p<=229 fallback
    // band is COMPARED (fallback correctness in scope). Logs wall time + evals/s + speedup.
    using at::ell::ApCacheHeader;
    using at::ell::read_ap_cache;
    using at::murm::NeCacheHeader;
    using at::murm::NeRow;
    using at::murm::read_ne_cache;
    using at::murm::naive_height;

    const std::string cache_path = std::string(AT_M5_DATA_DIR) + "/ap_cache_x65536.bin";
    {
        std::ifstream probe(cache_path, std::ios::binary);
        if (!probe.good()) {
            MESSAGE("[SKIP] 2^16 a_p cache absent (local/gitignored) — run `at ap-cache --X 65536` first.");
            at::verify::g_oracle_skipped = true;
            return;
        }
    }

    // R1: COMPLETE required — read_ap_cache (default) REFUSES a partial checkpoint (throws).
    ApCacheHeader ch;
    std::vector<int16_t> cv = read_ap_cache(cache_path, ch);
    CHECK(ch.complete == true);

    NeCacheHeader nh;
    std::vector<NeRow> allrows = read_ne_cache(std::string(AT_M5_DATA_DIR) + "/ne_cache_x65536.txt", nh);
    const i64 X = 65536;
    std::vector<NeRow> rows;
    for (const NeRow& r : allrows) if (naive_height(r.A, r.B) <= X) rows.push_back(r);
    const std::size_t C = rows.size();
    REQUIRE(C > 0);

    // P1: header-grid identity — reconstruct the generator's descriptor, REQUIRE equality BEFORE
    // any value compare (an intersecting-but-different grid passing green = silent under-coverage).
    const std::string expected_desc =
        "family=height(X=65536,fam=" + std::to_string(C) + ")"
        "|order=conductor-asc(N,idx)|prime_rule=3<p<=N,p_ndiv_(4A^3+27B^2)"
        "|ne=data/m5/ne_cache_x65536.txt,ne_gen=" + nh.generator_hash +
        ",ne_X=" + std::to_string(nh.X) + ",ne_count=" + std::to_string(nh.count) +
        "|algo=charsum_referee(ap_fast;spot=ap_charsum)";
    REQUIRE(ch.curve_set == expected_desc);

    // reconstruct the generator's grid: conductor-asc order (N, then ne-row idx); good primes.
    std::vector<i64> disc(C);
    i64 maxN = 0;
    for (std::size_t c = 0; c < C; ++c) {
        disc[c] = 4 * rows[c].A * rows[c].A * rows[c].A + 27 * rows[c].B * rows[c].B;
        if (rows[c].N > maxN) maxN = rows[c].N;
    }
    std::vector<std::size_t> order(C);
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b) {
        if (rows[a].N != rows[b].N) return rows[a].N < rows[b].N;
        return a < b;
    });
    std::vector<i64> gp;
    {
        std::vector<char> sv(static_cast<std::size_t>(maxN) + 1, 1);
        for (i64 i = 2; i <= maxN; ++i)
            if (sv[i]) { if (i > 3) gp.push_back(i); for (i64 j = i * i; j <= maxN; j += i) sv[j] = 0; }
    }

    // walk + compare exact (plain loop; no per-eval doctest macros over ~112M evals).
    std::size_t off = 0;
    long long checked = 0, band_le229 = 0;
    bool mismatch = false, overrun = false, threw = false;
    at::verify::Progress prog("twin_m0b_vs_charsum_x16", static_cast<long long>(C), 256);
    for (std::size_t i = 0; i < C && !mismatch && !overrun && !threw; ++i) {
        const std::size_t c = order[i];
        const Curve E{0, 0, 0, rows[c].A, rows[c].B};
        const i64 d = disc[c], N = rows[c].N;
        for (i64 p : gp) {
            if (p > N) break;
            if (d % p == 0) continue;
            if (off >= cv.size()) { overrun = true; break; }
            const int cache_ap = static_cast<int>(cv[off++]);
            int side = -2, m0b_ap = 0;
            try {
                m0b_ap = ap_shanks_mestre(E, static_cast<u64>(p), &side);
            } catch (const std::exception& e) {
                MESSAGE("x16 TWIN THREW — curve A=" << rows[c].A << " B=" << rows[c].B
                        << " N=" << N << " p=" << p << " : " << e.what());
                threw = true; break;
            }
            if (m0b_ap != cache_ap) {
                MESSAGE("x16 TWIN MISMATCH — curve A=" << rows[c].A << " B=" << rows[c].B
                        << " N=" << N << " p=" << p << " | cache(charsum)=" << cache_ap
                        << " M0b(shanks-mestre)=" << m0b_ap << " | resolved_by="
                        << (side == -1 ? "charsum-fallback" : side == 0 ? "E-side"
                            : side == 1 ? "twist-side" : "?"));
                mismatch = true; break;
            }
            if (p <= static_cast<i64>(at::ell::kMestreThreshold)) ++band_le229;
            ++checked;
        }
        prog.tick(static_cast<long long>(i) + 1, checked);   // curves done / rate / ETA (+ a_p detail)
    }
    const double secs = prog.elapsed();

    CHECK(threw == false);
    CHECK(overrun == false);
    CHECK(off == cv.size());          // consumed the whole cache — grid identity end-to-end
    CHECK(band_le229 > 0);            // p<=229 charsum-fallback band exercised + compared
    CHECK(mismatch == false);        // EXACT: every a_p matched
    MESSAGE("x16 twin: " << checked << " (curve,prime) a_p compared EXACT (" << band_le229
            << " in the p<=229 charsum-fallback band); " << secs << "s, "
            << (secs > 0 ? static_cast<double>(checked) / secs : 0.0) << " evals/s");
    MESSAGE("M0b SPEEDUP: x16 twin wall " << secs << "s vs charsum reference-gen 36860s (same grid) -> "
            << (secs > 0 ? 36860.0 / secs : 0.0) << "x");
}

TEST_CASE("twin_m0b_vs_charsum_x17") {
    // M0b full-cache twin at 2^17 — Shanks-Mestre reproduces the checksummed 2^17 a_p reference
    // cache EXACTLY (385,792,535 values), same §7.1+P1+R1 contract as x16. The data-note gate:
    // must be green before the note cites M0b. Cross-ALGORITHM (SM vs charsum); the cache and M0b
    // both run on FreeBSD (a_p is a platform-independent integer, so "same platform" is moot).
    using at::ell::ApCacheHeader;
    using at::ell::read_ap_cache;
    using at::murm::NeCacheHeader;
    using at::murm::NeRow;
    using at::murm::read_ne_cache;
    using at::murm::naive_height;

    const std::string cache_path = std::string(AT_M5_DATA_DIR) + "/ap_cache_x131072.bin";
    {
        std::ifstream probe(cache_path, std::ios::binary);
        if (!probe.good()) {
            MESSAGE("[SKIP] 2^17 a_p cache absent (local/gitignored) — run `at ap-cache --X 131072` first.");
            at::verify::g_oracle_skipped = true;
            return;
        }
    }

    ApCacheHeader ch;
    std::vector<int16_t> cv = read_ap_cache(cache_path, ch);   // R1: COMPLETE required (refuses partial)
    CHECK(ch.complete == true);

    NeCacheHeader nh;
    std::vector<NeRow> allrows = read_ne_cache(std::string(AT_M5_DATA_DIR) + "/ne_cache_x131072.txt", nh);
    const i64 X = 131072;
    std::vector<NeRow> rows;
    for (const NeRow& r : allrows) if (naive_height(r.A, r.B) <= X) rows.push_back(r);
    const std::size_t C = rows.size();
    REQUIRE(C > 0);

    // P1: header-grid identity (the generator was run with --cache data/m5/ne_cache_x131072.txt).
    const std::string expected_desc =
        "family=height(X=131072,fam=" + std::to_string(C) + ")"
        "|order=conductor-asc(N,idx)|prime_rule=3<p<=N,p_ndiv_(4A^3+27B^2)"
        "|ne=data/m5/ne_cache_x131072.txt,ne_gen=" + nh.generator_hash +
        ",ne_X=" + std::to_string(nh.X) + ",ne_count=" + std::to_string(nh.count) +
        "|algo=charsum_referee(ap_fast;spot=ap_charsum)";
    REQUIRE(ch.curve_set == expected_desc);

    std::vector<i64> disc(C);
    i64 maxN = 0;
    for (std::size_t c = 0; c < C; ++c) {
        disc[c] = 4 * rows[c].A * rows[c].A * rows[c].A + 27 * rows[c].B * rows[c].B;
        if (rows[c].N > maxN) maxN = rows[c].N;
    }
    std::vector<std::size_t> order(C);
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b) {
        if (rows[a].N != rows[b].N) return rows[a].N < rows[b].N;
        return a < b;
    });
    std::vector<i64> gp;
    {
        std::vector<char> sv(static_cast<std::size_t>(maxN) + 1, 1);
        for (i64 i = 2; i <= maxN; ++i)
            if (sv[i]) { if (i > 3) gp.push_back(i); for (i64 j = i * i; j <= maxN; j += i) sv[j] = 0; }
    }

    std::size_t off = 0;
    long long checked = 0, band_le229 = 0;
    bool mismatch = false, overrun = false, threw = false;
    at::verify::Progress prog("twin_m0b_vs_charsum_x17", static_cast<long long>(C), 256);
    for (std::size_t i = 0; i < C && !mismatch && !overrun && !threw; ++i) {
        const std::size_t c = order[i];
        const Curve E{0, 0, 0, rows[c].A, rows[c].B};
        const i64 d = disc[c], N = rows[c].N;
        for (i64 p : gp) {
            if (p > N) break;
            if (d % p == 0) continue;
            if (off >= cv.size()) { overrun = true; break; }
            const int cache_ap = static_cast<int>(cv[off++]);
            int side = -2, m0b_ap = 0;
            try {
                m0b_ap = ap_shanks_mestre(E, static_cast<u64>(p), &side);
            } catch (const std::exception& e) {
                MESSAGE("x17 TWIN THREW — curve A=" << rows[c].A << " B=" << rows[c].B
                        << " N=" << N << " p=" << p << " : " << e.what());
                threw = true; break;
            }
            if (m0b_ap != cache_ap) {
                MESSAGE("x17 TWIN MISMATCH — curve A=" << rows[c].A << " B=" << rows[c].B
                        << " N=" << N << " p=" << p << " | cache(charsum)=" << cache_ap
                        << " M0b(shanks-mestre)=" << m0b_ap << " | resolved_by="
                        << (side == -1 ? "charsum-fallback" : side == 0 ? "E-side"
                            : side == 1 ? "twist-side" : "?"));
                mismatch = true; break;
            }
            if (p <= static_cast<i64>(at::ell::kMestreThreshold)) ++band_le229;
            ++checked;
        }
        prog.tick(static_cast<long long>(i) + 1, checked);
    }
    const double secs = prog.elapsed();

    CHECK(threw == false);
    CHECK(overrun == false);
    CHECK(off == cv.size());
    CHECK(band_le229 > 0);
    CHECK(mismatch == false);
    MESSAGE("x17 twin: " << checked << " (curve,prime) a_p compared EXACT (" << band_le229
            << " in the p<=229 charsum-fallback band); " << secs << "s, "
            << (secs > 0 ? static_cast<double>(checked) / secs : 0.0) << " evals/s");
    MESSAGE("M0b SPEEDUP: x17 twin wall " << secs << "s vs charsum reference-gen 52711s / 48-thread "
            << "(same grid) -> " << (secs > 0 ? 52711.0 / secs : 0.0) << "x wall");
}
