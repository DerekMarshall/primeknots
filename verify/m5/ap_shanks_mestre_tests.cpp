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
//   twin_m0b_vs_charsum_x16 / _x17 — the full-cache twins (parallel; see run_m0b_cache_twin).
#include "doctest/doctest.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <limits>
#include <mutex>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
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

namespace {

// Parallel M0b full-cache twin (docs/notes/m0b-pinning.md §7). Shanks–Mestre recomputes every
// (curve,prime) a_p and compares EXACT to the checksummed reference cache. The generator built
// the cache prime-major in conductor-ascending curve order; we recover each curve's cache slice
// with a prefix-sum of per-curve valid-prime counts, so the per-curve comparisons are INDEPENDENT
// and run across all cores — `ap_shanks_mestre` is a pure stateless function (derandomized
// hash(A,B,p) sampler, no shared RNG), and cv/rows/gp are read-only. There is NO correctness
// reason for a single thread; the generator and the ss-run were already threaded, this closes the
// gap (single-core the x17 twin was ~3 h on the 48-core box; parallel it is ~minutes).
//
// THREADING DISCIPLINE: workers touch NO doctest macros (doctest's context is thread-local and
// not thread-safe). Workers only read shared const data and write plain atomics / a mutex-guarded
// witness string. Every CHECK/MESSAGE runs on the MAIN thread after join. First mismatch/throw by
// CANONICAL ORDER (position in the conductor-asc grid) wins the witness — a mismatch is a
// deliverable, reported with the full witness, not retried.
void run_m0b_cache_twin(const std::string& label, i64 X, const std::string& cache_file,
                        const std::string& ne_file, const std::string& ne_desc,
                        double baseline_secs, const std::string& baseline_note) {
    using at::ell::ApCacheHeader;
    using at::ell::read_ap_cache;
    using at::murm::NeCacheHeader;
    using at::murm::NeRow;
    using at::murm::read_ne_cache;
    using at::murm::naive_height;

    const std::string cache_path = std::string(AT_M5_DATA_DIR) + cache_file;
    {
        std::ifstream probe(cache_path, std::ios::binary);
        if (!probe.good()) {
            MESSAGE("[SKIP] " << X << " a_p cache absent (local/gitignored) — run `at ap-cache` first.");
            at::verify::g_oracle_skipped = true;
            return;
        }
    }

    ApCacheHeader ch;
    std::vector<int16_t> cv = read_ap_cache(cache_path, ch);   // R1: COMPLETE required (refuses partial)
    CHECK(ch.complete == true);

    NeCacheHeader nh;
    std::vector<NeRow> allrows = read_ne_cache(std::string(AT_M5_DATA_DIR) + ne_file, nh);
    std::vector<NeRow> rows;
    for (const NeRow& r : allrows) if (naive_height(r.A, r.B) <= X) rows.push_back(r);
    const std::size_t C = rows.size();
    REQUIRE(C > 0);

    // P1: header-grid identity — reconstruct the generator's descriptor, REQUIRE equality BEFORE
    // any value compare (an intersecting-but-different grid passing green = silent under-coverage).
    const std::string expected_desc =
        "family=height(X=" + std::to_string(X) + ",fam=" + std::to_string(C) + ")"
        "|order=conductor-asc(N,idx)|prime_rule=3<p<=N,p_ndiv_(4A^3+27B^2)"
        "|ne=" + ne_desc + ",ne_gen=" + nh.generator_hash +
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

    // Per-curve valid-prime counts (good primes p ≤ N, p ∤ disc) → prefix-sum base offsets into
    // cv, in the SAME conductor-asc/prime-major order the generator wrote. base[i] is the cache
    // offset of curve order[i]; base[C] is the total, which MUST equal the cache size (grid
    // identity end-to-end — replaces the sequential cursor's off==cv.size() + overrun checks).
    std::vector<std::size_t> base(C + 1, 0);
    for (std::size_t i = 0; i < C; ++i) {
        const std::size_t c = order[i];
        const i64 d = disc[c], N = rows[c].N;
        std::size_t cnt = 0;
        for (i64 p : gp) { if (p > N) break; if (d % p == 0) continue; ++cnt; }
        base[i + 1] = base[i] + cnt;
    }
    const std::size_t total = base[C];
    CHECK(total == cv.size());
    if (total != cv.size()) {
        MESSAGE(label << " GRID/CACHE SIZE MISMATCH — Σvalid=" << total << " cache=" << cv.size());
        return;
    }

    unsigned hw = std::thread::hardware_concurrency();
    const int T = hw ? static_cast<int>(hw) : 4;
    std::atomic<long long> curves_done{0}, checked_a{0}, band_a{0};
    std::atomic<bool> stop{false}, a_threw{false}, a_mismatch{false};
    std::mutex wmx;
    std::string witness;
    long long wit_key = std::numeric_limits<long long>::max();   // smallest order-index wins

    auto worker = [&](int t) {
        long long lchecked = 0, lband = 0;
        for (std::size_t i = static_cast<std::size_t>(t); i < C && !stop.load(std::memory_order_relaxed);
             i += static_cast<std::size_t>(T)) {
            const std::size_t c = order[i];
            const Curve E{0, 0, 0, rows[c].A, rows[c].B};
            const i64 d = disc[c], N = rows[c].N;
            std::size_t off = base[i];
            for (i64 p : gp) {
                if (p > N) break;
                if (d % p == 0) continue;
                const int cache_ap = static_cast<int>(cv[off++]);
                int side = -2, m0b_ap = 0;
                try {
                    m0b_ap = ap_shanks_mestre(E, static_cast<u64>(p), &side);
                } catch (const std::exception& e) {
                    std::lock_guard<std::mutex> lg(wmx);
                    if (static_cast<long long>(i) < wit_key) {
                        wit_key = static_cast<long long>(i);
                        std::ostringstream ss;
                        ss << label << " TWIN THREW — curve A=" << rows[c].A << " B=" << rows[c].B
                           << " N=" << N << " p=" << p << " : " << e.what();
                        witness = ss.str();
                    }
                    a_threw.store(true);
                    stop.store(true);
                    break;
                }
                if (m0b_ap != cache_ap) {
                    std::lock_guard<std::mutex> lg(wmx);
                    if (static_cast<long long>(i) < wit_key) {
                        wit_key = static_cast<long long>(i);
                        std::ostringstream ss;
                        ss << label << " TWIN MISMATCH — curve A=" << rows[c].A << " B=" << rows[c].B
                           << " N=" << N << " p=" << p << " | cache(charsum)=" << cache_ap
                           << " M0b(shanks-mestre)=" << m0b_ap << " | resolved_by="
                           << (side == -1 ? "charsum-fallback" : side == 0 ? "E-side"
                               : side == 1 ? "twist-side" : "?");
                        witness = ss.str();
                    }
                    a_mismatch.store(true);
                    stop.store(true);
                    break;
                }
                if (p <= static_cast<i64>(at::ell::kMestreThreshold)) ++lband;
                ++lchecked;
            }
            curves_done.fetch_add(1, std::memory_order_relaxed);
        }
        checked_a.fetch_add(lchecked, std::memory_order_relaxed);
        band_a.fetch_add(lband, std::memory_order_relaxed);
    };

    at::verify::Progress prog(label, static_cast<long long>(C), 256);
    std::vector<std::thread> pool;
    for (int t = 0; t < T; ++t) pool.emplace_back(worker, t);
    // Main thread is the progress monitor (workers must not touch doctest/stderr state).
    for (;;) {
        const long long dn = curves_done.load(std::memory_order_relaxed);
        prog.tick(dn, checked_a.load(std::memory_order_relaxed));
        if (dn >= static_cast<long long>(C) || stop.load(std::memory_order_relaxed)) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    for (std::thread& th : pool) th.join();
    const double secs = prog.elapsed();
    prog.tick(curves_done.load(), checked_a.load());

    const long long checked = checked_a.load(), band_le229 = band_a.load();
    CHECK(a_threw.load() == false);
    CHECK(a_mismatch.load() == false);
    CHECK(band_le229 > 0);            // p<=229 charsum-fallback band exercised + compared
    if (a_threw.load() || a_mismatch.load()) MESSAGE(witness);
    MESSAGE(label << ": " << checked << " (curve,prime) a_p compared EXACT (" << band_le229
            << " in the p<=229 charsum-fallback band); " << secs << "s on " << T << " threads, "
            << (secs > 0 ? static_cast<double>(checked) / secs : 0.0) << " evals/s");
    MESSAGE("M0b SPEEDUP: " << label << " wall " << secs << "s vs charsum reference-gen "
            << baseline_secs << "s " << baseline_note << " -> "
            << (secs > 0 ? baseline_secs / secs : 0.0) << "x wall");
}

}  // namespace

TEST_CASE("twin_m0b_vs_charsum_x16") {
    // M0b full-cache twin at 2^16: Shanks-Mestre reproduces the checksummed 2^16 a_p reference
    // cache EXACTLY, every curve × every good prime (§7.1 + P1 + R1 contract). Parallel.
    run_m0b_cache_twin("twin_m0b_vs_charsum_x16", 65536, "/ap_cache_x65536.bin",
                       "/ne_cache_x65536.txt", "data/m5/ne_cache_x65536.txt", 36860.0, "(same grid)");
}

TEST_CASE("twin_m0b_vs_charsum_x17") {
    // M0b full-cache twin at 2^17 — reproduces the checksummed 2^17 a_p reference cache EXACTLY
    // (385,792,535 values). The data-note gate: green before the note cites M0b. Cross-ALGORITHM
    // (SM vs charsum); a_p is a platform-independent integer, so "same platform" is moot. Parallel.
    run_m0b_cache_twin("twin_m0b_vs_charsum_x17", 131072, "/ap_cache_x131072.bin",
                       "/ne_cache_x131072.txt", "data/m5/ne_cache_x131072.txt", 52711.0,
                       "/ 48-thread");
}
