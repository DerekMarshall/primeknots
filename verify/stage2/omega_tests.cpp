// Stage 2 Phase 2 — R3: the O/4O square-class enumerator, twinned across two
// independent code paths (closed-form ω²=ω+m vs. a generic O/4 multiply).
#include "doctest/doctest.h"

#include <array>

#include "redei/omega_mod4.h"
#include "stage2/fixture.h"

using at::core::u64;
using namespace at::redei;
using namespace at::verify::stage2;

TEST_CASE("twin_omega_square_classes_mod4") {
    auto ps = primes_1mod4_up_to(twin_bound());
    u64 cases = 0;
    for (u64 p1 : ps) {
        std::array<bool, 16> a = square_classes_mod4(p1);          // closed form
        std::array<bool, 16> b = square_classes_mod4_via_mul(p1);  // generic mul
        CHECK(a == b);
        // sanity: 0 = 0² and 1 = 1² are always squares; the set is nontrivial.
        CHECK(is_square_mod4(0, 0, p1));
        CHECK(is_square_mod4(1, 0, p1));
        int count = 0;
        for (bool s : a) count += s ? 1 : 0;
        CHECK(count >= 1);
        CHECK(count <= 16);
        ++cases;
    }
    MESSAGE("cases: " << cases
                      << " primes; closed-form vs generic-mul square classes agree");
    REQUIRE(cases == twin_expected_primes());
}
