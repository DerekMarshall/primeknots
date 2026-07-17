// Stage M0b — PRODUCTION-CAPABLE gate at 2¹⁸ (m0b-pinning §7, concrete spec pinned 2026-07-17).
// Two tests, both over the SAME deterministic tail-weighted sample of the committed 2¹⁸ family:
//   twin_m0b_bruteforce_x18_tailweighted — M0b (ap_shanks_mestre) vs the O(p) referee (ap_fast),
//     exact integers, over every good prime of 250 stratified tail-weighted curves; an ap_charsum
//     (frozen modpow referee) three-way spot on small primes ties ap_fast to the frozen referee.
//   oracle_ellap_m0b_spot — ~200 tail (curve,prime) pairs vs PARI `ellap` (independent oracle).
// Both green ⇒ PRODUCTION-CAPABLE (mark the ladder with commit hashes; HARD GATE before 2¹⁸).
#include "doctest/doctest.h"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <limits>
#include <mutex>
#include <numeric>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "harness.h"
#include "progress.h"
#include "ell/ap.h"                 // ap_charsum (frozen referee)
#include "ell/ap_fast.h"            // ap_fast + build_qr_table (the O(p) referee)
#include "ell/ap_shanks_mestre.h"
#include "ell/curve.h"
#include "murm/height_family.h"
#include "murm/ne_cache.h"
#include "oracle/pari.h"

using at::core::i64;
using at::core::u64;
using at::ell::Curve;

#ifndef AT_M5_DATA_DIR
#define AT_M5_DATA_DIR "data/m5"
#endif

namespace {

// The pinned deterministic tail-weighted sample (m0b-pinning §7, 2026-07-17): conductor-quartile
// strata of the committed 2¹⁸ family; 30/50/70/100 curves from Q1/Q2/Q3/Q4; within a stratum take
// positions ⌊i·|stratum|/count⌋ in conductor-ascending order (ties by ne-row index). No RNG.
struct Sample {
    std::vector<at::murm::NeRow> rows;   // 250 selected curves
    std::vector<int> stratum;            // 0..3, parallel to rows
    bool loaded = false;
};

Sample select_x18_sample(const std::string& ne_path) {
    Sample s;
    at::murm::NeCacheHeader h;
    std::vector<at::murm::NeRow> all;
    try {
        all = at::murm::read_ne_cache(ne_path, h);
    } catch (const std::exception&) { return s; }
    if (all.size() < 4) return s;

    std::vector<std::size_t> ord(all.size());
    std::iota(ord.begin(), ord.end(), std::size_t{0});
    std::sort(ord.begin(), ord.end(), [&](std::size_t a, std::size_t b) {
        if (all[a].N != all[b].N) return all[a].N < all[b].N;   // conductor-ascending
        return a < b;                                            // ties by ne-row index
    });

    const std::size_t n = all.size();
    const int counts[4] = {30, 50, 70, 100};
    for (int q = 0; q < 4; ++q) {
        const std::size_t base = (n * static_cast<std::size_t>(q)) / 4;      // quartile by position
        const std::size_t sz = (n * static_cast<std::size_t>(q + 1)) / 4 - base;
        for (int i = 0; i < counts[q]; ++i) {
            const std::size_t pos = base + (static_cast<std::size_t>(i) * sz) / counts[q];
            s.rows.push_back(all[ord[pos]]);
            s.stratum.push_back(q);
        }
    }
    s.loaded = true;
    return s;
}

}  // namespace

TEST_CASE("twin_m0b_bruteforce_x18_tailweighted") {
    using at::ell::ap_charsum;
    using at::ell::ap_fast;
    using at::ell::ap_shanks_mestre;
    using at::ell::build_qr_table;

    const Sample s = select_x18_sample(std::string(AT_M5_DATA_DIR) + "/ne_cache_x262144.txt");
    if (!s.loaded) {
        MESSAGE("[SKIP] ne_cache_x262144.txt absent — cannot run the x18 sample twin.");
        at::verify::g_oracle_skipped = true;
        return;
    }
    const std::size_t C = s.rows.size();
    REQUIRE(C == 250);

    // selection fingerprint (reproducibility record; the pinned rule reproduces the exact list)
    long long fp = 0, cnt_q[4] = {0, 0, 0, 0};
    i64 maxN = 0;
    for (std::size_t c = 0; c < C; ++c) {
        fp = fp * 1000003 + s.rows[c].A * 131 + s.rows[c].B * 17 + s.rows[c].N;
        cnt_q[s.stratum[c]]++;
        if (s.rows[c].N > maxN) maxN = s.rows[c].N;
    }
    MESSAGE("x18 sample: 250 curves (Q1/Q2/Q3/Q4 = " << cnt_q[0] << "/" << cnt_q[1] << "/"
            << cnt_q[2] << "/" << cnt_q[3] << "), maxN=" << maxN << ", selection fp=" << fp);

    std::vector<i64> disc(C);
    for (std::size_t c = 0; c < C; ++c)
        disc[c] = 4 * s.rows[c].A * s.rows[c].A * s.rows[c].A + 27 * s.rows[c].B * s.rows[c].B;

    // primes up to maxN
    std::vector<i64> gp;
    {
        std::vector<char> sv(static_cast<std::size_t>(maxN) + 1, 1);
        for (i64 i = 2; i <= maxN; ++i)
            if (sv[i]) { if (i > 3) gp.push_back(i); for (i64 j = i * i; j <= maxN; j += i) sv[j] = 0; }
    }

    const i64 kSpotCap = 3000;   // ap_charsum three-way tie on p ≤ this (cheap; ties the frozen referee)
    unsigned hw = std::thread::hardware_concurrency();
    const int T = hw ? static_cast<int>(hw) : 4;
    std::atomic<long long> checked{0}, spot{0}, primes_done{0};
    std::atomic<bool> stop{false}, bad{false};
    std::mutex wmx;
    std::string witness;
    long long wit_key = std::numeric_limits<long long>::max();

    auto worker = [&](int t) {
        long long lchecked = 0, lspot = 0;
        for (std::size_t pi = static_cast<std::size_t>(t); pi < gp.size() && !stop.load(std::memory_order_relaxed);
             pi += static_cast<std::size_t>(T)) {
            const i64 p = gp[pi];
            const at::ell::QRTable qr = build_qr_table(static_cast<u64>(p));
            for (std::size_t c = 0; c < C; ++c) {
                if (s.rows[c].N < p) continue;
                if (disc[c] % p == 0) continue;
                const Curve E{0, 0, 0, s.rows[c].A, s.rows[c].B};
                const int ref = ap_fast(E, qr);                 // O(p) referee
                int side = -2;
                const int m0b = ap_shanks_mestre(E, static_cast<u64>(p), &side);
                bool ok = (m0b == ref);
                if (ok && p <= kSpotCap) {                       // three-way: tie the frozen referee
                    ok = (ap_charsum(E, static_cast<u64>(p)) == m0b);
                    ++lspot;
                }
                if (!ok) {
                    std::lock_guard<std::mutex> lg(wmx);
                    const long long key = static_cast<long long>(c) * 10000000LL + p;
                    if (key < wit_key) {
                        wit_key = key;
                        std::ostringstream ss;
                        ss << "x18 TWIN MISMATCH — curve A=" << s.rows[c].A << " B=" << s.rows[c].B
                           << " N=" << s.rows[c].N << " p=" << p << " | ap_fast=" << ref
                           << " ap_charsum=" << ap_charsum(E, static_cast<u64>(p)) << " M0b=" << m0b
                           << " resolved_by=" << (side == -1 ? "charsum-fallback" : side == 0 ? "E-side"
                               : side == 1 ? "twist-side" : "?");
                        witness = ss.str();
                    }
                    bad.store(true); stop.store(true); break;
                }
                ++lchecked;
            }
            primes_done.fetch_add(1, std::memory_order_relaxed);
        }
        checked.fetch_add(lchecked, std::memory_order_relaxed);
        spot.fetch_add(lspot, std::memory_order_relaxed);
    };

    at::verify::Progress prog("twin_m0b_bruteforce_x18_tailweighted", static_cast<long long>(gp.size()), 20000);
    std::vector<std::thread> pool;
    for (int t = 0; t < T; ++t) pool.emplace_back(worker, t);
    for (;;) {
        const long long dn = primes_done.load(std::memory_order_relaxed);
        prog.tick(dn, checked.load(std::memory_order_relaxed));
        if (dn >= static_cast<long long>(gp.size()) || stop.load(std::memory_order_relaxed)) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    for (std::thread& th : pool) th.join();
    const double secs = prog.elapsed();

    if (bad.load()) MESSAGE(witness);
    MESSAGE("x18 sample twin: " << checked.load() << " (curve,prime) a_p M0b==ap_fast EXACT ("
            << spot.load() << " also ==ap_charsum, p<=" << kSpotCap << " frozen-referee tie); "
            << secs << "s on " << T << " threads");
    CHECK(bad.load() == false);
    CHECK(checked.load() > 1000000);       // the sample genuinely ran a large grid
    CHECK(spot.load() > 0);                // the frozen-referee tie fired
}

TEST_CASE("oracle_ellap_m0b_spot") {
    using at::ell::ap_shanks_mestre;

    std::optional<std::string> gp = oracle::find_gp();
    if (!gp) {
        MESSAGE("[SKIP] oracle_ellap_m0b_spot: PARI/GP `gp` not found on PATH — oracle unavailable.");
        at::verify::g_oracle_skipped = true;
        return;
    }
    const Sample s = select_x18_sample(std::string(AT_M5_DATA_DIR) + "/ne_cache_x262144.txt");
    if (!s.loaded) {
        MESSAGE("[SKIP] ne_cache_x262144.txt absent — cannot draw the oracle spot sample.");
        at::verify::g_oracle_skipped = true;
        return;
    }
    MESSAGE("oracle: using gp at " << *gp);

    // Tail pairs: the largest primes of the highest-conductor sampled curves (Q3/Q4) — where M0b's
    // large-p point-counting path is exercised and an INDEPENDENT oracle (PARI ellap) matters most.
    i64 maxN = 0;
    std::vector<std::size_t> tail;   // indices of Q3/Q4 sampled curves
    for (std::size_t c = 0; c < s.rows.size(); ++c) {
        if (s.stratum[c] >= 2) tail.push_back(c);
        if (s.rows[c].N > maxN) maxN = s.rows[c].N;
    }
    std::vector<char> sv(static_cast<std::size_t>(maxN) + 1, 1);
    for (i64 i = 2; i <= maxN; ++i)
        if (sv[i]) for (i64 j = i * i; j <= maxN; j += i) sv[j] = 0;

    struct Pair { i64 A, B, N, p; int m0b; };
    std::vector<Pair> pairs;
    const int per_curve = 12;                       // ~200 pairs across the top tail curves
    for (std::size_t k = 0; k < tail.size() && static_cast<int>(pairs.size()) < 200; ++k) {
        const std::size_t c = tail[tail.size() - 1 - k];   // highest conductor first
        const i64 A = s.rows[c].A, B = s.rows[c].B, N = s.rows[c].N;
        const i64 dsc = 4 * A * A * A + 27 * B * B;
        int taken = 0;
        for (i64 p = std::min<i64>(N, maxN); p > 3 && taken < per_curve; --p) {
            if (!sv[p]) continue;
            if (dsc % p == 0) continue;
            const Curve E{0, 0, 0, A, B};
            pairs.push_back({A, B, N, p, ap_shanks_mestre(E, static_cast<u64>(p))});
            ++taken;
        }
    }
    REQUIRE(pairs.size() >= 150);

    std::ostringstream script;
    for (const Pair& pr : pairs)
        script << "print(ellap(ellinit([0,0,0," << pr.A << "," << pr.B << "])," << pr.p << "))\n";
    const std::string out = oracle::run_gp(*gp, script.str());

    std::istringstream lines(out);
    std::string line;
    long long matched = 0;
    bool bad = false;
    for (const Pair& pr : pairs) {
        REQUIRE(std::getline(lines, line));
        const int oracle_ap = std::stoi(line);
        if (oracle_ap != pr.m0b) {
            MESSAGE("oracle MISMATCH — A=" << pr.A << " B=" << pr.B << " N=" << pr.N << " p=" << pr.p
                    << " | PARI ellap=" << oracle_ap << " M0b=" << pr.m0b);
            bad = true;
            break;
        }
        ++matched;
    }
    MESSAGE("oracle_ellap_m0b_spot: " << matched << " tail (curve,prime) M0b a_p matched PARI ellap "
            << "(p up to " << pairs.front().p << ")");
    CHECK(bad == false);
    CHECK(matched == static_cast<long long>(pairs.size()));
}
