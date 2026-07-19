// Stage M4 (Phase 2, step 1) — the Sawin–Sutherland density D(u), FORMULA side of the
// separate-TU protocol (m4-pinning §P3, [SS25] Conjecture 1 / eq. 2). Built from the
// in-house J₁ + generated constants + Lemma 3/4 local factors, with the eigenvalue sums
// Tr(T_p|S_k) from a from-scratch level-1 Eichler–Selberg trace (reusing M3 Hurwitz).
//   anchor_ss_local_factors            — Lemma 3/4 spot values.
//   anchor_hecke_trace_eichler_selberg — level1_hecke_trace vs published Ramanujan τ.
//   anchor_ss_density_survivor_terms   — term-level re-assembly of the CORRECTED eq (2)
//                                        products (referee C): structural survivors +
//                                        gcd(m,q)>1 μ cases.
//   theorem_ss_density_shape           — D(u) reproduces SS Fig. 1; hump/zero/trough are
//                                        the (corrected) R2 targets.
#include "doctest/doctest.h"

#include <cmath>
#include <string>

#include "core/bessel.h"
#include "core/constants.h"
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
    // ℓ_{p,ν} = 0 for p∈{2}∪{>3}, ν<10 (empty level-1 eigenform sum, weight ν+2<12) —
    // still true under the Eichler–Selberg trace (it is the weight<12 vanishing, exact).
    CHECK(ss_ell(2, 3) == 0.0);
    CHECK(ss_ell(5, 4) == 0.0);
    CHECK(ss_ell(7, 9) == 0.0);
    MESSAGE("anchor_ss_local_factors: ℓ̂_{p,2} rationals + ℓ_{3,ν} Chebyshev cancellations "
            "+ empty-eigenform ℓ_{p,ν}=0 (Lemmas 3,4) verified");
}

TEST_CASE("anchor_hecke_trace_eichler_selberg") {
    // level1_hecke_trace(k,p) = Tr(T_p | S_k(SL₂ℤ)) via the from-scratch level-1
    // Eichler–Selberg trace (ss_density.cpp), reusing M3 Hurwitz class numbers — the
    // load-bearing upgrade of referee C (the old code truncated weight≥12 to 0). Anchored
    // on the published Ramanujan τ(p) = Tr(T_p | S₁₂) (OEIS A000594) and the vanishing
    // cases (odd weight; no level-1 cusp forms below weight 12; dim S₁₄ = 0).
    CHECK(level1_hecke_trace(12, 2) == -24);      // τ(2)
    CHECK(level1_hecke_trace(12, 3) == 252);      // τ(3)
    CHECK(level1_hecke_trace(12, 5) == 4830);     // τ(5)
    CHECK(level1_hecke_trace(12, 7) == -16744);   // τ(7)
    CHECK(level1_hecke_trace(10, 2) == 0);        // weight < 12: no level-1 cusp forms
    CHECK(level1_hecke_trace(11, 3) == 0);        // odd weight
    CHECK(level1_hecke_trace(14, 2) == 0);        // dim S₁₄ = 0 (the case the truncation hid)
    MESSAGE("anchor_hecke_trace_eichler_selberg: τ(2,3,5,7)=Tr(T_p|S₁₂) + vanishing verified");
}

TEST_CASE("anchor_ss_density_survivor_terms") {
    // Term-level re-assembly of the CORRECTED eq (2) products (referee C): ss_density_term
    // must place ℓ̂ on p|q and ℓ on p|m,p∤q (BOTH exponent 2v_p(m)), with the
    // μ(gcd)/(q·m·φ(q/gcd)) coefficient. Checked against a hand re-assembly from the public
    // local factors + in-house J₁. This is the assignment the swapped reading got wrong.
    const double u = 0.87;
    const double f = 4.0 * at::core::kPi * std::sqrt(u);

    // (a) m=32=2⁵, q=1: coprime; p=2∤q, 2v₂(m)=10 → ℓ_{2,10} (needs Tr T₂|S₁₂=−24, which
    //     the old truncation zeroed). No ℓ̂ (q=1).
    const double ea = ss_ell(2, 10);
    CHECK(ea != 0.0);
    CHECK(std::abs(ss_density_term(u, 1, 32) - (1.0 / 32.0) * ea * at::core::bessel_j1(f * 32.0)) < 1e-12);

    // (b) m=3⁵=243, q=1: p=3 branch, ℓ_{3,10} (Chebyshev + Tr T₃|S₁₂ correction).
    const double eb = ss_ell(3, 10);
    CHECK(std::abs(ss_density_term(u, 1, 243) - (1.0 / 243.0) * eb * at::core::bessel_j1(f * 243.0)) < 1e-12);

    // (c) m=5⁵=3125, q=1: 10-powerful p>3, ℓ_{5,10} = −pref·Tr T₅|S₁₂ (=4830).
    const double ec = ss_ell(5, 10);
    CHECK(ec != 0.0);
    CHECK(std::abs(ss_density_term(u, 1, 3125) - (1.0 / 3125.0) * ec * at::core::bessel_j1(f * 3125.0)) < 1e-12);

    // (d) gcd(m,q)=6>1, μ(6)=+1: q=6, m=12=2²·3. Both 2,3 | q → ℓ̂_{2,4}·ℓ̂_{3,2}; NO ℓ
    //     (no prime of m is coprime to q). φ(6/6)=φ(1)=1.
    const double ed = (1.0 / (6.0 * 12.0 * 1.0)) * ss_ell_hat(2, 4) * ss_ell_hat(3, 2)
                      * at::core::bessel_j1(f * 12.0 / 6.0);
    CHECK(std::abs(ss_density_term(u, 6, 12) - ed) < 1e-12);

    // (e) gcd(m,q)=2>1, μ(2)=−1: q=6, m=2. p=2|q → ℓ̂_{2,2}; p=3|q,∤m → ℓ̂_{3,0} (the ν=0
    //     factor our swapped reading never used); no ℓ. φ(6/2)=φ(3)=2. Tests the μ sign.
    const double ee = (-1.0 / (6.0 * 2.0 * 2.0)) * ss_ell_hat(2, 2) * ss_ell_hat(3, 0)
                      * at::core::bessel_j1(f * 2.0 / 6.0);
    CHECK(std::abs(ss_density_term(u, 6, 2) - ee) < 1e-12);

    MESSAGE("anchor_ss_density_survivor_terms: m=32/3⁵/5⁵ survivors + gcd(m,q)>1 μ=±1 "
            "re-assemble exactly (ℓ̂ on p|q, ℓ on p|m,p∤q, exponent 2v_p(m))");
}

TEST_CASE("theorem_ss_density_shape") {
    // CORRECTED R2 targets (referee C, ERRATA #28) — extracted NUMERICALLY from D(u).
    // The earlier {0.475,0.645,0.805} came from the swapped-product misreading; the
    // corrected eq (2) gives {0.465,0.671,0.870}, matching SS Fig. 1 (digitized
    // {0.466,0.67,0.87}) on all three features and the empirical trough (~0.886).
    const double kHumpU = 0.465, kZeroU = 0.671, kTroughU = 0.870;
    const double kTol = 0.02;

    const SSShape s = ss_shape(1000, 0.005);
    CHECK(std::abs(s.hump_u - kHumpU) < kTol);
    CHECK(std::abs(s.zero_u - kZeroU) < kTol);
    CHECK(std::abs(s.trough_u - kTroughU) < kTol);
    CHECK(s.hump_v > 4.0);       // hump ≈ +4.78
    CHECK(s.trough_v < -3.5);    // trough ≈ −4.06 (Fig. 1 shows ≈ −4)
    MESSAGE("theorem_ss_density_shape: hump u=" << s.hump_u << " (D=" << s.hump_v
            << "), zero u=" << s.zero_u << ", trough u=" << s.trough_u << " (D=" << s.trough_v
            << ") — CORRECTED R2 targets {0.465,0.671,0.870}, reproduces SS Fig. 1");
}
