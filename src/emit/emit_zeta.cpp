#include "emit/emit_zeta.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "emit/json.h"
#include "zeta/explicit_formula.h"
#include "zeta/ruelle.h"
#include "zeta/zero_finder.h"

namespace at::emit {
namespace fs = std::filesystem;
using namespace at::zeta;

namespace {
// Stream a double with 12 significant digits; NaN/Inf → null (valid JSON).
std::string num(double x) {
    if (!std::isfinite(x)) return "null";
    std::ostringstream o;
    o << std::setprecision(12) << x;
    return o.str();
}

std::vector<double> read_odlyzko(const std::string& path) {
    std::vector<double> g;
    if (path.empty()) return g;
    std::ifstream f(path);
    std::string line;
    while (std::getline(f, line))
        if (!line.empty()) {
            try { g.push_back(std::stod(line)); } catch (...) {}
        }
    return g;
}
}  // namespace

void emit_stage5(const std::string& out_dir, double zeros_t_max,
                 const std::string& odlyzko_path, const std::string& generated_by) {
    fs::create_directories(out_dir);
    ZeroFindResult zr = find_zeros(zeros_t_max);
    std::vector<double> odl = read_odlyzko(odlyzko_path);
    const bool have_odl = !odl.empty();

    // Max |mine − Odlyzko| over the found zeros (empiric).
    double max_dev = 0.0;
    for (std::size_t i = 0; i < zr.zeros.size() && i < odl.size(); ++i)
        max_dev = std::max(max_dev, std::fabs(zr.zeros[i] - odl[i]));

    // ---- zeros.json ------------------------------------------------------
    {
        std::ofstream f(fs::path(out_dir) / "zeros.json");
        f << "{\n  \"schema\": \"zeros/1\",\n";
        f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
        f << "  \"params\": {\"t_max\": " << num(zeros_t_max)
          << ", \"engine\": \"riemann-siegel scan + euler-maclaurin refine\""
          << ", \"oracle\": \"" << (have_odl ? "odlyzko" : "none") << "\"},\n";
        f << "  \"aggregate\": {\"complete\": " << (zr.complete ? "true" : "false")
          << ", \"n_expected\": " << zr.n_expected
          << ", \"n_found\": " << zr.zeros.size()
          << ", \"t_top\": " << num(zr.t_top)
          << ", \"n_good_gram\": " << zr.n_good_gram
          << ", \"n_gram_law_exceptions\": " << zr.n_gram_law_exceptions
          << ", \"n_close_pairs\": " << zr.n_close_pairs
          << ", \"n_refined_blocks\": " << zr.n_refined_blocks
          << ", \"max_odlyzko_dev\": " << (have_odl ? num(max_dev) : "null")
          << "},\n";
        f << "  \"zeros\": [";
        std::size_t cap = std::min<std::size_t>(zr.zeros.size(), 2000);
        for (std::size_t i = 0; i < cap; ++i) {
            f << (i ? ",\n    " : "\n    ") << "{\"n\": " << (i + 1)
              << ", \"gamma\": " << num(zr.zeros[i]);
            if (i < odl.size())
                f << ", \"delta\": " << num(zr.zeros[i] - odl[i]);
            f << "}";
        }
        f << "\n  ]\n}\n";
    }

    // ---- psi_reconstruction.json ----------------------------------------
    {
        const double x0 = 2.0, x1 = 30.0;
        const int NX = 400;
        std::vector<double> xs(NX), ref(NX);
        for (int i = 0; i < NX; ++i) {
            xs[i] = x0 + (x1 - x0) * i / (NX - 1);
            ref[i] = psi0_reference(xs[i]);
        }
        std::vector<std::size_t> counts;
        for (std::size_t m : {1UL, 2UL, 3UL, 5UL, 10UL, 25UL, 50UL, 100UL, 200UL})
            if (m <= zr.zeros.size()) counts.push_back(m);
        if (!zr.zeros.empty()) counts.push_back(zr.zeros.size());

        std::ofstream f(fs::path(out_dir) / "psi_reconstruction.json");
        f << "{\n  \"schema\": \"psi_reconstruction/1\",\n";
        f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
        f << "  \"params\": {\"x_min\": " << num(x0) << ", \"x_max\": " << num(x1)
          << ", \"n_x\": " << NX << ", \"n_zeros_available\": " << zr.zeros.size()
          << ", \"note\": \"series converges to midpoint-normalized psi_0\"},\n";
        f << "  \"x\": [";
        for (int i = 0; i < NX; ++i) f << (i ? "," : "") << num(xs[i]);
        f << "],\n  \"reference\": [";
        for (int i = 0; i < NX; ++i) f << (i ? "," : "") << num(ref[i]);
        f << "],\n  \"prime_powers\": [";
        bool first = true;
        for (int n = 2; n <= (int)x1; ++n) {
            double lam = von_mangoldt(n);
            if (lam > 0) {
                f << (first ? "" : ", ") << "{\"x\": " << n << ", \"lambda\": " << num(lam) << "}";
                first = false;
            }
        }
        // "smooth" x-points: ≥ 0.4 from any prime power, so the error there isn't
        // dominated by Gibbs overshoot at the jumps.
        std::vector<char> smooth(NX, 1);
        for (int i = 0; i < NX; ++i)
            for (int pp = 2; pp <= (int)x1; ++pp)
                if (von_mangoldt(pp) > 0 && std::fabs(xs[i] - pp) < 0.4) smooth[i] = 0;
        f << "],\n  \"frames\": [";
        for (std::size_t fi = 0; fi < counts.size(); ++fi) {
            std::size_t m = counts[fi];
            double maxerr = 0.0, maxerr_smooth = 0.0;
            std::ostringstream vals;
            for (int i = 0; i < NX; ++i) {
                double v = psi_reconstruct(xs[i], zr.zeros, m);
                vals << (i ? "," : "") << num(v);
                double e = std::fabs(v - ref[i]);
                maxerr = std::max(maxerr, e);
                if (smooth[i]) maxerr_smooth = std::max(maxerr_smooth, e);
            }
            f << (fi ? ",\n    " : "\n    ") << "{\"m\": " << m
              << ", \"max_err\": " << num(maxerr)
              << ", \"max_err_smooth\": " << num(maxerr_smooth)
              << ", \"values\": [" << vals.str() << "]}";
        }
        f << "\n  ]\n}\n";
    }

    // ---- dyn_zeta.json ---------------------------------------------------
    {
        struct Model { const char* name; SFT sft; bool closed; };
        std::vector<Model> models = {
            {"golden_mean_const_roof", golden_mean_sft(), true},
            {"golden_mean_log_roof", golden_mean_log_roof(), false},
        };
        std::ofstream f(fs::path(out_dir) / "dyn_zeta.json");
        f << "{\n  \"schema\": \"dyn_zeta/1\",\n";
        f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
        f << "  \"construction_gap\": \"This subshift: fully constructed — orbits are "
             "closed paths, the Euler product = determinant identity IS geometry. "
             "Deninger's arithmetic flow: not constructed — the same identity (explicit "
             "formula) holds, but the dynamical system is conjectural.\",\n";
        const int kOrbitN = 150;
        f << "  \"orbit_truncation_N\": " << kOrbitN << ",\n";
        f << "  \"models\": [";
        for (std::size_t mi = 0; mi < models.size(); ++mi) {
            const auto& M = models[mi];
            double abscissa = abscissa_of_convergence(M.sft);
            // Start just above the abscissa (the truncated orbit product converges
            // only for s > abscissa; right at it, N terms don't suffice).
            double s0 = abscissa + 0.12, s1 = abscissa + 3.0;
            const int NS = 120;
            f << (mi ? ",\n    " : "\n    ") << "{\"name\": \"" << M.name << "\"";
            f << ", \"abscissa\": " << num(abscissa);
            f << ", \"roof\": [";
            for (std::size_t k = 0; k < M.sft.roof.size(); ++k)
                f << (k ? "," : "") << num(M.sft.roof[k]);
            f << "]";
            std::ostringstream sarr, det, orb, cf;
            double maxdiff = 0.0;
            for (int i = 0; i < NS; ++i) {
                double s = s0 + (s1 - s0) * i / (NS - 1);
                double d = ruelle_zeta_determinant(M.sft, s);
                double o = ruelle_zeta_orbit(M.sft, s, kOrbitN);
                sarr << (i ? "," : "") << num(s);
                det << (i ? "," : "") << num(d);
                orb << (i ? "," : "") << num(o);
                if (M.closed) cf << (i ? "," : "") << num(golden_mean_closed_form(s));
                maxdiff = std::max(maxdiff, std::fabs(d - o));
            }
            f << ", \"s\": [" << sarr.str() << "]";
            f << ", \"determinant\": [" << det.str() << "]";
            f << ", \"orbit\": [" << orb.str() << "]";
            if (M.closed) f << ", \"closed_form\": [" << cf.str() << "]";
            f << ", \"max_orbit_det_diff\": " << num(maxdiff) << "}";
        }
        f << "\n  ]\n}\n";
    }
}

}  // namespace at::emit
