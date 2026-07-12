# CLAIMS-E — explainer claims ledger

Provenance for every nontrivial claim in `docs/explainers/`. One row per `<!-- claim:ID -->`
marker in E1/E2/E3. **Source** is a paper locus, one of our emitted artifacts, or a test /
rule / ledger row — never memory (register rule 2 + rule 4).

The `explainers` checker verifies **existence** (every marker in the prose has a row here);
**content accuracy** — that each row's claim faithfully reflects its source, as an
understatement or special case and never an inversion (register rule 1) — is a referee-level
read, exactly as `docs/deck/CLAIMS.md` is refereed, not machine-checked.

Papers (pinned, `docs/papers/`): **HLOP** = He–Lee–Oliver–Pozdnyakov, arXiv:2204.10140;
**LOP** = Lee–Oliver–Pozdnyakov, arXiv:2307.00256; **Z25** = Zubrilina, arXiv:2310.07681 =
Invent. math. (2025), DOI 10.1007/s00222-025-01347-8; **SS25** = Sawin–Sutherland,
arXiv:2504.12295.

## E1 — murmurations

| ID | Claim | Source |
|----|-------|--------|
| E1-1 | The name "murmuration" comes from the starling analogy | HLOP title + p.1 caption |
| E1-2 | The naming paper's first page is a starling photograph | HLOP p.1 (photo, W. Baxter, CC BY-SA 2.0) |
| E1-3 | `a_p(E) = p + 1 − #E(F_p)` — the signed deviation from `p+1`, not a raw count | HLOP p.2 (L_p def); verify/m0/ap_tests.cpp |
| E1-4 | `|a_p| ≤ 2√p` (Hasse); large-negative `a_p` ⟺ more points at `p` | HLOP p.2 (a_p def) + Hasse's theorem (m0-pinning) |
| E1-5 | For 11a1, `a₂ = −2`, `a₃ = −1` | test `anchor_ap_small_primes` (verify/m0/ap_tests.cpp; Cremona aplist "11 a") |
| E1-6 | Our from-scratch counter reproduces 11a1's traces vs the reference | test `anchor_ap_11a1` (verify/m0/ap_tests.cpp) |
| E1-7 | Rank = # independent generators of the free part of the Mordell–Weil group | HLOP p.2 (Mordell–Weil) |
| E1-8 | No known general algorithm computes the rank | HLOP p.2 ("no general algorithm to compute the rank") |
| E1-9 | Root number `ε∈{±1}` = functional-equation sign; the theorems weight by it | Z25 Thm 1 (ε(f) weight), Fig 2 "fixed root number"; SS25 eq (1) ε(E) |
| E1-10 | `ε = (−1)^{ord_{s=1}L}`; weak BSD: that order = rank | HLOP p.2 (weak BSD); PR-2 Amendment 1 (ε=(−1)^ord, a theorem) |
| E1-11 | `f_r(n) = (1/#E_r) Σ a_{pₙ}(E)`, average over rank-`r`, conductor `[N₁,N₂]` | HLOP eq (1.1), p.3 |
| E1-12 | `f_0` vs `f_1` (and `f_0` vs `f_2`) trace opposing waves | HLOP p.3 / Fig 1; viz/data/murmuration_curve.json |
| E1-13 | Example-1 class sizes: 4328, 5194 in [7500,10⁴]; 8536, 1380 in [5000,10⁴]; via LMFDB | HLOP Example 1, p.3 |
| E1-14 | The pattern is population-level, not an individual-curve rank readout | HLOP p.3 (averages over classes); abstract |
| E1-15 | Decaying frequency + scale-invariant in `p/N` (converges to a continuous function of P/N) | Z25 p.1 (Sutherland obs.); Sutherland [S22] via Z25 |
| E1-16 | The limit is a murmuration density `M(y)`, `y = p/N` | Z25 p.2 (M_k(y) def, y=P/X) |
| E1-17 | Our plots use `p/N` on the horizontal axis | viz/data/murmuration_curve.json (y_axis "p/N2"); sawin_..._murmuration.json (u=p/N) |
| E1-18 | Square-free vs all levels give slightly different densities (key properties shared) | Z25 p.2, footnote 2 |
| E1-19 | SS order by naive height `H=max(4|A|³,27B²)`; different conjectured density | SS25 p.1 (height); m4-pinning §P1; sawin_..._murmuration.json family |
| E1-20 | Ordering is a real degree of freedom; our M4/PR-1 work the height axis | SS25 + docs/preregistered/PR-1.md |
| E1-21 | Announced 2022 by HLOP (arXiv:2204.10140) | HLOP (arXiv id 2204 = 2022-04) |
| E1-22 | Discovery stemmed from ML/computational work on LMFDB; source unexplained | Z25 p.1 (verbatim); HLOP abstract |
| E1-23 | HLOP: the behaviour "has never been reported in the literature" | HLOP p.3 (verbatim) |
| E1-24 | Z25 established the first case: closed-form density, fixed weight, square-free level, root-number weighted | Z25 abstract + Theorem 1 |
| E1-25 | Z25 published in Inventiones Mathematicae (2025) | arXiv:2310.07681 journal-ref, DOI 10.1007/s00222-025-01347-8 |
| E1-26 | SS height density = Conjecture 1; only a variant (Theorem 2) is proven | SS25 (Conjecture 1 / Theorem 2); sawin_..._murmuration.json source |
| E1-27 | Open: all-levels density (Z25 fn2 "slightly different"); rank-split ≠ root-number; height-tail deviation | Z25 p.2 fn2; docs/preregistered/PR-1.md; m4-pinning |
| E1-28 | Murmurations give statistical rank info; they do not resolve BSD (individual-curve) | HLOP p.2 (BSD = individual rank); abstract (statistical goal) |
| E1-29 | M1 replicates HLOP: unweighted `a_p` mean, conductor-ordered, rank-split, Cremona, 300 primes, vs p/N | viz/data/murmuration_curve.json (params) |
| E1-30 | M2 replicates the LOP Dirichlet-character murmuration | viz/data/dirichlet_murmuration.json (source LOP) |
| E1-31 | M3 replicates Z25's proved weight-2 density and checks the empirical avg vs `M₂(y)` | viz/data/zubrilina_murmuration.json (params: Z25 Theorem 1); M3 verification suite (verify/, ctest label m3) |
| E1-32 | M4+PR-1 test SS Conjecture 1 (1048 at ≤10⁴, 5042 at ≤2¹⁶); hump+zero within τ, trough an open deviation flat 10⁴→2¹⁶ | sawin_..._murmuration.json, ss_x_extension_murmuration.json; PR-1.md; test `prereg_ss_x_extension` |
| E1-33 | Our trough deviation is unresolved (does not decay on our range); PR-2 tests a candidate, outcome pending | PR-1.md postscript; docs/preregistered/PR-2.md |
| E1-34 | "Verified" = numerically, over a range, cross-checked; no formal proof claimed | CLAUDE.md rule 7 / RESEARCH-M §0 rule 7 |

## E2 — primeknots (arithmetic-topology ladder)

| ID | Claim | Source |
|----|-------|--------|
| E2-1 | Arithmetic topology (Mazur / Kapranov–Reznikov / Morishita) is an analogy between number theory and 3-manifold topology | docs/RESEARCH.md §§2–8; Morishita [M12] (docs/papers/README) |
| E2-2 | In the analogy: primes ≈ knots, arithmetic relations ≈ linking | RESEARCH.md §2 (reciprocity-as-linking); Morishita [M12] |
| E2-3 | The dictionary is an analogy, not a theorem; repo name is a homage, not a scope claim | CLAUDE.md rule 7 (m0-pinning §282) |
| E2-4 | Legendre symbol `(p/q)` plays the role of a mod-2 linking number | RESEARCH.md §2 (Stage 0) |
| E2-5 | Quadratic reciprocity = the linking symmetry, verified over a range with two symbol impls | test `theorem_quadratic_reciprocity` (verify/) |
| E2-6 | The pairwise linkings assemble into a linking matrix / graph | viz/data/linking_matrix.json, linking_graph.json; RESEARCH.md §3 |
| E2-7 | Rédei symbol `[a,b,c]` = triple/Borromean linking analogue | RESEARCH.md §4 (Stage 2) |
| E2-8 | `[13, 61, 937] = −1` (the pinned normalization anchor) | test `anchor_redei_13_61_937_is_minus_1`; CLAUDE.md rule 1 |
| E2-9 | Rédei reciprocity verified over a sampled range; Borromean picture emitted | test `theorem_redei_reciprocity_s3`; viz/data/borromean.json |
| E2-10 | Ideal class group ≈ first homology H₁ of the arithmetic 3-manifold | RESEARCH.md §5 (Stage 3) |
| E2-11 | Rédei–Reichardt 4-rank theorem verified over discriminants; class group via reduced forms | test `theorem_redei_reichardt_4rank`; viz/data/classgroups.json |
| E2-12 | Arithmetic Chern–Simons partition function (CKKPPY) has a closed Gauss-sum form | RESEARCH.md §6; CKKPPY (arXiv:1706.03336, docs/papers/README) |
| E2-13 | The two computations of the partition function (character sum vs Gauss sum) agree | test `theorem_ckkppy_partition_identity`; viz/data/cs_partition.json |
| E2-14 | Stage 5 reconstructs the Weil explicit formula (primes ↔ zeta zeros) | RESEARCH.md (Stage 5, zeta); CLAUDE.md ("the Weil explicit formula") |
| E2-15 | Our zeros (in-house Riemann–Siegel) match Odlyzko's table | test `anchor_zeros_match_odlyzko`; viz/data/zeros.json, psi_reconstruction.json |
| E2-16 | Stage 6 counts S₃ homomorphisms (non-abelian DW) against a mass formula | tests `theorem_dw_s3_decomposition`, `theorem_dw_mass_formula`; viz/data/dw_s3.json |
| E2-17 | QR, Rédei reciprocity, Rédei–Reichardt, CKKPPY, Weil explicit formula are genuine theorems | RESEARCH.md §§2–8 (each cites its normative source) |
| E2-18 | This project's contribution is executable cross-checked replication, not a new proof | CLAUDE.md rules 2, 7 |
| E2-19 | Stages 0–6 are complete/green with their verification categories + emitted artifacts | CLAUDE.md (verification vocabulary); viz/data/*.json (all stages emitted); verify/ ctest suites (labels stage0..stage6) |
| E2-20 | The primeknots stages are replication; the scientific frontier is the murmurations ladder | CLAUDE.md rule 7; RESEARCH-M §0 rule 5 (two modes) |

## E3 — method (the harness)

| ID | Claim | Source |
|----|-------|--------|
| E3-1 | Every load-bearing function has two independent implementations, or an oracle check | CLAUDE.md rule 2 |
| E3-2 | `a_p` computed two ways (fast table charsum vs frozen referee), checked to agree | test `twin_ap_fast_vs_charsum_m4` (verify/m4) |
| E3-3 | Optimize one twin; keep the naive twin as referee, never optimize both | CLAUDE.md "Working style" (performance after correctness) |
| E3-4 | Oracles (PARI/LMFDB/Odlyzko) referee, are never called from src/ | CLAUDE.md rule 3 |
| E3-5 | An absent oracle SKIPs visibly (skip code), never a silent pass | CLAUDE.md rule 3; ctest SKIP_RETURN_CODE 77 (verify/CMakeLists.txt) |
| E3-6 | N and ε are oracle-provenance INPUT, labelled `oracle`, distinct from computed quantities | RESEARCH-M §0 rule 6 (provenance); ne_cache header (data/m5/ne_cache_x65536.txt) |
| E3-7 | Published values pinned as anchors: [13,61,937]=−1, a₂(11a1)=−2, Odlyzko zeros | tests `anchor_redei_13_61_937_is_minus_1`, `anchor_ap_small_primes`, `anchor_zeros_match_odlyzko` |
| E3-8 | Never fit a convention to the expected answer; fix the reading against the source | CLAUDE.md rule 1 (the prime directive) |
| E3-9 | Invariance test is senior to the anchor test (passing anchor + failing invariance ⇒ convention still wrong) | memory `invariance-is-senior-to-anchor`; CLAUDE.md rule 1 (verification vocabulary) |
| E3-10 | Research-mode: tolerance/decision-rule/threshold committed to VCS before the data exists | RESEARCH-M §7; docs/preregistered/ |
| E3-11 | PR-1 committed the finite-X-vs-persistent decision rule + interpretation fork before reading the larger scale | docs/preregistered/PR-1.md (decision rule; commit before ladder) |
| E3-12 | PR-2 committed the exact contrast threshold before the split data was computed | docs/preregistered/PR-2.md §"Step 2" |
| E3-13 | Every exploratory statistic looked at is logged in a looks ledger | docs/preregistered/LOOKS.md; RESEARCH-M §7 |
| E3-14 | The errata ledger currently holds 26 entries | docs/ERRATA.md (26 rows) |
| E3-15 | Rule 8 (no constant typed from memory): a typed constant is invisible to twins (#25) | CLAUDE.md rule 8; docs/ERRATA.md #25 |
| E3-16 | #26 records a self-caught protocol breach (confirmation outside protocol; tolerance fit post-hoc) | docs/ERRATA.md #26 |
| E3-17 | Committed viz/data JSON is byte-identical to what the code emits at HEAD | verify/freshness_check.py |
| E3-18 | Only the build stamp is normalized; emitters are compiled for cross-platform bit-reproducibility | freshness_check.py (generated_by normalized); memory `cross-compiler-emit-determinism` (-ffp-contract=off) |
| E3-19 | Every headline number is twinned, anchored, or oracle-refereed; research claims pre-registered; mistakes in a ledger | CLAUDE.md rules 1–3, 6; docs/preregistered/, docs/ERRATA.md |
| E3-20 | No formal proof / new theorem claimed; the machinery makes open questions legible | CLAUDE.md rule 7 |
