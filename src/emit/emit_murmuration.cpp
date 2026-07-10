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
                    u64 bN1, u64 bN2, std::size_t famB, int null_rank,
                    const CollapseResult& r, const CollapseResult& nul) {
    const bool pass = r.ratio < 3.0;
    const bool null_fails = nul.ratio >= 3.0;
    // Low power when the family is small enough that the noise floor swamps the
    // antiphase signal — the null cannot be rejected (rider R2). Flagged, not hidden.
    const bool low_power = !null_fails;
    f << "{\"rank\": " << rank
      << ", \"rangeA\": [" << aN1 << "," << aN2 << "], \"family_A\": " << famA
      << ", \"rangeB\": [" << bN1 << "," << bN2 << "], \"family_B\": " << famB
      << ", \"grid_points\": " << r.grid_points
      << ", \"rms\": " << num(r.rms) << ", \"floor_F\": " << num(r.floor_F)
      << ", \"ratio\": " << num(r.ratio) << ", \"tolerance\": 3"
      << ", \"pass\": " << (pass ? "true" : "false")
      << ", \"null_pairing\": \"rank " << rank << " vs rank " << null_rank
      << " (antiphase label-shuffle)\""
      << ", \"null_ratio\": " << num(nul.ratio)
      << ", \"null_fails_tolerance\": " << (null_fails ? "true" : "false")
      << ", \"low_power\": " << (low_power ? "true" : "false") << "}";
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

    // Scale-collapse [2500,5000] vs [5000,10000], per rank (R2). The null is the
    // same statistic with an antiphase (opposite-parity) rank on the B side (R1c):
    // r0↔r1, r2↔r1 (r0,r2 even; r1 odd).
    const MurmCurve a0 = mc(0, 2500, 5000), a1 = mc(1, 2500, 5000), a2 = mc(2, 2500, 5000);
    const MurmCurve b1 = mc(1, 5000, 10000);
    const int GRID = 60;
    const CollapseResult c0 = scale_collapse(a0, p0_50, GRID), n0 = scale_collapse(a0, b1, GRID);      // null: r0-A vs r1-B
    const CollapseResult c1 = scale_collapse(a1, b1, GRID),   n1c = scale_collapse(a1, p0_50, GRID);   // null: r1-A vs r0-B
    const CollapseResult c2 = scale_collapse(a2, p2_50, GRID),n2c = scale_collapse(a2, b1, GRID);      // null: r2-A vs r1-B

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
    write_collapse(f, 0, 2500, 5000, a0.family_size, 5000, 10000, p0_50.family_size, 1, c0, n0); f << ",\n    ";
    write_collapse(f, 1, 2500, 5000, a1.family_size, 5000, 10000, b1.family_size, 0, c1, n1c); f << ",\n    ";
    write_collapse(f, 2, 2500, 5000, a2.family_size, 5000, 10000, p2_50.family_size, 1, c2, n2c);
    f << "\n  ]\n}\n";
}

}  // namespace at::emit
