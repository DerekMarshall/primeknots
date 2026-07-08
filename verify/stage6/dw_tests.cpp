// Stage 6 — assembled |Hom| / Z_DW: decomposition, mass formula (exact rational),
// signature (R3), invariance, anchors. Reads the PARI referee cache; no oracle
// (the cache is refereed by oracle_tests.cpp / compute_cubic_counts.py --check).
#include "doctest/doctest.h"

#include <algorithm>
#include <vector>

#include "dw/s3_count.h"
#include "stage6/fixture.h"

using namespace at::dw;
using namespace at::verify::stage6;

namespace {
const CubicInstance* find_S(const std::vector<CubicInstance>& v,
                            const std::vector<std::uint64_t>& S) {
    for (const auto& ci : v) if (ci.primes == S) return &ci;
    return nullptr;
}
}  // namespace

TEST_CASE("invariance_s_ordering") {
    // t, k, c and hence |Hom| / Z_DW depend on S only as a set: permuting the
    // prime list changes nothing (cyclic_k and |S| are order-independent).
    auto cache = load_cubic_cache();
    REQUIRE(cache.size() >= 10);
    int checked = 0;
    for (const auto& ci : cache) {
        std::vector<std::uint64_t> fwd = ci.primes, rev = ci.primes;
        std::reverse(rev.begin(), rev.end());
        if (fwd.size() >= 3) std::rotate(fwd.begin(), fwd.begin() + 1, fwd.end());
        int t = static_cast<int>(ci.primes.size());
        Rational z0 = z_dw(t, cyclic_k(ci.primes), ci.c);
        Rational z1 = z_dw(static_cast<int>(rev.size()), cyclic_k(rev), ci.c);
        Rational z2 = z_dw(static_cast<int>(fwd.size()), cyclic_k(fwd), ci.c);
        bool inv = equal(z0, z1) && equal(z0, z2);
        CHECK(inv);
        ++checked;
    }
    MESSAGE("invariance checked over " << checked << " prime sets");
}

TEST_CASE("anchor_dw_c_zero_and_nonzero") {
    // REQUIRE the sweep contains an S with c = 0 AND an S with c >= 2 (an all-zero
    // sweep would prove nothing). Check assembled Z_DW on named anchors.
    auto cache = load_cubic_cache();
    REQUIRE(!cache.empty());
    bool has_zero = false, has_ge2 = false;
    for (const auto& ci : cache) {
        if (ci.c == 0) has_zero = true;
        if (ci.c >= 2) has_ge2 = true;
    }
    CHECK(has_zero);
    CHECK(has_ge2);

    // S = {13}: t=1, k=1 (13≡1 mod12), c=0 ⇒ |Hom| = 1+3+2+0 = 6, Z_DW = 1.
    const CubicInstance* a = find_S(cache, {13});
    REQUIRE(a != nullptr);
    CHECK(a->c == 0);
    CHECK(hom_count(1, cyclic_k(a->primes), a->c) == 6);
    bool z_is_one = equal(z_dw(1, cyclic_k(a->primes), a->c), rational(1, 1));
    CHECK(z_is_one);

    // S = {5,17,29}: t=3, k=0 (all ≡5 mod12), c=2 ⇒ |Hom| = 1+21+0+12 = 34.
    const CubicInstance* b = find_S(cache, {5, 17, 29});
    REQUIRE(b != nullptr);
    CHECK(b->c == 2);
    CHECK(cyclic_k(b->primes) == 0);
    CHECK(hom_count(3, 0, b->c) == 34);
    bool z_is_17_3 = equal(z_dw(3, 0, b->c), rational(17, 3));
    CHECK(z_is_17_3);

    // S = {5,13,17,29}: t=4, k=1, c=7 (the larger anchor).
    const CubicInstance* d = find_S(cache, {5, 13, 17, 29});
    REQUIRE(d != nullptr);
    CHECK(d->c == 7);
    CHECK(hom_count(4, cyclic_k(d->primes), d->c) == 1 + 3 * 15 + 2 + 42);
    MESSAGE("anchors: {13} Z_DW=1 (c=0); {5,17,29} Z_DW=17/3 (c=2); {5,13,17,29} c=7");
}

TEST_CASE("theorem_dw_mass_formula") {
    // z_dw == 1/6 + (2^t−1)/2 + (3^k−1)/6 + c, exact rational, every instance (R2).
    auto cache = load_cubic_cache();
    REQUIRE(!cache.empty());
    for (const auto& ci : cache) {
        int t = static_cast<int>(ci.primes.size()), k = cyclic_k(ci.primes);
        bool ok = equal(z_dw(t, k, ci.c), mass_formula_rhs(t, k, ci.c));
        CHECK(ok);
    }
    MESSAGE("mass formula (exact rational) verified over " << cache.size() << " instances");
}

TEST_CASE("theorem_signature_all_totally_real") {
    // R3: negative-disc set empty by Stickelberger + pool ≡ 1 mod 4 (|disc|≡1 mod4
    // ⇒ disc<0 would be ≡3 mod4, forbidden). Every referee instance is totally real.
    auto cache = load_cubic_cache();
    REQUIRE(!cache.empty());
    long long total_cubics = 0;
    for (const auto& ci : cache) {
        CHECK(ci.all_real);           // no complex (1,1) cubic occurred
        for (long long d : ci.discs) CHECK(d > 0);
        total_cubics += static_cast<long long>(ci.discs.size());
    }
    MESSAGE("all " << total_cubics << " cubics across the sweep are totally real (disc>0)");
}

TEST_CASE("theorem_dw_s3_decomposition") {
    // Per-instance term structure + per-stratum REQUIREs.
    auto cache = load_cubic_cache();
    REQUIRE(!cache.empty());
    int strat_t[5] = {0, 0, 0, 0, 0};
    int k_zero = 0, k_pos = 0, c_zero = 0, c_pos = 0;
    for (const auto& ci : cache) {
        int t = static_cast<int>(ci.primes.size()), k = cyclic_k(ci.primes);
        // scope: every prime ≡ 1 mod 4, so 2 ∉ S and 3 ∉ S (H3).
        for (std::uint64_t p : ci.primes) {
            CHECK(p % 4 == 1);
            CHECK(p != 3);
            CHECK(p != 2);
        }
        // |Hom| = 1 + 3(2^t−1) + (3^k−1) + 6c, terms consistent.
        CHECK(hom_count(t, k, ci.c) ==
              trivial_term() + c2_term(t) + c3_term(k) + s3_term(ci.c));
        // c iso classes ⇒ discs list has exactly c entries.
        CHECK(static_cast<long long>(ci.discs.size()) == ci.c);
        if (t <= 4) ++strat_t[t];
        (k == 0) ? ++k_zero : ++k_pos;
        (ci.c == 0) ? ++c_zero : ++c_pos;
    }
    // strata populated (R2/H5): t=1..4, k=0 & k>0, c=0 & c>0 all present.
    CHECK(strat_t[1] > 0);
    CHECK(strat_t[2] > 0);
    CHECK(strat_t[3] > 0);
    CHECK(strat_t[4] > 0);
    CHECK(k_zero > 0);
    CHECK(k_pos > 0);
    CHECK(c_zero > 0);
    CHECK(c_pos > 0);
    MESSAGE("strata t=[_," << strat_t[1] << "," << strat_t[2] << "," << strat_t[3]
            << "," << strat_t[4] << "] k0=" << k_zero << " k+=" << k_pos
            << " c0=" << c_zero << " c+=" << c_pos);
}
