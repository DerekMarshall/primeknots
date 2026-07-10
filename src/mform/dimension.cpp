#include "mform/dimension.h"

#include <vector>

namespace at::mform {

namespace {

// Distinct prime factors of N (with exponents), by trial division. N is small
// (family levels ≤ ~10⁴), so O(√N) is ample.
struct PrimePow { i64 p, e; };
std::vector<PrimePow> factor(i64 N) {
    std::vector<PrimePow> f;
    for (i64 p = 2; p * p <= N; ++p) {
        if (N % p == 0) {
            i64 e = 0;
            while (N % p == 0) { N /= p; ++e; }
            f.push_back({p, e});
        }
    }
    if (N > 1) f.push_back({N, 1});
    return f;
}

i64 gcd_(i64 a, i64 b) { while (b) { i64 t = a % b; a = b; b = t; } return a; }

// Euler φ.
i64 phi(i64 n) {
    i64 r = n;
    for (const PrimePow& pp : factor(n)) r -= r / pp.p;
    return n == 1 ? 1 : r;
}

// Kronecker symbols (−4|p) and (−3|p) for a PRIME p (the only inputs here). These
// are the elliptic-point local factors; the p=2,3 values are the special cases the
// genus formula pins (§P2). Odd p: (−4|p)=(−1|p) [+1 iff p≡1 mod 4]; (−3|p) [+1 iff
// p≡1 mod 3]. p=2: (−4|2)=0, (−3|2)=−1.  p=3: (−3|3)=0, (−4|3)=(−1|3)=−1.
int kron_m4(i64 p) { return p == 2 ? 0 : (p % 4 == 1 ? 1 : -1); }
int kron_m3(i64 p) {
    if (p == 3) return 0;
    if (p == 2) return -1;
    return p % 3 == 1 ? 1 : -1;
}

}  // namespace

i64 dim_s2_gamma0(i64 N) {
    if (N <= 0) return 0;
    if (N == 1) return 0;
    const std::vector<PrimePow> f = factor(N);

    // Index μ = N·∏_{p|N}(1+1/p) = ∏_{p^e||N} p^{e−1}(p+1).
    i64 mu = 1;
    for (const PrimePow& pp : f) {
        i64 pe1 = 1;
        for (i64 i = 0; i < pp.e - 1; ++i) pe1 *= pp.p;
        mu *= pe1 * (pp.p + 1);
    }

    // Elliptic points. For square-free N neither 4|N nor 9|N ever fires; the
    // guards make the formula correct for general N too.
    i64 nu2 = 0, nu3 = 0;
    if (N % 4 != 0) { nu2 = 1; for (const PrimePow& pp : f) nu2 *= (1 + kron_m4(pp.p)); }
    if (N % 9 != 0) { nu3 = 1; for (const PrimePow& pp : f) nu3 *= (1 + kron_m3(pp.p)); }

    // Cusps ν∞ = Σ_{d|N} φ(gcd(d, N/d)).
    i64 nuinf = 0;
    for (i64 d = 1; d <= N; ++d)
        if (N % d == 0) nuinf += phi(gcd_(d, N / d));

    // g = 1 + μ/12 − ν₂/4 − ν₃/3 − ν∞/2, computed ×12 to stay in ℤ (it always is).
    const i64 twelve_g = 12 + mu - 3 * nu2 - 4 * nu3 - 6 * nuinf;
    return twelve_g / 12;   // exact: the genus is an integer (asserted in verify)
}

namespace {
// β = μ ∗ μ (Dirichlet square of Möbius), multiplicative: β(1)=1, β(p)=−2,
// β(p²)=1, β(p^k)=0 for k ≥ 3. The Dirichlet inverse of the oldform multiplicity
// σ₀ (a newform of level M appears in S₂(Γ₀(N)) with multiplicity σ₀(N/M)).
i64 beta(i64 n) {
    i64 b = 1;
    for (const PrimePow& pp : factor(n)) {
        if (pp.e == 1) b *= -2;
        else if (pp.e == 2) b *= 1;
        else return 0;
    }
    return b;
}
}  // namespace

i64 dim_s2_new(i64 N) {
    if (N <= 0) return 0;
    i64 s = 0;
    for (i64 d = 1; d <= N; ++d)
        if (N % d == 0) s += beta(N / d) * dim_s2_gamma0(d);
    return s;
}

}  // namespace at::mform
