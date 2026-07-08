#include "zeta/ruelle.h"

#include <cmath>
#include <utility>

namespace at::zeta {
namespace {

using Mat = std::vector<std::vector<double>>;

// L_s[i][j] = A[i][j] · e^{−s·roof[j]}  (weight by destination symbol, so a
// closed path's total weight is e^{−s·Σroof} = e^{−s·ℓ(orbit)}).
Mat weighted(const SFT& s, double sval) {
    std::size_t n = s.A.size();
    Mat L(n, std::vector<double>(n, 0.0));
    for (std::size_t i = 0; i < n; ++i)
        for (std::size_t j = 0; j < n; ++j)
            L[i][j] = s.A[i][j] * std::exp(-sval * s.roof[j]);
    return L;
}

Mat matmul(const Mat& X, const Mat& Y) {
    std::size_t n = X.size();
    Mat Z(n, std::vector<double>(n, 0.0));
    for (std::size_t i = 0; i < n; ++i)
        for (std::size_t k = 0; k < n; ++k) {
            double xik = X[i][k];
            if (xik == 0.0) continue;
            for (std::size_t j = 0; j < n; ++j) Z[i][j] += xik * Y[k][j];
        }
    return Z;
}

double trace(const Mat& X) {
    double t = 0.0;
    for (std::size_t i = 0; i < X.size(); ++i) t += X[i][i];
    return t;
}

// det via Gaussian elimination with partial pivoting.
double determinant(Mat M) {
    std::size_t n = M.size();
    double det = 1.0;
    for (std::size_t c = 0; c < n; ++c) {
        std::size_t piv = c;
        for (std::size_t r = c + 1; r < n; ++r)
            if (std::fabs(M[r][c]) > std::fabs(M[piv][c])) piv = r;
        if (M[piv][c] == 0.0) return 0.0;
        if (piv != c) { std::swap(M[piv], M[c]); det = -det; }
        det *= M[c][c];
        for (std::size_t r = c + 1; r < n; ++r) {
            double f = M[r][c] / M[c][c];
            for (std::size_t j = c; j < n; ++j) M[r][j] -= f * M[c][j];
        }
    }
    return det;
}

Mat identity(std::size_t n) {
    Mat I(n, std::vector<double>(n, 0.0));
    for (std::size_t i = 0; i < n; ++i) I[i][i] = 1.0;
    return I;
}

}  // namespace

SFT golden_mean_sft() { return {{{1, 1}, {1, 0}}, {1.0, 1.0}}; }
SFT golden_mean_log_roof() { return {{{1, 1}, {1, 0}}, {std::log(2.0), std::log(3.0)}}; }

long long closed_orbit_count(const SFT& s, int n) {
    std::size_t d = s.A.size();
    Mat P(d, std::vector<double>(d, 0.0));
    for (std::size_t i = 0; i < d; ++i)
        for (std::size_t j = 0; j < d; ++j) P[i][j] = s.A[i][j];
    Mat R = P;
    for (int k = 1; k < n; ++k) R = matmul(R, P);
    return static_cast<long long>(std::llround(trace(R)));
}

double ruelle_zeta_determinant(const SFT& s, double sval) {
    std::size_t n = s.A.size();
    Mat L = weighted(s, sval);
    Mat ImL = identity(n);
    for (std::size_t i = 0; i < n; ++i)
        for (std::size_t j = 0; j < n; ++j) ImL[i][j] -= L[i][j];
    return 1.0 / determinant(ImL);
}

double ruelle_zeta_orbit(const SFT& s, double sval, int N) {
    Mat L = weighted(s, sval);
    Mat Ln = L;
    double acc = 0.0;
    for (int n = 1; n <= N; ++n) {
        acc += trace(Ln) / n;
        if (n < N) Ln = matmul(Ln, L);
    }
    return std::exp(acc);
}

double golden_mean_closed_form(double sval) {
    double z = std::exp(-sval);
    return 1.0 / (1.0 - z - z * z);
}

double abscissa_of_convergence(const SFT& s) {
    // Largest eigenvalue of L_s decreases in s; find s* with spectral radius 1
    // via the trace-growth rate: ρ(L_s) = lim (tr L_s^n)^{1/n}. Bisect on s.
    auto spectral_radius = [&](double sval) {
        Mat L = weighted(s, sval);
        Mat Ln = L;
        for (int k = 1; k < 40; ++k) Ln = matmul(Ln, L);
        double tr = trace(Ln);
        return std::pow(std::fabs(tr), 1.0 / 40.0);
    };
    double lo = -5.0, hi = 5.0;
    for (int it = 0; it < 80; ++it) {
        double mid = 0.5 * (lo + hi);
        if (spectral_radius(mid) > 1.0) lo = mid; else hi = mid;
    }
    return 0.5 * (lo + hi);
}

}  // namespace at::zeta
