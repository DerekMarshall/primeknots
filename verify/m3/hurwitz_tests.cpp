// Stage M3 P4 — Hurwitz class numbers, the one Phase-1 code (m3-pinning §P4).
//   anchor_hurwitz_small       — H(0),H(3),H(4),… quoted from source (the weights).
//   twin_hurwitz_forms_vs_decomposition — direct count vs Σ_{f²|n} primitive counts.
//   oracle_hurwitz_vs_pari     — H(n) vs PARI qfbhclassno(n) over a REQUIREd range.
#include "doctest/doctest.h"

#include <sstream>
#include <string>
#include <vector>

#include "harness.h"
#include "mform/hurwitz.h"
#include "oracle/pari.h"

using at::core::i64;
using at::core::u64;
using namespace at::mform;

TEST_CASE("anchor_hurwitz_small") {
    // H(0) = −1/12, H(3) = 1/3, H(4) = 1/2 carry the automorphism convention
    // (Z25 p.6: multiples of x²+xy+y² count 1/3, of x²+y² count 1/2). The rest are
    // standard Hurwitz–Kronecker values.
    struct A { i64 n; Frac h; };
    const std::vector<A> anchors = {
        {0, {-1, 12}}, {3, {1, 3}}, {4, {1, 2}}, {7, {1, 1}}, {8, {1, 1}},
        {11, {1, 1}}, {12, {4, 3}}, {15, {2, 1}}, {16, {3, 2}}, {19, {1, 1}},
        {20, {2, 1}}, {23, {3, 1}}, {24, {2, 1}},
        {1, {0, 1}}, {2, {0, 1}}, {5, {0, 1}},   // n ≡ 1,2 mod 4 → 0
    };
    u64 checked = 0;
    for (const A& a : anchors) {
        CHECK(hurwitz(a.n) == a.h);
        ++checked;
    }
    MESSAGE("anchor_hurwitz_small: " << checked << " Hurwitz values matched source "
            "(H(0)=−1/12, H(3)=1/3, H(4)=1/2, …)");
    REQUIRE(checked == anchors.size());
}

TEST_CASE("twin_hurwitz_forms_vs_decomposition") {
    u64 checked = 0;
    for (i64 n = 0; n <= 3000; ++n) {
        CHECK(hurwitz_by_forms(n) == hurwitz_by_decomposition(n));
        ++checked;
    }
    MESSAGE("twin_hurwitz_forms_vs_decomposition: direct count == Σ_{f²|n} primitive "
            "over " << checked << " values of n in [0,3000]");
    REQUIRE(checked == 3001);
}

TEST_CASE("oracle_hurwitz_vs_pari") {
    std::optional<std::string> gp = oracle::find_gp();
    if (!gp) {
        MESSAGE("[SKIP] oracle_hurwitz_vs_pari: PARI/GP `gp` not found on PATH.");
        at::verify::g_oracle_skipped = true;
        return;
    }
    MESSAGE("oracle: using gp at " << *gp);
    const i64 N = 4000;
    std::ostringstream script;
    for (i64 n = 0; n <= N; ++n) script << "print(qfbhclassno(" << n << "))\n";
    const std::string out = oracle::run_gp(*gp, script.str());

    std::istringstream lines(out);
    std::string line;
    u64 matched = 0;
    for (i64 n = 0; n <= N; ++n) {
        REQUIRE(std::getline(lines, line));
        // PARI prints "num/den" or an integer.
        i64 pn = 0, pd = 1;
        const auto slash = line.find('/');
        if (slash == std::string::npos) { pn = std::stoll(line); pd = 1; }
        else { pn = std::stoll(line.substr(0, slash)); pd = std::stoll(line.substr(slash + 1)); }
        const Frac ours = hurwitz(n);
        CHECK(ours.num == pn);
        CHECK(ours.den == pd);
        ++matched;
    }
    MESSAGE("oracle_hurwitz_vs_pari: " << matched << " H(n) matched PARI qfbhclassno(n), n∈[0," << N << "]");
    REQUIRE(matched == static_cast<u64>(N + 1));
}
