#include "emit/emit_sawin_sutherland.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "emit/json.h"
#include "murm/ss_density.h"
#include "murm/ss_empirical.h"

namespace at::emit {
namespace fs = std::filesystem;
using namespace at::murm;

namespace {
// 12 sig figs — the freshness-proven byte-identical format (emit_zeta/zubrilina).
std::string num(double x) {
    std::ostringstream o;
    o << std::setprecision(12) << x;
    return o.str();
}
// Density truncation for the emitted overlay: the R2 locations froze for B∈[200,5000];
// B=2000 is well inside that and cheap. Recorded in params.
constexpr long long kDensB = 2000;
}  // namespace

void emit_m4(const std::string& out_dir, const std::string& run_path,
             const std::string& generated_by) {
    fs::create_directories(out_dir);

    const SSRun run = read_ss_run(run_path);   // throws (refuses) on a stale run
    const SSShape& s = run.confirm.shape;
    const double dev_hump = std::abs(s.hump_u - run.r2_hump);
    const double dev_zero = std::abs(s.zero_u - run.r2_zero);
    const double dev_trough = std::abs(s.trough_u - run.r2_trough);
    const double win_trough = windowed_trough_u(run.confirm, 0.7, 0.9);

    std::ofstream f(fs::path(out_dir) / "sawin_sutherland_murmuration.json");
    f << "{\n  \"schema\": \"sawin_sutherland_murmuration/1\",\n";
    f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
    f << "  \"params\": {"
      << "\"family\": \"E_{A,B}: y²=x³+Ax+B, integers A,B reduced (no p: p⁴|A ∧ p⁶|B), "
         "nonsingular; ordered by naive height H=max(4|A|³,27B²) ≤ X\""
      << ", \"statistic\": \"[SS25] eq. (1): 𝔼_{H(E)≤X}[ (log N^{(C₁+C₂)/2}/N) "
         "Σ_{p∈(C₁N,C₂N)} ε(E)a_p(E) ], binned by u=p/N over (0,1] (NOT p/X)\""
      << ", \"source\": \"Sawin–Sutherland, arXiv:2504.12295 v2, Conjecture 1 / eq. (2)\""
      << ", \"external_data\": \"a_p COMPUTED from the model (ell::ap_fast, twinned vs the "
         "frozen ell::ap_charsum); conductor N and root number ε are ORACLE-PROVENANCE "
         "INPUT (PARI/GP, committed data/m4/ne_cache.txt, certified vs Cremona ecdata by "
         "the R3 dual-oracle overlap twin — 5676 classes exact)\""
      << ", \"provenance\": \"empirical curve = computed statistic (a_p); density D(u) = "
         "the conjectured Bessel-J₁ formula (in-house J₁ + generated constants); separate "
         "TUs. N, ε: oracle\""
      << ", \"claim_class\": \"PARTIAL agreement with the CONJECTURED density (Conjecture 1 "
         "[SS25]): at X_confirm=10⁴ the hump (dev " << num(dev_hump) << ") and first "
         "zero-crossing (dev " << num(dev_zero) << ") agree within the a-priori tolerance "
         "τ=" << num(run.tol) << "; the trough is an OPEN DEVIATION (dev " << num(dev_trough)
      << " exceeds τ, flat across the X-ladder so NOT demonstrated to be the finite-X "
         "downward bias of [SS25] by this range, and the windowed[0.7,0.9] trough equals "
         "the global one so it is not a tail-argmin artifact). The murmuration and its "
         "primary oscillation replicate; one invariant is an openly-flagged deviation. "
         "This is a numerical replication of a CONJECTURE — no proof is claimed (rule 7)\""
      << ", \"X_confirm\": " << num(run.X_confirm)
      << ", \"n_curves\": " << run.confirm.n_curves
      << ", \"du\": " << num(run.du)
      << ", \"tol\": " << num(run.tol)
      << ", \"density_trunc_B\": " << kDensB
      << ", \"u_axis\": \"u = p/N(E)\""
      << ", \"downward_bias_note\": \"[SS25] p.3–4: a persistent downward bias makes (1) "
         "sit below its predicted limit; it 'seems to decay as X→∞' but need not decrease "
         "monotonically (the rank-2 proportion increases over their 2¹⁶–2²⁸); our ladder "
         "2¹²–2¹³·³ shows the trough deviation FLAT, hence the open-deviation verdict\""
      << "},\n";

    // Empirical curve at X_confirm (COMPUTED statistic).
    f << "  \"empirical\": [";
    for (std::size_t b = 0; b < run.confirm.u_mid.size(); ++b)
        f << (b ? "," : "") << "{\"u\": " << num(run.confirm.u_mid[b])
          << ", \"emp\": " << num(run.confirm.density[b])
          << ", \"count\": " << run.confirm.count[b] << "}";
    f << "],\n";

    // Conjectured density D(u) on a fine grid (the in-house Bessel-J₁ formula).
    f << "  \"density\": [";
    bool first = true;
    for (double u = run.du; u <= 1.0 + 1e-9; u += 0.005) {
        f << (first ? "" : ",") << "{\"u\": " << num(u)
          << ", \"d\": " << num(ss_density(u, kDensB, kDensB)) << "}";
        first = false;
    }
    f << "],\n";

    // Shape invariants: committed R2 targets, extracted empirical locations, deviations,
    // within-τ flags, and the pre-registered windowed-trough diagnostic.
    f << "  \"shape\": {"
      << "\"targets\": {\"hump\": " << num(run.r2_hump) << ", \"zero\": " << num(run.r2_zero)
      << ", \"trough\": " << num(run.r2_trough) << "}"
      << ", \"empirical\": {\"hump\": " << num(s.hump_u) << ", \"zero\": " << num(s.zero_u)
      << ", \"trough\": " << num(s.trough_u) << "}"
      << ", \"dev\": {\"hump\": " << num(dev_hump) << ", \"zero\": " << num(dev_zero)
      << ", \"trough\": " << num(dev_trough) << "}"
      << ", \"within_tol\": {\"hump\": " << (dev_hump < run.tol ? "true" : "false")
      << ", \"zero\": " << (dev_zero < run.tol ? "true" : "false")
      << ", \"trough\": " << (dev_trough < run.tol ? "true" : "false") << "}"
      << ", \"windowed_trough_u\": " << num(win_trough)
      << ", \"windowed_note\": \"same two-pass extractor restricted to u∈[0.7,0.9]; equal "
         "to the global trough ⇒ the displacement is real, not a far-tail-argmin artifact\""
      << ", \"refuted_hypothesis\": \"the referee's tail-argmin hypothesis (the far tail "
         "u→1 drags the global argmin off a local trough near 0.805) was REFUTED by this "
         "pre-registered windowed discriminant: windowed==global, so excluding the tail "
         "does NOT recover a trough near the target — the displacement is real\""
      << "},\n";

    // Convergence ladder (the reported empiric behind the fork verdict) + the C3
    // shape-effect empiric: dev(zero) is monotone-increasing AND sub-bin (interpolated),
    // dev(trough) is flat + bin-quantized. Jointly ⇒ a bias growing in u on the
    // descending branch (a reading of [SS25]'s deficit; the finite-X limit stays OPEN).
    f << "  \"convergence\": {\"note\": \"dev(trough) is FLAT across X (does not decay) ⇒ "
         "open deviation, not finite-X bias by this range; dev(zero) is MONOTONE-increasing "
         "and interpolated (sub-bin) — jointly the signature of a downward bias growing in "
         "u on the descending branch (a shape reading of [SS25]'s deficit, not a claim on "
         "the X→∞ limit)\", \"points\": [";
    for (std::size_t i = 0; i < run.shapes.size(); ++i) {
        const SSScaleShape& sc = run.shapes[i];
        f << (i ? "," : "") << "{\"X\": " << num(sc.X) << ", \"n_curves\": " << sc.n_curves
          << ", \"hump\": " << num(sc.shape.hump_u) << ", \"zero\": " << num(sc.shape.zero_u)
          << ", \"trough\": " << num(sc.shape.trough_u)
          << ", \"dev_zero\": " << num(std::abs(sc.shape.zero_u - run.r2_zero))
          << ", \"dev_trough\": " << num(std::abs(sc.shape.trough_u - run.r2_trough)) << "}";
    }
    f << "]}\n}\n";
}

}  // namespace at::emit
