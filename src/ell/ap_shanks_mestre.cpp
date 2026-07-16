#include "ell/ap_shanks_mestre.h"

#include <cmath>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <unordered_map>

#include "core/modpow.h"
#include "ell/ap.h"                 // ap_charsum (the small-p fallback + the twin referee)
#include "symbols/legendre.h"
#include "symbols/tonelli_shanks.h"

namespace at::ell {

using at::core::u64;

namespace {

// ---- arithmetic mod p (p prime, p < 2^32 in our range; products via core::modmul) ----
inline u64 addm(u64 a, u64 b, u64 p) { a += b; return a >= p ? a - p : a; }
inline u64 subm(u64 a, u64 b, u64 p) { return a >= b ? a - b : a + p - b; }
inline u64 mulm(u64 a, u64 b, u64 p) { return at::core::modmul(a % p, b % p, p); }
inline u64 invm(u64 a, u64 p) { return at::core::modpow(a, p - 2, p); }   // p prime, a ≠ 0

u64 isqrt(u64 n) {
    if (n < 2) return n;
    u64 x = static_cast<u64>(std::sqrt(static_cast<double>(n)));
    while (x > 0 && x > n / x) --x;               // x*x > n  (overflow-safe)
    while ((x + 1) <= n / (x + 1)) ++x;           // (x+1)^2 <= n
    return x;
}

// splitmix64 finalizer — deterministic mixing for the derandomized point sequence (R1)
inline u64 mix(u64 z) {
    z += 0x9e3779b97f4a7c15ULL;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

// ---- affine points on y² = x³ + Ax + B mod p ----
struct Pt { u64 x, y; bool inf; };
constexpr Pt INF{0, 0, true};

inline Pt negp(const Pt& P, u64 p) { return P.inf ? P : Pt{P.x, P.y ? p - P.y : 0, false}; }

// point key for the BSGS hash map: distinct for distinct points, sentinel for ∞.
// x,y < p ≤ ~4.15M ⇒ x*p + y < p² < 1.8e13 fits u64 comfortably.
inline u64 key(const Pt& P, u64 p) { return P.inf ? ~0ULL : P.x * p + P.y; }

Pt addp(const Pt& P, const Pt& Q, u64 A, u64 p) {
    if (P.inf) return Q;
    if (Q.inf) return P;
    if (P.x == Q.x) {
        if ((P.y + Q.y) % p == 0) return INF;                 // Q = −P (incl. y = 0 doubling)
        const u64 num = addm(mulm(3, mulm(P.x, P.x, p), p), A, p);   // 3x² + A
        const u64 lam = mulm(num, invm(mulm(2, P.y, p), p), p);      // /(2y)
        const u64 x3 = subm(subm(mulm(lam, lam, p), P.x, p), Q.x, p);
        const u64 y3 = subm(mulm(lam, subm(P.x, x3, p), p), P.y, p);
        return {x3, y3, false};
    }
    const u64 lam = mulm(subm(Q.y, P.y, p), invm(subm(Q.x, P.x, p), p), p);
    const u64 x3 = subm(subm(mulm(lam, lam, p), P.x, p), Q.x, p);
    const u64 y3 = subm(mulm(lam, subm(P.x, x3, p), p), P.y, p);
    return {x3, y3, false};
}

Pt mulp(u64 k, Pt P, u64 A, u64 p) {          // scalar multiple, double-and-add
    Pt R = INF;
    while (k) {
        if (k & 1) R = addp(R, P, A, p);
        P = addp(P, P, A, p);
        k >>= 1;
    }
    return R;
}

// A deterministic point on y² = x³ + Ax + B (R1): x from a hash sequence seeded by (seed,iter);
// take the first x whose RHS is a square. Returns ∞ only in the degenerate no-point case.
Pt next_point(u64 A, u64 B, u64 p, u64 seed, int iter) {
    for (u64 k = 0; k < 512; ++k) {
        const u64 x = mix(seed + static_cast<u64>(iter) * 0x100000001b3ULL + k) % p;
        const u64 rhs = addm(addm(mulm(x, mulm(x, x, p), p), mulm(A, x, p), p), B, p);
        const int ls = at::symbols::legendre_euler(rhs, p);
        if (ls == 0) return {x, 0, false};
        if (ls == 1) {
            const auto r = at::symbols::sqrt_mod(rhs, p);
            if (r) return {x, *r, false};
        }
    }
    return INF;
}

// Smallest M ∈ [lo,hi] with M·P = O, via BSGS (baby j·P, giant −lo·P − i·(w·P)). M is a
// multiple of |P|. Returns 0 if none in range (not expected for a good prime).
u64 smallest_mult_in_hasse(const Pt& P, u64 A, u64 p, u64 lo, u64 hi) {
    const u64 width = hi - lo;
    const u64 w = isqrt(width) + 1;
    std::unordered_map<u64, u64> baby;
    baby.reserve(w * 2 + 4);
    Pt R = INF;
    for (u64 j = 0; j <= w; ++j) {                 // baby steps j·P (keep smallest j per point)
        baby.emplace(key(R, p), j);
        R = addp(R, P, A, p);
    }
    const Pt negbase = negp(mulp(lo, P, A, p), p);  // −lo·P
    const Pt negG = negp(mulp(w, P, A, p), p);       // −w·P
    Pt T = negbase;
    const u64 imax = width / w + 2;
    for (u64 i = 0; i <= imax; ++i) {                // T = −lo·P − i·(w·P)
        const auto it = baby.find(key(T, p));
        if (it != baby.end()) {
            const u64 M = lo + i * w + it->second;   // (lo + i·w + j)·P = O
            if (M <= hi && mulp(M, P, A, p).inf) return M;
        }
        T = addp(T, negG, A, p);
    }
    return 0;
}

// Reduce a multiple M of |P| to the exact order |P| (Sutherland Alg. 7.4): for each prime q|M,
// divide out q while (n/q)·P = O.
u64 point_order(const Pt& P, u64 A, u64 p, u64 M) {
    u64 n = M, m = M;
    auto strip = [&](u64 q) { while (n % q == 0 && mulp(n / q, P, A, p).inf) n /= q; };
    for (u64 q = 2; q * q <= m; ++q) {
        if (m % q) continue;
        while (m % q == 0) m /= q;
        strip(q);
    }
    if (m > 1) strip(m);
    return n;
}

inline u64 lcm_u64(u64 a, u64 b) { return a / std::gcd(a, b) * b; }

// If λ has exactly one multiple in [lo,hi], return it (else 0); λ | #E always.
u64 unique_multiple(u64 lam, u64 lo, u64 hi) {
    if (lam == 0) return 0;
    const u64 kmin = (lo + lam - 1) / lam;   // ceil(lo/λ)
    const u64 kmax = hi / lam;               // floor(hi/λ)
    if (kmax >= kmin && kmax == kmin) return kmin * lam;
    return 0;
}

}  // namespace

int ap_shanks_mestre(const Curve& E, u64 p, int* resolved_by) {
    if (p <= 3) throw std::invalid_argument("ap_shanks_mestre: short model requires p > 3");

    // Reduce to the short model y² = x³ + Ax + B mod p — IDENTICAL to ap_charsum/ap_fast.
    auto rp = [p](at::core::i64 v) -> u64 {
        at::core::i64 m = v % static_cast<at::core::i64>(p);
        return static_cast<u64>(m < 0 ? m + static_cast<at::core::i64>(p) : m);
    };
    const u64 a1 = rp(E.a1), a2 = rp(E.a2), a3 = rp(E.a3), a4 = rp(E.a4), a6 = rp(E.a6);
    const u64 B2 = addm(mulm(a1, a1, p), mulm(4, a2, p), p);
    const u64 B4 = addm(mulm(2, a4, p), mulm(a1, a3, p), p);
    const u64 B6 = addm(mulm(a3, a3, p), mulm(4, a6, p), p);
    const u64 C4 = subm(mulm(B2, B2, p), mulm(24, B4, p), p);
    const u64 B2cube = mulm(mulm(B2, B2, p), B2, p);
    const u64 C6 = subm(addm(subm(0, B2cube, p), mulm(36, mulm(B2, B4, p), p), p), mulm(216, B6, p), p);
    const u64 A = subm(0, mulm(27, C4, p), p);
    const u64 B = subm(0, mulm(54, C6, p), p);

    // Bad reduction: p | (4A³ + 27B²) ⇒ singular. REFUSE (never point-count a singular reduction).
    const u64 disc = addm(mulm(4, mulm(A, mulm(A, A, p), p), p), mulm(27, mulm(B, B, p), p), p);
    if (disc == 0)
        throw std::invalid_argument("ap_shanks_mestre: bad reduction (p | 4A^3+27B^2) — defer to oracle");

    if (p <= kMestreThreshold) {                             // §5 small-p fallback (Thm 7.7 needs p>229)
        if (resolved_by) *resolved_by = -1;
        return ap_charsum(E, p);
    }

    // Hasse interval [lo, hi] ∋ #E (|a_p| ≤ 2√p); widen with +2 slack (isqrt is floor).
    const u64 hw = 2 * isqrt(p) + 2;
    const u64 lo = p + 1 - hw, hi = p + 1 + hw;

    // Quadratic twist Ẽ: y² = x³ + s²A x + s³B, s = smallest non-residue (Sutherland §7.6).
    u64 s = 2;
    while (at::symbols::legendre_euler(s, p) != -1) ++s;
    const u64 s2 = mulm(s, s, p);
    const u64 At = mulm(A, s2, p);
    const u64 Bt = mulm(B, mulm(s, s2, p), p);

    // R2 loop invariant: each iteration extends λ on E AND the twist; check uniqueness on EITHER.
    // Thm 7.7 (p > 229) guarantees one of them has a unique Hasse multiple ⇒ termination
    // [correctness register]. Thm 7.6 (6/π²) bounds the EXPECTED iterations [performance register].
    u64 lamE = 1, lamT = 1;
    const u64 seedE = mix(A) ^ mix(mulm(B, 3, p) + 1) ^ mix(p * 2 + 1);
    const u64 seedT = mix(At + 7) ^ mix(mulm(Bt, 5, p) + 2) ^ mix(p * 3 + 2);

    for (int iter = 0; iter < 96; ++iter) {
        const Pt PE = next_point(A, B, p, seedE, iter);
        if (!PE.inf) {
            const u64 M = smallest_mult_in_hasse(PE, A, p, lo, hi);
            if (M) lamE = lcm_u64(lamE, point_order(PE, A, p, M));
        }
        if (const u64 nE = unique_multiple(lamE, lo, hi)) {
            if (resolved_by) *resolved_by = 0;               // E-side λ unique
            return static_cast<int>(static_cast<long long>(p + 1) - static_cast<long long>(nE));
        }

        const Pt PT = next_point(At, Bt, p, seedT, iter);
        if (!PT.inf) {
            const u64 M = smallest_mult_in_hasse(PT, At, p, lo, hi);
            if (M) lamT = lcm_u64(lamT, point_order(PT, At, p, M));
        }
        if (const u64 nEt = unique_multiple(lamT, lo, hi)) {    // #Ẽ unique ⇒ #E = 2p+2 − #Ẽ
            if (resolved_by) *resolved_by = 1;                  // twist-side λ unique
            return static_cast<int>(static_cast<long long>(nEt) - static_cast<long long>(p + 1));
        }
    }
    throw std::runtime_error("ap_shanks_mestre: #E undetermined after 96 iters (p>229 should terminate)");
}

}  // namespace at::ell
