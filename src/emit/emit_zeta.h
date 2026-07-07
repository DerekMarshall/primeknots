#pragma once
#include <string>

// Stage 5 emitters (ARCHITECTURE §5). Writes:
//   zeros.json               — computed zeros + Odlyzko delta + Turing certificate
//   psi_reconstruction.json  — ψ₀ staircase + per-zero-count reconstruction frames
//   dyn_zeta.json            — model-flow Ruelle zeta: orbit product vs determinant
// The Odlyzko path is a plain data file read only to annotate the viz delta (an
// empty path omits deltas); src/ still contains no oracle/correctness logic.
namespace at::emit {

void emit_stage5(const std::string& out_dir, double zeros_t_max,
                 const std::string& odlyzko_path, const std::string& generated_by);

}  // namespace at::emit
