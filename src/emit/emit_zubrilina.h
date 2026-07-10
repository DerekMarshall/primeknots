#pragma once
#include <string>

// Stage M3 (completion) emitter (ARCHITECTURE-M §6). Writes zubrilina_murmuration.json:
// the empirical short-interval murmuration (per-prime scatter, assembled from the
// class-number trace formula) overlaid on the M₂(y) density formula, with residuals,
// the empirical/formula kink teeth (derivative discontinuities at y=n²/4), the L2
// distance, and the convergence trend over X. ZERO external eigenvalue data — the
// ARCHITECTURE-M §9 repo-reproducible default (Hurwitz class numbers → trace formula
// → averaged; dimensions via the genus/Möbius normalization).
namespace at::emit {

void emit_m3(const std::string& out_dir, const std::string& generated_by);

}  // namespace at::emit
