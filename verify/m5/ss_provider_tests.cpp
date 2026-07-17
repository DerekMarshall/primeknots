// Stage M0b — a_p PROVIDER copy-drift guard (wiring rider R2, 2026-07-17).
//
// ss_empirical_partials_m0b (the --ap m0b production path) is a byte-copy of the canonical
// ss_empirical_partials with exactly one line swapped (ell::ap_fast → ell::ap_shanks_mestre).
// SS_GENERATOR_HASH covers only ss_empirical.cpp, so a future edit to EITHER loop could let the
// two diverge SILENTLY (the hash would not notice). This scratch-scale twin runs BOTH providers
// over the H ≤ 4000 family and REQUIREs byte-identical partials — same a_p integers, same
// prime-major iteration and accumulation order ⇒ identical doubles. It converts the one-off
// production-wiring gate into a STANDING invariant: the copied loop can never drift without this
// test going red and naming the first divergent (curve, bin). Seconds; needs only the committed
// ne_cache (both providers compute a_p from scratch — no a_p cache dependency).
#include "doctest/doctest.h"

#include <fstream>
#include <string>
#include <vector>

#include "harness.h"
#include "murm/height_family.h"
#include "murm/ne_cache.h"
#include "murm/ss_empirical.h"

#ifndef AT_M5_DATA_DIR
#define AT_M5_DATA_DIR "data/m5"
#endif

TEST_CASE("twin_ss_provider_fast_vs_m0b") {
    using at::core::i64;
    using namespace at::murm;

    const std::string ne = std::string(AT_M5_DATA_DIR) + "/ne_cache_x65536.txt";
    {
        std::ifstream probe(ne);
        if (!probe.good()) {
            MESSAGE("[SKIP] ne_cache_x65536.txt absent — cannot run the provider twin.");
            at::verify::g_oracle_skipped = true;
            return;
        }
    }

    NeCacheHeader h;
    std::vector<NeRow> all = read_ne_cache(ne, h);
    std::vector<NeRow> rows;                                  // scratch-scale family: H ≤ 4000
    for (const NeRow& r : all)
        if (naive_height(r.A, r.B) <= 4000) rows.push_back(r);
    REQUIRE(rows.size() > 100);                               // a genuine range actually ran

    const double du = 0.025;
    const int threads = 4;                                   // any fixed count: both use the same order
    const SSPartials pf = ss_empirical_partials(rows, du, threads);       // canonical (ap_fast)
    const SSPartials pm = ss_empirical_partials_m0b(rows, du, threads);   // production path (Shanks–Mestre)

    REQUIRE(pf.NB == pm.NB);
    REQUIRE(pf.A.size() == pm.A.size());

    long long compared = 0;
    bool ident = true;
    std::size_t dc = 0;
    int db = 0;
    for (std::size_t c = 0; c < pf.A.size() && ident; ++c)
        for (int b = 0; b < pf.NB; ++b) {
            if (pf.num[c][b] != pm.num[c][b] || pf.cnt[c][b] != pm.cnt[c][b]) {
                ident = false; dc = c; db = b; break;
            }
            ++compared;
        }

    if (!ident)
        MESSAGE("PROVIDER DRIFT — the m0b loop no longer matches the canonical loop. First at "
                "curve idx " << dc << " (A=" << pf.A[dc] << " B=" << pf.B[dc] << ") bin " << db
                << ": fast num=" << pf.num[dc][db] << " cnt=" << pf.cnt[dc][db]
                << " | m0b num=" << pm.num[dc][db] << " cnt=" << pm.cnt[dc][db]);
    MESSAGE("provider twin (H<=4000): " << pf.A.size() << " curves x " << pf.NB << " bins, "
            << compared << " values compared — " << std::string(ident ? "BYTE-IDENTICAL" : "DRIFT"));
    CHECK(ident);
}
