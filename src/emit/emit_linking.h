#pragma once
#include <cstddef>
#include <string>

#include "core/int_types.h"

namespace at::emit {

// Build the Stage 1 linking matrix over primes ≡ 1 (mod 4) ≤ prime_bound and
// write viewer JSON into out_dir (created if needed):
//   linking_matrix.json  — primes, base64 bit-rows, density/degree stats
//   linking_graph.json   — nodes/edges for the force layout (capped at
//                          graph_nodes leading primes, a "thresholded range")
// Both files carry the {schema, generated_by, params} envelope (ARCHITECTURE §5).
void emit_stage1(const std::string& out_dir, at::core::u64 prime_bound,
                 std::size_t graph_nodes);

}  // namespace at::emit
