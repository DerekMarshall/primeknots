#include "cs/rhs.h"

#include "symbols/legendre.h"

namespace at::cs {

// See rhs.h for the mathematical definition. This is the closed form: no
// character enumeration, no brute-force sum.
long rhs_partition(std::span<const at::core::u64> primes) {
  const std::size_t r = primes.size();
  if (r == 0) {
    // Empty product: 2^{r-1} degenerates; the DW invariant of no primes is 1.
    return 1L;
  }

  // lk2(p_i, p_j): 0 when (p_i / p_j) == +1, 1 when == -1.
  auto lk2 = [](at::core::u64 a, at::core::u64 p) -> int {
    return at::symbols::legendre_euler(a, p) == -1 ? 1 : 0;
  };

  for (std::size_t i = 0; i < r; ++i) {
    int row_sum = 0;  // parity accumulator
    for (std::size_t j = 0; j < r; ++j) {
      if (j == i) continue;
      row_sum ^= lk2(primes[i], primes[j]);
    }
    if (row_sum != 0) {
      return 0L;  // some row sum is odd → invariant vanishes
    }
  }

  // All row sums even → Z = 2^{r-1}.
  return 1L << (r - 1);
}

}  // namespace at::cs
