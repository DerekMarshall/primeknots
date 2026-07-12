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

// Stage M5 / PR-1 X-extension emitter. Same schema (sawin_sutherland_murmuration/1),
// consuming the committed extension run data/m5/ss_x65536.txt (full ladder incl. the
// 2¹⁶ rung + the 2¹⁶ confirm curve). claim_class carries the PR-1 single-rung verdict IN
// WORDS: partial agreement — trough PERSISTENT at 2¹⁶ (dev identical to 10⁴, flat over
// 6.5× X) → "consistent with persistent, proceed to PR-2" (evidence-grade, NOT the
// finite-X verdict); the interpolated zero-crossing retreated (sub-bin, reported, not the
// gate). D(u) recomputed cheaply and byte-portably, as in emit_m4.
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
