#include "zeta/zero_finder.h"

#include <algorithm>
#include <cmath>

#include "core/constants.h"
#include "zeta/riemann_siegel.h"

namespace at::zeta {
namespace {

using at::core::kPi;   // generated (oracle/gen_constants.py), never typed

// Refine a sign-change bracket [lo,hi] of Z to a zero, using the accurate
// Euler-Maclaurin engine (the referee). Bisection: robust, ~50 halvings puts
// the ordinate well below 1e-13.
double refine_zero(double lo, double hi) {
    double zlo = Z_euler_maclaurin(lo);
    for (int i = 0; i < 52; ++i) {
        double mid = 0.5 * (lo + hi);
        double zm = Z_euler_maclaurin(mid);
        if ((zlo < 0) == (zm < 0)) { lo = mid; zlo = zm; } else { hi = mid; }
    }
    return 0.5 * (lo + hi);
}

// Append refined zeros found in (a,b) to `out`, scanning with `n_sub` samples.
// Uses the fast Z() for sign detection; refine_zero uses Euler-Maclaurin.
void scan_range(double a, double b, int n_sub, std::vector<double>& out) {
    double step = (b - a) / n_sub;
    double prev_t = a, prev_z = Z(a);
    for (int i = 1; i <= n_sub; ++i) {
        double t = (i == n_sub) ? b : a + i * step;
        double z = Z(t);
        bool sign_change = (prev_z < 0) != (z < 0);
        if (sign_change) out.push_back(refine_zero(prev_t, t));
        prev_t = t; prev_z = z;
    }
}

// (−1)^n Z(g_n) > 0  ⇒  Gram point g_n is "good" (Gram's law holds locally).
bool gram_good(long n, double gn) {
    double z = Z_euler_maclaurin(gn);
    return ((n & 1) == 0) ? (z > 0) : (z < 0);
}

}  // namespace

double N_main_term(double T) { return theta(T) / kPi + 1.0; }

ZeroFindResult find_zeros(double t_max, double t_min) {
    ZeroFindResult r;
    const bool full = (t_min <= 0.0);

    // Gram points g_n covering the window: start one Gram point at/below t_min
    // (so the first scanned interval brackets the low edge), up to g_M ≤ t_max.
    std::vector<double> g;
    std::vector<long> gidx;
    long n = -1;
    if (!full) {  // advance to the last Gram index with g_n ≤ t_min
        while (gram_point(n + 1) <= t_min) ++n;
    }
    for (;; ++n) {
        double gn = gram_point(n);
        if (gn > t_max) break;
        g.push_back(gn);
        gidx.push_back(n);
    }
    if (g.size() < 2) return r;

    // Initial scan of every Gram interval (g_n, g_{n+1}].
    std::vector<double> zeros;
    for (size_t k = 0; k + 1 < g.size(); ++k) scan_range(g[k], g[k + 1], 8, zeros);
    std::sort(zeros.begin(), zeros.end());

    auto count_below = [&](double x) {
        return static_cast<long>(
            std::lower_bound(zeros.begin(), zeros.end(), x) - zeros.begin());
    };

    // Certify by the block method between consecutive good Gram points g_p,g_q:
    // the count in (g_p,g_q] must equal q−p (Rosser holds through 10⁴ zeros). A
    // deficit ⇒ refine that block (close-pair recovery). With a full scan the
    // first good point also anchors the absolute count == n+1.
    long prev_good_k = -1, prev_good_n = 0;
    long first_good_n = 0, top_good_n = 0;
    double first_good_t = g.front();
    long n_refined = 0;
    bool complete = true;
    bool have_base = false;
    for (size_t k = 0; k < g.size(); ++k) {
        long nn = gidx[k];
        if (!gram_good(nn, g[k])) continue;
        bool ok = true;
        if (!have_base) {
            have_base = true;
            first_good_n = nn;
            first_good_t = g[k];
            // Full scan: the first good point anchors the absolute count == n+1.
            if (full && count_below(g[k]) != nn + 1) ok = false;
        } else {
            long expected = nn - prev_good_n;                     // block width
            long got = count_below(g[k]) - count_below(g[prev_good_k]);
            if (got != expected) {
                double a = g[prev_good_k], b = g[k];
                for (int sub : {64, 512, 4096}) {
                    zeros.erase(std::lower_bound(zeros.begin(), zeros.end(), a),
                                std::lower_bound(zeros.begin(), zeros.end(), b));
                    std::vector<double> block;
                    scan_range(a, b, sub, block);
                    zeros.insert(std::lower_bound(zeros.begin(), zeros.end(), a),
                                 block.begin(), block.end());
                    std::sort(zeros.begin(), zeros.end());
                    if (count_below(g[k]) - count_below(g[prev_good_k]) == expected)
                        break;
                }
                ++n_refined;
                got = count_below(g[k]) - count_below(g[prev_good_k]);
                ok = (got == expected);
            }
        }
        if (ok) { ++r.n_good_gram; r.t_top = g[k]; top_good_n = nn; }
        else complete = false;  // unreconciled block: a deliverable, not smoothed
        prev_good_k = static_cast<long>(k);
        prev_good_n = nn;
    }

    // Certified count: full scan ⇒ N(t_top) = top Gram index + 1; windowed ⇒
    // block-summed count in (first good point, t_top].
    double lo = full ? g.front() : first_good_t;
    r.n_expected = full ? (top_good_n + 1) : (top_good_n - first_good_n);

    // Gram-law exceptions (computed before the move).
    for (size_t k = 0; k + 1 < g.size() && g[k + 1] <= r.t_top; ++k) {
        long c = count_below(g[k + 1]) - count_below(g[k]);
        if (c != 1) ++r.n_gram_law_exceptions;
    }

    // Keep only certified zeros in (lo, t_top].
    r.zeros = std::vector<double>(
        std::upper_bound(zeros.begin(), zeros.end(), lo),
        std::upper_bound(zeros.begin(), zeros.end(), r.t_top));
    r.n_refined_blocks = n_refined;
    r.complete = complete && (static_cast<long>(r.zeros.size()) == r.n_expected);

    for (size_t i = 1; i < r.zeros.size(); ++i) {
        double t = r.zeros[i];
        double mean_gap = 2.0 * kPi / std::log(t / (2.0 * kPi));
        if (r.zeros[i] - r.zeros[i - 1] < 0.4 * mean_gap) ++r.n_close_pairs;
    }
    return r;
}

}  // namespace at::zeta
