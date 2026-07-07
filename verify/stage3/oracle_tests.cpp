// Stage 3 Phase 1 — oracle: our narrow class number h⁺ refereed by PARI/GP's
// bnfnarrow (NOT qfbclassno/quadclassunit, which are ordinary — see
// docs/notes/stage3-pinning.md §1.3). SKIPs cleanly if gp is absent.
#include "doctest/doctest.h"

#include <sstream>
#include <string>
#include <vector>

#include "classgroup/qform.h"
#include "oracle/pari.h"
#include "harness.h"
#include "stage3/fixture.h"

using at::core::i128;
using namespace at::classgroup;
using namespace at::verify::stage3;

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
