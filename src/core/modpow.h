#pragma once
#include "core/int_types.h"

namespace at::core {

// --- Naive referee twins (FROZEN, per ARCHITECTURE.md §2) ------------------
// Direct schoolbook modular arithmetic via u128. Obviously correct; never
// optimized. These are the ground truth the fast path is checked against.

// (a * b) mod m, computed directly through a 128-bit product.
u64 modmul(u64 a, u64 b, u64 m);

// base^exp mod m by binary exponentiation using modmul. Convention: 0^0 = 1.
u64 modpow_naive(u64 base, u64 exp, u64 m);

// --- Fast path -------------------------------------------------------------
// base^exp mod m. Uses Montgomery multiplication internally for odd m
// (ARCHITECTURE.md §2), and falls back to the direct path for even m.
// Convention: 0^0 = 1.
u64 modpow(u64 base, u64 exp, u64 m);

}  // namespace at::core
