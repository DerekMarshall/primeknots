# Explainer deck — OUTLINE (for review at the gate)

Single self-contained `docs/deck/index.html`, keyboard-nav, D3/CDN only, no build
step; plus a committed PDF snapshot (headless Chrome, static screenshots replace
iframes). Audience: **dual-track** — engineers without number theory, and
mathematicians without C++. Slides carry the spine only; depth links out to
`docs/RESEARCH.md`, the pinning logs, and the papers.

**~34 slides, 4 acts.** Each row: slide id — title — one-line claim. `[viewer:X]`
= embed the live viewer by iframe (same emitted JSON as the repo). `[data:ID]` =
carries a tested number ⇒ needs a `CLAIMS.md` row; the `deck` ctest check fails if
any `[data:ID]` in the built HTML lacks one.

---

## Two decisions for Derek AT THIS GATE (flagged, not assumed)

- **(a) Origin-story framing.** Act I tells the failure that motivates the whole
  project. **Default: describe the BEHAVIORS** — unverified synthesis, fitting a
  result to expectation, citation fabrication — **not the vendors/tools** that
  exhibited them. Naming a model/company is a different, sharper claim; say if you
  want it, otherwise it stays behavior-level.
- **(b) Publication.** Building `index.html` + the PDF is *not* publishing.
  **GitHub Pages (or any public hosting) is a separate, named boundary action** —
  it will not be bundled into "done". I will build and commit the deck; I will not
  turn on Pages without your explicit go.

---

## Act I — THE FAILURE MODE (4)

- `S1` — **Title.** primeknots: named theorems of number theory, verified from scratch, where the process makes fitting impossible.
- `S2` — **A survey that read as mathematics.** A roadmap mixed real arithmetic topology with a fabricated "chaotic operator O_x"; the fiction was indistinguishable in prose.
- `S3` — **Why it survived five revisions.** Each critique was absorbed as camouflage — the text got more plausible, not more true; agreement with a reviewer is not evidence.
- `S4` — **The moral → Rule 1.** Agreement with expectation is evidence *only* when the process makes fitting impossible; hence: never fit conventions to expected answers.

## Act II — THE DICTIONARY AND THE LADDER (bulk, one cluster per stage)

- `S5` — **The dictionary.** Primes ↔ knots, Spec ℤ[1/S] ↔ 3-manifold-with-boundary, Legendre symbol ↔ linking number (Morishita [M12]); the whole ladder is one analogy, made executable.
- `S6` — **Stage 0 · reciprocity, the base case.** Quadratic reciprocity + both supplements, verified exhaustively to 10⁶ with a twin (Euler's criterion vs the reciprocity law). `[data:qr-exhaustive]`
- `S7` — **Stage 1 · the linking matrix.** `(p/q)=(−1)^{lk₂(p,q)}` is symmetric — the heatmap *is* quadratic reciprocity. `[viewer:linking]` `[data:linking-symmetry]`
- `S8` — **Stage 2 · the Rédei symbol.** The triple symbol `[a,b,c]` is the mod-2 triple Milnor invariant (Morishita [M04]); Rédei reciprocity = full S₃ symmetry, verified over every valid triple in range. `[data:redei-s3]`
- `S9` — **Stage 2 · Borromean primes + the anchor.** Pairwise-unlinked, triple-linked triples; anchor `[13,61,937]=−1`; the 2-adic (N4) normalization pinned to Stevenhagen [S22], with invariance senior to the anchor. `[viewer:borromean]` `[data:redei-anchor]`
- `S10` — **Stage 3 · class groups as H₁.** The Rédei–Reichardt 4-rank of the narrow class group (form side) equals the F₂-rank of the Rédei/linking matrix (linking side), over ≥10³ discriminants. `[viewer:classgroups]` `[data:four-rank]`
- `S11` — **Stage 4 · abelian arithmetic Chern–Simons.** The mod-2 Dijkgraaf–Witten partition function `Z_c`, computed as a brute character sum (LHS) and a closed form (RHS) in disjoint authorship sessions, agree on first comparison. `[viewer:cs]` `[data:cs-identity]`
- `S12` — **Stage 4 · the honesty clause.** The mod-2 (Hirano) case is the **degenerate** one: CS is F₂-linear, so the identity reduces to linear algebra on the linking matrix. The contribution was the pinning + the p=2-over-ℚ hypothesis erratum; the odd-p Gauss sum is future work.
- `S13` — **Stage 5 · the Weil explicit formula.** ψ(x) rebuilt from ζ zeros (Riemann–Siegel, with an Euler–Maclaurin twin and Turing-certified completeness); zeros match Odlyzko to ≥8 dp over the first 10⁴, incl. the Lehmer pair. `[viewer:explicit_formula]` `[data:zeros-odlyzko]` `[data:psi-midpoint]`
- `S14` — **Stage 5 · the model flow and the Deninger gap.** A subshift's Ruelle zeta satisfies Euler-product = determinant *as geometry*; arithmetic satisfies the same identity shape with **no constructed system — that gap IS the program** (Deninger [D98]). `[data:ruelle-twin]`
- `S15` — **Stage 6 · non-abelian S₃ counting.** `Z_DW = |Hom(π₁,S₃)|/6 = 1/6 + (2^t−1)/2 + (3^k−1)/6 + c`; the S₃ term `c` (cubic iso classes) refereed two independent ways — PARI `nflist` and Belabas's `cubic`. `[viewer:dw]` `[data:dw-decomposition]` `[data:belabas-twin]`

## Act III — THE HARNESS (6)

- `S16` — **Rule 1, restated with teeth.** If the anchor comes out wrong, fix the *reading* against the source; flipping a sign to hit a known value is falsification.
- `S17` — **Five test categories.** `twin_` (two algorithms), `theorem_` (named theorem over a range), `anchor_` (a published value), `invariance_` (independence from a choice), `oracle_` (PARI/Odlyzko/Belabas) — and invariance is senior to anchor.
- `S18` — **Two-phase gates + pinning logs.** Convention-free code first; normalization pinned to a source in `docs/notes/` (`[PIN TO SOURCE]`) before the load-bearing code is written.
- `S19` — **Pre-registration.** Stage 4's LHS and RHS were built in separate TUs in disjoint sessions, each seeing only its own spec; a mismatch would have been a deliverable, not a bug to reconcile.
- `S20` — **Stratify the sweep — and sometimes prove the stratum void.** mod-8 third-slot, corank cells, all-odd×odd-r (empty by parity), and the signature stratum **void by Stickelberger** — coverage is asserted, not hoped.
- `S21` — **The ERRATA table.** Every party — roadmap generator, spec author, human reviewer, external LLM referee, coding agent — caught at least once; every catch a computation or a citation; authority-catches: 0. `[data:errata-count]`

## Act IV — RESULTS AND HORIZONS (4)

- `S22` — **The gallery.** Four pictures, each rendered from emitted JSON: linking heatmap, Borromean hypergraph, 4-rank diagonal, ψ beside the model flow.
- `S23` — **What was verified.** The full ladder, green; N-case counts pulled from emitted JSON / test output (never retyped). `[data:suite-total]`
- `S24` — **Honest scope.** Numerically verified over N cases ≠ proved; Deninger's system is not constructed; the mod-2 CS case is degenerate. What we did *not* claim.
- `S25` — **Horizons.** CKKPPY odd-p over totally imaginary fields as future infrastructure; murmurations as the field's live frontier. References from RESEARCH.md §10 only.

---

## Register rules (enforced by CLAIMS.md + the `deck` ctest check)

- **"proved"** only for literature theorems, with citation. Our results are
  **"verified numerically over N cases"**, N from emitted JSON or test output —
  never retyped, never rounded.
- **Empirics labeled observed/unasserted:** Borromean rate ≈ 51.6%; negative-Pell
  fraction 0.6692 vs the Koymans–Pagano asymptotic 0.5805; N_{S₃} growth.
- **Deninger:** "not constructed — that gap IS the program."
- **Stage 4:** the honesty clause (S12) is mandatory; a slide that oversells mod-2
  fails review.
- **No slide asserts anything the suite doesn't test.**

## Form & checker

- `docs/deck/index.html` — self-contained, keyboard-nav, D3 from CDN, iframes to
  `../../viz/*.html`; `docs/deck/deck.pdf` — headless-Chrome snapshot (static
  images swap in for iframes).
- `docs/deck/CLAIMS.md` — one row per `[data:ID]`: `id | claim | status | source
  (test name + case count / pinning-log § / paper eq)`.
- `verify/deck_check.py` wired into ctest (label `deck`): parse `index.html` for
  `data-claim="ID"`, fail if any ID lacks a `CLAIMS.md` row. The deck is an
  emitted artifact; it gets validated like one.
