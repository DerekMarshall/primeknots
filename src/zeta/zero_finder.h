#pragma once

#include <vector>

// Stage 5 — locate the nontrivial zeros and CERTIFY the set is complete
// (RESEARCH.md §7.1, rider R1, stage5-pinning.md §2).
//
// Sign-change scanning of Z(t) alone can step over a close pair (Lehmer). The
// count is therefore certified against N(T) = θ(T)/π + 1 + S(T) via Turing's
// method: in the first-10⁴-zero range Rosser's rule holds, so at every "good"
// Gram point g_n (where (−1)ⁿ Z(g_n) > 0) the number of zeros below g_n equals
// n+1 exactly. We REQUIRE found-count == n+1 at every good Gram point; a deficit
// triggers adaptive refinement of that block (close-pair recovery) and, if it
// cannot be reconciled, is reported as a witness rather than smoothed over.
// Gram points SEED the scan; the block certificate DEFINES the count.

namespace at::zeta {

struct ZeroFindResult {
    std::vector<double> zeros;      // ascending ordinates γ with 0 < γ < t_top
    long n_expected = 0;            // N(t_top) via Turing (= top good Gram index + 1)
    double t_top = 0;               // height certified up to (a good Gram point)
    bool complete = false;          // zeros.size() == n_expected AND all good points check
    long n_good_gram = 0;           // # good Gram points verified (count == n+1)
    long n_gram_law_exceptions = 0; // # Gram intervals not holding exactly one zero
    long n_close_pairs = 0;         // # gaps < 0.4 × local mean spacing (empiric)
    long n_refined_blocks = 0;      // # blocks that needed adaptive close-pair refinement
};

// Locate + certify all zeros with t_min < γ < t_max. t_top is clipped down to
// the largest good Gram point ≤ t_max so the completeness claim is exact. The
// count is certified by the block method between consecutive good Gram points
// (count in (g_p,g_q] == q−p); with t_min=0 the first good point also anchors
// the absolute count (count == n+1), giving N(t_top). A windowed scan (t_min>0)
// certifies completeness within the window cheaply — used for the Lehmer test.
ZeroFindResult find_zeros(double t_max, double t_min = 0.0);

// Riemann–von Mangoldt main term θ(T)/π + 1 (the naive integer estimate that is
// exact only at Gram points; off a Gram point S(T) ≠ 0 and round() can be wrong
// — e.g. T=500 gives 270 but N(500)=269). Exposed for the anchor test.
double N_main_term(double T);

}  // namespace at::zeta
