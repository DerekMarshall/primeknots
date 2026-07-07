#pragma once
#include <cstddef>
#include <span>

#include "core/int_types.h"
#include "linking/f2matrix.h"
#include "linking/linking_matrix.h"

// The Rédei matrix and the linking-side 4-rank prediction (Rédei–Reichardt).
// "Linking side" of the Stage 3 comparison — consumes the Stage 1 linking data.
// Diagonal convention pinned in docs/notes/stage3-pinning.md §2.
namespace at::classgroup {

// R over F₂ for primes p1<...<pt (≡1 mod 4): off-diagonal R_ij = lk₂(pi,pj) from
// the Stage 1 LinkingMatrix; diagonal R_ii = Σ_{j≠i} R_ij (so every row sums to
// 0 — asserted, not assumed, by verify_stage3).
at::linking::F2Matrix redei_matrix(std::span<const at::core::u64> primes,
                                   const at::linking::LinkingMatrix& lk);

// Rédei–Reichardt prediction: 4-rank of Cl⁺(D) = t − 1 − rank_{F₂}(R).
std::size_t fourrank_from_linking(std::span<const at::core::u64> primes,
                                  const at::linking::LinkingMatrix& lk);

}  // namespace at::classgroup
