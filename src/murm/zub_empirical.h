#pragma once
#include <cstdint>
#include <vector>

#include "mform/hurwitz.h"

// Stage M3 (completion) — the EMPIRICAL side of the Zubrilina murmuration: the
// short-interval statistic of Theorem 1 (Z25 p.2), assembled from the class-number
// trace formula (mform::trace) over square-free levels, normalized by the newform
// dimension (mform::dimension). ZERO external eigenvalue data — the empirical curve
// IS the trace formula evaluated per level and averaged (m3-pinning §P6):
//
//   emp(P) = [ Σ□_{N∈[X,X+Y], P∤N} Tr_N(P) ] / [ Σ□_{N∈[X,X+Y], P∤N} dim S₂^new(N) ],
//   one point per prime P with y = P/X ≤ y_max   (Theorem 1, k=2)  →  M₂(y).
//
// Authored independently of the formula side (murm::zub_density); this is the
// separate-TU protocol's empirical half. Kinks at y = n²/4 (the teeth) come from
// the r-sum upper limit ⌊2√(P/N)⌋ switching as 2√y crosses an integer.
namespace at::murm {

struct ZubPoint {
    double y;      // P/X
    double p;      // the prime P
    double emp;    // empirical ε-weighted normalized average
    double dens;   // M₂(y) from the formula side (overlay target)
};

struct ZubEmpirical {
    double X = 0;
    long long win_lo = 0, win_hi = 0;   // level window [X, X+Y]
    int n_levels = 0;                   // # square-free levels in the window
    long long max_arg = 0;              // largest Hurwitz argument 4PN actually hit
    std::vector<ZubPoint> points;       // one per prime P (ascending)
    double l2 = 0;                      // RMS(emp − dens) over the points (reported empiric)
};

// One point per prime P ≤ y_max·X; window [X, X+Y]. `dens_prime_bound` truncates
// the M₂ Euler products (formula side). Builds its own Hurwitz sieve sized to the
// max argument.
ZubEmpirical zub_empirical_short(double X, long long Y, double y_max,
                                 unsigned dens_prime_bound = 50000);

// Same, but reusing a prebuilt sieve (H.bound() must cover 4·Pmax·(X+Y)) — lets the
// convergence-trend evaluate several X against one sieve without rebuilding.
ZubEmpirical zub_empirical_with_table(double X, long long Y, double y_max,
                                      const at::mform::HurwitzTable& H,
                                      unsigned dens_prime_bound = 50000);

// R4 kink detector for the EMPIRICAL curve (committed pre-run). The per-prime
// scatter fluctuates (a short window holds only ~10² exact newform traces), so it
// is first smoothed onto a uniform y-grid, then the derivative-steepening events
// (teeth) are read off as local maxima of the positive second difference. The teeth
// at y=n²/4 are √-cusps (the r-sum's upper limit switching); the detector also fires
// on M₂'s intrinsic smooth oscillation maxima, which the finite empirical resolution
// cannot separate from the cusps — reported, not fitted away.
struct KinkDetector {
    double y_lo, y_hi;        // detection range
    double grid;              // uniform resample step
    double smooth_hw;         // moving-average half-width (in y)
    double threshold_frac;    // keep local maxima of Δ² above this · max|Δ²|
    double min_sep;           // thin detections to one per this window
};

// Moving-average of the per-prime scatter onto the uniform grid [y_lo, y_hi] step
// `grid`, half-width `smooth_hw`. Overlaid on M₂ in the viewer; input to detect_teeth.
struct SmoothCurve { std::vector<double> y, v; };
SmoothCurve smooth_empirical(const ZubEmpirical& e, const KinkDetector& kd);

// The teeth: y where the smoothed curve's second difference is a local max above the
// threshold, thinned by min_sep. Upward (convex, +Δ²) corners only — the √-onset sign.
std::vector<double> detect_teeth(const SmoothCurve& s, const KinkDetector& kd);

}  // namespace at::murm
