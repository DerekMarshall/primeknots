#pragma once
#include "core/int_types.h"

// Stage M3 — dimensions of weight-2 cusp-form spaces for Γ₀(N) (m3-pinning §P2 /
// the M3-completion normalization). This is the DENOMINATOR of the murmuration
// statistic (Σ_N dim S₂^new(N)); it is a normalization, not plumbing, so it is
// pinned and adjudicated against LMFDB/PARI independently of any eigenvalue.
//
// dim S₂(Γ₀(N)) = genus g(X₀(N)) (weight 2), the standard formula
//   g = 1 + μ/12 − ν₂/4 − ν₃/3 − ν∞/2                       (Diamond–Shurman, Ch. 3)
// with the index μ = [PSL₂(ℤ):Γ₀(N)] = N·∏_{p|N}(1+1/p), the elliptic-point counts
//   ν₂ = 0 if 4|N else ∏_{p|N}(1+(−4|p)),  ν₃ = 0 if 9|N else ∏_{p|N}(1+(−3|p)),
// (Kronecker symbols: (−4|2)=0, (−3|3)=0, (−3|2)=−1 — the ONLY special cases; for
// square-free N never 4|N or 9|N), and the cusp count ν∞ = Σ_{d|N} φ(gcd(d,N/d))
// (= 2^{ω(N)} for square-free N). dim S₂^new is the Atkin–Lehner–Li new subspace,
// recovered by Möbius inversion of the oldform-multiplicity σ₀ (β = μ ∗ μ):
//   dim S₂^new(N) = Σ_{d|N} β(N/d)·dim S₂(Γ₀(d)),  β(1)=1, β(p)=−2, β(p²)=1, β(p^k≥3)=0.
namespace at::mform {

using at::core::i64;

// Genus of X₀(N) = dim_ℂ S₂(Γ₀(N)) (the full cusp space, weight 2). N ≥ 1.
i64 dim_s2_gamma0(i64 N);

// dim_ℂ S₂^new(Γ₀(N)) — the new subspace (Atkin–Lehner). N ≥ 1.
i64 dim_s2_new(i64 N);

}  // namespace at::mform
