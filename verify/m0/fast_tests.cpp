// Stage M0 — the fast path (ARCHITECTURE-M §5), refereed by the frozen charsum
// referee and checked for deterministic parallelism.
//   twin_ap_charsum_vs_table       — QR-table fast path == frozen ap_charsum.
//   invariance_parallel_vs_serial  — the a_p grid is bit-identical parallel/serial.
#include "doctest/doctest.h"

#include "ell/ap.h"
#include "ell/ap_fast.h"
#include "ell/curve.h"
#include "harness.h"
#include "m0/fixture.h"

using at::core::i128;
using at::core::u64;
using namespace at::ell;
using at::verify::m0::primes_in;
using at::verify::m0::sample_curves;

TEST_CASE("twin_ap_charsum_vs_table") {
    const auto curves = sample_curves();
    const auto primes = primes_in(5, 1000);  // p > 3
    u64 checked = 0;
    for (u64 p : primes) {
        const QRTable qr = build_qr_table(p);   // built once per prime (the fast shape)
        for (const Curve& E : curves) {
            if (discriminant(E) % static_cast<i128>(p) == 0) continue;  // good primes
            CHECK(ap_fast(E, qr) == ap_charsum(E, p));
            ++checked;
        }
    }
    MESSAGE("twin_ap_charsum_vs_table: fast path == frozen referee over " << checked
            << " (curve, p) pairs, p ∈ (3,1000] good");
    // 166 primes in (3,1000], 8 curves, minus 7 (curve,p) bad-reduction exclusions
    // (11,7,5,17,19,37,389 divide the sampled conductors; 5077 > 1000).
    REQUIRE(checked == 166 * 8 - 7);
}

TEST_CASE("invariance_parallel_vs_serial") {
    const auto curves = sample_curves();
    const auto primes = primes_in(5, 1000);
    const auto serial = ap_fast_grid(curves, primes, /*parallel=*/false);
    const auto parallel = ap_fast_grid(curves, primes, /*parallel=*/true);

    const bool identical = (serial == parallel);
    CHECK(identical);
    const u64 cells = static_cast<u64>(curves.size()) * primes.size();
    MESSAGE("invariance_parallel_vs_serial: " << cells
            << " grid cells bit-identical (parallel vs serial)");
    REQUIRE(identical);
    REQUIRE(cells == static_cast<u64>(curves.size()) * primes.size());
    REQUIRE(cells >= 8 * 166);
}
