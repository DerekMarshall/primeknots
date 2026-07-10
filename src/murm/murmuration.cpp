#include "murm/murmuration.h"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <stdexcept>
#include <string>
#include <thread>

#include "ell/ap.h"
#include "ell/ap_fast.h"

namespace at::murm {

using at::ell::ap_enumerate;
using at::ell::ap_fast;
using at::ell::ap_from_atkin_lehner;
using at::ell::build_qr_table;
using at::ell::QRTable;

namespace {

// First `count` primes, ascending (sieve to a safe bound).
std::vector<u64> first_primes(std::size_t count) {
    if (count == 0) return {};
    // p_n < n(ln n + ln ln n) for n ≥ 6; pad generously.
    double n = static_cast<double>(count);
    std::size_t bound = count < 6 ? 15
                                  : static_cast<std::size_t>(n * (std::log(n) + std::log(std::log(n))) * 1.3) + 20;
    std::vector<bool> comp(bound + 1, false);
    for (std::size_t i = 2; i * i <= bound; ++i)
        if (!comp[i])
            for (std::size_t j = i * i; j <= bound; j += i) comp[j] = true;
    std::vector<u64> ps;
    for (std::size_t k = 2; k <= bound && ps.size() < count; ++k)
        if (!comp[k]) ps.push_back(k);
    return ps;
}

int valuation(u64 N, u64 p) {
    int v = 0;
    while (N % p == 0) { N /= p; ++v; }
    return v;
}

// a_p for one class at prime p (m0-pinning: computed good primes, §3-converted bad).
int ap_of(const EcCurve& c, u64 p, const QRTable* qr) {
    if (c.N % p == 0) {                      // bad prime
        auto it = c.bad_w.find(p);
        if (it == c.bad_w.end())
            throw std::runtime_error("murmuration: missing Atkin–Lehner sign for bad prime "
                                     + std::to_string(p) + " of " + c.label);
        return ap_from_atkin_lehner(it->second, valuation(c.N, p));
    }
    if (p <= 3) return ap_enumerate(c.model, p);   // small-prime path (§5)
    return ap_fast(c.model, *qr);                  // fast path (p > 3)
}

// Linear interpolation of avg vs y over ascending points; clamped at the ends.
double interp(const std::vector<MurmPoint>& pts, double y) {
    if (y <= pts.front().y) return pts.front().avg;
    if (y >= pts.back().y) return pts.back().avg;
    std::size_t lo = 0, hi = pts.size() - 1;
    while (hi - lo > 1) {
        std::size_t mid = (lo + hi) / 2;
        (pts[mid].y <= y ? lo : hi) = mid;
    }
    const double t = (y - pts[lo].y) / (pts[hi].y - pts[lo].y);
    return pts[lo].avg + t * (pts[hi].avg - pts[lo].avg);
}

}  // namespace

std::vector<const EcCurve*> filter_family(const std::vector<EcCurve>& all,
                                          int rank, u64 N1, u64 N2) {
    std::vector<const EcCurve*> fam;
    for (const EcCurve& c : all)
        if (c.rank == rank && c.N >= N1 && c.N <= N2) fam.push_back(&c);
    return fam;
}

MurmCurve murmuration_curve(const std::vector<EcCurve>& all, int rank,
                            u64 N1, u64 N2, std::size_t n_primes) {
    const std::vector<const EcCurve*> fam = filter_family(all, rank, N1, N2);
    const std::vector<u64> primes = first_primes(n_primes);

    MurmCurve curve;
    curve.rank = rank;
    curve.N1 = N1;
    curve.N2 = N2;
    curve.family_size = fam.size();
    curve.points.assign(primes.size(), MurmPoint{});
    if (fam.empty()) return curve;

    auto do_prime = [&](std::size_t idx) {
        const u64 p = primes[idx];
        QRTable qr;
        if (p > 3) qr = build_qr_table(p);
        long long sum = 0;                        // exact integer sum, fixed order
        for (const EcCurve* c : fam) sum += ap_of(*c, p, p > 3 ? &qr : nullptr);
        curve.points[idx] = MurmPoint{p, static_cast<double>(p) / static_cast<double>(N2),
                                      static_cast<double>(sum) / static_cast<double>(fam.size())};
    };

    const unsigned hw = std::max(2u, std::thread::hardware_concurrency());
    std::atomic<std::size_t> next{0};
    auto worker = [&] {
        std::size_t i;
        while ((i = next.fetch_add(1)) < primes.size()) do_prime(i);
    };
    std::vector<std::thread> pool;
    for (unsigned k = 0; k < hw; ++k) pool.emplace_back(worker);
    for (std::thread& th : pool) th.join();
    return curve;
}

CollapseResult scale_collapse(const MurmCurve& A, const MurmCurve& B, int grid_points) {
    const double ylo = std::max(A.points.front().y, B.points.front().y);
    const double yhi = std::min(A.points.back().y, B.points.back().y);
    CollapseResult r;
    r.grid_points = grid_points;
    if (!(yhi > ylo) || grid_points < 2) return r;

    double sse = 0, fvar = 0;
    for (int g = 0; g < grid_points; ++g) {
        const double y = ylo + (yhi - ylo) * g / (grid_points - 1);
        const double d = interp(A.points, y) - interp(B.points, y);
        sse += d * d;
        // Var(mean) ≈ p / |E| with p = y·N2 (Var(a_p) ~ p, HLOP §3.2); two-family
        // floor. F assumes a_p independent across the family — anticonservative,
        // since the murmuration IS cross-family correlation (m1-pinning P4 / R1a).
        fvar += y * A.N2 / static_cast<double>(A.family_size)
              + y * B.N2 / static_cast<double>(B.family_size);
    }
    r.rms = std::sqrt(sse / grid_points);
    r.floor_F = std::sqrt(fvar / grid_points);
    r.ratio = r.floor_F > 0 ? r.rms / r.floor_F : 0;
    return r;
}

}  // namespace at::murm
