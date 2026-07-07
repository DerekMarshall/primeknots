// Stage 3 Phase 1 — oracle: our narrow class number h⁺ refereed by PARI/GP's
// bnfnarrow (NOT qfbclassno/quadclassunit, which are ordinary — see
// docs/notes/stage3-pinning.md §1.3). SKIPs cleanly if gp is absent.
#include "doctest/doctest.h"

#include <sstream>
#include <string>
#include <vector>

#include <algorithm>

#include "classgroup/class_group.h"
#include "classgroup/qform.h"
#include "oracle/pari.h"
#include "harness.h"
#include "stage3/fixture.h"

using at::core::i128;
using namespace at::classgroup;
using namespace at::verify::stage3;

namespace {
// Parse a gp vector like "[4, 2]" or "[]" into a sorted ascending vector.
std::vector<long long> parse_cyc(const std::string& line) {
    std::vector<long long> v;
    std::string t;
    for (char ch : line) {
        if (ch == '[' || ch == ']' || ch == ' ') continue;
        if (ch == ',') { if (!t.empty()) { v.push_back(std::stoll(t)); t.clear(); } }
        else t += ch;
    }
    if (!t.empty()) v.push_back(std::stoll(t));
    std::sort(v.begin(), v.end());
    return v;
}
}  // namespace

TEST_CASE("oracle_narrow_class_number") {
    std::optional<std::string> gp = oracle::find_gp();
    if (!gp) {
        MESSAGE("[SKIP] oracle_narrow_class_number: PARI/GP `gp` not found on PATH.");
        at::verify::g_oracle_skipped = true;
        return;
    }
    std::vector<i128> Ds = family_discriminants();

    std::ostringstream script;
    for (i128 D : Ds)
        script << "print(bnfnarrow(bnfinit(x^2-" << static_cast<long long>(D)
               << ")).no)\n";
    std::string out = oracle::run_gp(*gp, script.str());

    std::istringstream lines(out);
    std::string line;
    i128 matched = 0;
    for (i128 D : Ds) {
        REQUIRE(std::getline(lines, line));
        i128 gp_h = static_cast<i128>(std::stoll(line));  // narrow h⁺ from bnfnarrow
        CHECK(narrow_class_number_by_cycles(D) == gp_h);
        ++matched;
    }
    MESSAGE("cases: " << static_cast<long long>(matched)
                      << " narrow class numbers matched PARI/GP bnfnarrow");
    REQUIRE(matched == expected_disc_count());
}

// Narrow class-group STRUCTURE (SNF invariant factors) matched against
// bnfnarrow.cyc, over a sample of the 4-rank sweep.
TEST_CASE("oracle_narrow_structure_match") {
    std::optional<std::string> gp = oracle::find_gp();
    if (!gp) {
        MESSAGE("[SKIP] oracle_narrow_structure_match: PARI/GP `gp` not found.");
        at::verify::g_oracle_skipped = true;
        return;
    }
    std::vector<FamMember> fam = sweep_members();
    std::size_t step = fam.size() / 250 + 1;  // ~250 samples (bnfinit is heavy)
    std::vector<FamMember> sample;
    for (std::size_t i = 0; i < fam.size(); i += step) sample.push_back(fam[i]);

    std::ostringstream script;
    for (const FamMember& m : sample)
        script << "print(bnfnarrow(bnfinit(x^2-" << (long long)m.D << ")).cyc)\n";
    std::string out = oracle::run_gp(*gp, script.str());

    std::istringstream lines(out);
    std::string line;
    i128 matched = 0;
    for (const FamMember& m : sample) {
        REQUIRE(std::getline(lines, line));
        std::vector<long long> gp_cyc = parse_cyc(line);
        std::vector<i128> ours = ClassGroup::compute(m.D).invariant_factors();
        std::vector<long long> ours_ll(ours.begin(), ours.end());  // already ascending
        REQUIRE(ours_ll.size() == gp_cyc.size());
        for (std::size_t k = 0; k < gp_cyc.size(); ++k) CHECK(ours_ll[k] == gp_cyc[k]);
        ++matched;
    }
    MESSAGE("cases: " << static_cast<long long>(matched)
                      << " narrow class-group structures matched bnfnarrow.cyc");
    REQUIRE(matched == static_cast<i128>(sample.size()));
}
