#pragma once
#include <complex>
#include <string>
#include <vector>

#include "core/int_types.h"

// Stage M2 — the LOP Dirichlet-character murmuration statistic and its pinned
// densities (m2-pinning §§P2–P3). Everything from scratch (characters, Gauss sums,
// primes); no external data. murm depends on symbols/core only.
namespace at::murm {

using at::core::u64;

struct DirichletMurm {
    double X = 0;
    std::string interval;        // "sharp" [X,X+X^δ] or "geometric" [X,cX]
    double param = 0;            // δ (sharp) or c (geometric)
    u64 n_prime_conductors = 0;  // # prime N in the interval
    u64 chars_even = 0, chars_odd = 0;   // total primitive even/odd characters summed
    std::vector<double> ys;
    std::vector<std::complex<double>> even;  // P̃₊ / P₊ at each y
    std::vector<std::complex<double>> odd;   // P̃₋ / P₋ at each y
};

// P̃±(y,X,δ), sharp interval N ∈ [X, X+X^δ] (LOP eq. 1.2): prefactor log X / X^δ.
DirichletMurm murm_sharp(double X, double delta, const std::vector<double>& ys);

// P±(y,X,c), geometric interval N ∈ [X, cX] (LOP eq. 1.1): prefactor log X / X.
DirichletMurm murm_geometric(double X, double c, const std::vector<double>& ys);

// Pinned closed-form densities (LOP Theorem 1.1, read verbatim from the PDF).
std::complex<double> density_sharp_even(double y);          // (1.4) cos(2πy)
std::complex<double> density_sharp_odd(double y);           // (1.4) −i·sin(2πy)
std::complex<double> density_geom_even(double y, double c); // (1.3) ∫₁^c cos(2πy/x)dx
std::complex<double> density_geom_odd(double y, double c);  // (1.3) −i·∫₁^c sin(2πy/x)dx

}  // namespace at::murm
