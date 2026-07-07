// Stage 1 — oracle: a sample of linking-matrix entries refereed by PARI/GP's
// kronecker(). SKIPs cleanly (exit 77) if gp is absent.
#include "doctest/doctest.h"

#include <sstream>
#include <string>
#include <vector>

#include "linking/linking_matrix.h"
#include "oracle/pari.h"
#include "harness.h"
#include "stage1/fixture.h"

using at::core::u64;
using at::linking::LinkingMatrix;
using namespace at::verify::stage1;

TEST_CASE("oracle_kronecker_linking") {
    std::optional<std::string> gp = oracle::find_gp();
    if (!gp) {
        MESSAGE("[SKIP] oracle_kronecker_linking: PARI/GP `gp` not found on PATH.");
        at::verify::g_oracle_skipped = true;
        return;
    }
    const LinkingMatrix& m = shared();
    const std::size_t n = m.primes.size();

    // Sample entries and referee (p_i / p_j) = (-1)^{lk(i,j)} against gp.
    struct Cell { std::size_t i, j; };
    std::vector<Cell> cells;
    u64 state = 0xC0FFEE'123456ULL;
    const u64 want = at::verify::g_extended ? 20'000ULL : 5'000ULL;
    while (cells.size() < want) {
        std::size_t i = next_sample(state) % n;
        std::size_t j = next_sample(state) % n;
        if (i != j) cells.push_back({i, j});
    }

    std::ostringstream script;
    for (const Cell& c : cells) {
        script << "print(kronecker(" << m.primes[c.i] << ", " << m.primes[c.j]
               << "))\n";
    }
    std::string out = oracle::run_gp(*gp, script.str());

    std::istringstream lines(out);
    std::string line;
    u64 matched = 0;
    for (const Cell& c : cells) {
        REQUIRE(std::getline(lines, line));
        int oracle_symbol = std::stoi(line);              // ±1 for distinct primes
        int ours = m.lk.get(c.i, c.j) ? -1 : 1;           // (-1)^{lk}
        CHECK(ours == oracle_symbol);
        ++matched;
    }
    MESSAGE("cases: " << matched
                      << " linking entries matched PARI/GP kronecker() over "
                      << n << " primes");
    REQUIRE(matched == cells.size());
    REQUIRE(matched == want);
}
