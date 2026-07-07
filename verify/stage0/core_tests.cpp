// Stage 0 — core primitives: modpow twins and deterministic Miller–Rabin.
#include "doctest/doctest.h"

#include <algorithm>
#include <vector>

#include "core/modpow.h"
#include "core/primes.h"
#include "harness.h"
#include "stage0/util.h"

using namespace at::core;
using at::verify::stage0::composite_sieve;
using at::verify::stage0::next_sample;

// twin: fast Montgomery modpow vs. the direct u128 referee. The two share only
// the square-and-multiply ladder; the load-bearing difference is the modular
// multiply (Montgomery REDC vs. schoolbook (u128)a*b % m), which is exactly
// where overflow/reduction bugs would hide.
TEST_CASE("twin_modmul_montgomery_vs_direct") {
    u64 state = 0x0123456789ABCDEFULL;
    const u64 n = at::verify::g_extended ? 5'000'000ULL : 200'000ULL;
    u64 cases = 0;
    for (u64 i = 0; i < n; ++i) {
        u64 m = next_sample(state) | 1ULL;      // odd modulus, up to ~2^63
        if (m < 3) m = 3;
        u64 b = next_sample(state) % m;
        u64 e = next_sample(state);
        CHECK(modpow(b, e, m) == modpow_naive(b, e, m));
        ++cases;
    }

    // Even moduli exercise the direct-path fallback; small ranges exhaustively.
    for (u64 m : {2ULL, 4ULL, 6ULL, 10ULL, 64ULL, 1'000'000ULL}) {
        for (u64 b = 0; b < 40; ++b) {
            for (u64 e = 0; e < 20; ++e) {
                CHECK(modpow(b, e, m) == modpow_naive(b, e, m));
                ++cases;
            }
        }
    }

    // A few pinned values, including the 0^0 == 1 convention.
    CHECK(modpow(2, 10, 1000) == 24);
    CHECK(modpow(3, 0, 7) == 1);
    CHECK(modpow(0, 0, 7) == 1);
    CHECK(modpow(7, 5, 7) == 0);
    CHECK(modpow(2, 1'000'000, 1'000'000'007ULL) ==
          modpow_naive(2, 1'000'000, 1'000'000'007ULL));

    MESSAGE("cases: " << cases << " (modpow twin comparisons)");
    REQUIRE(cases == n + 6 * 40 * 20);  // certifies the full sweep actually ran
}

// twin: deterministic Miller–Rabin vs. a sieve of Eratosthenes, exhaustively
// over [0, N]. The prime count falling out (π(10^6) = 78498) is a bonus
// ground-truth check.
TEST_CASE("twin_is_prime_vs_sieve") {
    const u64 N = at::verify::g_extended ? 20'000'000ULL : 1'000'000ULL;
    std::vector<bool> comp = composite_sieve(N);
    u64 primes = 0, cases = 0;
    for (u64 k = 0; k <= N; ++k) {
        bool sieve_prime = !comp[k];
        CHECK(is_prime(k) == sieve_prime);
        ++cases;
        if (sieve_prime) ++primes;
    }
    MESSAGE("cases: " << cases << " integers in [0, " << N << "]; primes found: "
                      << primes);
    REQUIRE(cases == N + 1);  // certifies the full range was swept
    // Exact prime count (PARI primepi) pins the range end-to-end.
    REQUIRE(primes == (at::verify::g_extended ? 1'270'607ULL /*π(2e7)*/
                                              : 78'498ULL /*π(1e6)*/));
}
