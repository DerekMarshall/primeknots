#pragma once
#include <vector>

#include "classgroup/class_group.h"  // negative_pell_solvable
#include "core/int_types.h"
#include "core/primes.h"
#include "harness.h"
#include "symbols/legendre.h"

// Stage 4 test helpers (Hirano mod-2 route). The sweep is gated by the
// negative-Pell hypothesis N(ε)=−1 (R3) and stratified by row-parity (R2).
namespace at::verify::stage4 {

using at::core::i128;
using at::core::u64;

enum ParityClass { ALL_EVEN = 0, ALL_ODD = 1, MIXED = 2 };

struct Instance {
    i128 D;
    std::vector<u64> primes;  // p1<...<pr, ≡1 mod4, distinct
    int r;
    ParityClass parity;       // of the linking-matrix row sums
};

inline int lk2(u64 p, u64 q) { return at::symbols::legendre_euler(p, q) == 1 ? 0 : 1; }

// Row-parity class of a tuple, from the mod-2 linking numbers.
inline ParityClass classify(const std::vector<u64>& P) {
    int r = static_cast<int>(P.size());
    int evens = 0, odds = 0;
    for (int i = 0; i < r; ++i) {
        int rs = 0;
        for (int j = 0; j < r; ++j)
            if (j != i) rs ^= lk2(P[i], P[j]);
        if (rs) ++odds; else ++evens;
    }
    if (odds == 0) return ALL_EVEN;
    if (evens == 0) return ALL_ODD;
    return MIXED;
}

inline u64 sweep_prime_bound() { return at::verify::g_extended ? 200ULL : 100ULL; }
inline int sweep_max_r() { return 4; }
inline long expected_total() { return at::verify::g_extended ? 4810 : 386; }    // pinned
inline long expected_excluded() { return at::verify::g_extended ? 2715 : 164; }  // R3 empiric

// All r-tuples (r = 2..sweep_max_r) of distinct primes ≡ 1 (mod 4) ≤ bound with
// N(ε_K)=−1 (pell_neg). `excluded` counts tuples dropped by the pell gate (R3).
inline std::vector<Instance> sweep(long& excluded) {
    std::vector<u64> P;
    for (u64 n = 5; n <= sweep_prime_bound(); n += 4)
        if (at::core::is_prime(n)) P.push_back(n);
    std::vector<Instance> out;
    excluded = 0;
    std::vector<u64> cur;
    // recursive subset generation, sizes 2..sweep_max_r
    auto rec = [&](auto&& self, std::size_t start) -> void {
        int r = static_cast<int>(cur.size());
        if (r >= 2) {
            i128 D = 1;
            for (u64 p : cur) D *= p;
            if (at::classgroup::negative_pell_solvable(D))
                out.push_back({D, cur, r, classify(cur)});
            else
                ++excluded;
        }
        if (r >= sweep_max_r()) return;
        for (std::size_t i = start; i < P.size(); ++i) {
            cur.push_back(P[i]);
            self(self, i + 1);
            cur.pop_back();
        }
    };
    rec(rec, 0);
    return out;
}

}  // namespace at::verify::stage4
