#pragma once

#include <vector>

// Stage 5 — the model system (RESEARCH.md §7.3, rider R4, stage5-pinning.md §4).
// A subshift of finite type with 0-1 transition matrix A and a roof function r
// on the alphabet defines a suspension flow whose closed orbits γ have length
// ℓ(γ) = Σ_{symbols in γ} r. Its Ruelle/dynamical zeta is
//
//   ζ_flow(s) = Π_γ (1 − e^{−s ℓ(γ)})^{−1}
//             = 1 / det(I − L_s),   L_s[i][j] = A[i][j]·e^{−s r[j]}       (PP90)
//
// The two sides are computed independently (orbit/periodic-point trace sum vs a
// matrix determinant) and twinned over a grid of s. Unlike Deninger's arithmetic
// flow, this system is fully constructed — the identity here IS geometry.

namespace at::zeta {

struct SFT {
    std::vector<std::vector<int>> A;  // n×n 0-1 transition matrix
    std::vector<double> roof;         // roof length per symbol (size n)
};

// Golden-mean shift: A = [[1,1],[1,0]] (forbids "11"), constant roof r ≡ 1.
// Closed form ζ(s) = 1/(1 − z − z²) with z = e^{−s}; tr(Aⁿ) = Lucas L_n.
SFT golden_mean_sft();
// Same shift with a "log roof" r = (log 2, log 3): orbit lengths are sums of
// logs, echoing the arithmetic "length log p". Used for the viewer panel.
SFT golden_mean_log_roof();

// Number of closed orbits of length n = Fix(σⁿ) = tr(Aⁿ). Exact integer.
long long closed_orbit_count(const SFT& s, int n);

// ζ_flow(s) via the determinant side: 1 / det(I − L_s).
double ruelle_zeta_determinant(const SFT& s, double sval);

// ζ_flow(s) via the orbit side: exp( Σ_{n=1}^{N} tr(L_sⁿ)/n ), truncated at N.
double ruelle_zeta_orbit(const SFT& s, double sval, int N);

// Golden-mean constant-roof closed form 1/(1 − z − z²), z = e^{−s} (analytic
// third check, valid only for the r ≡ 1 golden-mean shift).
double golden_mean_closed_form(double sval);

// Abscissa of convergence: the s where the spectral radius of L_s equals 1
// (topological pressure). The orbit product/determinant agree for s above it.
double abscissa_of_convergence(const SFT& s);

}  // namespace at::zeta
