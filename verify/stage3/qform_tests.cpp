// Stage 3 Phase 1 — forms layer: reduction, composition, cycle-based class
// number. Convention-free; narrow class group of real quadratic fields.
#include "doctest/doctest.h"

#include <set>
#include <tuple>
#include <vector>

#include "classgroup/qform.h"
#include "stage3/fixture.h"

using at::core::i128;
using namespace at::classgroup;
using namespace at::verify::stage3;

namespace {
// Canonical class key (lex-min over the cycle) — for class equality in tests.
std::tuple<i128, i128, i128> class_key(const QForm& f) {
    QForm red = reduce_indefinite(f);
    std::vector<QForm> cy = cycle(red);
    QForm best = cy[0];
    for (const QForm& g : cy)
        if (std::tie(g.a, g.b, g.c) < std::tie(best.a, best.b, best.c)) best = g;
    return {best.a, best.b, best.c};
}
// x -> x+1 substitution: (a,b,c) -> (a, b+2a, a+b+c); disc-preserving, proper-equiv.
QForm translate(const QForm& f) { return {f.a, f.b + 2 * f.a, f.a + f.b + f.c}; }
// Inverse (opposite) form: (a,b,c) -> (a,-b,c); its class is the group inverse.
QForm opposite(const QForm& f) { return {f.a, -f.b, f.c}; }

// A handful of DISTINCT class representatives for discriminant D: principal plus
// reduced prime forms for small split primes, deduplicated by class.
std::vector<QForm> class_reps(i128 D, std::size_t maxn) {
    std::vector<QForm> reps;
    std::set<std::tuple<i128, i128, i128>> seen;
    auto add = [&](const QForm& f) {
        auto k = class_key(f);
        if (seen.insert(k).second) reps.push_back(reduce_indefinite(f));
    };
    add(principal_form(D));
    for (i128 q = 2; q < 300 && reps.size() < maxn; ++q) {
        i128 fourq = 4 * q;
        for (i128 b = 0; b < 2 * q; ++b) {
            if ((b * b - D) % fourq == 0) {
                QForm pf{q, b, (b * b - D) / fourq};
                if (is_primitive(pf)) add(pf);
                break;
            }
        }
    }
    return reps;
}
}  // namespace

// TWIN: narrow h⁺ two independent ways — cycle enumeration vs. composition-based
// group closure — over the whole family sweep.
TEST_CASE("twin_classno_cycles_vs_composition") {
    std::vector<i128> Ds = family_discriminants();
    REQUIRE(static_cast<i128>(Ds.size()) == expected_disc_count());
    i128 cases = 0;
    for (i128 D : Ds) {
        i128 by_cycles = narrow_class_number_by_cycles(D);
        i128 by_comp = narrow_class_number_by_composition(D);
        CHECK(by_cycles == by_comp);
        CHECK(by_cycles >= 1);
        ++cases;
    }
    MESSAGE("cases: " << static_cast<long long>(cases)
                      << " discriminants; h⁺ by cycle-count == h⁺ by composition");
    REQUIRE(cases == expected_disc_count());
}

// reduce() output is reduced AND in the same proper-equivalence class as its
// input (cycle membership), and primitivity is preserved. Non-reduced inputs are
// manufactured by repeated x->x+1 translation of reduced forms.
TEST_CASE("theorem_reduce_is_reduced_and_equivalent") {
    std::vector<i128> Ds = family_discriminants();
    i128 cases = 0;
    for (i128 D : Ds) {
        QForm principal = principal_form(D);
        std::vector<QForm> starts = cycle(principal);  // a full class of reduced forms
        for (const QForm& r : starts) {
            QForm g = r;
            for (int t = 0; t < 4; ++t) g = translate(g);  // now (usually) non-reduced
            QForm red = reduce_indefinite(g);
            CHECK(red.disc() == D);
            CHECK(is_reduced_indefinite(red));
            CHECK(is_primitive(red) == is_primitive(g));
            CHECK(class_key(red) == class_key(r));  // same class as the original
            ++cases;
        }
    }
    MESSAGE("cases: " << static_cast<long long>(cases)
                      << " forms reduced; each reduced & class-preserving");
    REQUIRE(cases > 0);
}

// Composition is a well-formed abelian-group law on classes: disc & primitivity
// preserved, principal = identity, commutative, associative, and inverses
// (f ∘ opposite(f) ~ principal). Associativity is the axiom that classically
// exposes composition bugs, so it is tested over genuinely distinct triples.
TEST_CASE("theorem_compose_group_axioms") {
    std::vector<i128> Ds = family_discriminants();
    i128 cases = 0, assoc_triples = 0, multiclass_discs = 0;
    for (i128 D : Ds) {
        QForm id = principal_form(D);
        auto idk = class_key(id);
        std::vector<QForm> R = class_reps(D, 6);  // distinct classes
        if (R.size() > 1) ++multiclass_discs;
        for (const QForm& f : R) {
            CHECK(compose(f, f).disc() == D);
            CHECK(is_primitive(compose(f, f)));
            CHECK(class_key(compose(f, id)) == class_key(f));            // identity
            CHECK(class_key(compose(f, opposite(f))) == idk);           // inverse
            ++cases;
            for (const QForm& g : R) {
                CHECK(class_key(compose(f, g)) == class_key(compose(g, f)));  // commutative
                for (const QForm& h : R) {
                    CHECK(class_key(compose(compose(f, g), h)) ==
                          class_key(compose(f, compose(g, h))));             // associative
                    ++assoc_triples;
                }
            }
        }
    }
    MESSAGE("cases: " << static_cast<long long>(cases) << " elements; "
                      << static_cast<long long>(assoc_triples)
                      << " associativity triples; " << static_cast<long long>(multiclass_discs)
                      << " discriminants with >1 class");
    REQUIRE(cases > 0);
    REQUIRE(multiclass_discs > 0);  // associativity/inverse tested non-trivially
}
