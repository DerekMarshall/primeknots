// Stage M0 — a_p machinery, convention-free tests (m0-pinning.md §1–§3).
//   anchor_ap_11a1            — the frozen charsum referee reproduces 11a1's
//                               good-prime a_p, quoted from Cremona's aplist.
//   anchor_ap_bad_prime       — the pinned Atkin–Lehner → a_q conversion (§3c),
//                               vs PARI-adjudicated values (reduction-type aware).
//   invariance_weierstrass_model — a_p is a model-choice invariant (§2).
#include "doctest/doctest.h"

#include <vector>

#include "ell/ap.h"
#include "ell/curve.h"
#include "harness.h"

using at::core::u64;
using namespace at::ell;

// 11a1 minimal model (allcurves.00000-09999 @ 25cec5ec…, line "11 a 1").
static const Curve k11a1{0, -1, 1, -10, -20};

// Good primes 3 < p < 100, excluding the bad prime 11 (22 primes).
static const std::vector<u64> kGoodPrimes11a1 = {
    5, 7, 13, 17, 19, 23, 29, 31, 37, 41, 43,
    47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};

TEST_CASE("anchor_ap_11a1") {
    // Good-prime a_p quoted from aplist.00000-09999, line "11 a" (m0-pinning §1).
    struct AP { u64 p; int ap; };
    const std::vector<AP> quoted = {
        {5, 1}, {7, -2}, {13, 4}, {17, -2}, {19, 0}, {23, -1}, {29, 0}, {31, 7},
        {37, 3}, {41, -8}, {43, -6}, {47, 8}, {53, -6}, {59, 5}, {61, 12},
        {67, -7}, {71, -3}, {73, 4}, {79, -10}, {83, -6}, {89, 15}, {97, -7}};

    u64 checked = 0;
    for (const AP& q : quoted) {
        CHECK(ap_charsum(k11a1, q.p) == q.ap);
        ++checked;
    }
    MESSAGE("anchor_ap_11a1: " << checked
            << " good-prime a_p (5..97, p∤11) matched the Cremona aplist values");
    REQUIRE(checked == 22);
    REQUIRE(checked == kGoodPrimes11a1.size());
}

TEST_CASE("anchor_ap_bad_prime") {
    // (label, q, ecdata A–L sign w_q as ±1, ord_q(N), expected a_q). Adjudicated
    // by PARI ellap at the pinned models (m0-pinning §3c). The additive rows
    // (27a1@3, 32a1@2) prove a nonzero A–L sign still gives a_q = 0.
    struct BP { const char* label; int q; int w; int ord; int ap; };
    const std::vector<BP> pins = {
        {"11a1", 11, -1, 1, +1},   // multiplicative q ∥ N  → a_q = −w_q
        {"14a1", 2, +1, 1, -1},    // multiplicative
        {"14a1", 7, -1, 1, +1},    // multiplicative
        {"27a1", 3, -1, 3, 0},     // additive q² | N      → a_q = 0
        {"32a1", 2, -1, 5, 0},     // additive
    };

    u64 checked = 0;
    for (const BP& b : pins) {
        CHECK(ap_from_atkin_lehner(b.w, b.ord) == b.ap);
        ++checked;
    }
    MESSAGE("anchor_ap_bad_prime: " << checked
            << " bad-prime conversions matched PARI-adjudicated a_q");
    REQUIRE(checked == 5);
}

TEST_CASE("invariance_weierstrass_model") {
    // Same curve (11a1), three integral models; a_p must not see the choice (§2).
    const Curve A = k11a1;                  // minimal
    const Curve B = translate(A, 1, 1, 1);  // u = 1 shift
    const Curve C = scale(A, 2);            // a_i ↦ 2^i a_i  (good primes are ∤ 2)

    u64 checked = 0;
    for (u64 p : kGoodPrimes11a1) {
        const int a = ap_charsum(A, p);
        const int b = ap_charsum(B, p);
        const int c = ap_charsum(C, p);
        CHECK(a == b);
        CHECK(a == c);
        ++checked;
    }
    MESSAGE("invariance_weierstrass_model: a_p agreed across 3 models of 11a1 over "
            << checked << " good primes");
    REQUIRE(checked == 22);
}
