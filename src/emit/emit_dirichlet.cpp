#include "emit/emit_dirichlet.h"

#include <algorithm>
#include <complex>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

#include "emit/json.h"
#include "murm/dirichlet_murmuration.h"

namespace at::emit {
namespace fs = std::filesystem;
using namespace at::murm;

namespace {
std::string num(double x) {
    std::ostringstream o;
    o << std::setprecision(12) << x;
    return o.str();
}

// max_y ‖stat − density‖ for a computed curve (sharp or geometric).
double max_err(const DirichletMurm& m, bool geometric, double c) {
    double e = 0;
    for (std::size_t i = 0; i < m.ys.size(); ++i) {
        const auto de = geometric ? density_geom_even(m.ys[i], c) : density_sharp_even(m.ys[i]);
        const auto od = geometric ? density_geom_odd(m.ys[i], c) : density_sharp_odd(m.ys[i]);
        e = std::max({e, std::abs(m.even[i] - de), std::abs(m.odd[i] - od)});
    }
    return e;
}

void write_curve(std::ostream& f, const DirichletMurm& m, bool geometric, double c) {
    f << "{\"interval\": \"" << m.interval << "\", \"X\": " << num(m.X)
      << ", \"param\": " << num(m.param) << ", \"n_prime_conductors\": " << m.n_prime_conductors
      << ", \"chars_even\": " << m.chars_even << ", \"chars_odd\": " << m.chars_odd
      << ", \"points\": [";
    for (std::size_t i = 0; i < m.ys.size(); ++i) {
        const auto de = geometric ? density_geom_even(m.ys[i], c) : density_sharp_even(m.ys[i]);
        const auto od = geometric ? density_geom_odd(m.ys[i], c) : density_sharp_odd(m.ys[i]);
        f << (i ? "," : "") << "{\"y\": " << num(m.ys[i])
          << ", \"even_re\": " << num(m.even[i].real()) << ", \"even_im\": " << num(m.even[i].imag())
          << ", \"odd_re\": " << num(m.odd[i].real()) << ", \"odd_im\": " << num(m.odd[i].imag())
          << ", \"density_even\": " << num(de.real())
          << ", \"density_odd_im\": " << num(od.imag()) << "}";
    }
    f << "]}";
}
}  // namespace

void emit_m2(const std::string& out_dir, const std::string& generated_by) {
    fs::create_directories(out_dir);
    const double DX = 2000, DELTA = 0.7, C = 2.0;
    std::vector<double> ys;
    for (int i = 1; i <= 100; ++i) ys.push_back(0.02 * i);   // 0.02 … 2.00

    const DirichletMurm sharp = murm_sharp(DX, DELTA, ys);
    const DirichletMurm geom = murm_geometric(DX, C, ys);

    // Convergence trend (empiric) over X, both intervals. The X = DX runs above are
    // reused for the last point (no recompute).
    const std::vector<double> Xc = {500, 1000, DX};
    std::ostringstream gconv, sconv;
    for (std::size_t i = 0; i < Xc.size(); ++i) {
        const double xe_g = (Xc[i] == DX) ? max_err(geom, true, C)
                                          : max_err(murm_geometric(Xc[i], C, ys), true, C);
        const double xe_s = (Xc[i] == DX) ? max_err(sharp, false, C)
                                          : max_err(murm_sharp(Xc[i], DELTA, ys), false, C);
        gconv << (i ? "," : "") << "{\"X\": " << num(Xc[i]) << ", \"max_err\": " << num(xe_g) << "}";
        sconv << (i ? "," : "") << "{\"X\": " << num(Xc[i]) << ", \"max_err\": " << num(xe_s) << "}";
    }
    // Cross-parity null on the display sharp curve.
    double null_val = 0;
    for (std::size_t i = 0; i < ys.size(); ++i)
        null_val = std::max(null_val, std::abs(sharp.even[i] - density_sharp_odd(ys[i])));

    std::ofstream f(fs::path(out_dir) / "dirichlet_murmuration.json");
    f << "{\n  \"schema\": \"dirichlet_murmuration/1\",\n";
    f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
    f << "  \"params\": {\"family\": \"primitive even/odd Dirichlet characters mod prime N\""
      << ", \"statistic\": \"(log X / X[^delta]) * sum_{N prime} sum_{chi in D_pm(N)} chi(ceil(yX)_p)/tau(chi) (LOP eq 1.1/1.2)\""
      << ", \"source\": \"LOP arXiv:2307.00256, Theorem 1.1\""
      << ", \"external_data\": \"none — computed from scratch (characters, Gauss sums, primes); "
         "ARCHITECTURE-M §9 repo-reproducible default (the clean case)\""
      << ", \"display_X\": " << num(DX) << ", \"sharp_delta\": " << num(DELTA)
      << ", \"geom_c\": " << num(C) << "},\n";
    f << "  \"sharp\": ";   write_curve(f, sharp, false, C); f << ",\n";
    f << "  \"geometric\": "; write_curve(f, geom, true, C);  f << ",\n";
    f << "  \"convergence\": {\"note\": \"observed empiric — max_y ‖stat − density‖ vs X; "
         "the theorem is X→∞\", \"geometric\": [" << gconv.str() << "], \"sharp\": [" << sconv.str() << "]},\n";
    f << "  \"null\": {\"kind\": \"even (real) statistic vs odd (imaginary) density — cross-parity\""
      << ", \"value\": " << num(null_val) << ", \"must_exceed\": 0.5}\n";
    f << "}\n";
}

}  // namespace at::emit
