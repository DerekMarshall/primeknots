// Stage 4 — invariance tests. Per the pre-registered protocol, these validate
// that each side is well-defined (independent of the arbitrary choices in its
// construction) BEFORE any LHS/RHS comparison.
#include "doctest/doctest.h"

#include <algorithm>
#include <span>
#include <vector>

#include "cs/lhs.h"
#include "cs/rhs.h"
#include "stage4/fixture.h"

using at::core::i128;
using at::core::u64;
using namespace at::verify::stage4;

namespace {
// Independent character-sum enumeration of the LHS (different parametrization
// from the lhs.cpp TU): fix the coset rep with c₁=0 and enumerate the remaining
// 2^{r−1} vectors directly, rather than summing over all 2^r and halving.
long independent_charsum(const std::vector<u64>& P) {
    int r = static_cast<int>(P.size());
    if (r <= 1) return 1;
    long total = 0;
    for (unsigned v = 0; v < (1u << (r - 1)); ++v) {
        // c₁=0; c_{k} = bit (k-1) of v for k=2..r
        auto ci = [&](int i) { return i == 0 ? 0 : ((v >> (i - 1)) & 1u); };
        int cs = 0;
        for (int i = 0; i < r; ++i)
            for (int j = i + 1; j < r; ++j)
                cs ^= (lk2(P[i], P[j]) & (ci(i) ^ ci(j)));
        total += cs ? -1 : 1;
    }
    return total;
}
}  // namespace

// S-ordering invariance: Z_c depends only on the SET S, not its ordering. Both
// sides must be unchanged under permuting the prime list.
TEST_CASE("invariance_s_ordering") {
    long excluded = 0;
    std::vector<Instance> S = sweep(excluded);
    REQUIRE(!S.empty());
    u64 state = 0xA5A5C3C3ULL;
    long cases = 0;
    for (const Instance& I : S) {
        std::span<const u64> sorted(I.primes.data(), I.primes.size());
        long lref = at::cs::lhs_partition(sorted);
        long rref = at::cs::rhs_partition(sorted);
        for (int t = 0; t < 3; ++t) {
            std::vector<u64> perm = I.primes;
            // deterministic Fisher–Yates shuffle
            for (std::size_t k = perm.size(); k > 1; --k) {
                state = state * 6364136223846793005ULL + 1442695040888963407ULL;
                std::size_t j = (state >> 33) % k;
                std::swap(perm[k - 1], perm[j]);
            }
            std::span<const u64> ps(perm.data(), perm.size());
            CHECK(at::cs::lhs_partition(ps) == lref);
            CHECK(at::cs::rhs_partition(ps) == rref);
            ++cases;
        }
    }
    MESSAGE("cases: " << cases << " permutations; lhs and rhs both S-ordering invariant");
    REQUIRE(cases > 0);
}

// Character-basis invariance of the LHS: the brute character sum is independent
// of how Hom(T₊,ℤ/2) is parametrized. Compare lhs_partition to an independent
// enumeration (c₁=0 coset reps) over the whole sweep.
TEST_CASE("invariance_lhs_character_basis") {
    long excluded = 0;
    std::vector<Instance> S = sweep(excluded);
    REQUIRE(!S.empty());
    long cases = 0;
    for (const Instance& I : S) {
        std::span<const u64> ps(I.primes.data(), I.primes.size());
        CHECK(at::cs::lhs_partition(ps) == independent_charsum(I.primes));
        ++cases;
    }
    MESSAGE("cases: " << cases
                      << " tuples; lhs == independent character-sum enumeration");
    REQUIRE(cases == expected_total());
}
