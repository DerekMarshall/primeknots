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

// Load isogeny-class representatives with conductor in [N1, N2] from all
// allcurves.*/aplist.* files under `dir`. Calls ell::assert_minimal per curve
// (C1 precondition). Throws std::runtime_error if a curve in range has no aplist
// row, or if a model fails minimality. The first 25 primes (< 100) map to AP25.
std::vector<EcCurve> load_ecdata_range(const std::string& dir, u64 N1, u64 N2);

// The 25 primes < 100, in AP25 order (index i ↔ i-th prime).
extern const u64 kAP25Primes[25];

}  // namespace at::ell
