#include "murm/rank_split.h"

#include <cmath>

namespace at::murm {

namespace {
// Build a filtered copy of the partials: only curves c with keep(rank[c]) true, in the
// SAME canonical order (so the re-aggregation sum order matches the committed pipeline).
template <typename Pred>
SSPartials filter_partials(const SSPartials& P, const std::vector<int>& rank, Pred keep) {
    SSPartials out;
    out.du = P.du;
    out.NB = P.NB;
    for (std::size_t c = 0; c < P.A.size(); ++c) {
        if (!keep(rank[c])) continue;
        out.A.push_back(P.A[c]); out.B.push_back(P.B[c]); out.N.push_back(P.N[c]);
        out.num.push_back(P.num[c]); out.cnt.push_back(P.cnt[c]);
    }
    return out;
}

// Mean of the density over bins whose centre lies in [u_lo, u_hi] (the descending window).
double window_mean(const SSEmpirical& e, double u_lo, double u_hi) {
    double s = 0; int n = 0;
    for (std::size_t b = 0; b < e.u_mid.size(); ++b)
        if (e.u_mid[b] >= u_lo && e.u_mid[b] <= u_hi) { s += e.density[b]; ++n; }
    return n ? s / n : 0.0;
}
}  // namespace

RankSplit rank_split(const SSPartials& P, const std::vector<int>& rank, double X,
                     double target_trough, double tol, double contrast_threshold,
                     double u_lo, double u_hi) {
    RankSplit r;
    r.X = X; r.du = P.du; r.tol = tol; r.target_trough = target_trough;
    r.u_lo = u_lo; r.u_hi = u_hi; r.contrast_threshold = contrast_threshold;

    // Re-aggregate each subpopulation through the committed ss_aggregate (no a_p recompute).
    r.full     = ss_aggregate(P, X);
    r.leaveout = ss_aggregate(filter_partials(P, rank, [](int rk) { return rk != 2; }), X);
    r.s2       = ss_aggregate(filter_partials(P, rank, [](int rk) { return rk == 2; }), X);
    r.s0       = ss_aggregate(filter_partials(P, rank, [](int rk) { return rk == 0; }), X);
    r.n_full = r.full.n_curves; r.n_leaveout = r.leaveout.n_curves;
    r.n_s2 = r.s2.n_curves; r.n_s0 = r.s0.n_curves;

    // PRIMARY (u-space): leave-out trough recovery.
    r.dev_full = std::abs(r.full.shape.trough_u - target_trough);
    r.dev_leaveout = std::abs(r.leaveout.shape.trough_u - target_trough);
    // Recovers iff ≥ one bin closer to target than the full family AND within τ.
    r.recovers = (r.dev_leaveout <= r.dev_full - P.du + 1e-12) &&
                 (r.dev_leaveout <= tol + 1e-12);

    // SECONDARY (value-space): S₂-vs-S₀ descending-branch mean gap, one-sided (downward).
    r.mean_s2 = window_mean(r.s2, u_lo, u_hi);
    r.mean_s0 = window_mean(r.s0, u_lo, u_hi);
    r.gap = r.mean_s2 - r.mean_s0;
    r.contrast_downward = (r.gap < -contrast_threshold);

    // MEASUREMENT, not a verdict: the PR-2 recovery gate is superseded (it targeted the
    // corrupted 0.805; ERRATA #28). The leave-out trough POSITION relative to the full family
    // is a statement about the empirical curve and stands regardless of the density target.
    r.primary_branch = r.recovers
        ? "leave-out trough moved ≥1 bin toward the density trough (measurement; PR-2 recovery gate superseded, ERRATA #28)"
        : "leave-out trough UNCHANGED from the full family (measurement; PR-2 recovery gate superseded, ERRATA #28)";
    r.secondary_branch = r.contrast_downward
        ? "contrast DOWNWARD-significant (S2 mean < S0 mean by > threshold)"
        : "inconclusive (|gap| below the f2-limited threshold — pre-declared, not a null)";
    return r;
}

}  // namespace at::murm
