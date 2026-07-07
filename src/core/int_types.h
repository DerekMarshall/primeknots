#pragma once
#include <cstdint>

// Core integer aliases. Every intermediate product in modular arithmetic goes
// through u128 (ARCHITECTURE.md §2) so 64-bit multiplies never overflow.
namespace at::core {
using u64  = std::uint64_t;
using u128 = unsigned __int128;  // 128-bit intermediates; compiler builtin
}  // namespace at::core
