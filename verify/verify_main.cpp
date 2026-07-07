// Custom doctest main shared by every verify_stageN binary.
//
// It exists (rather than DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN) to enforce the
// oracle clean-SKIP contract from docs/notes/open-questions.md R4:
//
//   any assertion failure         -> exit 1   (never confused with skip)
//   clean skip, no failures       -> exit 77  (ctest SKIP_RETURN_CODE)
//   all pass                      -> exit 0
//
// It also strips the project-specific `--extended` flag before handing the rest
// of argv to doctest, so doctest does not choke on an option it doesn't know.

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include <cstring>
#include <vector>

#include "harness.h"

namespace at::verify {
bool g_oracle_skipped = false;
bool g_extended = false;
}  // namespace at::verify

int main(int argc, char** argv) {
    std::vector<char*> forwarded;
    forwarded.reserve(argc);
    for (int i = 0; i < argc; ++i) {
        if (std::strcmp(argv[i], "--extended") == 0) {
            at::verify::g_extended = true;
        } else {
            forwarded.push_back(argv[i]);
        }
    }

    doctest::Context ctx;
    ctx.applyCommandLine(static_cast<int>(forwarded.size()), forwarded.data());

    int failed = ctx.run();
    if (ctx.shouldExit()) return failed;   // --help, --list-*, --version, etc.
    if (failed != 0) return 1;             // a real failure is never a skip
    if (at::verify::g_oracle_skipped) return 77;
    return 0;
}
