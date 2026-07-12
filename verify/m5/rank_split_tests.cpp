// Stage M5 (research mode) — PR-2 rank-split confirmation.
//
// prereg_rank_split (STEP 1 portion, the checkable part): the committed analytic-rank
// cache is generator-hash-bound to the same (A,B) family as the N/ε cache, the
// analytic-rank/ε PARITY cross-check is 0 mismatches over ALL 5042 (ε=(−1)^r is the
// functional-equation sign — a theorem, so this is a genuine check, not a tautology), and
// f₂ + the rank distribution are pinned (LOOKS L4 / PR-2 postscript). The PRIMARY leave-out
// trough recovery and the value-space contrast (step 3) extend this test when they run.
#include "doctest/doctest.h"

#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "harness.h"
#include "murm/ne_cache.h"
#include "murm/rank_cache.h"
#include "murm/rank_split.h"
#include "murm/ss_empirical.h"

using at::core::i64;
using namespace at::murm;

#ifndef AT_M5_DATA_DIR
#define AT_M5_DATA_DIR "data/m5"
#endif

TEST_CASE("prereg_rank_split") {
    const std::string rank_path = std::string(AT_M5_DATA_DIR) + "/rank_cache_x65536.txt";
    const std::string ne_path = std::string(AT_M5_DATA_DIR) + "/ne_cache_x65536.txt";
    RankCacheHeader rh;
    NeCacheHeader nh;
    std::vector<RankRow> rr;
    std::vector<NeRow> ne;
    try {
        rr = read_rank_cache(rank_path, rh);   // refuses a stale generator hash
        ne = read_ne_cache(ne_path, nh);
    } catch (const std::exception& e) {
        MESSAGE("[SKIP] prereg_rank_split: cache unreadable/stale (" << e.what() << ").");
        at::verify::g_oracle_skipped = true;
        return;
    }

    REQUIRE(rr.size() == ne.size());
    CHECK(rh.count == 5042);
    CHECK(nh.count == 5042);
    CHECK(rh.X == 65536);

    // Join by (A,B); parity cross-check + rank distribution over the full family.
    std::map<std::pair<i64, i64>, int> rank_of;
    for (const RankRow& r : rr) rank_of[{r.A, r.B}] = r.rank;

    long long mism = 0, n2 = 0;
    std::map<int, long long> dist;
    for (const NeRow& e : ne) {
        auto it = rank_of.find({e.A, e.B});
        REQUIRE(it != rank_of.end());
        const int r = it->second;
        ++dist[r];
        if (r == 2) ++n2;
        const bool r_even = (r % 2 == 0);
        const bool eps_plus = (e.eps == 1);
        if (r_even != eps_plus) ++mism;
    }

    MESSAGE("rank dist: r0=" << dist[0] << " r1=" << dist[1] << " r2=" << dist[2]
            << " r3=" << dist[3]);
    MESSAGE("analytic-rank/eps parity mismatches: " << mism << " / " << ne.size()
            << " (functional-equation-sign theorem check)");
    // The theorem check: ε=(−1)^{analytic rank}, so a mismatch = a numerical
    // mis-determination. Zero mismatches is required (the generator aborts otherwise).
    CHECK(mism == 0);

    const double f2 = static_cast<double>(n2) / static_cast<double>(ne.size());
    MESSAGE("f2 = " << n2 << "/" << ne.size() << " = " << f2 << " (pre-declared look, L4)");

    // Pinned (LOOKS L4 / PR-2 postscript): a regression in the rank cache is caught here.
    CHECK(n2 == 738);
    CHECK(dist[0] == 1754);
    CHECK(dist[1] == 2525);
    CHECK(dist[2] == 738);
    CHECK(dist[3] == 25);
    REQUIRE(mism == 0);

    // --- PR-2 step 3: the committed gates + the branch taken (pinned) ---
    SSPartialsMeta pm;
    SSPartials P;
    try {
        P = read_ss_partials(std::string(AT_M5_DATA_DIR) + "/ss_partials_x65536.txt", pm, false);
    } catch (const std::exception& e) {
        MESSAGE("[SKIP] step 3: partials unreadable/stale (" << e.what() << ").");
        at::verify::g_oracle_skipped = true;
        return;
    }
    // Align analytic rank to the partials' canonical order.
    std::vector<int> pr(P.A.size(), -1);
    for (std::size_t c = 0; c < P.A.size(); ++c) pr[c] = rank_of[{P.A[c], P.B[c]}];

    // Committed thresholds (PR-2 Amendment 4 + step 2): target 0.805, τ=0.06, contrast 0.668.
    const RankSplit s = rank_split(P, pr, pm.X, 0.805, 0.06, 0.668, 0.7, 0.9);
    MESSAGE("step3: full trough=" << s.full.shape.trough_u << " (dev " << s.dev_full
            << "); F\\S2 trough=" << s.leaveout.shape.trough_u << " (dev " << s.dev_leaveout
            << "); recovers=" << s.recovers << "; gap=" << s.gap
            << " (downward=" << s.contrast_downward << ")");

    // PRIMARY (u-space): H0 — leave-out does NOT recover. Pinned: F∖S2 trough unchanged at
    // the displaced bin 0.8875, dev identical to the full family; a regression that moved it
    // would (correctly) fail here.
    CHECK(s.n_leaveout == 4304);
    CHECK(std::abs(s.leaveout.shape.trough_u - 0.8875) < 1e-9);
    CHECK(std::abs(s.dev_leaveout - s.dev_full) < 1e-9);
    CHECK(s.recovers == false);

    // SECONDARY (value-space): downward-significant — S2 markedly more negative on the
    // descending branch. Pinned: gap past the committed −0.668, downward.
    CHECK(s.gap < -0.668);
    CHECK(s.contrast_downward == true);

    REQUIRE(s.recovers == false);        // the primary null is the finding
    REQUIRE(s.contrast_downward == true);
}
