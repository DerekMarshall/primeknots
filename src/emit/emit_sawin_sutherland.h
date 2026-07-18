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

// Stage M5 / PR-1 X-extension emitter (schema ss_x_extension_murmuration/1). run_path is
// the VERDICT rung data/m5/ss_x262144.txt (the 2¹⁸, M0b-produced run); the emitter also
// reads its 2¹⁶/2¹⁷ siblings (ss_x65536.txt, ss_x131072.txt) from the same directory to
// assemble PR-1's four-rung ladder {10⁴,2¹⁶,2¹⁷,2¹⁸}. A stale/missing sibling fails the
// emit loudly (each read_ss_run refuses a stale run) — no silent partial ladder (rule 5).
// The emitter APPLIES PR-1's committed Rung-3 decision rule to the four rungs' trough
// deviations and DERIVES the finite-range H1/H0 verdict from the data (rule 1); claim_class
// + ladder_verdict carry it IN WORDS: partial agreement — hump + first zero within τ=0.06
// at every rung, the trough a PERSISTENT open deviation (u=0.8875, dev 0.0825 flat over a
// 26× X span), so the verdict is H0 (persistent, ≤2¹⁸) — a finite-range statement, NOT the
// X→∞ verdict [SS25] describe. D(u) recomputed cheaply and byte-portably, as in emit_m4.
void emit_m5_extension(const std::string& out_dir, const std::string& run_path,
                       const std::string& generated_by);

// Stage M5 / PR-2 step 3 emitter — the analytic-rank split (schema ss_rank_split/1).
// Reads the committed partials + analytic-rank cache, re-aggregates (murm::rank_split, NO
// a_p recompute) the full family, F∖S₂, S₂, and S₀, overlays the conjectured D(u) and the
// R2 targets, and records the committed gates + the branch taken (PRIMARY u-space recovery,
// SECONDARY value-space contrast vs 0.668) IN WORDS in claim_class + the Wachs clause (R1).
void emit_m5_rank_split(const std::string& out_dir, const std::string& partials_path,
                        const std::string& rank_path, const std::string& generated_by);

}  // namespace at::emit
