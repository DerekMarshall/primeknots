#pragma once
#include <cstdint>

// Core integer aliases. Every intermediate product in modular arithmetic goes
// through u128 (ARCHITECTURE.md §2) so 64-bit multiplies never overflow.
namespace at::core {
using u64  = std::uint64_t;
using i64  = std::int64_t;
using u128 = unsigned __int128;  // 128-bit intermediates; compiler builtin
using i128 = __int128;           // signed 128-bit (conic solutions, Stage 2+)
}  // namespace at::core
