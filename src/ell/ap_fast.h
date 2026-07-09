#pragma once
#include <cstdint>
#include <vector>

#include "core/int_types.h"
#include "ell/curve.h"

// Stage M0 — the a_p fast path (ARCHITECTURE-M §5): per-prime batching. This is
// the OPTIMIZED generator; its source (ell/ap_fast.cpp) is hashed into the a_p
// cache header (ell/ap_cache) so a stale cache is refused. It is checked against
// the frozen referee ell::ap_charsum (twin_ap_charsum_vs_table) — never the other
// way round; do not "optimize" the referee to match this.
namespace at::ell {

using at::core::u64;

// Per-prime quadratic-residue table: chi[k] = (k/p) ∈ {−1,0,+1} for k ∈ [0,p).
// Built once per prime in O(p); the fast path then does O(p) lookups per curve.
struct QRTable {
    u64 p;
    std::vector<int8_t> chi;
};
QRTable build_qr_table(u64 p);   // precondition: p > 3 prime

// Fast a_p for a good prime p > 3: same short-model character sum as ap_charsum,
// with the Legendre symbol read from the table instead of a modpow.
//   a_p = − Σ_x chi[(x³ + A x + B) mod p],  (A,B) = (−27 c4, −54 c6) mod p.
int ap_fast(const Curve& E, const QRTable& qr);

// a_p over a (curve × prime) grid; result[i][j] = a_{primes[j]}(curves[i]), all
// primes > 3. `parallel` fans out over primes; the output MUST be bit-identical to
// the serial path (tested — invariance_parallel_vs_serial, Stage-1 precedent).
std::vector<std::vector<int>> ap_fast_grid(const std::vector<Curve>& curves,
                                           const std::vector<u64>& primes,
                                           bool parallel);

}  // namespace at::ell
