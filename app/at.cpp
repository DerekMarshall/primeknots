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
            // PR-1 X-extension: the 2¹⁶ rung + full ladder, same schema, Reading-B verdict
            // in claim_class. Consumes the committed extension run data/m5/ss_x65536.txt
            // (the ~10 h a_p is never re-run here; the run file reaggregates from partials).
            const char* run_path = opt(argc, argv, "--ss-run", "data/m5/ss_x65536.txt");
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
            P = at::murm::ss_empirical_partials(rows, run.du, threads);
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
                        at::murm::ss_empirical_partials(crows, run.du, threads);
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

    std::fprintf(stderr, "at: unknown command '%s'\n\n", argv[1]);
    print_usage(stderr);
    return 2;
}
