#pragma once
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace at::linking {

// Dense matrix over F2 (GF(2)), rows bit-packed into 64-bit words so row
// operations are word-parallel (ARCHITECTURE.md §2). Column j of row i lives in
// bit (j % 64) of word (j / 64) of that row.
class F2Matrix {
public:
    F2Matrix() = default;
    F2Matrix(std::size_t rows, std::size_t cols);

    std::size_t rows() const { return rows_; }
    std::size_t cols() const { return cols_; }

    bool get(std::size_t i, std::size_t j) const;
    void set(std::size_t i, std::size_t j, bool value = true);

    std::size_t row_popcount(std::size_t i) const;  // set bits in row i
    std::size_t popcount() const;                    // set bits in the matrix

    // Rank over F2 by Gaussian elimination, word-parallel. Non-mutating: it
    // reduces a copy.
    std::size_t rank() const;

    // Principal submatrix on an index set: result(a,b) = (*this)(idx[a], idx[b]).
    F2Matrix submatrix(std::span<const std::size_t> idx) const;

    bool operator==(const F2Matrix& other) const;
    bool operator!=(const F2Matrix& other) const { return !(*this == other); }

    // Raw word access for callers doing their own word-parallel work / emission.
    const std::uint64_t* row_words(std::size_t i) const { return &data_[i * stride_]; }
    std::size_t stride() const { return stride_; }  // words per row

private:
    std::size_t rows_ = 0, cols_ = 0, stride_ = 0;
    std::vector<std::uint64_t> data_;
};

}  // namespace at::linking
