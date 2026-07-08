// Stage 6 oracle: the LMFDB twin for c. Compares the cached LMFDB non-Galois
// (S3) cubic iso-class count to the PARI referee count, per prime set. SKIPs
// cleanly (visible message, exit 77) when the LMFDB cache is absent — as it is
// while LMFDB's public API imposes a reCAPTCHA bot-challenge on bulk extraction
// (recorded in docs/notes/stage6-pinning.md R4). The PARI referee's completeness
// is certified independently by the D_max = ∏ p² bound.
#include "doctest/doctest.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "harness.h"
#include "stage6/fixture.h"

#ifndef AT_LMFDB_CUBIC
#define AT_LMFDB_CUBIC "data/lmfdb/cubic_s3.txt"
#endif

using namespace at::verify::stage6;

namespace {
// LMFDB cache line format: "primes_csv | c_lmfdb". Returns (primes-key -> c).
std::vector<std::pair<std::vector<std::uint64_t>, long long>> load_lmfdb() {
    std::vector<std::pair<std::vector<std::uint64_t>, long long>> out;
    std::ifstream f(AT_LMFDB_CUBIC);
    if (!f) return out;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto bar = line.find('|');
        if (bar == std::string::npos) continue;
        std::vector<std::uint64_t> primes;
        for (long long x : parse_csv_ll(line.substr(0, bar)))
            primes.push_back(static_cast<std::uint64_t>(x));
        out.push_back({primes, std::stoll(line.substr(bar + 1))});
    }
    return out;
}
}  // namespace

TEST_CASE("twin_cubic_count_lmfdb_vs_pari") {
    auto lmfdb = load_lmfdb();
    if (lmfdb.empty()) {
        MESSAGE("[SKIP] twin_cubic_count_lmfdb_vs_pari: LMFDB cache " AT_LMFDB_CUBIC
                " absent (LMFDB API reCAPTCHA-blocked; see stage6-pinning.md R4). "
                "PARI completeness is certified by the D_max bound meanwhile.");
        at::verify::g_oracle_skipped = true;
        return;
    }
    auto pari = load_cubic_cache();
    REQUIRE(!pari.empty());
    int compared = 0;
    for (const auto& [primes, c_lmfdb] : lmfdb) {
        const CubicInstance* p = nullptr;
        for (const auto& ci : pari) if (ci.primes == primes) { p = &ci; break; }
        REQUIRE(p != nullptr);
        CHECK(p->c == c_lmfdb);   // Rule 1: a mismatch is a witnessed deliverable
        ++compared;
    }
    MESSAGE("LMFDB vs PARI c agreed on " << compared << " prime sets");
}
