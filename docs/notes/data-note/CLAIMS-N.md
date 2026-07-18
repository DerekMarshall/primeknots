# CLAIMS-N — data-note claims ledger

Provenance for every nontrivial claim in `data-note.md`. One row per `<!-- claim:ID -->` marker
in the note. **Source** is a paper locus, one of our emitted/committed artifacts, or a test /
rule / ledger row / PR commit hash — never memory (rule 8; draft rule R3). Same discipline as
`docs/explainers/CLAIMS-E.md` and `docs/deck/CLAIMS.md`.

The checker verifies **existence** (every marker in the prose has a row here). **Content
accuracy** — that each row faithfully reflects its source, as an understatement or special case
and never an inversion (register discipline) — is a referee-level read, exactly as CLAIMS-E and
the deck ledger are refereed, not machine-checked.

Papers (pinned, `docs/papers/`): **HLOP** = He–Lee–Oliver–Pozdnyakov, arXiv:2204.10140;
**SS25** = Sawin–Sutherland, arXiv:2504.12295. PR docs: `docs/preregistered/PR-{1,2,3}.md`.

**Citation convention (this ledger).** Pre-registration is cited as a **(pre-registered → read)**
commit pair — the commit that fixed the rule *before* the data, then the commit that recorded the
result. Every §3/§4 results and mechanism number is traced to a **committed artifact by filename**.
Verbatim SS25 quotes (eq (1)/(2)) are transcription-checked against the note's rendering in the
"SS25 transcription check" section below (character level; register rule 1 — faithful, an
understatement or special case, never an inversion).

Status: **§1–§4 rows complete; §5–§6 + Abstract [PENDING]** (drafted per the ENDGAME sequence;
§3/§4 land at CHECKPOINT A for the referee pass).

## §1 — Introduction

| ID | Claim | Source |
|----|-------|--------|
| N1-1 | Murmurations = oscillating correlation between `a_p(E)` and `p`; discovered by HLOP (2022), since observed across several settings and proved in the first (Zubrilina's weight-2 density) | HLOP arXiv:2204.10140 (abstract, p.3 "never reported"); Z25 arXiv:2310.07681 Thm 1 (proved, weight-2); docs/explainers/E1-murmurations.md (E1-1, E1-21, E1-24) |
| N1-2 | SS25 Conjecture 1: `X→∞` limit of statistic (1) = `∫ D(u) du`, `D(u)` the Bessel-`J₁` double sum of eq (2) with local factors `ℓ_{p,ν}, ℓ̂_{p,ν}` (Lemmas 3–4) | SS25 Conjecture 1 / eq (2), p.2 (quoted, m4-pinning.md §P3 lines 110–120); Lemmas 3–4 pp.4–5; src/murm/ss_density.h |
| N1-3 | SS25 Theorem 2: proven variant — prime sum → sum over integers with no prime factor `≤P` against a smooth weight; same integrand; `P→∞` limit established | SS25 Theorem 2, p.2 (quoted, m4-pinning.md §P3 line 156–158) |
| N1-4 | The prime-sum height murmuration density is CONJECTURAL; Theorem 2 is the rigorous backdrop, not the compared object | m4-pinning.md §P3 (lines 40, 160–161); SS25 (Conjecture 1 vs Theorem 2); CLAIMS-E E1-26 |
| N1-5 | "Verified" = numerical over a stated range, twin/anchor/oracle-checked; never a formal proof | CLAUDE.md rule 7 / RESEARCH-M §0 rule 7 |
| N1-6 | We compute statistic (1) over `X = 10⁴, 2¹⁶, 2¹⁷`, bin by `u=p/N`, compare to `D(u)` via pre-fixed shape invariants (hump/zero/trough) | docs/preregistered/PR-1.md (commit fa4e35a); src/murm/ss_empirical.h:17–19; data/m5/ss_x{65536,131072}.txt |
| N1-7 | No asymptotic claim; our range is at the bottom of SS25's observed-decay window (naive height `2¹⁶–2²⁸`); "persistent" = over our finite range | SS25 (decay window, p.1); docs/preregistered/PR-1.md (scale caveat, commit fa4e35a) |

## §2 — Methods

| ID | Claim | Source |
|----|-------|--------|
| N2-1 | Family `E_{A,B}: y²=x³+Ax+B`, reduced (no `p`: `p⁴|A ∧ p⁶|B`) + nonsingular (`4A³+27B²≠0`), ordered by `H(E)=max(4|A|³,27B²)` (SS25 p.1 verbatim) | SS25 p.1 (verbatim "do NOT vary"); src/murm/height_family.h:6–20 |
| N2-2 | Family enumerated over box `|A|≤(X/4)^{1/3}, |B|≤(X/27)^{1/2}`; count certified by an independent sieve agreeing at every cutoff | src/murm/height_family.h:22–30; test `height_family_count_sieve` (verify/, m4) |
| N2-3 | Statistic (1): `D̂(u_b) = (Δu·|fam|)⁻¹ Σ_{(E,p):p/N∈bin_b} (u_mid·lnN/N)·ε·a_p`; `Δu=0.025`, 40 bins | SS25 eq (1); src/murm/ss_empirical.h:17–19; data/m5/ss_partials_x65536.txt header (`du 0.025`, `NB 40`) |
| N2-4 | Good primes `p>3`, `p ∤ (4A³+27B²)`, `p ≤ N` (`u≤1`); reduced short model `p`-minimal at `p>3` ⇒ `p∤Δ(model) ⟺ p∤N` | src/murm/ss_empirical.h:23–25; src/murm/ss_empirical.cpp:64–71 |
| N2-5 | `a_p = p+1−#E(𝔽_p)` computed from scratch; `N, ε` oracle-provenance INPUT (PARI), every column provenance-labelled | RESEARCH-M §0 rule 6 (provenance); src/murm/ne_cache.h (header comment); data/m5/ne_cache_x65536.txt header |
| N2-6 | Oracle `N/ε` certified before use by a dual-oracle overlap check vs LMFDB/Cremona | src/murm/ne_cache.h (dual-oracle overlap note); test `oracle_dual_overlap_NE` (verify/m4) |
| N2-7 | Load-bearing functions dual-implemented; `a_p` by `ap_fast` twinned vs frozen `ap_charsum`, exact on a certified sample | CLAUDE.md rule 2; test `twin_ap_fast_vs_charsum_m4`; CLAIMS-E E3-1, E3-2 |
| N2-8 | Anchors pinned; oracles referee-only (never in `src/`); absent oracle SKIPs visibly | CLAUDE.md rules 1, 3; ctest `SKIP_RETURN_CODE 77` (verify/CMakeLists.txt); CLAIMS-E E3-4, E3-5, E3-7 |
| N2-9 | Full method in the E3 method explainer | docs/explainers/E3-method.md; docs/explainers/CLAIMS-E.md (E3-*) |
| N2-10 | Commit-before-run pre-registration, cited as (pre-registered → read) pairs: PR-1 `dd6beb0` + Rung-3 clause `4a17ebe` (before 2¹⁷ read) → `8f64ba1`; PR-2 `f7415a4` (threshold, before split) → `876999f`; PR-3 `21060a0` → `0d21b62` | RESEARCH-M §7; docs/preregistered/PR-{1,2,3}.md; `git log` of each PR doc (pairs verified: rule-introducing commit precedes its read) |
| N2-11 | Every exploratory statistic logged in the looks ledger | docs/preregistered/LOOKS.md; RESEARCH-M §7 |
| N2-12 | `D(u)` evaluated in a separate TU: Bessel-`J₁` double sum, Lemmas 3–4 local factors, in-house `J₁`, generated (not typed) constants, truncated `q,m ≤ 2000` | src/murm/ss_density.h; m4-pinning.md §P3; src/emit/emit_sawin_sutherland.cpp (`kDensB = 2000`); CLAUDE.md rule 8 |
| N2-13 | Emitted density byte-portable across compilers/platforms (verified) | verify/freshness_check.py; memory `cross-compiler-emit-determinism` (`-ffp-contract=off`); CLAIMS-E E3-17, E3-18 |
| N2-14 | `a_p` by three independent algorithms (`ap_charsum`, `ap_fast`, Shanks–Mestre) agreeing exactly over the full 2¹⁶ **and 2¹⁷** grids (112 M + 385 M, 0 mismatches) at ~145× less CPU; at 2¹⁸ a tail-weighted 27.9 M-value sample + PARI `ellap` spot (204 pairs, p≤6.9 M) — the production-capability gate; cross-algorithm, a_p platform-independent integer | CLAIMS-E E3-2b; docs/notes/m0b-pinning.md §7 (ladder + PRODUCTION-CAPABLE); tests `twin_m0b_vs_charsum_x16/x17` ✓ (fbe51a5), `twin_m0b_bruteforce_x18_tailweighted` + `oracle_ellap_m0b_spot` ✓ (69c17d5) |

## §3 — Results

| ID | Claim | Source |
|----|-------|--------|
| N3-1 | Four-rung ladder: trough at u=0.8875, d=0.0825 frozen at 10⁴/2¹⁶/2¹⁷/2¹⁸; \|fam\| 1048/5042/9014/15936 | data/m4/ss_empirical.txt; data/m5/ss_x{65536,131072,262144}.txt (`shape` rows) |
| N3-2 | hump dev 0.0125 + first-zero dev ≤0.0282 within τ=0.06; trough dev 0.0825 > τ (open deviation) | same ss_x*.txt `shape` rows; τ=0.06 committed (PR-1 R0c; ss_x*.txt `# tol`) |
| N3-3 | Verdict H0 (persistent, ≤2¹⁸): d flat 0.0825 at 2¹⁶/2¹⁷/2¹⁸, no ≥Δu recovery at either step; Rung-3 clause quoted verbatim | docs/preregistered/PR-1.md §"Three completed rungs" (clause 4a17ebe) + Rung-3 postscript (8f64ba1) |
| N3-4 | Supporting: first-zero series 0.672894/0.670328/0.673202/0.671945 flat/non-monotone (direction only, not the gate) | ss_x*.txt `shape` rows (zero_u) |
| N3-5 | Supporting: trough depth eases −3.715/−3.652/−3.580 (2¹⁶→2¹⁷→2¹⁸) while position holds 0.8875 | data/m5/ss_x{65536,131072,262144}.txt `shape` rows (trough_v) |

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

- **eq (1) — statistic.** SS25 p.2, quoted in m4-pinning §P2:
  `E_{E:H(E)≤X}[ (log N(E)^{(C₁+C₂)/2}/N(E)) · Σ_{p∈(C₁N,C₂N)} ε(E)·a_p(E) ]`. Note (§2) renders it
  as a binned density: window (C₁,C₂) = the bin (bΔu,(b+1)Δu], so (C₁+C₂)/2 = u_mid, ÷Δu.
  **u_mid vs per-pair-u — resolved against `src/murm/ss_empirical.cpp:69–76`:** the weight uses the
  bin-midpoint `u_mid=(b+½)Δu`, which IS eq (1)'s `(C₁+C₂)/2` exponent, NOT the per-pair `u=p/N`;
  so `D̂` is a faithful **binned density estimator** of (1), not a re-definition. No discrepancy;
  prose aligned ("a binned estimator of").
- **eq (2) — density.** SS25 eq (2) p.2, quoted in m4-pinning §P3: the
  `2π√u · Σ_q Σ_m [μ(gcd(m,q))/(q·m·φ(q/gcd(m,q)))] · J₁(4π√u·m/q) · ∏_{p|m,p∤q} ℓ̂_{p,2v_p(m)} ·
  ∏_{p|q} ℓ_{p,v_p(m)}` double sum. Note (§1) renders the integrand `D(u)` character-identically;
  local factors ℓ,ℓ̂ per Lemmas 3–4 (`src/murm/ss_density.h`). No discrepancy.

## §5–§6 + Abstract — [PENDING]
*(ENDGAME Phase 2: §5 Reproducibility, §6 Discussion, Abstract last.)*
