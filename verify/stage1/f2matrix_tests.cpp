// Stage 1 — F2Matrix: rank (word-packed vs. naive dense referee) and submatrix
// extraction.
#include "doctest/doctest.h"

#include <cstddef>
#include <vector>

#include "linking/f2matrix.h"
#include "stage1/fixture.h"

using at::core::u64;
using at::linking::F2Matrix;
using at::verify::stage1::next_sample;

namespace {
// Naive dense F2 rank on a bool matrix — the frozen referee twin for the
// bit-packed F2Matrix::rank(). Obviously correct, never optimized.
std::size_t naive_rank(const std::vector<std::vector<int>>& a, std::size_t rows,
                       std::size_t cols) {
    std::vector<std::vector<int>> m = a;
    std::size_t r = 0;
    for (std::size_t c = 0; c < cols && r < rows; ++c) {
        std::size_t piv = r;
        while (piv < rows && m[piv][c] == 0) ++piv;
        if (piv == rows) continue;
        std::swap(m[piv], m[r]);
        for (std::size_t i = 0; i < rows; ++i) {
            if (i != r && m[i][c]) {
                for (std::size_t k = 0; k < cols; ++k) m[i][k] ^= m[r][k];
            }
        }
        ++r;
    }
    return r;
}
}  // namespace

// twin: bit-packed rank vs. naive dense rank over random small F2 matrices,
// plus pinned known cases (identity, zero, all-ones).
TEST_CASE("twin_f2rank_packed_vs_naive") {
    {  // identity has full rank
        F2Matrix id(5, 5);
        for (std::size_t i = 0; i < 5; ++i) id.set(i, i);
        CHECK(id.rank() == 5);
    }
    {  // zero matrix has rank 0
        F2Matrix z(7, 7);
        CHECK(z.rank() == 0);
    }
    {  // all-ones matrix has rank 1
        F2Matrix o(4, 6);
        for (std::size_t i = 0; i < 4; ++i)
            for (std::size_t j = 0; j < 6; ++j) o.set(i, j);
        CHECK(o.rank() == 1);
    }

    u64 state = 0xF2'F2'F2'F2'00'00'00'01ULL;
    const int trials = at::verify::g_extended ? 50'000 : 2'000;
    u64 cases = 0;
    for (int t = 0; t < trials; ++t) {
        std::size_t rows = 1 + next_sample(state) % 40;
        std::size_t cols = 1 + next_sample(state) % 100;  // spans multiple words
        F2Matrix m(rows, cols);
        std::vector<std::vector<int>> a(rows, std::vector<int>(cols, 0));
        for (std::size_t i = 0; i < rows; ++i) {
            for (std::size_t j = 0; j < cols; ++j) {
                if (next_sample(state) & 1ULL) {
                    m.set(i, j);
                    a[i][j] = 1;
                }
            }
        }
        CHECK(m.rank() == naive_rank(a, rows, cols));
        ++cases;
    }
    MESSAGE("cases: " << cases
                      << " random F2 matrices; rank(packed) == rank(naive)");
    REQUIRE(cases == static_cast<u64>(trials));
}

// Submatrix extraction: result(a,b) must equal the parent entry at
// (idx[a], idx[b]) for arbitrary (possibly repeated) index sets.
TEST_CASE("twin_submatrix_vs_direct_lookup") {
    u64 state = 0xABCD'1234'5678'9F01ULL;
    const int trials = at::verify::g_extended ? 20'000 : 1'000;
    u64 cases = 0;
    for (int t = 0; t < trials; ++t) {
        std::size_t n = 5 + next_sample(state) % 60;
        F2Matrix m(n, n);
        for (std::size_t i = 0; i < n; ++i)
            for (std::size_t j = 0; j < n; ++j)
                if (next_sample(state) & 1ULL) m.set(i, j);

        std::size_t k = 1 + next_sample(state) % n;
        std::vector<std::size_t> idx(k);
        for (std::size_t a = 0; a < k; ++a) idx[a] = next_sample(state) % n;

        F2Matrix s = m.submatrix(idx);
        REQUIRE(s.rows() == k);
        REQUIRE(s.cols() == k);
        for (std::size_t a = 0; a < k; ++a) {
            for (std::size_t b = 0; b < k; ++b) {
                CHECK(s.get(a, b) == m.get(idx[a], idx[b]));
                ++cases;
            }
        }
    }
    MESSAGE("cases: " << cases << " submatrix entries over " << trials
                      << " random extractions");
    REQUIRE(cases > 0);
}
