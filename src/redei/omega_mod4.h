#pragma once
#include <array>

#include "core/int_types.h"

// 2-adic square-class arithmetic in O = Z[ω], ω = (1+√p1)/2, for p1 ≡ 1 (mod 4)
// (ω² = ω + m, m = (p1−1)/4). Underlies the Rédei-symbol N4 normalization; see
// docs/notes/stage2-pinning.md §8 R3. Residue classes a+bω mod 4 are encoded as
// index 4a+b with a,b ∈ {0,1,2,3}.
namespace at::redei {

// The 16-entry table: square_classes(p1)[4A+B] is true iff (A+Bω) is a square in
// O/4O. Computed by squaring all 16 classes via ω²=ω+m (closed form).
std::array<bool, 16> square_classes_mod4(at::core::u64 p1);

// Twin (independent code path): squares all 16 classes via a generic O/4
// multiply instead of the closed form. Used by the R3 twin test.
std::array<bool, 16> square_classes_mod4_via_mul(at::core::u64 p1);

// Is (A + Bω) a square in O/4O? (A,B reduced mod 4, negatives handled.)
bool is_square_mod4(at::core::i128 A, at::core::i128 B, at::core::u64 p1);

// The N4 correction exponent w ∈ {0,1}: redei_symbol multiplies (·/p3) by
// (2/p3)^w. β = x + y√p1 (so (A,B) = (x−y, 2y) in O). Algorithm: peel factors of
// 2 (each flips w) to a 2-unit β'; then w flips unless β' or −β' is a square
// mod 4. (pinning log §8 R3.)
int two_adic_class(at::core::i128 x, at::core::i128 y, at::core::u64 p1);

}  // namespace at::redei
