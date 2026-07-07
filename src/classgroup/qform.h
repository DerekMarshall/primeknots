#pragma once
#include <vector>

#include "core/int_types.h"

// Binary quadratic forms (a,b,c), disc = b²−4ac, for the NARROW class group of a
// real quadratic field (D>0, indefinite ⇒ cycle-based reduction). Algorithms
// pinned to Cohen [Coh93 ch.5]; correctness certified against PARI/GP. See
// docs/notes/stage3-pinning.md. Convention-free (Phase 1B): no Rédei matrix or
// class-group structure here.
namespace at::classgroup {

struct QForm {
    at::core::i128 a, b, c;
    at::core::i128 disc() const { return b * b - 4 * a * c; }
    bool operator==(const QForm& o) const { return a == o.a && b == o.b && c == o.c; }
};

bool is_primitive(const QForm& f);          // gcd(|a|,|b|,|c|) == 1
bool is_reduced_indefinite(const QForm& f); // |√D − 2|a|| < b < √D  (D>0)

QForm rho(const QForm& f);                   // Cohen §5.6 reduction step
QForm reduce_indefinite(const QForm& f);     // apply rho to a reduced form
std::vector<QForm> cycle(const QForm& reduced_form);  // the proper-equiv class

// Principal (identity) reduced form of discriminant D>0.
QForm principal_form(at::core::i128 D);

// Gauss composition (Cohen §5.4), reduced result. Same disc, primitive.
QForm compose(const QForm& f, const QForm& g);

// Narrow class number h⁺(D), two independent ways (Phase 1B twin):
//  (A) enumerate reduced forms, count cycles;
//  (B) BFS-close the group under composition from prime-form generators.
at::core::i128 narrow_class_number_by_cycles(at::core::i128 D);
at::core::i128 narrow_class_number_by_composition(at::core::i128 D);

}  // namespace at::classgroup
