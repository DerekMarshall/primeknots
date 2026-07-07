// Stage 0 — oracle: our Jacobi/Legendre symbols refereed by PARI/GP's
// kronecker(). SKIPs cleanly and visibly if gp is absent (CLAUDE.md rule 3).
#include "doctest/doctest.h"

#include <sstream>
#include <string>
#include <vector>

#include "core/modpow.h"
#include "oracle/pari.h"
#include "symbols/jacobi.h"
#include "symbols/legendre.h"
#include "harness.h"

using namespace at::core;
using namespace at::symbols;

TEST_CASE("oracle_kronecker") {
    std::optional<std::string> gp = oracle::find_gp();
    if (!gp) {
        // Visible SKIP: verify_main maps this (no failures) to exit 77 so ctest
        // reports Skipped, never a silent pass. See open-questions.md R4.
        MESSAGE("[SKIP] oracle_kronecker: PARI/GP `gp` not found on PATH — "
                "oracle unavailable.");
        at::verify::g_oracle_skipped = true;
        return;
    }
    MESSAGE("oracle: using gp at " << *gp);

    // Sample (a, n) pairs over odd moduli n (primes and odd composites,
    // including n = 1 and non-coprime a). For odd positive n, kronecker(a,n)
    // coincides with our jacobi(a,n); one gp spawn covers the whole batch.
    struct Pair { u64 a, n; };
    std::vector<u64> moduli = {1,   3,   5,   7,    9,    11,   15,   21,  25,
                               35,  45,  63,  91,   105,  143,  255,  1001,
                               9973, 9999, 10007};
    std::vector<Pair> pairs;
    u64 state = 0x1234ABCDULL;
    for (u64 n : moduli) {
        for (int k = 0; k < 25; ++k) {
            state = state * 6364136223846793005ULL + 1442695040888963407ULL;
            u64 a = (state >> 1) % (3 * n + 1);  // includes a >= n and a = 0
            pairs.push_back({a, n});
        }
    }

    std::ostringstream script;
    for (const Pair& pr : pairs) {
        script << "print(kronecker(" << pr.a << ", " << pr.n << "))\n";
    }

    std::string out = oracle::run_gp(*gp, script.str());

    std::istringstream lines(out);
    std::string line;
    u64 matched = 0;
    for (const Pair& pr : pairs) {
        REQUIRE(std::getline(lines, line));
        int oracle_value = std::stoi(line);
        CHECK(jacobi(pr.a, pr.n) == oracle_value);
        ++matched;
    }
    MESSAGE("cases: " << matched
                      << " Jacobi values matched PARI/GP kronecker()");
    REQUIRE(matched == pairs.size());  // whole batch consumed and compared
    REQUIRE(pairs.size() == moduli.size() * 25);

    // Also referee Legendre directly against kronecker on a batch of primes.
    std::vector<u64> primes = {13, 61, 937, 101, 103, 107, 109, 9973, 10007};
    std::ostringstream lscript;
    struct LPair { u64 a, p; };
    std::vector<LPair> lpairs;
    for (u64 p : primes) {
        for (u64 a = 0; a < 12; ++a) {
            lpairs.push_back({a, p});
            lscript << "print(kronecker(" << a << ", " << p << "))\n";
        }
    }
    std::string lout = oracle::run_gp(*gp, lscript.str());
    std::istringstream llines(lout);
    u64 lmatched = 0;
    for (const LPair& pr : lpairs) {
        REQUIRE(std::getline(llines, line));
        CHECK(legendre_euler(pr.a, pr.p) == std::stoi(line));
        ++lmatched;
    }
    MESSAGE("cases: " << lmatched
                      << " Legendre values matched PARI/GP kronecker()");
    REQUIRE(lmatched == primes.size() * 12);
}

// oracle: core::modpow refereed by PARI/GP's Mod(a,m)^e, a fully independent
// modular-exponentiation implementation. This closes the shared-ladder caveat
// in docs/notes/twin-independence.md: modpow and modpow_naive share the
// square-and-multiply skeleton, so gp is the outside witness that the ladder
// itself is correct. Exercises both the odd (Montgomery) and even (direct)
// modulus paths, with large exponents to stress the ladder.
TEST_CASE("oracle_modpow") {
    std::optional<std::string> gp = oracle::find_gp();
    if (!gp) {
        MESSAGE("[SKIP] oracle_modpow: PARI/GP `gp` not found on PATH.");
        at::verify::g_oracle_skipped = true;
        return;
    }

    struct T { u64 a, m, e; };
    std::vector<u64> mods = {13ULL,    61ULL,     937ULL,   65'537ULL,
                             9'973ULL, 1'000'000'007ULL,           // odd primes
                             15ULL,    100ULL,    1'000ULL,        // odd/even composites
                             4'294'967'296ULL,                     // 2^32, even -> direct path
                             1'000'000ULL};
    std::vector<u64> exps = {0ULL,     1ULL,      2ULL,     3ULL,   7ULL,
                             63ULL,    64ULL,     1'000ULL, 1'000'000ULL,
                             4'294'967'295ULL, 1'000'000'000'000ULL};  // stress the ladder
    std::vector<T> ts;
    u64 state = 0x9E3779B97F4A7C15ULL;
    for (u64 m : mods) {
        for (u64 e : exps) {
            for (int k = 0; k < 3; ++k) {
                state = state * 6364136223846793005ULL + 1442695040888963407ULL;
                ts.push_back({(state >> 1) % m, m, e});
            }
        }
    }

    std::ostringstream script;
    for (const T& t : ts) {
        script << "print(lift(Mod(" << t.a << ", " << t.m << ")^" << t.e
               << "))\n";
    }
    std::string out = oracle::run_gp(*gp, script.str());

    std::istringstream lines(out);
    std::string line;
    u64 matched = 0;
    for (const T& t : ts) {
        REQUIRE(std::getline(lines, line));
        CHECK(modpow(t.a, t.e, t.m) == std::stoull(line));
        ++matched;
    }
    MESSAGE("cases: " << matched
                      << " modpow values matched PARI/GP Mod(a,m)^e");
    REQUIRE(matched == ts.size());
    REQUIRE(matched == mods.size() * exps.size() * 3);
}
