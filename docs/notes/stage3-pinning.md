# Stage 3 pinning log — class groups & the Rédei matrix [PIN TO SOURCE]

**Status: Phase 1 deliverable, for review before Phase 2 (ClassGroup / redei_matrix)
is authorized.** Resolves the two [PIN TO SOURCE] items of RESEARCH.md §5. Rule 5:
discrepancies are recorded, not smoothed.

Sources:
- **[Coh93]** H. Cohen, *A Course in Computational Algebraic Number Theory*,
  GTM 138, ch. 5. The implementation bible (RESEARCH.md §5). **Not committed** to
  `docs/papers/` (copyrighted textbook) — so, unlike Stage 2, algorithm choices
  are pinned to Cohen *by reference* and their correctness is certified against
  the PARI/GP referee, not by verbatim quotation.
- **[B89]** D. Buell, *Binary Quadratic Forms*, Springer 1989.
- **[S22]** Stevenhagen, arXiv:1806.06250 (committed) — for the Rédei matrix
  (§2 below); quoted verbatim.
- **PARI/GP** referee: `bnfnarrow`, `quadclassunit`, `quadunit`, `qfbred`, `Qfb`.

## Restricted family

`D = p₁···p_t`, distinct primes `≡ 1 (mod 4)`. Then `D ≡ 1 (mod 4)`, squarefree,
positive ⇒ **D is the fundamental discriminant** of the real quadratic field
`ℚ(√D)`, and the prime divisors of `D` are exactly `{p₁,…,p_t}` (t of them).
Real field ⇒ **indefinite** forms ⇒ cycle-based reduction (item i).

---

## 1. [PIN i] NARROW vs ORDINARY, and cycle-based reduction

### 1.1 The group we compute

The group of primitive binary quadratic forms of discriminant `D` under **proper
(SL₂(ℤ)) equivalence** and Gauss composition is isomorphic to the **narrow**
(a.k.a. form / strict) class group `Cl⁺(D)` [Coh93 §5.2; B89]. Rédei–Reichardt
and genus theory (RESEARCH.md §5, [S22 Thm 2.1]) are statements about `Cl⁺` —
`r₂(Cl⁺)=t−1`, `r₄(Cl⁺)=t−1−rank R`. So **we must compute the narrow group.**
GL₂(ℤ)-equivalence (improper allowed) would instead give the *ordinary* `Cl(D)`.

### 1.2 Cycle-based reduction (the pitfall)

For `D>0` (indefinite), reduction is **cycle-based** [Coh93 §5.6, B89]:
- A reduced form is one with `|√D − 2|a|| < b < √D`.
- **Reduced forms are NOT unique per class.** Each proper-equivalence class is a
  single *cycle* of reduced forms, traversed by the operator `ρ(a,b,c) =
  (c, b′, c′)`, `b′ ≡ −b (mod 2c)` chosen into the reduced interval, `c′=(b′²−D)/(4c)`.
- Equivalence test = **same cycle** (reduce both; check membership in one cycle).
- **h⁺ = number of cycles** of reduced forms.

**Failure mode (pinned):** treating each reduced form as a canonical class
representative silently **overcounts h⁺** (a class contributes as many reduced
forms as its cycle length, typically > 1). The class number must count *cycles*,
and equivalence must test *cycle membership* — never reduced-form equality.

### 1.3 ORACLE CORRECTION — `qfbclassno`/`quadclassunit` are ORDINARY, not narrow

**Verified against gp (2026-07-07).** For every tested D:
`qfbclassno(D) == quadclassunit(D).no`, i.e. both return the **ordinary** class
number `h`. The **narrow** number is `bnfnarrow`, and it satisfies
`h⁺ = h · (2 if N(ε)=+1 else 1)` where `ε = quadunit(D)`:

| D | factor | ordinary h (`qfbclassno`) | N(ε) | narrow h⁺ (`bnfnarrow.no`) | narrow cyc |
|---|---|---|---|---|---|
| 65   | 5·13     | 2 | −1 | 2 | [2] |
| 1105 | 5·13·17  | 4 | −1 | 4 | [2,2] |
| 221  | 13·17    | 2 | **+1** | **4** | [4] |
| 8845 | 5·29·61  | 4 | **+1** | **8** | [4,2] |
| 32045| 5·13·17·29 | 8 | −1 | 8 | [2,2,2] |

So `qfbclassno`/`quadclassunit` **undercount `Cl⁺` by a factor 2 exactly on the
`N(ε)=+1` discriminants** — which are precisely the ones where the 4-rank story
is non-trivial. **The user-suggested `oracle_qfbclassno` / `oracle_quadclassunit`
are therefore replaced by the correct narrow oracle `bnfnarrow`** (`.no` and
`.cyc`). This substitution is the substance of PIN item (i).

Consequence for tests:
- `oracle_narrow_class_number` (Phase 1B): our cycle-count `h⁺` **REQUIRE**d equal
  to `bnfnarrow(bnfinit(x²−D)).no` over the sample. (Also confirms empirically
  that form-composition = narrow, closing any doubt about §1.1.)
- Phase 2 `oracle_quadclassunit` is likewise re-pointed at `bnfnarrow.cyc` for the
  narrow group *structure* (SNF invariants).
- `qfbclassno` may still appear as a labelled *ordinary* cross-reference, never as
  the narrow oracle.

Genus check as internal confirmation: `r₂(Cl⁺)=t−1` holds for the narrow
structures above (e.g. D=8845 narrow [4,2] has 2-rank 2 = t−1; ordinary would
mislead on the 4-rank, e.g. D=221 narrow [4] has 4-rank 1 vs ordinary [2] 4-rank 0).

### 1.4 Algorithm verification (Cohen not committed)

`reduce`/`ρ` pinned to [Coh93 §5.6]; `compose` to [Coh93 §5.4] (Gauss/Dirichlet).
Correctness certified against gp: `reduce(f)` yields a reduced form in the same
cycle as `qfbred`; `compose` matches gp `Qfb` composition after reduction;
`h⁺`/structure match `bnfnarrow`. These are the Phase 1B twins/oracles.

---

## 2. [PIN ii] Rédei matrix diagonal

### 2.1 Source (verbatim, [S22 eq (16)], p. 5)

> "the entry `ε_ij` describes the action of the Artin symbol … For `i≠j`, it is
> an F₂-valued Legendre (or for `pⱼ=2` Kronecker) symbol: `ε_ij = χ_{p*ᵢ}([𝔭ⱼ]) =
> (p*ᵢ/pⱼ) ∈ F₂. (16)` The diagonal entries `ε_jj = Σ_{i≠j} ε_ij` of `R₄` follow
> from the sum-zero-property of γ: the rows of `R₄` add up to `0 ∈ F₂ᵗ`."

Here `p* = (−1)^{(p−1)/2} p` [S22 (5)].

### 2.2 Specialization to our family

Every `pᵢ ≡ 1 (mod 4)` ⇒ `p*ᵢ = pᵢ`. So:
- **Off-diagonal:** `R_ij = ε_ij = (pᵢ/pⱼ) ∈ F₂ = lk₂(pᵢ,pⱼ)` — exactly the
  Stage 1 linking-matrix entry. Symmetric by quadratic reciprocity (`pᵢ≡1 mod4`).
- **Diagonal (pinned):** `R_ii = Σ_{j≠i} R_ij`. Equivalently, the convention
  stated in RESEARCH.md, `(−1)^{R_ii} = ((D/pᵢ)/pᵢ)`: since `D/pᵢ = ∏_{j≠i} pⱼ`,
  `((D/pᵢ)/pᵢ) = ∏_{j≠i}(pⱼ/pᵢ) = (−1)^{Σ_{j≠i} R_ij}` (using reciprocity
  `(pⱼ/pᵢ)=(pᵢ/pⱼ)`). The two forms of the diagonal coincide.

### 2.3 Rows-sum-to-zero — a REQUIRE, not an assumption

Row `i` sums to `R_ii + Σ_{j≠i} R_ij = 2·Σ_{j≠i} R_ij = 0 ∈ F₂`. Phase-2 code
constructs `R` with `R_ij = lk₂` off-diagonal and `R_ii = Σ_{j≠i} R_ij`, and a
test **REQUIRE**s every row (and, by symmetry, column) of the constructed `R`
sums to 0 over the whole sweep — verifying the sum-zero property on the built
matrix rather than assuming it.

**Failure mode (pinned):** a wrong diagonal changes `rank_{F₂}(R)`, hence the
predicted `4-rank = t−1−rank(R)`, breaking the Rédei–Reichardt comparison. The
diagonal is fixed by §2.1–2.2, never tuned to make ranks agree (rule 1).

---

## 3. Phase 1B code map (this phase — convention-free forms layer)

| Object | Source | Code (`src/classgroup/`) |
|---|---|---|
| `(a,b,c)`, disc `b²−4ac` | — | `QForm{i128 a,b,c}`, `disc()` |
| primitive test `gcd(a,b,c)=1` | — | `is_primitive()` |
| ρ operator, reduced test | [Coh93 §5.6] | `rho()`, `is_reduced()` |
| reduce to a reduced form | [Coh93 §5.6] | `reduce()` |
| cycle of a reduced form | [Coh93 §5.6] | `cycle()` |
| narrow `h⁺` = #cycles | [Coh93 §5.2/5.6] | `narrow_class_number(D)` |
| Gauss composition | [Coh93 §5.4] | `compose()` |
| identity / principal form | — | `principal_form(D)` |

**Phase 1B tests:** `twin_classno_cycles_vs_composition` (h⁺ by cycle-count vs by
composition-based order-finding), `theorem_reduce_is_reduced_and_equivalent`,
primitivity, case counts certified, `oracle_narrow_class_number` vs `bnfnarrow`
(§1.3). No `redei_matrix`/`ClassGroup` yet — those are Phase 2.

## 4. Phase 2 preview (after sign-off)

`ClassGroup::compute` (invariants via composition + SNF), `rank2`, `rank4`;
`redei_matrix` + `fourrank_from_linking` consuming the Stage 1 matrix;
`theorem_genus_2rank`, `oracle` structure vs `bnfnarrow`,
`theorem_redei_reichardt_4rank` (form-side r₄ vs linking-side t−1−rank R),
stratified by corank; `classgroups.json` + viewer.
