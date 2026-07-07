#pragma once
#include <string>

#include "core/int_types.h"

namespace at::emit {

// Scan the restricted family (products of ≤4 distinct primes ≡ 1 (mod 4), with
// D ≤ prime_bound-derived cap), compute for each D the narrow class group two
// ways — form-side 4-rank (composition) and linking-side t−1−rank(R) — plus
// invariants, 2-rank, negative-Pell indicator, and ordinary h; write
// classgroups.json (ARCHITECTURE §5). The 4-rank scatter is the deliverable.
void emit_stage3(const std::string& out_dir, at::core::i128 disc_bound,
                 const std::string& generated_by);

}  // namespace at::emit
