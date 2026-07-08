# ERRATA — the epistemic ledger

Every error the harness caught, across the whole project, with its origin and
the mechanism that caught it. This table is the project's actual thesis:

> **Every party that touched this project — the roadmap generator, the LLM
> research assistant, the RESEARCH.md author, the human reviewer, the coding
> agent, and the external referee — was caught in at least one error. And every
> single catch was a *computation* or a *citation check*: a witness value, an
> exhaustive sweep, an algebraic identity, a parity argument, an oracle
> cross-check, or a verbatim quote. Not one error was caught, or fixed, by an
> argument from authority.**

That is the point of building it this way. "A result computed one way is a
rumor" (CLAUDE.md #2); a normalization defended by whose-idea-it-was is a rumor
too. The rule that did the work every time was **Rule 1** — a disagreement is a
deliverable with a full witness, never reconciled by flipping a sign or trusting
the source.

## The ledger

| # | Stage | The error | Who introduced it | What caught it (a computation or a citation) |
|---|---|---|---|---|
| 1 | scope (pre-code) | The "chaotic operator O_x" material was slated for inclusion | roadmap generator | **Coherence check** — the O_x literature fails basic coherence tests; excluded in the roadmap's own exclusion pass (RESEARCH.md §9) |
| 2 | 2 | C07 Lemma 5.1.2 cited as a relative-discriminant equality `D_{L/K₁}=(a₂)` | Gemini (LLM research assistant) | **Quote-verbatim protocol** — the amendment pass transcribed the lemma: it states a *norm* equality `N_{K₁/ℚ}(α₂)=a₂ mod □` + an F₄/F ramification clause, not that discriminant identity (stage2-pinning §9.1, DISCREPANT) |
| 3 | 2 | Degeneracy guard `p₃∤z` (R1) classified as a symbol-*definedness* condition | pinning author (agent) | **External-referee catch → local CFT re-derivation** — the Hilbert symbol `(β,π)_{𝔭₃}` is ±1 for any nonzero β; `p₃∤z` is only the computational boundary of the Legendre fast path [S22 (47)–(48)] (stage2-pinning §R1/§9) |
| 4 | 2 | Reciprocity coverage counter reported `14773` "permutations" | agent (test author) | **Parity argument** — a permutation count must be even; 14773 is odd (it was Σ element-incidences). True count `2·Σkₜ ≈ 29546` (stage2-pinning §9.6) |
| 5 | 3 | `qfbclassno`/`quadclassunit` proposed as the class-group oracle | human reviewer (user-suggested oracle) | **Agent's gp cross-check** — both return the *ordinary* `h`; the form class group is the *narrow* `Cl⁺`, undercounted by 2 exactly on the `N(ε)=+1` discriminants. Replaced by `bnfnarrow` (stage3-pinning §1.3) |
| 6 | 3 | `compose` used signed `a` in the CRT moduli/ranges → garbage for indefinite forms (`a<0`), e.g. `(0,−13,0)` at D=205 | agent (implementation) | **`theorem_compose_group_axioms` (associativity)** — the class-*number* twin had missed it, which is exactly why the associativity property test exists (commit `8b4ee7b`) |
| 7 | 3 | Prime-form generators capped at 8 primes `<200` → a *proper subgroup* of `Cl⁺` for larger D | agent (implementation) | **`theorem_genus_2rank` over the discriminant sweep** — 26 genus-theorem failures; fixed by generating prime forms up to the Minkowski bound `√D/2` |
| 8 | 4 | RESEARCH.md §6: "p = 2 over ℚ, Stages 1–3 verbatim" | RESEARCH.md author (paraphrase) | **Phase-1 hypothesis pinning** — CKKPPY Thm 1.1 requires *odd* p and a *totally imaginary* F with μ_{p²}; p=2-over-ℚ is doubly unlicensed. Substituted the in-hypothesis Hirano mod-2 route (stage4-pinning H1) |
| 9 | 4 | "Equal-parity" candidate for the RHS closed form | reviewer-raised candidate (rider R1) | **Brute-force witness, K=ℚ(√65)** — equal-parity predicts `Z=2`; the brute character sum gives `0`. Killed in favour of "all row-sums even" *before* the RHS was coded (stage4-pinning §R1) |
| 10 | 5 | RESEARCH.md §7.2: the ψ series "converging onto ψ(x)" | RESEARCH.md author | **Phase-1 pinning + PARI witness at x=8** — the series converges to the jump *midpoint* ψ₀(x)=ψ(x)−Λ(x)/2 = 6.3868…, not ψ(8)=6.7334… (open-questions R6, stage5-pinning §3) |
| 11 | 6 | S₃ hom-term written as `2c` (embedded-subfield count) | agent (Phase-1 draft) | **Hand-referee (rider R1) + exact-rational mass formula** — LMFDB counts iso classes; the 3 conjugate subfields of a closure are isomorphic ⇒ one class ⇒ `6c`. The `theorem_dw_mass_formula` guards the assembly (stage6-pinning R1) |
| 12 | infra | The 5 copyrighted paper PDFs were left in git *history* (a later `git rm --cached` only untracked them going forward) and pushed to the public remote | agent (committed the PDFs; the untrack did not purge history) | **Human reviewer's PDF-history audit** — a reachability computation, `git log --diff-filter=A -- docs/papers/` + `git rev-list --objects main | grep pdf` + `gh api contents@<old-commit>` confirming the blob present on the remote. Fixed by a `filter-branch` purge + re-sign + force-push |
| 13 | 6 | Belabas driver's rad-filter stripped factors of 2 before the ⊆-S test, so even-discriminant cubics (rad ∋ 2 ∉ S) leaked in | agent (referee driver) | **Disagreement with the PARI count** — Belabas reported 7 for `{5,17,29}` vs PARI's 2; the extra discs (5780=2²·5·17², …) exposed the filter bug. The twin caught its own referee |

## Tally by party — nobody was exempt

| Party | Caught in |
|---|---|
| Roadmap generator | #1 (O_x scope) |
| LLM research assistant (Gemini) | #2 (fabricated discriminant citation) |
| RESEARCH.md author | #8 (p=2 unlicensed), #10 (ψ midpoint) |
| Human reviewer | #5 (ordinary-vs-narrow oracle), #9 (equal-parity candidate), #12 (PDF-history audit) |
| Coding agent | #3 (R1 class), #4 (strat counter), #6 (signed-a), #7 (generator cap), #11 (2c/6c), #12 (PDFs in history), #13 (Belabas filter) |
| External referee | #2, #3 (amendment-pass requests), #5 (PARI/gp), #9's witness path, #13 (PARI disagreement) |

## Tally by mechanism — every catch was a computation or a citation

coherence check (1) · verbatim quote (1) · CFT re-derivation (1) · parity
argument (1) · oracle gp cross-check (1) · algebraic identity / associativity (1)
· exhaustive sweep (1) · source-hypothesis reading (2) · anchor witness (2) ·
exact-rational identity (1) · git reachability computation (1) · twin
disagreement (1). **Arguments from authority: 0.**

## The senior rule

Where an `anchor_` (a published value) and an `invariance_` (independence from an
arbitrary choice) disagreed, `invariance_` won — a passing anchor with a failing
invariance means the convention is still wrong (memory: *invariance is senior to
anchor*). Several of the above (especially #9, #11) are cases where the tempting
move was to keep the value that reproduced a known number; the rule that the
*computation across choices* is senior to the *matched constant* is what kept the
experiment honest.
