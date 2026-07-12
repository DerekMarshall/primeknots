<!-- EXPLAINER E2. Linked from README; content accuracy (rule 1) is referee-owned.
     Register: docs/explainers/CLAIMS-E.md. Rule 1: simplifications are understatements
     or special cases, never inversions. Analogies are labelled. The arithmetic-topology
     DICTIONARY is an analogy; the individual theorems it organizes are genuine theorems
     that we verify numerically (twinned / oracle-refereed / over a range). -->

# Primeknots, explained

## One analogy, held at arm's length

There is a long-standing **analogy** — make no mistake that it is an analogy — between number
theory and three-dimensional topology, usually called *arithmetic topology* and developed by
Mazur, Kapranov–Reznikov, and Morishita. <!-- claim:E2-1 --> Picture it this way: the set of
all primes is like a 3-dimensional space, each individual prime is like a **knot** (a closed
loop) sitting in it, and arithmetic relationships between primes behave like the way knots
**link** through one another. <!-- claim:E2-2 --> The dictionary is a source of intuition and
of precise conjectures; it is **not** itself a theorem, and this project never treats it as
one. What the project does is take the genuine number-theoretic theorems the dictionary
organizes and **verify them numerically**, from scratch, each one twinned against a second
independent computation or an external referee. The permanent repository name *primeknots* is
a nod to this analogy, not a claim that primes are literally knots. <!-- claim:E2-3 -->

The stages climb the dictionary. Each is a real theorem with its own verification suite.

## Stage 0–1 — Linking numbers and reciprocity as a symmetry

The **Legendre symbol** `(p/q)` records whether `p` is a square modulo `q`. In the analogy it
plays the role of a **mod-2 linking number** between the two "prime knots." <!-- claim:E2-4 -->
The celebrated **law of quadratic reciprocity** then reads as a *symmetry*: the linking of `p`
with `q` matches the linking of `q` with `p` (up to a correction at 2 and ∞). We verify
quadratic reciprocity exhaustively over a range of prime pairs, with two independent
implementations of the symbol (`theorem_quadratic_reciprocity`). <!-- claim:E2-5 --> Assembling
these pairwise linkings over many primes gives a **linking matrix**, emitted and rendered as a
graph (`linking_matrix.json`, `linking_graph.json`). <!-- claim:E2-6 -->

## Stage 2 — Triple linking: Rédei symbols and Borromean primes

Some linking is genuinely three-way. The **Rédei symbol** `[a, b, c]` is a triple analogue of
the Legendre symbol — the arithmetic counterpart of a **Borromean** linking, where three loops
are entangled although no two of them link pairwise. <!-- claim:E2-7 --> The load-bearing
published value here, which the whole normalization is pinned to, is

```
    [13, 61, 937] = −1,
```

<!-- claim:E2-8 --> reproduced by our conic-solver (with an independent brute-force twin) and
locked as a regression anchor (`anchor_redei_13_61_937_is_minus_1`). We also verify **Rédei
reciprocity** — the symmetry of the triple symbol — over a sampled range
(`theorem_redei_reciprocity_s3`), and emit the Borromean-triple picture (`borromean.json`).
<!-- claim:E2-9 -->

## Stage 3 — Class groups as first homology, and the 4-rank

If a prime knot is a loop, the **ideal class group** of a quadratic field is the analogue of
the **first homology** of the arithmetic 3-manifold — it counts "how many independent loops"
the space has. <!-- claim:E2-10 --> The **Rédei–Reichardt theorem** computes a refined piece of
this — the *4-rank* of the class group — from a matrix built out of the Rédei/Legendre linking
data. We verify the Rédei–Reichardt 4-rank identity over a range of discriminants
(`theorem_redei_reichardt_4rank`), with the class group computed independently via reduced
binary quadratic forms (`classgroups.json`). <!-- claim:E2-11 -->

## Stage 4 — The arithmetic Chern–Simons partition function

Chern–Simons theory attaches to a 3-manifold a number — a *partition function*. Its arithmetic
analogue, worked out by Chung–Kim–Kim–Pappas–Park–Yoo (CKKPPY), attaches such a number to a
number field, and there is a closed-form identity expressing it as a **Gauss sum**.
<!-- claim:E2-12 --> We compute the partition function two independent ways — a direct sum over
characters (the "path integral" side) and the closed Gauss-sum form — and check they agree
(`theorem_ckkppy_partition_identity`, `cs_partition.json`). This is the stage where the
"physics" analogy is cashed out as an exact arithmetic identity. <!-- claim:E2-13 -->

## Stage 5 — The Weil explicit formula and the zeros of zeta

Independently of the knot dictionary, the project reconstructs the **Weil explicit formula**,
the exact duality between prime numbers and the non-trivial zeros of the Riemann zeta function.
<!-- claim:E2-14 --> Our zeros are computed by an in-house Riemann–Siegel routine and matched
against Odlyzko's published high-precision table as an external anchor
(`anchor_zeros_match_odlyzko`, `zeros.json`, `psi_reconstruction.json`). <!-- claim:E2-15 -->

## Stage 6 (stretch) — Non-abelian counting

The final, stretch stage counts homomorphisms into a non-abelian group `S₃` — the
Dijkgraaf–Witten analogue beyond the abelian Chern–Simons case — and checks the counts against
a mass formula (`theorem_dw_s3_decomposition`, `theorem_dw_mass_formula`, `dw_s3.json`).
<!-- claim:E2-16 -->

## What is analogy and what is theorem

To be exact about the register: the *primes-as-knots dictionary* is an analogy (Stage headers,
`docs/RESEARCH.md` §§2–8, following Morishita). Quadratic reciprocity, Rédei reciprocity, the
Rédei–Reichardt 4-rank theorem, the CKKPPY identity, and the Weil explicit formula are
**genuine theorems**. <!-- claim:E2-17 --> What this project contributes is not a new theorem
but an **executable, cross-checked replication** of each: two independent implementations (or an
oracle) must agree, published values are pinned as anchors, and the emitted JSON is rendered by
a static viewer. "Verified" means numerically, over a stated range — never a new formal proof
(the project claims none). <!-- claim:E2-18 -->

## What we actually did / what remains open

Stages 0–6 are complete and green, each with the five verification categories where applicable
(twin / theorem / anchor / invariance / oracle) and an emitted artifact. <!-- claim:E2-19 -->
What is *not* claimed: none of these stages proves anything new, extends the dictionary, or
resolves an open problem; the arithmetic-topology correspondence remains a heuristic framework,
and Stage 6 is an explicitly labelled stretch. The scientific frontier of the project is the
separate **murmurations** ladder (see E1), not the primeknots stages, which are replication.
<!-- claim:E2-20 -->
