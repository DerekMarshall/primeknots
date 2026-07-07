#pragma once
#include <optional>

#include "core/int_types.h"

namespace at::symbols {

// Square root modulo an odd prime p (Tonelli–Shanks).
//   - returns some r with r^2 ≡ a (mod p) when a is a quadratic residue
//     (including r = 0 when a ≡ 0);
//   - returns std::nullopt when a is a non-residue.
// Non-residues are *detected* (via the Euler criterion) and cause an early
// nullopt return — the routine never loops on them (RESEARCH.md §2 caveat).
std::optional<at::core::u64> sqrt_mod(at::core::u64 a, at::core::u64 p);

}  // namespace at::symbols
