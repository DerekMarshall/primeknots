// Stage 4 — the CKKPPY/Hirano partition identity: LHS (brute character phase
// sum) == RHS (closed form), over the pell_neg-gated sweep, stratified by
// row-parity (R2). The two sides were implemented in isolation; a mismatch is a
// deliverable (full witness), never reconciled by editing a side (Rule 1).
#include "doctest/doctest.h"

#include <sstream>
#include <span>
#include <string>
#include <vector>

#include "cs/lhs.h"
#include "cs/rhs.h"
#include "stage4/fixture.h"

using at::core::i128;
using at::core::u64;
using namespace at::verify::stage4;

namespace {
long lhs(const std::vector<u64>& P) { return at::cs::lhs_partition({P.data(), P.size()}); }
long rhs(const std::vector<u64>& P) { return at::cs::rhs_partition({P.data(), P.size()}); }
}  // namespace

// ANCHORS: the published Hirano Ex 4.2.5 values, plus the R1 witness ℚ(√65).
TEST_CASE("anchor_ckkppy_hirano_examples") {
    CHECK(lhs({5, 13}) == 0);        // R1 witness (distinguishes all-even from equal-parity)
    CHECK(rhs({5, 13}) == 0);
    CHECK(lhs({5, 29, 37}) == 0);    // Hirano Ex 4.2.5(1)
    CHECK(rhs({5, 29, 37}) == 0);
    CHECK(lhs({5, 13, 73}) == 4);    // Hirano Ex 4.2.5(2)
    CHECK(rhs({5, 13, 73}) == 4);
    MESSAGE("cases: 3 anchors — Z_c(√65)=0, Z_c(√(5·29·37))=0, Z_c(√(5·13·73))=4");
}

// The main identity over the whole gated, stratified sweep.
TEST_CASE("theorem_ckkppy_partition_identity") {
    long excluded = 0;
    std::vector<Instance> S = sweep(excluded);
    REQUIRE(static_cast<long>(S.size()) == expected_total());

    long cell[2][3] = {{0, 0, 0}, {0, 0, 0}};  // [r parity][ALL_EVEN/ALL_ODD/MIXED]
    long cases = 0, mismatches = 0;
    for (const Instance& I : S) {
        long L = lhs(I.primes), R = rhs(I.primes);
        if (L != R) {
            std::ostringstream w;
            w << "PARTITION-IDENTITY MISMATCH: S={";
            for (std::size_t k = 0; k < I.primes.size(); ++k)
                w << (k ? "," : "") << I.primes[k];
            w << "} lhs=" << L << " rhs=" << R;
            MESSAGE(w.str());  // Rule 1: full witness
            ++mismatches;
        }
        CHECK(L == R);
        cell[I.r % 2][I.parity]++;
        ++cases;
    }
    MESSAGE("cases: " << cases << " gated tuples (LHS==RHS); " << excluded
                      << " excluded by pell_neg (R3); strata "
                      << "even[ae=" << cell[0][ALL_EVEN] << ",ao=" << cell[0][ALL_ODD]
                      << ",mx=" << cell[0][MIXED] << "] odd[ae=" << cell[1][ALL_EVEN]
                      << ",ao=" << cell[1][ALL_ODD] << ",mx=" << cell[1][MIXED] << "]");

    REQUIRE(cases == expected_total());
    REQUIRE(excluded == expected_excluded());  // R3 empiric

    // R2: every nonempty cell in force, and all-odd × odd-r asserted EMPTY.
    REQUIRE(cell[0][ALL_EVEN] > 0);
    REQUIRE(cell[0][ALL_ODD] > 0);
    REQUIRE(cell[0][MIXED] > 0);
    REQUIRE(cell[1][ALL_EVEN] > 0);
    REQUIRE(cell[1][MIXED] > 0);
    REQUIRE(cell[1][ALL_ODD] == 0);  // parity: odd # of odd row-sums impossible
    REQUIRE(mismatches == 0);
}
