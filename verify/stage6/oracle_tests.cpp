// Stage 6 twin: the LMFDB-independent referee (Belabas's `cubic` enumerator,
// Math. Comp. 66 (1997) — a separate code path from PARI's nflist) vs the PARI
// referee, per prime set. Both counts are committed caches (data/belabas/,
// data/cubic/), so this comparison runs everywhere including CI; it is the twin
// that replaces the reCAPTCHA-blocked LMFDB API (docs/notes/stage6-pinning.md
// R4). Rule 1: a disagreement is a witnessed deliverable, not a reconciled bug.
#include "doctest/doctest.h"

#include <fstream>
#include <string>
#include <vector>

#include "harness.h"
#include "stage6/fixture.h"

#ifndef AT_BELABAS_CACHE
#define AT_BELABAS_CACHE "data/belabas/cubic_s3.txt"
#endif

using namespace at::verify::stage6;

namespace {
// Belabas cache line: "primes_csv | c (or 'skip') | disc_bound".
struct BelabasRow { std::vector<std::uint64_t> primes; bool feasible; long long c; };

std::vector<BelabasRow> load_belabas() {
    std::vector<BelabasRow> out;
    std::ifstream f(AT_BELABAS_CACHE);
    if (!f) return out;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> parts;
        std::size_t start = 0, bar;
        while ((bar = line.find('|', start)) != std::string::npos) {
            parts.push_back(line.substr(start, bar - start));
            start = bar + 1;
        }
        parts.push_back(line.substr(start));
        if (parts.size() < 2) continue;
        BelabasRow r;
        for (long long x : parse_csv_ll(parts[0]))
            r.primes.push_back(static_cast<std::uint64_t>(x));
        std::string cfield = parts[1];
        std::size_t a = cfield.find_first_not_of(" \t");
        r.feasible = (cfield.find("skip") == std::string::npos);
        r.c = r.feasible ? std::stoll(cfield.substr(a)) : -1;
        out.push_back(r);
    }
    return out;
}
}  // namespace

TEST_CASE("twin_cubic_count_belabas_vs_pari") {
    auto bel = load_belabas();
    if (bel.empty()) {
        MESSAGE("[SKIP] twin_cubic_count_belabas_vs_pari: Belabas cache "
                AT_BELABAS_CACHE " absent (run oracle/build_belabas.sh && "
                "oracle/belabas_cubic.py).");
        at::verify::g_oracle_skipped = true;
        return;
    }
    auto pari = load_cubic_cache();
    REQUIRE(!pari.empty());
    int compared = 0, skipped = 0;
    for (const auto& br : bel) {
        if (!br.feasible) { ++skipped; continue; }  // Dmax too large: recorded, not counted
        const CubicInstance* p = nullptr;
        for (const auto& ci : pari) if (ci.primes == br.primes) { p = &ci; break; }
        REQUIRE(p != nullptr);
        CHECK(br.c == p->c);   // two independent enumerators agree
        ++compared;
    }
    MESSAGE("Belabas vs PARI c agreed on " << compared
            << " feasible prime sets (" << skipped << " skipped: Dmax too large)");
    CHECK(compared >= 10);   // the twin must actually cover the bulk of the sweep
}
