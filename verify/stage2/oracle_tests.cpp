// Stage 2 Phase 1 — oracle: our conic solubility/solutions refereed by PARI/GP
// qfsolve on the form diag(1, -a, -b). SKIPs cleanly if gp is absent.
#include "doctest/doctest.h"

#include <sstream>
#include <string>
#include <vector>

#include "oracle/pari.h"
#include "redei/conic.h"
#include "harness.h"
#include "stage2/fixture.h"

using at::core::i128;
using at::core::u64;
using at::redei::ConicSolution;
using at::redei::solve_conic;
using namespace at::verify::stage2;

TEST_CASE("oracle_conic_qfsolve") {
    std::optional<std::string> gp = oracle::find_gp();
    if (!gp) {
        MESSAGE("[SKIP] oracle_conic_qfsolve: PARI/GP `gp` not found on PATH.");
        at::verify::g_oracle_skipped = true;
        return;
    }

    // Sample distinct-prime pairs up to a bound beyond the brute-force twin
    // range, so the fast path is refereed where brute force is too slow.
    std::vector<u64> ps = primes_1mod4_up_to(at::verify::g_extended ? 20'000ULL
                                                                     : 5'000ULL);
    struct Pair { u64 a, b; };
    std::vector<Pair> pairs;
    u64 state = 0xB07E0F1A'12345678ULL;
    const u64 want = at::verify::g_extended ? 4'000ULL : 1'000ULL;
    while (pairs.size() < want) {
        u64 a = ps[next_sample(state) % ps.size()];
        u64 b = ps[next_sample(state) % ps.size()];
        if (a != b) pairs.push_back({a, b});
    }

    std::ostringstream script;
    for (const Pair& p : pairs) {
        script << "{v=qfsolve([1,0,0;0,-" << p.a << ",0;0,0,-" << p.b
               << "]);if(type(v)==\"t_COL\",print(\"SOL \",v[1],\" \",v[2],\" \",v[3]),print(\"NONE\"))}\n";
    }
    std::string out = oracle::run_gp(*gp, script.str());

    std::istringstream lines(out);
    std::string line;
    u64 checked = 0, soluble = 0;
    for (const Pair& p : pairs) {
        REQUIRE(std::getline(lines, line));
        std::istringstream ls(line);
        std::string tag;
        ls >> tag;
        bool gp_soluble = (tag == "SOL");

        std::vector<ConicSolution> ours = solve_conic(p.a, p.b, 1);
        CHECK(ours.empty() != gp_soluble);  // solubility agrees with the oracle

        if (gp_soluble) {
            long long x, y, z;
            ls >> x >> y >> z;
            // gp's own solution must satisfy our conic equation.
            i128 lhs = (i128)x * x - (i128)p.a * y * y - (i128)p.b * z * z;
            CHECK(lhs == 0);
            ++soluble;
        }
        ++checked;
    }
    MESSAGE("cases: " << checked << " conics refereed by gp qfsolve (" << soluble
                      << " soluble); solubility + gp solution validity confirmed");
    REQUIRE(checked == pairs.size());
    REQUIRE(checked == want);
}
