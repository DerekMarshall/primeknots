// Stage M5 (M0b Phase 2b) — reference a_p cache seals.
//
//   twin_ap_cache_refuses_partial [R1] — a PARTIAL checkpoint (complete=false) is REFUSED by
//     the default reader; only the generator's resume (allow_incomplete=true) reads it, and a
//     COMPLETE cache round-trips. Demonstrated firing (an 11th-hour crash must never leave a
//     plausible cache that a twin passes against < full coverage).
//   prereg_ap_cache_reproduces_shape [R2c + R3] — aggregating the reference cache reproduces
//     ss_empirical's shape AND per-bin counts EXACTLY (same a_p, same grid). This is the
//     end-to-end seal (R2c) and verifies the cache grid against the CONSUMER, not a re-derived
//     rule (R3). SKIPs cleanly if the (local, gitignored) cache is absent — pointed at the
//     scratch cache via AT_AP_CACHE* now, at the 2¹⁶ cache when it lands.
#include "doctest/doctest.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "harness.h"
#include "ell/ap_cache.h"
#include "murm/height_family.h"
#include "murm/ne_cache.h"
#include "murm/ss_empirical.h"

using at::core::i64;
using namespace at::murm;

#ifndef AT_M5_DATA_DIR
#define AT_M5_DATA_DIR "data/m5"
#endif

TEST_CASE("twin_ap_cache_refuses_partial") {
    using namespace at::ell;
    const std::string path = "ap_cache_partial_refusal.tmp";
    const std::vector<int16_t> vals = {2, -1, 0, 3, -2};

    ApCacheHeader h;
    h.generator_hash = ap_generator_hash();     // valid hash: ONLY the partial-state triggers refusal
    h.curve_set = "test|partial-refusal";
    h.prime_bound = 100;
    h.complete = false;
    h.n_done = 42;
    write_ap_cache(path, h, vals);

    // Default read (allow_incomplete=false) REFUSES a PARTIAL cache, naming the state.
    bool refused = false;
    try {
        ApCacheHeader r;
        (void)read_ap_cache(path, r);
    } catch (const std::exception& e) {
        refused = true;
        MESSAGE("R1 refusal fired: " << std::string(e.what()));
    }
    CHECK(refused);

    // The generator's resume path (allow_incomplete=true) DOES read it.
    ApCacheHeader r2;
    const std::vector<int16_t> got = read_ap_cache(path, r2, /*allow_incomplete=*/true);
    CHECK(r2.complete == false);
    CHECK(r2.n_done == 42u);
    const bool partial_vals_ok = (got == vals);
    CHECK(partial_vals_ok);

    // A COMPLETE cache round-trips through the default reader.
    h.complete = true;
    h.n_done = 5;
    write_ap_cache(path, h, vals);
    ApCacheHeader r3;
    const std::vector<int16_t> got3 = read_ap_cache(path, r3);
    CHECK(r3.complete == true);
    const bool complete_vals_ok = (got3 == vals);
    CHECK(complete_vals_ok);

    std::remove(path.c_str());
}

TEST_CASE("prereg_ap_cache_reproduces_shape") {
    using namespace at::ell;
    const char* cpath = std::getenv("AT_AP_CACHE");
    const char* nepath = std::getenv("AT_AP_CACHE_NE");
    const char* xenv = std::getenv("AT_AP_CACHE_X");
    const std::string cache_path =
        cpath ? cpath : (std::string(AT_M5_DATA_DIR) + "/ap_cache_x65536.bin");
    const std::string ne_path =
        nepath ? nepath : (std::string(AT_M5_DATA_DIR) + "/ne_cache_x65536.txt");
    const i64 X = xenv ? static_cast<i64>(std::strtoll(xenv, nullptr, 10)) : 65536;

    {
        std::ifstream probe(cache_path, std::ios::binary);
        if (!probe.good()) {
            MESSAGE("[SKIP] a_p cache " << cache_path
                    << " absent (local/gitignored) — reproduce-shape seal runs when it lands.");
            at::verify::g_oracle_skipped = true;
            return;
        }
    }

    ApCacheHeader ch;
    std::vector<int16_t> cv;
    try {
        cv = read_ap_cache(cache_path, ch);          // COMPLETE required (R1)
    } catch (const std::exception& e) {
        MESSAGE("[SKIP] a_p cache unreadable/partial: " << e.what());
        at::verify::g_oracle_skipped = true;
        return;
    }

    NeCacheHeader nh;
    std::vector<NeRow> allrows = read_ne_cache(ne_path, nh);
    std::vector<NeRow> rows;
    for (const NeRow& r : allrows)
        if (naive_height(r.A, r.B) <= X) rows.push_back(r);
    const std::size_t C = rows.size();
    REQUIRE(C > 0);

    // Reconstruct the generator's CONDUCTOR-ASC storage order + good-prime iteration, and
    // aggregate the cache's a_p exactly as ss_empirical bins them (u=p/N, prefactor).
    std::vector<std::size_t> order(C);
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b) {
        if (rows[a].N != rows[b].N) return rows[a].N < rows[b].N;
        return a < b;
    });
    i64 maxN = 0;
    for (const NeRow& r : rows) if (r.N > maxN) maxN = r.N;
    std::vector<i64> gp;
    {
        std::vector<char> sieve(static_cast<std::size_t>(maxN) + 1, 1);
        for (i64 i = 2; i <= maxN; ++i)
            if (sieve[i]) { if (i > 3) gp.push_back(i); for (i64 j = i * i; j <= maxN; j += i) sieve[j] = 0; }
    }
    auto disc_of = [&](const NeRow& r) { return 4 * r.A * r.A * r.A + 27 * r.B * r.B; };

    const double du = 0.025;
    const int NB = static_cast<int>(std::lround(1.0 / du));
    SSPartials P;
    P.du = du; P.NB = NB;
    P.A.resize(C); P.B.resize(C); P.N.resize(C);
    P.num.assign(C, std::vector<double>(NB, 0.0));
    P.cnt.assign(C, std::vector<i64>(NB, 0));
    for (std::size_t c = 0; c < C; ++c) { P.A[c] = rows[c].A; P.B[c] = rows[c].B; P.N[c] = rows[c].N; }

    std::size_t off = 0;
    bool layout_ok = true;
    for (std::size_t i = 0; i < C && layout_ok; ++i) {
        const std::size_t c = order[i];
        const i64 d = disc_of(rows[c]);
        const double lnN_over_N = std::log(static_cast<double>(rows[c].N)) / static_cast<double>(rows[c].N);
        for (i64 p : gp) {
            if (p > rows[c].N) break;
            if (d % p == 0) continue;                    // bad prime — same skip as ss_empirical (R3)
            if (off >= cv.size()) { layout_ok = false; break; }
            const int a = static_cast<int>(cv[off++]);
            const double u = static_cast<double>(p) / static_cast<double>(rows[c].N);
            const int b = static_cast<int>(u / du);
            if (b >= 0 && b < NB) {
                const double u_mid = (b + 0.5) * du;
                P.num[c][b] += (u_mid * lnN_over_N) * static_cast<double>(rows[c].eps) * static_cast<double>(a);
                P.cnt[c][b] += 1;
            }
        }
    }
    CHECK(layout_ok);
    const bool consumed_all = (off == cv.size());        // grid size == cache size, exactly
    CHECK(consumed_all);

    // The CONSUMER's own computation is the ground truth (R3: verify against the consumer,
    // not a re-derived rule). Same a_p ⇒ identical counts and shape (float order differs only
    // in the last ULP of the interpolated zero).
    const SSEmpirical ref = ss_empirical(rows, static_cast<double>(X), du, 4);
    const SSEmpirical got = ss_aggregate(P, static_cast<double>(X));

    CHECK(got.n_curves == ref.n_curves);
    bool counts_match = true;
    for (int b = 0; b < NB; ++b) if (got.count[b] != ref.count[b]) counts_match = false;
    CHECK(counts_match);                                 // R3: per-bin grid counts identical
    CHECK(std::abs(got.shape.hump_u - ref.shape.hump_u) < 1e-9);
    CHECK(std::abs(got.shape.zero_u - ref.shape.zero_u) < 1e-9);
    CHECK(std::abs(got.shape.trough_u - ref.shape.trough_u) < 1e-9);   // R2c: reproduce-shape seal
    MESSAGE("ap_cache reproduces ss_empirical @X=" << X << ": hump=" << got.shape.hump_u
            << " zero=" << got.shape.zero_u << " trough=" << got.shape.trough_u
            << "  (counts match; " << off << " a_p aggregated)");
}
