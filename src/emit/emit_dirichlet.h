#pragma once
#include <string>

// Stage M2 emitter (ARCHITECTURE-M §6). Writes dirichlet_murmuration.json: the LOP
// murmuration curves (sharp + geometric) with their pinned densities, the parity
// structure, the convergence trend (empiric), and the cross-parity null. ZERO
// external data — the ARCHITECTURE-M §9 repo-reproducible default, named as such in
// the params.
namespace at::emit {

void emit_m2(const std::string& out_dir, const std::string& generated_by);

}  // namespace at::emit
