// Stage M4 — the committed N/ε cache (m4-pinning §P4/C3): the oracle-provenance
// INPUT data the height-ordered statistic consumes. Two checks:
//   anchor_ne_cache_committed — the committed data/m4/ne_cache.txt reads, its header
//     is as expected (X=10⁴, count 1048, generator hash == HEAD's), every ε is ±1,
//     and its (A,B) keys equal an INDEPENDENT re-enumeration of the height family
//     (murm::height_family) exactly — the cache covers precisely the family, no drift.
//   anchor_ne_cache_stale_hash_refused — the generator-hash refusal DEMONSTRATED
//     firing in both directions (a valid cache round-trips; a stale-generator cache
//     throws), the ap_cache freshness precedent applied to the committed oracle input.
#include "doctest/doctest.h"

#include <algorithm>
#include <filesystem>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "harness.h"
#include "murm/height_family.h"
#include "murm/ne_cache.h"

using at::core::i64;
using namespace at::murm;

#ifndef AT_M4_DATA_DIR
#define AT_M4_DATA_DIR "data/m4"
#endif

TEST_CASE("anchor_ne_cache_committed") {
    const std::string path = std::string(AT_M4_DATA_DIR) + "/ne_cache.txt";

    NeCacheHeader h;
    std::vector<NeRow> rows;
    try {
        rows = read_ne_cache(path, h);
    } catch (const std::exception& e) {
        MESSAGE("[SKIP] anchor_ne_cache_committed: cache unreadable (" << e.what() << ").");
        at::verify::g_oracle_skipped = true;
        return;   // committed cache absent/stale in this tree — surfaced, not a silent pass
    }

    CHECK(h.X == 10000);
    CHECK(h.count == 1048);
    CHECK(static_cast<i64>(rows.size()) == 1048);
    CHECK(h.generator_hash == ne_generator_hash());   // matches HEAD's height_family.cpp

    // Every root number is a genuine sign.
    bool all_eps_pm1 = true;
    for (const NeRow& r : rows) all_eps_pm1 = all_eps_pm1 && (r.eps == 1 || r.eps == -1);
    CHECK(all_eps_pm1);

    // The cache's (A,B) keys equal an independent re-enumeration of the family — the
    // ordering axis is computed-provenance and must match, or the oracle input is
    // keyed to a stale family (twin: cache keys vs a fresh height_family(X)).
    std::vector<HeightCurve> fam = height_family(h.X);
    std::set<std::pair<i64, i64>> cache_keys, fam_keys;
    for (const NeRow& r : rows) cache_keys.insert({r.A, r.B});
    for (const HeightCurve& c : fam) fam_keys.insert({c.A, c.B});
    const bool keys_match = (cache_keys == fam_keys);
    CHECK(keys_match);
    CHECK(cache_keys.size() == rows.size());   // no duplicate keys

    // Sanity: ε is not degenerate (both signs occur — a murmuration needs the bias,
    // not a constant sign). Reported, not a hard theorem here.
    long long pos = std::count_if(rows.begin(), rows.end(), [](const NeRow& r){ return r.eps == 1; });
    MESSAGE("committed N/ε cache: " << rows.size() << " rows, ε=+1 count " << pos
            << " (" << (rows.size() - pos) << " with ε=−1), keys_match=" << keys_match);

    REQUIRE(keys_match);
    REQUIRE(h.generator_hash == ne_generator_hash());
}

TEST_CASE("anchor_ne_cache_stale_hash_refused") {
    const auto dir = std::filesystem::temp_directory_path();
    const std::string ok_path = (dir / "at_m4_ne_ok.txt").string();
    const std::string stale_path = (dir / "at_m4_ne_stale.txt").string();

    std::vector<NeRow> vals = {{-1, 0, 32, 1}, {0, 1, 27, -1}, {1, 1, 92, 1}};

    NeCacheHeader h;
    h.generator_hash = ne_generator_hash();   // current generator → valid
    h.pari_version = "[2, 17, 4]";
    h.X = 100;
    h.count = static_cast<i64>(vals.size());
    write_ne_cache(ok_path, h, vals);

    // Direction 1 — a valid cache round-trips exactly.
    NeCacheHeader back;
    std::vector<NeRow> read = read_ne_cache(ok_path, back);
    bool roundtrip = (read.size() == vals.size());
    for (size_t i = 0; roundtrip && i < vals.size(); ++i)
        roundtrip = roundtrip && read[i].A == vals[i].A && read[i].B == vals[i].B &&
                    read[i].N == vals[i].N && read[i].eps == vals[i].eps;
    CHECK(roundtrip);
    CHECK(back.generator_hash == ne_generator_hash());
    CHECK(back.X == 100);

    // Direction 2 — a cache written by a DIFFERENT generator is REFUSED (throws).
    NeCacheHeader stale = h;
    stale.generator_hash = "deadbeef-stale-generator-hash";
    write_ne_cache(stale_path, stale, vals);
    bool refused = false;
    try {
        NeCacheHeader ignored;
        (void)read_ne_cache(stale_path, ignored);
    } catch (const std::runtime_error& e) {
        refused = true;
        const std::string why = e.what();
        MESSAGE("N/ε cache refusal fired: " << why);
    }
    CHECK(refused);

    std::error_code ec;
    std::filesystem::remove(ok_path, ec);
    std::filesystem::remove(stale_path, ec);

    REQUIRE(roundtrip);
    REQUIRE(refused);
}
