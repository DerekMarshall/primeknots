// Stage M4 (Phase 2, rider R3 / m4-pinning §P4/C3) — the DUAL-ORACLE OVERLAP TWIN,
// run BEFORE the statistic consumes any oracle column. The height-ordered statistic
// takes each curve's conductor N and root number ε from PARI (oracle-provenance input
// data, §2 amendment); this certifies PARI against the pinned Cremona ecdata over
// their overlap, so the live oracle is not trusted blindly:
//   PARI ellglobalred conductor  ==  ecdata N          (exact),
//   PARI ellrootno               ==  (−1)^{ecdata rank} (the parity relation — the
//                                    rank-independent ε twin, from-scratch Tate, is
//                                    the deferred upgrade, m4-pinning §P4(c)).
// Oracle-class: SKIPs (77) when gp is absent (CI) or ecdata is missing.
#include "doctest/doctest.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ell/ecdata.h"
#include "harness.h"
#include "oracle/pari.h"

using at::core::u64;

#ifndef AT_ECDATA_DIR
#define AT_ECDATA_DIR "data/cremona"
#endif

TEST_CASE("oracle_dual_overlap_NE") {
    std::optional<std::string> gp = oracle::find_gp();
    if (!gp) {
        MESSAGE("[SKIP] oracle_dual_overlap_NE: PARI/GP `gp` not found on PATH.");
        at::verify::g_oracle_skipped = true;
        return;
    }
    std::vector<at::ell::EcCurve> curves;
    try {
        curves = at::ell::load_ecdata_range(AT_ECDATA_DIR, 2500, 4000);
    } catch (const std::exception& e) {
        MESSAGE("[SKIP] oracle_dual_overlap_NE: ecdata unavailable (" << e.what() << ").");
        at::verify::g_oracle_skipped = true;
        return;
    }
    MESSAGE("oracle: using gp at " << *gp << "; " << curves.size()
            << " Cremona classes in [2500,4000]");

    std::ostringstream script;
    for (const at::ell::EcCurve& c : curves) {
        const at::ell::Curve& m = c.model;
        script << "E=ellinit([" << m.a1 << "," << m.a2 << "," << m.a3 << ","
               << m.a4 << "," << m.a6 << "]);print(ellglobalred(E)[1],\" \",ellrootno(E))\n";
    }
    const std::string out = oracle::run_gp(*gp, script.str());

    std::istringstream lines(out);
    std::string line;
    u64 checked = 0, N_ok = 0, eps_ok = 0;
    for (const at::ell::EcCurve& c : curves) {
        REQUIRE(std::getline(lines, line));
        long long Np = 0, eps = 0;
        std::istringstream(line) >> Np >> eps;
        const int eps_rank = (c.rank % 2 == 0) ? 1 : -1;
        CHECK(Np == static_cast<long long>(c.N));            // PARI conductor == ecdata N
        CHECK(eps == eps_rank);                              // PARI root number == (−1)^rank
        N_ok += (Np == static_cast<long long>(c.N)) ? 1 : 0;
        eps_ok += (eps == eps_rank) ? 1 : 0;
        ++checked;
    }
    MESSAGE("oracle_dual_overlap_NE: certified " << checked << " Cremona-range curves — "
            "PARI conductor==ecdata N (" << N_ok << "/" << checked << "), "
            "PARI ellrootno==(−1)^rank (" << eps_ok << "/" << checked << ")");
    REQUIRE(checked >= 1000);
    REQUIRE(N_ok == checked);
    REQUIRE(eps_ok == checked);
}
