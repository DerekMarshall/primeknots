#include "classgroup/redei_matrix.h"

#include <algorithm>

namespace at::classgroup {

using at::core::u64;
using at::linking::F2Matrix;
using at::linking::LinkingMatrix;

namespace {
std::size_t idx_of(const LinkingMatrix& lk, u64 p) {
    auto it = std::lower_bound(lk.primes.begin(), lk.primes.end(), p);
    return static_cast<std::size_t>(it - lk.primes.begin());  // primes assumed present
}
}  // namespace

F2Matrix redei_matrix(std::span<const u64> primes, const LinkingMatrix& lk) {
    std::size_t t = primes.size();
    F2Matrix R(t, t);
    for (std::size_t i = 0; i < t; ++i)
        for (std::size_t j = i + 1; j < t; ++j)
            if (lk.lk.get(idx_of(lk, primes[i]), idx_of(lk, primes[j]))) {
                R.set(i, j, true);  // off-diagonal = Stage 1 lk₂
                R.set(j, i, true);
            }
    for (std::size_t i = 0; i < t; ++i) {  // diagonal R_ii = Σ_{j≠i} R_ij
        bool s = false;
        for (std::size_t j = 0; j < t; ++j)
            if (j != i && R.get(i, j)) s = !s;
        R.set(i, i, s);
    }
    return R;
}

std::size_t fourrank_from_linking(std::span<const u64> primes,
                                  const LinkingMatrix& lk) {
    std::size_t t = primes.size();
    std::size_t r = redei_matrix(primes, lk).rank();
    // rows sum to 0 ⇒ rank ≤ t−1, so t−1−r ≥ 0.
    return (t == 0 ? 0 : t - 1) - r;
}

}  // namespace at::classgroup
