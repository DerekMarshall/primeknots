#include "emit/emit_sawin_sutherland.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

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
      << ", \"claim_class\": \"From-scratch replication of [SS25] Conjecture 1 at X_confirm=10⁴. "
         "An initial comparison flagged an apparent trough displacement (from a 0.805 target); a "
         "pre-registered audit round traced it to a TRANSCRIPTION ERROR in eq (2) (products "
         "swapped, wrong exponent) — ERRATA #28. Corrected (density trough 0.870), the hump (dev "
      << num(dev_hump) << "), zero (dev " << num(dev_zero) << "), and trough (dev "
      << num(dev_trough) << ") all agree within the a-priori τ=" << num(run.tol) << " — the "
         "trough a small resolved residual, not a deviation. A numerical replication of a "
         "CONJECTURE — no proof is claimed (rule 7)\""
      << ", \"X_confirm\": " << num(run.X_confirm)
      << ", \"n_curves\": " << run.confirm.n_curves
      << ", \"du\": " << num(run.du)
      << ", \"tol\": " << num(run.tol)
      << ", \"density_trunc_B\": " << kDensB
      << ", \"u_axis\": \"u = p/N(E)\""
      << ", \"downward_bias_note\": \"[SS25] p.3–4 document a persistent finite-X downward bias "
         "in (1); Table 4 gives its size (mean 1.17/0.91/0.73 at 2¹⁶/2¹⁷/2¹⁸). Our residuals "
         "(≤~0.012 in u after ERRATA #28) sit an order of magnitude inside that regime\""
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
         "to the global trough (the earlier tail-argmin check)\""
      << ", \"refuted_hypothesis\": \"the earlier 'trough displacement' was the eq (2) "
         "transcription error (ERRATA #28), not a tail-argmin artifact and not real; against "
         "the corrected density trough 0.870 the empirical trough is within τ\""
      << "},\n";

    // Convergence ladder (the reported empiric behind the fork verdict) + the C3
    // shape-effect empiric: dev(zero) is monotone-increasing AND sub-bin (interpolated),
    // dev(trough) is flat + bin-quantized. Jointly ⇒ a bias growing in u on the
    // descending branch (a reading of [SS25]'s deficit; the finite-X limit stays OPEN).
    f << "  \"convergence\": {\"note\": \"Corrected (ERRATA #28): against the density trough "
         "0.870 the trough dev is a small resolved residual within τ, not the spurious "
         "displacement the swapped products produced; the hump and zero are on target\", "
         "\"points\": [";
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

    // PR-1's four-rung ladder {10⁴, 2¹⁶, 2¹⁷, 2¹⁸} lives in three committed run files that
    // sit together under data/m5/. run_path is the VERDICT rung (2¹⁸ = ss_x262144.txt, the
    // M0b-produced run); the 2¹⁶/2¹⁷ rungs are read from its siblings in the same directory.
    // Each read_ss_run REFUSES a stale run (generator-hash guard), so a missing or wrong
    // sibling fails the emit loudly — a partial ladder is never emitted silently (rule 5).
    const SSRun run = read_ss_run(run_path);                             // 2¹⁸ (primary/verdict)
    const fs::path dir = fs::path(run_path).parent_path();
    const SSRun r16 = read_ss_run((dir / "ss_x65536.txt").string());    // 2¹⁶ rung
    const SSRun r17 = read_ss_run((dir / "ss_x131072.txt").string());   // 2¹⁷ rung

    // Guard the rung identities: emit the PR-1 artifact only from the PR-1 ladder, so a
    // stray --ss-run can never mislabel the data (the rungs are the fixed pre-registration).
    if (run.X_confirm != 262144.0)
        throw std::runtime_error("emit_m5_extension: verdict run is not the 2^18 rung: " + run_path);
    if (r16.X_confirm != 65536.0 || r17.X_confirm != 131072.0)
        throw std::runtime_error("emit_m5_extension: sibling rungs are not {2^16,2^17} in " + dir.string());

    // The 10⁴ trend anchor from the primary run's internal ladder (present in every file).
    const SSScaleShape* a10 = nullptr;
    for (const SSScaleShape& sc : run.shapes) if (sc.X == 10000.0) a10 = &sc;
    if (!a10) throw std::runtime_error("emit_m5_extension: 10^4 anchor row absent from " + run_path);

    const SSShape& s = run.confirm.shape;         // the 2¹⁸ rung (the primary/verdict rung)
    const double dev_hump = std::abs(s.hump_u - run.r2_hump);
    const double dev_zero = std::abs(s.zero_u - run.r2_zero);
    const double dev_trough = std::abs(s.trough_u - run.r2_trough);
    const double win_trough = windowed_trough_u(run.confirm, 0.7, 0.9);

    const double d18 = dev_trough;                // |trough_u(2¹⁸) − target|, for the shape display

    // PR-1's Rung-3 clause is SUPERSEDED (ERRATA #28): it gated |trough_u − 0.805| against a
    // target the eq (2) transcription fix invalidates. It is NOT re-run against the corrected
    // 0.870 (never registered against it); the earlier H0 (pronounced 8f64ba1) is void as
    // pronounced. The emit reports the corrected per-invariant comparison, not a verdict.
    const std::string verdict = "superseded";

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
         "(docs/preregistered/PR-1.md), Rung-3 clause + finite-range four-rung verdict\""
      << ", \"external_data\": \"a_p COMPUTED from scratch — 2¹⁶/2¹⁷ via ell::ap_fast (O(p)), "
         "2¹⁸ via ell::ap_shanks_mestre (M0b, O(p^{1/4})), each twinned vs the frozen "
         "ell::ap_charsum (and M0b exact-match vs ap_fast on the checksummed caches); a_p is "
         "a path-independent integer. N, ε ORACLE-PROVENANCE INPUT (committed "
         "data/m5/ne_cache_x{65536,131072,262144}.txt, R3 dual-oracle-overlap certified; ≤10⁴ "
         "overlap matches the frozen M4 cache row-for-row)\""
      << ", \"provenance\": \"empirical = computed statistic (a_p); density D(u) = the "
         "conjectured Bessel-J₁ formula (in-house J₁, generated constants); separate TUs. N,ε: oracle\""
      << ", \"claim_class\": \"From-scratch replication of [SS25] Conjecture 1 over the PR-1 "
         "four-rung ladder X∈{10⁴,2¹⁶,2¹⁷,2¹⁸}. An initial comparison showed an apparent "
         "persistent trough displacement (≈0.0825 from a 0.805 target); a pre-registered audit "
         "round traced it to a TRANSCRIPTION ERROR in our reading of eq (2) — the two products "
         "swapped and the wrong exponent — ERRATA #28. Corrected (density trough 0.870), all "
         "three shape invariants agree within the a-priori τ=" << num(run.tol) << " at 2¹⁸: hump "
         "(dev " << num(dev_hump) << "), zero (dev " << num(dev_zero) << "), trough (dev "
      << num(d18) << ", binned) — the zero on target, the hump and trough small resolved "
         "residuals — at the bottom of the authors' 2¹⁶–2²⁸ window, where their own "
         "empirical-density discrepancy is an order of magnitude larger (Table 4). Replication "
         "and constraint, not proof (rule 7)\""
      << ", \"ladder_verdict\": \"SUPERSEDED (ERRATA #28). PR-1's Rung-3 clause gated "
         "|trough_u − 0.805| against a target the eq (2) transcription fix invalidates: rule "
         "committed (dd6beb0, 4a17ebe), rule followed, H0 pronounced (8f64ba1), target then "
         "found corrupted. The verdict is void as pronounced; it is NOT re-run against the "
         "corrected 0.870, which it was never registered against. The mechanism follow-ups "
         "(PR-2 rank split, PR-3 leakage) stand as bounds established during the investigation\""
      << ", \"consistency_twin\": \"the 2¹⁸ run is M0b-produced (ap_shanks_mestre, O(p^{1/4}), "
         "--ap m0b) and validated before the verdict is read: (i) its ≤10⁴ ladder shapes "
         "reproduce the frozen M4 run byte-for-byte; (ii) its H≤2¹⁷ subset reproduces the "
         "committed FreeBSD 2¹⁷ partials EXACTLY — all 9014 curves, full precision (M0b's a_p == "
         "the O(p) referee's on real data); (iii) its H≤2¹⁶ subset, re-aggregated at 65536, "
         "reproduces the committed macOS 2¹⁶ density to the 6-figure print floor "
         "(max|ΔS|=5×10⁻⁶, cross-platform). Family count 15936 matches the sieve-certified "
         "ne_cache and the 5/6-power law (2¹⁶→2¹⁷→2¹⁸ ratios 1.788,1.768 vs 2^{5/6}=1.782)\""
      << ", \"run_provenance\": \"the 2¹⁸ rung is M0b's FIRST production run and the first "
         "M0b-produced committed artifact — a_p over 15936 curves in ~16.9 min (1014 s) on 48 "
         "FreeBSD cores via ap_shanks_mestre, conductor-sorted chunked (chunk 512), real-FS "
         "checkpoints, no mid-run shape read (R3); R1 provenance stamped in the header "
         "(ap_provider m0b, provider_hash bc3f174a…). Speedup ≈330× over the pre-M0b O(p) "
         "estimate (~3.9 d on 48 cores). The 2¹⁶/2¹⁷ rungs are ap_fast (O(p), twinned vs the "
         "frozen ap_charsum); a_p is a path-independent integer\""
      << ", \"X_confirm\": " << num(run.X_confirm)
      << ", \"n_curves\": " << run.confirm.n_curves
      << ", \"du\": " << num(run.du)
      << ", \"tol\": " << num(run.tol)
      << ", \"verdict\": \"" << verdict << "\""
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
         "trough (the earlier tail-argmin check)\""
      << ", \"refuted_hypothesis\": \"the earlier 'trough displacement' was neither a "
         "tail-argmin artifact nor real — it was the eq (2) transcription error (ERRATA #28); "
         "against the corrected density trough 0.870 the empirical trough is within τ\""
      << "},\n";

    // The convergence ladder is now the four PR-1 rungs {10⁴,2¹⁶,2¹⁷,2¹⁸} the verdict is
    // pronounced over (matching PR-1's Rung-3 postscript table), NOT the M4 exploration
    // sub-ladder. 10⁴ is the trend anchor; the three extension rungs are each file's confirm.
    struct Rung { double X; i64 n; SSShape sh; };
    const Rung ladder[] = {
        {10000.0,  a10->n_curves,         a10->shape},
        {65536.0,  r16.confirm.n_curves,  r16.confirm.shape},
        {131072.0, r17.confirm.n_curves,  r17.confirm.shape},
        {262144.0, run.confirm.n_curves,  run.confirm.shape},
    };
    f << "  \"convergence\": {\"note\": \"Corrected (ERRATA #28): against the density trough "
         "0.870, the trough dev is ≈0.0175 (binned) / ≈0.012 (interpolated) — within τ=0.06, a "
         "small resolved residual, not the spurious 0.0825 the swapped products produced. The "
         "zero sits on target (~0.671) and the hump is on target (~0.465). Both non-zero "
         "residuals point RIGHTWARD (empirical > target) — consistent with a single small "
         "horizontal dilation (PR-5 territory: whether that shift decays with X, gated before "
         "any rung beyond 2¹⁸). 2¹⁸ is the bottom of [SS25]'s 2¹⁶–2²⁸ window\", \"points\": [";
    bool fp = true;
    for (const Rung& rg : ladder) {
        f << (fp ? "" : ",") << "{\"X\": " << num(rg.X) << ", \"n_curves\": " << rg.n
          << ", \"hump\": " << num(rg.sh.hump_u) << ", \"zero\": " << num(rg.sh.zero_u)
          << ", \"trough\": " << num(rg.sh.trough_u)
          << ", \"dev_zero\": " << num(std::abs(rg.sh.zero_u - run.r2_zero))
          << ", \"dev_trough\": " << num(std::abs(rg.sh.trough_u - run.r2_trough)) << "}";
        fp = false;
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

    // Committed gates (PR-2 Amendment 4 + step 2, m4-pinning §R0c). The recovery-gate TARGET
    // is corrected 0.805→0.870 (ERRATA #28); the PR-2 recovery *verdict* is superseded (its
    // gate referenced the corrupted target) — the MEASUREMENT (leave-out trough position)
    // stands regardless, being a statement about the empirical curve, not the density target.
    const double target = 0.870, tol = 0.06, contrast_thr = 0.668, u_lo = 0.7, u_hi = 0.9;
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
      << ", \"claim_class\": \"PR-2 step 3 (analytic-rank split, X=2¹⁶), REFRAMED (ERRATA #28). "
         "The MEASUREMENT stands: removing the 738 analytic-rank-2 curves does NOT move the "
         "empirical trough (F∖S₂ trough u=" << num(s.leaveout.shape.trough_u) << ", identical to "
         "the full family and on the geometric-holdout annulus (10⁴,2¹⁶]) — a bound on the "
         "trough's sensitivity to that subpopulation. It was established while investigating a "
         "trough 'displacement' later traced to our eq (2) transcription (ERRATA #28); the PR-2 "
         "recovery-gate VERDICT is superseded (its gate targeted the corrupted 0.805). SECONDARY "
         "(value-space): the rank-2 subpopulation is markedly more negative on the descending "
         "branch (S₂−S₀ mean gap " << num(s.gap) << ", past the committed −" << num(contrast_thr)
      << ") — a real value-space effect, retained as a measurement. WACHS CLAUSE: rank per se is "
         "not distinguished from correlated BSD invariants (Wachs 2603.04604). No proof; MW rank "
         "never asserted (rule 7)\""
      << ", \"f2\": " << num(f2)
      << ", \"tol\": " << num(tol) << ", \"contrast_threshold\": " << num(contrast_thr)
      << ", \"X\": " << num(s.X)
      << ", \"n_full\": " << s.n_full << ", \"n_leaveout\": " << s.n_leaveout
      << ", \"n_s2\": " << s.n_s2 << ", \"n_s0\": " << s.n_s0
      << ", \"recovers\": " << (s.recovers ? "true" : "false")
      << ", \"contrast_downward\": " << (s.contrast_downward ? "true" : "false")
      << ", \"branch\": \"PRIMARY H0 (no recovery); SECONDARY downward-significant\""
      << ", \"coherence\": \"the two measurements are mutually consistent: excising S₂ lifts the "
         "descending-branch mean by only f₂·|gap| ≈ " << num(f2 * std::abs(s.gap))
      << " density units — a sub-bin perturbation on an amplitude-≈4 curve, so a 14.6%-mass "
         "subpopulation, even 2.6× past the value threshold, cannot move the empirical trough "
         "POSITION. Clauses: (1) rank-2 curves show an enhanced downward bias (2.6× threshold); "
         "(2) that bias does not move the trough position (leave-out trough unchanged on family "
         "and virgin annulus)\""
      << ", \"density_trunc_B\": " << kDensB << ", \"u_axis\": \"u = p/N(E)\""
      << "},\n";

    f << "  \"targets\": {\"hump\": 0.465, \"zero\": 0.671, \"trough\": " << num(target) << "},\n";

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
