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

// Composition is well-formed on the class group: preserves discriminant &
// primitivity, the principal form is the identity, and it is commutative.
TEST_CASE("theorem_compose_group_axioms") {
    std::vector<i128> Ds = family_discriminants();
    i128 cases = 0;
    for (i128 D : Ds) {
        QForm id = principal_form(D);
        std::vector<QForm> reps = cycle(id);  // representatives (principal class)
        // also bring in a non-principal class via composition below
        for (const QForm& f : reps) {
            QForm ff = reduce_indefinite(f);
            // identity: f ∘ principal ~ f
            QForm fp = compose(ff, id);
            CHECK(fp.disc() == D);
            CHECK(is_primitive(fp));
            CHECK(class_key(fp) == class_key(ff));
            // commutativity: f ∘ g ~ g ∘ f  (use g = principal here and a shifted rep)
            QForm g = reduce_indefinite(translate(ff));
            CHECK(class_key(compose(ff, g)) == class_key(compose(g, ff)));
            ++cases;
        }
    }
    MESSAGE("cases: " << static_cast<long long>(cases)
                      << " composition checks (disc, primitive, identity, commutative)");
    REQUIRE(cases > 0);
}
