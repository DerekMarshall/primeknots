#include "classgroup/qform.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <set>
#include <tuple>
#include <vector>

#include "core/primes.h"
#include "symbols/legendre.h"
#include "symbols/tonelli_shanks.h"

namespace at::classgroup {

using at::core::i128;
using at::core::u64;

namespace {

i128 iabs(i128 v) { return v < 0 ? -v : v; }

i128 gcd_i128(i128 a, i128 b) {
    a = iabs(a);
    b = iabs(b);
    while (b) { i128 t = a % b; a = b; b = t; }
    return a;
}

// Extended gcd: returns g=gcd(|a|,|b|)>=0, sets x,y with a*x+b*y=g.
i128 egcd(i128 a, i128 b, i128& x, i128& y) {
    if (b == 0) { x = a < 0 ? -1 : 1; y = 0; return iabs(a); }
    i128 x1, y1;
    i128 g = egcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return g;
}

// floor(sqrt(n)) for n >= 0.
i128 isqrt_i128(i128 n) {
    if (n <= 0) return 0;
    i128 x = static_cast<i128>(std::sqrt(static_cast<double>(n)));
    while (x > 0 && x * x > n) --x;
    while ((x + 1) * (x + 1) <= n) ++x;
    return x;
}

// Solve x ≡ r1 (mod m1), x ≡ r2 (mod m2) with m1,m2 > 0 (possibly non-coprime).
// Returns {solvable, x in [0,lcm), lcm}.
struct Crt { bool ok; i128 x, lcm; };
Crt crt2(i128 r1, i128 m1, i128 r2, i128 m2) {
    i128 p, q;
    i128 g = egcd(m1, m2, p, q);       // p*m1 + q*m2 = g
    if ((r2 - r1) % g != 0) return {false, 0, 0};
    i128 lcm = m1 / g * m2;
    i128 t = ((r2 - r1) / g) % (m2 / g);
    i128 x = r1 + m1 * ((t * (p % (m2 / g))) % (m2 / g));
    x = ((x % lcm) + lcm) % lcm;
    return {true, x, lcm};
}

using Key = std::tuple<i128, i128, i128>;
Key key(const QForm& f) { return {f.a, f.b, f.c}; }

}  // namespace

bool is_primitive(const QForm& f) {
    return gcd_i128(gcd_i128(f.a, f.b), f.c) == 1;
}

bool is_reduced_indefinite(const QForm& f) {
    i128 D = f.disc();
    if (D <= 0) return false;
    i128 b = f.b;
    if (b <= 0) return false;
    if (!(b * b < D)) return false;                 // b < √D
    i128 aa = iabs(f.a);
    i128 hi = 2 * aa + b;
    if (!(D < hi * hi)) return false;               // √D < 2|a| + b
    i128 lo = 2 * aa - b;                            // √D > 2|a| − b
    if (lo > 0 && !(D > lo * lo)) return false;
    return true;
}

QForm rho(const QForm& f) {
    i128 D = f.disc();
    i128 b = f.b, c = f.c;
    i128 s = isqrt_i128(D);
    i128 cc = iabs(c);
    i128 twoc = 2 * cc;
    i128 target = (cc > s) ? cc : s;
    i128 r = ((target + b) % twoc + twoc) % twoc;   // in [0, 2|c|)
    i128 bp = target - r;                            // ≡ −b (mod 2|c|), in reduced band
    i128 cp = (bp * bp - D) / (4 * c);
    return {c, bp, cp};
}

QForm reduce_indefinite(const QForm& f) {
    QForm g = f;
    for (int i = 0; i < 10000 && !is_reduced_indefinite(g); ++i) g = rho(g);
    return g;
}

std::vector<QForm> cycle(const QForm& f0) {
    std::vector<QForm> out;
    QForm f = f0;
    do {
        out.push_back(f);
        f = rho(f);
    } while (!(f == f0) && out.size() < 100000);
    return out;
}

QForm canonical_form(const QForm& f) {
    QForm red = reduce_indefinite(f);
    std::vector<QForm> cy = cycle(red);
    QForm best = cy[0];
    for (const QForm& g : cy)
        if (std::tie(g.a, g.b, g.c) < std::tie(best.a, best.b, best.c)) best = g;
    return best;
}

namespace {
QForm canonical(const QForm& f) { return canonical_form(f); }
}  // namespace

QForm principal_form(i128 D) {
    // disc((1,1,(1−D)/4)) = 1 − (1−D) = D  (D ≡ 1 mod 4).
    return reduce_indefinite(QForm{1, 1, (1 - D) / 4});
}

QForm compose(const QForm& f, const QForm& g) {
    i128 D = f.disc();
    i128 a1 = f.a, b1 = f.b, a2 = g.a, b2 = g.b;
    i128 s = (b1 + b2) / 2;
    i128 e = gcd_i128(gcd_i128(a1, a2), s);   // e >= 0 (gcd_i128 takes |·|)
    i128 A = (a1 / e) * (a2 / e);             // = a1 a2 / e², SIGNED (indefinite)
    // Congruences use the moduli's magnitudes (indefinite forms can have a<0):
    //   B ≡ b1 (mod 2|a1|/e), B ≡ b2 (mod 2|a2|/e); lift so 4A | (B²−D).
    i128 m1 = 2 * iabs(a1) / e, m2 = 2 * iabs(a2) / e;
    Crt cr = crt2(((b1 % m1) + m1) % m1, m1, ((b2 % m2) + m2) % m2, m2);
    i128 B = cr.x, L = cr.lcm;
    i128 modA = 2 * iabs(A);
    i128 fourA = 4 * A;  // signed; divisibility/quotient are sign-agnostic
    for (i128 cand = B; cand < B + modA; cand += L) {
        i128 bb = ((cand % modA) + modA) % modA;
        if ((bb * bb - D) % fourA == 0) {
            return reduce_indefinite(QForm{A, bb, (bb * bb - D) / fourA});
        }
    }
    i128 bb = ((B % modA) + modA) % modA;  // fallback
    return reduce_indefinite(QForm{A, bb, (bb * bb - D) / fourA});
}

i128 narrow_class_number_by_cycles(i128 D) {
    i128 s = isqrt_i128(D);
    std::set<Key> reduced;
    for (i128 a = -s; a <= s; ++a) {
        if (a == 0) continue;
        for (i128 b = 1; b <= s; ++b) {
            i128 num = b * b - D;             // = 4ac
            if (num % (4 * a) != 0) continue;
            QForm fm{a, b, num / (4 * a)};
            if (fm.disc() != D) continue;
            if (!is_reduced_indefinite(fm)) continue;
            if (!is_primitive(fm)) continue;
            reduced.insert(key(fm));
        }
    }
    std::set<Key> visited;
    i128 count = 0;
    for (const Key& k : reduced) {
        if (visited.count(k)) continue;
        ++count;
        QForm start{std::get<0>(k), std::get<1>(k), std::get<2>(k)};
        QForm gform = start;
        do {
            visited.insert(key(gform));
            gform = rho(gform);
        } while (!(gform == start));
    }
    return count;
}

std::vector<QForm> class_group_elements(i128 D) {
    // BFS-close the group under composition from prime-form generators.
    QForm id = canonical(principal_form(D));
    std::set<Key> classes{key(id)};
    std::vector<QForm> elems{id};

    // Prime-form generators for all split primes q ≤ √D/2 (Minkowski bound):
    // this provably generates the whole class group (Cl⁺ is generated by prime
    // ideals of norm ≤ the Minkowski bound). A fixed small cap would silently
    // return a proper subgroup for larger D.
    std::vector<QForm> gens;
    i128 qmax = isqrt_i128(D) / 2 + 1;
    for (i128 q = 2; q <= qmax; ++q) {
        if (!at::core::is_prime(static_cast<u64>(q))) continue;
        if (D % q == 0) continue;
        u64 Dq = static_cast<u64>(((D % q) + q) % q);
        if (q != 2 && at::symbols::legendre_euler(Dq, static_cast<u64>(q)) != 1) continue;
        i128 fourq = 4 * q;
        i128 bb = -1;
        for (i128 t = 0; t < 2 * q; ++t)
            if ((t * t - D) % fourq == 0) { bb = t; break; }
        if (bb < 0) continue;
        QForm pf{q, bb, (bb * bb - D) / fourq};
        if (!is_primitive(pf)) continue;
        gens.push_back(canonical(pf));
    }

    // Closure: repeatedly compose known classes by generators until stable.
    std::vector<QForm> frontier = elems;
    while (!frontier.empty()) {
        std::vector<QForm> next;
        for (const QForm& f : frontier) {
            for (const QForm& gg : gens) {
                QForm prod = canonical(compose(f, gg));
                if (classes.insert(key(prod)).second) {
                    elems.push_back(prod);
                    next.push_back(prod);
                }
            }
        }
        frontier.swap(next);
    }
    return elems;
}

i128 narrow_class_number_by_composition(i128 D) {
    return static_cast<i128>(class_group_elements(D).size());
}

}  // namespace at::classgroup
