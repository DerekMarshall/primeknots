// Stage 3 Phase 2 — the named theorems: genus (2-rank = t−1) and Rédei–Reichardt
// (form-side 4-rank == linking-side t−1−rank R). The central punchline: the
// class group computed two independent ways (homology side vs. linking side).
#include "doctest/doctest.h"

#include <span>
#include <sstream>
#include <string>
#include <vector>

#include "classgroup/class_group.h"
#include "classgroup/redei_matrix.h"
#include "linking/linking_matrix.h"
#include "stage3/fixture.h"

using at::core::i128;
using at::core::u64;
using namespace at::classgroup;
using namespace at::verify::stage3;

// Genus theory (Gauss): 2-rank of Cl⁺(D) = t − 1 over the whole family.
TEST_CASE("theorem_genus_2rank") {
    std::vector<FamMember> fam = sweep_members();
    REQUIRE(static_cast<i128>(fam.size()) == sweep_expected_count());
    i128 cases = 0;
    for (const FamMember& m : fam) {
        std::size_t t = m.primes.size();
        CHECK(ClassGroup::compute(m.D).rank2() == t - 1);
        ++cases;
    }
    MESSAGE("cases: " << static_cast<long long>(cases)
                      << " discriminants; 2-rank(Cl⁺) == t−1 (Gauss)");
    REQUIRE(cases == sweep_expected_count());
}

// The Rédei matrix diagonal convention (pinning §2.3): every row sums to 0.
TEST_CASE("theorem_redei_matrix_rows_sum_zero") {
    std::vector<FamMember> fam = sweep_members();
    at::linking::LinkingMatrix LM = sweep_linking();
    i128 cases = 0;
    for (const FamMember& m : fam) {
        at::linking::F2Matrix R = redei_matrix(
            std::span<const u64>(m.primes.data(), m.primes.size()), LM);
        std::size_t t = m.primes.size();
        for (std::size_t i = 0; i < t; ++i) {
            bool s = false;
            for (std::size_t j = 0; j < t; ++j)
                if (R.get(i, j)) s = !s;
            REQUIRE(s == false);  // row i sums to 0 ∈ F₂
        }
        ++cases;
    }
    MESSAGE("cases: " << static_cast<long long>(cases)
                      << " Rédei matrices; every row sums to 0");
    REQUIRE(cases == sweep_expected_count());
}

// Rédei–Reichardt (1934): 4-rank of Cl⁺(D) [form composition, homology side]
// equals t − 1 − rank_{F₂}(R) [linking side]. ≥1e3 discriminants, stratified by
// 4-rank (corank) so 0, 1, ≥2 are all in force. Rule 1: a disagreement is the
// deliverable — full witness printed, test fails.
TEST_CASE("theorem_redei_reichardt_4rank") {
    std::vector<FamMember> fam = sweep_members();
    at::linking::LinkingMatrix LM = sweep_linking();
    REQUIRE(static_cast<i128>(fam.size()) == sweep_expected_count());
    i128 cases = 0, s0 = 0, s1 = 0, s2 = 0;
    for (const FamMember& m : fam) {
        std::span<const u64> ps(m.primes.data(), m.primes.size());
        std::size_t r4_form = ClassGroup::compute(m.D).rank4();  // homology side
        std::size_t r4_link = fourrank_from_linking(ps, LM);     // linking side
        if (r4_form != r4_link) {
            // Rule 1 witness: D, the Rédei matrix, and both ranks.
            at::linking::F2Matrix R = redei_matrix(ps, LM);
            std::ostringstream w;
            w << "RÉDEI–REICHARDT DISAGREEMENT D=" << (long long)m.D
              << " t=" << m.primes.size() << " r4_form=" << r4_form
              << " r4_link=" << r4_link << " R=[";
            for (std::size_t i = 0; i < m.primes.size(); ++i) {
                for (std::size_t j = 0; j < m.primes.size(); ++j) w << R.get(i, j);
                w << (i + 1 < m.primes.size() ? ";" : "");
            }
            w << "]";
            MESSAGE(w.str());
        }
        CHECK(r4_form == r4_link);
        if (r4_link == 0) ++s0; else if (r4_link == 1) ++s1; else ++s2;
        ++cases;
    }
    MESSAGE("cases: " << static_cast<long long>(cases)
                      << " discriminants; 4-rank strata r4=0:" << (long long)s0
                      << " r4=1:" << (long long)s1 << " r4≥2:" << (long long)s2);
    REQUIRE(cases == sweep_expected_count());
    REQUIRE(s0 > 0);  // trivial 4-rank present
    REQUIRE(s1 > 0);  // corank-1 present in force
    REQUIRE(s2 > 0);  // corank ≥2 present in force
}
