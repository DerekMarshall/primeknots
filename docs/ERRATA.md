# ERRATA — the epistemic ledger

Every error the harness caught, across the whole project, with its origin and the
mechanism that caught it. This table is the project's actual thesis:

> **Every party that touched this project — the roadmap generator, the spec
> author, the human reviewer, the external (LLM) referee, and the coding agent —
> was caught in at least one error. And every single catch was a *computation* or
> a *citation check*: a witness value, an exhaustive sweep, an algebraic identity,
> a parity argument, an oracle cross-check, a theorem, a reachability enumeration,
> a second compiler, or a verbatim quote. Not one error was caught, or fixed, by
> an argument from authority.**

"A result computed one way is a rumor" (CLAUDE.md #2); a normalization defended by
whose-idea-it-was is a rumor too. The rule that did the work every time was **Rule
1** — a disagreement is a deliverable with a full witness, never reconciled by
flipping a sign or trusting the source. It is unflattering to everyone, evenly —
including the referee that made a good catch and fabricated a citation in the same
review, and the agent whose own catches sit beside its own bugs.

## The ledger

| # | Stage | The error | Who introduced it | What caught it (a computation or a citation) |
|---|---|---|---|---|
| 1 | scope | The "chaotic operator O_x" material was slated for inclusion | roadmap generator | **Coherence check** — the O_x literature fails basic coherence tests; excluded in the roadmap's own exclusion pass (RESEARCH.md §9) |
| 2 | 2 | Degeneracy guard `p₃∤z` (R1) classified as a symbol-*definedness* condition | human reviewer (rider) | **External-referee catch → local CFT re-derivation** — the Hilbert symbol `(β,π)_{𝔭₃}` is ±1 for any nonzero β; `p₃∤z` is only the computational boundary of the Legendre fast path [S22 (47)–(48)] (stage2-pinning §R1/§9). *This is the referee's genuine catch.* |
| 3 | 2 | C07 Lemma 5.1.2 cited as a relative-discriminant equality `D_{L/K₁}=(a₂)` | **external (LLM) referee** | **Quote-verbatim protocol** — transcribing the lemma showed it states a *norm* equality `N_{K₁/ℚ}(α₂)=a₂ mod □` + an F₄/F ramification clause, not that discriminant identity (stage2-pinning §9.1, DISCREPANT). *The same referee that caught #2 fabricated this.* |
| 4 | 2 | Reciprocity coverage counter reported `14773` "permutations" | coding agent | **Parity argument** — a permutation count must be even; 14773 is odd (it was Σ element-incidences). True count `2·Σkₜ ≈ 29546` (stage2-pinning §9.6) |
| 5 | 3 | `qfbclassno`/`quadclassunit` proposed as the class-group oracle | human reviewer (suggested oracle) | **Agent's gp cross-check** — both return the *ordinary* `h`; the form class group is the *narrow* `Cl⁺`, undercounted by 2 exactly on the `N(ε)=+1` discriminants. Replaced by `bnfnarrow` (stage3-pinning §1.3) |
| 6 | 3 | `compose` used signed `a` in the CRT moduli/ranges → garbage for indefinite forms (`a<0`), e.g. `(0,−13,0)` at D=205 | coding agent | **`theorem_compose_group_axioms` (associativity)** — the class-*number* twin had missed it, which is exactly why the associativity property test exists |
| 7 | 3 | Prime-form generators capped at 8 primes `<200` → a *proper subgroup* of `Cl⁺` for larger D | coding agent | **`theorem_genus_2rank` over the discriminant sweep** — 26 genus-theorem failures; fixed by generating prime forms up to the Minkowski bound `√D/2` |
| 8 | 4 | RESEARCH.md §6: "p = 2 over ℚ, Stages 1–3 verbatim" | spec author (RESEARCH.md) | **Phase-1 hypothesis pinning** — CKKPPY Thm 1.1 requires *odd* p and a *totally imaginary* F with μ_{p²}; p=2-over-ℚ is doubly unlicensed. Substituted the in-hypothesis Hirano mod-2 route (stage4-pinning H1) |
| 9 | 4 | "Equal-parity" candidate for the RHS closed form | human reviewer (rider R1) | **Brute-force witness, K=ℚ(√65)** — equal-parity predicts `Z=2`; the brute character sum gives `0`. Killed in favour of "all row-sums even" *before* the RHS was coded (stage4-pinning §R1) |
| 10 | 5 | RESEARCH.md §7.2: the ψ series "converging onto ψ(x)" | spec author (RESEARCH.md) | **Phase-1 pinning + PARI witness at x=8** — the series converges to the jump *midpoint* ψ₀(x)=ψ(x)−Λ(x)/2 = 6.3868…, not ψ(8)=6.7334… (open-questions R6, stage5-pinning §3) |
| 11 | 6 | Rider: "the signature mix (both signs of disc) *must be present* in the sweep" | human reviewer (rider) | **Stickelberger's theorem** — for S ⊆ {p≡1 mod4}, \|disc\|≡1 mod4, so disc<0 ⇒ disc≡3 mod4, forbidden. The complex stratum is not merely absent, it is **void by theorem**; the "mix present" rider is falsified and withdrawn (stage6-pinning R3) |
| 12 | 6 | S₃ hom-term written as `2c` (embedded-subfield count) | coding agent (Phase-1 draft) | **Hand-referee (rider R1) + exact-rational mass formula** — LMFDB counts iso classes; the 3 conjugate subfields of a closure are isomorphic ⇒ one class ⇒ `6c`. `theorem_dw_mass_formula` guards the assembly (stage6-pinning R1) |
| 13 | infra | The 5 copyrighted paper PDFs were left in git *history* (a later `git rm --cached` only untracked them) and pushed to the public remote | coding agent | **Human reviewer's git-reachability audit** — `git log --diff-filter=A -- docs/papers/` + `git rev-list --objects main \| grep pdf` + `gh api contents@<old-commit>`. Fixed by a `filter-branch` purge + re-sign; verified from a fresh clone (publication-audit.md) |
| 14 | 6 | Belabas driver's rad-filter stripped factors of 2 before the ⊆-S test, so even-disc cubics (rad ∋ 2 ∉ S) leaked | coding agent (referee driver) | **Disagreement with the PARI count** — Belabas reported 7 for `{5,17,29}` vs PARI's 2; the extra discs (5780=2²·5·17², …) exposed the filter bug. The twin caught its own referee |
| 15 | infra/CI | `std::sqrt`/`std::max`/`std::swap` used without `<cmath>`/`<algorithm>`/`<utility>` — a latent missing-include bug | coding agent | **The CI/GCC build** — Apple clang's libc++ pulls these in transitively and masked it locally; GCC's libstdc++ does not, so the CI leg (a *second compiler*) failed at `qform.cpp` and surfaced it. Fixed across 10 files |
| 16 | deck | The Stage-0 deck slide claimed "verified exhaustively over all odd primes < 10⁷" — conflating three coverages: the reciprocity check was exhaustive only to 3×10⁴ (5.26M pairs); the 10⁷ bound applied only to the supplements and the twin | coding agent (deck author) | **External referee (deck review)** — the `deck` ctest passed because it verifies row *existence* + *numbers present*, and every number was real and in the ledger; it does not verify that a bound is *attributed* to the right object. Fixed by disaggregating slide + row; checker gap documented (CLAIMS.md header) |
| 17 | deck | The Borromean slide credited `[13,61,937]=−1` to "Stevenhagen's worked value"; it is the standard Borromean triple of Morishita [M12] ([S22] is the *normalization* source, not the anchor's) | coding agent (deck author) | **External referee (deck review) → citation trace** — RESEARCH.md §10 ("the standard Borromean triple [M12]") and stage2-pinning line 181. Attribution by vibe, corrected against the source |

## Tally by party — nobody was exempt

| Party | Caught in |
|---|---|
| Roadmap generator | #1 (O_x scope) |
| Spec author (RESEARCH.md) | #8 (p=2 unlicensed), #10 (ψ midpoint) |
| Human reviewer (riders/oracles) | #2 (R1 misclassification), #5 (ordinary-vs-narrow oracle), #9 (equal-parity), #11 (signature-mix, void by Stickelberger) |
| External (LLM) referee | #3 (fabricated `D_{L/K₁}=(a₂)` citation) — *the same party made the genuine R1 catch in #2, and caught the agent's deck errors #16–#17* |
| Coding agent | #4 (strat counter), #6 (signed-a), #7 (generator cap), #12 (2c/6c), #13 (PDFs in history), #14 (Belabas filter), #15 (missing headers), #16 (deck coverage conflation), #17 (deck mis-attribution) |

## Tally by mechanism — every catch was a computation or a citation

coherence check (1) · CFT re-derivation (1) · verbatim quote (1) · parity argument
(1) · oracle gp cross-check (1) · algebraic identity / associativity (1) ·
exhaustive sweep (1) · source-hypothesis reading (1) · anchor witness (2) ·
Stickelberger's theorem (1) · exact-rational identity (1) · git reachability
enumeration (1) · twin disagreement (1) · second-compiler build (1) · external
referee review + citation trace (2). **Arguments from authority: 0.**

The last two (#16, #17) are the ledger folding in on itself: the deck that
*presents* this thesis had a coverage conflation and an attribution-by-vibe of
its own, and the referee layer caught both — one of them (#16) exactly where the
machine checker is blind (numbers present ≠ numbers correctly attributed). The
harness catches the harness.

## The senior rule

Where an `anchor_` (a published value) and an `invariance_` (independence from an
arbitrary choice) disagreed, `invariance_` won — a passing anchor with a failing
invariance means the convention is still wrong (memory: *invariance is senior to
anchor*). #9 and #12 are exactly the cases where the tempting move was to keep the
value that reproduced a known number; the rule that the *computation across
choices* is senior to the *matched constant* is what kept the experiment honest.

Two of these (#11, #14) are the strongest form of the thesis: a rider was not
merely unmet but **proven impossible** (the signature stratum is void, by
Stickelberger), and a referee **caught its own** implementation bug (Belabas vs
PARI). "Stratify the sweep — and sometimes prove the stratum void."
