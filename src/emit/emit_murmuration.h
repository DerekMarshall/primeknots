#pragma once
#include <cstddef>
#include <string>

// Stage M1 emitter (ARCHITECTURE-M §6). Writes murmuration_curve.json: the four
// HLOP Figure-1 panels (rank-classed conductor-family a_p means), the REQUIRE-able
// family counts, and the per-rank scale-collapse results with their null controls
// (riders R1/R2). Reads the pinned ecdata slices from `ecdata_dir`; src/ stays
// oracle-free (ecdata is committed data, not a live oracle call).
namespace at::emit {

void emit_m1(const std::string& out_dir, const std::string& ecdata_dir,
             std::size_t n_primes, const std::string& generated_by);

}  // namespace at::emit
