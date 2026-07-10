#pragma once
#include "core/int_types.h"

// Stage M3 — Hurwitz class numbers H(n) (Zubrilina's H₁(−n)), the class-number
// ingredient of the Eichler–Selberg trace formula (m3-pinning §P4). Definition
// pinned from [Z25] p.6: H₁(−d) is the number of SL₂(ℤ)-classes of positive-definite
// binary quadratic forms of discriminant −d, weighted by 2/|Aut| — forms that are
// multiples of x²+y² (disc −4·□) count 1/2, multiples of x²+xy+y² (disc −3·□) count
// 1/3; H(0) = −1/12; H(n) = 0 for n ≡ 1,2 (mod 4). H(n) ∈ ℚ (denominators 1,2,3,12).
//
// Shares the binary-quadratic-forms MATHEMATICS with primeknots Stage 3 — the
// 1801 machinery under 2025 mathematics — but the code is separate: Stage 3 reduces
// INDEFINITE forms (D>0, real quadratic, cycle-based); Hurwitz needs POSITIVE-DEFINITE
// reduction (D<0), implemented here (−a < b ≤ a ≤ c). Conceptual reuse, not a shared
// reduction routine (m3-pinning §P4 records the distinction).
namespace at::mform {

using at::core::i64;

// A reduced rational (den > 0, gcd(|num|,den) = 1).
struct Frac {
    i64 num = 0, den = 1;
    bool operator==(const Frac& o) const { return num == o.num && den == o.den; }
};
Frac frac(i64 num, i64 den);
Frac add(const Frac& a, const Frac& b);

// Two independent algorithms (Phase-1 twin):
Frac hurwitz_by_forms(i64 n);          // (A) direct weighted count of all reduced forms
Frac hurwitz_by_decomposition(i64 n);  // (B) Σ_{f²|n} weighted PRIMITIVE class number

// The referee (= method A). Do not optimize; the fast large-argument path is M3
// Phase 2's problem, checked against this.
Frac hurwitz(i64 n);

}  // namespace at::mform
