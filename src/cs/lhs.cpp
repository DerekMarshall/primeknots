#include "cs/lhs.h"

#include <vector>

#include "symbols/legendre.h"

namespace at::cs {

long lhs_partition(std::span<const at::core::u64> primes) {
    const std::size_t r = primes.size();

    // Degenerate cases: T+ has dimension r-1. With r == 0 there is nothing to
    // sum over as a pair complex; treat the empty product as the single trivial
    // character contributing +1. With r == 1, T+ = {0}: one character, phase 0.
    if (r <= 1) {
        return 1;
    }

    // Precompute the mod-2 linking numbers lk2(i, j) for i < j:
    //   lk2 = 0 if (p_i / p_j) == +1, else 1 (i.e. when the symbol is -1).
    // Stored as a full symmetric r x r matrix for O(1) lookup in the hot loop.
    std::vector<std::vector<int>> lk2(r, std::vector<int>(r, 0));
    for (std::size_t i = 0; i < r; ++i) {
        for (std::size_t j = i + 1; j < r; ++j) {
            const int sym = at::symbols::legendre_euler(primes[i], primes[j]);
            const int v = (sym == 1) ? 0 : 1;
            lk2[i][j] = v;
            lk2[j][i] = v;
        }
    }

    // Enumerate every character by brute force. Represent a character by a
    // vector c in (Z/2)^r acting as rho(x) = sum_i c_i x_i. On the pair-vector
    // e+_{ij} (ones in positions i and j) this gives rho(e+_{ij}) = c_i XOR c_j.
    // The vectors c and c XOR (1,...,1) induce the SAME character on the
    // sum-zero subspace T+, so summing over all 2^r vectors c double-counts the
    // 2^{r-1} genuine characters; we divide the accumulated total by 2 at the
    // end.
    const unsigned long num_c = 1UL << r;
    long total = 0;
    for (unsigned long c = 0; c < num_c; ++c) {
        // CS(c) = sum_{i<j} (c_i XOR c_j) * lk2(i, j)  (mod 2)
        int cs = 0;
        for (std::size_t i = 0; i < r; ++i) {
            const int ci = static_cast<int>((c >> i) & 1UL);
            for (std::size_t j = i + 1; j < r; ++j) {
                const int cj = static_cast<int>((c >> j) & 1UL);
                cs ^= (ci ^ cj) & lk2[i][j];
            }
        }
        total += (cs == 0) ? 1 : -1;
    }

    return total / 2;
}

}  // namespace at::cs
