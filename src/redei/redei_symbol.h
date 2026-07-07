#pragma once
#include "core/int_types.h"
#include "redei/conic.h"

// The Rédei symbol [p1,p2,p3] for distinct primes ≡ 1 (mod 4), pairwise unlinked
// ((pi/pj)=1). Implemented from docs/notes/stage2-pinning.md (incl. riders
// R1–R3). Value in {+1,-1}; -1 ("triple linked") ⇔ F2 bit 1.
namespace at::redei {

// [p1,p2,p3] ∈ {+1,-1}. Preconditions (EC1–EC4) asserted. Picks a primitive
// conic solution with p3 ∤ z (R1) and applies the (2/p3)^w normalization (R3).
int redei_symbol(at::core::u64 p1, at::core::u64 p2, at::core::u64 p3);

// F2 bit of the symbol: 1 iff [p1,p2,p3] == -1.
inline int redei_bit(at::core::u64 p1, at::core::u64 p2, at::core::u64 p3) {
    return redei_symbol(p1, p2, p3) == -1 ? 1 : 0;
}

// The value computed from ONE primitive solution (x,y,z) and one square root
// s (s² ≡ p1 mod p3): ((x+y·s)/p3) · (2/p3)^w, with w = two_adic_class(x,y,p1).
// Returns 0 (degenerate) iff (x+y·s) ≡ 0 (mod p3), i.e. the p3|z case for this
// branch (R1). Exposed so invariance_* / branch tests can compare across
// solutions and both square roots.
int redei_value_from_solution(at::core::u64 p1, at::core::u64 p3, at::core::u64 s,
                              at::core::i128 x, at::core::i128 y);

}  // namespace at::redei
