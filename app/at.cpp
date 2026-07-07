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

#include "emit/emit_linking.h"

namespace {
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
        unsigned long long bound = std::strtoull(opt(argc, argv, "--bound", "1000"), nullptr, 10);
        unsigned long long gnodes = std::strtoull(opt(argc, argv, "--graph-nodes", "40"), nullptr, 10);

        if (stage == "1") {
            at::emit::emit_stage1(out, bound, static_cast<std::size_t>(gnodes));
            std::fprintf(stderr,
                "at emit: stage 1 -> %s/linking_matrix.json, %s/linking_graph.json "
                "(prime_bound=%llu, graph_nodes=%llu)\n",
                out.c_str(), out.c_str(), bound, gnodes);
            return 0;
        }
        std::fprintf(stderr,
            "at emit: no emitter for stage '%s' (only stage 1 is built)\n",
            stage.c_str());
        return 2;
    }

    std::fprintf(stderr, "at: unknown command '%s'\n\n", argv[1]);
    print_usage(stderr);
    return 2;
}
