# Stage 2 pinning log — Rédei symbol [PIN TO SOURCE]

**Status: Phase 1A deliverable, for review before Phase 2 (`redei_symbol`) is
authorized.** This log pins every entry condition and α-normalization congruence
of the Rédei symbol to the normative sources, mapped to the code identifiers the
Phase-2 implementation will use, each with the one-line failure mode the
invariance/anchor tests target. Where [S22] and [C07] state a condition
differently, both readings and the reconciliation are recorded — nothing is
silently chosen (CLAUDE.md rule 5).

Sources (committed to `docs/papers/`):
- **[S22]** P. Stevenhagen, *Rédei reciprocity, governing fields and negative
  Pell*, arXiv:1806.06250v3 (2021). Normative for the symbol. Page/eq refs below
  are to this file.
- **[C07]** J. Corsman, *Rédei Symbols and Governing Fields*, PhD thesis,
  McMaster (2007). The computational reference. Section/prop refs below.
- **[M04]** Morishita, *Milnor invariants and Massey products for prime numbers*,
  cited via [S22, p. 2] and [C07, §5.4] for the ≡1 mod 4 Massey-product reading.

## 0. Restricted setting (this project, Stages 1–3)

Distinct primes **p₁, p₂, p₃ ≡ 1 (mod 4)**, pairwise unlinked:
`(pᵢ/pⱼ) = 1` for all `i ≠ j` (equivalently `lk₂(pᵢ,pⱼ)=0` in the Stage 1
matrix). This is the cleanest case: [S22, p. 2] — *"In the case of prime
arguments a,b,c ≡ 1 mod 4, no dyadic ramification subtleties arise, and the
symbol has been interpreted by Morishita [M04] as an arithmetic Milnor
invariant, leading to a description as a triple Massey product."* We pin only
this case; general/composite/≡3-mod-4 arguments are explicitly out of scope.

## 1. Notation map (paper → code)

| Paper | Meaning | Planned code identifier |
|---|---|---|
| `a₁,a₂,a₃` [C07] / `a,b,c` [S22] | symbol arguments | `p1,p2,p3` (args of `redei_symbol`) |
| `K₁ = ℚ(√a₁)` | base quadratic field | implicit; `√p1` represented by pair `(x,y)` |
| `E = ℚ(√a₁,√a₂)` [S22 (31)] | biquadratic field | implicit |
| conic `x²−a₁y²−a₂z²=0` [S22 (26)] | soluble by entry conds | `solve_conic(p1,p2,·)` (Phase 1B, done) |
| `β = x+y√a₁` [S22 (33)], `α₂` [C07 L5.1.2] | norm `a₂z² ≡ a₂ mod □` | `beta=(x,y)` in ℚ(√p1) |
| `t`/`s` with `s²≡a₁ (mod p₃)` | splits p₃ in K₁ | `s = sqrt_mod(p1 % p3, p3)` (Stage 0) |
| `(α₂,π_𝔭)_{K₁,𝔭}` [C07 (5.5)] = `(β/𝔭)` [S22 (47)] | local symbol at 𝔭\|p₃ | `legendre_euler((x+y*s) % p3, p3)` |
| `[a₁,a₂,a₃] ∈ 𝔽₂` [C07 D5.2.3] / `{±1}` [S22] | the symbol | return of `redei_symbol` |

## 2. Entry conditions (when [p₁,p₂,p₃] is defined and *pure*)

**EC1 — arguments are distinct primes ≡ 1 (mod 4).**
- Source: restricted setting; [S22 (5)] signed prime discriminant `p*≡1 mod 4`;
  [C07 §5.4] the Massey form assumes `pᵢ≡1 mod 4`.
- Code: precondition asserts in `redei_symbol` (`is_prime(pᵢ)`, `pᵢ%4==1`,
  distinct).
- Drop it → for `p≡3 mod 4`, `Δ=4p` and `2∈S(p)`; the dyadic conditions of
  [S22 §7]/[C07 P5.2.1] activate and this log does not cover them.

**EC2 — pairwise unlinked: `(pᵢ/pⱼ)=1` for all `i≠j`.**
- Source: [S22 (1)] `(a,b)_p=(a,c)_p=(b,c)_p=1` at all `p`; [C07 P5.1.1] (a D₈
  representation exists iff `(a₁,a₂)_p=1 ∀p`, which for odd primes means
  `(a₁/a₂)=(a₂/a₁)=1`) and [C07 P5.2.1] (the a₃ conditions reduce, for primes, to
  `(p₃/p₁)=(p₃/p₂)=1`).
- Code: assert `legendre_euler(pᵢ,pⱼ)==1` (≡ `LinkingMatrix` entry `lk₂=0`).
- Drop `(p₁/p₂)=1` → conic `x²−p₁y²−p₂z²=0` is **insoluble**, no β exists.
  Drop `(p₁/p₃)=1` → p₃ does not split in `ℚ(√p₁)`, the local symbol `(β/𝔭₃)`
  is undefined.

**EC3 — discriminant coprimality `gcd(Δ(p₁),Δ(p₂),Δ(p₃))=1`.**
- Source: [S22 (2)].
- Code: automatic for distinct primes (`Δ(pᵢ)=pᵢ`).
- Drop it → [S22] shows the symbol is not well-defined. (Cannot occur here.)

**EC4 — at most one argument negative / the ∞-part.**
- Source: [S22 (44)] (`Art(𝔠)` if `c>0`, `Art(𝔠∞)` if `c<0`); [C07 P5.2.1] "at
  most one of a₁,a₂,a₃ negative".
- Code: all `pᵢ>0`, so the infinite prime contributes trivially; no `∞` handling
  needed.
- Drop it (c<0) → an `Art(∞,·)` factor enters [S22 (44)]; not relevant here.

## 3. Construction: conic → β  (convention-free part)

**N1 — the conic solution `(x,y,z)` must be PRIMITIVE (`gcd(x,y,z)=1`).**
- Source: [S22 Cor 7.4] — *"every **primitive** integral solution (x,y,z) to
  (26) yields an extension K⊂F(x,y,z) that is unramified at all odd primes"*;
  [S22 (26)] the conic; [C07 L5.1.2] chooses κ₁₂ ramified only where required.
- Code: `solve_conic` already returns primitive solutions (Phase 1B, verified).
- Drop primitivity → for an odd prime `q | gcd(x,y,z)`, `E(√β)/E` ramifies at
  `q`, so `F` is **not** minimally ramified and the symbol is wrong/ill-defined.
  This is why primitivity is a hard precondition, not a convenience.

**N2 — `β = x + y√p₁ ∈ ℚ(√p₁)`, with `N(β) = x² − p₁y² = p₂z² ≡ p₂ (mod □)`.**
- Source: [S22 (33)] `F = E(√β)`, `β = x+y√a`; [C07 L5.1.2] the element `α₂` with
  `N_{K₁/ℚ}(α₂)=a₂ mod ℚ*²` — Corsman's `α₂` **is** Stevenhagen's `β` (both have
  norm `a₂` up to squares).
- Code: `beta` carried as the integer pair `(x,y)` representing `x+y√p1`.
- Drop/replace β by the wrong element → wrong dihedral field `F`, wrong symbol.

## 4. Normalization at primes — the [PIN TO SOURCE] crux

**N3 — odd-prime ramification is handled entirely by primitivity (N1).**
- Source: [S22 Prop 7.2] (for odd `p`, `K⊂F` unramified over `p` when `p|Δ(a)`;
  else exactly one of `F, F_{p*}` is unramified) and [S22 Cor 7.4] (primitive ⇒
  unramified at all odd primes). [C07 L5.1.2] gives the same via κ₁₂.
- Consequence: the "congruence conditions at primes dividing p₁p₂" mentioned in
  `RESEARCH.md §4 step 2` are **subsumed by primitivity** in this restricted
  setting — no extra odd-prime congruence on `(x,y,z)` is needed.
- Drop → covered by N1's failure mode.

**N4 — 2-adic normalization: `E(√β)/E` must be UNRAMIFIED at 2. THE crux.**

Two readings, both recorded:

- **[S22] reading (explicit twist):** [S22 Def 7.6(2)] a minimally-ramified `F`
  must be unramified over 2 when `Δ(a)Δ(b)` is odd — here `Δ(p₁)Δ(p₂)=p₁p₂` is
  odd, so this applies. [S22 Prop 7.3(1)]: *"If Δ(a) is odd, then `Q⊂F_t` is
  unramified at 2 for a unique `t∈{±1,±2}`."* [S22 Cor 7.4]: the unramified field
  is `F_t` with `β_t = t·(x+y√d₁)`. The twisting subgroup [S22 (43)]
  `T_{p₁,p₂}=⟨p₁*,p₂*⟩` for odd `Δ(a),Δ(b)` **excludes −1 and 2**, so a twist by
  `t∈{±1,±2}` is *not* absorbed and **does** change the symbol value by
  `(t/p₃)`. The condition for `t=1` (raw β already unramified at 2) is
  `β ≡ □ (mod 4)` in `O_{ℚ(√p₁)}` — [S22 Prop 7.3 proof]: *"for a 2-unit to have
  a square root that is unramified at 2, we need … it is a square modulo 4"*;
  worked in [S22 Ex 5.5].

- **[C07] / [M04] reading (no dyadic condition on ≡1-mod-4 arguments):** [C07
  P5.2.1] imposes an extra 2-adic condition (`a₃≡1 mod 8` and `(2,aᵢ)₂=1`) **only
  in the case `a₁≡a₂≡2 or 3 mod 4`** — *not* triggered when `p₁≡p₂≡1 mod 4`.
  [C07 D5.2.6/P5.2.7] (pure symbols): the only 2-adic requirement is when
  `2∈S(aᵢ)∩S(aⱼ)`, which cannot happen for odd `pᵢ≡1 mod 4` (2 is unramified in
  `ℚ(√pᵢ)`). [C07 §5.4] / [M04]: for `pᵢ≡1 mod 4`, `[p₁,p₂,p₃]=tr_{p₃}⟨χ_{p₁},
  χ_{p₂},χ_{p₃}⟩` — a clean triple Massey product with **no** 2-adic twist.

- **Reconciliation (the reading we pin):** The two are consistent. The *value* is
  `(β_min/𝔭₃)` for the 2-**minimally**-ramified `β_min = t·β`. For `pᵢ≡1 mod 4`,
  the sources agree the symbol is a clean Milnor/Massey invariant — i.e. the
  correct `t` is determined intrinsically, not by a 2-adic *condition on the
  arguments*. Whether the **raw primitive β already satisfies `β≡□ mod 4`** (so
  `t=1`) for every valid triple, or whether the specific `t∈{±1,±2}` making
  `β≡□ mod 4` must be applied per solution, is the single open realization
  question — and it is **structurally decidable, not a convention to choose**:
  `invariance_redei_solution_choice` (Phase 2, run FIRST per
  [[invariance-is-senior-to-anchor]]) computes the symbol from several *different*
  primitive solutions; if raw β is under-normalized, different solutions with
  different `β mod 4` yield different `(β/𝔭₃)` and the invariance test **fails**.
- Drop N4 (use un-normalized β) → value corrupted by `(t/p₃)`;
  `invariance_redei_solution_choice` fails. **This is precisely the test that
  guards N4.**

**Phase-2 protocol for N4 (rule 1):** implement `β_min` = twist `β` by the unique
`t∈{±1,±2}` making `β ≡ □ (mod 4)` in `O_{ℚ(√p₁)}` (the [S22] recipe), *then*
run `invariance_*` FIRST. If invariance holds and the reciprocity sweep +
anchor agree, the normalization is confirmed. If the anchor `[13,61,937]=−1`
disagrees, the fix is a **corrected reading here with a citation** (e.g. the
exact `O_{ℚ(√p₁)}`-basis used for `mod 4`, or the `t=1` sufficiency for
≡1-mod-4) — **never** a sign flip. Irreconcilable ambiguity → `open-questions.md`,
stop the thread.

## 5. Computational formula and the sqrt-branch small theorem

For `p₃` split in `ℚ(√p₁)` (guaranteed by EC2, `(p₁/p₃)=1`), with `s²≡p₁ (mod
p₃)` (Tonelli–Shanks), the prime `𝔭₃` above p₃ has residue field `𝔽_{p₃}` and
`√p₁ ↦ s`. Then [S22 (47),(48)] / [C07 (5.5)]:

    [p₁,p₂,p₃] = (β_min / 𝔭₃) = ( (x_min + y_min·s) / p₃ )   ∈ {±1},

where `(x_min,y_min)` are the coordinates of `β_min` (N4). Code:
`legendre_euler((x_min + y_min*s) mod p3, p3)`.

**Sqrt-branch invariance (a small theorem, not a convention):** the other root
`−s` gives `((x−ys)/p₃)`, and the product of the two is
`((x²−y²p₁)/p₃) = ((p₂z²)/p₃) = (p₂/p₃) = 1` by EC2. So both roots give the same
value ⇒ `invariance_sqrt_branch`. [RESEARCH.md §4]; [C07 P5.2.2] (independence of
the choice of prime above a split p).

## 6. Value convention

`[·]` is reported in `{±1}` (matching the anchor statement `[13,61,937]=−1`),
with the `𝔽₂` bit taken as `(value == −1)` — i.e. `−1 ⇔ 1 ∈ 𝔽₂ ⇔ "triple
linked"`. [S22, p. 8] notes the traditional `{±1}` vs `𝔽₂` ambiguity; we fix the
`{±1}` convention for the value and expose the `𝔽₂` bit for Stage 3 linear
algebra. This convention is a *reporting* choice (which representation we print),
not a normalization of the symbol; it does not touch N1–N4.

## 7. Anchor

`[13, 61, 937] = −1` — the standard Borromean triple [M12], pairwise unlinked
and triple-linked. Necessary but not sufficient (rule 1): it is chaperoned by
`invariance_*` (senior) and `theorem_redei_reciprocity_s3` before Stage 2 is
green.
