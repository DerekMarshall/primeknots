#include "mform/trace.h"

#include <cmath>

namespace at::mform {

Frac newform_weighted_trace_k2(i64 N, i64 P) {
    const i64 four_pn = 4 * P * N;
    // H₁(−4PN)/2 − (P+1)
    const Frac h_principal = hurwitz(four_pn);
    Frac s = add(frac(h_principal.num, h_principal.den * 2), frac(-(P + 1), 1));
    // Elliptic terms: Σ_{1≤r≤2√(P/N)} H₁(r²N²−4PN) = Σ_r hurwitz(4PN − r²N²).
    const i64 rmax = static_cast<i64>(std::floor(2.0 * std::sqrt(
        static_cast<double>(P) / static_cast<double>(N))));
    for (i64 r = 1; r <= rmax; ++r) {
        const i64 arg = four_pn - r * r * N * N;   // = |r²N²−4PN| > 0 for r ≤ 2√(P/N)
        s = add(s, hurwitz(arg));
    }
    return s;
}

Frac newform_weighted_trace_k2(i64 N, i64 P, const HurwitzTable& H) {
    const i64 four_pn = 4 * P * N;
    const Frac h_principal = H(four_pn);
    Frac s = add(frac(h_principal.num, h_principal.den * 2), frac(-(P + 1), 1));
    const i64 rmax = static_cast<i64>(std::floor(2.0 * std::sqrt(
        static_cast<double>(P) / static_cast<double>(N))));
    for (i64 r = 1; r <= rmax; ++r) {
        const i64 arg = four_pn - r * r * N * N;
        s = add(s, H(arg));
    }
    return s;
}

}  // namespace at::mform
