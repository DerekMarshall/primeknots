#pragma once
#include <string>

#include "core/int_types.h"

namespace at::emit {

// Scan all pairwise-unlinked triples of primes ≡ 1 (mod 4) ≤ prime_bound,
// compute the Rédei symbol of each, and write borromean.json into out_dir:
// the verified triple table (with symbol values), the primes vertex set, and
// invariance-test provenance (ARCHITECTURE §5). Borromean triples are those
// with symbol −1 (pairwise unlinked, triple-linked).
void emit_stage2(const std::string& out_dir, at::core::u64 prime_bound,
                 const std::string& generated_by);

}  // namespace at::emit
