// Stage 0 — symbols: Legendre twins, Jacobi, Tonelli–Shanks, and quadratic
// reciprocity + its two supplementary laws (RESEARCH.md §2).
#include "doctest/doctest.h"

#include <algorithm>
#include <vector>

#include "core/modpow.h"
#include "symbols/jacobi.h"
#include "symbols/legendre.h"
#include "symbols/tonelli_shanks.h"
#include "harness.h"
#include "stage0/util.h"

using namespace at::core;
using namespace at::symbols;
using at::verify::stage0::next_sample;
using at::verify::stage0::primes_up_to;

namespace {
// Independent Jacobi via prime factorization: (a/n) = ∏ (a/p_i)^{e_i}, each
// factor by Euler's criterion. Different route than the binary recursion in
// jacobi(), so it is a genuine referee for it. n odd, n >= 1.
int jacobi_via_factorization(u64 a, u64 n) {
    int prod = 1;
    u64 m = n;
    for (u64 d = 3; d * d <= m; d += 2) {
        while (m % d == 0) {
            prod *= legendre_euler(a, d);
            m /= d;
        }
    }
    if (m > 1) prod *= legendre_euler(a, m);  // last remaining prime factor
    return prod;                               // n == 1 -> empty product -> 1
}
}  // namespace

// twin (a): Euler's criterion vs. (b) reciprocity recursion, exhaustive over
// all odd primes p < 10^6 with sampled a per prime (RESEARCH.md §2 mandate).
TEST_CASE("twin_legendre_euler_vs_reciprocity") {
    const u64 bound = at::verify::g_extended ? 10'000'000ULL : 1'000'000ULL;
    std::vector<u64> ps = primes_up_to(bound);
    u64 state = 0xDEADBEEFCAFEULL;
    u64 checks = 0;
    for (u64 p : ps) {
        if (p == 2) continue;  // Legendre symbol is defined for odd primes
        u64 amax = std::min<u64>(p - 1, 64);
        for (u64 a = 1; a <= amax; ++a) {  // small a exhaustively
            CHECK(legendre_euler(a, p) == legendre_recip(a, p));
            ++checks;
        }
        for (int k = 0; k < 16; ++k) {     // sampled larger a
            u64 a = next_sample(state) % p;
            CHECK(legendre_euler(a, p) == legendre_recip(a, p));
            ++checks;
        }
        CHECK(legendre_euler(0, p) == 0);
        CHECK(legendre_recip(0, p) == 0);
    }
    MESSAGE("legendre twin: " << checks << " agreements over " << ps.size()
                              << " primes");
}

// twin: jacobi() (binary recursion) vs. product-of-Legendre-over-factors.
TEST_CASE("twin_jacobi_vs_legendre_factorization") {
    const u64 bound = at::verify::g_extended ? 200'001ULL : 20'001ULL;
    u64 state = 0xABCDEF0123ULL;
    for (u64 n = 1; n <= bound; n += 2) {  // odd n only
        for (u64 a = 0; a <= std::min<u64>(n, 30); ++a) {
            CHECK(jacobi(a, n) == jacobi_via_factorization(a, n));
        }
        for (int k = 0; k < 8; ++k) {
            u64 a = next_sample(state) % (3 * n + 1);  // includes a >= n
            CHECK(jacobi(a, n) == jacobi_via_factorization(a, n));
        }
    }
}

// theorem: quadratic reciprocity for all distinct odd prime pairs below a
// bound, plus the linking-form symmetry (p/q)=(q/p) when both ≡ 1 (mod 4).
TEST_CASE("theorem_quadratic_reciprocity") {
    const u64 bound = at::verify::g_extended ? 30'000ULL : 3'000ULL;
    std::vector<u64> ps = primes_up_to(bound);
    u64 pairs = 0, linking = 0;
    for (size_t i = 0; i < ps.size(); ++i) {
        u64 p = ps[i];
        if (p == 2) continue;
        for (size_t j = i + 1; j < ps.size(); ++j) {
            u64 q = ps[j];
            int lhs = legendre_euler(p, q) * legendre_euler(q, p);
            int rhs = (p % 4 == 3 && q % 4 == 3) ? -1 : 1;  // (-1)^{(p-1)/2·(q-1)/2}
            CHECK(lhs == rhs);
            ++pairs;
            if (p % 4 == 1 && q % 4 == 1) {  // linking form: symmetric
                CHECK(legendre_euler(p, q) == legendre_euler(q, p));
                ++linking;
            }
        }
    }
    MESSAGE("QR: " << pairs << " prime pairs; " << linking
                   << " both ≡ 1 (mod 4) checked symmetric");
}

// theorem: first supplementary law (-1/p) = (-1)^{(p-1)/2}; +1 iff p ≡ 1 (mod 4).
TEST_CASE("theorem_supplement_first_minus1") {
    const u64 bound = at::verify::g_extended ? 10'000'000ULL : 1'000'000ULL;
    for (u64 p : primes_up_to(bound)) {
        if (p == 2) continue;
        int expected = (p % 4 == 1) ? 1 : -1;
        CHECK(legendre_euler(p - 1, p) == expected);  // (p-1) ≡ -1 (mod p)
    }
}

// theorem: second supplementary law (2/p) = (-1)^{(p^2-1)/8}; +1 iff p ≡ ±1 (mod 8).
TEST_CASE("theorem_supplement_second_two") {
    const u64 bound = at::verify::g_extended ? 10'000'000ULL : 1'000'000ULL;
    for (u64 p : primes_up_to(bound)) {
        if (p == 2) continue;
        int expected = (p % 8 == 1 || p % 8 == 7) ? 1 : -1;
        CHECK(legendre_euler(2, p) == expected);
    }
}

// twin/both-paths: Tonelli–Shanks vs. the Legendre symbol. Residues must yield
// a genuine root (r^2 ≡ a); non-residues must be *detected* as nullopt, not
// loop (RESEARCH.md §2 caveat). Both paths are asserted to actually occur.
TEST_CASE("twin_sqrt_mod_vs_legendre") {
    const u64 bound = at::verify::g_extended ? 500'000ULL : 50'000ULL;
    std::vector<u64> ps = primes_up_to(bound);
    u64 state = 0x5A5A5A5AULL;
    u64 residues = 0, nonresidues = 0;
    for (u64 p : ps) {
        if (p == 2) continue;
        u64 amax = std::min<u64>(p - 1, 50);
        for (u64 a = 0; a <= amax; ++a) {
            auto r = sqrt_mod(a, p);
            if (a % p == 0) {
                REQUIRE(r.has_value());
                CHECK(*r == 0);
                continue;
            }
            if (legendre_euler(a, p) == 1) {
                REQUIRE(r.has_value());
                CHECK(modmul(*r, *r, p) == a % p);
                ++residues;
            } else {
                CHECK_FALSE(r.has_value());  // non-residue detected
                ++nonresidues;
            }
        }
        for (int k = 0; k < 8; ++k) {
            u64 a = next_sample(state) % p;
            if (a == 0) continue;
            auto r = sqrt_mod(a, p);
            if (legendre_euler(a, p) == 1) {
                REQUIRE(r.has_value());
                CHECK(modmul(*r, *r, p) == a);
                ++residues;
            } else {
                CHECK_FALSE(r.has_value());
                ++nonresidues;
            }
        }
    }
    MESSAGE("tonelli–shanks: " << residues << " roots verified, "
                               << nonresidues << " non-residues detected");
    CHECK(residues > 0);      // both code paths were actually exercised
    CHECK(nonresidues > 0);
}
