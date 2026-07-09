#pragma once
#include "core/int_types.h"

// Stage M0 — elliptic curves E/ℚ in Weierstrass form and the invariants needed
// to reduce to a short model at good primes p > 3. Conventions pinned in
// docs/notes/m0-pinning.md §1–§2 (Silverman III.1). ell → {symbols, core}
// (ARCHITECTURE-M §1); no family/averaging concepts live here.
namespace at::ell {

using at::core::i64;
using at::core::i128;

// Long Weierstrass model  y² + a1 x y + a3 y = x³ + a2 x² + a4 x + a6.
struct Curve {
    i64 a1, a2, a3, a4, a6;
};

// Standard b- and c-invariants (Silverman III.1). Returned as 128-bit: exact for
// the M0 conductor-<10000 slice; the frozen referee (ell::ap_charsum) never calls
// these — it reduces mod p first — so these are for tests/inspection only.
i128 b2(const Curve& E);
i128 b4(const Curve& E);
i128 b6(const Curve& E);
i128 c4(const Curve& E);
i128 c6(const Curve& E);
i128 discriminant(const Curve& E);   // Δ = (c4³ − c6²)/1728

// Admissible changes of variable producing an isomorphic INTEGRAL model of the
// same curve over ℚ (identical a_p at every good prime — m0-pinning §2):
//   translate: (x,y) ↦ (x + r, y + s·x + t)          [u = 1, always integral]
Curve translate(const Curve& E, i64 r, i64 s, i64 t);
//   scale:     (x,y) ↦ (x/u², y/u³)  ⇒  a_i ↦ u^i·a_i  [integral for integer u]
Curve scale(const Curve& E, i64 u);

// Minimality precondition (m0-pinning §5, correction C1). Good reduction at p is
// p ∤ Δ_minimal ⟺ p ∤ N (conductor, oracle) — NOT p ∤ Δ(model): Δ scales by u¹²
// under a rescaling, so a non-minimal model has extra primes in Δ that do not
// divide N. Point-counting a_p on a non-p-minimal model counts a SINGULAR
// reduction and returns a wrong a_p silently. This asserts the consistency
// "p ∤ Δ(model) ⟺ p ∤ N for all p" (equivalently rad(Δ(model)) = rad(N)); it
// THROWS std::runtime_error if some prime divides Δ(model) but not N. Every curve
// entering the a_p path must pass this (the M1 loader calls it per curve).
void assert_minimal(const Curve& E, at::core::u64 conductor_N);

}  // namespace at::ell
