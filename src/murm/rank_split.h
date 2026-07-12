#pragma once
#include <string>
#include <vector>

#include "core/int_types.h"
#include "murm/ss_empirical.h"

// Stage M5 / PR-2 step 3 — the analytic-rank split of the height-murmuration statistic.
// The re-aggregation code (PR-2 Amendment 2): the a_p work is NEVER recomputed — the
// committed per-curve partials (data/m5/ss_partials_x65536.txt) are FILTERED by the
// analytic-rank column (data/m5/rank_cache_x65536.txt) and re-summed through the SAME
// committed `ss_aggregate` / `extract_shape` (nothing in the statistic TU is touched).
//
// Two axes, kept separate (PR-2 Amendment 4; m4-pinning §R0c):
//   PRIMARY  (u-space, well-powered): the LEAVE-OUT trough recovery. D̂ over F∖S₂ (all
//     curves of analytic rank ≠ 2). "Recovers" iff |trough_u(F∖S₂) − 0.805| ≤ τ=0.06 AND
//     is ≥ one bin (Δu) closer to 0.805 than the full-family trough (dev 0.0825).
//   SECONDARY (value-space, f₂-limited): the S₂-vs-S₀ descending-branch (u∈[0.7,0.9]) mean
//     gap. Downward-significant iff gap = mean D̂_{S₂} − mean D̂_{S₀} < −threshold (the
//     committed 0.668, PR-2 step 2), one-sided (downward). Sub-threshold ⇒ pre-declared
//     INCONCLUSIVE (never a value-space null; the primary carries the verdict).
namespace at::murm {

using at::core::i64;

struct RankSplit {
    double X = 0, du = 0, tol = 0, target_trough = 0;
    double u_lo = 0, u_hi = 0, contrast_threshold = 0;

    SSEmpirical full, leaveout, s2, s0;   // D̂ for the full family, F∖S₂, S₂, S₀
    i64 n_full = 0, n_leaveout = 0, n_s2 = 0, n_s0 = 0;

    // PRIMARY (u-space)
    double dev_full = 0, dev_leaveout = 0;   // |trough_u − target|
    bool recovers = false;

    // SECONDARY (value-space)
    double mean_s2 = 0, mean_s0 = 0, gap = 0;   // descending-branch window means; gap = s2 − s0
    bool contrast_downward = false;

    std::string primary_branch, secondary_branch;
};

// `rank` is aligned to P.A (rank[c] = analytic rank of curve c). Thresholds are the
// committed PR-2 values, passed in (not baked here) so the caller reads them from the doc.
RankSplit rank_split(const SSPartials& P, const std::vector<int>& rank, double X,
                     double target_trough, double tol, double contrast_threshold,
                     double u_lo, double u_hi);

}  // namespace at::murm
