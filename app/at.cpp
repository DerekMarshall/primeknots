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

#include "emit/emit_borromean.h"
#include "emit/emit_classgroups.h"
#include "emit/emit_cs.h"
#include "emit/emit_dw.h"
#include "emit/emit_linking.h"
#include "emit/emit_murmuration.h"
#include "emit/emit_zeta.h"

#ifndef AT_GIT_FALLBACK
#define AT_GIT_FALLBACK "unknown"
#endif

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
        std::fprintf(stderr,
            "at emit: no emitter for stage '%s' (stages 1-6, m1 are built)\n",
            stage.c_str());
        return 2;
    }

    std::fprintf(stderr, "at: unknown command '%s'\n\n", argv[1]);
    print_usage(stderr);
    return 2;
}
