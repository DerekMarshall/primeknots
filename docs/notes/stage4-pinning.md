# Stage 4 pinning log — arithmetic Chern–Simons / Dijkgraaf–Witten [PIN TO SOURCE, maximally]

**Status: Phase 1 deliverable, for review before any `cs/` code.** Resolves H1
(hypotheses), H2 (object map), H3 (p=2 vs odd-p) with verbatim quotes. Rule 5:
discrepancies recorded, not smoothed. **Contains an erratum against RESEARCH.md
§6** (H1).

Sources (committed `docs/papers/`):
- **[CKKPPY]** Chung–Kim–Kim–Pappas–Park–Yoo, *Abelian arithmetic Chern–Simons
  theory and arithmetic linking numbers*, arXiv:1706.03336. RESEARCH.md's named
  Stage 4 target (the "path-integral identity").
- **[CS2]** Chung–Kim–Kim–Park–Yoo, *Arithmetic Chern–Simons Theory II*,
  arXiv:1609.03012. Setup + decomposition formula.
- **[Hir]** Hirano, *On mod 2 arithmetic Dijkgraaf–Witten invariants for certain
  real quadratic number fields*, arXiv:1911.12964. The p=2 real-quadratic route.
- PARI/GP as referee where applicable.

---

## H1. HYPOTHESES FIRST — and an erratum against RESEARCH.md §6

### H1.1 What [CKKPPY] Theorem 1.1 actually requires (verbatim)

The main path-integral identity, [CKKPPY Thm 1.1], opens:

> "**Let p be an odd prime**, a = dim H¹(X,ℤ/pℤ), b = dim Ker(d), and {ξ_j} a
> finite set of homologically p-trivial ideals. … Then
> Σ_{ρ∈H¹(X,ℤ/pℤ)} exp[2πi((ρ,ρ) + Σ_j⟨ρ,[ξ_j]_p⟩)] = p^{(a+b)/2}(det(d̄)/p)
> i^{[(a−b)(p−1)²/4]} exp[−2πi(¼ Σ_{i,j} ht_p(ξ_i,ξ_j))]."

The standing setup, [CKKPPY §2] (verbatim):

> "Let F be a **totally imaginary** algebraic number field with ring of integers
> O_F such that **μ_{n²} ⊂ F**, and let X = Spec(O_F)."

[CS2 §1, p.1] independently: *"Let X = Spec(O_F)… We assume that F is totally
imaginary."* The totally-imaginary hypothesis is what makes `Inv: H³(X,μ_n) ≅
(1/n)ℤ/ℤ` (Artin–Verdier) hold with no real-place correction.

### H1.2 ERRATUM against RESEARCH.md §6

RESEARCH.md §6 states: *"implement LHS … and RHS … independently and assert
equality, **for p = 2 first (where Stage 1–3 machinery applies verbatim)**."*
This is **doubly unlicensed** by [CKKPPY Thm 1.1]:

1. **p = 2 is excluded.** Theorem 1.1 says "Let p be an **odd** prime." The
   `i^{[(a−b)(p−1)²/4]}` factor and the Gauss-sum evaluation are odd-p.
2. **ℚ is excluded as base field.** The setup needs F **totally imaginary** with
   **μ_{p²} ⊂ F**. ℚ is totally real and `μ_{p²} ⊄ ℚ`. So "over ℚ" is not in the
   theorem's hypotheses at all.

The paraphrase "Stage 1–3 machinery applies verbatim for p=2 over ℚ" was mine and
is **wrong**. Recorded here as the erratum; RESEARCH.md §6 should be annotated.

### H1.3 The minimal in-hypothesis instance — RECOMMENDED: the Hirano route

Candidates evaluated:
- **[CKKPPY Thm 1.1] verbatim (odd p, totally imaginary, μ_{p²}⊂F):** the smallest
  base field has `F ⊇ ℚ(ζ_{p²})` (e.g. p=3 ⇒ `ℚ(ζ_9)`, degree 6, class number 1).
  Reaching non-trivial linking needs larger totally-imaginary fields, p-th power
  residue symbols, and class groups of those fields — a large new
  infrastructure, disjoint from Stages 1–3. `F = ℚ(i)` (the user's candidate) has
  only `μ_4`, so it supports **n = 2 only** and does **not** satisfy Thm 1.1's
  odd-p requirement — it is a CS2/DW-mod-2 (not Thm 1.1) instance.
- **[Hir] mod-2, real quadratic `K = ℚ(√(p₁···p_r))`, `pᵢ ≡ 1 mod 4`
  (RECOMMENDED):** exactly the Stage 1–3 family, `p = 2`, DW invariant expressed
  in **Legendre symbols among the pᵢ (= the Stage 1 linking matrix) via Gauss
  genus theory (= Stage 3)**. This is *Hirano's extension* of Kim's theory to
  fields with real places (modified étale cohomology), **not** CKKPPY Thm 1.1 —
  but it is in-hypothesis for our family, reuses Stages 1–3, and preserves the
  punchline "the DW/CS partition function is a function of the linking matrix."

**Decision: Stage 4 targets the Hirano mod-2 real-quadratic DW invariant.** The
CKKPPY Thm 1.1 odd-p Gauss sum is recorded below (H2.2/H3) as the general theory
and a possible far-future direction, but is not the Stage-4 build target.

### H1.4 Hirano's own standing hypotheses (verbatim, [Hir §4.2])

> "we consider the case `K = ℚ(√(p₁p₂···p_r))`, where pᵢ is a prime number such
> that **pᵢ ≡ 1 mod 4**. Let n = 2, A = ℤ/2ℤ and c = id∪β(id) ∈ H³(A,ℤ/2ℤ).
> **Assume that the norm of the fundamental unit in O_K^× is −1.** Then the
> narrow ideal class group Cl_K⁺ is same as Cl_K. Note that the discriminant of K
> is p₁p₂···p_r because of the assumption, pᵢ ≡ 1 mod 4."

So the Hirano route requires: distinct primes `pᵢ ≡ 1 (mod 4)`, and **`N(ε_K) =
−1`** (negative Pell solvable ⇔ `Cl⁺ = Cl`). This is exactly the `pell_neg =
true` subset already emitted at Stage 3 — Stage 4 runs over those discriminants.
`X̄` is the Artin–Verdier compactification `X ⊔ X_∞` (real places included);
`n = 2` is forced for real K ([Hir §2]: *"if K has a real prime, n must be 2"*).

---

## H2. OBJECT MAP (paper → code)

### H2.1 Recommended route: Hirano mod-2, `K = ℚ(√(p₁···p_r))`

| Object | Source (verbatim identifiers) | Meaning / code |
|---|---|---|
| `X̄` | [Hir §2.1] Artin–Verdier site `X ⊔ X_∞` | compactified Spec O_K; not built (implicit) |
| `A`, `c` | [Hir §4.2] `A=ℤ/2ℤ`, `c=id∪β(id)∈H³(A,ℤ/2)` | the DW cocycle (fixed) |
| characters `M(X̄,A)` | `Hom_c(π₁(X̄),ℤ/2) = Hom(Cl_K⁺/2Cl_K⁺,ℤ/2) = Hom(T₊,ℤ/2)` | enumerate `ρ ∈ Hom(T₊,ℤ/2)` |
| `T₊` | [Hir §4.2] `{(x₁..x_r)∈(ℤ/2)^r : Σxᵢ=0}`, dim `r−1` | the genus group; `dim = r−1 = t−1` (Stage 3!) |
| `e⁺_{ij}` | [Hir §4.2] `(0,..,1,..,1,..,0)` (1 in i,j) | pair generator of `T₊` |
| genus iso (4.2.1) | `Cl_K⁺/2Cl_K⁺ ≅ T×`, `[𝔞]↦((N𝔞/p₁),…,(N𝔞/p_r))` | **Legendre symbols = Stage 1 data** |
| `lk₂(pᵢ,pⱼ)` | [Hir Cor 4.2.3] `(−1)^{lk₂}=(pᵢ/pⱼ)` | **the Stage 1 linking matrix entry** |
| CS action `CS_c(ρ)` | [Hir Cor 4.2.3] `= Σ_{i<j} ρ(e⁺_{ij}) lk₂(pᵢ,pⱼ)` | LHS phase (LINEAR in ρ) |
| partition fn `Z_c(X̄)` | [Hir Cor 4.2.4] `= Σ_{ρ∈Hom(T₊,ℤ/2)} ∏_{i<j}(pᵢ/pⱼ)^{ρ(e⁺_{ij})}` | the DW invariant |

Also [Hir Thm 4.2.2] (multiplicative form): `(−1)^{CS_c(ρ)} = ∏_{i<j,
ρ(e^×_{ij})=−1}(pⱼ/pᵢ)`.

**Failure modes:** wrong `T₊` (e.g. all of `(ℤ/2)^r` instead of the sum-zero
subgroup) → wrong character count `2^r` vs `2^{r−1}` → wrong `Z_c`. Wrong
`e⁺_{ij}` indexing → CS_c couples the wrong primes. Using ordinary `Cl` when
`N(ε)=+1` (Cl⁺≠Cl) → wrong character group (why we restrict to `pell_neg`).

**Numerical anchors ([Hir Ex 4.2.5], r=3):**
- `Z_c(ℚ(√(5·29·37))) = 0` (lk₂: 5–29=0, 29–37=1, 37–5=1).
- `Z_c(ℚ(√(5·13·73))) = 4` (all lk₂ = 1).

### H2.2 [CKKPPY Thm 1.1] RHS Gauss-sum factors (odd-p route, for the record)

Each factor of `p^{(a+b)/2}(det(d̄)/p) i^{[(a−b)(p−1)²/4]} exp[−2πi(¼Σht_p(ξ_i,ξ_j))]`:
- **power of p:** `p^{(a+b)/2}`, `a=dim H¹(X,ℤ/pℤ)`, `b=dim Ker(d)` [Thm 1.1].
- **Legendre determinant:** `(det(d̄)/p)`, `d̄: H¹/Ker(d) →≅ Im(d)` its dual;
  [CKKPPY p.3] *"the determinant is … well-defined modulo squares in ℤ/pℤ. (It is
  just the discriminant of the corresponding quadratic form.)"*
- **8th-root/`i`-power:** `i^{[(a−b)(p−1)²/4]}` [Thm 1.1] (`(p−1)²/4 ∈ ℤ` for odd p).
- **height pairing:** `exp[−2πi(¼ Σ_{i,j} ht_p(ξ_i,ξ_j))]`; `ht_p = ℓk_p` the
  arithmetic linking number `= ⟨d⁻¹[ξ_i], [ξ_j]⟩` [CKKPPY §2/§3], computable via
  **p-th power residue symbols** [CKKPPY Cor 3.11].
- **inv normalization:** `Inv: H³(X,μ_n) ≅ (1/n)ℤ/ℤ` [CKKPPY §2, Artin–Verdier].
- **the quadratic form / linking:** `d: H¹(X,ℤ/nℤ) →^ζ H¹(X,μ_n) →^δ H²(X,μ_n)
  →^r Ext²_X(ℤ/nℤ,𝔾_m) ≅ Cl(F)/n` [CKKPPY §2]; `(A,B)=⟨A,dB⟩` [§2].

---

## H3. p = 2 vs odd p (structure)

- **Odd p ([CKKPPY Thm 1.1]):** genuine non-trivial Gauss-sum identity with an
  `i`-power (8th-root) factor and `(det/p)` Legendre symbol; base field totally
  imaginary with `μ_{p²}`; linking via **p-th power residue symbols**. The two
  sides are a real identity (LHS phase sum vs RHS closed form).
- **p = 2 ([Hir], our route):** real quadratic K via **modified étale
  cohomology** (real places); linking via **Legendre symbols** (Stage 1); genus
  theory gives `dim T₊ = r−1` (Stage 3). Because `CS_c(ρ)` is **F₂-linear** in ρ,
  the character sum collapses to a closed form (H below): `Z_c ∈ {0, 2^{r−1}}`.
  No `i`-power subtlety — the mod-2 case is degenerate in that sense.

---

## Phase 2 protocol (pre-registered — executed only after sign-off)

Target: Hirano route, over the `pᵢ ≡ 1 mod 4`, `N(ε_K)=−1` (pell_neg) family.

- **LHS (brute character phase sum), own TU/session, sees only:** enumerate all
  `2^{r−1}` characters `ρ ∈ Hom(T₊,ℤ/2)`; `CS_c(ρ) = Σ_{i<j} ρ(e⁺_{ij})
  lk₂(pᵢ,pⱼ)` [Cor 4.2.3]; `Z = Σ_ρ (−1)^{CS_c(ρ)}`. `lk₂` from the Stage 1 matrix.
- **RHS (closed form), own TU/session, sees only:** since `CS_c(ρ)=ρ(w)` with
  `w = Σ_{i<j} lk₂(pᵢ,pⱼ)·e⁺_{ij} ∈ T₊` (F₂-linearity), `Z_c = 2^{r−1}` if `w=0`
  in `T₊`, else `0`. Concretely `w=0 ⇔ every row-sum Σ_{j≠i} lk₂(pᵢ,pⱼ)` is even
  — **the Rédei-matrix rows-sum condition from Stage 3, on the linking matrix.**
  Derived here and **verified against both [Ex 4.2.5] anchors** (5·29·37: row-sum
  of p=5 is 0+1=1 odd ⇒ 0; 5·13·73: all row-sums even ⇒ 2²=4). The Phase-2 RHS
  session re-derives this from the spec independently.
- **Neither side edited after the first comparison.** A mismatch is a deliverable
  (full witness: `{p₁..p_r}`, both values), resolved only by a corrected reading
  here with citation — never by editing a side to agree.
- **Invariance first:** independence of the character-basis of `Hom(T₊,ℤ/2)` and
  of the ordering of `S = {p₁,…,p_r}`, before any LHS/RHS comparison.
- **Case counts certified** per precedent (number of valid `pell_neg` r-tuples,
  independently counted). Anchors: `Z_c(5·29·37)=0`, `Z_c(5·13·73)=4`.
