#pragma once
#include "core/int_types.h"
#include "ell/curve.h"

namespace at::ell {

using at::core::u64;

// FROZEN referee (m0-pinning §1). Trace of Frobenius a_p = p + 1 − #E(𝔽_p) at a
// good prime p > 3, by the point-count character sum
//     a_p = − Σ_{x mod p} ( (x³ + A x + B) / p ),   (A,B) = (−27·c4, −54·c6) mod p,
// each Legendre symbol via symbols::legendre_euler. Precondition: p prime, p > 3,
// p ∤ N (good reduction). Deliberately naive and O(p): this is the referee the
// Phase-2 fast path is checked against — do NOT optimize it.
int ap_charsum(const Curve& E, u64 p);

// Small-prime computed path (m0-pinning §5, rider R1). Direct point count on the
// LONG model mod p, valid at any good prime (p ∤ Δ(model)) — used for p ∈ {2,3},
// which the short-model referee (ap_charsum, p > 3) cannot reach. The point at
// infinity is counted EXPLICITLY. a_p = p + 1 − #E(𝔽_p). O(p²): a referee, frozen.
int ap_enumerate(const Curve& E, u64 p);

// Bad-prime a_q from ecdata's Atkin–Lehner sign (m0-pinning §3, PARI-adjudicated).
// w_sign is the ecdata '+'/'-' as +1/−1; ord_q_N is the conductor valuation at q.
//   multiplicative (ord_q_N == 1):  a_q = −w_sign
//   additive       (ord_q_N ≥ 2):  a_q = 0     (A–L sign present, but a_q vanishes)
int ap_from_atkin_lehner(int w_sign, int ord_q_N);

}  // namespace at::ell
