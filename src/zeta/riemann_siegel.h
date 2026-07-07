#pragma once

// Stage 5 — the Hardy Z-function and Riemann-Siegel machinery (RESEARCH.md §7.1,
// docs/notes/stage5-pinning.md §1). Z(t) = e^{i θ(t)} ζ(1/2+it) is real; its sign
// changes are the ordinates of the nontrivial zeros. Two independent engines:
//
//   Z_riemann_siegel  — main sum (⌊√(t/2π)⌋ terms) + remainder C_0..C_3 (fast;
//                        the named Stage-5 method). Edwards [E74] Ch.7.
//   Z_euler_maclaurin — ζ(1/2+it) by Euler-Maclaurin (stable everywhere; the
//                        naive referee twin, never optimized).
//
// Z() dispatches: Euler-Maclaurin for small t and inside the thin band where the
// Riemann-Siegel remainder function Ψ is near its removable singularities
// (frac(√(t/2π)) ≈ 1/4 or 3/4), Riemann-Siegel otherwise.

namespace at::zeta {

// Riemann-Siegel theta, θ(t) = arg Γ(1/4+it/2) − (t/2)logπ.
// Asymptotic series (primary); accurate for the whole zero range (t ≥ 14).
double theta(double t);
// θ via complex log-Γ (Lanczos) — independent twin for theta().
double theta_lgamma(double t);

// Hardy Z-function (dispatches between the two engines below).
double Z(double t);

// The two engines, exposed for twin_ verification and fallback control.
double Z_riemann_siegel(double t);    // main sum + C_0..C_3
double Z_euler_maclaurin(double t);   // via Euler-Maclaurin ζ

// ζ(1/2+it) real/imag via Euler-Maclaurin (used by Z_euler_maclaurin; exposed
// for the reconstruction and tests). Returns {Re, Im}.
struct Cx { double re, im; };
Cx zeta_half_plus_it(double t);

// n-th Gram point g_n: the solution of θ(g_n) = nπ, n ≥ −1. Newton from the
// asymptotic seed. Gram points SEED the zero scan; they never define the count.
double gram_point(long n);

// True where Z() would route to Euler-Maclaurin (small t or near-singular band).
// Exposed so the invariance/twin tests can target each engine's valid domain.
bool uses_euler_maclaurin(double t);

}  // namespace at::zeta
