#pragma once
#include <string>

// Stage M4 emitter (ARCHITECTURE-M §6). Writes sawin_sutherland_murmuration.json: the
// empirical height-ordered murmuration (statistic (1) of [SS25], a_p COMPUTED from
// scratch, N/ε oracle-provenance) overlaid on the CONJECTURED density D(u) (Conjecture
// 1, the in-house Bessel-J₁ sum), with the committed R2 shape targets, the two-pass
// hump/zero/trough, the pre-registered windowed-trough diagnostic, and the convergence
// ladder. Consumes the committed COMPUTED run data/m4/ss_empirical.txt (so the ~19-min
// statistic is never re-run at emit/CI time); D(u) is recomputed cheaply and is
// byte-portable (in-house J₁ + generated constants, -ffp-contract=off).
//
// params.claim_class carries, IN WORDS, the honest register: partial agreement with the
// conjectured density — hump + first zero-crossing within the a-priori τ=0.06; the
// trough is an OPEN DEVIATION (m4-pinning confirmation §). No proof is claimed.
namespace at::emit {

void emit_m4(const std::string& out_dir, const std::string& run_path,
             const std::string& generated_by);

}  // namespace at::emit
