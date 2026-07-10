#include "emit/emit_murmuration.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "ell/ecdata.h"
#include "emit/json.h"
#include "murm/murmuration.h"

namespace at::emit {
namespace fs = std::filesystem;
using at::core::u64;
using namespace at::murm;

namespace {

// Double formatting matched to emit_zeta (12 sig figs) — freshness-proven to be
// byte-identical across compilers for IEEE doubles.
std::string num(double x) {
    std::ostringstream o;
    o << std::setprecision(12) << x;
    return o.str();
}

void write_points(std::ostream& f, const MurmCurve& c) {
    f << "[";
    for (std::size_t i = 0; i < c.points.size(); ++i) {
        const MurmPoint& pt = c.points[i];
        f << (i ? "," : "") << "[" << pt.p << "," << num(pt.y) << "," << num(pt.avg) << "]";
    }
    f << "]";
}

void write_panel(std::ostream& f, const char* panel, const char* color, const MurmCurve& c) {
    f << "{\"panel\": \"" << panel << "\", \"color\": \"" << color << "\", \"rank\": " << c.rank
      << ", \"N1\": " << c.N1 << ", \"N2\": " << c.N2
      << ", \"family_size\": " << c.family_size << ", \"points\": ";
    write_points(f, c);
    f << "}";
}

void write_collapse(std::ostream& f, int rank, u64 aN1, u64 aN2, std::size_t famA,
                    u64 bN1, u64 bN2, std::size_t famB, int parity_rank,
                    const CollapseResult& r, const CollapseResult& rev,
                    const CollapseResult& par) {
    const bool pass = r.ratio < 3.0;
    const bool rev_rejects = rev.ratio >= 3.0;     // scaling-power measurement (R1b)
    const bool par_rejects = par.ratio >= 3.0;     // parity non-degeneracy (R1c)
    f << "{\"rank\": " << rank
      << ", \"rangeA\": [" << aN1 << "," << aN2 << "], \"family_A\": " << famA
      << ", \"rangeB\": [" << bN1 << "," << bN2 << "], \"family_B\": " << famB
      << ", \"grid_points\": " << r.grid_points
      << ", \"rms\": " << num(r.rms) << ", \"floor_F\": " << num(r.floor_F)
      << ", \"ratio\": " << num(r.ratio) << ", \"tolerance\": 3"
      << ", \"consistent_with_scaling\": " << (pass ? "true" : "false")
      // Reversal null: a NON-scaling scramble; its ratio is the test's power against
      // a wrong scaling at these family sizes (reported, not a gate).
      << ", \"reversal_null\": {\"kind\": \"within-curve reversal (scaling power)\""
      << ", \"ratio\": " << num(rev.ratio) << ", \"rejects\": " << (rev_rejects ? "true" : "false")
      << "}"
      // Parity null: opposite-parity rank; tests non-degeneracy, differs in TWO
      // variables (rank AND range), NOT a scaling null.
      << ", \"parity_null\": {\"kind\": \"rank " << rank << " vs rank " << parity_rank
      << " — parity non-degeneracy (differs in rank AND range; not a scaling null)\""
      << ", \"ratio\": " << num(par.ratio) << ", \"rejects\": " << (par_rejects ? "true" : "false")
      << "}"
      << ", \"limited_power_vs_scaling\": " << (rev_rejects ? "false" : "true") << "}";
}

}  // namespace

void emit_m1(const std::string& out_dir, const std::string& ecdata_dir,
             std::size_t n_primes, const std::string& generated_by) {
    fs::create_directories(out_dir);
    // One load covers every range used ([2500,5000], [5000,10000], [7500,10000]).
    const std::vector<at::ell::EcCurve> all = at::ell::load_ecdata_range(ecdata_dir, 2500, 10000);

    auto mc = [&](int r, u64 n1, u64 n2) { return murmuration_curve(all, r, n1, n2, n_primes); };

    // HLOP Figure 1 panels.
    const MurmCurve p0_75 = mc(0, 7500, 10000);   // top: f0
    const MurmCurve p1_75 = mc(1, 7500, 10000);   // top: f1
    const MurmCurve p0_50 = mc(0, 5000, 10000);   // bottom: f0  (also collapse B, r=0)
    const MurmCurve p2_50 = mc(2, 5000, 10000);   // bottom: f2  (also collapse B, r=2)

    // Scale-collapse [2500,5000] vs [5000,10000], per rank (R2), with TWO controls:
    //  reversal null (within-curve scramble → scaling-power measurement, R1b), and
    //  parity null (opposite-parity rank on B → non-degeneracy, R1c: r0↔r1, r2↔r1).
    const MurmCurve a0 = mc(0, 2500, 5000), a1 = mc(1, 2500, 5000), a2 = mc(2, 2500, 5000);
    const MurmCurve b1 = mc(1, 5000, 10000);
    const int GRID = 60;
    const CollapseResult c0 = scale_collapse(a0, p0_50, GRID);
    const CollapseResult rev0 = scale_collapse_null(a0, p0_50, GRID), par0 = scale_collapse(a0, b1, GRID);
    const CollapseResult c1 = scale_collapse(a1, b1, GRID);
    const CollapseResult rev1 = scale_collapse_null(a1, b1, GRID), par1 = scale_collapse(a1, p0_50, GRID);
    const CollapseResult c2 = scale_collapse(a2, p2_50, GRID);
    const CollapseResult rev2 = scale_collapse_null(a2, p2_50, GRID), par2 = scale_collapse(a2, b1, GRID);

    std::ofstream f(fs::path(out_dir) / "murmuration_curve.json");
    f << "{\n  \"schema\": \"murmuration_curve/1\",\n";
    f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
    f << "  \"params\": {\"ordering\": \"conductor\""
      << ", \"statistic\": \"unweighted arithmetic mean of a_p (HLOP eq. 1.1)\""
      << ", \"class\": \"rank\", \"source\": \"HLOP Figure 1 (arXiv:2204.10140)\""
      << ", \"ecdata_pin\": \"2026-04-22 / 25cec5ec\", \"n_primes\": " << n_primes
      << ", \"y_axis\": \"p / N2\""
      << ", \"provenance\": \"a_p computed at good primes + Atkin-Lehner-converted at bad primes "
         "(oracle sign, m0-pinning §3); rank and conductor are oracle (ecdata)\"},\n";
    // The four published counts — REQUIRE-anchored in verify (anchor_family_counts).
    f << "  \"family_counts\": {\"E0_7500_10000\": " << p0_75.family_size
      << ", \"E1_7500_10000\": " << p1_75.family_size
      << ", \"E0_5000_10000\": " << p0_50.family_size
      << ", \"E2_5000_10000\": " << p2_50.family_size << "},\n";
    f << "  \"panels\": [\n    ";
    write_panel(f, "top", "#2563eb", p0_75); f << ",\n    ";
    write_panel(f, "top", "#dc2626", p1_75); f << ",\n    ";
    write_panel(f, "bottom", "#2563eb", p0_50); f << ",\n    ";
    write_panel(f, "bottom", "#0f9d58", p2_50);
    f << "\n  ],\n";
    f << "  \"collapse\": [\n    ";
    write_collapse(f, 0, 2500, 5000, a0.family_size, 5000, 10000, p0_50.family_size, 1, c0, rev0, par0); f << ",\n    ";
    write_collapse(f, 1, 2500, 5000, a1.family_size, 5000, 10000, b1.family_size, 0, c1, rev1, par1); f << ",\n    ";
    write_collapse(f, 2, 2500, 5000, a2.family_size, 5000, 10000, p2_50.family_size, 1, c2, rev2, par2);
    f << "\n  ]\n}\n";
}

}  // namespace at::emit
