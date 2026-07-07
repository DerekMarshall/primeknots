# Stage 6 pinning — S₃ Dijkgraaf–Witten counting vs cubic fields (Phase 1)

**Scope of this document.** Phase 1, pinning only — *no* `src/dw/` code. It fixes
the object, the archimedean-place convention (H1), the by-image decomposition with
derived multiplicities (H2), the field-side dictionary and wild-ramification scope
(H3), the two internal free theorems (H4, H5), and the LMFDB query semantics (H6).
Per the authorization, the assembled multiplicities are to be **refereed by hand**
by the user before Phase 2; where the derivation and the rider phrasing appear to
diverge, this document flags it rather than smoothing it (CLAUDE.md rule 5).

**Citation honesty.** In-repo sources quoted verbatim: CS II [CKKPY16]
(`docs/papers/cs2-1609.03012.pdf`). Sources not in-repo (books / papers we have
not committed) — Neukirch–Schmidt–Wingberg [NSW], Lee–Park [LP19], BCGKPT
[BCGKPT17], the LMFDB schema — are cited and any page/label-level claim from them
is tagged **UNVERIFIED** per non-negotiable #6. Where a convention can be settled
by a self-contained arithmetic witness rather than a citation, the witness leads.

---

## 0. The object

    Z_DW(X) = |Hom(π₁ᵉᵗ(X), S₃)| / |S₃|,     X = Spec ℤ[1/S],  S a finite set of primes,

untwisted (trivial cocycle), groupoid cardinality weight 1/|S₃| = 1/6
(RESEARCH.md §8). π₁ᵉᵗ(X) is the profinite étale fundamental group.

CS II sets up exactly this π for a number field F (verbatim, p. 2–3):

> "Given a finite set S of primes, we consider the scheme
>  X_S := Spec(𝒪_F[1/S]) = X \ {𝔓_v}_{v∈S}."

> "π = π₁(X, 𝔟), be the profinite étale fundamental group of X, where we take
>  𝔟 : Spec(F̄) → X to be the geometric point coming from an algebraic closure
>  of F."

and the object counted (verbatim, p. 3):

> "𝓜(A) := Hom_cont(π, A)/A be the set of isomorphism classes of principal
>  A-bundles over X."

So |Hom_cont(π₁ᵉᵗ(X_S), S₃)| is the honest count; Z_DW divides by 6. For us
F = ℚ, A = S₃.

---

## H1 — The archimedean ("infinity") convention  (this stage's N4)

**The question.** Does π₁ᵉᵗ(Spec ℤ[1/S]) constrain ramification at the real place
∞ of ℚ? The two readings give *different populations* of cubic fields, and a
signature mismatch produces a wrong count that looks clean.

**What CS II actually does: it sidesteps the question.** CS II fixes the ground
field to be totally imaginary (verbatim, p. 1):

> "Let X = Spec(𝒪_F), the spectrum of the ring of integers in a number field F.
>  We assume that F is totally imaginary."

A totally imaginary F has **no real places**, so the "ramification at ∞" question
never arises there — CS II avoids it by hypothesis (the same totally-imaginary
hypothesis that forced the Stage 4 erratum, see `stage4-pinning.md` H1). CS II
therefore does **not** pin the F = ℚ convention. The real-place theory is the
subject of Lee–Park [LP19] *Arithmetic Chern–Simons theory with real places* and
BCGKPT [BCGKPT17]; neither is in-repo (**UNVERIFIED** at page level).

**Decision — Convention A (∞ unconstrained), pinned by a self-contained witness.**
π₁ᵉᵗ(Spec ℤ[1/S]) is the étale fundamental group of the *scheme* Spec ℤ[1/S]. Its
finite connected covers are Spec 𝒪_K[1/S] for number fields K/ℚ whose ring of
integers is unramified over ℤ[1/S] — i.e. unramified at every **finite** prime
p ∉ S. The archimedean place ∞ is **not a point of the scheme** Spec ℤ[1/S], so
étale-ness imposes no condition there.

*Witness (rigorous, no citation needed).* ℚ(i): ℤ[i]/ℤ is ramified only at 2
(disc = −4), so Spec ℤ[i][1/2] → Spec ℤ[1/2] is a finite étale cover. Yet ℚ(i)
is imaginary — the real place of ℚ becomes a complex place ("ramifies at ∞").
The étale cover exists regardless. Hence π₁ᵉᵗ(Spec ℤ[1/2]) sees ℚ(i): **∞ is
unconstrained**, and imaginary/complex extensions are included.

**Citation for the general statement.** π₁ᵉᵗ(Spec 𝒪_{k,S}) ≅ G_S(k) = Gal(k_S/k),
the Galois group of the maximal extension unramified outside the finite primes of
S — NSW, *Cohomology of Number Fields* (**UNVERIFIED** page; the witness above is
the binding justification).

**Consequence, stated explicitly.**
- **Convention A (adopted): ∞ unconstrained.** Cubic fields of **both signatures**
  count — totally real (3,0), disc > 0, *and* complex (1,1), disc < 0 — provided
  they are unramified outside the finite primes S.
- **Convention B (compactified / totally-real): only (3,0).** If one instead uses
  the Arakelov-compactified curve Spec ℤ ∪ {∞} / the real-places fundamental group
  of Lee–Park [LP19], ∞ is required unramified, forcing totally real cubics only.
  We are **not** adopting this; it is recorded as the alternative.

**Population every downstream referee is asked for (Convention A):**
*all* cubic fields K with rad(disc K) ⊆ S, **both** signs of disc(K) — i.e. the
LMFDB query and the PARI enumeration (H6, Phase 2) must return signature (3,0)
**and** (1,1). Filtering to one signature here would silently answer Convention B.

---

## H2 — Decomposition by image, with derived multiplicities

Every φ ∈ Hom(π, S₃) has image a subgroup of S₃: {e}, one of three order-2
subgroups ⟨(12)⟩,⟨(13)⟩,⟨(23)⟩ (all conjugate), the unique normal order-3 subgroup
A₃ = ⟨(123)⟩, or all of S₃. Counting homs by image:

    |Hom(π, S₃)| = 1  +  3·(2^a − 1)  +  (3^k − 1)  +  2·c
                   └─┘   └─────────┘    └───────┘     └──┘
                  triv     image C₂      image C₃    image S₃

**Term-by-term derivation** (this is the part to referee by hand):

- **Trivial — count 1.** The zero map.

- **Image order 2 — count 3·(2^a − 1).** A hom with image an order-2 subgroup H is
  a surjection π ↠ H ≅ C₂. The number of surjections onto a *fixed* C₂ equals the
  number of nontrivial homs π → C₂ = (#quadratic characters unramified outside S)
  = 2^a − 1, where 2^a = |Hom(π, C₂)|. There are **3** distinct order-2 subgroups
  of S₃, giving 3 distinct images, hence
    #{φ : |im φ| = 2} = 3·(2^a − 1).
  *Independent data source:* our own quadratic-character machinery (Stage 1/Stage
  4) — quadratic fields unramified outside S.

  ⚑ **FLAG for hand-referee (H2 vs H4 phrasing).** The rider H2 lists the
  multiplicity as "3 conjugate C₂ subgroups", which gives the **hom-count term
  3·(2^a − 1)** above. The rider H4 says "the image-C₂ term must equal 2^t − 1".
  These differ by the factor 3. Reading adopted here: the *character/surjection*
  count is 2^a − 1 (= 2^t − 1 for the H4 pool, the quantity Stage 4 computes and
  the quantity H4 pins); the *contribution to |Hom|* is 3× that. i.e. H4's "2^t−1"
  is the per-subgroup surjection count, and the |Hom| assembly carries the ×3.
  **This is the one multiplicity where the derivation and a rider's literal
  wording diverge — please confirm which enters the assembled |Hom|.**

- **Image order 3 — count 3^k − 1.** A₃ is the *unique* (normal) order-3 subgroup,
  so there is **no** subgroup multiplicity. A hom with image A₃ is a surjection
  π ↠ A₃ ≅ C₃; #surjections onto C₃ = 3^k − 1 where 3^k = |Hom(π, C₃)| and k is the
  number of independent C₃-characters unramified outside S. Cross-check via fields:
  each cyclic cubic field L contributes |Aut(C₃)| = 2 surjections, and there are
  (3^k − 1)/2 such fields, so 2·(3^k − 1)/2 = 3^k − 1 ✓ (the Aut(C₃) factor and the
  field↔character 2:1 cancel — no leftover multiplicity).
  *Independent data source:* our own cyclic-cubic conductor count (H5), or PARI.

- **Image S₃ — count 2·c.** A surjection π ↠ S₃ corresponds to a Galois S₃-extension
  M/ℚ (unramified outside S) with a chosen isomorphism Gal(M/ℚ) ≅ S₃; there are
  |Aut(S₃)| = 6 such isomorphisms per M, so #surjections = 6·#{S₃-fields M}. Each
  S₃-field M has exactly **3** conjugate non-Galois cubic subfields (fixed fields of
  the three C₂ ⊂ S₃), so #{non-Galois cubic fields} c = 3·#{S₃-fields}. Therefore
    #{φ : im φ = S₃} = 6·#{S₃-fields} = 6·(c/3) = 2·c.
  *Independent data source (the only externally refereed term):* LMFDB cubic tables
  **and** PARI enumeration, twinned (Phase 2).

**The point of the decomposition.** Three of the four terms (1, C₂, C₃) are built
from our own machinery and closed forms; only the S₃ term is the referee-supplied
unknown c. So if the assembled |Hom| disagrees with an independent total, the term
structure localizes the bug.

    Z_DW(Spec ℤ[1/S]) = [ 1 + 3(2^a − 1) + (3^k − 1) + 2c ] / 6.

*Sanity check S = ∅:* a=k=0, c=0 ⇒ |Hom| = 1, Z_DW = 1/6 (only the trivial map;
π₁ᵉᵗ(Spec ℤ) is trivial by Minkowski). ✓

---

## H3 — Field-side dictionary, and the wild-ramification scope

**Dictionary (image-S₃ classes ↔ non-Galois cubics).** Under Convention A, the
image-S₃ homs are 2·c with c = #{non-Galois cubic fields K/ℚ : rad(disc K) ⊆ S,
either signature}. Equivalence used: a non-Galois cubic K has Galois closure M an
S₃-extension; K/ℚ (equivalently M/ℚ) is unramified outside S ⟺ every prime
ramifying in K lies in S ⟺ rad(disc K) ⊆ S. For a **tame** prime p (p ∤ e for the
ramification index e ≤ 3), p ramifies in K ⟺ p | disc K, and M/ℚ ramifies exactly
where K does.

**Wild ramification — handled by scope restriction.** Wild ramification in a cubic
occurs only when p | e with e ∈ {2,3}, i.e. at p = 2 (e = 2) or p = 3 (e = 3). Our
Stage-6 prime pool is **S ⊆ {p ≡ 1 (mod 4)}** (H4), which in particular **excludes
2 and 3**. Every prime available to S is therefore ≥ 5 > 3 ≥ e, so **all
ramification in scope is tame** and the clean criterion "p ramifies ⟺ p | disc K"
holds throughout. The wild p = 3 case (and wild p = 2) is **out of scope by the
pool restriction** — stated as a scope restriction, not silently ignored:

> **Scope restriction (Stage 6).** S contains only primes ≡ 1 (mod 4). Hence
> 2, 3 ∉ S, all ramification is tame, and no conductor-3^j (cyclic) or wild-cubic
> discriminant occurs. Extending S to include 2 or 3 is deferred (would require
> pinning the 3-adic/2-adic valuation bounds on disc, cf. tame vs wild).

---

## H4 — Internal free theorem: the image-C₂ term for the S ⊆ {p ≡ 1 mod 4} pool

For S = {p₁,…,p_t} with every pᵢ ≡ 1 (mod 4):

- The quadratic extensions unramified outside S are ℚ(√d), d = ∏_{i∈T} pᵢ over
  subsets T ⊆ S. Since each pᵢ ≡ 1 (mod 4), any product d ≡ 1 (mod 4), so
  disc ℚ(√d) = d is **odd** ⇒ unramified at 2 (no dyadic discriminants can occur),
  and d > 0 ⇒ **totally real**, signature (2,0).
- The nontrivial ones number **2^t − 1** (all 2^t subsets minus the empty set),
  each a distinct real quadratic field. So a = t and the character count is
  2^t − 1, all real. **This is exactly the Stage-4 character count** — Stage 4
  enumerates the 2^t characters on (ℤ/2)^t and the 2^t − 1 nontrivial ones are
  these quadratic fields.

**REQUIRE (not observe):** in the Stage-6 sweep, for every S with all pᵢ ≡ 1 (mod 4),
the number of quadratic characters unramified outside S must equal **2^t − 1**, all
of signature (2,0), with **zero** dyadic discriminants — and this must agree with
the Stage-4 count for the same S. The contribution to |Hom| is **3·(2^t − 1)**
(the ×3 from the three conjugate C₂ subgroups; see the H2/H4 flag — the *character*
count 2^t−1 is what this free theorem pins).

---

## H5 — Cyclic cubic term (conductor-based)

A cyclic cubic field has conductor f a product of distinct primes ≡ 1 (mod 3)
(and, when 3 is available, possibly 9 — excluded here since 3 ∉ S). For each prime
p ≡ 1 (mod 3) there is exactly (3−1)/2 = 1 cyclic cubic field of conductor p; with
k such primes available in S the count of cyclic cubic fields unramified outside S
is **(3^k − 1)/2**, and the image-C₃ contribution to |Hom| is
2·(3^k − 1)/2 = **3^k − 1** (H2). Citation for the conductor/count of cyclic cubics:
cyclic-cubic conductor theory (Hasse; standard) — **UNVERIFIED** page; the count
(3^k−1)/2 is also directly checkable against PARI in Phase 2.

**Pool intersection.** k = #{p ∈ S : p ≡ 1 (mod 3)}. Combined with the H4 pool
p ≡ 1 (mod 4), the primes contributing to the C₃ term are exactly **p ≡ 1 (mod 12)**.
Primes ≡ 5 (mod 12) (i.e. ≡ 1 mod 4 but ≡ 2 mod 3) contribute **nothing** to C₃.

**REQUIRE:** the sweep must contain S-sets with **k = 0** (C₃ term = 0, e.g. S built
only from primes ≡ 5 mod 12) **and** with **k ≥ 1** (C₃ term = 3^k − 1 > 0, S
containing primes ≡ 1 mod 12). An all-zero-C₃ sweep would leave the cyclic term
untested. 3 ∉ S ⇒ no conductor-9 cyclic cubics (scope, H3).

---

## H6 — LMFDB query semantics (pinned; cached extract is a Phase 2 deliverable)

The S₃ term's referee is LMFDB's number-field table (and PARI, twinned). Pinning
the query like a paper citation (field names **UNVERIFIED** against the live LMFDB
schema until the Phase 2 extract is run):

- **Table:** LMFDB number fields (`nf` / "Global Number Fields").
- **Degree:** 3.
- **Galois group label:** non-Galois cubics = **S₃** = T-label **3T2** (the image-S₃
  term). Cyclic cubics = **C₃** = **3T1** (the image-C₃ cross-check, H5).
- **Discriminant support = "disc supported on S":** every prime ramifying in K lies
  in S, i.e. the set of ramified primes (LMFDB "ramps" / the primes dividing disc)
  ⊆ S. Equivalently rad(disc K) ⊆ S (tame, H3).
- **Signature / disc sign (Convention A, H1):** include **both** — totally real
  (3,0), disc > 0, **and** complex (1,1), disc < 0. Do **not** filter by signature
  (that would answer Convention B).
- **Cached extract (Phase 2):** stored under `data/` with recorded query string,
  date, and SHA-256, per the `data/odlyzko` and ARCHITECTURE §4 precedent; tests
  read only the local cache and SKIP visibly if absent. The PARI enumeration
  (`nflist` or a discriminant-bounded cubic search) is the independent twin and
  must agree with the LMFDB extract **before** either enters the |Hom| assembly.

---

## Summary of what Phase 2 will assemble and require

    Z_DW(Spec ℤ[1/S]) = [ 1 + 3(2^t − 1) + (3^k − 1) + 2c ] / 6
      t = |S| (all p ≡ 1 mod 4);  k = #{p ∈ S : p ≡ 1 mod 12};
      c = #{non-Galois cubic fields, rad(disc) ⊆ S, both signatures}  (referee: LMFDB ∥ PARI)

Pre-registered Phase 2 tests (per authorization): `twin_cubic_count_lmfdb_vs_pari`
(c two ways, agreeing before assembly); `invariance_s_ordering`; anchors with ≥ 1
S having c = 0 and ≥ 1 with c ≥ several, both REQUIREd present; `theorem_dw_s3_
decomposition` over the sweep with per-stratum REQUIREs (t; 3 ∈ S = false in scope;
signature mix under Convention A — both signs of disc present among the c cubics
where the data has them); emit `dw_s3.json` + viewer panel; empiric: the N_{S₃}
distribution vs t. Rule 1 (a count mismatch is a witnessed deliverable, not a
reconciled bug) and the boundary-action rule are unchanged.

**Open items explicitly left for the hand-referee before Phase 2:**
1. The **H2/H4 ×3 multiplicity** on the C₂ term (does the assembled |Hom| carry
   3·(2^t−1) or 2^t−1? — the derivation says 3·(2^t−1) for a hom count).
2. Confirm **Convention A** (both signatures) is the intended reading of
   π₁ᵉᵗ(Spec ℤ[1/S]); if Convention B is wanted, the H6 query filters to disc > 0.
