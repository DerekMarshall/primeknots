# CLAIMS-N — data-note claims ledger

Provenance for every nontrivial claim in `data-note.md`. One row per `<!-- claim:ID -->` marker
in the note. The `Source` column is a paper locus, one of our emitted or committed artifacts, or a
test, rule, ledger row, or PR commit hash, never memory (rule 8; draft rule R3). Same discipline as
`docs/explainers/CLAIMS-E.md` and `docs/deck/CLAIMS.md`.

The checker verifies existence: every marker in the prose has a row here. Content accuracy, that
each row faithfully reflects its source (an understatement or special case, never an inversion,
per register discipline), is a referee-level read, exactly as CLAIMS-E and the deck ledger are
refereed, not machine-checked.

Papers (pinned, `docs/papers/`): **HLOP** = He–Lee–Oliver–Pozdnyakov, arXiv:2204.10140;
**SS25** = Sawin–Sutherland, arXiv:2504.12295. PR docs: `docs/preregistered/PR-{1,2,3}.md`.

**Citation convention (this ledger).** Pre-registration is cited as a (pre-registered → read)
commit pair: the commit that fixed the rule *before* the data, then the commit that recorded the
result. Every §3/§4 results and mechanism number is traced to a committed artifact by filename.
Verbatim SS25 quotes (eq (1)/(2)) are transcription-checked against the note's rendering in the
"SS25 transcription check" section below (character level; register rule 1, faithful, an
understatement or special case, never an inversion).

**Reframe note (2026-07-19, referee C / ERRATA #28).** The eq (2) transcription fix moved the
density trough target 0.805 → 0.870 and dissolved the "persistent trough displacement." §3/§4/§6 and
the Abstract were reframed accordingly; `docs/notes/reframe-registry.md` is the propagation map
(canonical wording S1–S5, location registry). Numbers here are from the corrected density (targets
0.465/0.671/0.870) and the committed runs.

Status: all rows complete — §1–§6 (47 `claim:Nx-y` markers, marker↔row parity green) plus the
Abstract-summary note; finalized pending Derek's Checkpoint-B read.

## §1 — Introduction

| ID | Claim | Source |
|----|-------|--------|
| N1-1 | Murmurations = oscillating correlation between `a_p(E)` and `p`; discovered by HLOP (2022), since observed across several settings and proved in the first (Zubrilina's weight-2 density) | HLOP arXiv:2204.10140 (abstract, p.3 "never reported"); Z25 arXiv:2310.07681 Thm 1 (proved, weight-2); docs/explainers/E1-murmurations.md (E1-1, E1-21, E1-24) |
| N1-2 | SS25 Conjecture 1: `X→∞` limit of statistic (1) = `∫ D(u) du`, `D(u)` the Bessel-`J₁` double sum of eq (2) with local factors `ℓ_{p,ν}` (Lemma 3, positive ν), `ℓ̂_{p,ν}` (Lemma 4, even ν≥0), the products `∏_{p|q} ℓ̂_{p,2v_p(m)} · ∏_{p|m,p∤q} ℓ_{p,2v_p(m)}` | SS25 Conjecture 1 / eq (2), p.2 **re-transcribed from the [SS25] PDF** (the internal m4-pinning §P3 quote carried the #28 error; standing rule: transcription terminates at the source); Lemmas 3–4 pp.4–5; src/murm/ss_density.h (corrected 2026-07-18); **`D(u)` rendered as a LaTeX display tagged (2); char-level re-checked vs SS25 eq (2) p.2 in the CORRECTED form (ℓ̂ on p\|q, ℓ on p\|m,p∤q, both 2v_p(m)); the note flags the earlier misread inline and defers it to §6/#28** |
| N1-3 | SS25 Theorem 2: proven variant — prime sum → sum over integers with no prime factor `≤P` against a smooth weight; same integrand; `P→∞` limit established | SS25 Theorem 2, p.2 (quoted, m4-pinning.md §P3 line 156–158) |
| N1-4 | The prime-sum height murmuration density is CONJECTURAL; Theorem 2 is the rigorous backdrop, not the compared object | m4-pinning.md §P3 (lines 40, 160–161); SS25 (Conjecture 1 vs Theorem 2); CLAIMS-E E1-26 |
| N1-5 | "Verified" = numerical over a stated range, twin/anchor/oracle-checked; never a formal proof | CLAUDE.md rule 7 / RESEARCH-M §0 rule 7 |
| N1-6 | We compute statistic (1) over `X = 10⁴, 2¹⁶, 2¹⁷, 2¹⁸`, bin by `u=p/N`, compare to `D(u)` via pre-fixed shape invariants (hump/zero/trough) | docs/preregistered/PR-1.md (pre-registered dd6beb0 → read 8f64ba1); src/murm/ss_empirical.h:17–19; data/m4/ss_empirical.txt + data/m5/ss_x{65536,131072,262144}.txt |
| N1-7 | No asymptotic claim; our range (≤2¹⁸) is at the bottom of SS25's observed-decay window (naive height `2¹⁶–2²⁸`); "persistent" = over our finite range | SS25 (decay window, p.1); docs/preregistered/PR-1.md (scale caveat, pre-registered dd6beb0 → read 8f64ba1) |

## §2 — Methods

| ID | Claim | Source |
|----|-------|--------|
| N2-1 | Family `E_{A,B}: y²=x³+Ax+B`, reduced (no `p`: `p⁴\|A ∧ p⁶\|B`) + nonsingular (`4A³+27B²≠0`, the discriminant, no abs), ordered by `H(E)=max(4\|A\|³,27\|B\|²)` (SS25 p.1 verbatim; note renders `\max(4\lvert A\rvert^3,27\lvert B\rvert^2)`) | SS25 p.1 verbatim; **char-level re-check 2026-07-18: prior draft had `27B²`, corrected to the source's `27\|B\|²` (equal value, but "verbatim" now holds)**; src/murm/height_family.h:6–20 |
| N2-2 | Family enumerated over box `|A|≤(X/4)^{1/3}, |B|≤(X/27)^{1/2}`; count certified by an independent sieve agreeing at every cutoff | src/murm/height_family.h:22–30; test `height_family_count_sieve` (verify/, m4) |
| N2-3 | Statistic (1): `D̂(u_b) = (Δu·\|fam\|)⁻¹ Σ_{(E,p):p/N∈bin_b} (u_mid·lnN/N)·ε·a_p`; `Δu=0.025`, 40 bins | SS25 eq (1); src/murm/ss_empirical.h:17–19; data/m5/ss_partials_x65536.txt header (`du 0.025`, `NB 40`); **eq (1) rendered with the window as `p/N(E)∈(C₁,C₂)` — equivalent to the paper's `p∈(C₁N(E),C₂N(E)]` (`p∈(C₁N,C₂N] ⟺ p/N∈(C₁,C₂]`), the compact form matching the note's `u=p/N` binning; exponent inside the log `log(N(E)^{(C₁+C₂)/2})` unchanged; display tagged (1) per [SS25]. `D̂` is our binned estimator of it, unnumbered. Char-level re-checked vs SS25 eq (1) p.2** |
| N2-4 | Good primes `p>3`, `p ∤ (4A³+27B²)`, `p ≤ N` (`u≤1`); reduced short model `p`-minimal at `p>3` ⇒ `p∤Δ(model) ⟺ p∤N` | src/murm/ss_empirical.h:23–25; src/murm/ss_empirical.cpp:64–77 |
| N2-5 | `a_p = p+1−#E(𝔽_p)` computed from scratch; `N, ε` oracle-provenance INPUT (PARI), every column provenance-labelled | RESEARCH-M §0 rule 6 (provenance); src/murm/ne_cache.h (header comment); data/m5/ne_cache_x65536.txt header |
| N2-6 | Oracle `N/ε` certified before use by a dual-oracle overlap check vs LMFDB/Cremona | src/murm/ne_cache.h (dual-oracle overlap note); test `oracle_dual_overlap_NE` (verify/m4) |
| N2-7 | Functions the result depends on are dual-implemented; `a_p` by `ap_fast` twinned vs frozen `ap_charsum`, exact on a certified sample | CLAUDE.md rule 2; test `twin_ap_fast_vs_charsum_m4`; CLAIMS-E E3-1, E3-2 |
| N2-8 | Anchors pinned; oracles referee-only (never in `src/`); absent oracle SKIPs visibly | CLAUDE.md rules 1, 3; ctest `SKIP_RETURN_CODE 77` (verify/CMakeLists.txt); CLAIMS-E E3-4, E3-5, E3-7 |
| N2-9 | Full method in the E3 method explainer | docs/explainers/E3-method.md; docs/explainers/CLAIMS-E.md (E3-*) |
| N2-10 | Commit-before-run pre-registration, cited as (pre-registered → read) pairs: PR-1 `dd6beb0` + Rung-3 clause `4a17ebe` (before 2¹⁷ read) → `8f64ba1`; PR-2 `f7415a4` (threshold, before split) → `876999f`; PR-3 `21060a0` → `0d21b62` | RESEARCH-M §7; docs/preregistered/PR-{1,2,3}.md; `git log` of each PR doc (pairs verified: rule-introducing commit precedes its read) |
| N2-11 | Every exploratory statistic logged in the looks ledger | docs/preregistered/LOOKS.md; RESEARCH-M §7 |
| N2-12 | `D(u)` evaluated in a separate TU: Bessel-`J₁` double sum, Lemmas 3–4 local factors, in-house `J₁`, generated (not typed) constants, truncated `q,m ≤ 2000` | src/murm/ss_density.h; m4-pinning.md §P3; src/emit/emit_sawin_sutherland.cpp (`kDensB = 2000`); CLAUDE.md rule 8 |
| N2-13 | Emitted density byte-portable across compilers/platforms (verified) | verify/freshness_check.py; memory `cross-compiler-emit-determinism` (`-ffp-contract=off`); CLAIMS-E E3-17, E3-18 |
| N2-14 | `a_p` by three independent algorithms (`ap_charsum`, `ap_fast`, Shanks–Mestre) agreeing exactly over the full 2¹⁶ **and 2¹⁷** grids (112 M + 385 M, 0 mismatches) at ~145× less CPU; at 2¹⁸ a tail-weighted 27.9 M-value sample + PARI `ellap` spot (204 pairs, p≤6.9 M) — the production-capability gate; cross-algorithm, a_p platform-independent integer | CLAIMS-E E3-2b; docs/notes/m0b-pinning.md §7 (ladder + PRODUCTION-CAPABLE); tests `twin_m0b_vs_charsum_x16/x17` ✓ (fbe51a5), `twin_m0b_bruteforce_x18_tailweighted` + `oracle_ellap_m0b_spot` ✓ (69c17d5) |
| N2-15 | eq (1) is a definition we evaluate, eq (2)'s equality to the X→∞ limit is the conjecture; D̂ is compared to D(u), reported as empirical agreement, never a proof | SS25 Conjecture 1 / eq (2), p.2; m4-pinning.md §P3; CLAUDE.md rule 7 (no proof claimed) |
| N2-16 | `D(u)`'s reading of eq (2) is transcribed from the [SS25] PDF, not an internal quote — the standing rule that a transcription check terminates at the source artifact, adopted after ERRATA #28 | docs/ERRATA.md #28 (durable fix); CLAUDE.md (no-fabricated-citations / transcription-at-source); src/murm/ss_density.h (corrected reading, 2026-07-18) |
| N2-17 | The local factors' level-1 Hecke eigenvalue sums `Σ_f a_p(f)=Tr(T_p\|S_{ν+2})` computed by a from-scratch Eichler–Selberg trace reusing M3 Hurwitz (rule 3: no oracle values in core), anchored on `Tr(T_p\|S₁₂)=τ(p)`; density checked term-level (m=32 via Tr T₂, m=3⁵, gcd(m,q)>1 μ-cases) and curve-level; C++ + PARI oracle are two codings of ONE reading (agreement alone insufficient — #25 class), external referee = authors' figure (trough ~0.87), reconciled at #28 | src/murm/ss_density.cpp (`level1_hecke_trace`, Eichler–Selberg, `#include mform/hurwitz.h`); verify/m4/ss_density_tests.cpp (`anchor_hecke_trace_eichler_selberg`, `anchor_ss_density_survivor_terms`, `twin_ss_density_vs_term_sum`); oracle/ss_density_oracle.gp; docs/ERRATA.md #28, #29 |

## §3 — Results

| ID | Claim | Source |
|----|-------|--------|
| N3-1 | Four-rung ladder, binned: hump u=0.4625 and trough u=0.8875 stable at every rung; first zero ≈0.671; depth −3.47/−3.72/−3.65/−3.58; \|fam\| 1048/5042/9014/15936; all three invariants agree with D(u) within τ=0.06 | data/m4/ss_empirical.txt; data/m5/ss_x{65536,131072,262144}.txt (`shape` rows); τ=0.06 committed (`# tol`); visualized in **Fig 1** (four-rung overlay, `figures/make_figures.py` → `figA_ss_overlay.pdf`) |
| N3-2 | Both-estimators table at the 2¹⁸ verdict rung, vs corrected targets 0.465/0.671/0.870: hump binned 0.4625 / interp 0.47064 / CI [0.46699,0.47696] / dev +0.0056; zero 0.671945 / 0.67194 / [0.66567,0.67715] / +0.0009; trough 0.8875 / 0.88216 / [0.87611,0.88567] / +0.0122. Deviation = interp − target; residuals ride the interpolated column | data/m5/ss_x262144.txt (`shape` binned); `docs/notes/referee_b2b3.py` (parabolic interp + bootstrap 95% CI, N_boot=2000, seed 20260718, reproduce gate OK); corrected targets: `# r2` headers + emitted `sawin_sutherland_murmuration.json` `targets` + `at ss-density-scan` (0.465/0.671/0.870) |
| N3-3 | Uniform reading (S1): all three within τ=0.06; zero ON target (CI [0.666,0.677] ∋ 0.671); hump + trough small resolved residuals ≈0.006/≈0.012, CIs exclude targets marginally (hump lb 0.467>0.465) / clearly (trough lb 0.876>0.870); largest residual ≈0.012 at trough ≈0.2τ | `docs/notes/referee_b2b3.py` (2¹⁸ CIs); `docs/notes/reframe-registry.md` S1; τ=0.06 (`# tol`) |
| N3-4 | PR-1 verdict SUPERSEDED, void as pronounced (S3): rule committed (dd6beb0; Rung-3 clause 4a17ebe before 2¹⁷ read), followed verbatim (d=0.0825 flat at 2¹⁶/2¹⁷/2¹⁸), H0 pronounced (8f64ba1) against a 0.805 target ERRATA #28 found corrupt; NOT re-run against 0.870 (never registered against it); corrected comparison is descriptive | docs/preregistered/PR-1.md §"Rung-3" (clause 4a17ebe, read 8f64ba1); docs/ERRATA.md #28; docs/notes/reframe-registry.md S3 |
| N3-5 | Supporting (not a gate): trough depth eases −3.72→−3.65→−3.58 (2¹⁶→2¹⁷→2¹⁸), 10⁴ depth −3.47 shallower ⇒ three-rung not monotone four-rung; has the sign of large-X amplitude decay but bootstrap depth CIs overlap heavily ⇒ point-estimate trend within sampling noise, not resolved | data/m4/ss_empirical.txt + data/m5/ss_x*.txt `shape` rows (trough_v); `docs/notes/referee_b2b3.py` B3 depth CIs (overlap across rungs) |

## §4 — Mechanism constraints

| ID | Claim | Source |
|----|-------|--------|
| N4-1 | PR-2 (measurement stands; coded verdict superseded): excising 738 analytic-rank-2 curves leaves the trough POSITION at u=0.8875 (full family + geometric-holdout annulus (10⁴,2¹⁶]) — a standing bound; the recovery-gate VERDICT (recover toward 0.805) is superseded (gated the corrupt target, ERRATA #28), the leave-out measurement untouched; secondary value-space contrast (rank-2 descending-branch gap −1.73, past committed −0.668) significant but too dilute to move position | docs/preregistered/PR-2.md §step-3 (f7415a4 → 876999f); viz/data/ss_rank_split_murmuration.json (`leaveout_trough_u` 0.8875, gap −1.7326, `contrast_threshold` −0.668, `verdict`/`branch` reframed); docs/ERRATA.md #28 |
| N4-2 | PR-3 four-rung leakage bound, REGENERATED vs corrected density: departure dep=S(u*)−D(u*) (D(u*)=−2.94 at 2¹⁸); \|f\|≤0.5% opposite-signed at 2¹⁶/2¹⁷/2¹⁸ (−0.005/−0.005/−0.005), +3.5% same-signed at 10⁴ (smallest family, largest \|δ\|, no bootstrap); non-growing; f≈1 needs unsigned bias ~2 orders larger; a bound, not an explanation | docs/preregistered/PR-3.md postscript (21060a0 → 0d21b62; 2¹⁸ row 8f64ba1) + SUPERSEDED/#28 postscript; `at ss-leakage --ladder 10000,65536,131072,262144` over data/m5/ss_partials_x*.txt (δ/L/dep/f table, corrected `target_trough` 0.870 app/at.cpp); docs/ERRATA.md #28 |
| N4-3 | D1 (congruence stratification) DIED-unrun per PR-3's pre-committed trigger (D2 bounds it out) | docs/preregistered/PR-3.md §"Trigger" + "D1 recorded DIED" |
| N4-4 | Wachs clause (verbatim): rank not distinguished from correlated BSD invariants (Tamagawa, \|Ш\|, real period) | docs/preregistered/PR-2.md + PR-3.md (Wachs 2603.04604), verbatim as committed |
| N4-5 | Sutherland-bias attribution (verbatim): unsigned bias + named classes + cancellation are Sutherland's; PR-3 tests only the leakage relationship | docs/preregistered/PR-3.md §0a + "Every branch carries"; A. Sutherland Sept-2023 talk |

## SS25 transcription check (updated 2026-07-19, ERRATA #28)

The two SS25 formulas the note renders, quoted from cited loci beside the note's rendering,
character-checked so a skeptical [SS25] reader confirms fidelity (register rule 1).

- **eq (1): statistic.** SS25 p.2, quoted in m4-pinning §P2:
  `E_{E:H(E)≤X}[ (log N(E)^{(C₁+C₂)/2}/N(E)) · Σ_{p∈(C₁N,C₂N]} ε(E)·a_p(E) ]`. The interval is
  **half-open (C₁N, C₂N]** (left-open, right-closed, p.2 — ERRATA #30). Note (§2) renders it as a
  binned density: window (C₁,C₂] = the bin (bΔu,(b+1)Δu], so (C₁+C₂)/2 = u_mid, ÷Δu.
  **u_mid vs per-pair-u, resolved against `src/murm/ss_empirical.cpp`:** the weight uses the
  bin-midpoint `u_mid=(b+½)Δu`, which is eq (1)'s `(C₁+C₂)/2` exponent, not the per-pair `u=p/N`,
  so `D̂` is a binned density estimator of (1), not a re-definition. No discrepancy.
- **eq (2): density.** SS25 eq (2) p.2, **re-transcribed from the [SS25] PDF** (not the internal
  m4-pinning §P3 quote, which carried the #28 error): the
  `2π√u · Σ_q Σ_m [μ(gcd(m,q))/(q·m·φ(q/gcd(m,q)))] · J₁(4π√u·m/q) · ∏_{p|q} ℓ̂_{p,2v_p(m)} ·
  ∏_{p|m,p∤q} ℓ_{p,2v_p(m)}` double sum — **ℓ̂ over p|q, ℓ over p|m,p∤q, both exponent 2v_p(m)**
  (only even ν appear, [SS25] p.9; the ν-domains of Lemmas 3/4 force it). Note (§1) renders the
  integrand `D(u)` character-identically to this CORRECTED form; local factors ℓ,ℓ̂ per Lemmas 3–4
  (`src/murm/ss_density.h`, corrected 2026-07-18).
  **ERRATA #28 (flag, don't smooth):** a prior transcription check (B2, 2026-07-17) certified the
  SWAPPED products (ℓ̂ on p|m,p∤q; ℓ on p|q; exponent v_p) as "matching — no discrepancy." That
  check was itself reading off the corrupted *internal quote*, not the source PDF, so it could not
  catch the error (twin-blindness, #25 class); the authors' published figure did. This row now
  reflects the source, and the standing rule is that a transcription check terminates at the paper
  PDF (N2-16).

## §5 — Reproducibility

| ID | Claim | Source |
|----|-------|--------|
| N5-1 | Emitted density JSON byte-identical across compilers/platforms; the density evaluator + emitter are built `-ffp-contract=off`; a freshness check re-emits at the commit and requires byte-equality on CI GCC + local. The comparison first NORMALIZES one field (the `generated_by` git-describe stamp, which legitimately varies by checkout), then requires every remaining byte to match; "byte-identical" = identical after that single-field normalization | src/CMakeLists.txt:163–173 (`ss_density.cpp`, `emit_sawin_sutherland.cpp`); verify/freshness_check.py; CLAIMS-E E3-17/E3-18; N2-13 |
| N5-2 | Integer a_p platform-independent: pre-registered tail-weighted sample of 79,268 (curve,prime) pairs (p≤4.15M) via `ap_charsum` on laptop `g++-16`/macOS + FreeBSD `clang 21.1.8`/15.1 is byte-identical, 0 mismatches; primary evidence = 3-algorithm exact agreement over full 2¹⁶/2¹⁷ grids + byte-identical emit | docs/notes/libm-partial-diff-spec.md (Q1, sha e7ba27d7, `at ap-sample`); tests `twin_m0b_vs_charsum_x16/x17`; N2-14 |
| N5-3 | Raw partials NOT `-ffp-contract=off` (`ss_empirical.cpp` outside the list) ⇒ cross-toolchain float drift, max 1.887e-15 on the shared 8640-curve 2¹⁷ set; does not reach the published curve — byte-identity is established empirically by the freshness check, and the 12-significant-figure emit precision (~3 orders coarser than the drift) explains why | docs/notes/libm-partial-diff-spec.md (Q2); src/CMakeLists.txt:54–55 vs 163–173; src/emit/emit_sawin_sutherland.cpp:29 (`setprecision(12)`); docs/notes/libm_float_diff.py |
| N5-4 | Corrections logged as numbered errata (RESEARCH-M + pinning notes); note inputs committed (code, N/ε caches, partials, `at ap-sample`); the one non-regenerable input — the abandoned laptop 2¹⁷ partials ckpt (the float-drift artifact) — is FORCE-committed via a scoped `.gitignore` exception; a_p caches + sample outputs gitignored, regenerable (`at ap-cache`/`at ap-sample`), SHA-256-pinned | docs/RESEARCH-M.md (ERRATA); .gitignore (`!data/m5/ss_partials_x131072.txt.ckpt`); docs/notes/libm-partial-diff-spec.md (Step-0 manifest) |
| N5-5 | How to cite/verify: repo URL + release tag [PLACEHOLDER] + Zenodo DOI [PLACEHOLDER]; one-command reproduction (cmake + `ctest -L m5 -LE heavy` + `at emit` + figures; matplotlib prereq for figures only); the heavy 2¹⁸ gate (`m5gate.twin_m0b_bruteforce_x18`, ~1–2 h on the box) is run separately via `-L heavy`; cache-regen path (`at ap-cache`, hours on the box) + run/partials (`at ss-run`) + referee-round grid tools (`ss-density-scan`, `referee_b2b3.py`) documented; a SINGLE-platform `at ap-sample` verifies the committed sample vs its pinned SHA-256, and byte-identity of two single-platform runs is the cross-platform evidence | data-note/README.md; docs/notes/libm-partial-diff-spec.md; docs/notes/referee-round-2026-07.md; venue amendment (Derek 2026-07-17) |
| N5-6 | eq (1)'s prime interval is half-open (C₁N,C₂N]; our binning had used the opposite (left-closed) convention (ERRATA #30); committed grids pre-date the fix; only 2–3 conductor-40 curves/rung (good primes on a bin right edge, all low-u) are affected; no reported number changes; a from-scratch regen would differ at exactly those pairs (documented, not latent); recompute deferred (heavy, no scientific effect) | docs/notes/eq1-interval.md; docs/ERRATA.md #30; src/murm/ss_empirical.cpp:69–77 (right-closed integer bin) |

## §6 — Discussion

| ID | Claim | Source |
|----|-------|--------|
| N6-1 | Matched-X context: SS25's OWN Table 4 mean per-bin \|empirical − density\| = 1.17/0.91/0.73 at 2¹⁶/2¹⁷/2¹⁸, decaying to ≈0.10 only by 2²⁸; our rungs sit at the bottom of that window; our shape-invariant POSITIONS agree within τ while the trough-bin amplitude departure (≈0.64 at 2¹⁸) is of the same order as SS25's own per-bin figure — positional agreement with an un-converged amplitude, not a discrepancy of the replication | SS25 Table 4 (P=∞ column; docs/notes/referee-round-2026-07.md §A.2); `at ss-leakage` (dep* = S*−D* = −0.641 at 2¹⁸); N4-2 |
| N6-2 | The eq (2) transcription error (ERRATA #28): earlier note reported ≈0.0825 displacement vs a 0.805 target; the two local-factor products were transposed and the p\|q exponent mis-set (v_p → 2v_p), mislocating the density trough 0.87 → 0.805; error SHARED across pinning note + C++ + PARI oracle (one reading, three impls — agreement certified nothing); caught EXTERNALLY by the authors' figure + a blind re-transcription; standing rule = transcription terminates at source | docs/ERRATA.md #28; docs/notes/reframe-registry.md; docs/notes/referee-round-2026-07.md (Step 1 oracle twin-blind; Block C gate; B blind transcription) |
| N6-3 | Rightward-residual observation (S4): the two non-zero position residuals are both rightward (hump +0.006, trough +0.012, interp; zero on target); consistent with a single small horizontal dilation (one deformation, not two offsets) — the finite-height reading; sharpens PR-5 | `docs/notes/referee_b2b3.py` (2¹⁸ interp deviations, both positive); docs/notes/reframe-registry.md S4 |
| N6-4 | PR-4 retired in public: an earlier draft named a pre-registered split by BSD invariant at fixed rank (Tamagawa product, analytic \|Ш\|, real period; Wachs modulation) to find what carried the deficit; with the deficit resolved to the small residual, the motivation is gone and PR-4 is retired, not carried as pending future work | reframe direction (Derek, Block C, 2026-07-19); Wachs arXiv:2603.04604 (context-class); §3 (residual resolved); N6-1 |
| N6-5 | PR-5 re-motivated (S5): the live question is whether the coherent rightward shift decays with X (amplitude easing §3 + positional dilation N6-3 = two faces of finite-height convergence); PR-5 = decision rule on the position+depth trajectory, committed before any rung beyond 2¹⁸; named future work, not run | docs/notes/reframe-registry.md S5; PR-5 = named future work (no written PR); N3-5, N6-3 |
| N6-6 | Scale caveat + Sutherland Sept-2023 precedent: BSD/Mestre–Nagao sum `lim (1/log x) Σ_{p≤x} a_p log p/p = −r+1/2` (Kim–Murty 2023: if the limit exists, it equals this value and RH holds for L(E,s)); large rank ⟹ large conductor; rank-ordering emerges only at very large conductor ⇒ a settled small-conductor picture need not be asymptotic | docs/papers/Sutherland_Slides.pdf ("How rank effects trace distributions" slide + the following rank-stratified plot); N1-7 |
| N6-7 | Register, restated once: the claim is replication and constraint over X≤2¹⁸ (three invariants within τ, small resolved residual ≈0.012 largest at trough, two mechanisms bounded out); NOT an X→∞ claim; nothing bears on that limit or proves anything | §3 (N3-1..N3-4); §4 (N4-1, N4-2); N1-5, N1-7; CLAUDE.md rule 7 |

## Abstract — summary, unmarked

The Abstract is a one-paragraph summary of §3–§6 and introduces no new claim, so it carries no
`claim:` markers of its own (the checker enforces marker→row; its statements are the section
markers): the four-rung ladder and within-τ agreement are N3-1/N3-2/N3-3, the eq (2) transcription
error and correction are N6-2/N1-2, the two mechanism bounds are N4-1/N4-2, the finite-range register
is N1-7/N6-7, and the verification/no-proof summary is N1-5/N2-7/N2-8/N5-1..N5-3.
