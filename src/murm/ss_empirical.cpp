#include "murm/ss_empirical.h"

#include <cmath>
#include <thread>
#include <vector>

#include "ell/ap_fast.h"
#include "ell/curve.h"
#include "murm/height_family.h"

// NOTE: this TU holds ONLY the statistic (the numbers in the committed run). Its
// sha256 is the run's generator hash (wired in CMake). The committed-file IO, the
// generator-hash accessor, and the windowed-trough diagnostic live in ss_run_io.cpp
// so that editing the reader/format/diagnostic does NOT invalidate a committed run.
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

SSPartials ss_empirical_partials(const std::vector<NeRow>& rows, double du, int n_threads) {
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

    // Prime-major, primes interleaved across threads (thread t ← primes t, t+T, …)
    // for load balance; each thread writes its OWN per-curve partials, then reduced
    // in thread order → deterministic given n_threads.
    std::vector<std::vector<std::vector<double>>> tnum(
        n_threads, std::vector<std::vector<double>>(C, std::vector<double>(NB, 0.0)));
    std::vector<std::vector<std::vector<i64>>> tcnt(
        n_threads, std::vector<std::vector<i64>>(C, std::vector<i64>(NB, 0)));

    auto worker = [&](int t) {
        for (std::size_t pi = static_cast<std::size_t>(t); pi < primes.size();
             pi += static_cast<std::size_t>(n_threads)) {
            const i64 p = primes[pi];
            if (p <= 3) continue;
            const at::ell::QRTable qr = at::ell::build_qr_table(static_cast<at::core::u64>(p));
            for (std::size_t c = 0; c < C; ++c) {
                if (P.N[c] < p) continue;             // u = p/N ≤ 1 only
                if (disc[c] % p == 0) continue;       // bad reduction (p | N for p>3 here)
                const double u = static_cast<double>(p) / static_cast<double>(P.N[c]);
                int b = static_cast<int>(u / du);
                if (b < 0 || b >= NB) continue;
                const at::ell::Curve E{0, 0, 0, P.A[c], P.B[c]};
                const int a = at::ell::ap_fast(E, qr);
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

SSEmpirical ss_aggregate(const SSPartials& P, double X) {
    const i64 Xi = static_cast<i64>(X);
    SSEmpirical out;
    out.X = X;
    out.du = P.du;
    out.u_mid.resize(P.NB);
    out.density.assign(P.NB, 0.0);
    out.count.assign(P.NB, 0);
    for (int b = 0; b < P.NB; ++b) out.u_mid[b] = (b + 0.5) * P.du;

    std::vector<double> acc(P.NB, 0.0);
    for (std::size_t c = 0; c < P.A.size(); ++c) {
        if (naive_height(P.A[c], P.B[c]) > Xi) continue;   // height filter
        out.n_curves += 1;
        for (int b = 0; b < P.NB; ++b) { acc[b] += P.num[c][b]; out.count[b] += P.cnt[c][b]; }
    }
    if (out.n_curves > 0) {
        const double denom = P.du * static_cast<double>(out.n_curves);
        for (int b = 0; b < P.NB; ++b) out.density[b] = acc[b] / denom;
    }
    out.shape = extract_shape(out.u_mid, out.density);
    return out;
}

SSEmpirical ss_empirical(const std::vector<NeRow>& rows, double X, double du, int n_threads) {
    return ss_aggregate(ss_empirical_partials(rows, du, n_threads), X);
}

}  // namespace at::murm
