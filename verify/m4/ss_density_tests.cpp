// Stage M4 (Phase 2, step 1) — the Sawin–Sutherland density D(u), FORMULA side of the
// separate-TU protocol (m4-pinning §P3, [SS25] Conjecture 1 / eq. 2). Built from the
// in-house J₁ + generated constants + Lemma 3/4 local factors (rational + Chebyshev-U;
// eigenvalue sums vanish for weight <12 and are truncated above, folded into the
// density-evaluation tolerance).
//   anchor_ss_local_factors    — Lemma 3/4 spot values (ℓ̂_{p,2}; ℓ_{3,·} Chebyshev
//                                cancellations; ℓ_{p,ν}=0 for the empty eigenform sums).
//   theorem_ss_density_shape   — D(u) reproduces SS Fig. 1; the hump/zero/trough are
//                                extracted NUMERICALLY and COMMITTED as the R2 targets.
#include "doctest/doctest.h"

#include <cmath>
#include <string>

#include "harness.h"
#include "murm/ss_density.h"

using namespace at::murm;

TEST_CASE("anchor_ss_local_factors") {
    // ℓ̂_{p,2} (Lemma 4), hand-verified rationals:
    CHECK(std::abs(ss_ell_hat(2, 2) - (-3.9912)) < 1e-3);
    CHECK(std::abs(ss_ell_hat(3, 2) - (-1.49984)) < 1e-4);
    CHECK(std::abs(ss_ell_hat(5, 2) - (-1.21)) < 1e-3);       // −(5−.2+.04)/4
    CHECK(std::abs(ss_ell_hat(7, 2) - (-1.14626)) < 1e-4);
    // ℓ_{3,ν} (Lemma 3): the Chebyshev U_ν combination cancels to 0 at ν=1,2; ℓ_{3,0}=2/3.
    CHECK(std::abs(ss_ell(3, 0) - 2.0 / 3) < 1e-9);
    CHECK(std::abs(ss_ell(3, 1)) < 1e-9);
    CHECK(std::abs(ss_ell(3, 2)) < 1e-9);
    // ℓ_{p,ν} = 0 for p∈{2}∪{>3}, ν<10 (empty level-1 eigenform sum, weight <12).
    CHECK(ss_ell(2, 3) == 0.0);
    CHECK(ss_ell(5, 4) == 0.0);
    CHECK(ss_ell(7, 9) == 0.0);
    MESSAGE("anchor_ss_local_factors: ℓ̂_{p,2} rationals + ℓ_{3,ν} Chebyshev cancellations "
            "+ empty-eigenform ℓ_{p,ν}=0 (Lemmas 3,4) verified");
}

TEST_CASE("theorem_ss_density_shape") {
    // COMMITTED R2 targets — extracted NUMERICALLY from D(u) (never eyeballed from the
    // figure). Stable across truncation B∈[200,5000] to < the scan step. These are the
    // pass/fail targets the empirical statistic (step 3) is compared against.
    const double kHumpU = 0.475, kZeroU = 0.645, kTroughU = 0.805;
    const double kTol = 0.02;   // ≫ the <0.005 B-stability; formula side is deterministic

    const SSShape s = ss_shape(1000, 0.005);
    CHECK(std::abs(s.hump_u - kHumpU) < kTol);
    CHECK(std::abs(s.zero_u - kZeroU) < kTol);
    CHECK(std::abs(s.trough_u - kTroughU) < kTol);
    CHECK(s.hump_v > 3.5);       // hump ≈ +3.96 (Fig. 1 shows ≈ +4)
    CHECK(s.trough_v < -3.0);    // trough ≈ −3.40 (Fig. 1 shows ≈ −4)
    // SS Fig. 1 (X=2²⁸, B=10⁵) shows, over u=p/N∈(0,1]: a hump ≈0.4–0.5 (≈+4), a zero
    // ≈0.65, a trough ≈0.8–0.9 (≈−4) — our numerically-extracted {0.475,0.645,0.805}
    // and {+3.96,−3.40} fall in that band (the reproduction). The R1 error budget:
    // J₁ 6.4e-12 ≪ density-eval tol 0.056 (pointwise, B-truncation + eigenvalue-trunc)
    // ≪ this shape tol 0.02 (location) — instrument ≪ evaluation ≪ decision.
    MESSAGE("theorem_ss_density_shape: hump u=" << s.hump_u << " (D=" << s.hump_v
            << "), zero u=" << s.zero_u << ", trough u=" << s.trough_u << " (D=" << s.trough_v
            << ") — committed R2 targets {0.475,0.645,0.805}, reproduces SS Fig. 1");
}
