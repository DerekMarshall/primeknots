// Stage M0 — oracle: our computed a_p refereed by PARI/GP `ellap`, over a sample
// of the (curve, prime) grid. Good primes go through the frozen charsum referee;
// bad primes through the pinned Atkin–Lehner → a_q conversion (m0-pinning §3c,
// the live half of the adjudication). SKIPs cleanly if gp is absent (rule 3).
#include "doctest/doctest.h"

#include <sstream>
#include <string>
#include <vector>

#include "ell/ap.h"
#include "ell/curve.h"
#include "oracle/pari.h"
#include "harness.h"

using at::core::i128;
using at::core::u64;
using namespace at::ell;

static std::string model_str(const Curve& E) {
    std::ostringstream s;
    s << "[" << E.a1 << "," << E.a2 << "," << E.a3 << "," << E.a4 << "," << E.a6 << "]";
    return s.str();
}

TEST_CASE("oracle_ellap") {
    std::optional<std::string> gp = oracle::find_gp();
    if (!gp) {
        MESSAGE("[SKIP] oracle_ellap: PARI/GP `gp` not found on PATH — oracle unavailable.");
        at::verify::g_oracle_skipped = true;
        return;
    }
    MESSAGE("oracle: using gp at " << *gp);

    // Curated sample; models quoted from allcurves.00000-09999 @ 25cec5ec… .
    // Spans ranks 0–3 (37a1 r1, 389a1 r2, 5077a1 r3) — a spread the referee
    // must handle uniformly (rank is irrelevant to a_p at good primes).
    struct Cv { const char* label; Curve E; };
    const std::vector<Cv> curves = {
        {"11a1", {0, -1, 1, -10, -20}}, {"14a1", {1, 0, 1, 4, -6}},
        {"15a1", {1, 1, 1, -10, -10}},  {"17a1", {1, -1, 1, -1, -14}},
        {"19a1", {0, 1, 1, -9, -15}},   {"37a1", {0, 0, 1, -1, 0}},
        {"389a1", {0, 1, 1, -2, 0}},    {"5077a1", {0, 0, 1, -7, 6}},
    };
    // Includes p ∈ {2,3}: the small-prime enumeration path (m0-pinning §5) is
    // refereed by PARI too, not just the p>3 charsum referee.
    const std::vector<u64> primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41,
                                     43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};

    // Good-prime jobs (p ∤ Δ_min ⇔ good reduction, for a minimal model). p ∈ {2,3}
    // go through ap_enumerate (long model); p > 3 through the frozen charsum referee.
    struct Job { u64 p; int ours; };
    std::vector<Job> good_jobs;
    std::ostringstream script;
    for (const Cv& c : curves) {
        const i128 D = discriminant(c.E);
        for (u64 p : primes) {
            if (D % static_cast<i128>(p) == 0) continue;  // bad prime: handled below
            const int ours = (p <= 3) ? ap_enumerate(c.E, p) : ap_charsum(c.E, p);
            good_jobs.push_back({p, ours});
            script << "print(ellap(ellinit(" << model_str(c.E) << ")," << p << "))\n";
        }
    }

    // Bad-prime conversion jobs (live 3c cross-check), (model, q, w, ord) from §3c.
    struct Bad { const char* model; u64 q; int w; int ord; };
    const std::vector<Bad> bads = {
        {"[0,-1,1,-10,-20]", 11, -1, 1}, {"[1,0,1,4,-6]", 2, 1, 1},
        {"[1,0,1,4,-6]", 7, -1, 1},      {"[0,0,1,0,-7]", 3, -1, 3},
        {"[0,0,0,4,0]", 2, -1, 5},
    };
    std::vector<int> bad_ours;
    for (const Bad& b : bads) {
        bad_ours.push_back(ap_from_atkin_lehner(b.w, b.ord));
        script << "print(ellap(ellinit(" << b.model << ")," << b.q << "))\n";
    }

    const std::string out = oracle::run_gp(*gp, script.str());
    std::istringstream lines(out);
    std::string line;

    u64 good_matched = 0, bad_matched = 0;
    for (const Job& j : good_jobs) {
        REQUIRE(std::getline(lines, line));
        CHECK(j.ours == std::stoi(line));
        ++good_matched;
    }
    for (size_t i = 0; i < bads.size(); ++i) {
        REQUIRE(std::getline(lines, line));
        CHECK(bad_ours[i] == std::stoi(line));
        ++bad_matched;
    }

    MESSAGE("oracle_ellap: " << good_matched
            << " good-prime a_p (charsum p>3 + enumeration p∈{2,3}) + " << bad_matched
            << " bad-prime a_q (A–L conversion) matched PARI ellap");
    REQUIRE(good_matched == good_jobs.size());   // whole batch consumed
    REQUIRE(good_jobs.size() == 192);            // 25 primes × 8 curves − 8 bad (curve,p)
    REQUIRE(bad_matched == 5);
}
