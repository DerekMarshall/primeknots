#pragma once
#include "core/int_types.h"

namespace at::core {

// Deterministic primality test. Miller–Rabin with the fixed witness set
// {2,3,5,7,11,13,17,19,23,29,31,37}, which is deterministic for all
// n < 3.317 × 10^24 (RESEARCH.md §2) — far beyond this project's range.
// The naive referee twin is a sieve of Eratosthenes, kept in the Stage 0
// verification suite rather than here.
bool is_prime(u64 n);

}  // namespace at::core
