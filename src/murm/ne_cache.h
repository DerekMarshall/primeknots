#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "core/int_types.h"

// Stage M4 — the conductor / root-number cache for the height-ordered family
// (m4-pinning §P4/C3; RESEARCH-M §2 "oracle-provenance INPUT DATA" amendment).
//
// N(E) and ε(E) are OBTAINED FROM AN ORACLE (PARI/GP), not computed from scratch:
// a live oracle supplying trusted INPUT to the statistic, not refereeing our
// numbers. The murmuration quantity a_p stays COMPUTED (ell::ap_charsum). This
// header/reader is oracle-FREE (rule 3): it only parses the committed text cache
// written by the generator `at ne-cache` (which is the one place PARI is called,
// in app/, never in src/). Residual risk: a systematic oracle error in N/ε would
// propagate silently — mitigated by the dual-oracle overlap twin
// (verify/m4/oracle_dual_overlap_NE, 5676 Cremona classes exact) run BEFORE this
// cache is consumed.
//
// Freshness (ERRATA #18 principle, ap_cache pattern): the loader REFUSES a stale
// cache. The `generator_hash` is the sha256 of murm/height_family.cpp — the family
// enumeration that fixes the cache's (A,B) key set; if that source changes the key
// set can change, so a mismatch throws rather than silently reuse. The cache is a
// COMMITTED text artifact (unlike the gitignored a_p cache), so the emit is
// repo-reproducible without live PARI.
namespace at::murm {

using at::core::i64;

constexpr uint32_t kNeCacheFormatVersion = 1;

// One height-family curve E_{A,B}: y² = x³ + Ax + B, with its oracle-supplied
// conductor and root number.
struct NeRow {
    i64 A = 0, B = 0;     // reduced, nonsingular short model (computed-provenance key)
    i64 N = 0;            // conductor N(E)        — provenance: oracle (PARI)
    int eps = 0;          // root number ε(E) ∈ {−1,+1} — provenance: oracle (PARI)
};

struct NeCacheHeader {
    uint32_t format_version = kNeCacheFormatVersion;
    std::string generator_hash;   // sha256 of murm/height_family.cpp (the key generator)
    std::string pari_version;     // PARI/GP version string (provenance, not a gate)
    i64 X = 0;                    // height cutoff H(E) ≤ X
    i64 count = 0;                // number of rows (== |height_family(X)|)
};

// The compiled-in generator hash (CMake sha256 of murm/height_family.cpp). A cache
// is valid only if header.generator_hash equals this.
const char* ne_generator_hash();

// Write the committed text cache (provenance header + `A B N eps` rows).
void write_ne_cache(const std::string& path, const NeCacheHeader& header,
                    const std::vector<NeRow>& rows);

// Parse the cache into out_header and return the rows. THROWS std::runtime_error
// if the magic/format is wrong, if the row count disagrees with the header, or —
// the refusal that matters — if header.generator_hash != ne_generator_hash()
// (a stale cache is never silently reused). Oracle-FREE.
std::vector<NeRow> read_ne_cache(const std::string& path, NeCacheHeader& out_header);

}  // namespace at::murm
