#pragma once
#include <vector>

#include "core/int_types.h"

// Stage M4 — the height-ordered elliptic-curve family (m4-pinning §P1/§P5), the
// ordering axis Sawin–Sutherland use. E_{A,B}: y² = x³ + Ax + B, integers A,B,
// **reduced** (no prime p with p⁴|A AND p⁶|B — the unique minimal representative of
// each isomorphism class), nonsingular (4A³+27B² ≠ 0). Naive height
//   H(E) := max(4|A|³, 27B²)   (quoted verbatim from [SS25] p.1; do NOT vary).
// The ordering is oracle-free (from (A,B) alone); the STATISTIC's conductor/root
// number are oracle-supplied (§P4/C3), handled elsewhere.
namespace at::murm {

using at::core::i64;

struct HeightCurve { i64 A, B; };   // reduced, nonsingular short Weierstrass model

// H(E) = max(4|A|³, 27B²).
i64 naive_height(i64 A, i64 B);

// The family {E_{A,B} : H(E) ≤ X}, reduced + nonsingular. Enumerated DIRECTLY:
// iterate the box |A| ≤ (X/4)^{1/3}, |B| ≤ (X/27)^{1/2}, filter per-curve.
std::vector<HeightCurve> height_family(i64 X);

// The SAME count by an independent algorithm (m4-pinning §P5 / rider R4): a sieve
// that marks non-reduced (A,B) as multiples of (p⁴,p⁶), then counts unmarked
// nonsingular cells. Different algorithm from the per-curve trial-division filter —
// the two must agree at every cutoff (the certified count).
i64 height_family_count_sieve(i64 X);

}  // namespace at::murm
