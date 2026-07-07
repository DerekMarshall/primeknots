#pragma once
#include <algorithm>
#include <vector>

#include "core/int_types.h"
#include "core/primes.h"
#include "harness.h"
#include "linking/linking_matrix.h"

// Stage 3 test helpers. Phase 1 uses the convention-free forms layer only.
namespace at::verify::stage3 {

using at::core::i128;
using at::core::u64;

inline u64 pool_bound() { return at::verify::g_extended ? 200ULL : 100ULL; }
inline i128 disc_bound() { return at::verify::g_extended ? 2'000'000 : 200'000; }
inline i128 expected_disc_count() {
    return at::verify::g_extended ? 1394 : 208;  // pinned (computed independently)
}

// Our restricted family: squarefree products of 1..3 distinct primes ≡ 1 (mod 4)
// (t = 1,2,3) with D ≤ disc_bound. Deterministic, ascending, deduplicated.
inline std::vector<i128> family_discriminants() {
    std::vector<u64> P;
    for (u64 n = 5; n <= pool_bound(); n += 4)
        if (at::core::is_prime(n)) P.push_back(n);
    std::vector<i128> D;
    const i128 B = disc_bound();
    for (std::size_t i = 0; i < P.size(); ++i) {
        if ((i128)P[i] <= B) D.push_back(P[i]);
        for (std::size_t j = i + 1; j < P.size(); ++j) {
            i128 d2 = (i128)P[i] * P[j];
            if (d2 <= B) D.push_back(d2);
            for (std::size_t k = j + 1; k < P.size(); ++k) {
                i128 d3 = (i128)P[i] * P[j] * P[k];
                if (d3 <= B) D.push_back(d3);
            }
        }
    }
    std::sort(D.begin(), D.end());
    D.erase(std::unique(D.begin(), D.end()), D.end());
    return D;
}

// --- Phase 2 4-rank sweep (≥ 1e3 discriminants, with prime factorizations) ---
struct FamMember {
    i128 D;
    std::vector<u64> primes;  // p1 < ... < pt, D = ∏ pi
};

inline u64 sweep_pool() { return 200; }
inline i128 sweep_bound() { return 2'000'000; }
inline int sweep_max_t() { return 4; }
inline i128 sweep_expected_count() { return 1986; }  // computed independently

inline void sweep_gen(const std::vector<u64>& P, std::size_t start,
                      std::vector<u64>& cur, std::vector<FamMember>& out) {
    if (!cur.empty()) {
        i128 d = 1;
        for (u64 p : cur) d *= p;
        out.push_back({d, cur});
    }
    if (static_cast<int>(cur.size()) >= sweep_max_t()) return;
    for (std::size_t i = start; i < P.size(); ++i) {
        i128 d = 1;
        for (u64 p : cur) d *= p;
        if (d * static_cast<i128>(P[i]) > sweep_bound()) continue;
        cur.push_back(P[i]);
        sweep_gen(P, i + 1, cur, out);
        cur.pop_back();
    }
}

inline std::vector<FamMember> sweep_members() {
    std::vector<u64> P;
    for (u64 n = 5; n <= sweep_pool(); n += 4)
        if (at::core::is_prime(n)) P.push_back(n);
    std::vector<FamMember> out;
    std::vector<u64> cur;
    sweep_gen(P, 0, cur, out);
    return out;
}

// Stage 1 linking matrix over the sweep prime pool (the "linking side" input).
inline at::linking::LinkingMatrix sweep_linking() {
    return at::linking::LinkingMatrix::build(sweep_pool());
}

}  // namespace at::verify::stage3
