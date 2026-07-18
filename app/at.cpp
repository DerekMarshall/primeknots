// at — the arithmetic-topology toolkit CLI.
//
// `at emit --stage <N> --out <dir>` writes the visualization JSON for a stage
// (ARCHITECTURE.md §5). Stage 1 emits the linking matrix + graph. Later stages
// register here as they are built. See docs/notes/open-questions.md R2 for why
// this lives in app/.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <map>
#include <numeric>
#include <sstream>
#include <thread>
#include <utility>
#include <vector>

#include "emit/emit_borromean.h"
#include "emit/emit_classgroups.h"
#include "emit/emit_cs.h"
#include "ell/ap.h"
#include "ell/ap_cache.h"
#include "ell/ap_fast.h"
#include "ell/curve.h"
#include "ell/ecdata.h"
#include "emit/emit_dirichlet.h"
#include "emit/emit_dw.h"
#include "emit/emit_linking.h"
#include "emit/emit_murmuration.h"
#include "emit/emit_sawin_sutherland.h"
#include "emit/emit_zeta.h"
#include "emit/emit_zubrilina.h"
#include "murm/height_family.h"
#include "murm/ne_cache.h"
#include "murm/rank_cache.h"
#include "murm/rank_split.h"
#include "murm/ss_empirical.h"
#include "oracle/pari.h"

#ifndef AT_GIT_FALLBACK
#define AT_GIT_FALLBACK "unknown"
#endif

using at::core::i64;

namespace {
// Provenance resolved at emit time: current `git describe`, so generated_by
// reflects the working state when the JSON was produced (not configure time).
// Falls back to the configure-time value when run outside a git checkout.
std::string resolve_generated_by() {
    std::string out;
    if (FILE* p = ::popen("git describe --always --dirty --tags 2>/dev/null", "r")) {
        char buf[256];
        while (std::fgets(buf, sizeof buf, p)) out += buf;
        ::pclose(p);
        while (!out.empty() && (out.back() == '\n' || out.back() == '\r')) out.pop_back();
    }
    return out.empty() ? std::string(AT_GIT_FALLBACK) : out;
}

void print_usage(std::FILE* out) {
    std::fprintf(out,
        "at — arithmetic-topology toolkit\n"
        "usage:\n"
        "  at emit --stage <N> [--out <dir>] [--bound <B>] [--graph-nodes <M>]\n"
        "      --stage        stage to emit (1 supported)\n"
        "      --out          output directory (default viz/data)\n"
        "      --bound        prime bound (default 1000)\n"
        "      --graph-nodes  force-graph vertex cap (default 40)\n"
        "  at --help\n");
}

const char* opt(int argc, char** argv, const char* key, const char* dflt) {
    for (int i = 2; i + 1 < argc; ++i)
        if (std::strcmp(argv[i], key) == 0) return argv[i + 1];
    return dflt;
}
}  // namespace

int main(int argc, char** argv) {
    if (argc < 2 || std::strcmp(argv[1], "--help") == 0 ||
        std::strcmp(argv[1], "-h") == 0) {
        print_usage(stdout);
        return 0;
    }

    if (std::strcmp(argv[1], "emit") == 0) {
        std::string stage = opt(argc, argv, "--stage", "");
        std::string out = opt(argc, argv, "--out", "viz/data");
        // stage-appropriate default bound (stages 3,4 sweep larger discriminants;
        // stage 5's "bound" is the zero-search height t_max).
        const char* bound_default = (stage == "3" || stage == "4") ? "2000000"
                                    : (stage == "5") ? "500" : "1000";
        unsigned long long bound = std::strtoull(opt(argc, argv, "--bound", bound_default), nullptr, 10);
        unsigned long long gnodes = std::strtoull(opt(argc, argv, "--graph-nodes", "40"), nullptr, 10);

        if (stage == "1") {
            at::emit::emit_stage1(out, bound, static_cast<std::size_t>(gnodes),
                                  resolve_generated_by());
            std::fprintf(stderr,
                "at emit: stage 1 -> %s/linking_matrix.json, %s/linking_graph.json "
                "(prime_bound=%llu, graph_nodes=%llu)\n",
                out.c_str(), out.c_str(), bound, gnodes);
            return 0;
        }
        if (stage == "2") {
            at::emit::emit_stage2(out, bound, resolve_generated_by());
            std::fprintf(stderr,
                "at emit: stage 2 -> %s/borromean.json (prime_bound=%llu)\n",
                out.c_str(), bound);
            return 0;
        }
        if (stage == "3") {
            at::emit::emit_stage3(out, static_cast<at::core::i128>(bound),
                                  resolve_generated_by());
            std::fprintf(stderr,
                "at emit: stage 3 -> %s/classgroups.json (disc_bound=%llu)\n",
                out.c_str(), bound);
            return 0;
        }
        if (stage == "4") {
            at::emit::emit_stage4(out, bound, resolve_generated_by());
            std::fprintf(stderr,
                "at emit: stage 4 -> %s/cs_partition.json (disc_bound=%llu)\n",
                out.c_str(), bound);
            return 0;
        }
        if (stage == "5") {
            // --bound here is the zero-search height t_max (default 500).
            double t_max = static_cast<double>(bound);
            const char* odl = opt(argc, argv, "--odlyzko", "data/odlyzko/zeros1");
            at::emit::emit_stage5(out, t_max, odl, resolve_generated_by());
            std::fprintf(stderr,
                "at emit: stage 5 -> %s/{zeros,psi_reconstruction,dyn_zeta}.json "
                "(t_max=%.0f)\n", out.c_str(), t_max);
            return 0;
        }
        if (stage == "6") {
            const char* cache = opt(argc, argv, "--cubic-cache", "data/cubic/s3_counts.txt");
            at::emit::emit_stage6(out, cache, resolve_generated_by());
            std::fprintf(stderr, "at emit: stage 6 -> %s/dw_s3.json\n", out.c_str());
            return 0;
        }
        if (stage == "m1") {
            // Murmurations M1: --ecdata-dir (pinned ecdata slices), --primes (prime
            // count for the panels + collapse curves; default 300 for the snapshot).
            const char* ecdata = opt(argc, argv, "--ecdata-dir", "data/cremona");
            std::size_t nprimes = static_cast<std::size_t>(
                std::strtoull(opt(argc, argv, "--primes", "300"), nullptr, 10));
            at::emit::emit_m1(out, ecdata, nprimes, resolve_generated_by());
            std::fprintf(stderr,
                "at emit: stage m1 -> %s/murmuration_curve.json (n_primes=%zu)\n",
                out.c_str(), nprimes);
            return 0;
        }
        if (stage == "m2") {
            // Dirichlet-character murmurations: zero external data (§9 clean case).
            at::emit::emit_m2(out, resolve_generated_by());
            std::fprintf(stderr, "at emit: stage m2 -> %s/dirichlet_murmuration.json\n", out.c_str());
            return 0;
        }
        if (stage == "m3") {
            // Zubrilina murmuration: empirical trace-formula average vs M₂(y) density,
            // teeth at n²/4. Zero external data (§9 repo-reproducible default).
            at::emit::emit_m3(out, resolve_generated_by());
            std::fprintf(stderr, "at emit: stage m3 -> %s/zubrilina_murmuration.json\n", out.c_str());
            return 0;
        }
        if (stage == "m4") {
            // Sawin–Sutherland height murmuration: the COMPUTED empirical statistic (1)
            // overlaid on the CONJECTURED density D(u). Consumes the committed run
            // data/m4/ss_empirical.txt (the ~19-min statistic is never re-run here).
            const char* run_path = opt(argc, argv, "--ss-run", "data/m4/ss_empirical.txt");
            at::emit::emit_m4(out, run_path, resolve_generated_by());
            std::fprintf(stderr, "at emit: stage m4 -> %s/sawin_sutherland_murmuration.json\n", out.c_str());
            return 0;
        }
        if (stage == "m5") {
            // PR-1 X-extension: the four-rung ladder {10⁴,2¹⁶,2¹⁷,2¹⁸} + the finite-range
            // H0 verdict in claim_class. run_path is the 2¹⁸ VERDICT rung; the emitter reads
            // its 2¹⁶/2¹⁷ siblings from the same directory to assemble the ladder (no a_p is
            // re-run here — each run file reaggregates from its committed partials).
            const char* run_path = opt(argc, argv, "--ss-run", "data/m5/ss_x262144.txt");
            at::emit::emit_m5_extension(out, run_path, resolve_generated_by());
            std::fprintf(stderr, "at emit: stage m5 -> %s/ss_x_extension_murmuration.json\n", out.c_str());
            return 0;
        }
        if (stage == "m5split") {
            // PR-2 step 3: the analytic-rank split overlay (full, F∖S₂, S₂, S₀ + D(u) +
            // targets + gates + branch). Re-aggregates the committed partials filtered by the
            // rank column — no a_p recompute — so it is cheap and byte-portable (freshness).
            const char* partials = opt(argc, argv, "--partials", "data/m5/ss_partials_x65536.txt");
            const char* rankc = opt(argc, argv, "--rank-cache", "data/m5/rank_cache_x65536.txt");
            at::emit::emit_m5_rank_split(out, partials, rankc, resolve_generated_by());
            std::fprintf(stderr, "at emit: stage m5split -> %s/ss_rank_split_murmuration.json\n", out.c_str());
            return 0;
        }
        std::fprintf(stderr,
            "at emit: no emitter for stage '%s' (stages 1-6, m1-m3 are built)\n",
            stage.c_str());
        return 2;
    }

    if (std::strcmp(argv[1], "ecdata-extract") == 0) {
        // Regenerate the committed derived extract from the pinned RAW ecdata slices
        // (R2). Run once; the extract is committed and the loader prefers it.
        const char* ecdata = opt(argc, argv, "--ecdata-dir", "data/cremona");
        unsigned long long lo = std::strtoull(opt(argc, argv, "--lo", "2500"), nullptr, 10);
        unsigned long long hi = std::strtoull(opt(argc, argv, "--hi", "10000"), nullptr, 10);
        std::string outf = opt(argc, argv, "--out", "data/cremona/m1_extract.txt");
        auto curves = at::ell::load_ecdata_raw(ecdata, lo, hi);
        at::ell::write_extract(outf, curves, lo, hi);
        std::fprintf(stderr, "at ecdata-extract: %zu classes [%llu,%llu] -> %s\n",
                     curves.size(), lo, hi, outf.c_str());
        return 0;
    }

    if (std::strcmp(argv[1], "ne-cache") == 0) {
        // Generate the committed N/ε cache for the height family {H(E) ≤ X}
        // (m4-pinning §P4/C3). The ONE place the CLI calls PARI: N and ε are
        // oracle-provenance INPUT data. Run once per X; the cache is committed and
        // the statistic/emit read it (repo-reproducible without live PARI).
        i64 X = static_cast<i64>(std::strtoull(opt(argc, argv, "--X", "10000"), nullptr, 10));
        std::string outf = opt(argc, argv, "--out", "data/m4/ne_cache.txt");

        std::optional<std::string> gp = oracle::find_gp();
        if (!gp) {
            std::fprintf(stderr, "at ne-cache: PARI/GP `gp` not found on PATH (required to fill N/ε).\n");
            return 3;
        }
        std::vector<at::murm::HeightCurve> fam = at::murm::height_family(X);
        std::fprintf(stderr, "at ne-cache: X=%lld, %zu curves; querying PARI for N, ε...\n",
                     static_cast<long long>(X), fam.size());

        // Batch: one line per curve, `N eps`. ellglobalred[1] is the conductor;
        // ellrootno is the global root number (the ε in the statistic's weighting).
        std::ostringstream script;
        script << "print(version);\n";
        // Large families (X ≥ 2^18 ⇒ ~16k separate ellinit/ellglobalred statements) overflow
        // gp's fixed 8 MB stack (default parisizemax=0, no auto-grow); 9014 curves at 2^17 fit,
        // 15936 at 2^18 do not. Let the stack grow as needed — harmless at small X (allocates
        // nothing up front), and emits no stdout so the version provenance line stays line 1.
        script << "default(parisizemax,1<<31);\n";
        for (const at::murm::HeightCurve& e : fam)
            script << "E=ellinit([0,0,0," << e.A << "," << e.B
                   << "]);print(ellglobalred(E)[1],\" \",ellrootno(E))\n";
        const std::string out = oracle::run_gp(*gp, script.str());

        std::istringstream lines(out);
        std::string vline;
        std::getline(lines, vline);   // the version(...) vector, kept as provenance

        std::vector<at::murm::NeRow> rows;
        rows.reserve(fam.size());
        std::string line;
        for (const at::murm::HeightCurve& e : fam) {
            if (!std::getline(lines, line)) {
                std::fprintf(stderr, "at ne-cache: PARI output ended early (curve A=%lld B=%lld)\n",
                             static_cast<long long>(e.A), static_cast<long long>(e.B));
                return 4;
            }
            long long N = 0, eps = 0;
            std::istringstream(line) >> N >> eps;
            if (eps != 1 && eps != -1) {
                std::fprintf(stderr, "at ne-cache: PARI ε not ±1 for A=%lld B=%lld: '%s'\n",
                             static_cast<long long>(e.A), static_cast<long long>(e.B), line.c_str());
                return 4;
            }
            rows.push_back({e.A, e.B, static_cast<i64>(N), static_cast<int>(eps)});
        }

        at::murm::NeCacheHeader h;
        h.generator_hash = at::murm::ne_generator_hash();
        h.pari_version = vline;
        h.X = X;
        h.count = static_cast<i64>(rows.size());
        at::murm::write_ne_cache(outf, h, rows);
        std::fprintf(stderr, "at ne-cache: wrote %zu rows -> %s (provenance: oracle on N, ε)\n",
                     rows.size(), outf.c_str());
        return 0;
    }

    if (std::strcmp(argv[1], "ss-run") == 0) {
        // THE murmuration confirmation run (m4-pinning step 3): the empirical statistic
        // (1) over the height family, a_p COMPUTED from scratch, N/ε from the committed
        // oracle cache. Heavy (~minutes at 10⁴, ~12 h at 2¹⁶); run once, the result is
        // committed and consumed cheaply by `at emit` + the confirmation/prereg tests.
        // The committed DESIGN (τ, R2 targets, Δu) is fixed in m4-pinning §R0 and PR-1 —
        // pre-registered, not tuned here. --X selects the confirmation scale: 10⁴ (M4,
        // default) or an X-extension rung (PR-1: 65536, ...).
        //
        // PR-1 R2/R3 (X-extension rungs only, via --partials/--checkpoint): the per-curve
        // partials are persisted keyed by (A,B) so PR-2 re-aggregates subpopulations with
        // NO a_p recompute; a chunked checkpoint gives crash-safety, is byte-identical to
        // a single pass at fixed thread count (twin_ss_partials_chunked_vs_single), and is
        // NEVER inspected mid-run — the shape is extracted exactly once, at completion.
        i64 X = static_cast<i64>(std::strtoull(opt(argc, argv, "--X", "10000"), nullptr, 10));
        const char* cache = opt(argc, argv, "--cache", "data/m4/ne_cache.txt");
        std::string outf = opt(argc, argv, "--out", "data/m4/ss_empirical.txt");
        std::string partials_path = opt(argc, argv, "--partials", "");     // "" ⇒ don't persist (M4)
        std::string default_ckpt = partials_path.empty() ? "" : partials_path + ".ckpt";
        std::string ckpt_path = opt(argc, argv, "--checkpoint", default_ckpt.c_str());
        std::size_t chunk = static_cast<std::size_t>(std::strtoull(
            opt(argc, argv, "--chunk", "512"), nullptr, 10));
        int threads = static_cast<int>(std::strtoull(
            opt(argc, argv, "--threads", "0"), nullptr, 10));
        if (threads <= 0) {
            unsigned hc = std::thread::hardware_concurrency();
            threads = hc ? static_cast<int>(hc) : 4;
        }
        // a_p provider (m0b-pinning §7, production wiring gate): fast (default, ap_fast O(p))
        // or m0b (ap_shanks_mestre O(p^{1/4})). The two produce byte-identical partials — same
        // iteration/accumulation order, a_p equal integers — so --ap=m0b must reproduce the
        // committed artifacts EXACTLY. Default unchanged until the gate + PRODUCTION-CAPABLE.
        const std::string ap_mode = opt(argc, argv, "--ap", "fast");
        const bool use_m0b = (ap_mode == "m0b");
        if (ap_mode != "fast" && !use_m0b) {
            std::fprintf(stderr, "at ss-run: --ap must be 'fast' or 'm0b' (got '%s')\n", ap_mode.c_str());
            return 2;
        }
        std::fprintf(stderr, "at ss-run: a_p provider = %s\n",
                     use_m0b ? "m0b (ap_shanks_mestre, O(p^1/4))" : "fast (ap_fast, O(p))");

        at::murm::NeCacheHeader hdr;
        std::vector<at::murm::NeRow> rows = at::murm::read_ne_cache(cache, hdr);
        if (hdr.X < X) {
            std::fprintf(stderr, "at ss-run: cache X=%lld < requested --X %lld "
                         "(regenerate the N/ε cache at the larger scale)\n",
                         static_cast<long long>(hdr.X), static_cast<long long>(X));
            return 4;
        }
        const std::size_t C = rows.size();
        std::fprintf(stderr, "at ss-run: %zu curves from %s (cache X=%lld); confirm X=%lld; "
                     "%d threads; computing a_p...\n", C, cache,
                     static_cast<long long>(hdr.X), static_cast<long long>(X), threads);

        at::murm::SSRun run;
        run.generator_hash = at::murm::ss_generator_hash();
        run.ap_provider = use_m0b ? "m0b" : "fast";       // R1: provider provenance
        if (use_m0b) run.provider_hash = at::murm::m0b_provider_hash();
        run.du = 0.025;                                   // committed bin width (§R0)
        run.tol = 0.06;                                   // a-priori empirical tolerance (§R0c)
        run.r2_hump = 0.475; run.r2_zero = 0.645; run.r2_trough = 0.805;  // committed R2 (formula side)
        run.X_confirm = static_cast<double>(X);           // pre-named confirmation scale (§R0b / PR-1)
        // Ladder = the M4 peek-untouched rungs that are ≤ X, plus X itself. For X=10⁴ this
        // is exactly {4000,6000,8000,10000} (M4). For an extension rung the ≤10⁴ points are
        // FREE consistency re-derivations (same curves, same order ⇒ same M4 shape).
        for (double m4x : {4000.0, 6000.0, 8000.0, 10000.0})
            if (m4x <= run.X_confirm) run.ladder.push_back(m4x);
        if (run.ladder.empty() || run.ladder.back() != run.X_confirm)
            run.ladder.push_back(run.X_confirm);

        const bool do_checkpoint = !partials_path.empty() && chunk > 0 && !ckpt_path.empty();
        const auto t_start = std::chrono::steady_clock::now();
        auto elapsed_s = [&]() {
            return std::chrono::duration_cast<std::chrono::seconds>(
                       std::chrono::steady_clock::now() - t_start).count();
        };

        at::murm::SSPartials P;
        bool loaded = false;

        // Fast path (PR-2 reaggregation / clean re-emit): if a COMPLETE committed
        // partials file already holds this run's a_p work, load it and skip the heavy
        // compute entirely. read_ss_partials refuses a stale hash or an incomplete
        // (checkpoint) file, so this never silently reuses the wrong numbers.
        if (!partials_path.empty()) {
            std::ifstream probe(partials_path);
            if (probe.good()) {
                probe.close();
                try {
                    at::murm::SSPartialsMeta pm;
                    at::murm::SSPartials Pl =
                        at::murm::read_ss_partials(partials_path, pm, /*allow_incomplete=*/false);
                    const int NBexp = static_cast<int>(std::lround(1.0 / run.du));
                    if (pm.generator_hash == std::string(at::murm::ss_generator_hash()) &&
                        pm.X == run.X_confirm && pm.du == run.du && pm.NB == NBexp &&
                        Pl.A.size() == C) {
                        P = std::move(Pl);
                        loaded = true;
                        std::fprintf(stderr, "at ss-run: loaded COMPLETE partials %s (%zu curves) "
                                     "— reaggregating, NO a_p recompute\n", partials_path.c_str(), C);
                    }
                } catch (const std::exception&) { /* unusable → fall through to compute */ }
            }
        }

        if (loaded) {
            /* P already populated from the committed partials */
        } else if (!do_checkpoint) {
            // Single-pass (M4 path; also the fast small-X path). Nothing persisted.
            P = use_m0b ? at::murm::ss_empirical_partials_m0b(rows, run.du, threads)
                        : at::murm::ss_empirical_partials(rows, run.du, threads);
        } else {
            // ---- chunked pass with crash-safety checkpoint (PR-1 R3) ----
            const int NB = static_cast<int>(std::lround(1.0 / run.du));
            P.du = run.du; P.NB = NB;
            P.A.resize(C); P.B.resize(C); P.N.resize(C);
            P.num.assign(C, std::vector<double>(NB, 0.0));
            P.cnt.assign(C, std::vector<i64>(NB, 0));
            std::map<std::pair<i64, i64>, std::size_t> where;
            for (std::size_t c = 0; c < C; ++c) {
                P.A[c] = rows[c].A; P.B[c] = rows[c].B; P.N[c] = rows[c].N;
                where[{rows[c].A, rows[c].B}] = c;
            }
            std::vector<char> done(C, 0);
            std::size_t n_done = 0;

            // Resume: load completed (A,B) curves from a compatible checkpoint.
            {
                std::ifstream probe(ckpt_path);
                if (probe.good()) {
                    probe.close();
                    try {
                        at::murm::SSPartialsMeta cm;
                        at::murm::SSPartials Pc =
                            at::murm::read_ss_partials(ckpt_path, cm, /*allow_incomplete=*/true);
                        const bool compat =
                            cm.generator_hash == std::string(at::murm::ss_generator_hash()) &&
                            cm.X == run.X_confirm && cm.du == run.du &&
                            cm.NB == NB && cm.threads == threads;
                        if (compat) {
                            for (std::size_t j = 0; j < Pc.A.size(); ++j) {
                                auto it = where.find({Pc.A[j], Pc.B[j]});
                                if (it == where.end()) continue;   // stale row; ignore
                                const std::size_t c = it->second;
                                if (done[c]) continue;
                                P.num[c] = Pc.num[j]; P.cnt[c] = Pc.cnt[j];
                                done[c] = 1; ++n_done;
                            }
                            std::fprintf(stderr, "at ss-run: RESUMED %zu/%zu curves from checkpoint %s\n",
                                         n_done, C, ckpt_path.c_str());
                        } else {
                            std::fprintf(stderr, "at ss-run: checkpoint %s incompatible "
                                         "(hash/X/du/NB/threads); ignoring, fresh start\n",
                                         ckpt_path.c_str());
                        }
                    } catch (const std::exception& e) {
                        std::fprintf(stderr, "at ss-run: checkpoint %s unreadable (%s); fresh start\n",
                                     ckpt_path.c_str(), e.what());
                    }
                }
            }

            at::murm::SSPartialsMeta meta;
            meta.generator_hash = at::murm::ss_generator_hash();
            meta.ap_provider = use_m0b ? "m0b" : "fast";       // R1: provider provenance
            if (use_m0b) meta.provider_hash = at::murm::m0b_provider_hash();
            meta.X = run.X_confirm; meta.du = run.du; meta.NB = NB;
            meta.threads = threads; meta.ne_cache = cache;

            // Process CHEAPEST-FIRST (conductor N ascending). The earliest checkpoint then
            // lands within minutes (reaper-resilience: bank progress before a reap), and each
            // sorted chunk builds QR tables only up to its own tight maxN (lower overhead than
            // canonical order, where most chunks hit the ~4M-conductor tail). Byte-identity is
            // unaffected: a curve's partials are independent of its chunk-mates at fixed thread
            // count, and ss_aggregate sums P in CANONICAL order regardless of compute order.
            std::vector<std::size_t> order(C);
            std::iota(order.begin(), order.end(), std::size_t{0});
            std::sort(order.begin(), order.end(),
                      [&](std::size_t a, std::size_t b) { return rows[a].N < rows[b].N; });

            for (std::size_t start = 0; start < C; start += chunk) {
                std::vector<at::murm::NeRow> crows;
                std::vector<std::size_t> orig;
                const std::size_t stop = std::min(start + chunk, C);
                for (std::size_t k = start; k < stop; ++k) {
                    const std::size_t c = order[k];
                    if (!done[c]) { crows.push_back(rows[c]); orig.push_back(c); }
                }
                if (!crows.empty()) {
                    at::murm::SSPartials cp =
                        use_m0b ? at::murm::ss_empirical_partials_m0b(crows, run.du, threads)
                                : at::murm::ss_empirical_partials(crows, run.du, threads);
                    for (std::size_t j = 0; j < orig.size(); ++j) {
                        const std::size_t c = orig[j];
                        P.num[c] = cp.num[j]; P.cnt[c] = cp.cnt[j];
                        done[c] = 1;
                    }
                    n_done += crows.size();

                    // Overwrite the checkpoint with ALL done curves (complete=0), atomically
                    // via tmp+rename so a crash mid-write leaves the previous good file. This
                    // dump carries a_p work only — NO shape is computed here (R3 no-peeking).
                    at::murm::SSPartials Pdone;
                    Pdone.du = run.du; Pdone.NB = NB;
                    for (std::size_t c = 0; c < C; ++c)
                        if (done[c]) {
                            Pdone.A.push_back(P.A[c]); Pdone.B.push_back(P.B[c]); Pdone.N.push_back(P.N[c]);
                            Pdone.num.push_back(P.num[c]); Pdone.cnt.push_back(P.cnt[c]);
                        }
                    at::murm::SSPartialsMeta cmeta = meta;
                    cmeta.complete = false;
                    cmeta.n_curves = static_cast<i64>(Pdone.A.size());
                    const std::string tmp = ckpt_path + ".tmp";
                    at::murm::write_ss_partials(tmp, Pdone, cmeta);
                    std::rename(tmp.c_str(), ckpt_path.c_str());
                }
                std::fprintf(stderr, "  checkpoint: %zu/%zu curves done  (%llds elapsed)\n",
                             n_done, C, static_cast<long long>(elapsed_s()));
            }

            // Final partials: complete=1, all curves in canonical order. Committed artifact.
            meta.complete = true;
            meta.n_curves = static_cast<i64>(C);
            at::murm::write_ss_partials(partials_path, P, meta);
            std::remove(ckpt_path.c_str());
            std::fprintf(stderr, "at ss-run: wrote %s (%zu curves, %llds a_p) — partials for PR-2\n",
                         partials_path.c_str(), C, static_cast<long long>(elapsed_s()));
        }

        // ---- aggregate ONCE, at completion, by the committed extractor ----
        for (double Xl : run.ladder) {
            at::murm::SSEmpirical e = at::murm::ss_aggregate(P, Xl);
            run.shapes.push_back({Xl, e.n_curves, e.shape});
            if (Xl == run.X_confirm) run.confirm = e;
            std::fprintf(stderr,
                "  X=%.0f  |fam|=%lld  hump=%.4f zero=%.6f trough=%.4f\n",
                Xl, static_cast<long long>(e.n_curves), e.shape.hump_u, e.shape.zero_u, e.shape.trough_u);
        }
        at::murm::write_ss_run(outf, run);
        std::fprintf(stderr, "at ss-run: wrote %s (X_confirm=%.0f, τ=%.2f, %llds total)\n",
                     outf.c_str(), run.X_confirm, run.tol, static_cast<long long>(elapsed_s()));
        return 0;
    }

    if (std::strcmp(argv[1], "ap-cache") == 0) {
        // ---- M0b reference a_p cache generator (Phase 2b), v2 (prime-major) ---------------
        // FROZEN-REFEREE-ONLY (P2 firewall): a_p via ell::ap_fast, spot-checked against the
        // frozen ell::ap_charsum; NO ap_shanks_mestre path here, ever. Grid = the statistic's
        // good-prime domain (ss_empirical.cpp:64–68): {p prime : 3 < p ≤ N(E), p ∤ disc},
        // disc = 4A³+27B². [R3] the factored object is 4A³+27B², NOT the model discriminant
        // −16(4A³+27B²); the factor of 2 differs only at p=2, outside the p>3 grid, so the
        // bad-prime SET on the grid is identical either way.
        // PRIME-MAJOR with the p ≤ N(E) skip (mirrors ss_empirical — no wasted compute; the
        // v1 ap_fast_grid path had NO skip and was fatally slow), QR built once per prime. Raw
        // a_p placed by write-index idx(c,p) = (π(p−1)−2) − #{bad primes of c below p}; disc ≤
        // 2X so bad primes factor trivially. Slots sentinel-filled and REQUIRED all written
        // [R2a]. int16 (|a_p| ≤ 2√p), stored CONDUCTOR-ASC (N, then ne-row index). Resumable by
        // PRIME-BLOCK (R1: complete/n_done; on a checkpoint n_done = primes done). Grid
        // DEFINITION packed into curve_set (P1); twins REQUIRE its equality before any value.
        i64 X = static_cast<i64>(std::strtoull(opt(argc, argv, "--X", "65536"), nullptr, 10));
        const char* cache = opt(argc, argv, "--cache", "data/m5/ne_cache_x65536.txt");
        std::string outf = opt(argc, argv, "--out", "");
        if (outf.empty()) outf = "data/m5/ap_cache_x" + std::to_string(X) + ".bin";
        std::string ckpt_path = opt(argc, argv, "--checkpoint", (outf + ".ckpt").c_str());
        std::size_t block = static_cast<std::size_t>(
            std::strtoull(opt(argc, argv, "--chunk", "8192"), nullptr, 10));   // primes per checkpoint block
        if (block == 0) block = 8192;
        int threads = static_cast<int>(std::strtoull(opt(argc, argv, "--threads", "0"), nullptr, 10));
        if (threads <= 0) { unsigned hc = std::thread::hardware_concurrency(); threads = hc ? static_cast<int>(hc) : 4; }
        const bool do_spot = std::strcmp(opt(argc, argv, "--spot-check", "1"), "0") != 0;

        at::murm::NeCacheHeader nehdr;
        std::vector<at::murm::NeRow> allrows = at::murm::read_ne_cache(cache, nehdr);
        if (nehdr.X < X) {
            std::fprintf(stderr, "at ap-cache: cache X=%lld < requested --X %lld\n",
                         static_cast<long long>(nehdr.X), static_cast<long long>(X));
            return 4;
        }
        std::vector<at::murm::NeRow> rows;
        for (const auto& r : allrows)
            if (at::murm::naive_height(r.A, r.B) <= X) rows.push_back(r);
        const std::size_t C = rows.size();
        if (C == 0) { std::fprintf(stderr, "at ap-cache: empty family\n"); return 4; }

        std::vector<i64> disc(C);
        i64 maxN = 0;
        for (std::size_t c = 0; c < C; ++c) {
            disc[c] = 4 * rows[c].A * rows[c].A * rows[c].A + 27 * rows[c].B * rows[c].B;
            if (rows[c].N > maxN) maxN = rows[c].N;
        }
        // deterministic storage order: N ascending, ties by ne-row index
        std::vector<std::size_t> order(C);
        std::iota(order.begin(), order.end(), std::size_t{0});
        std::sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b) {
            if (rows[a].N != rows[b].N) return rows[a].N < rows[b].N;
            return a < b;
        });
        // global ascending primes in (3, maxN] + prefix count pref[x] = #primes ≤ x
        std::vector<i64> gp;
        std::vector<i64> pref(static_cast<std::size_t>(maxN) + 1, 0);
        {
            std::vector<char> sieve(static_cast<std::size_t>(maxN) + 1, 1);
            i64 running = 0;
            for (i64 i = 2; i <= maxN; ++i) {
                if (sieve[i]) {
                    ++running;
                    if (i > 3) gp.push_back(i);
                    for (i64 j = i * i; j <= maxN; j += i) sieve[j] = 0;
                }
                pref[i] = running;
            }
        }
        auto pi_le = [&](i64 x) -> i64 { return x < 2 ? 0 : pref[x > maxN ? maxN : x]; };  // #primes ≤ x
        // bad primes of each curve = prime factors (>3) of disc = 4A³+27B² [R3], ascending.
        // disc ≤ 2X ⇒ trial division to √disc is trivial.
        std::vector<std::vector<i64>> badp(C);
        for (std::size_t c = 0; c < C; ++c) {
            i64 d = disc[c] < 0 ? -disc[c] : disc[c];
            for (i64 f = 2; f * f <= d; ++f)
                if (d % f == 0) { while (d % f == 0) d /= f; if (f > 3) badp[c].push_back(f); }
            if (d > 3) badp[c].push_back(d);
            std::sort(badp[c].begin(), badp[c].end());
        }
        auto good_count = [&](std::size_t c) -> std::size_t {   // #good primes ≤ N_c
            i64 total = pi_le(rows[c].N) - 2;                   // primes in (3, N_c]
            i64 bad = 0;
            for (i64 q : badp[c]) { if (q <= rows[c].N) ++bad; else break; }
            return static_cast<std::size_t>(total - bad);
        };
        auto idx_of = [&](std::size_t c, i64 p) -> std::size_t {  // slot of good prime p in curve c
            i64 below = pi_le(p - 1) - 2;                        // #primes in (3, p)
            i64 bad = 0;
            for (i64 q : badp[c]) { if (q < p) ++bad; else break; }
            return static_cast<std::size_t>(below - bad);
        };
        // grid DEFINITION descriptor (P1) — twins REQUIRE equality of this before any value
        const std::string grid_desc =
            "family=height(X=" + std::to_string(X) + ",fam=" + std::to_string(C) + ")"
            "|order=conductor-asc(N,idx)|prime_rule=3<p<=N,p_ndiv_(4A^3+27B^2)"
            "|ne=" + std::string(cache) + ",ne_gen=" + nehdr.generator_hash +
            ",ne_X=" + std::to_string(nehdr.X) + ",ne_count=" + std::to_string(nehdr.count) +
            "|algo=charsum_referee(ap_fast;spot=ap_charsum)";

        constexpr int16_t SENT = -32768;   // impossible a_p (|a_p| ≤ 2√maxN ≪ 32768): unwritten slot
        std::vector<std::vector<int16_t>> vals(C);
        for (std::size_t c = 0; c < C; ++c) vals[c].assign(good_count(c), SENT);
        std::size_t n_primes_done = 0;      // resume cursor: leading primes of gp fully processed
        const auto t_start = std::chrono::steady_clock::now();
        auto elapsed_s = [&]() {
            return std::chrono::duration_cast<std::chrono::seconds>(
                       std::chrono::steady_clock::now() - t_start).count();
        };

        // Flat = concat vals[order[i]] over ALL curves (prime-major fills every curve in
        // lockstep, so a checkpoint always carries all curves — sentinels in not-yet-done
        // prime slots). COMPLETE: n_done = C (curve count — the finished reference all
        // consumers read). PARTIAL checkpoint: n_done = primes done (the resume cursor). The
        // complete flag disambiguates; consumers read COMPLETE only (R1).
        auto write_cache = [&](bool complete) {
            at::ell::ApCacheHeader h;
            h.generator_hash = at::ell::ap_generator_hash();
            h.prime_bound = static_cast<uint64_t>(maxN);
            h.curve_set = grid_desc;
            h.complete = complete;
            h.n_done = complete ? static_cast<uint64_t>(C)
                                : static_cast<uint64_t>(n_primes_done);
            std::vector<int16_t> flat;
            for (std::size_t i = 0; i < C; ++i) {
                const std::vector<int16_t>& v = vals[order[i]];
                flat.insert(flat.end(), v.begin(), v.end());
            }
            if (complete) {
                at::ell::write_ap_cache(outf, h, flat);
            } else {
                at::ell::write_ap_cache(ckpt_path + ".tmp", h, flat);
                std::rename((ckpt_path + ".tmp").c_str(), ckpt_path.c_str());
            }
        };

        // Resume from a compatible PARTIAL checkpoint — the ONLY allow_incomplete read (R1).
        {
            std::ifstream probe(ckpt_path);
            if (probe.good()) {
                probe.close();
                try {
                    at::ell::ApCacheHeader ch;
                    std::vector<int16_t> cv =
                        at::ell::read_ap_cache(ckpt_path, ch, /*allow_incomplete=*/true);
                    std::size_t expect = 0;
                    for (std::size_t c = 0; c < C; ++c) expect += vals[c].size();
                    if (ch.curve_set == grid_desc && !ch.complete &&
                        ch.n_done <= gp.size() && cv.size() == expect) {
                        std::size_t off = 0;
                        for (std::size_t i = 0; i < C; ++i) {
                            const std::size_t c = order[i];
                            const std::size_t k = vals[c].size();
                            std::copy(cv.begin() + off, cv.begin() + off + k, vals[c].begin());
                            off += k;
                        }
                        n_primes_done = static_cast<std::size_t>(ch.n_done);
                        std::fprintf(stderr, "at ap-cache: RESUMED %zu/%zu primes from %s\n",
                                     n_primes_done, gp.size(), ckpt_path.c_str());
                    } else {
                        std::fprintf(stderr, "at ap-cache: checkpoint incompatible; fresh start\n");
                    }
                } catch (const std::exception& e) {
                    std::fprintf(stderr, "at ap-cache: checkpoint unreadable (%s); fresh start\n",
                                 e.what());
                }
            }
        }

        std::fprintf(stderr, "at ap-cache: %zu curves (X=%lld), maxN=%lld, %zu primes, %d threads; "
                     "prime-major a_p (frozen referee ap_fast)...\n", C, static_cast<long long>(X),
                     static_cast<long long>(maxN), gp.size(), threads);

        for (std::size_t pstart = n_primes_done; pstart < gp.size(); pstart += block) {
            const std::size_t pstop = std::min(pstart + block, gp.size());
            // Prime-major, primes interleaved across threads; each (c,p) writes a DISTINCT slot
            // vals[c][idx_of(c,p)] (distinct primes ⇒ distinct slots ⇒ no data race on vals[c]).
            auto worker = [&](int t) {
                for (std::size_t pj = pstart + static_cast<std::size_t>(t); pj < pstop;
                     pj += static_cast<std::size_t>(threads)) {
                    const i64 p = gp[pj];
                    const at::ell::QRTable qr = at::ell::build_qr_table(static_cast<at::core::u64>(p));
                    for (std::size_t c = 0; c < C; ++c) {
                        if (rows[c].N < p) continue;         // p ≤ N(E) skip (mirrors ss_empirical:67)
                        if (disc[c] % p == 0) continue;      // bad reduction (p | disc)
                        const at::ell::Curve E{0, 0, 0, rows[c].A, rows[c].B};
                        vals[c][idx_of(c, p)] = static_cast<int16_t>(at::ell::ap_fast(E, qr));
                    }
                }
            };
            std::vector<std::thread> pool;
            for (int t = 1; t < threads; ++t) pool.emplace_back(worker, t);
            worker(0);
            for (std::thread& th : pool) th.join();
            n_primes_done = pstop;

            // Spot-check: the block's last prime on the highest-conductor curve that uses it,
            // vs the frozen ap_charsum (P2). Abort on any disagreement.
            if (do_spot && pstop > pstart) {
                const i64 p = gp[pstop - 1];
                for (std::size_t ii = C; ii-- > 0;) {          // highest-N curve first
                    const std::size_t c = order[ii];
                    if (rows[c].N < p || disc[c] % p == 0) continue;
                    const at::ell::Curve E{0, 0, 0, rows[c].A, rows[c].B};
                    const int ref = at::ell::ap_charsum(E, static_cast<at::core::u64>(p));
                    const int got = static_cast<int>(vals[c][idx_of(c, p)]);
                    if (ref != got) {
                        std::fprintf(stderr, "at ap-cache: SPOT-CHECK FAIL A=%lld B=%lld p=%lld: "
                                     "ap_fast=%d != ap_charsum=%d — ABORT\n",
                                     static_cast<long long>(rows[c].A), static_cast<long long>(rows[c].B),
                                     static_cast<long long>(p), got, ref);
                        return 5;
                    }
                    break;
                }
            }

            write_cache(false);
            std::fprintf(stderr, "  ap-cache checkpoint: %zu/%zu primes (p≤%lld)  (%llds elapsed)\n",
                         n_primes_done, gp.size(),
                         static_cast<long long>(gp[pstop - 1]), static_cast<long long>(elapsed_s()));
        }

        // [R2a] slot-count identity: every slot must be written exactly once — a remaining
        // sentinel means a missed/collided write-index. REQUIRED for every curve.
        for (std::size_t c = 0; c < C; ++c)
            for (std::size_t s = 0; s < vals[c].size(); ++s)
                if (vals[c][s] == SENT) {
                    std::fprintf(stderr, "at ap-cache: SLOT-COUNT IDENTITY FAIL — curve A=%lld B=%lld "
                                 "slot %zu/%zu unwritten (write-index bug) — ABORT\n",
                                 static_cast<long long>(rows[c].A), static_cast<long long>(rows[c].B),
                                 s, vals[c].size());
                    return 6;
                }

        write_cache(true);              // COMPLETE reference cache
        std::remove(ckpt_path.c_str());
        std::size_t total_vals = 0;
        for (std::size_t c = 0; c < C; ++c) total_vals += vals[c].size();
        std::fprintf(stderr, "at ap-cache: wrote %s (%zu curves, %zu a_p values, %.1f MB, %llds) "
                     "[P3: record size+wall-time]\n", outf.c_str(), C, total_vals,
                     static_cast<double>(total_vals * sizeof(int16_t)) / 1e6,
                     static_cast<long long>(elapsed_s()));
        return 0;
    }

    if (std::strcmp(argv[1], "ap-sample") == 0) {
        // libm-diff quantity 1 (data-note §5, sampled fallback): a DETERMINISTIC, tail-weighted
        // cross-platform integer a_p sample. Computes a_p via the frozen referee ap_charsum for a
        // fixed sample and prints "A B p a_p" (deterministic order). Run on laptop AND FreeBSD, then
        // diff stdout. No RNG, no seed. Spec: docs/notes/libm-partial-diff-spec.md.
        i64 X = static_cast<i64>(std::strtoull(opt(argc, argv, "--X", "131072"), nullptr, 10));
        const char* cache = opt(argc, argv, "--cache", "data/m5/ne_cache_x131072.txt");
        at::murm::NeCacheHeader nehdr;
        std::vector<at::murm::NeRow> allrows = at::murm::read_ne_cache(cache, nehdr);
        if (nehdr.X < X) {
            std::fprintf(stderr, "at ap-sample: cache X=%lld < --X %lld\n",
                         static_cast<long long>(nehdr.X), static_cast<long long>(X));
            return 4;
        }
        std::vector<at::murm::NeRow> rows;
        for (const auto& r : allrows)
            if (at::murm::naive_height(r.A, r.B) <= X) rows.push_back(r);
        const std::size_t C = rows.size();
        if (C == 0) { std::fprintf(stderr, "at ap-sample: empty family\n"); return 4; }
        std::vector<i64> disc(C);
        i64 maxN = 0;
        for (std::size_t c = 0; c < C; ++c) {
            disc[c] = 4 * rows[c].A * rows[c].A * rows[c].A + 27 * rows[c].B * rows[c].B;
            if (rows[c].N > maxN) maxN = rows[c].N;
        }
        std::vector<std::size_t> order(C);   // conductor-asc (N, then ne-row idx) — highest N last
        std::iota(order.begin(), order.end(), std::size_t{0});
        std::sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b) {
            if (rows[a].N != rows[b].N) return rows[a].N < rows[b].N;
            return a < b;
        });
        std::vector<i64> gp;                 // good primes in (3, maxN]
        {
            std::vector<char> sieve(static_cast<std::size_t>(maxN) + 1, 1);
            for (i64 i = 2; i <= maxN; ++i)
                if (sieve[i]) {
                    if (i > 3) gp.push_back(i);
                    for (i64 j = i * i; j <= maxN; j += i) sieve[j] = 0;
                }
        }
        // tail-weighted prime sample: 4 index-quartiles, counts 300/500/700/1000 (ratio 3:5:7:10),
        // fixed stride within each quartile (deterministic).
        const std::size_t Pn = gp.size();
        const std::size_t qsz = Pn / 4;
        const int want[4] = {300, 500, 700, 1000};
        std::vector<i64> sample_primes;
        for (int q = 0; q < 4; ++q) {
            const std::size_t lo = static_cast<std::size_t>(q) * qsz;
            const std::size_t hi = (q == 3) ? Pn : lo + qsz;   // last quartile absorbs remainder
            const std::size_t span = hi - lo;
            for (int k = 0; k < want[q]; ++k) {
                std::size_t idx = lo + (static_cast<std::size_t>(k) * span) / static_cast<std::size_t>(want[q]);
                if (idx >= hi) idx = hi - 1;
                sample_primes.push_back(gp[idx]);
            }
        }
        // per sampled prime: the 40 highest-conductor curves with N >= p and p ∤ disc.
        // Build the deterministic work-list serially, compute a_p in parallel into fixed slots,
        // then print in work-list order — output bytes are identical regardless of thread count
        // (so laptop and FreeBSD are diffable).
        struct Item { i64 A, B, p; };
        std::vector<Item> work;
        i64 maxp = 0;
        for (i64 p : sample_primes) {
            int taken = 0;
            for (std::size_t ii = C; ii-- > 0 && taken < 40;) {
                const std::size_t c = order[ii];             // highest N first
                if (rows[c].N < p) continue;
                if (disc[c] % p == 0) continue;
                work.push_back({rows[c].A, rows[c].B, p});
                ++taken;
                if (p > maxp) maxp = p;
            }
        }
        int threads = static_cast<int>(std::strtoull(opt(argc, argv, "--threads", "0"), nullptr, 10));
        if (threads <= 0) { unsigned hc = std::thread::hardware_concurrency(); threads = hc ? static_cast<int>(hc) : 4; }
        std::vector<int> aps(work.size(), 0);
        auto worker = [&](int t) {
            for (std::size_t i = static_cast<std::size_t>(t); i < work.size();
                 i += static_cast<std::size_t>(threads)) {
                const at::ell::Curve E{0, 0, 0, work[i].A, work[i].B};
                aps[i] = at::ell::ap_charsum(E, static_cast<at::core::u64>(work[i].p));
            }
        };
        std::vector<std::thread> pool;
        for (int t = 1; t < threads; ++t) pool.emplace_back(worker, t);
        worker(0);
        for (std::thread& th : pool) th.join();
        for (std::size_t i = 0; i < work.size(); ++i)
            std::printf("%lld %lld %lld %d\n", static_cast<long long>(work[i].A),
                        static_cast<long long>(work[i].B), static_cast<long long>(work[i].p), aps[i]);
        std::fprintf(stderr, "at ap-sample: X=%lld, %zu curves, %zu sampled primes, %zu pairs, "
                     "max p=%lld, %d threads (ap_charsum, deterministic tail-weighted)\n",
                     static_cast<long long>(X), C, sample_primes.size(), work.size(),
                     static_cast<long long>(maxp), threads);
        return 0;
    }

    if (std::strcmp(argv[1], "rank-cache") == 0) {
        // PR-2 step 1: the ANALYTIC-RANK column for the height family, keyed by (A,B)
        // (PR-2 Amendment 1). r = ord_{s=1} L(E,s) from PARI ellanalyticrank; oracle-
        // provenance (numerically determined), NEVER the Mordell–Weil rank. The ONE new
        // place the CLI calls PARI for PR-2. Reads the committed N/ε cache for the (A,B)
        // keys AND ε, so the MANDATORY parity cross-check (analytic-rank parity == ε
        // parity; ε=(−1)^r is a theorem) runs over ALL curves — any mismatch ABORTS and
        // is a deliverable. Prints f₂ (the analytic-rank-2 fraction) — the pre-declared
        // look (PR-2 §Looks). Run once; the cache is committed and re-aggregation reads it.
        const char* cache = opt(argc, argv, "--cache", "data/m5/ne_cache_x65536.txt");
        std::string outf = opt(argc, argv, "--out", "data/m5/rank_cache_x65536.txt");
        std::string prec = opt(argc, argv, "--prec", "38");

        std::optional<std::string> gp = oracle::find_gp();
        if (!gp) {
            std::fprintf(stderr, "at rank-cache: PARI/GP `gp` not found on PATH (required for ellanalyticrank).\n");
            return 3;
        }
        at::murm::NeCacheHeader hdr;
        std::vector<at::murm::NeRow> rows = at::murm::read_ne_cache(cache, hdr);
        std::fprintf(stderr, "at rank-cache: %zu curves from %s (X=%lld); querying PARI "
                     "ellanalyticrank (prec %s)...\n", rows.size(), cache,
                     static_cast<long long>(hdr.X), prec.c_str());

        std::ostringstream script;
        script << "default(realprecision, " << prec << ");\n";
        script << "print(version);\n";
        for (const at::murm::NeRow& e : rows)
            script << "print(ellanalyticrank(ellinit([0,0,0," << e.A << "," << e.B << "]))[1])\n";
        const std::string out = oracle::run_gp(*gp, script.str());

        std::istringstream lines(out);
        std::string vline;
        std::getline(lines, vline);   // version(...) vector, kept as provenance

        std::vector<at::murm::RankRow> rrows;
        rrows.reserve(rows.size());
        std::string line;
        for (const at::murm::NeRow& e : rows) {
            if (!std::getline(lines, line)) {
                std::fprintf(stderr, "at rank-cache: PARI output ended early (A=%lld B=%lld)\n",
                             static_cast<long long>(e.A), static_cast<long long>(e.B));
                return 4;
            }
            long long r = -1;
            std::istringstream(line) >> r;
            if (r < 0) {
                std::fprintf(stderr, "at rank-cache: bad rank '%s' for A=%lld B=%lld\n",
                             line.c_str(), static_cast<long long>(e.A), static_cast<long long>(e.B));
                return 4;
            }
            rrows.push_back({e.A, e.B, static_cast<int>(r)});
        }

        // MANDATORY parity cross-check over ALL curves: ε = (−1)^r (a theorem). A
        // mismatch is a numerical mis-determination of the analytic rank → ABORT.
        long long mism = 0;
        for (std::size_t i = 0; i < rows.size(); ++i) {
            const bool r_even = (rrows[i].rank % 2 == 0);
            const bool eps_plus = (rows[i].eps == 1);
            if (r_even != eps_plus) {
                if (mism < 20)
                    std::fprintf(stderr, "  PARITY MISMATCH A=%lld B=%lld rank=%d eps=%d\n",
                                 static_cast<long long>(rrows[i].A),
                                 static_cast<long long>(rrows[i].B), rrows[i].rank, rows[i].eps);
                ++mism;
            }
        }
        if (mism > 0) {
            std::fprintf(stderr, "at rank-cache: ABORT — %lld analytic-rank/ε parity mismatches "
                         "of %zu (deliverable; increase --prec or investigate)\n", mism, rows.size());
            return 5;
        }

        // Rank distribution + f₂ (the pre-declared look).
        int maxr = 0;
        for (const at::murm::RankRow& r : rrows) if (r.rank > maxr) maxr = r.rank;
        std::vector<long long> dist(static_cast<std::size_t>(maxr) + 1, 0);
        for (const at::murm::RankRow& r : rrows) ++dist[static_cast<std::size_t>(r.rank)];
        const double f2 = rrows.empty() ? 0.0
            : static_cast<double>(dist.size() > 2 ? dist[2] : 0) / static_cast<double>(rrows.size());

        at::murm::RankCacheHeader rh;
        rh.generator_hash = at::murm::rank_generator_hash();
        rh.pari_version = vline;
        rh.prec = prec;
        rh.X = hdr.X;
        rh.count = static_cast<i64>(rrows.size());
        at::murm::write_rank_cache(outf, rh, rrows);

        std::fprintf(stderr, "at rank-cache: wrote %zu rows -> %s (provenance: oracle-analytic-rank)\n",
                     rrows.size(), outf.c_str());
        std::fprintf(stderr, "at rank-cache: parity cross-check PASSED (0 mismatches over %zu)\n",
                     rows.size());
        std::fprintf(stderr, "at rank-cache: analytic-rank distribution:");
        for (std::size_t r = 0; r < dist.size(); ++r)
            std::fprintf(stderr, "  r%zu=%lld", r, dist[r]);
        std::fprintf(stderr, "\nat rank-cache: f2 = %lld/%zu = %.6f  (the pre-declared look)\n",
                     dist.size() > 2 ? dist[2] : 0, rrows.size(), f2);
        return 0;
    }

    if (std::strcmp(argv[1], "rank-split") == 0) {
        // PR-2 step 3: the analytic-rank split. Re-aggregates the committed partials,
        // FILTERED by the committed rank column, through the frozen ss_aggregate — NO a_p
        // recompute. Applies the committed gates MECHANICALLY (R4) and prints the branch;
        // interpretation is written afterwards, in the doc.
        const char* partials_path = opt(argc, argv, "--partials", "data/m5/ss_partials_x65536.txt");
        const char* rank_path = opt(argc, argv, "--rank-cache", "data/m5/rank_cache_x65536.txt");
        // Committed thresholds (pre-registered): PR-2 Amendment 4 + step 2, m4-pinning §R0c.
        const double target_trough = 0.805, tol = 0.06, contrast_threshold = 0.668;
        const double u_lo = 0.7, u_hi = 0.9;

        at::murm::SSPartialsMeta pm;
        at::murm::SSPartials P = at::murm::read_ss_partials(partials_path, pm, false);
        at::murm::RankCacheHeader rh;
        std::vector<at::murm::RankRow> rr = at::murm::read_rank_cache(rank_path, rh);

        // Align the analytic rank to P.A order by (A,B).
        std::map<std::pair<i64, i64>, int> rank_of;
        for (const at::murm::RankRow& r : rr) rank_of[{r.A, r.B}] = r.rank;
        std::vector<int> rank(P.A.size(), -1);
        i64 missing = 0;
        for (std::size_t c = 0; c < P.A.size(); ++c) {
            auto it = rank_of.find({P.A[c], P.B[c]});
            if (it == rank_of.end()) { ++missing; continue; }
            rank[c] = it->second;
        }
        if (missing) {
            std::fprintf(stderr, "at rank-split: %lld partials curves have no rank-cache entry "
                         "(caches mismatched)\n", static_cast<long long>(missing));
            return 4;
        }

        const at::murm::RankSplit s = at::murm::rank_split(
            P, rank, pm.X, target_trough, tol, contrast_threshold, u_lo, u_hi);

        std::fprintf(stderr,
            "at rank-split (X=%.0f): committed gates applied mechanically (R4)\n", s.X);
        std::fprintf(stderr,
            "  full     |fam|=%lld  trough_u=%.4f  dev=%.4f\n",
            static_cast<long long>(s.n_full), s.full.shape.trough_u, s.dev_full);
        std::fprintf(stderr,
            "  F\\S2     |fam|=%lld  trough_u=%.4f  dev=%.4f   (PRIMARY, u-space)\n",
            static_cast<long long>(s.n_leaveout), s.leaveout.shape.trough_u, s.dev_leaveout);
        std::fprintf(stderr,
            "  S2       |fam|=%lld  trough_u=%.4f  window[%.2f,%.2f] mean=%.4f\n",
            static_cast<long long>(s.n_s2), s.s2.shape.trough_u, u_lo, u_hi, s.mean_s2);
        std::fprintf(stderr,
            "  S0       |fam|=%lld  trough_u=%.4f  window mean=%.4f\n",
            static_cast<long long>(s.n_s0), s.s0.shape.trough_u, s.mean_s0);
        std::fprintf(stderr,
            "  PRIMARY  recovery gate (dev_leaveout <= dev_full-du=%.4f AND <= tau=%.2f): %s\n",
            s.dev_full - s.du, tol, s.recovers ? "RECOVERS" : "does NOT recover");
        std::fprintf(stderr,
            "  SECONDARY contrast gap (mean_S2 - mean_S0)=%.4f vs threshold -%.3f (downward): %s\n",
            s.gap, contrast_threshold, s.contrast_downward ? "SIGNIFICANT" : "inconclusive");
        std::fprintf(stderr, "  BRANCH primary  : %s\n", s.primary_branch.c_str());
        std::fprintf(stderr, "  BRANCH secondary: %s\n", s.secondary_branch.c_str());

        // Pre-registered annulus cross-check (PR-2 Amendment 3, committed): the same
        // leave-out on the geometric holdout (10⁴, 2¹⁶] alone — curves never touched by any
        // exploration — to show the primary verdict is not an artifact of the ≤10⁴ seed set.
        // Not a new split (R2-permitted): same test, holdout subset.
        at::murm::SSPartials Pa;
        Pa.du = P.du; Pa.NB = P.NB;
        std::vector<int> rank_a;
        for (std::size_t c = 0; c < P.A.size(); ++c) {
            if (at::murm::naive_height(P.A[c], P.B[c]) <= 10000) continue;   // annulus: H>10⁴
            Pa.A.push_back(P.A[c]); Pa.B.push_back(P.B[c]); Pa.N.push_back(P.N[c]);
            Pa.num.push_back(P.num[c]); Pa.cnt.push_back(P.cnt[c]);
            rank_a.push_back(rank[c]);
        }
        const at::murm::RankSplit sa = at::murm::rank_split(
            Pa, rank_a, pm.X, target_trough, tol, contrast_threshold, u_lo, u_hi);
        std::fprintf(stderr,
            "  ANNULUS (10^4,2^16] cross-check: |fam|=%lld  full trough_u=%.4f  "
            "F\\S2 trough_u=%.4f (dev %.4f) -> %s ; contrast gap=%.4f -> %s\n",
            static_cast<long long>(sa.n_full), sa.full.shape.trough_u,
            sa.leaveout.shape.trough_u, sa.dev_leaveout,
            sa.recovers ? "RECOVERS" : "does NOT recover", sa.gap,
            sa.contrast_downward ? "downward-sig" : "inconclusive");
        return 0;
    }

    if (std::strcmp(argv[1], "ss-leakage") == 0) {
        // PR-3 D2 — root-number-imbalance leakage (docs/preregistered/PR-3.md §"D2
        // operationalization", pinned 2026-07-17, commit 21060a0). READ-ONLY: reads the
        // committed partials + ne_cache and recomputes a_p for NOTHING. All quantities in
        // emitted density units. delta=(n+−n−)/|fam|; S=signed density; U=unsigned density
        // (ε divided out: since ε=±1, num/ε = ε·num); D=ss_density (conjectured, X-indep);
        // departure Δ=S−D; leakage L=delta·U; f=L/Δ at the trough bin b*. Fits nothing.
        const char* part_path = opt(argc, argv, "--partials", "");
        const char* ne_path = opt(argc, argv, "--ne", "");
        const char* check_run = opt(argc, argv, "--check-run", "");  // committed ss_x*.txt for the S twin
        if (!*part_path || !*ne_path) {
            std::fprintf(stderr, "usage: at ss-leakage --partials <ss_partials_x*.txt> "
                         "--ne <ne_cache_x*.txt> [--ladder \"4000,6000,...\"] [--check-run <ss_x*.txt>]\n");
            return 2;
        }
        std::vector<double> ladder;
        {
            std::string ls = opt(argc, argv, "--ladder", "4000,6000,8000,10000,65536,131072");
            std::stringstream ss(ls);
            std::string tok;
            while (std::getline(ss, tok, ','))
                if (!tok.empty()) ladder.push_back(std::strtod(tok.c_str(), nullptr));
        }

        at::murm::NeCacheHeader nehdr;
        std::vector<at::murm::NeRow> ne = at::murm::read_ne_cache(ne_path, nehdr);
        std::map<std::pair<at::core::i64, at::core::i64>, int> eps_of;
        for (const auto& r : ne) eps_of[{r.A, r.B}] = r.eps;

        at::murm::SSPartialsMeta pmeta;
        at::murm::SSPartials P = at::murm::read_ss_partials(part_path, pmeta);  // throws if incomplete (R3)
        const int NB = P.NB;
        const double du = P.du;
        const std::size_t C = P.A.size();

        std::vector<int> eps(C);
        for (std::size_t c = 0; c < C; ++c) {
            auto it = eps_of.find({P.A[c], P.B[c]});
            if (it == eps_of.end()) {
                std::fprintf(stderr, "at ss-leakage: curve (%lld,%lld) absent from ne_cache — "
                             "mismatched provenance\n", (long long)P.A[c], (long long)P.B[c]);
                return 4;
            }
            eps[c] = it->second;
        }

        const long long kDensB = 2000;  // same truncation the emitter uses (emit_sawin_sutherland.cpp)
        std::vector<double> Dc(NB), umid(NB);
        for (int b = 0; b < NB; ++b) {
            umid[b] = (b + 0.5) * du;
            Dc[b] = at::murm::ss_density(umid[b], kDensB, kDensB);
        }

        // Cumulative aggregation at a height cutoff Xr → (S, U, delta, |fam|, n±, trough bin).
        struct Rung {
            double X; at::core::i64 nfam, np, nm; double delta;
            std::vector<double> S, U; int bstar; double trough_u, zero_u;
        };
        auto aggregate = [&](double Xr, bool cumulative, double Xlo) -> Rung {
            const at::core::i64 Xi = (at::core::i64)Xr, Xloi = (at::core::i64)Xlo;
            std::vector<double> accS(NB, 0.0), accU(NB, 0.0);
            at::core::i64 nfam = 0, np = 0, nm = 0;
            for (std::size_t c = 0; c < C; ++c) {
                const at::core::i64 h = at::murm::naive_height(P.A[c], P.B[c]);
                if (h > Xi) continue;
                if (!cumulative && h <= Xloi) continue;  // annulus shell (Xlo, Xr]
                nfam++;
                if (eps[c] == 1) np++; else nm++;
                for (int b = 0; b < NB; ++b) { accS[b] += P.num[c][b]; accU[b] += eps[c] * P.num[c][b]; }
            }
            Rung r{Xr, nfam, np, nm, 0.0, std::vector<double>(NB, 0.0), std::vector<double>(NB, 0.0), 0, 0, 0};
            if (nfam == 0) return r;
            r.delta = (double)(np - nm) / (double)nfam;
            const double denom = du * (double)nfam;
            for (int b = 0; b < NB; ++b) { r.S[b] = accS[b] / denom; r.U[b] = accU[b] / denom; }
            const at::murm::SSShape sh = at::murm::extract_shape(umid, r.S);
            r.trough_u = sh.trough_u; r.zero_u = sh.zero_u;
            int bstar = (int)std::lround(sh.trough_u / du - 0.5);
            r.bstar = bstar < 0 ? 0 : (bstar >= NB ? NB - 1 : bstar);
            return r;
        };

        std::fprintf(stderr, "at ss-leakage: PR-3 D2 — partials %s (%zu curves, NB=%d, du=%.4f); "
                     "ne %s; D=ss_density(kDensB=%lld)\n", part_path, C, NB, du, ne_path, kDensB);

        // Optional free consistency twin: my S at X_confirm == committed ss_x density column.
        if (*check_run) {
            at::murm::SSRun run = at::murm::read_ss_run(check_run);
            Rung rc = aggregate(run.X_confirm, true, 0);
            double maxdev = 0;
            for (int b = 0; b < NB && b < (int)run.confirm.density.size(); ++b)
                maxdev = std::max(maxdev, std::fabs(rc.S[b] - run.confirm.density[b]));
            // The committed run prints density at default ostream precision (6 sig figs,
            // ss_run_io.cpp), so the floor is ~5e-6 for magnitudes ~few; a real curve-order
            // or ε-join bug would be O(density)~O(1). 1e-3 cleanly separates the two.
            const double kSTwinTol = 1e-3;
            std::fprintf(stderr, "  S-twin vs %s (X=%.0f): max|ΔS|=%.3e (print floor ~5e-6)  -> %s\n",
                         check_run, run.X_confirm, maxdev, maxdev < kSTwinTol ? "OK" : "MISMATCH");
            if (maxdev >= kSTwinTol) { std::fprintf(stderr, "  aborting: S aggregation does not reproduce the committed run\n"); return 5; }
        }

        std::printf("# PR-3 D2 leakage table (docs/preregistered/PR-3.md). Densities in emitted units.\n");
        std::printf("# CUMULATIVE (nested; H<=X). f = L(u*)/departure(u*) at trough bin u*.\n");
        std::printf("# X n_curves n+ n- delta trough_u S* U* L*=d.U* D* dep*=S*-D* f sign\n");
        std::vector<Rung> rungs;
        for (double Xr : ladder) {
            Rung r = aggregate(Xr, true, 0);
            if (r.nfam == 0) continue;
            rungs.push_back(r);
            const int b = r.bstar;
            const double L = r.delta * r.U[b], dep = r.S[b] - Dc[b], f = dep != 0 ? L / dep : 0.0;
            const bool same = dep != 0 && L != 0 && ((L < 0) == (dep < 0));
            std::printf("%.0f %lld %lld %lld %.6f %.4f  %.5f %.5f %.6f  %.5f %.6f  %.4f %s\n",
                        Xr, (long long)r.nfam, (long long)r.np, (long long)r.nm, r.delta, r.trough_u,
                        r.S[b], r.U[b], L, Dc[b], dep, f, same ? "same" : "OPP");
        }

        std::printf("# ANNULUS increments (quasi-independent shells H in (X_{k-1},X_k]).\n");
        std::printf("# shell n_curves n+ n- delta S* U* L*=d.U* dep*=S*-D* f sign  (u* fixed = top-rung trough)\n");
        const int bref = rungs.empty() ? 0 : rungs.back().bstar;
        double Xprev = 0;
        for (double Xr : ladder) {
            Rung r = aggregate(Xr, false, Xprev);
            if (r.nfam > 0) {
                const int b = bref;
                const double L = r.delta * r.U[b], dep = r.S[b] - Dc[b], f = dep != 0 ? L / dep : 0.0;
                const bool same = dep != 0 && L != 0 && ((L < 0) == (dep < 0));
                std::printf("(%.0f,%.0f] %lld %lld %lld %.6f  %.5f %.5f %.6f  %.6f  %.4f %s\n",
                            Xprev, Xr, (long long)r.nfam, (long long)r.np, (long long)r.nm, r.delta,
                            r.S[b], r.U[b], L, dep, f, same ? "same" : "OPP");
            }
            Xprev = Xr;
        }

        // Descending-branch profile for the top rung (u > zero_u): L(u) vs departure(u).
        if (!rungs.empty()) {
            const Rung& r = rungs.back();
            std::printf("# DESCENDING-BRANCH profile, top rung X=%.0f (u>zero_u=%.4f): L(u)=d.U(u) vs dep(u)=S-D\n", r.X, r.zero_u);
            std::printf("# u S U D dep=S-D L=d.U f=L/dep\n");
            for (int b = 0; b < NB; ++b) {
                if (umid[b] <= r.zero_u) continue;
                const double L = r.delta * r.U[b], dep = r.S[b] - Dc[b], f = dep != 0 ? L / dep : 0.0;
                std::printf("%.4f  %.5f %.5f %.5f  %.6f %.6f  %.4f\n", umid[b], r.S[b], r.U[b], Dc[b], dep, L, f);
            }
        }
        return 0;
    }

    std::fprintf(stderr, "at: unknown command '%s'\n\n", argv[1]);
    print_usage(stderr);
    return 2;
}
