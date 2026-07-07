// Stage 2 Phase 1 — conic solver: fast path vs. Holzer brute referee,
// primitivity/membership, and multiple distinct solutions. Convention-free.
#include "doctest/doctest.h"

#include <vector>

#include "redei/conic.h"
#include "symbols/legendre.h"
#include "stage2/fixture.h"

using at::core::i128;
using at::core::u64;
using at::redei::ConicSolution;
using at::redei::on_conic;
using at::redei::solve_conic;
using at::redei::solve_conic_holzer;
using namespace at::verify::stage2;

namespace {
i128 gcd3(i128 a, i128 b, i128 c) {
    auto g = [](i128 x, i128 y) {
        x = x < 0 ? -x : x;
        y = y < 0 ? -y : y;
        while (y) { i128 t = x % y; x = y; y = t; }
        return x;
    };
    return g(g(a, b), c);
}
bool primitive(const ConicSolution& s) { return gcd3(s.x, s.y, s.z) == 1; }
bool distinct(const ConicSolution& p, const ConicSolution& q) {
    return !(p.x == q.x && p.y == q.y && p.z == q.z);
}
}  // namespace

// twin: fast path and Holzer brute force must agree on solubility over every
// pair of distinct primes ≡ 1 (mod 4) in range, and every solution either
// produces must be primitive and lie on the conic.
TEST_CASE("twin_conic_fast_vs_holzer") {
    std::vector<u64> ps = primes_1mod4_up_to(twin_bound());
    REQUIRE(ps.size() == twin_expected_primes());  // pinned to gp π(x;4,1)

    u64 pairs = 0, soluble = 0, insoluble = 0;
    for (std::size_t i = 0; i < ps.size(); ++i) {
        for (std::size_t j = i + 1; j < ps.size(); ++j) {
            u64 a = ps[i], b = ps[j];
            std::vector<ConicSolution> fast = solve_conic(a, b, 1);
            std::vector<ConicSolution> brute = solve_conic_holzer(a, b);

            // Twin agreement on solubility.
            CHECK(fast.empty() == brute.empty());
            // Cross-check solubility against the Legendre criterion (a/b)=(b/a)=1.
            bool legendre_soluble = at::symbols::legendre_euler(a, b) == 1 &&
                                    at::symbols::legendre_euler(b, a) == 1;
            CHECK(!brute.empty() == legendre_soluble);

            for (const auto& s : fast) {
                CHECK(on_conic(s, a, b));
                CHECK(primitive(s));
            }
            for (const auto& s : brute) {
                CHECK(on_conic(s, a, b));
                CHECK(primitive(s));
            }
            if (brute.empty()) ++insoluble; else ++soluble;
            ++pairs;
        }
    }
    MESSAGE("cases: " << pairs << " prime pairs (" << soluble << " soluble, "
                      << insoluble << " insoluble); fast/brute/Legendre agree");
    REQUIRE(pairs == ps.size() * (ps.size() - 1) / 2);
    REQUIRE(soluble > 0);
    REQUIRE(insoluble > 0);
}

// count > 1: for every soluble pair, solve_conic returns several essentially
// different primitive solutions (these feed the Stage 2 invariance tests).
TEST_CASE("theorem_conic_multiple_distinct_solutions") {
    std::vector<u64> ps = primes_1mod4_up_to(twin_bound());
    const int want = 5;
    u64 soluble_pairs = 0, total_solutions = 0;
    for (std::size_t i = 0; i < ps.size(); ++i) {
        for (std::size_t j = i + 1; j < ps.size(); ++j) {
            u64 a = ps[i], b = ps[j];
            std::vector<ConicSolution> sols = solve_conic(a, b, want);
            if (sols.empty()) continue;  // insoluble
            ++soluble_pairs;
            // at least two essentially different solutions
            CHECK(sols.size() >= 2);
            for (std::size_t u = 0; u < sols.size(); ++u) {
                CHECK(on_conic(sols[u], a, b));
                CHECK(primitive(sols[u]));
                for (std::size_t v = u + 1; v < sols.size(); ++v)
                    CHECK(distinct(sols[u], sols[v]));
            }
            total_solutions += sols.size();
        }
    }
    MESSAGE("cases: " << soluble_pairs << " soluble pairs, " << total_solutions
                      << " distinct primitive solutions total");
    REQUIRE(soluble_pairs > 0);
}

// Pinned example: the (13, 61) conic from the Borromean anchor triple.
TEST_CASE("anchor_conic_13_61_has_primitive_solution") {
    std::vector<ConicSolution> s = solve_conic(13, 61, 1);
    REQUIRE(s.size() == 1);
    CHECK(on_conic(s[0], 13, 61));
    CHECK(primitive(s[0]));
    // x² − 13 y² − 61 z² = 0
    i128 lhs = (i128)s[0].x * s[0].x - 13 * (i128)s[0].y * s[0].y -
               61 * (i128)s[0].z * s[0].z;
    CHECK(lhs == 0);
    MESSAGE("cases: 1 — (13,61) conic primitive solution verified");
}
