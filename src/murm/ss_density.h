#pragma once
#include <vector>

// Stage M4 — the Sawin–Sutherland murmuration density D(u), the FORMULA side of the
// separate-TU protocol (m4-pinning §P3, Conjecture 1 / eq. (2) of [SS25]). Authored
// independently of the empirical statistic; not edited after first comparison.
//
//   D(u) = 2π√u · Σ_{q squarefree} Σ_{m≥1}
//              [ μ(gcd(m,q)) / (q·m·φ(q/gcd(m,q))) ] · J₁(4π√u·m/q)
//              · ∏_{p|q} ℓ̂_{p,2v_p(m)} · ∏_{p|m, p∤q} ℓ_{p,2v_p(m)}          (u = p/N)
//
// CORRECTED 2026-07-18 (referee C): the earlier reading had the two products SWAPPED
// (ℓ̂ on p|m,p∤q; ℓ on p|q) and the p|q exponent wrong — BOTH products carry 2v_p(m)
// (only even ν appear, [SS25] p.9). The ν-domains force it: ℓ̂ takes nonnegative even ν
// (Lemma 4; ν=0 for p|q,p∤m), ℓ takes positive ν (Lemma 3). This moved the density
// trough from a spurious 0.805 to 0.870, matching the published figure (ERRATA #28).
//
// with the local factors ℓ_{p,ν}, ℓ̂_{p,ν} verbatim from Lemmas 3 & 4 ([SS25] pp.4–5):
// rational in p, plus Chebyshev U_ν (second kind) at p=3, plus eigenvalue sums
// Σ_{f∈S₀^{ν+2}(SL₂ℤ)} a_p(f) that VANISH for weight ν+2 < 12 (i.e. ν < 10). Every
// value is built from IEEE ops + generated constants + the in-house J₁ (no libm), so
// the emitted curve is byte-portable ([[cross-compiler-emit-determinism]]).
//
// The eigenvalue sums are supplied by `level1_hecke_trace` via a from-scratch level-1
// EICHLER–SELBERG trace reusing M3's Hurwitz class numbers (NOT truncated — the earlier
// weight-≥12 truncation returned 0, harmless only under the corrupted products above but
// load-bearing under the correct ones, since a surviving ℓ-term REQUIRES 2v_p(m)≥10; the
// truncation's bound expired with the products it was certified against).
namespace at::murm {

// Chebyshev polynomial of the second kind U_ν(x) (U₀=1, U₁=2x, recurrence).
double cheb_U(int nu, double x);

// Σ_{f∈S₀^{k}(SL₂ℤ) eigenform} a_p(f) = Tr(T_p | S_k(SL₂ℤ)). Exactly 0 for k<12 or k
// odd (no level-1 cusp forms). Weight ≥12 with cusp forms: see ss_density.cpp.
long long level1_hecke_trace(int k, long long p);

// Local factors (m4-pinning §P3 / Lemmas 3,4). ν ≥ 0; ℓ̂ takes even ν.
double ss_ell(long long p, int nu);       // ℓ_{p,ν}
double ss_ell_hat(long long p, int nu);    // ℓ̂_{p,ν}

// D(u): the density at u, truncating the double sum to m ≤ m_bound, q ≤ q_bound
// (squarefree). Returns the integrand value (not integrated).
double ss_density(double u, long long m_bound, long long q_bound);

// Shape invariants of D(u) on (0,1], extracted NUMERICALLY (never eyeballed from the
// figure — transcribing-from-a-picture is the constants failure mode): the global
// positive hump, the first +→− zero crossing after it, and the global trough. These
// are the R2 calibrated targets (m4-pinning §P3). Scanned at step `du`, bound B.
struct SSShape { double hump_u, hump_v, zero_u, trough_u, trough_v; };
SSShape ss_shape(long long B, double du);

// The neutral two-pass shape extractor used by BOTH sides (a generic peak-finder, not
// a density formula — sharing it does not couple the formula/empirical TUs): global
// hump + global trough (pass 1), then the FIRST +→− zero crossing past the hump
// (pass 2, linearly interpolated), so an early low-u wiggle is never mistaken for the
// post-hump zero. `us` ascending, `ds` the sampled curve.
SSShape extract_shape(const std::vector<double>& us, const std::vector<double>& ds);

}  // namespace at::murm
