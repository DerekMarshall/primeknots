# ERRATA — the error ledger

Every error the harness caught across the project, with its origin and what caught
it. Every party — roadmap generator, spec author, human reviewer, external (LLM)
referee, coding agent — appears at least once. Every catch was a computation or a
citation check (a witness value, a sweep, an identity, a parity argument, an oracle
cross-check, a theorem, a reachability enumeration, a second compiler, a verbatim
quote), not an argument from authority.

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
| 18 | infra/CI | The committed `viz/data/zeros.json` snapshot embedded Odlyzko-oracle-dependent fields (per-zero `delta`, `max_odlyzko_dev`, `oracle:"odlyzko"`) — not reproducible from the repo alone, since the Odlyzko table is gitignored (absent in CI) | coding agent (committed the snapshot at 52e14c3 with the oracle present locally) | **The freshness guard's first CI run** — `zeros.json .aggregate.max_odlyzko_dev: type float≠NoneType`; the guard fired the moment it reached CI. Fixed by committing the *oracle-less* `zeros.json` (the Odlyzko cross-check stays in the suite, `anchor_zeros_match_odlyzko`); the freshness check emits stage 5 `--odlyzko ""` |
| 19 | M1 (murm.) | A **pre-declared statistical control was changed after the results were seen**: the M1 scale-collapse null was declared a within-curve reversal REQUIREd to fail, but when the reversal did not robustly fail it was swapped for an antiphase/parity null — a post-hoc change to a pre-registered design (the M-ladder's first entry) | coding agent (M1 Phase 2) | **External referee (M1 review), from prose** — flagged the swap as a deviation; the git record could **not** corroborate the timing either way (the reversal null was intra-session, never committed). Resolution: reversal reinstated as a *reported* scaling-power measurement, antiphase relabeled the parity non-degeneracy control, deviation postscript (m1-pinning §P4); durable fix — **any pre-declared design (tolerance/null/threshold) is committed before its run** (RESEARCH-M §7, ARCHITECTURE-M) |
| 20 | M3 (murm.) | While hand-adjudicating the M3 trace formula at N=11 (P1 review), a wrong first-pass value of the Hurwitz class number **H(220)** was used | external referee (M3 pinning review) | **The P4 Hurwitz twin** — `mform::hurwitz(220)` (twinned direct-vs-decomposition + PARI `qfbhclassno`-verified) gave the correct value, exposing the slip before it entered a verified anchor cell. Corrected in-review; the N=11 cells (P∈{2,3,5} = −2,−1,1) now match M0's a_P exactly. The convention was solid before the terms consumed it — exactly P4's purpose |
| 21 | infra/CI (murm.) | `std::max({…})` (initializer-list form) in `emit_dirichlet.cpp` **and** `verify/m2/dirichlet_tests.cpp` (M2) used without `<algorithm>` — the same latent missing-include class as #15 — plus two-arg `std::max` in `zub_density.cpp`/`zub_empirical.cpp` (M3) riding on transitive includes. Latent since M2 because the **`murmurations` branch had accrued M0–M3 without ever running CI**: `.github/workflows/ci.yml` triggers `on: push` only for `[main]`, so branch pushes never invoked the second compiler | coding agent (M2/M3) | **The first CI runs on the branch (`gh workflow run ci.yml --ref murmurations`)** — GCC/libstdc++ failed where Apple clang's libc++ had masked it; the parallel (`-j`) build stops at the first error, so the offenders surfaced over two passes (`emit_dirichlet.cpp:30`, then `dirichlet_tests.cpp:116` once the first was fixed). The systemic finding (branch never CI'd) is the real lesson: a second-environment referee that only guards `main` leaves branch work unrefereed until a manual dispatch. Fixed all four; process fix — dispatch CI on the branch (and read to a *clean* build, not just the first error) after material pushes (recorded in [[github-remote]]) |

## Tally by party — nobody was exempt

| Party | Caught in |
|---|---|
| Roadmap generator | #1 (O_x scope) |
| Spec author (RESEARCH.md) | #8 (p=2 unlicensed), #10 (ψ midpoint) |
| Human reviewer (riders/oracles) | #2 (R1 misclassification), #5 (ordinary-vs-narrow oracle), #9 (equal-parity), #11 (signature-mix, void by Stickelberger) |
| External (LLM) referee | #3 (fabricated `D_{L/K₁}=(a₂)` citation), #20 (own H(220) slip) — *the same party made the genuine R1 catch in #2, caught the agent's deck errors #16–#17 and the M1 null-swap #19, and had its own #20 caught by the P4 twin* |
| Coding agent | #4 (strat counter), #6 (signed-a), #7 (generator cap), #12 (2c/6c), #13 (PDFs in history), #14 (Belabas filter), #15 (missing headers), #16 (deck coverage conflation), #17 (deck mis-attribution), #18 (oracle-dependent snapshot), #19 (M1 post-hoc null swap), #21 (missing headers again — branch un-CI'd) |

## Tally by mechanism — every catch was a computation or a citation

coherence check (1) · CFT re-derivation (1) · verbatim quote (1) · parity argument
(1) · oracle gp cross-check (1) · algebraic identity / associativity (1) ·
exhaustive sweep (1) · source-hypothesis reading (1) · anchor witness (2) ·
Stickelberger's theorem (1) · exact-rational identity (1) · git reachability
enumeration (1) · twin disagreement (1) · second-compiler build (1) · external
referee review (deck + M1-deviation, from prose / citation trace) (3) · CI freshness
guard (1) · twin class-number computation (P4 Hurwitz, #20) (1). **Arguments from
authority: 0.**

Entries #16–#20 are catches of this project's own artifacts and process: two deck
defects (a coverage conflation and a mis-attribution, both caught by the referee —
one exactly where the machine checker is blind), a published-data snapshot that
embedded an oracle dependency it could not reproduce (caught by the freshness guard
on its first CI run, a second environment — the same mechanism as #15), and the
M-ladder's first entries: #19 — a *pre-declared control changed after the results were
seen*, caught by the referee from prose, whose fix is now a process rule
(commit-the-design-before-the-run; RESEARCH-M §7); and #20 — the referee's OWN H(220)
slip, caught by the P4 Hurwitz twin before it reached a verified cell (the convention
solid before the elliptic terms consumed it); and #21 — a *recurrence* of #15's
missing-`<algorithm>` class, latent since M2 because the `murmurations` branch had
accrued M0–M3 without CI ever running (the workflow's `push` trigger is `main`-only),
surfaced the moment the second compiler was invoked on the branch by manual dispatch.
The lesson of #21 is not the include — it is that branch work went unrefereed by the
second environment; the process fix is to dispatch CI on the branch after material
pushes. Note: the explainer deck's "18 entries" claim is the Stage 0–6 ladder count;
M-ladder rows (#19+) accrue here and the deck is extended to them when the M-stages
are presented (deferred, tracked).

## The senior rule

Where an `anchor_` (a published value) and an `invariance_` (independence from an
arbitrary choice) disagreed, `invariance_` won — a passing anchor with a failing
invariance means the convention is still wrong (memory: *invariance is senior to
anchor*). #9 and #12 are the cases where the tempting move was to keep the value
that reproduced a known number; the computation across choices, not the matched
constant, decided each one.

In #11 and #14 the catch was definitive: the signature stratum is void by
Stickelberger's theorem (not merely absent from the sweep), and a referee caught
its own implementation bug (Belabas vs PARI).
