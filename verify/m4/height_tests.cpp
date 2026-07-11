// Stage M4 (Phase 2, rider R4 / m4-pinning §P5) — the height-ordered family and its
// CERTIFIED counts. The count at each cutoff is the statistic's denominator scale, so
// it is checked by two independent enumerations (and a third, gp, offline).
//   twin_height_family_count  — direct (per-curve trial) == sieve (mark p⁴/p⁶ multiples).
//   anchor_height_family_count — the counts, certified against an independent PARI/GP
//                                enumeration (14, 166, 1048, 7130 at X=10²…10⁵).
//   theorem_height_family_reduced — every returned curve is reduced, nonsingular, H≤X.
#include "doctest/doctest.h"

#include <cstddef>
#include <string>
#include <vector>

#include "harness.h"
#include "murm/height_family.h"

using at::core::i64;
using namespace at::murm;

namespace {
i64 iabs(i64 x) { return x < 0 ? -x : x; }
bool reduced_ref(i64 A, i64 B) {   // independent re-derivation of the reduced predicate
    for (i64 p = 2; p * p <= (iabs(A) > iabs(B) ? iabs(A) : iabs(B)) + 1; ++p) {
        i64 va = 0, a = iabs(A);
        while (a && a % p == 0) { a /= p; ++va; }
        i64 vb = 0, b = iabs(B);
        while (b && b % p == 0) { b /= p; ++vb; }
        const bool p4A = (A == 0) || va >= 4;
        const bool p6B = (B == 0) || vb >= 6;
        if (p4A && p6B) return false;
    }
    return true;
}
}  // namespace

TEST_CASE("twin_height_family_count") {
    const std::vector<i64> Xs = {100, 300, 1000, 3000, 10000, 100000, 1000000};
    std::size_t checked = 0;
    for (i64 X : Xs) {
        const std::size_t direct = height_family(X).size();
        const i64 sieve = height_family_count_sieve(X);
        CHECK(static_cast<i64>(direct) == sieve);
        ++checked;
    }
    MESSAGE("twin_height_family_count: direct (per-curve trial) == sieve (mark p⁴/p⁶ "
            "multiples) at " << checked << " height cutoffs up to 10⁶");
    REQUIRE(checked == Xs.size());
}

TEST_CASE("anchor_height_family_count") {
    // Certified by an independent PARI/GP factor-based enumeration (m4-pinning §P5):
    struct A { i64 X, count; };
    const std::vector<A> anchors = {{100, 14}, {1000, 166}, {10000, 1048}, {100000, 7130}};
    for (const A& a : anchors) {
        CHECK(static_cast<i64>(height_family(a.X).size()) == a.count);
        CHECK(height_family_count_sieve(a.X) == a.count);
    }
    MESSAGE("anchor_height_family_count: reduced-nonsingular family sizes "
            "14/166/1048/7130 at X=10²/10³/10⁴/10⁵ — matched (gp-certified)");
    REQUIRE(anchors.size() == 4);
}

TEST_CASE("theorem_height_family_reduced") {
    // Every returned curve is reduced (independent predicate), nonsingular, and H ≤ X.
    const i64 X = 100000;
    const std::vector<HeightCurve> fam = height_family(X);
    std::size_t checked = 0;
    for (const HeightCurve& e : fam) {
        CHECK(4 * e.A * e.A * e.A + 27 * e.B * e.B != 0);   // nonsingular
        CHECK(naive_height(e.A, e.B) <= X);
        CHECK(reduced_ref(e.A, e.B));                       // reduced (independent check)
        ++checked;
    }
    MESSAGE("theorem_height_family_reduced: all " << checked << " curves (X=10⁵) are "
            "reduced, nonsingular, H≤X");
    REQUIRE(checked == fam.size());
    REQUIRE(checked == 7130);
}
