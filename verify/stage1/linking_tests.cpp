// Stage 1 — the linking matrix: reciprocity-as-symmetry at scale, the
// Legendre-twin cross-check, and build determinism under parallelism.
#include "doctest/doctest.h"

#include <algorithm>
#include <vector>

#include "core/primes.h"
#include "linking/linking_matrix.h"
#include "symbols/legendre.h"
#include "stage1/fixture.h"

using at::core::u64;
using at::linking::LinkingMatrix;
using at::symbols::legendre_recip;
using namespace at::verify::stage1;

// The vertex set itself: every label is a prime ≡ 1 (mod 4), strictly ascending,
// and the count matches the PARI π(x; 4, 1) value (refereed, not self-counted).
TEST_CASE("theorem_linking_vertex_set") {
    const LinkingMatrix& m = shared();
    REQUIRE(m.primes.size() == expected_primes());
    u64 cases = 0;
    for (std::size_t i = 0; i < m.primes.size(); ++i) {
        CHECK(m.primes[i] % 4 == 1);
        CHECK(at::core::is_prime(m.primes[i]));
        if (i > 0) CHECK(m.primes[i] > m.primes[i - 1]);
        ++cases;
    }
    MESSAGE("cases: " << cases << " primes ≡ 1 (mod 4) ≤ " << bound()
                      << " (== π(x;4,1) from gp)");
    REQUIRE(cases == expected_primes());
}

// Quadratic reciprocity at scale: build() computes lk(i,j) from (p_i / p_j) and
// lk(j,i) from (p_j / p_i) independently; asserting they agree over every pair
// is reciprocity (p_i/p_j) = (p_j/p_i) verified across the whole matrix. Also
// pins the zero diagonal.
TEST_CASE("theorem_quadratic_reciprocity_at_scale") {
    const LinkingMatrix& m = shared();
    const std::size_t n = m.primes.size();
    REQUIRE(n == expected_primes());
    u64 pairs = 0;
    for (std::size_t i = 0; i < n; ++i) {
        CHECK(m.lk.get(i, i) == false);  // no self-linking
        for (std::size_t j = i + 1; j < n; ++j) {
            CHECK(m.lk.get(i, j) == m.lk.get(j, i));  // reciprocity
            ++pairs;
        }
    }
    MESSAGE("cases: " << pairs << " symmetric pairs over " << n
                      << " primes ≤ " << bound() << " (reciprocity at scale)");
    REQUIRE(pairs == n * (n - 1) / 2);
}

// Structural symmetry of the as-built matrix: compare it word-for-word against
// its own transpose over the WHOLE matrix (no sampling). Unlike the pairwise
// reciprocity check above, this compares the bit-packed storage directly via
// operator==, so it also catches any stray bit in a row's padding words.
TEST_CASE("theorem_matrix_symmetry_bitrows") {
    const LinkingMatrix& m = shared();
    const std::size_t n = m.primes.size();
    REQUIRE(n == expected_primes());

    at::linking::F2Matrix t(n, n);
    for (std::size_t i = 0; i < n; ++i)
        for (std::size_t j = 0; j < n; ++j)
            if (m.lk.get(j, i)) t.set(i, j, true);
    REQUIRE(m.lk == t);  // M == Mᵀ, full bit-packed comparison

    // Degree symmetry as an independent structural witness: row i popcount ==
    // column i popcount, every row.
    std::size_t rows_checked = 0;
    for (std::size_t i = 0; i < n; ++i) {
        std::size_t col = 0;
        for (std::size_t k = 0; k < n; ++k)
            if (m.lk.get(k, i)) ++col;
        CHECK(m.lk.row_popcount(i) == col);
        ++rows_checked;
    }
    MESSAGE("cases: " << rows_checked << " bit-rows vs columns; M == Mᵀ over "
                      << n << "×" << n << " (" << n * n << " bits, no sampling)");
    REQUIRE(rows_checked == n);
}

// twin: the matrix was filled with the fast Legendre twin (legendre_euler, via
// modpow). Recompute a documented sample with the reciprocity twin
// (legendre_recip, via jacobi — no modpow) and require agreement. Sampling
// policy is recorded in docs/notes/stage1-sampling-policy.md: exhaustive over
// the leading K-prime block, plus a fixed deterministic random sample.
TEST_CASE("twin_linking_euler_vs_reciprocity") {
    const LinkingMatrix& m = shared();
    const std::size_t n = m.primes.size();
    u64 cases = 0;

    const std::size_t K = std::min<std::size_t>(n, 200);  // leading block, exhaustive
    for (std::size_t i = 0; i < K; ++i) {
        for (std::size_t j = 0; j < K; ++j) {
            if (i == j) continue;
            bool linked = legendre_recip(m.primes[i], m.primes[j]) == -1;
            CHECK(m.lk.get(i, j) == linked);
            ++cases;
        }
    }

    u64 state = 0x11'22'33'44'55'66'77'88ULL;
    const u64 samples = at::verify::g_extended ? 2'000'000ULL : 200'000ULL;
    u64 sampled = 0;
    while (sampled < samples) {
        std::size_t i = next_sample(state) % n;
        std::size_t j = next_sample(state) % n;
        if (i == j) continue;
        bool linked = legendre_recip(m.primes[i], m.primes[j]) == -1;
        CHECK(m.lk.get(i, j) == linked);
        ++cases;
        ++sampled;
    }
    MESSAGE("cases: " << cases << " entries recomputed with the reciprocity twin ("
                      << K << "-prime block exhaustive + " << samples
                      << " sampled)");
    REQUIRE(cases == K * (K - 1) + samples);
}

// The build is bit-identical regardless of thread count — a hard requirement
// once the fill is parallelized. Compare serial, hardware-parallel, and a fixed
// 8-thread build; all three matrices (and prime lists) must be identical.
TEST_CASE("invariance_build_parallel_vs_serial") {
    // Kept modest even under --extended: this builds three matrices.
    const u64 b = at::verify::g_extended ? 200'000ULL : 50'000ULL;
    LinkingMatrix serial = LinkingMatrix::build(b, 1);
    LinkingMatrix par_hw = LinkingMatrix::build(b, 0);  // hardware concurrency
    LinkingMatrix par_8 = LinkingMatrix::build(b, 8);

    REQUIRE(serial.primes == par_hw.primes);
    REQUIRE(serial.primes == par_8.primes);
    CHECK(serial.lk == par_hw.lk);  // bit-identical
    CHECK(serial.lk == par_8.lk);
    MESSAGE("cases: " << serial.primes.size()
                      << " primes; serial == parallel(hw) == parallel(8), "
                         "bit-identical");
    REQUIRE(serial.lk == par_hw.lk);
    REQUIRE(serial.lk == par_8.lk);
}
