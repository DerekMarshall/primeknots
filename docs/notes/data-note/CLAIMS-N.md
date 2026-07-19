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

Status: all rows complete — §1–§6 (41 `claim:Nx-y` markers, marker↔row parity green) plus the
Abstract-summary note; finalized pending Derek's Checkpoint-B read.

## §1 — Introduction

| ID | Claim | Source |
|----|-------|--------|
| N1-1 | Murmurations = oscillating correlation between `a_p(E)` and `p`; discovered by HLOP (2022), since observed across several settings and proved in the first (Zubrilina's weight-2 density) | HLOP arXiv:2204.10140 (abstract, p.3 "never reported"); Z25 arXiv:2310.07681 Thm 1 (proved, weight-2); docs/explainers/E1-murmurations.md (E1-1, E1-21, E1-24) |
| N1-2 | SS25 Conjecture 1: `X→∞` limit of statistic (1) = `∫ D(u) du`, `D(u)` the Bessel-`J₁` double sum of eq (2) with local factors `ℓ_{p,ν}, ℓ̂_{p,ν}` (Lemmas 3–4) | SS25 Conjecture 1 / eq (2), p.2 (quoted, m4-pinning.md §P3 lines 110–120); Lemmas 3–4 pp.4–5; src/murm/ss_density.h; **`D(u)` rendered as a LaTeX display tagged (2) per [SS25], `u=p/N` dropped to prose; char-level re-checked vs SS25 eq (2) p.2 (2026-07-18): matches term-for-term (`2π√u`, the `μ/qmφ` weight, `J₁(4π√u m/q)`, both `ℓ`-products), `φ`=`\varphi`** |
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
| N2-3 | Statistic (1): `D̂(u_b) = (Δu·\|fam\|)⁻¹ Σ_{(E,p):p/N∈bin_b} (u_mid·lnN/N)·ε·a_p`; `Δu=0.025`, 40 bins | SS25 eq (1); src/murm/ss_empirical.h:17–19; data/m5/ss_partials_x65536.txt header (`du 0.025`, `NB 40`); **eq (1) rendered with the window as `p/N(E)∈(C₁,C₂)` — equivalent to the paper's `p∈(C₁N(E),C₂N(E))` (`p∈(C₁N,C₂N) ⟺ p/N∈(C₁,C₂)`), the compact form matching the note's `u=p/N` binning; exponent inside the log `log(N(E)^{(C₁+C₂)/2})` unchanged; display tagged (1) per [SS25]. `D̂` is our binned estimator of it, unnumbered. Char-level re-checked vs SS25 eq (1) p.2 (2026-07-18, after the substack layout change)** |
| N2-4 | Good primes `p>3`, `p ∤ (4A³+27B²)`, `p ≤ N` (`u≤1`); reduced short model `p`-minimal at `p>3` ⇒ `p∤Δ(model) ⟺ p∤N` | src/murm/ss_empirical.h:23–25; src/murm/ss_empirical.cpp:64–71 |
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

## §3 — Results

| ID | Claim | Source |
|----|-------|--------|
| N3-1 | Four-rung ladder: trough at u=0.8875, d=0.0825 frozen at 10⁴/2¹⁶/2¹⁷/2¹⁸; \|fam\| 1048/5042/9014/15936 | data/m4/ss_empirical.txt; data/m5/ss_x{65536,131072,262144}.txt (`shape` rows); visualized in **Fig 1** (four-rung overlay, `figures/make_figures.py` → `figA_ss_overlay.pdf`) |
| N3-2 | hump dev 0.0125 + first-zero dev ≤0.0282 within τ=0.06; trough dev 0.0825 > τ (open deviation) | same ss_x*.txt `shape` rows; τ=0.06 committed (PR-1 R0c; ss_x*.txt `# tol`) |
| N3-3 | Verdict H0 (persistent, ≤2¹⁸): d flat 0.0825 at 2¹⁶/2¹⁷/2¹⁸, no ≥Δu recovery at either step; Rung-3 clause quoted verbatim | docs/preregistered/PR-1.md §"Three completed rungs" (clause 4a17ebe) + Rung-3 postscript (8f64ba1) |
| N3-4 | Supporting: first-zero series 0.672894/0.670328/0.673202/0.671945 flat/non-monotone (direction only, not the gate) | ss_x*.txt `shape` rows (zero_u) |
| N3-5 | Supporting: trough depth eases −3.715/−3.652/−3.580 (2¹⁶→2¹⁷→2¹⁸) while position holds 0.8875; the 10⁴ depth −3.47 is shallower than 2¹⁶, so the easing is a three-rung observation, not a four-rung monotone trend | data/m4/ss_empirical.txt + data/m5/ss_x{65536,131072,262144}.txt `shape` rows (trough_v) |

## §4 — Mechanism constraints

| ID | Claim | Source |
|----|-------|--------|
| N4-1 | PR-2 null: excising analytic-rank-2 does not move the trough (recovery gate <1 bin unmet, family + virgin annulus); deficit not carried by rank-2; secondary contrast significant but too dilute | docs/preregistered/PR-2.md §step-3 postscript (f7415a4 → 876999f); data/m5/rank_cache_x65536.txt |
| N4-2 | PR-3 D2 four-rung bound: \|f\|≤2%, opposite-signed at 2¹⁶/2¹⁷/2¹⁸, non-growing; f≈1 needs unsigned bias ~3 orders larger | docs/preregistered/PR-3.md postscript (21060a0 → 0d21b62; 2¹⁸ row 8f64ba1); `at ss-leakage` over data/m5/ss_partials_x{65536,131072,262144}.txt |
| N4-3 | D1 (congruence stratification) DIED-unrun per PR-3's pre-committed trigger (D2 bounds it out) | docs/preregistered/PR-3.md §"Trigger" + "D1 recorded DIED" |
| N4-4 | Wachs clause (verbatim): rank not distinguished from correlated BSD invariants (Tamagawa, \|Ш\|, real period) | docs/preregistered/PR-2.md + PR-3.md (Wachs 2603.04604), verbatim as committed |
| N4-5 | Sutherland-bias attribution (verbatim): unsigned bias + named classes + cancellation are Sutherland's; PR-3 tests only the leakage relationship | docs/preregistered/PR-3.md §0a + "Every branch carries"; A. Sutherland Sept-2023 talk |

## SS25 transcription check (B2, 2026-07-17)

The two SS25 formulas the note renders, quoted from cited loci beside the note's rendering,
character-checked so a skeptical [SS25] reader confirms fidelity (register rule 1).

- **eq (1): statistic.** SS25 p.2, quoted in m4-pinning §P2:
  `E_{E:H(E)≤X}[ (log N(E)^{(C₁+C₂)/2}/N(E)) · Σ_{p∈(C₁N,C₂N)} ε(E)·a_p(E) ]`. Note (§2) renders it
  as a binned density: window (C₁,C₂) = the bin (bΔu,(b+1)Δu], so (C₁+C₂)/2 = u_mid, ÷Δu.
  **u_mid vs per-pair-u, resolved against `src/murm/ss_empirical.cpp:69–76`:** the weight uses the
  bin-midpoint `u_mid=(b+½)Δu`, which is eq (1)'s `(C₁+C₂)/2` exponent, not the per-pair `u=p/N`,
  so `D̂` is a binned density estimator of (1), not a re-definition. No discrepancy;
  prose aligned ("a binned estimator of").
- **eq (2): density.** SS25 eq (2) p.2, quoted in m4-pinning §P3: the
  `2π√u · Σ_q Σ_m [μ(gcd(m,q))/(q·m·φ(q/gcd(m,q)))] · J₁(4π√u·m/q) · ∏_{p|m,p∤q} ℓ̂_{p,2v_p(m)} ·
  ∏_{p|q} ℓ_{p,v_p(m)}` double sum. Note (§1) renders the integrand `D(u)` character-identically;
  local factors ℓ,ℓ̂ per Lemmas 3–4 (`src/murm/ss_density.h`). No discrepancy.

## §5 — Reproducibility

| ID | Claim | Source |
|----|-------|--------|
| N5-1 | Emitted density JSON byte-identical across compilers/platforms; the density evaluator + emitter are built `-ffp-contract=off`; a freshness check re-emits at the commit and requires byte-equality on CI GCC + local. The comparison first NORMALIZES one field (the `generated_by` git-describe stamp, which legitimately varies by checkout), then requires every remaining byte to match; "byte-identical" = identical after that single-field normalization | src/CMakeLists.txt:163–173 (`ss_density.cpp`, `emit_sawin_sutherland.cpp`); verify/freshness_check.py; CLAIMS-E E3-17/E3-18; N2-13 |
| N5-2 | Integer a_p platform-independent: pre-registered tail-weighted sample of 79,268 (curve,prime) pairs (p≤4.15M) via `ap_charsum` on laptop `g++-16`/macOS + FreeBSD `clang 21.1.8`/15.1 is byte-identical, 0 mismatches; primary evidence = 3-algorithm exact agreement over full 2¹⁶/2¹⁷ grids + byte-identical emit | docs/notes/libm-partial-diff-spec.md (Q1, sha e7ba27d7, `at ap-sample`); tests `twin_m0b_vs_charsum_x16/x17`; N2-14 |
| N5-3 | Raw partials NOT `-ffp-contract=off` (`ss_empirical.cpp` outside the list) ⇒ cross-toolchain float drift, max 1.887e-15 on the shared 8640-curve 2¹⁷ set; does not reach the published curve — byte-identity is established empirically by the freshness check, and the 12-significant-figure emit precision (~3 orders coarser than the drift) explains why | docs/notes/libm-partial-diff-spec.md (Q2); src/CMakeLists.txt:54–55 vs 163–173; src/emit/emit_sawin_sutherland.cpp:29 (`setprecision(12)`); docs/notes/libm_float_diff.py |
| N5-4 | Corrections logged as numbered errata (RESEARCH-M + pinning notes); note inputs committed (code, N/ε caches, partials, `at ap-sample`); the one non-regenerable input — the abandoned laptop 2¹⁷ partials ckpt (the float-drift artifact) — is FORCE-committed via a scoped `.gitignore` exception; a_p caches + sample outputs gitignored, regenerable (`at ap-cache`/`at ap-sample`), SHA-256-pinned | docs/RESEARCH-M.md (ERRATA #NN); .gitignore (`!data/m5/ss_partials_x131072.txt.ckpt`); docs/notes/libm-partial-diff-spec.md (Step-0 manifest) |
| N5-5 | How to cite/verify: repo URL + release tag [PLACEHOLDER] + Zenodo DOI [PLACEHOLDER]; one-command reproduction (cmake + `ctest -L m5 -LE heavy` + `at emit` + figures; matplotlib prereq for figures only); the heavy 2¹⁸ gate (`m5gate.twin_m0b_bruteforce_x18`, ~1–2 h on the box) is run separately via `-L heavy`; cache-regen path (`at ap-cache`, hours on the box) + run/partials (`at ss-run`) + referee-round grid tools (`ss-density-scan`, `referee_b2b3.py`) documented; a SINGLE-platform `at ap-sample` verifies the committed sample vs its pinned SHA-256, and byte-identity of two single-platform runs is the cross-platform evidence | data-note/README.md; docs/notes/libm-partial-diff-spec.md; docs/notes/referee-round-2026-07.md; venue amendment (Derek 2026-07-17) |

## §6 — Discussion

| ID | Claim | Source |
|----|-------|--------|
| N6-1 | §4 mechanisms don't explain the deficit (bounds/nulls); a candidate not isolated = BSD invariants (Tamagawa product, analytic \|Ш\|, real period) correlated with rank, which Wachs shows modulate at fixed rank; separating them = pre-registered PR-4 (split by BSD invariant at fixed rank), named future work, NOT claimed | §4 (N4-1, N4-4); Wachs arXiv:2603.04604 (docs/papers/README context-class); PR-4 = named future work, no written PR |
| N6-2 | Scale caveat + Sutherland Sept-2023 precedent: BSD/Mestre–Nagao sum `lim (1/log x) Σ_{p≤x} a_p log p/p = −r+1/2` (Kim–Murty 2023: if the limit exists, it equals this value and RH holds for L(E,s)); large rank ⟹ large conductor; rank-ordering emerges only at very large conductor ⇒ a settled small-conductor picture need not be asymptotic | docs/papers/Sutherland_Slides.pdf ("How rank effects trace distributions" slide + the following rank-stratified plot); N1-7 |
| N6-3 | Trough depth eases while position holds (§3/N3-5); a pre-registered amplitude-trajectory gate PR-5 (decision rule on the depth trajectory, fixed before data) is the condition for rungs beyond 2¹⁸; named, not run | N3-5; PR-5 = named future work, no written PR |
| N6-4 | Verdict = H0 over the finite range X≤2¹⁸ (persistent there); NOT an X→∞ claim; nothing in the note bears on that limit | §3 (N3-3); N1-7; SS25 (decay window, p.1) |

## Abstract — summary, unmarked

The Abstract is a one-paragraph summary of §3–§6 and introduces no new claim, so it carries no
`claim:` markers of its own (the checker enforces marker→row; its statements are the section
markers): the four-rung ladder and H0 verdict are N3-1/N3-2/N3-3, the two mechanism constraints are
N4-1/N4-2, the finite-range register is N1-7/N6-4, and the verification/no-proof summary is
N1-5/N2-7/N2-8/N5-1..N5-3.
