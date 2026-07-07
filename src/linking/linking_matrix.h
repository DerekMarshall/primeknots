#pragma once
#include <vector>

#include "core/int_types.h"
#include "linking/f2matrix.h"

namespace at::linking {

// The mod-2 linking matrix of primes ≡ 1 (mod 4) (RESEARCH.md §3).
//
// For distinct primes p_i, p_j ≡ 1 (mod 4), the entry lk(i,j) ∈ F2 is defined by
//     (p_i / p_j) = (-1)^{lk(i,j)},
// i.e. lk(i,j) = 1 when the Legendre symbol is -1 (the primes are "linked").
// Quadratic reciprocity forces lk(i,j) = lk(j,i); the matrix's symmetry is
// *verified*, not assumed — build() computes each ordered entry from its own
// direction, and verify_stage1 asserts the two agree across the whole matrix.
//
// The diagonal is 0: the linking adjacency has no self-loops. NOTE: this is not
// the Rédei matrix — that is a distinct Stage 3 object whose diagonal is pinned
// to a source and is out of scope here.
struct LinkingMatrix {
    std::vector<at::core::u64> primes;  // primes ≡ 1 (mod 4), ascending
    F2Matrix lk;                        // symmetric, diagonal 0

    // Build over all primes ≡ 1 (mod 4) up to prime_bound (default 1e6 at the
    // call sites that emit).
    //   threads == 0 : use hardware concurrency
    //   threads == 1 : force single-threaded
    // The fill is row-partitioned; because each entry is a pure function of
    // (p_i, p_j) and each thread writes only its own rows, the result is
    // bit-identical for any thread count (verify_stage1 proves it).
    static LinkingMatrix build(at::core::u64 prime_bound, unsigned threads = 0);
};

}  // namespace at::linking
