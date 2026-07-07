#pragma once

// Shared state for the verification binaries. See docs/notes/open-questions.md
// R4 for the clean-SKIP exit-code contract these back.
namespace at::verify {

// Set by an oracle test when its external referee (e.g. gp) is unavailable and
// the test skips instead of asserting. The custom doctest main maps a clean
// skip (no failures) to exit code 77 so ctest reports SKIP, never a silent pass.
extern bool g_oracle_skipped;

// Set when `--extended` is passed; suites scale their ranges up for overnight
// sweeps (ARCHITECTURE.md §3). Default runs use the fixed in-suite ranges.
extern bool g_extended;

}  // namespace at::verify
