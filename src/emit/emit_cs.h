#pragma once
#include <string>

#include "core/int_types.h"

namespace at::emit {

// Scan the pell_neg-gated family (products of 2..4 distinct primes ≡ 1 (mod 4),
// N(ε)=−1) and write cs_partition.json: per prime set, the mod-2 arithmetic DW
// partition function computed both ways (LHS brute character sum, RHS closed
// form), the match flag, and the row-parity ("phase") class; plus aggregate
// strata and the pell exclusion count. (ARCHITECTURE §5, Stage 4.)
void emit_stage4(const std::string& out_dir, at::core::u64 prime_bound,
                 const std::string& generated_by);

}  // namespace at::emit
