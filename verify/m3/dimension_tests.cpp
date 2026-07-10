// Stage M3 (completion) — dim S₂^new(N), the statistic's DENOMINATOR normalization
// (m3-pinning §P2 / completion). Eigenvalue-free, so it adjudicates the genus/Möbius
// formula independently of the trace formula and of every a_p.
//   anchor_dim_s2new            — published LMFDB newform dimensions (CI-green).
//   theorem_dim_oldnew_consistency — dim S₂(Γ₀(N)) == Σ_{M|N} σ₀(N/M)·dim S₂^new(M)
//                                    (the oldform reconstruction inverts the Möbius sieve).
//   oracle_dim_s2new            — REQUIREd sweep vs PARI mfdim([N,2],0) over ALL
//                                 square-free N in the family range (PARI = LMFDB's
//                                 dimension engine). SKIPs (77) if gp absent.
#include "doctest/doctest.h"

#include <sstream>
#include <string>
#include <vector>

#include "harness.h"
#include "mform/dimension.h"
#include "oracle/pari.h"

using at::core::i64;
using at::core::u64;
using namespace at::mform;

namespace {
bool squarefree(i64 n) {
    for (i64 p = 2; p * p <= n; ++p)
        if (n % (p * p) == 0) return false;
    return n >= 1;
}
// σ₀(k) = number of divisors.
i64 num_divisors(i64 k) {
    i64 c = 0;
    for (i64 d = 1; d <= k; ++d) if (k % d == 0) ++c;
    return c;
}
}  // namespace

TEST_CASE("anchor_dim_s2new") {
    // Published dim S₂^new(Γ₀(N)) (LMFDB classical newform dimensions), spanning the
    // small levels used elsewhere in M3 and into the completion family range
    // ([600,1990]). N=1990=2·5·199 sits in the display window.
    struct A { i64 N, dim; };
    const std::vector<A> anchors = {
        {11, 1}, {14, 1}, {15, 1}, {17, 1}, {19, 1}, {21, 1}, {22, 0}, {23, 2},
        {26, 2}, {33, 1}, {35, 3}, {37, 2}, {39, 3},
        {101, 8}, {389, 32}, {997, 82}, {1801, 149}, {1990, 65},
    };
    u64 checked = 0;
    for (const A& a : anchors) {
        CHECK(dim_s2_new(a.N) == a.dim);
        ++checked;
    }
    MESSAGE("anchor_dim_s2new: " << checked << " published LMFDB newform dimensions matched "
            "(11→1, 37→2, 389→32, 1990→65, …)");
    REQUIRE(checked == anchors.size());
}

TEST_CASE("theorem_dim_oldnew_consistency") {
    // The oldform reconstruction: a newform of level M appears in S₂(Γ₀(N)) with
    // multiplicity σ₀(N/M), so dim S₂(Γ₀(N)) = Σ_{M|N} σ₀(N/M)·dim S₂^new(M). This
    // inverts the Möbius sieve used to define dim_s2_new — an internal cross-check
    // independent of the genus arithmetic being right in isolation.
    u64 checked = 0;
    for (i64 N = 1; N <= 400; ++N) {
        i64 recon = 0;
        for (i64 M = 1; M <= N; ++M)
            if (N % M == 0) recon += num_divisors(N / M) * dim_s2_new(M);
        CHECK(recon == dim_s2_gamma0(N));
        ++checked;
    }
    MESSAGE("theorem_dim_oldnew_consistency: Σ_{M|N} σ₀(N/M)·dim Sⁿᵉʷ(M) == dim S₂(Γ₀(N)) "
            "over N∈[1,400] (" << checked << " levels)");
    REQUIRE(checked == 400);
}

TEST_CASE("oracle_dim_s2new") {
    std::optional<std::string> gp = oracle::find_gp();
    if (!gp) {
        MESSAGE("[SKIP] oracle_dim_s2new: PARI/GP `gp` not found on PATH.");
        at::verify::g_oracle_skipped = true;
        return;
    }
    MESSAGE("oracle: using gp at " << *gp);
    // ALL square-free N in the family range [2, 2000] (covers every convergence
    // window used, [600,688] … [1800,1990]).
    std::vector<i64> Ns;
    for (i64 N = 2; N <= 2000; ++N) if (squarefree(N)) Ns.push_back(N);

    std::ostringstream script;
    for (i64 N : Ns) script << "print(mfdim([" << N << ",2],0))\n";
    const std::string out = oracle::run_gp(*gp, script.str());

    std::istringstream lines(out);
    std::string line;
    u64 matched = 0;
    for (i64 N : Ns) {
        REQUIRE(std::getline(lines, line));
        const i64 pari = std::stoll(line);
        CHECK(dim_s2_new(N) == pari);
        ++matched;
    }
    MESSAGE("oracle_dim_s2new: " << matched << " square-free levels — dim_s2_new(N) == "
            "PARI mfdim([N,2],0) over the family range [2,2000]");
    REQUIRE(matched == Ns.size());
}
