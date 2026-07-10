#pragma once
#include <cstddef>
#include <vector>

#include "core/int_types.h"
#include "ell/ecdata.h"

// Stage M1 — family definition + per-prime averaging (RESEARCH-M §3, m1-pinning).
// murm depends on ell (a_p machinery) only; no ordering/density concepts beyond the
// conductor family HLOP Figure 1 uses. The statistic is HLOP eq. (1.1): the
// UNWEIGHTED arithmetic mean of a_p over rank-r isogeny classes with N ∈ [N1,N2].
namespace at::murm {

using at::core::u64;
using at::ell::EcCurve;

struct MurmPoint {
    u64 p;        // the prime
    double y;     // p / N2 (the scale-invariance axis, m1-pinning P4)
    double avg;   // arithmetic mean of a_p over the family
};

struct MurmCurve {
    int rank = 0;
    u64 N1 = 0, N2 = 0;
    std::size_t family_size = 0;
    std::vector<MurmPoint> points;   // one per prime, ascending
};

// Rank-r isogeny classes with N ∈ [N1,N2] (the loader already gives one rep/class).
std::vector<const EcCurve*> filter_family(const std::vector<EcCurve>& all,
                                          int rank, u64 N1, u64 N2);

// f_r over the first n_primes primes: arithmetic mean of a_p across the family.
// Good primes via the M0 fast path (p>3) / enumeration (p∈{2,3}); bad primes p|N
// via the pinned Atkin–Lehner conversion (m0-pinning §3). Parallel over primes,
// deterministic (each prime's integer sum is taken in a fixed curve order).
MurmCurve murmuration_curve(const std::vector<EcCurve>& all, int rank,
                            u64 N1, u64 N2, std::size_t n_primes);

// Scale-collapse of two same-rank curves from disjoint ranges, compared on a shared
// grid of y = p/N2 (linear interpolation). Reports the RMS distance, the
// sampling-noise floor F, and the ratio RMS/F (m1-pinning P4 / rider R1). F is the
// two-family noise floor: F² = mean_y( p_A(y)/|E_A| + p_B(y)/|E_B| ), Var(a_p)≈p.
struct CollapseResult {
    double rms = 0;        // L² distance of the two curves in y
    double floor_F = 0;    // sampling-noise floor
    double ratio = 0;      // rms / F  (pass iff < 3, the pinned T = 3F)
    int grid_points = 0;
};
CollapseResult scale_collapse(const MurmCurve& A, const MurmCurve& B, int grid_points);

// The null control (rider R1c) is the SAME statistic applied to a wrong-label
// pairing: scale_collapse(A_r, B_r') with r' an antiphase (opposite-parity) rank.
// Because even- and odd-rank murmurations oscillate in antiphase (HLOP), a genuine
// collapse of A_r onto B_r must NOT collapse A_r onto B_r' — so the null must FAIL
// the tolerance where the test has power (rank 0,1; rank 2 is low-power, R2). No
// separate function is needed: the caller passes the mismatched-rank curve.

}  // namespace at::murm
