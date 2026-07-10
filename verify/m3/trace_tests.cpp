// Stage M3 Phase 2 — the assembled trace formula, adjudicated against M0's a_P and
// the modularity a_f(P) = a_E(P) (the ladder adjudicating itself), riders R1/R2/R3.
//   anchor_trace_dim1        — N∈{11,14,15} (dim 1, ε=+1): trace == a_P(E)·ε.
//   anchor_trace_mixed_sign  — N=37 (37a ε=−1, 37b ε=+1): trace == signed a_P sum (R2).
//   anchor_trace_old_part    — N=33 (new+old): trace == newform only; N=22 (pure old): 0 (R3).
//   theorem_trace_is_integral — the assembled RHS ∈ ℤ over a square-free sweep (P1 sanity).
// Root numbers quoted from PARI ellrootno (m3-pinning); a_P from ell (M0).
#include "doctest/doctest.h"

#include <string>
#include <vector>

#include "core/primes.h"
#include "ell/ap.h"
#include "ell/curve.h"
#include "harness.h"
#include "mform/trace.h"

using at::core::i64;
using at::core::u64;
using namespace at::mform;

namespace {
// a_P of an elliptic curve via M0: enumeration for P∈{2,3}, charsum for P>3.
i64 aP(const at::ell::Curve& E, i64 P) {
    return P <= 3 ? at::ell::ap_enumerate(E, static_cast<u64>(P))
                  : at::ell::ap_charsum(E, static_cast<u64>(P));
}
std::vector<i64> primes_upto(i64 N) {
    std::vector<i64> ps;
    for (i64 p = 2; p <= N; ++p) if (at::core::is_prime(static_cast<u64>(p))) ps.push_back(p);
    return ps;
}
}  // namespace

TEST_CASE("anchor_trace_dim1") {
    // (N, minimal model [ecdata], ε [ellrootno]) — each level has one newform.
    struct L { i64 N; at::ell::Curve E; int eps; };
    const std::vector<L> levels = {
        {11, {0, -1, 1, -10, -20}, +1},   // 11a
        {14, {1, 0, 1, 4, -6}, +1},        // 14a
        {15, {1, 1, 1, -10, -10}, +1},     // 15a
    };
    u64 checked = 0;
    for (const L& l : levels)
        for (i64 P : primes_upto(50)) {
            if (l.N % P == 0) continue;                       // need P ∤ N
            const Frac t = newform_weighted_trace_k2(l.N, P);
            CHECK(t.den == 1);                                // RHS ∈ ℤ
            CHECK(t.num == aP(l.E, P) * l.eps);
            ++checked;
        }
    // referee-verified cells (N=11): P=2→−2, P=3→−1, P=5→1.
    CHECK(newform_weighted_trace_k2(11, 2) == Frac{-2, 1});
    CHECK(newform_weighted_trace_k2(11, 3) == Frac{-1, 1});
    CHECK(newform_weighted_trace_k2(11, 5) == Frac{1, 1});
    MESSAGE("anchor_trace_dim1: assembled trace == a_P(E)·ε over " << checked
            << " (N∈{11,14,15}, P) cells; referee cells N=11 P∈{2,3,5} = −2,−1,1");
    REQUIRE(checked >= 40);
}

TEST_CASE("anchor_trace_mixed_sign") {
    // N=37 carries BOTH root numbers — tests the ε=−1 branch of the P3 sign chain (R2).
    const at::ell::Curve E37a{0, 0, 1, -1, 0};    // 37a, ε=−1 (rank 1)
    const at::ell::Curve E37b{0, 1, 1, -23, -50}; // 37b, ε=+1
    u64 checked = 0;
    for (i64 P : primes_upto(50)) {
        if (37 % P == 0) continue;
        const Frac t = newform_weighted_trace_k2(37, P);
        CHECK(t.den == 1);
        CHECK(t.num == aP(E37a, P) * (-1) + aP(E37b, P) * (+1));   // signed (mixed) sum
        ++checked;
    }
    MESSAGE("anchor_trace_mixed_sign: N=37 assembled trace == a(37a)·(−1)+a(37b)·(+1) over "
            << checked << " P — the ε=−1 branch exercised (R2)");
    REQUIRE(checked >= 12);
}

TEST_CASE("anchor_trace_old_part") {
    // R3: the trace formula gives the NEW part only (old-part vanishing lemma).
    const at::ell::Curve E33a{1, 1, 0, -11, 0};   // 33a, ε=+1 (newform of level 33)
    u64 new_checked = 0, old_checked = 0;
    for (i64 P : primes_upto(50)) {
        if (33 % P == 0) continue;
        // N=33: dim S₂=3 (2 old from 11, 1 new); trace must equal the NEWFORM 33a only.
        CHECK(newform_weighted_trace_k2(33, P) == Frac{aP(E33a, P) * (+1), 1});
        ++new_checked;
    }
    for (i64 P : primes_upto(50)) {
        if (22 % P == 0) continue;
        // N=22: dim S₂=2, all old (from 11), no newform — the newform trace is 0.
        CHECK(newform_weighted_trace_k2(22, P) == Frac{0, 1});
        ++old_checked;
    }
    MESSAGE("anchor_trace_old_part: N=33 trace == newform 33a only (old part 2 excluded) over "
            << new_checked << " P; N=22 (pure old) trace == 0 over " << old_checked << " P (R3)");
    REQUIRE(new_checked >= 12);
    REQUIRE(old_checked >= 12);
}

TEST_CASE("theorem_trace_is_integral") {
    // The assembled RHS (rational H₁ terms + −(P+1)) must be an integer — an
    // oracle-free sanity on the whole formula (m3-pinning §P1).
    const std::vector<i64> Ns = {11, 13, 14, 15, 17, 19, 21, 22, 23, 26, 33, 34, 35, 37, 38, 39};
    u64 checked = 0;
    for (i64 N : Ns)
        for (i64 P : primes_upto(60)) {
            if (N % P == 0) continue;
            CHECK(newform_weighted_trace_k2(N, P).den == 1);
            ++checked;
        }
    MESSAGE("theorem_trace_is_integral: assembled RHS ∈ ℤ over " << checked
            << " (square-free N, P) pairs");
    REQUIRE(checked >= 200);
}
