#include "murm/ss_empirical.h"

#include <cmath>
#include <thread>
#include <vector>

#include "ell/ap_shanks_mestre.h"
#include "ell/curve.h"
#include "murm/height_family.h"

// M0b production-path a_p provider for statistic (1) — the SEPARATE-TU twin of
// ss_empirical_partials (m0b-pinning §7; PRODUCTION WIRING GATE 2026-07-17). This file is
// a byte-exact copy of ss_empirical_partials with EXACTLY ONE line changed: the a_p source
// is `ell::ap_shanks_mestre` (O(p^{1/4}) point counting) instead of `ell::ap_fast` (O(p)
// character sum). Everything else — the prime-major iteration, the thread interleaving, the
// per-thread accumulator layout, the reduction order, and the exact float expression
// (u_mid·lnN/N)·ε·a_p — is identical, so the emitted partials are BYTE-IDENTICAL to the
// ap_fast partials (a_p are equal integers; the twins prove that). The gate: an M0b-backed
// ss-run reproduces the committed 2¹⁶/2¹⁷ artifacts exactly on the same platform + thread count.
//
// It lives in its OWN translation unit on purpose: ss_empirical.cpp (the hashed statistic
// source, SS_GENERATOR_HASH) is left UNTOUCHED, so the committed runs stay readable and CI
// stays green. The default a_p provider is unchanged (ap_fast); --ap=m0b opts in. NO
// build_qr_table here (Shanks–Mestre needs no residue table); the p≤229 band is handled
// inside ap_shanks_mestre (charsum fallback). Bad primes (p | 4A³+27B²) are skipped BEFORE
// the a_p call, exactly as in the fast path, so ap_shanks_mestre never sees a bad reduction.
namespace at::murm {

namespace {
std::vector<i64> primes_upto(i64 N) {
    std::vector<char> sieve(N + 1, 1);
    std::vector<i64> ps;
    for (i64 i = 2; i <= N; ++i)
        if (sieve[i]) { ps.push_back(i); for (i64 j = i * i; j <= N; j += i) sieve[j] = 0; }
    return ps;
}
}  // namespace

SSPartials ss_empirical_partials_m0b(const std::vector<NeRow>& rows, double du, int n_threads) {
    if (n_threads < 1) n_threads = 1;
    const int NB = static_cast<int>(std::lround(1.0 / du));
    const std::size_t C = rows.size();

    SSPartials P;
    P.du = du;
    P.NB = NB;
    P.A.resize(C); P.B.resize(C); P.N.resize(C);
    P.num.assign(C, std::vector<double>(NB, 0.0));
    P.cnt.assign(C, std::vector<i64>(NB, 0));

    std::vector<i64> disc(C);
    std::vector<double> lnN_over_N(C);
    i64 maxN = 0;
    for (std::size_t c = 0; c < C; ++c) {
        P.A[c] = rows[c].A; P.B[c] = rows[c].B; P.N[c] = rows[c].N;
        disc[c] = 4 * rows[c].A * rows[c].A * rows[c].A + 27 * rows[c].B * rows[c].B;
        lnN_over_N[c] = std::log(static_cast<double>(rows[c].N)) / static_cast<double>(rows[c].N);
        if (rows[c].N > maxN) maxN = rows[c].N;
    }
    if (C == 0 || maxN < 5) return P;

    const std::vector<i64> primes = primes_upto(maxN);

    // Prime-major, primes interleaved across threads (thread t ← primes t, t+T, …) — IDENTICAL
    // order and reduction to ss_empirical_partials, so the float partials match bit-for-bit.
    std::vector<std::vector<std::vector<double>>> tnum(
        n_threads, std::vector<std::vector<double>>(C, std::vector<double>(NB, 0.0)));
    std::vector<std::vector<std::vector<i64>>> tcnt(
        n_threads, std::vector<std::vector<i64>>(C, std::vector<i64>(NB, 0)));

    auto worker = [&](int t) {
        for (std::size_t pi = static_cast<std::size_t>(t); pi < primes.size();
             pi += static_cast<std::size_t>(n_threads)) {
            const i64 p = primes[pi];
            if (p <= 3) continue;
            for (std::size_t c = 0; c < C; ++c) {
                if (P.N[c] < p) continue;             // u = p/N ≤ 1 only
                if (disc[c] % p == 0) continue;       // bad reduction (p | N for p>3 here)
                const double u = static_cast<double>(p) / static_cast<double>(P.N[c]);
                int b = static_cast<int>(u / du);
                if (b < 0 || b >= NB) continue;
                const at::ell::Curve E{0, 0, 0, P.A[c], P.B[c]};
                const int a = at::ell::ap_shanks_mestre(E, static_cast<at::core::u64>(p));
                const double u_mid = (b + 0.5) * du;
                tnum[t][c][b] += (u_mid * lnN_over_N[c]) * static_cast<double>(rows[c].eps) *
                                 static_cast<double>(a);
                tcnt[t][c][b] += 1;
            }
        }
    };

    std::vector<std::thread> pool;
    for (int t = 1; t < n_threads; ++t) pool.emplace_back(worker, t);
    worker(0);
    for (std::thread& th : pool) th.join();

    for (int t = 0; t < n_threads; ++t)
        for (std::size_t c = 0; c < C; ++c)
            for (int b = 0; b < NB; ++b) { P.num[c][b] += tnum[t][c][b]; P.cnt[c][b] += tcnt[t][c][b]; }
    return P;
}

}  // namespace at::murm
