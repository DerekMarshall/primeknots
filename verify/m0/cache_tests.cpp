// Stage M0 — the a_p cache (ARCHITECTURE-M §4): round-trip fidelity and the
// generator-hash refusal DEMONSTRATED firing in BOTH directions (freshness
// precedent) — a valid cache loads, a stale-generator cache is refused.
#include "doctest/doctest.h"

#include <filesystem>
#include <stdexcept>
#include <vector>

#include "ell/ap_cache.h"
#include "ell/ap_fast.h"
#include "ell/curve.h"
#include "harness.h"
#include "m0/fixture.h"

using at::core::u64;
using namespace at::ell;
using at::verify::m0::primes_in;
using at::verify::m0::sample_curves;

TEST_CASE("anchor_ap_cache_stale_hash_refused") {
    const auto curves = sample_curves();
    const auto primes = primes_in(5, 200);
    const auto grid = ap_fast_grid(curves, primes, /*parallel=*/false);

    std::vector<int16_t> vals;               // |a_p| ≤ 2√p fits int16
    for (const auto& row : grid)
        for (int v : row) vals.push_back(static_cast<int16_t>(v));

    const auto dir = std::filesystem::temp_directory_path();
    const std::string ok_path = (dir / "at_m0_apcache_ok.bin").string();
    const std::string stale_path = (dir / "at_m0_apcache_stale.bin").string();

    ApCacheHeader h;
    h.generator_hash = ap_generator_hash();  // current generator → valid
    h.ecdata_sha = "25cec5ecfec8b9f016eb1631ac633194c2bed39f";
    h.prime_bound = 200;
    h.curve_set = "m0-sample-8";
    write_ap_cache(ok_path, h, vals);

    // Direction 1 — a valid cache round-trips exactly.
    ApCacheHeader back;
    const std::vector<int16_t> read = read_ap_cache(ok_path, back);
    const bool roundtrip = (read == vals);
    CHECK(roundtrip);
    CHECK(back.generator_hash == ap_generator_hash());
    CHECK(back.prime_bound == 200u);
    MESSAGE("cache round-trip: " << read.size()
            << " int16 a_p values recovered, generator hash matched");

    // Direction 2 — a cache written by a DIFFERENT generator is REFUSED (throws),
    // never silently reused. This is the refusal firing, demonstrated.
    ApCacheHeader stale = h;
    stale.generator_hash = "deadbeef-stale-generator-hash";
    write_ap_cache(stale_path, stale, vals);
    bool refused = false;
    try {
        ApCacheHeader ignored;
        (void)read_ap_cache(stale_path, ignored);
    } catch (const std::runtime_error& e) {
        refused = true;
        const std::string why = e.what();   // std::string streams as text, not a pointer
        MESSAGE("cache refusal fired: " << why);
    }
    CHECK(refused);

    std::error_code ec;
    std::filesystem::remove(ok_path, ec);
    std::filesystem::remove(stale_path, ec);

    REQUIRE(roundtrip);   // direction 1 demonstrated
    REQUIRE(refused);     // direction 2 demonstrated
}
