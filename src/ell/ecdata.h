#pragma once
#include <map>
#include <string>
#include <vector>

#include "core/int_types.h"
#include "ell/curve.h"

// Stage M1 — reader for the pinned Cremona ecdata slices (data/cremona/, fetched +
// sha256-validated by oracle/fetch_ecdata.py). Everything here is oracle-provenance
// EXCEPT the good-prime a_p our code computes from the model. Bad-prime a_q is
// derived from ecdata's Atkin–Lehner sign via the pinned conversion (m0-pinning §3):
// AP25 supplies signs for bad p < 100, BQ for bad q > 100.
namespace at::ell {

using at::core::u64;

// One isogeny class (its NUM==1 representative). a_p is an isogeny invariant, so the
// class is the natural unit (RESEARCH-M §2 / HLOP §3: one representative per class).
struct EcCurve {
    u64 N = 0;                    // conductor (oracle)
    std::string label;            // e.g. "11a" (oracle)
    Curve model{};                // reduced minimal Weierstrass model (oracle) — drives computed a_p
    int rank = 0;                 // (oracle)
    std::map<u64, int> bad_w;     // bad prime p | N -> Atkin–Lehner sign w_p ∈ {+1,-1} (oracle)
};

// Load isogeny-class representatives with conductor in [N1, N2]. Prefers the
// committed derived extract `<dir>/m1_extract.txt` (repo-reproducible, R2); falls
// back to parsing the raw allcurves.*/aplist.* slices when the extract is absent.
// Calls ell::assert_minimal per curve (C1). Throws if a curve in range has no
// aplist row / model fails minimality / the extract does not cover [N1,N2].
std::vector<EcCurve> load_ecdata_range(const std::string& dir, u64 N1, u64 N2);

// Always parse the raw allcurves.*/aplist.* slices (ignores any committed extract).
// Used to (re)generate the extract from the pinned raw data.
std::vector<EcCurve> load_ecdata_raw(const std::string& dir, u64 N1, u64 N2);

// Write the derived extract (exactly the rows M1 consumes) covering [lo,hi], so
// the pipeline is reproducible from the repo without the gitignored raw slices.
// A "Modified Version" of ecdata under the Artistic License 2.0 — sha-linked to
// the pinned release in data/cremona/MANIFEST.json (R2 / m1-pinning §0).
void write_extract(const std::string& path, const std::vector<EcCurve>& curves,
                   u64 lo, u64 hi);

// The extract filename the loader prefers under an ecdata dir.
extern const char* kM1ExtractName;

// The 25 primes < 100, in AP25 order (index i ↔ i-th prime).
extern const u64 kAP25Primes[25];

}  // namespace at::ell
