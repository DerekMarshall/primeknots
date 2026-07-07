#include "linking/linking_matrix.h"

#include <algorithm>
#include <thread>

#include "core/primes.h"
#include "symbols/legendre.h"

namespace at::linking {

using at::core::u64;

namespace {
std::vector<u64> primes_1mod4_up_to(u64 bound) {
    std::vector<u64> ps;
    for (u64 n = 5; n <= bound; n += 4) {  // n ≡ 1 (mod 4), n > 1
        if (at::core::is_prime(n)) ps.push_back(n);
    }
    return ps;
}
}  // namespace

LinkingMatrix LinkingMatrix::build(u64 prime_bound, unsigned threads) {
    LinkingMatrix m;
    m.primes = primes_1mod4_up_to(prime_bound);
    const std::size_t n = m.primes.size();
    m.lk = F2Matrix(n, n);
    if (n == 0) return m;

    // Fill every off-diagonal entry directly via the fast Legendre twin. Row i
    // touches only row i's storage, so partitioning rows across threads is
    // race-free and bit-identical to a single-threaded fill. Computing (i,j) and
    // (j,i) independently is deliberate: it makes the matrix's symmetry a real,
    // testable property rather than something imposed by mirroring.
    auto fill_rows = [&m, n](std::size_t lo, std::size_t hi) {
        for (std::size_t i = lo; i < hi; ++i) {
            u64 pi = m.primes[i];
            for (std::size_t j = 0; j < n; ++j) {
                if (i == j) continue;
                if (at::symbols::legendre_euler(pi, m.primes[j]) == -1)
                    m.lk.set(i, j, true);  // (p_i / p_j) = -1  <=>  linked
            }
        }
    };

    unsigned hw = threads == 0 ? std::max(1u, std::thread::hardware_concurrency())
                               : threads;
    if (hw <= 1 || n < 64) {
        fill_rows(0, n);
    } else {
        std::vector<std::thread> pool;
        std::size_t chunk = (n + hw - 1) / hw;
        for (unsigned t = 0; t < hw; ++t) {
            std::size_t lo = std::min<std::size_t>(static_cast<std::size_t>(t) * chunk, n);
            std::size_t hi = std::min<std::size_t>(lo + chunk, n);
            if (lo < hi) pool.emplace_back(fill_rows, lo, hi);
        }
        for (std::thread& th : pool) th.join();
    }
    return m;
}

}  // namespace at::linking
