#include "classgroup/class_group.h"

#include <algorithm>
#include <map>
#include <tuple>
#include <vector>

namespace at::classgroup {

using at::core::i128;

namespace {
// Group composition on canonical class reps.
QForm mul(const QForm& x, const QForm& y) { return canonical_form(compose(x, y)); }

// g^n by binary exponentiation in the class group (n >= 0).
QForm powc(const QForm& g, i128 n, const QForm& id) {
    QForm result = id, base = g;
    while (n > 0) {
        if (n & 1) result = mul(result, base);
        base = mul(base, base);
        n >>= 1;
    }
    return result;
}

std::size_t log2_exact(i128 n) {  // n a power of two
    std::size_t k = 0;
    while (n > 1) { n >>= 1; ++k; }
    return k;
}
}  // namespace

ClassGroup ClassGroup::compute(i128 D) {
    ClassGroup cg;
    cg.D = D;
    cg.elements = class_group_elements(D);
    return cg;
}

std::size_t ClassGroup::rank2() const {
    QForm id = canonical_form(principal_form(D));
    i128 c2 = 0;
    for (const QForm& g : elements)
        if (mul(g, g) == id) ++c2;
    return log2_exact(c2);
}

std::size_t ClassGroup::rank4() const {
    QForm id = canonical_form(principal_form(D));
    i128 c2 = 0, c4 = 0;
    for (const QForm& g : elements) {
        QForm g2 = mul(g, g);
        if (g2 == id) ++c2;
        if (mul(g2, g2) == id) ++c4;  // g⁴ = id
    }
    return log2_exact(c4) - log2_exact(c2);
}

std::vector<i128> ClassGroup::invariant_factors() const {
    QForm id = canonical_form(principal_form(D));
    i128 h = order();
    if (h == 1) return {};

    // order of each element (order | h): peel prime factors of h.
    std::map<i128, int> hfac;
    {
        i128 m = h;
        for (i128 p = 2; p * p <= m; ++p)
            while (m % p == 0) { hfac[p]++; m /= p; }
        if (m > 1) hfac[m]++;
    }
    std::vector<i128> orders;
    orders.reserve(elements.size());
    for (const QForm& g : elements) {
        i128 ord = h;
        for (auto& [p, e] : hfac)
            while (ord % p == 0 && powc(g, ord / p, id) == id) ord /= p;
        orders.push_back(ord);
    }

    // Per prime p | h: p-part invariant factors from the rank sequence.
    // Collect all p-power factors, one multiset; then merge across primes.
    std::map<i128, std::vector<i128>> pparts;  // p -> exponents e_1 >= e_2 >= ...
    for (auto& [p, emax] : hfac) {
        // a_k = #{g : v_p(ord g) <= k}; r_{p^k} = log_p(a_k / a_{k-1}).
        std::vector<i128> a(emax + 1, 0);
        for (i128 ord : orders) {
            int v = 0;
            i128 t = ord;
            while (t % p == 0) { t /= p; ++v; }
            for (int k = v; k <= emax; ++k) a[k]++;
        }
        std::vector<int> r(emax + 1, 0);  // r[k] = r_{p^k}, k>=1
        for (int k = 1; k <= emax; ++k) {
            i128 ratio = a[k] / a[k - 1];
            int rk = 0;
            while (ratio > 1) { ratio /= p; ++rk; }
            r[k] = rk;
        }
        int rank_p = r[1];  // p-rank
        std::vector<i128> exps(rank_p, 0);
        for (int j = 1; j <= rank_p; ++j) {
            int ej = 0;
            for (int k = 1; k <= emax; ++k)
                if (r[k] >= j) ej = k;
            exps[j - 1] = ej;  // e_1 >= e_2 >= ... (r non-increasing)
        }
        std::vector<i128> factors;
        for (i128 e : exps) {
            i128 pe = 1;
            for (i128 i = 0; i < e; ++i) pe *= p;
            factors.push_back(pe);
        }
        pparts[p] = factors;  // descending exponents
    }

    // Merge into invariant factors: align p-parts by column (largest first).
    std::size_t maxrank = 0;
    for (auto& [p, f] : pparts) maxrank = std::max(maxrank, f.size());
    std::vector<i128> inv(maxrank, 1);
    for (auto& [p, f] : pparts)
        for (std::size_t col = 0; col < f.size(); ++col)
            inv[col] *= f[col];  // col 0 = largest factor
    std::sort(inv.begin(), inv.end());  // ascending d_1 | d_2 | ... | d_m
    return inv;
}

}  // namespace at::classgroup
