#pragma once
#include <vector>

#include "classgroup/qform.h"
#include "core/int_types.h"

// The narrow class group Cl⁺(D) of a real quadratic field (D>0 fundamental),
// computed from binary quadratic forms under Gauss composition (= the narrow
// group; see docs/notes/stage3-pinning.md §1). "Homology side" of the
// Rédei–Reichardt comparison.
namespace at::classgroup {

struct ClassGroup {
    at::core::i128 D = 0;
    std::vector<QForm> elements;  // canonical reps of all h⁺ classes

    at::core::i128 order() const { return static_cast<at::core::i128>(elements.size()); }

    // 2^k-ranks over F₂ (Stevenhagen [S22] §2): r_{2^k} = dim C[2^k]/C[2^{k−1}].
    std::size_t rank2() const;  // = #even invariant factors  (genus: t−1)
    std::size_t rank4() const;  // = #invariant factors divisible by 4

    // Full Smith-normal-form invariant factors (ascending), for the bnfnarrow
    // structure match. Heavier (needs element orders); use on a sample.
    std::vector<at::core::i128> invariant_factors() const;

    static ClassGroup compute(at::core::i128 D);
};

}  // namespace at::classgroup
