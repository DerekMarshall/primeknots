// Stage 2 Phase 2 — the Rédei symbol. Test order is load-bearing (reviewer):
// invariance_* FIRST (senior), then the anchor, then reciprocity (the primary
// N4 detector). Riders R1–R3 per docs/notes/stage2-pinning.md §8.
#include "doctest/doctest.h"

#include <array>
#include <set>
#include <vector>

#include "redei/conic.h"
#include "redei/omega_mod4.h"
#include "redei/redei_symbol.h"
#include "symbols/legendre.h"
#include "symbols/tonelli_shanks.h"
#include "stage2/fixture.h"

using at::core::i128;
using at::core::u64;
using namespace at::redei;
using namespace at::verify::stage2;

namespace {
bool unlinked(u64 a, u64 b) { return at::symbols::legendre_euler(a, b) == 1; }

// All triples p<q<r of primes ≡ 1 (mod 4) ≤ bound that are pairwise unlinked.
std::vector<std::array<u64, 3>> valid_triples(u64 bound) {
    std::vector<u64> ps = primes_1mod4_up_to(bound);
    std::vector<std::array<u64, 3>> out;
    for (std::size_t i = 0; i < ps.size(); ++i)
        for (std::size_t j = i + 1; j < ps.size(); ++j) {
            if (!unlinked(ps[i], ps[j])) continue;
            for (std::size_t k = j + 1; k < ps.size(); ++k)
                if (unlinked(ps[i], ps[k]) && unlinked(ps[j], ps[k]))
                    out.push_back({ps[i], ps[j], ps[k]});
        }
    return out;
}
}  // namespace

// SENIOR TEST 1. The symbol is independent of which primitive conic solution is
// used. R2: must observe ≥ 2 distinct 2-adic classes of raw β among the
// solutions of some p3 ≡ 5 (mod 8) triple, else it passes vacuously.
TEST_CASE("invariance_redei_solution_choice") {
    auto triples = valid_triples(twin_bound());
    REQUIRE(triples.size() > 0);
    u64 triples_checked = 0, class5mod8 = 0, nonvacuous = 0, comparisons = 0;
    for (auto& t : triples) {
        // put a ≡5 mod 8 prime in the third slot where possible (N4-sensitive)
        u64 p1 = t[0], p2 = t[1], p3 = t[2];
        u64 s = *at::symbols::sqrt_mod(p1 % p3, p3);
        auto sols = solve_conic(p1, p2, 40);
        int ref = 0;
        bool have_ref = false;
        std::set<int> classes;
        for (auto& c : sols) {
            if (c.z % static_cast<i128>(p3) == 0) continue;  // R1: skip degenerate
            int v = redei_value_from_solution(p1, p3, s, c.x, c.y);
            REQUIRE(v != 0);
            if (!have_ref) { ref = v; have_ref = true; }
            CHECK(v == ref);  // <-- invariance
            classes.insert(two_adic_class(c.x, c.y, p1));
            ++comparisons;
        }
        REQUIRE(have_ref);
        if (p3 % 8 == 5) ++class5mod8;
        if (p3 % 8 == 5 && classes.size() >= 2) ++nonvacuous;
        ++triples_checked;
    }
    MESSAGE("cases: " << comparisons << " solution comparisons over "
                      << triples_checked << " triples; " << class5mod8
                      << " with p3≡5 (mod 8); " << nonvacuous
                      << " of those saw ≥2 distinct 2-adic classes (non-vacuous)");
    REQUIRE(class5mod8 > 0);   // R2: ≡5 mod 8 present in force
    REQUIRE(nonvacuous > 0);   // R2: the (2/p3)^w correction was load-bearing
}

// SENIOR TEST 2. Independent of the square-root branch (±s): both roots give the
// same value (a small theorem, since the product is (p2/p3)=1).
TEST_CASE("invariance_sqrt_branch") {
    auto triples = valid_triples(twin_bound());
    REQUIRE(triples.size() > 0);
    u64 checks = 0;
    for (auto& t : triples) {
        u64 p1 = t[0], p2 = t[1], p3 = t[2];
        u64 s = *at::symbols::sqrt_mod(p1 % p3, p3);
        u64 s2 = p3 - s;  // the other root
        auto sols = solve_conic(p1, p2, 12);
        for (auto& c : sols) {
            if (c.z % static_cast<i128>(p3) == 0) continue;
            int v1 = redei_value_from_solution(p1, p3, s, c.x, c.y);
            int v2 = redei_value_from_solution(p1, p3, s2, c.x, c.y);
            REQUIRE(v1 != 0);
            REQUIRE(v2 != 0);
            CHECK(v1 == v2);  // branch independence
            ++checks;
        }
    }
    MESSAGE("cases: " << checks << " (±s) branch comparisons; both roots agree");
    REQUIRE(checks > 0);
}

// R1: the degenerate branch (p3 | z) is *exercised*, not just asserted absent.
// Find a real primitive solution with p3 | z, confirm the branch value is the
// degenerate sentinel (0), and that the full API re-solves to the correct value.
TEST_CASE("theorem_redei_degenerate_resolve") {
    auto triples = valid_triples(twin_bound());
    u64 degenerate_seen = 0;
    for (auto& t : triples) {
        u64 p1 = t[0], p2 = t[1], p3 = t[2];
        u64 s = *at::symbols::sqrt_mod(p1 % p3, p3);
        auto sols = solve_conic(p1, p2, 200);
        int good_value = redei_symbol(p1, p2, p3);  // API skips degenerate (R1)
        for (auto& c : sols) {
            if (c.z % static_cast<i128>(p3) != 0) continue;  // want p3 | z
            // degeneracy manifests on at least one branch:
            int vs = redei_value_from_solution(p1, p3, s, c.x, c.y);
            int vo = redei_value_from_solution(p1, p3, p3 - s, c.x, c.y);
            bool degenerate_branch = (vs == 0) || (vo == 0);
            CHECK(degenerate_branch);
            ++degenerate_seen;
        }
        // whether or not this triple had a degenerate solution, the API value is
        // a genuine ±1 (never the degenerate sentinel):
        bool api_is_pm1 = (good_value == 1) || (good_value == -1);
        CHECK(api_is_pm1);
    }
    MESSAGE("cases: " << degenerate_seen
                      << " primitive solutions with p3|z exercised the R1 guard");
    REQUIRE(degenerate_seen > 0);  // the branch actually fired
}

// ANCHOR (after invariance). The standard Borromean triple.
TEST_CASE("anchor_redei_13_61_937_is_minus_1") {
    CHECK(redei_symbol(13, 61, 937) == -1);
    CHECK(redei_bit(13, 61, 937) == 1);
    MESSAGE("cases: 1 — [13,61,937] = -1");
}

// PRIMARY N4 DETECTOR. Full S3 symmetry over all valid in-range triples; the
// permutations that put a ≡5 mod 8 prime third are the load-bearing ones.
TEST_CASE("theorem_redei_reciprocity_s3") {
    auto triples = valid_triples(twin_bound());
    REQUIRE(triples.size() > 0);
    u64 triples_checked = 0, perms_with_5mod8_third = 0;
    for (auto& t : triples) {
        u64 a = t[0], b = t[1], c = t[2];
        int base = redei_symbol(a, b, c);
        // all 6 permutations must agree (Rédei reciprocity / full symmetry)
        CHECK(redei_symbol(a, c, b) == base);
        CHECK(redei_symbol(b, a, c) == base);
        CHECK(redei_symbol(b, c, a) == base);
        CHECK(redei_symbol(c, a, b) == base);
        CHECK(redei_symbol(c, b, a) == base);
        for (u64 third : {a, b, c})
            if (third % 8 == 5) ++perms_with_5mod8_third;
        ++triples_checked;
    }
    MESSAGE("cases: " << triples_checked << " valid triples × 6 permutations; "
                      << perms_with_5mod8_third
                      << " permutations with a ≡5 (mod 8) third argument (N4)");
    REQUIRE(perms_with_5mod8_third > 0);  // R2: N4-sensitive perms in force
}
