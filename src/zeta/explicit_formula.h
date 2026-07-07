#pragma once

#include <vector>

#include "core/int_types.h"

// Stage 5 — the Weil/von Mangoldt explicit formula, both sides (RESEARCH.md
// §7.2, rider R3, stage5-pinning.md §3). The identity
//
//   ψ₀(x) = x − Σ_ρ x^ρ/ρ − log 2π − ½ log(1 − x⁻²)
//
// relates the ORBIT side (primes: ψ(x) = Σ_{p^k≤x} log p) to the SPECTRAL side
// (a sum over zeta zeros ρ = ½ ± iγ). Conjugate zeros are combined analytically
// as 2·Re(x^ρ/ρ) — never summed as separate floating-point terms (rider R3).
//
// The series converges to the MIDPOINT-normalized ψ₀, not ψ: at a prime power
// x=p^k, ψ jumps by Λ(x)=log p, and ψ₀(x)=ψ(x)−Λ(x)/2. That midpoint is the
// reference for the error metric (open-questions R6).

namespace at::zeta {

// Direct Chebyshev ψ(x) = Σ_{p^k ≤ x} log p  (orbit side; right-continuous).
double psi_chebyshev(double x);

// Von Mangoldt Λ(x): log p if x is (numerically) a prime power p^k, else 0.
double von_mangoldt(double x);

// Midpoint-normalized reference ψ₀(x): ψ(x) off prime powers, ψ(x)−Λ(x)/2 at a
// prime power. This is what the spectral series converges to.
double psi0_reference(double x);

// Spectral reconstruction using the first M zero ordinates (gammas ascending):
//   x − Σ_{n<M} 2√x (½cos(γ_n L)+γ_n sin(γ_n L))/(¼+γ_n²) − log2π − ½log(1−x⁻²)
// with L = log x.  M is clamped to gammas.size().
double psi_reconstruct(double x, const std::vector<double>& gammas, std::size_t M);

}  // namespace at::zeta
