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
}
