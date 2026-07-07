# RESEARCH.md — Mathematical Background and Verification Targets

Arithmetic topology as an executable project: numerically verify, in from-scratch C++, the chain of theorems running from quadratic reciprocity through Rédei symbols and class-group 2-structure up to the abelian arithmetic Chern–Simons partition function, with the Weil explicit formula as the Deninger-program capstone. Each stage verifies a **named theorem**. The organizing object is the **mod-2 linking matrix of primes**, built in Stage 1 and load-bearing in every stage after.

## 0. Epistemic rules for this document

1. This document is a *map*, not a *source*. Where a construction has delicate normalizations (Rédei symbol, Rédei matrix diagonal, CS cochain conventions), the cited paper is normative and this document is a paraphrase. Discrepancy between code and this document → check the paper. Discrepancy between code and the paper → the code is wrong.
2. "Provable" in this project means: a named theorem is verified numerically over an exhaustive or property-sampled range, with two independent implementations and/or an external oracle (PARI/GP, LMFDB) as referee. It does not mean formal proof. (A Lean port is a conceivable far-future stage; out of scope.)
3. Anything below marked **[CONJECTURAL]** is a research-level open problem. We compute *around* those objects (models, dualities, special cases); we do not claim to compute the objects themselves.
4. Anything marked **[PIN TO SOURCE]** has known-fiddly conventions. Do not improvise; do not let a test pass by adjusting a convention until the target value appears. That failure mode — fitting normalizations to the expected answer — destroys the entire point of the project.

## 1. The dictionary (context for everything)

Arithmetic topology (Mazur; Morishita; Kapranov; Reznikov — "M²KR") treats the Artin–Verdier-compactified Spec(ℤ) as an analogue of S³ and prime ideals as knots: Spec(𝔽_p) has étale fundamental group Ẑ, the profinite circle. The working entries for this project:

| Topology | Arithmetic | Where it becomes code |
|---|---|---|
| 3-sphere S³ | Spec(ℤ) ∪ {∞} | ambient context |
| Knot | Prime p (Spec 𝔽_p ↪ Spec ℤ) | `PrimeSet` |
| Linking number lk(K,L) mod 2 | Legendre symbol (p/q) | Stage 1 |
| lk(K,L) = lk(L,K) | Quadratic reciprocity | Stage 0 verification |
| Milnor invariant μ̄(123) / triple Massey product | Rédei symbol [p₁,p₂,p₃] | Stage 2 |
| Borromean rings | Borromean prime triples | Stage 2 |
| H₁(M, ℤ) | (Narrow) ideal class group | Stage 3 |
| Linking matrix determines homology of branched cover | Rédei–Reichardt 4-rank theorem | Stage 3 |
| Chern–Simons / Dijkgraaf–Witten partition function | CKKPY/CKKPPY arithmetic CS partition function | Stage 4 |
| Closed orbits of a flow ↔ spectrum (trace formula) | Primes ↔ zeta zeros (Weil explicit formula) | Stage 5 |
| Counting flat G-bundles | Counting Galois covers (e.g. cubic fields for S₃) | Stage 6 |

Primary background text: Morishita, *Knots and Primes* [M12]. Best short on-ramp for the gauge-theory stages: Kim, *Arithmetic Gauge Theory: A Brief Introduction* [K18].

## 2. Stage 0 — Primitives and reciprocity-as-linking-symmetry

**Objects.** Modular arithmetic on `uint64_t`/`unsigned __int128`; deterministic Miller–Rabin (the known witness sets are deterministic for all n < 3.3×10²⁴, far beyond our range); Legendre symbol; Jacobi symbol; Tonelli–Shanks square roots mod p.

**Dual implementation (mandatory).** Legendre symbol computed two independent ways:
- (a) Euler's criterion: (a/p) ≡ a^((p−1)/2) mod p, via modpow;
- (b) Jacobi-symbol reciprocity recursion (binary algorithm), specialized to prime modulus.
Assert agreement over exhaustive ranges (all p < 10⁶, sampled a) and randomized property tests. Cross-check a sample against PARI's `kronecker`.

**Named theorem verified.** Quadratic reciprocity, in linking form: for distinct primes p, q ≡ 1 (mod 4), (p/q) = (q/p). Verify exhaustively for all such pairs below a bound. Also verify the supplementary laws ((−1/p), (2/p)) since Stage 2 normalization logic depends on them.

**Caveats.** None mathematical. Engineering: overflow discipline (all intermediate products through u128), and Tonelli–Shanks must be tested on both residues and non-residues (must *detect* non-residues, not loop).

## 3. Stage 1 — The linking matrix of primes

**Definition.** For distinct primes p, q ≡ 1 (mod 4), define lk₂(p, q) ∈ 𝔽₂ by (p/q) = (−1)^{lk₂(p,q)}. Reciprocity (Stage 0) makes this symmetric — this is precisely lk(K,L) = lk(L,K). We restrict to p ≡ 1 (mod 4) throughout Stages 1–3: it makes the symbol symmetric, satisfies the Rédei-symbol entry conditions, and matches the cleanest case of the 4-rank theorem. (Extension to p ≡ 3 mod 4 introduces asymmetry corresponding to orientation issues; a documented later extension, not a Stage 1 concern.)

**Deliverable.** `LinkingMatrix`: bit-packed symmetric 𝔽₂ matrix over the first N primes ≡ 1 (mod 4), with (i) fast pair lookup, (ii) submatrix extraction for arbitrary prime subsets (Stages 3–4 consume this), (iii) 𝔽₂ rank via bitset Gaussian elimination, (iv) JSON emission.

**Verification.** Symmetry assertion over the whole matrix (this re-verifies reciprocity at scale); statistical sanity: linking density → 1/2, and the matrix should pass basic randomness diagnostics (it is conjecturally "random-like"; we record statistics, we do not assert a theorem).

**Visualization.** Heatmap of the matrix; force-directed graph (nodes = primes, edges = linked pairs); side-by-side rendered Hopf link / unlink for a selected pair. This is the project's signature image: the link diagram of the primes.

## 4. Stage 2 — Rédei symbols, Rédei reciprocity, Borromean primes

**The object.** The Rédei symbol [a₁, a₂, a₃] ∈ {±1}, introduced by Rédei (1939) for the study of 8-ranks of narrow class groups, is the arithmetic triple linking number: Morishita [M04] identifies it with the mod-2 triple Milnor invariant μ̄(123), i.e. a Massey product in Galois cohomology. **[PIN TO SOURCE]** — the entry conditions and normalization are the notoriously delicate part; Rédei's original treatment had gaps, and the modern, correct definition with full symmetry is due to Stevenhagen [S22]. Normative sources, in order: Stevenhagen [S22]; Corsman's thesis [C07] (the most explicitly computational treatment — use it as the implementation reference); Morishita [M12, ch. 8] for the topological interpretation.

**Restricted setting for this project.** Distinct primes p₁, p₂, p₃ ≡ 1 (mod 4) with all pairwise Legendre symbols equal to 1 (pairwise unlinked). Sketch of the computation (paraphrase — see rule 4):

1. Solve x² − p₁y² − p₂z² = 0 in coprime integers. Legendre's theorem guarantees a nontrivial solution given the entry conditions; Holzer's bound guarantees one with |x| ≤ √(p₁p₂), so bounded search terminates — but implement the Cremona–Rusin conic algorithm [CR03] for speed, with brute-force-under-Holzer-bound as the verification twin.
2. Form α = x + y√p₁ ∈ ℚ(√p₁), subject to the normalization conditions on (x, y, z) (congruence conditions at 2 and at primes dividing p₁p₂ ensuring the quadratic extension E(√α)/E, E = ℚ(√p₁, √p₂), is unramified where required — take these verbatim from [S22]/[C07]).
3. [p₁, p₂, p₃] is the splitting behavior of p₃ in E(√α)/E. Concretely: choose t with t² ≡ p₁ (mod p₃) (Tonelli–Shanks), reduce α mod a prime above p₃, and evaluate ((x + yt)/p₃).

**Choice-independence as property tests.** The symbol must not depend on (i) which solution (x,y,z) is used (test: generate several essentially different solutions, assert equal symbols), or (ii) which square root t is chosen (note: the two choices give conjugate values of α, and the product of the two resulting Legendre symbols is (N(α)/p₃) = (p₂z²/p₃) = 1 under the entry conditions — so invariance is itself a small theorem your tests confirm). Any choice-dependence observed = normalization bug.

**Named theorems verified.**
- **Anchor value:** [13, 61, 937] = −1, the standard Borromean triple (pairwise unlinked, triple-linked) [M12].
- **Rédei reciprocity:** full symmetry of [a₁, a₂, a₃] under all six permutations of the arguments [S22]. Verify over every valid triple in range. This is the deep one — the arithmetic analogue of the symmetry of Milnor invariants — and its rigorous proof in the correct generality is *2022 mathematics*. Numerically confirming it across thousands of triples with independently normalized computations is a real test of both the code and the paraphrase above.

**Discovery component.** Scan for Borromean triples (pairwise unlinked, [·,·,·] = −1) beyond the textbook example. No canonical published table of these exists at scale; a verified table is a genuine (minor) contribution and a natural artifact for the repo.

**Visualization.** 3-uniform hypergraph of Borromean triples layered over the Stage 1 linking graph; render Borromean rings on selection.

## 5. Stage 3 — Class groups as H₁ and the Rédei–Reichardt 4-rank theorem

**Objects.** Binary quadratic forms (a, b, c) of discriminant D; reduction; Gauss composition; the (narrow) class group Cl⁺(D) as the group of classes of primitive forms. Implementation bible: Cohen [Coh93, ch. 5]; also Buell [B89]. Values through i128 with a GMP escape hatch for large D.

**Dual implementation.** Class number two ways: (a) enumeration of reduced forms; (b) group structure via composition (element orders, Smith normal form of relations). Cross-check h(D) and the group structure against PARI (`quadclassunit`) as oracle. Optional analytic sanity check: numerically compare against the class number formula via truncated L(1, χ_D) — a consistency plot, not a proof.

**Named theorems verified.**
- **Genus theory (Gauss):** for the discriminants in our restricted family (D = p₁⋯p_t, distinct primes ≡ 1 mod 4), the 2-rank of Cl⁺(D) is t − 1. Verify by computing Cl⁺(D)[2] from the form group directly.
- **Rédei–Reichardt (1934):** the 4-rank of Cl⁺(D) equals t − 1 − rank_{𝔽₂}(R), where R is the **Rédei matrix**: off-diagonal R_{ij} = lk₂(p_i, p_j) from the Stage 1 matrix, diagonal R_{ii} fixed by the condition (−1)^{R_{ii}} = ((D/p_i) / p_i), equivalently rows sum to 0 in this restricted family. **[PIN TO SOURCE]** for the diagonal convention and for any extension beyond the restricted family (primes ≡ 3 mod 4 and p = 2 change the matrix shape and the narrow/ordinary class group distinction matters) — normative: [S22] and its references; [C07].

This is the project's central punchline made executable: **compute the class group two completely independent ways** — form composition (the "homology" side) and 𝔽₂ linear algebra on the linking matrix (the "linking" side) — **and assert the 4-ranks agree** across thousands of discriminants. Linking data controlling H₁, as a test suite. (Context, not scope: this Rédei-matrix technology, pushed much further, powers Alexander Smith's breakthrough on the Cohen–Lenstra 2^k-rank distribution and the Koymans–Pagano proof [KP22] of Stevenhagen's negative-Pell conjecture. The 8-rank connects to Stage 2's symbol — a documented stretch direction.)

**Visualization.** Cayley graphs of Cl⁺(D) colored by verification status; scatter of 4-rank vs. linking-matrix corank (should be a perfect diagonal); distribution histograms vs. Cohen–Lenstra-style predictions (recorded as empirics, not asserted).

## 6. Stage 4 — The abelian arithmetic Chern–Simons partition function

**The theory.** Kim [K15] defines, for a finite group G and c ∈ H³(G, ℤ/n), an arithmetic Chern–Simons action CS_c(ρ) = inv(ρ*(c)) on Galois representations of π₁ of (compactified) Spec(𝒪_F), via inv: H³ ≅ ℚ/ℤ from Artin–Verdier duality. CKKPY [CKKPY16] develop boundaries (ramified sets S, local–global gluing via Poitou–Tate) and a decomposition formula for computation. The Stage 4 target is the **abelian** theory of Chung–Kim–Kim–Pappas–Park–Yoo [CKKPPY17], *Abelian arithmetic Chern–Simons theory and arithmetic linking numbers*: a finite Gaussian-sum identity — the "arithmetic path integral" — in which:

- the **LHS** is a brute-force sum of quadratic-form phases exp(2πi(⟨ρ, dρ⟩-type CS action + linear source terms)) over the *finite* set of mod-p characters ρ ∈ H¹ (concretely enumerable via S-units modulo squares/p-th powers — a 2^{O(t)}-ish enumeration for our prime sets);
- the **RHS** is a closed-form Gauss sum whose ingredients are: a power of p, a Legendre symbol of the determinant of the relevant quadratic form — built from **the Stage 1 linking data** — an 8th-root-of-unity factor, and mod-n **height pairings** ht(ξ_i, ξ_j) of the source classes, themselves computed from power-residue symbols.

**Named theorem verified.** The main path-integral identity of [CKKPPY17] (Theorem 1.1 there): implement LHS (brute-force phase sum) and RHS (closed-form Gauss sum) **independently** and assert equality, for p = 2 first (where Stage 1–3 machinery applies verbatim), across many prime sets S; then mod small odd p using n-th power residue symbols (generalize the Stage 0 symbol layer accordingly).

**[PIN TO SOURCE], maximally.** Translating the paper's cohomological objects (which H¹ exactly; which quadratic form; which normalization of inv; which congruence/homological-triviality conditions on the ξ_j) into the explicit character enumeration is itself a substantial deliverable of this stage — expect it to be the majority of the work. A match obtained by tweaking a sign convention until LHS = RHS proves nothing; the two sides must be pinned to the paper independently, by different sessions/reviewers if possible, before being compared. Log the object-by-object identification (paper notation → code identifier) in `docs/notes/stage4-pinning.md` as it is established.

**Why this is the payoff stage.** The RHS says the TQFT partition function of the number ring is a function of the linking matrix. Stage 1's data structure → Stage 3's homology control → Stage 4's quantum invariant, one object all the way up.

**Visualization.** Partition function values plotted on ℂ (they land on scaled 8th roots of unity), colored/faceted by properties of the prime set (t, det of linking matrix, 4-rank). Phase structure of a TQFT on number rings, as a picture.

## 7. Stage 5 — Deninger's program: orbit/spectrum duality, computed

**Status framing.** Deninger conjectures **[CONJECTURAL]** a foliated dynamical system realizing compactified Spec(ℤ), with finite primes as closed orbits of length log p and zeta as a regularized determinant over foliation cohomology [D98], [D02]. No such system has been constructed; that construction *is* the program. What is unconditionally real — and computable — is the duality the system is meant to geometrize: the **Weil explicit formula**, in which primes (orbit side) and zeta zeros (spectral side) determine each other.

**Computations.**
1. **Zeros.** Implement Riemann–Siegel (main sum + first correction terms; Edwards [E74] is the implementation source) to locate the first few thousand nontrivial zeros; verify sign changes of Z(t), Gram-point bookkeeping, and cross-check every computed zero against Odlyzko's tables [O] to ≥ 8 decimals. Odlyzko is the oracle; our zeros are the verified twin.
2. **Explicit formula, animated.** ψ(x) = x − Σ_ρ x^ρ/ρ − log 2π − ½·log(1 − x⁻²): reconstruct the prime-power staircase by partial sums over our computed zeros, and render the animation of waves (one per zero) converging onto ψ(x). This is the single best picture in the subject and it is *exactly* the orbit-counting/spectrum trace duality Deninger wants to realize geometrically. Quantitative verification: max-error of the partial reconstruction vs. number of zeros used, against directly computed ψ(x).
3. **Model system with real geometry.** Implement a genuine suspension flow where the dynamical zeta function ζ_flow(s) = Π_γ (1 − e^{−s·ℓ(γ)})^{−1} is computable from finite data — e.g. the suspension of a subshift of finite type / hyperbolic toral automorphism, where the Ruelle zeta is an explicit rational expression in the transfer matrix (Parry–Pollicott [PP90]). Verify Euler product = determinant identity numerically. Displayed next to the arithmetic case, this makes the Deninger gap *visible*: here is a system where orbits exist and the identity is geometry; here is arithmetic, where the identity holds and the geometry is missing.

## 8. Stage 6 (stretch) — Non-abelian Dijkgraaf–Witten counting

For finite non-abelian G, the untwisted DW invariant of Spec(ℤ[1/S]) counts homomorphisms π₁ → G (weighted by 1/|G|, groupoid cardinality). For G = S₃, surjections onto S₃ unramified outside S correspond to non-Galois cubic fields with prescribed ramification: the count is verifiable against LMFDB's cubic field tables, and against PARI (`nfinit`/`polsubcyclo`-style enumeration or direct cubic discriminant search) for small conductors. Twisted versions and real-place subtleties: Hirano [H19], Lee–Park [LP19], BCGKPT [BCGKPT17]. This stage turns "finite non-abelian gauge group" into a tabulation with two referees.

## 9. Explicitly out of scope

- Constructing Deninger's foliated system, proving anything about zeta zeros, or any "chaotic operator" material (see the roadmap document's exclusion section — the O_x literature fails basic coherence tests and is not part of this project in any form).
- p-adic Lie gauge groups (GL_n(ℤ_p)) in Stage 4 — aspirational research, no established theory to verify.
- Formal (Lean/Coq) proof — different project.

## 10. References

Verified identifiers (arXiv IDs confirmed against arXiv/nLab/publisher):

- **[K15]** M. Kim, *Arithmetic Chern–Simons Theory I*, arXiv:1510.05818; published in *Galois Covers, Grothendieck–Teichmüller Theory and Dessins d'Enfants*, Springer PROMS 330 (2020).
- **[CKKPY16]** H.-J. Chung, D. Kim, M. Kim, J. Park, H. Yoo, *Arithmetic Chern–Simons Theory II*, arXiv:1609.03012; published in *p-adic Hodge Theory*, Simons Symposia, Springer (2020).
- **[CKKPPY17]** H.-J. Chung, D. Kim, M. Kim, G. Pappas, J. Park, H. Yoo, *Abelian arithmetic Chern–Simons theory and arithmetic linking numbers*, arXiv:1706.03336. **Stage 4 primary source.**
- **[BCGKPT17]** F. Bleher, T. Chinburg, R. Greenberg, M. Kakde, G. Pappas, M. Taylor, *Unramified arithmetic Chern–Simons invariants*, arXiv:1705.07110.
- **[H19]** H. Hirano, *On mod 2 arithmetic Dijkgraaf–Witten invariants for certain real quadratic number fields*, arXiv:1911.12964.
- **[LP19]** J. Lee, J. Park, *Arithmetic Chern–Simons theory with real places*, arXiv:1905.13610.
- **[K18]** M. Kim, *Arithmetic Gauge Theory: A Brief Introduction*, Mod. Phys. Lett. A 33 (2018) 1830012; arXiv:1712.07602. **Best entry point.**
- **[S22]** P. Stevenhagen, *Rédei reciprocity, governing fields and negative Pell*, Math. Proc. Camb. Phil. Soc. 172 (2022) 627–654; arXiv:1806.06250. **Stage 2 normative source.**
- **[C07]** J. Corsman, *Rédei symbols and governing fields*, PhD thesis, McMaster University (2007). **Stage 2 implementation reference.**
- **[KP22]** P. Koymans, C. Pagano, *On Stevenhagen's conjecture*, arXiv:2201.13424. (Context for Stage 3 stretch.)

From standard literature (publication data from memory — verify page numbers on first citation in any write-up):

- **[M12]** M. Morishita, *Knots and Primes: An Introduction to Arithmetic Topology*, Springer Universitext, 2012 (2nd ed. 2024). **Project-wide background text.**
- **[M04]** M. Morishita, *Milnor invariants and Massey products for prime numbers*, Compositio Math. 140 (2004).
- Mazur's unpublished notes on the Alexander polynomial (c. 1963–64); Kapranov and Reznikov's papers of the 1990s — historical anchors of M²KR, cited via [M12]'s bibliography.
- **[D98]** C. Deninger, *Some analogies between number theory and dynamical systems on foliated spaces*, Doc. Math., Extra Vol. ICM I (1998).
- **[D02]** C. Deninger, *A note on arithmetic topology and dynamical systems*, Contemp. Math. 300 (2002).
- **[Coh93]** H. Cohen, *A Course in Computational Algebraic Number Theory*, Springer GTM 138. **Stage 3 implementation bible.**
- **[B89]** D. Buell, *Binary Quadratic Forms*, Springer 1989.
- **[CR03]** J. Cremona, D. Rusin, *Efficient solution of rational conics*, Math. Comp. 72 (2003). **Stage 2 conic solver.**
- **[E74]** H. M. Edwards, *Riemann's Zeta Function*, Academic Press 1974 (Dover reprint). **Stage 5 Riemann–Siegel source.**
- **[PP90]** W. Parry, M. Pollicott, *Zeta functions and the periodic orbit structure of hyperbolic dynamics*, Astérisque 187–188 (1990). **Stage 5 model system.**
- **[O]** A. Odlyzko, tables of zeros of the zeta function (online). **Stage 5 oracle.**
- Rédei's originals: L. Rédei (& H. Reichardt), J. reine angew. Math. 170 (1934) — the 4-rank theorem; L. Rédei, J. reine angew. Math. 180 (1939) — the triple symbol. Cited for history; use [S22]/[C07] for definitions.

Oracles: PARI/GP (`kronecker`, `quadclassunit`, `qfbclassno`, number-field routines) and LMFDB (cubic fields, zeta zeros metadata). Oracles referee; they never replace our implementations.
