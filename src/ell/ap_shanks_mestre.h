#pragma once
#include "core/int_types.h"
#include "ell/curve.h"

// Stage M0b — fast a_p by Shanks–Mestre point counting (docs/notes/m0b-pinning.md).
// A THIRD, genuinely independent a_p algorithm: point counting, NOT a character sum
// (cf. ell::ap_charsum, the frozen referee; ell::ap_fast, the QR-table charsum). It is
// twinned AGAINST ap_charsum (twin_*), never the other way round.
//
// For a good prime p > 3 in the short model y² = x³ + Ax + B (A,B = (−27c₄,−54c₆) mod p,
// the SAME reduction ap_charsum/ap_fast use, so a_p agrees exactly): a_p = p + 1 − #E(F_p),
// with #E found by BSGS over the Hasse interval + the Mestre quadratic-twist disambiguation
// (Theorem 7.7, valid for p > 229 — Sutherland 18.783 Lec #7). For p ≤ MESTRE_THRESHOLD the
// twist-trick uniqueness is not guaranteed, so it DEFERS to the frozen ap_charsum (§5 fallback).
//
// Determinism (R1): point selection is a deterministic sequence seeded by a canonical hash of
// (A,B,p); the twist uses the smallest non-residue mod p. No RNG — a mismatch replays bit-
// identically. The output a_p is a path-independent integer regardless.
//
// Bad reduction (p | 4A³+27B² in the short model) is REFUSED — throws std::invalid_argument.
// Callers must not pass bad primes (the refusal is demonstrated firing in the tests; M0b never
// assumes a caller pre-filtered).
namespace at::ell {

using at::core::u64;

constexpr u64 kMestreThreshold = 229;   // Thm 7.7 bound; p ≤ threshold ⇒ charsum fallback (§5)

// If resolved_by != nullptr it is set to how #E was determined (a twin-witness diagnostic):
//   -1 = charsum fallback (p ≤ threshold);  0 = E-side λ had the unique Hasse multiple;
//    1 = twist-side λ was unique (⇒ #E = 2p+2 − #Ẽ). Unset on the throw paths.
int ap_shanks_mestre(const Curve& E, u64 p, int* resolved_by = nullptr);

}  // namespace at::ell
