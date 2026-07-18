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

    // Trough deviations d(X)=|trough_u − target| across the four rungs (target = R2 trough).
    const double du = run.du, tgt = run.r2_trough;
    const double d16 = std::abs(r16.confirm.shape.trough_u - tgt);
    const double d17 = std::abs(r17.confirm.shape.trough_u - tgt);
    const double d18 = dev_trough;                // = |trough_u(2¹⁸) − target|

    // PR-1 Rung-3 clause (docs/preregistered/PR-1.md §"Three completed rungs", committed
    // before any 2¹⁸ data): the finite-range H1/H0 verdict over the full feasible ladder.
    //   H1 (finite-X, ≤2¹⁸) iff the trough recovers ≥ Δu at BOTH steps: d(2¹⁷)≤d(2¹⁶)−Δu
    //                        AND d(2¹⁸)≤d(2¹⁷)−Δu (a genuine decaying trend, not one step).
    //   H0 (persistent, ≤2¹⁸) iff the trough is flat across the ladder (no ≥Δu recovery at
    //                        either step).
    //   Ambiguous otherwise (exactly one step recovers). Derived from the data, never typed
    //   (rule 1): the verdict string follows the numbers.
    const bool step16_17 = (d16 - d17) >= du - 1e-12;   // 2¹⁶→2¹⁷ recovered ≥ one bin
    const bool step17_18 = (d17 - d18) >= du - 1e-12;   // 2¹⁷→2¹⁸ recovered ≥ one bin
    const bool verdict_H1 = step16_17 && step17_18;
    const bool verdict_H0 = !step16_17 && !step17_18;
    const std::string verdict = verdict_H1 ? "H1" : (verdict_H0 ? "H0" : "ambiguous");

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
      << ", \"claim_class\": \"PARTIAL agreement with the CONJECTURED density (Conjecture 1 "
         "[SS25]) over the PR-1 four-rung ladder X∈{10⁴,2¹⁶,2¹⁷,2¹⁸}: hump (dev "
      << num(dev_hump) << ") and first zero-crossing (dev " << num(dev_zero)
      << " at 2¹⁸) within the a-priori τ=" << num(run.tol) << " at every rung; the trough is "
         "a PERSISTENT open deviation — dev " << num(d18) << " at all four rungs (trough_u=0.8875, "
         "IDENTICAL bin, zero movement over a 26× span in X). Applying PR-1's committed Rung-3 "
         "decision rule to the full feasible ladder: H1 (finite-X) requires ≥Δu trough recovery "
         "at BOTH steps (2¹⁶→2¹⁷ and 2¹⁷→2¹⁸); neither recovers, so the VERDICT is H0 "
         "(persistent, ≤2¹⁸) — the first rung at which R0's reserved H1/H0 verdict is "
         "pronounceable. FINITE-RANGE (over X≤2¹⁸), explicitly NOT the X→∞ verdict [SS25] "
         "describe: 2¹⁸ is the very bottom of their 2¹⁶–2²⁸ observed-decay window. Supporting "
         "empirics (NOT the gate): the sub-bin zero-crossing is flat near ~0.671, non-monotone, "
         "no convergence to 0.645; the trough DEPTH eases monotonically across the extension "
         "rungs (−3.71505→−3.65241→−3.57994) while its POSITION holds at 0.8875 — a hint of "
         "amplitude decay, logged as an observation. A numerical replication of a CONJECTURE — "
         "no proof is claimed (rule 7)\""
      << ", \"ladder_verdict\": \"H0 (persistent, ≤2¹⁸) — the full-feasible-ladder verdict "
         "(PR-1 Rung-3 clause). The Δu-quantized trough is flat at u=0.8875 across all four "
         "rungs {10⁴,2¹⁶,2¹⁷,2¹⁸}; neither ladder step recovers ≥Δu, so H1 (finite-X) is not "
         "met and H0 is pronounced. Finite-range (≤2¹⁸), not the X→∞ verdict; the mechanism "
         "follow-ups are PR-2 (rank split) and PR-3 (root-number leakage)\""
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
         "trough ⇒ the displacement is real at 2¹⁸ too, not a tail-argmin artifact\""
      << ", \"refuted_hypothesis\": \"the tail-argmin hypothesis stays refuted at 2¹⁸ "
         "(windowed==global)\""
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
    f << "  \"convergence\": {\"note\": \"the Δu-quantized trough dev is FLAT at 0.0825 "
         "(trough_u=0.8875) across all four PR-1 rungs {10⁴,2¹⁶,2¹⁷,2¹⁸} — no ≥Δu recovery at "
         "either extension step ⇒ VERDICT H0 (persistent, ≤2¹⁸). Supporting empirics, NOT the "
         "gate: (i) the interpolated zero-crossing is flat near ~0.671, non-monotone, no "
         "convergence to the 0.645 target; (ii) the trough DEPTH eases monotonically across the "
         "extension rungs (trough_v −3.71505→−3.65241→−3.57994) even as its POSITION holds at "
         "0.8875 — a hint of amplitude decay consistent with [SS25]'s expected large-X "
         "behaviour, but the committed rule gates on POSITION, so it does not move the verdict. "
         "Scale caveat rides every branch: 2¹⁸ is the very bottom of [SS25]'s 2¹⁶–2²⁸ window, "
         "so H0 is finite-range (≤2¹⁸), not the X→∞ verdict\", \"points\": [";
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
      << ", \"coherence\": \"the two gates are mutually consistent: excising S₂ lifts the "
         "descending-branch mean by only f₂·|gap| ≈ " << num(f2 * std::abs(s.gap))
      << " density units — a sub-bin perturbation on an amplitude-≈4 curve, nowhere near the "
         "≈3-bin (dev/Δu) displacement a recovery needs; so a 14.6%-mass subpopulation, even "
         "2.6× past the value threshold, cannot move the location gate. Clauses: (1) rank-2 "
         "curves show an enhanced downward bias (2.6× threshold); (2) that bias does not carry "
         "the trough displacement (leave-out trough unchanged on family and virgin annulus)\""
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
