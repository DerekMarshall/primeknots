#include "linking/f2matrix.h"

#include <algorithm>
#include <utility>

namespace at::linking {

F2Matrix::F2Matrix(std::size_t rows, std::size_t cols)
    : rows_(rows),
      cols_(cols),
      stride_((cols + 63) / 64),
      data_(rows * ((cols + 63) / 64), 0ULL) {}

bool F2Matrix::get(std::size_t i, std::size_t j) const {
    return (data_[i * stride_ + (j >> 6)] >> (j & 63)) & 1ULL;
}

void F2Matrix::set(std::size_t i, std::size_t j, bool value) {
    std::uint64_t& w = data_[i * stride_ + (j >> 6)];
    std::uint64_t bit = 1ULL << (j & 63);
    if (value)
        w |= bit;
    else
        w &= ~bit;
}

std::size_t F2Matrix::row_popcount(std::size_t i) const {
    std::size_t c = 0;
    const std::uint64_t* r = &data_[i * stride_];
    for (std::size_t k = 0; k < stride_; ++k) c += __builtin_popcountll(r[k]);
    return c;
}

std::size_t F2Matrix::popcount() const {
    std::size_t c = 0;
    for (std::uint64_t w : data_) c += __builtin_popcountll(w);
    return c;
}

std::size_t F2Matrix::rank() const {
    std::vector<std::uint64_t> m = data_;  // reduce a copy; rank() is const
    std::size_t r = 0;
    for (std::size_t col = 0; col < cols_ && r < rows_; ++col) {
        std::size_t w = col >> 6;
        std::uint64_t bit = 1ULL << (col & 63);
        std::size_t piv = r;
        while (piv < rows_ && !(m[piv * stride_ + w] & bit)) ++piv;
        if (piv == rows_) continue;  // no pivot in this column
        if (piv != r) {
            for (std::size_t k = 0; k < stride_; ++k)
                std::swap(m[piv * stride_ + k], m[r * stride_ + k]);
        }
        for (std::size_t row = r + 1; row < rows_; ++row) {  // eliminate below
            if (m[row * stride_ + w] & bit) {
                for (std::size_t k = w; k < stride_; ++k)
                    m[row * stride_ + k] ^= m[r * stride_ + k];
            }
        }
        ++r;
    }
    return r;
}

F2Matrix F2Matrix::submatrix(std::span<const std::size_t> idx) const {
    F2Matrix s(idx.size(), idx.size());
    for (std::size_t a = 0; a < idx.size(); ++a)
        for (std::size_t b = 0; b < idx.size(); ++b)
            if (get(idx[a], idx[b])) s.set(a, b, true);
    return s;
}

bool F2Matrix::operator==(const F2Matrix& other) const {
    return rows_ == other.rows_ && cols_ == other.cols_ && data_ == other.data_;
}

}  // namespace at::linking
