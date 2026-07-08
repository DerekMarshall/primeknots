#pragma once

#include <vector>

#include "core/int_types.h"

// Stage 6 — the untwisted S₃ Dijkgraaf–Witten invariant of X = Spec ℤ[1/S]
// (RESEARCH.md §8, docs/notes/stage6-pinning.md). Assembles
//
//   |Hom(π₁ᵉᵗ(X), S₃)| = 1 + 3(2^t − 1) + (3^k − 1) + 6c
//   Z_DW = |Hom| / 6  = 1/6 + (2^t − 1)/2 + (3^k − 1)/6 + c   (exact rational)
//
// from our own machinery for the t (quadratic/C₂) and k (cyclic-cubic/C₃) terms
// and the referee-supplied c (S₃ term, # non-Galois cubic iso classes with
// rad(disc) ⊆ S). t = |S|, k = #{p ∈ S : p ≡ 1 mod 12}. c is 6·per R1.

namespace at::dw {

// Exact reduced rational (den > 0). Small values only (groupoid cardinalities).
struct Rational {
    long long num = 0;
    long long den = 1;
};
Rational rational(long long num, long long den);   // reduces sign + gcd
Rational add(Rational a, Rational b);
bool equal(Rational a, Rational b);

// Decomposition terms of |Hom| (H2, corrected per R1).
long long trivial_term();                 // 1
long long c2_term(int t);                  // 3·(2^t − 1)   (3 conjugate C₂ subgroups)
long long c3_term(int k);                  // 3^k − 1       (unique A₃, no multiplicity)
long long s3_term(long long c);            // 6·c           (|Aut S₃|=6 per closure/iso class)

long long hom_count(int t, int k, long long c);   // 1 + c2 + c3 + s3
Rational z_dw(int t, int k, long long c);          // hom_count / 6, reduced

// Mass formula RHS, assembled as a sum of rationals by a DIFFERENT path than
// z_dw (for the exact-arithmetic cross-check theorem, R2):
//   1/6 + (2^t − 1)/2 + (3^k − 1)/6 + c
Rational mass_formula_rhs(int t, int k, long long c);

// k = #{p ∈ S : p ≡ 1 (mod 12)}  (the cyclic-cubic term's prime count, H5).
int cyclic_k(const std::vector<at::core::u64>& S);

}  // namespace at::dw
