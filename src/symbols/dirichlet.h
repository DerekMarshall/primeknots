#pragma once
#include <complex>
#include <vector>

#include "core/int_types.h"

// Stage M2 — Dirichlet characters mod a prime, and their Gauss sums, from scratch
// (RESEARCH-M §4, m2-pinning). Characters are built via a primitive root: with g a
// primitive root mod p, χ_k(g) = e(k/(p−1)), so χ_k(a) = e(k·ind_g(a)/(p−1)) for
// gcd(a,p)=1 and 0 otherwise. For a prime modulus, χ_k is primitive ⟺ k ≠ 0 and
// even ⟺ k even (χ_k(−1) = (−1)^k). No external data.
namespace at::symbols {

using at::core::u64;

// Least primitive root mod an odd prime p (deterministic). Uses the factorization
// of p−1 + modpow order checks — twinned against the naive order iteration below.
u64 primitive_root(u64 p);

// Naive multiplicative order of g mod p (iterate powers until 1); the twin referee
// for primitive_root: g is a primitive root ⟺ order == p−1.
u64 mult_order(u64 g, u64 p);

// Index (discrete-log) table base g mod p: ind[a] = k with g^k ≡ a (mod p) for
// a ∈ [1,p); ind[0] = 0 (unused — χ(0) is handled as 0). Length p.
std::vector<u64> index_table(u64 p, u64 g);

// χ_k(a) ∈ ℂ using the index table (0 when p | a). order = p−1.
std::complex<double> chi_value(u64 k, u64 order, const std::vector<u64>& ind, u64 a);

// Gauss sum τ(χ_k) = Σ_{a=1}^{p−1} χ_k(a)·e(a/p), e(t) = exp(2πi t) (the pinned
// normalization, m2-pinning §P2). Summed in ascending a (deterministic).
std::complex<double> gauss_sum(u64 k, u64 p, u64 order, const std::vector<u64>& ind);

// χ_k(−1) = (−1)^k for a prime modulus: +1 (even) / −1 (odd).
inline int char_parity(u64 k) { return (k % 2 == 0) ? +1 : -1; }

}  // namespace at::symbols
