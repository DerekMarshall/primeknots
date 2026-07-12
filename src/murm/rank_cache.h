#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "core/int_types.h"

// Stage M5 / PR-2 — the ANALYTIC-RANK column for the height family, keyed by (A,B)
// (PR-2 Amendment 1/2; RESEARCH-M §2 "oracle-analytic-rank" sub-class).
//
//   r(E) = ord_{s=1} L(E,s)   — the ANALYTIC rank (order of vanishing), from PARI
//   `ellanalyticrank` at a recorded precision. Provenance: ORACLE, analytic rank — a
//   NUMERICALLY-determined sub-class, distinct from the exact, unconditional N and ε
//   (data/m5/ne_cache_x65536.txt). It is NEVER the Mordell–Weil rank (that equality is
//   BSD and is never asserted anywhere in this project).
//
// This header/reader is oracle-FREE (rule 3): it only parses the committed text cache
// written by the generator `at rank-cache` (the one place PARI is called, in app/).
// generator_hash = sha256(murm/height_family.cpp) — the (A,B) key generator, the SAME
// guard as ne_cache: if the family enumeration changes, the key set can change, so a
// mismatch throws rather than silently reuse. Committed text artifact (repo-reproducible
// without live PARI), so PR-2's re-aggregation runs without recomputing anything.
//
// SAFEGUARD (checked by the generator AND the prereg test): the analytic-rank PARITY
// must equal the cached ε PARITY for EVERY curve — ε = (−1)^{ord_{s=1} L} is the sign of
// the functional equation, a THEOREM, so a parity mismatch is a numerical
// mis-determination of the rank, not a conjecture. Any nonzero mismatch count ABORTS the
// run and is itself a deliverable.
namespace at::murm {

using at::core::i64;

constexpr uint32_t kRankCacheFormatVersion = 1;

// One curve's analytic rank (order of vanishing of L(E,s) at s=1).
struct RankRow {
    i64 A = 0, B = 0;   // reduced, nonsingular short model (computed-provenance key)
    int rank = -1;      // analytic rank  — provenance: oracle (PARI ellanalyticrank)
};

struct RankCacheHeader {
    uint32_t format_version = kRankCacheFormatVersion;
    std::string generator_hash;   // sha256 of murm/height_family.cpp (the key generator)
    std::string pari_version;     // PARI/GP version string (provenance, not a gate)
    std::string prec;             // PARI precision used for ellanalyticrank (provenance)
    i64 X = 0;                    // height cutoff H(E) ≤ X
    i64 count = 0;                // number of rows (== |height_family(X)|)
};

// The compiled-in generator hash (CMake sha256 of murm/height_family.cpp). Same key
// generator as ne_cache, so a rank cache is valid only against the matching (A,B) set.
const char* rank_generator_hash();

// Write the committed text cache (provenance header + `A B rank` rows).
void write_rank_cache(const std::string& path, const RankCacheHeader& header,
                      const std::vector<RankRow>& rows);

// Parse the cache into out_header and return the rows. THROWS std::runtime_error on
// magic/format mismatch, row-count disagreement, or — the refusal that matters —
// header.generator_hash != rank_generator_hash() (a stale cache is never reused).
// Oracle-FREE.
std::vector<RankRow> read_rank_cache(const std::string& path, RankCacheHeader& out_header);

}  // namespace at::murm
