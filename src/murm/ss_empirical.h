#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "core/int_types.h"
#include "murm/ne_cache.h"
#include "murm/ss_density.h"   // SSShape, extract_shape

// Stage M4 — the EMPIRICAL side of the Sawin–Sutherland height murmuration
// (m4-pinning §P2/§P3; statistic (1) of [SS25]). Authored independently of the
// formula side (ss_density); this file computes the empirical curve, the other the
// conjectured density D(u) they are compared against (separate-TU protocol).
//
// The statistic, over the height family {E : H(E) ≤ X} (each row carries the
// oracle-supplied conductor N and root number ε), binned by u = p/N over (0,1]:
//
//   D̂(u_b) = (1 / (Δu · |fam|)) Σ_{(E,p): p/N(E)∈bin_b}
//                (u_mid · ln N(E) / N(E)) · ε(E) · a_p(E)
//
// with a_p COMPUTED from the model (ell::ap_fast, the M0 fast path, twinned against
// the frozen referee ell::ap_charsum — rule 2; only N, ε are oracle-provenance).
// Good primes p > 3 with p ∤ (4A³+27B²); for this family the reduced short model is
// p-minimal at every p > 3, so p ∤ Δ(model) ⟺ p ∤ N (good reduction) — the model
// discriminant is the exact good-prime test and keeps a_p's domain computed-only.
namespace at::murm {

using at::core::i64;

struct SSEmpirical {
    double X = 0, du = 0;
    i64 n_curves = 0;                 // |fam| at this X (the averaging denominator)
    std::vector<double> u_mid;        // bin centers in (0,1]
    std::vector<double> density;      // D̂(u_mid)
    std::vector<i64> count;           // (curve,prime) samples per bin
    SSShape shape;                    // two-pass hump/zero/trough of D̂
};

// Per-curve bin partials — a_p is computed ONCE per curve (the heavy step), so a
// whole convergence ladder re-aggregates cheaply from a single pass. The prefactor
// (u_mid·ln N/N·ε·a_p) is X-independent; only the height filter and the |fam|
// denominator change with X.
struct SSPartials {
    double du = 0;
    int NB = 0;
    std::vector<i64> A, B, N;                 // per curve
    std::vector<std::vector<double>> num;     // num[curve][bin]  (Σ prefactor·ε·a_p)
    std::vector<std::vector<i64>> cnt;        // cnt[curve][bin]
};

// The heavy pass: compute per-curve bin partials over ALL rows (a_p once per curve).
// Deterministic given n_threads (prime-major, primes interleaved across threads).
SSPartials ss_empirical_partials(const std::vector<NeRow>& rows, double du, int n_threads);

// Aggregate the partials at height cutoff X (filter curves with H ≤ X, sum, divide by
// Δu·|fam(X)|, extract the two-pass shape). Cheap.
SSEmpirical ss_aggregate(const SSPartials& P, double X);

// Convenience: partials + aggregate at a single X.
SSEmpirical ss_empirical(const std::vector<NeRow>& rows, double X, double du, int n_threads);

// The pre-registered localization discriminant (m4-pinning confirmation §): the trough
// u extracted by the SAME two-pass extractor restricted to the window [u_lo,u_hi]
// (around the target, away from the biased tail). Diagnostic, never a replacement test.
double windowed_trough_u(const SSEmpirical& e, double u_lo, double u_hi);

// -------- the committed empirical-run artifact (data/m4/ss_empirical.txt) ---------
// COMPUTED data (a_p from scratch); consumed by the emitter + confirmation test so
// the emit is cheap and CI never re-runs the ~19-min statistic. Generator-hash
// refusal (sha256 of murm/ss_empirical.cpp — the statistic source) guards staleness.
constexpr uint32_t kSSRunFormatVersion = 1;

struct SSScaleShape { double X = 0; i64 n_curves = 0; SSShape shape{}; };

struct SSRun {
    uint32_t format_version = kSSRunFormatVersion;
    std::string generator_hash;
    double X_confirm = 0, du = 0, tol = 0;           // committed confirmation scale + a-priori τ (§R0c)
    double r2_hump = 0, r2_zero = 0, r2_trough = 0;  // committed R2 targets (formula side)
    std::vector<double> ladder;                      // convergence scales (ascending)
    std::vector<SSScaleShape> shapes;                // extracted shape per ladder scale
    SSEmpirical confirm;                             // full curve at X_confirm
};

const char* ss_generator_hash();
void write_ss_run(const std::string& path, const SSRun& run);
SSRun read_ss_run(const std::string& path);          // throws on format/generator-hash mismatch

}  // namespace at::murm
