// Stage M0b (Phase 2a) — Shanks–Mestre a_p seals.
//
//   anchor_shanks_mestre_pari  — the §6 PARI-adjudicated anchors (gp 2.17.4, recorded in
//     m0b-pinning.md §6): supersingular a_p=0, the non-cyclic [52,4] curve whose λ=52 is
//     AMBIGUOUS ({208,260}) and is resolved only by the twist (A2 — the R2 disambiguation
//     path), the ordinary j=0 pair, and the 227/229/233 threshold straddle (fast/fallback).
//   anchor_shanks_mestre_bad_reduction_refused — a bad prime (p | disc) is REFUSED, demonstrated
//     firing (M0b never point-counts a singular reduction; it never assumes a pre-filtered caller).
//   twin_shanks_mestre_vs_charsum — over good primes p > 229 and several curves, Shanks–Mestre
//     reproduces the frozen ell::ap_charsum EXACTLY (integer equality). The cross-ALGORITHM twin.
#include "doctest/doctest.h"

#include <stdexcept>
#include <vector>

#include "harness.h"
#include "ell/ap.h"                 // ap_charsum (frozen referee)
#include "ell/ap_shanks_mestre.h"
#include "ell/curve.h"

using at::core::u64;
using at::ell::Curve;
using at::ell::ap_charsum;
using at::ell::ap_shanks_mestre;

TEST_CASE("anchor_shanks_mestre_pari") {
    // A1 — supersingular j=0 (y²=x³+1), p=233 ≡ 2 mod 3: a_p = 0  [#E=234].  p>229 ⇒ SM path.
    CHECK(ap_shanks_mestre(Curve{0, 0, 0, 0, 1}, 233) == 0);

    // A2 — non-cyclic j=1728 (y²=x³−x), p=233: ellgroup [52,4], λ=52 has TWO Hasse multiples
    // {208,260}; the twist's λ=130 has the unique {260} ⇒ #E=208 ⇒ a_p = 26. Exercises the
    // Mestre twist disambiguation (the whole point of the R2 loop invariant).
    CHECK(ap_shanks_mestre(Curve{0, 0, 0, -1, 0}, 233) == 26);

    // A5 — ordinary j=0 (y²=x³+1), p=241 ≡ 1 mod 3: a_p = 14  [#E=228].
    CHECK(ap_shanks_mestre(Curve{0, 0, 0, 0, 1}, 241) == 14);

    // A4 — threshold straddle on y²=x³+x+1: 227,229 route to the charsum fallback (≤229),
    // 233 to the Shanks–Mestre fast path — all three must match the adjudicated values.
    CHECK(ap_shanks_mestre(Curve{0, 0, 0, 1, 1}, 227) == 0);
    CHECK(ap_shanks_mestre(Curve{0, 0, 0, 1, 1}, 229) == -2);
    CHECK(ap_shanks_mestre(Curve{0, 0, 0, 1, 1}, 233) == -3);
}

TEST_CASE("anchor_shanks_mestre_bad_reduction_refused") {
    // A3 — y²=x³+x+2 has 7 | (4A³+27B²) in the short model: bad reduction. REFUSED (throws),
    // demonstrated firing — regardless of p ≤ 229 (the disc check precedes the fallback).
    CHECK_THROWS_AS((void)ap_shanks_mestre(Curve{0, 0, 0, 1, 2}, 7), std::invalid_argument);
}

TEST_CASE("twin_shanks_mestre_vs_charsum") {
    // Small-range cross-ALGORITHM twin: Shanks–Mestre ≡ the frozen ap_charsum exactly, over
    // good primes p > 229 (the SM fast path) and several curves. Bad (curve,p) pairs (SM refuses)
    // are skipped — the twin is for good reduction. No cache, no 2^18 — Phase 2a scope.
    const std::vector<Curve> curves = {
        {0, 0, 0, -1, 0}, {0, 0, 0, 0, 1}, {0, 0, 0, 1, 1},
        {0, 0, 0, 2, -3}, {0, 0, 0, -4, 4}, {0, 0, 0, 5, 7}, {0, 0, 0, -7, 10},
    };
    // primes in (229, 1200]
    std::vector<u64> primes;
    for (u64 n = 230; n <= 1200; ++n) {
        bool pr = n > 1;
        for (u64 d = 2; d * d <= n; ++d) if (n % d == 0) { pr = false; break; }
        if (pr) primes.push_back(n);
    }
    int checked = 0, mismatches = 0;
    for (const Curve& E : curves) {
        for (u64 p : primes) {
            int sm;
            try {
                sm = ap_shanks_mestre(E, p);
            } catch (const std::exception&) {
                continue;                       // bad reduction at p — skip (twin is good-prime)
            }
            const int ref = ap_charsum(E, p);
            if (sm != ref) ++mismatches;
            ++checked;
        }
    }
    MESSAGE("Shanks-Mestre vs charsum: " << checked << " good (curve,p) pairs checked, "
            << mismatches << " mismatches");
    CHECK(mismatches == 0);
    CHECK(checked > 200);                        // the twin actually ran a broad range
}
