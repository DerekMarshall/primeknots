// Stage M0 — a_p machinery, convention-free tests (m0-pinning.md §1–§3).
//   anchor_ap_11a1            — the frozen charsum referee reproduces 11a1's
//                               good-prime a_p, quoted from Cremona's aplist.
//   anchor_ap_bad_prime       — the pinned Atkin–Lehner → a_q conversion (§3c),
//                               vs PARI-adjudicated values (reduction-type aware).
//   invariance_weierstrass_model — a_p is a model-choice invariant (§2).
#include "doctest/doctest.h"

#include <stdexcept>
#include <string>
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

TEST_CASE("anchor_ap_small_primes") {
    // a_2, a_3 via the long-model enumeration path (m0-pinning §5, rider R1).
    // 11a1 is good at 2,3 (N=11); a_2=-2, a_3=-1 quoted from aplist line "11 a".
    CHECK(ap_enumerate(k11a1, 2) == -2);
    CHECK(ap_enumerate(k11a1, 3) == -1);
    MESSAGE("anchor_ap_small_primes: a_2=-2, a_3=-1 for 11a1 (enumeration, ∞ counted) "
            "matched Cremona aplist");
    // Sanity: the enumeration agrees with the p>3 charsum referee where both apply
    // (e.g. p=5), so the two referees share the same a_p convention.
    CHECK(ap_enumerate(k11a1, 5) == ap_charsum(k11a1, 5));
    REQUIRE(ap_enumerate(k11a1, 5) == 1);
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
    // Same curve (11a1); a_p must not see the model choice (§2) — among MINIMAL
    // models. Both A and B are minimal (N=11), so the API accepts them.
    const Curve A = k11a1;                  // minimal
    const Curve B = translate(A, 1, 1, 1);  // u = 1 shift — still minimal
    assert_minimal(A, 11);
    assert_minimal(B, 11);

    u64 checked = 0;
    for (u64 p : kGoodPrimes11a1) {
        CHECK(ap_charsum(A, p) == ap_charsum(B, p));
        ++checked;
    }
    MESSAGE("invariance_weierstrass_model: a_p agreed across 2 minimal models of 11a1 over "
            << checked << " good primes");
    REQUIRE(checked == 22);

    // Correction C1: a NON-minimal model must be REFUSED, not silently used. Scaling
    // by u=2 makes Δ ↦ 2¹²·Δ, so 2 | Δ(model) but 2 ∤ N=11 — non-minimal at 2.
    const Curve C = scale(A, 2);
    bool refused = false;
    try {
        assert_minimal(C, 11);
    } catch (const std::runtime_error& e) {
        refused = true;
        const std::string why = e.what();
        MESSAGE("minimality guard fired (C1): " << why);
    }
    CHECK(refused);
    REQUIRE(refused);   // the checked precondition demonstrated firing

    // The silent hazard the guard prevents: point-counting the non-minimal model at
    // p=2 counts a SINGULAR reduction (y²=x³ mod 2) and returns the wrong a_2.
    CHECK(ap_enumerate(C, 2) == 0);    // wrong (singular)
    CHECK(ap_enumerate(A, 2) == -2);   // true a_2 on the minimal model
    MESSAGE("hazard shown: enumeration on the non-minimal model gives a_2=0 (wrong); "
            "true a_2=-2 — the guard refuses C before any a_p is computed");
}
