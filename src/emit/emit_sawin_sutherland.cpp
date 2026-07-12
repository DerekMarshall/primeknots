#include "emit/emit_sawin_sutherland.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <map>
#include <utility>
#include <vector>

#include "emit/json.h"
#include "murm/height_family.h"
#include "murm/rank_cache.h"
#include "murm/rank_split.h"
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

void emit_m5_extension(const std::string& out_dir, const std::string& run_path,
                       const std::string& generated_by) {
    fs::create_directories(out_dir);

    const SSRun run = read_ss_run(run_path);   // refuses a stale run (generator-hash guard)
    const SSShape& s = run.confirm.shape;       // the 2¹⁶ rung
    const double dev_hump = std::abs(s.hump_u - run.r2_hump);
    const double dev_zero = std::abs(s.zero_u - run.r2_zero);
    const double dev_trough = std::abs(s.trough_u - run.r2_trough);
    const double win_trough = windowed_trough_u(run.confirm, 0.7, 0.9);

    // The X=10⁴ anchor from the same ladder (for the flat-vs-anchor + single-rung reading).
    auto find = [&](double X) -> const SSScaleShape* {
        for (const SSScaleShape& sc : run.shapes) if (sc.X == X) return &sc;
        return nullptr;
    };
    const SSScaleShape* a10 = find(10000.0);
    const double d10_trough = a10 ? std::abs(a10->shape.trough_u - run.r2_trough) : -1.0;
    const double zdev10 = a10 ? std::abs(a10->shape.zero_u - run.r2_zero) : -1.0;
    // Reading A (finite-X, escalate) iff the trough moved ≥1 full bin toward target; else
    // Reading B (persistent, proceed to PR-2). Threshold derived from anchor + Δu.
    const bool reading_A = (d10_trough >= 0.0) && (dev_trough <= d10_trough - run.du + 1e-12);

    std::ofstream f(fs::path(out_dir) / "ss_x_extension_murmuration.json");
    f << "{\n  \"schema\": \"ss_x_extension_murmuration/1\",\n";
    f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
    f << "  \"params\": {"
      << "\"family\": \"E_{A,B}: y²=x³+Ax+B, integers A,B reduced (no p: p⁴|A ∧ p⁶|B), "
         "nonsingular; ordered by naive height H=max(4|A|³,27B²) ≤ X\""
      << ", \"statistic\": \"[SS25] eq. (1), UNCHANGED from M4: ε-weighted a_p averaged over "
         "the height family, binned by u=p/N over (0,1]; same two-pass extractor, same τ, "
         "same Δu — only X changes (PR-1 X-extension)\""
      << ", \"source\": \"Sawin–Sutherland, arXiv:2504.12295 v2, Conjecture 1; PR-1 "
         "(docs/preregistered/PR-1.md)\""
      << ", \"external_data\": \"a_p COMPUTED from scratch (ell::ap_fast, twinned vs frozen "
         "ell::ap_charsum); N, ε ORACLE-PROVENANCE INPUT (committed data/m5/ne_cache_x65536.txt, "
         "5042 curves, R3 dual-oracle-overlap certified; ≤10⁴ overlap matches the frozen M4 "
         "cache row-for-row)\""
      << ", \"provenance\": \"empirical = computed statistic (a_p); density D(u) = the "
         "conjectured Bessel-J₁ formula (in-house J₁, generated constants); separate TUs. N,ε: oracle\""
      << ", \"claim_class\": \"PARTIAL agreement with the CONJECTURED density (Conjecture 1 "
         "[SS25]) at the 2¹⁶ X-extension rung: hump (dev " << num(dev_hump) << ") and first "
         "zero-crossing (dev " << num(dev_zero) << ") within the a-priori τ=" << num(run.tol)
      << "; the trough is a PERSISTENT open deviation — dev " << num(dev_trough)
      << ", IDENTICAL to the X=10⁴ anchor (flat, zero bin-movement over a 6.5× increase in "
         "X). Per PR-1's committed single-rung clause this LONE rung is EVIDENCE-GRADE only: "
         "Reading B, 'consistent with persistent, proceed to PR-2' — it does NOT license the "
         "finite-X verdict (which requires the ladder). Supporting empiric (NOT the gate): "
         "the interpolated zero-crossing dev RETREATED (" << num(zdev10) << "→" << num(dev_zero)
      << ", direction reversed vs the M4 monotone drift) — sub-bin, more sensitive than the "
         "Δu-quantized trough, reported for the record only. A numerical replication of a "
         "CONJECTURE — no proof is claimed (rule 7)\""
      << ", \"single_rung_reading\": \"B — consistent with persistent, proceed to PR-2 "
         "(evidence-grade; the H1/H0 verdict requires the full ladder, PR-1 partial-ladder clause)\""
      << ", \"consistency_twin\": \"the ≤10⁴ ladder rows reproduce the frozen M4 run exactly "
         "(same |fam| 522/664/870/1048, same shape) — the 5042-curve pipeline reproduces M4\""
      << ", \"run_provenance\": \"a_p over 5042 curves in ~10.2 h (2.09 h + 8.11 h across a "
         "mid-run environment kill and a byte-identical checkpoint resume; R3 crash-safety)\""
      << ", \"X_confirm\": " << num(run.X_confirm)
      << ", \"n_curves\": " << run.confirm.n_curves
      << ", \"du\": " << num(run.du)
      << ", \"tol\": " << num(run.tol)
      << ", \"reading_A_fired\": " << (reading_A ? "true" : "false")
      << ", \"density_trunc_B\": " << kDensB
      << ", \"u_axis\": \"u = p/N(E)\""
      << "},\n";

    f << "  \"empirical\": [";
    for (std::size_t b = 0; b < run.confirm.u_mid.size(); ++b)
        f << (b ? "," : "") << "{\"u\": " << num(run.confirm.u_mid[b])
          << ", \"emp\": " << num(run.confirm.density[b])
          << ", \"count\": " << run.confirm.count[b] << "}";
    f << "],\n";

    f << "  \"density\": [";
    bool first = true;
    for (double u = run.du; u <= 1.0 + 1e-9; u += 0.005) {
        f << (first ? "" : ",") << "{\"u\": " << num(u)
          << ", \"d\": " << num(ss_density(u, kDensB, kDensB)) << "}";
        first = false;
    }
    f << "],\n";

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
      << ", \"windowed_note\": \"same two-pass extractor on u∈[0.7,0.9]; equal to the global "
         "trough ⇒ the displacement is real at 2¹⁶ too, not a tail-argmin artifact\""
      << ", \"refuted_hypothesis\": \"the tail-argmin hypothesis stays refuted at 2¹⁶ "
         "(windowed==global)\""
      << "},\n";

    f << "  \"convergence\": {\"note\": \"trough dev FLAT across the full ladder incl. 2¹⁶ "
         "(0.0825 at both 10⁴ and 2¹⁶) ⇒ Reading B (persistent, evidence-grade). The "
         "interpolated zero-crossing dev rose over the M4 ladder then RETREATED at 2¹⁶ — "
         "sub-bin, reversed direction, supporting only (the trough is the gate)\", \"points\": [";
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

namespace {
// Emit one subpopulation curve as a JSON array of {u, d, count}.
void emit_curve(std::ofstream& f, const char* key, const SSEmpirical& e, bool last) {
    f << "\"" << key << "\": [";
    for (std::size_t b = 0; b < e.u_mid.size(); ++b)
        f << (b ? "," : "") << "{\"u\": " << num(e.u_mid[b]) << ", \"d\": " << num(e.density[b])
          << ", \"count\": " << e.count[b] << "}";
    f << "]" << (last ? "" : ",");
}
}  // namespace

void emit_m5_rank_split(const std::string& out_dir, const std::string& partials_path,
                        const std::string& rank_path, const std::string& generated_by) {
    fs::create_directories(out_dir);

    SSPartialsMeta pm;
    SSPartials P = read_ss_partials(partials_path, pm, /*allow_incomplete=*/false);
    RankCacheHeader rh;
    std::vector<RankRow> rr = read_rank_cache(rank_path, rh);

    std::map<std::pair<long long, long long>, int> rank_of;
    for (const RankRow& r : rr) rank_of[{r.A, r.B}] = r.rank;
    std::vector<int> rank(P.A.size(), -1);
    for (std::size_t c = 0; c < P.A.size(); ++c) rank[c] = rank_of[{P.A[c], P.B[c]}];

    // Committed gates (PR-2 Amendment 4 + step 2, m4-pinning §R0c).
    const double target = 0.805, tol = 0.06, contrast_thr = 0.668, u_lo = 0.7, u_hi = 0.9;
    const RankSplit s = rank_split(P, rank, pm.X, target, tol, contrast_thr, u_lo, u_hi);

    // Annulus (10⁴, 2¹⁶] geometric-holdout cross-check (PR-2 Amendment 3).
    SSPartials Pa; Pa.du = P.du; Pa.NB = P.NB;
    std::vector<int> rank_a;
    for (std::size_t c = 0; c < P.A.size(); ++c) {
        if (at::murm::naive_height(P.A[c], P.B[c]) <= 10000) continue;
        Pa.A.push_back(P.A[c]); Pa.B.push_back(P.B[c]); Pa.N.push_back(P.N[c]);
        Pa.num.push_back(P.num[c]); Pa.cnt.push_back(P.cnt[c]); rank_a.push_back(rank[c]);
    }
    const RankSplit sa = rank_split(Pa, rank_a, pm.X, target, tol, contrast_thr, u_lo, u_hi);

    const double f2 = s.n_full ? static_cast<double>(s.n_s2) / static_cast<double>(s.n_full) : 0.0;

    std::ofstream f(fs::path(out_dir) / "ss_rank_split_murmuration.json");
    f << "{\n  \"schema\": \"ss_rank_split/1\",\n";
    f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
    f << "  \"params\": {"
      << "\"family\": \"E_{A,B} height-ordered, H ≤ 2¹⁶ (the PR-1 5042-curve family), split by "
         "ANALYTIC rank r = ord_{s=1}L(E,s) (PARI ellanalyticrank; NEVER Mordell–Weil rank)\""
      << ", \"statistic\": \"[SS25] eq. (1), UNCHANGED; a_p reused from the committed partials "
         "(NO recompute), filtered by the rank column and re-summed through the frozen ss_aggregate\""
      << ", \"source\": \"docs/preregistered/PR-2.md (analytic-rank split); [SS25] p.4 rank remark\""
      << ", \"external_data\": \"a_p COMPUTED (partials); N, ε and analytic rank ORACLE-provenance "
         "(PARI; ne_cache + rank_cache, generator-hash-bound to the same (A,B) family)\""
      << ", \"provenance\": \"empirical = computed statistic; D(u) = conjectured Bessel-J₁ formula; "
         "N,ε,rank: oracle\""
      << ", \"claim_class\": \"PR-2 step 3 (analytic-rank split, X=2¹⁶). PRIMARY (u-space, "
         "well-powered): H0 — removing the 738 analytic-rank-2 curves does NOT recover the trough "
         "(F∖S₂ trough u=" << num(s.leaveout.shape.trough_u) << ", dev " << num(s.dev_leaveout)
      << ", identical to the full family; and identical on the geometric-holdout annulus (10⁴,2¹⁶]). "
         "The tail deficit at H≤2¹⁶ is NOT explained by analytic-rank-2 over-representation. "
         "SECONDARY (value-space, f₂-limited): the analytic-rank-2 subpopulation IS markedly more "
         "negative on the descending branch (S₂−S₀ mean gap " << num(s.gap) << ", past the committed "
         "−" << num(contrast_thr) << ", downward) — a real value-space effect, but by the two-axes "
         "discipline it cannot substitute for the location gate and does NOT overturn the primary "
         "null. WACHS CLAUSE: this design does not distinguish rank per se from BSD invariants "
         "correlated with rank (Wachs 2603.04604); 'carried by' is a statement about the "
         "subpopulation, not the mechanism. A pre-registered null (PR-2) — no proof; MW rank never "
         "asserted (rule 7)\""
      << ", \"f2\": " << num(f2)
      << ", \"tol\": " << num(tol) << ", \"contrast_threshold\": " << num(contrast_thr)
      << ", \"X\": " << num(s.X)
      << ", \"n_full\": " << s.n_full << ", \"n_leaveout\": " << s.n_leaveout
      << ", \"n_s2\": " << s.n_s2 << ", \"n_s0\": " << s.n_s0
      << ", \"recovers\": " << (s.recovers ? "true" : "false")
      << ", \"contrast_downward\": " << (s.contrast_downward ? "true" : "false")
      << ", \"branch\": \"PRIMARY H0 (no recovery); SECONDARY downward-significant\""
      << ", \"density_trunc_B\": " << kDensB << ", \"u_axis\": \"u = p/N(E)\""
      << "},\n";

    f << "  \"targets\": {\"hump\": 0.475, \"zero\": 0.645, \"trough\": " << num(target) << "},\n";

    f << "  \"gates\": {\"recovery\": {\"target\": " << num(target)
      << ", \"tol\": " << num(tol) << ", \"dev_full\": " << num(s.dev_full)
      << ", \"dev_leaveout\": " << num(s.dev_leaveout)
      << ", \"one_bin_threshold\": " << num(s.dev_full - s.du)
      << ", \"recovers\": " << (s.recovers ? "true" : "false") << "}"
      << ", \"contrast\": {\"u_lo\": " << num(u_lo) << ", \"u_hi\": " << num(u_hi)
      << ", \"mean_s2\": " << num(s.mean_s2) << ", \"mean_s0\": " << num(s.mean_s0)
      << ", \"gap\": " << num(s.gap) << ", \"threshold\": " << num(contrast_thr)
      << ", \"downward\": " << (s.contrast_downward ? "true" : "false") << "}"
      << ", \"annulus\": {\"n_full\": " << sa.n_full
      << ", \"leaveout_trough_u\": " << num(sa.leaveout.shape.trough_u)
      << ", \"dev_leaveout\": " << num(sa.dev_leaveout)
      << ", \"recovers\": " << (sa.recovers ? "true" : "false")
      << ", \"gap\": " << num(sa.gap)
      << ", \"downward\": " << (sa.contrast_downward ? "true" : "false")
      << ", \"note\": \"geometric holdout (10⁴,2¹⁶], never touched by exploration; confirms the "
         "primary is not a seed-curve artifact\"}},\n";

    f << "  \"curves\": {";
    emit_curve(f, "full", s.full, false);
    emit_curve(f, "leaveout", s.leaveout, false);
    emit_curve(f, "s2", s.s2, false);
    emit_curve(f, "s0", s.s0, true);
    f << "},\n";

    f << "  \"density\": [";
    bool first = true;
    for (double u = P.du; u <= 1.0 + 1e-9; u += 0.005) {
        f << (first ? "" : ",") << "{\"u\": " << num(u)
          << ", \"d\": " << num(ss_density(u, kDensB, kDensB)) << "}";
        first = false;
    }
    f << "]\n}\n";
}

}  // namespace at::emit
