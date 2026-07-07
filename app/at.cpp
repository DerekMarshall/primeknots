// at — the arithmetic-topology toolkit CLI.
//
// Stage 0 stub. The `emit` subcommand is the entry point for the JSON emitters
// (ARCHITECTURE.md §5), but those are authored per stage alongside the data
// they produce. Stage 0 emits no visualization data, so `emit` is a no-op that
// says so. See docs/notes/open-questions.md R2 for why this lives in app/.

#include <cstdio>
#include <cstring>
#include <string>

namespace {
void print_usage(std::FILE* out) {
    std::fprintf(out,
        "at — arithmetic-topology toolkit\n"
        "usage:\n"
        "  at emit --stage <N> --out <dir>   emit visualization JSON for stage N\n"
        "  at --help                         show this message\n");
}
}  // namespace

int main(int argc, char** argv) {
    if (argc < 2 || std::strcmp(argv[1], "--help") == 0 ||
        std::strcmp(argv[1], "-h") == 0) {
        print_usage(stdout);
        return 0;
    }

    if (std::strcmp(argv[1], "emit") == 0) {
        // Accept and echo the arguments so the CLI contract is exercised, but
        // there is nothing to emit yet.
        std::string stage = "?";
        for (int i = 2; i + 1 < argc; ++i) {
            if (std::strcmp(argv[i], "--stage") == 0) stage = argv[i + 1];
        }
        std::fprintf(stderr,
            "at emit: no emitters are wired yet (requested stage %s). Stage 0 "
            "produces no visualization data; emitters arrive with their "
            "stages.\n",
            stage.c_str());
        return 0;
    }

    std::fprintf(stderr, "at: unknown command '%s'\n\n", argv[1]);
    print_usage(stderr);
    return 2;
}
