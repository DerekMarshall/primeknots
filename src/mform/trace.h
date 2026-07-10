#pragma once
#include "core/int_types.h"
#include "mform/hurwitz.h"

// Stage M3 — the assembled Eichler–Selberg / Skoruppa–Zagier newform trace
// (m3-pinning §P1), weight 2:
//   Σ_{f∈H^new(N,2)} a_f(P)·ε(f) = H₁(−4PN)/2 + Σ_{1≤r≤2√(P/N)} H₁(r²N²−4PN) − (P+1),
// where H₁(−d) = mform::hurwitz(d). Root-number-weighted via Atkin–Lehner (§P3).
// Returned as a Frac and provably integral for the family (REQUIREd, §P1 sanity).
// Precondition: N square-free, P prime, P ∤ N (§P6). The r-sum boundary r²=4P/N,
// where H₁(0) would enter, cannot occur for this family (§P5/R5).
namespace at::mform {

using at::core::i64;

Frac newform_weighted_trace_k2(i64 N, i64 P);

}  // namespace at::mform
