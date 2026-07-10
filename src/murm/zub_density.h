#pragma once
#include <vector>

// Stage M3 — the Zubrilina weight-2 murmuration density M₂(y) (m3-pinning §P5), the
// FORMULA side of the separate-TU protocol. Authored independently of the trace-
// assembly (empirical) side; not edited after first comparison.
//   M₂(y) = α·Σ_{1≤r≤2√y} ν(r)·√(4y−r²) + β·√y − γ·y,   y = P/X
// with α,β,γ Euler products over primes and ν(r) a product over p|r (Z25 Theorem 1,
// read verbatim from the PDF). Derivative discontinuities (vertical-tangent onsets)
// at y = n²/4 (the anchor's teeth, §P5 / rider R4).
namespace at::murm {

// The weight-2 density at y (Euler products truncated to primes < prime_bound).
double zub_density_m2(double y, unsigned prime_bound = 100000);

// Derivative-discontinuity locator (rider R4): sample M₂ on [lo,hi] step h, return
// the y-locations where the discrete second difference spikes (vertical-tangent
// onsets). Expected {1/4, 1, 9/4} for the family.
std::vector<double> density_kink_locations(double lo, double hi, double h,
                                           unsigned prime_bound = 100000);

}  // namespace at::murm
