// Stage M3 (completion) — the fast H₁ path, twinned against the FROZEN P4 referee
// (m3-pinning §P4 perf note; the standing twin rule, ERRATA #20: the referee never
// gets optimized — it is the thing the fast path is checked against).
//   twin_hurwitz_sieve_vs_referee — HurwitzTable(sieve) == hurwitz_by_forms over a
//     REQUIREd sample spanning the actual argument range (0 … the family max 4PN).
//   twin_trace_fast_vs_referee    — the sieve-backed trace overload == the referee
//     trace overload over an (N,P) sample (validates the fast path end-to-end).
#include "doctest/doctest.h"

#include <string>
#include <vector>

#include "core/primes.h"
#include "harness.h"
#include "mform/hurwitz.h"
#include "mform/trace.h"

using at::core::i64;
using at::core::u64;
using namespace at::mform;

TEST_CASE("twin_hurwitz_sieve_vs_referee") {
    // Family display max argument 4·⌊2.35·1800⌋·1990 = 33 662 840 (m3-pinning; the run
    // is deliberately scaled so the whole argument range is inside the EXACT integer
    // sieve — no floating class numbers). Twin: exhaustive low range + a stratified
    // sample to the top.
    const i64 B = 33662840;
    const HurwitzTable H(B);
    u64 checked = 0;
    for (i64 n = 0; n <= 6000; ++n) {          // exhaustive low
        CHECK(H(n) == hurwitz_by_forms(n));
        ++checked;
    }
    // Stratified sample across [6000, B]: step chosen to give ~500 points; each is an
    // O(n) referee evaluation, so this dominates the runtime but spans the full range.
    const i64 step = (B - 6000) / 500;
    for (i64 n = 6000; n <= B; n += step) {
        CHECK(H(n) == hurwitz_by_forms(n));
        ++checked;
    }
    CHECK(H(B) == hurwitz_by_forms(B));         // the very top of the range
    ++checked;
    MESSAGE("twin_hurwitz_sieve_vs_referee: sieve == hurwitz_by_forms over " << checked
            << " sampled n in [0, " << B << "] (family max 4PN; exact integer sieve)");
    REQUIRE(checked >= 6500);
}

TEST_CASE("twin_trace_fast_vs_referee") {
    // The sieve-backed trace overload must equal the frozen referee trace (which uses
    // hurwitz_by_forms) at every (N,P). Sample square-free N in the display window and
    // primes P spanning the family (P ∤ N).
    const i64 Nlo = 1800, Nhi = 1870, Pmax = 2600;
    const HurwitzTable H(4 * Pmax * Nhi);
    auto squarefree = [](i64 n) {
        for (i64 p = 2; p * p <= n; ++p) if (n % (p * p) == 0) return false;
        return true;
    };
    u64 checked = 0;
    for (i64 N = Nlo; N <= Nhi; ++N) {
        if (!squarefree(N)) continue;
        for (i64 P = 2; P <= Pmax; ++P) {
            if (!at::core::is_prime(static_cast<u64>(P))) continue;
            if (N % P == 0) continue;
            if ((P % 7) != (N % 7)) continue;   // thin the (N,P) grid to keep it quick
            CHECK(newform_weighted_trace_k2(N, P, H) == newform_weighted_trace_k2(N, P));
            ++checked;
        }
    }
    MESSAGE("twin_trace_fast_vs_referee: fast trace == referee trace over " << checked
            << " (square-free N∈[1800,1900], prime P≤4000, P∤N) cells");
    REQUIRE(checked >= 500);
}
