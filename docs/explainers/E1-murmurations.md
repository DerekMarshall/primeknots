<!-- EXPLAINER E1. Linked from README; content accuracy (rule 1) is referee-owned, the
     structural claim-sourcing is checked by verify/explainers_check.py.
     Register: docs/explainers/CLAIMS-E.md (one row per <!-- claim:ID --> marker).
     Rule 1: every simplification is an understatement or special case of the pinned
     source, never an inversion. Analogies are labelled. Numbers come from emitted
     artifacts or pinned papers, never memory. -->

# Murmurations, explained

## The picture

Plot a certain average over a large family of elliptic curves, prime by prime, and the
dots do not scatter randomly. They organize into sweeping, opposing waves. Roughly — and
this is an analogy, not a claim — the plot looks like a *murmuration* of starlings wheeling
at dusk, which is where the name comes from. <!-- claim:E1-1 --> The image on the first
page of the paper that named the phenomenon is literally a photograph of starlings.
<!-- claim:E1-2 -->

The rest of this page says what is actually being plotted, what is surprising about it,
what has been proven, and where our own computations sit.

## What is being averaged: the Frobenius trace `a_p`

An elliptic curve over the rationals can be written `y² = x³ + Ax + B`. For each prime `p`
you can count how many points the curve has over the finite field with `p` elements
(picture the field as *clock arithmetic* with `p` hours — that phrase is an analogy for
"arithmetic modulo `p`," nothing more). Call that count `#E(F_p)`.

The quantity that murmurations are built from is **not** that raw count. It is the
**Frobenius trace**

```
    a_p(E) = p + 1 − #E(F_p).
```

<!-- claim:E1-3 --> That is, `a_p` measures the **deviation** of the point count from the
"expected" value `p + 1` — it is a signed number, positive or negative, and by Hasse's
theorem it is bounded: `|a_p| ≤ 2√p`. <!-- claim:E1-4 --> So `a_p` being large and negative
means the curve has *more* points than expected at `p`; large and positive means fewer. It
is a small, signed fluctuation, not a headcount.

A concrete, checked value: for the curve catalogued as **11a1**, the trace at `p = 2` is
**`a_2 = −2`** (and `a_3 = −1`). <!-- claim:E1-5 --> We do not take that on faith — our own
from-scratch point-counter reproduces 11a1's traces across a range of primes, checked
against the reference tables. <!-- claim:E1-6 -->

## Two things the curves are sorted by: rank and root number

**Rank.** By the Mordell–Weil theorem the rational points of an elliptic curve form a
finitely generated abelian group; the **rank** is the number of independent generators of
its infinite (free) part. <!-- claim:E1-7 --> It is emphatically *not* "the number of
rational solutions" — a rank-1 curve already has infinitely many rational points. Computing
the rank has no known general algorithm, which is part of why it is interesting.
<!-- claim:E1-8 -->

**Root number.** Each curve also carries a **root number** `ε ∈ {+1, −1}`, the sign in the
functional equation of its `L`-function. This is the variable the *theorems* about
murmurations actually weight the average by. <!-- claim:E1-9 --> Rank enters the story
through **parity**: the sign is `ε = (−1)^(order of vanishing of L at s=1)`, and the weak
Birch–Swinnerton-Dyer conjecture predicts that order of vanishing equals the rank.
<!-- claim:E1-10 --> So "sort by rank" and "weight by root number" are related but not
identical knobs — a distinction that matters once you move from the discovery pictures to
the proved theorems.

## The average, and the waves

He, Lee, Oliver, and Pozdnyakov (HLOP) studied, for curves of a *fixed rank* `r` with
conductor in a band `[N₁, N₂]`, the plain average of the trace at the `n`-th prime:

```
    f_r(n) = (1 / #E_r[N₁,N₂]) · Σ_{E of rank r, conductor in [N₁,N₂]} a_{pₙ}(E).
```

<!-- claim:E1-11 --> Plotted against the primes, `f_0` and `f_1` trace out two distinct,
largely *opposing* waves; a rank-0 band and a rank-2 band do the same. <!-- claim:E1-12 -->
In their worked example the bands are not small (the counts are of **isogeny-class
representatives**, HLOP's unit of enumeration): using the LMFDB database, the rank-0 and
rank-1 classes with conductor in `[7500, 10000]` contain **4328** and **5194** isogeny
classes respectively; the rank-0 and rank-2 classes in `[5000, 10000]` contain **8536** and
**1380**. <!-- claim:E1-13 --> The pattern is a **population-level** fact about such an
average — it is not a device for reading one individual curve's rank off its coefficients.
<!-- claim:E1-14 -->

## Why it is a *density*, not just a wiggle

Two features turn this from a curiosity into a mathematical object. First, the oscillation
has **decaying frequency** — the waves stretch out as the primes grow. Second, and
decisively, it is **scale-invariant in the ratio `p / N`** (prime over conductor): as you
slide the family to larger conductors, the picture does not wash out but converges, when
drawn against `p/N`, toward a *continuous-looking* limit function. <!-- claim:E1-15 -->
Sutherland made exactly this observation, and it is what lets one speak of a **murmuration
density** — a fixed curve `M(y)` in a "prime-over-scale" variable `y` that the averages
approach. <!-- claim:E1-16 --> This is why our own plots put that ratio, not `p` itself, on
the horizontal axis. <!-- claim:E1-17 -->

A precision worth stating, because three closely-related variables appear in the literature
and in our own artifacts and they are *not* interchangeable. Zubrilina's proved density is a
function of **`y := P/X`** — prime over the *family's level scale* `X` — and she pins this
"NOT `P/N`." Sawin–Sutherland instead bin by **`u = p/N(E)`** — prime over the *individual
curve's conductor* `N(E)`. Our Stage-M1 replication uses **`p/N₂`** — prime over the band's
*upper* conductor. All three are "prime over a conductor scale," and all three make the
oscillation scale-invariant; but which denominator you pick is part of the statistic's
definition, not a cosmetic axis label. <!-- claim:E1-35 -->

## Ordering sensitivity

The density is not unique — it depends on **how you enumerate the family**, and dramatically
so. Zubrilina's proved case is for square-free levels, and she notes that allowing *all*
levels appears to give a *slightly different* density that nonetheless shares its key
properties. <!-- claim:E1-18 --> Far more sharply, she reports that the elliptic-curve
murmuration "is very sensitive to the ordering by conductor, and **disappears almost
entirely** when the curves are ordered by naive height, `j`-invariant, or discriminant."
<!-- claim:E1-19 --> That is the striking part: the strong conductor-ordered wave nearly
vanishes under **naive height** `H = max(4|A|³, 27B²)`. Sawin and Sutherland nonetheless
isolate a **subtler** height-ordered density that survives (their Conjecture 1) — and it is
exactly this fainter, height-ordered signal that our own Stage-M4 and PR-1 work measures,
which is why the deviations we chase there are small and hard-won rather than the bold
opposing waves of the conductor picture. <!-- claim:E1-20 -->

## How it was found (per the authors, not the folklore)

The pattern was announced in 2022 by He, Lee, Oliver, and Pozdnyakov (arXiv:2204.10140).
<!-- claim:E1-21 --> By the authors' and Zubrilina's own accounts, the discovery
**stemmed from machine-learning and computational work on the LMFDB database** — classifiers
built to predict rank, whose behaviour prompted the question — and the wave surfaced when
the coefficients were **averaged and visualized**; the methods "did not explain the
mathematical source" of the phenomenon. <!-- claim:E1-22 --> HLOP state that, to their
knowledge, this oscillating behaviour "has never been reported in the literature."
<!-- claim:E1-23 --> The honest one-line version is: *humans investigating what a
rank-classifier had latched onto found a wave in the averages* — not "an AI saw it."

## Proven, conjectured, still open

- **Proven.** Zubrilina established the **first case**: a complete, closed-form murmuration
  density for holomorphic modular forms of fixed weight, square-free level, weighted by root
  number. <!-- claim:E1-24 --> (Published in *Inventiones Mathematicae*, 2025.)
  <!-- claim:E1-25 -->
- **Conjectured.** The Sawin–Sutherland density for elliptic curves ordered by **height** is
  **Conjecture 1** of their paper; only a variant (their Theorem 2) is proven, via a
  different limiting statement. <!-- claim:E1-26 -->
- **Open — even the conductor-ordered elliptic-curve case.** Zubrilina proves the density for
  *modular forms*; she is explicit that this does **not** settle elliptic curves: "Murmurations
  for elliptic curves over ℚ are not explained by these results, as they constitute a **very
  sparse subset** of weight 2 modular forms," and indeed "the best fit curve to approximate the
  data of elliptic curve murmurations does not match" the modular-form density. The average
  over the full modular-form family does not determine the average over the sparse elliptic-curve
  subfamily. <!-- claim:E1-27 --> The **height-ordered** elliptic-curve density is a separate
  conjecture (above), and the behaviour of its *tail* is where our own results flag a deviation
  (below). <!-- claim:E1-36 -->

Throughout: murmurations give **statistical** information about rank across a family. They do
not "crack" the Birch–Swinnerton-Dyer conjecture, which is about the rank of an *individual*
curve; BSD is the backdrop that makes rank worth predicting, not something these averages
resolve. <!-- claim:E1-28 -->

## What we actually did

This repository re-derives the phenomenon from scratch (our own point-counting and averaging,
two independent implementations of each load-bearing step, external oracles only as
referees) and renders each stage as a JSON artifact with a static viewer:

- **Stage M1** replicates the original HLOP picture: the unweighted average of `a_p`,
  conductor-ordered, rank-split, on Cremona's database (`viz/data/murmuration_curve.json`).
  Our emitted run uses the first **300** primes and plots against **`p/N₂`** (prime over the
  band's upper conductor); HLOP's own Figure 1 ran to the 1000th prime over the bands
  `[7500,10000]` and `[5000,10000]`. <!-- claim:E1-29 -->
- **Stage M2** replicates the Dirichlet-character murmuration of Lee–Oliver–Pozdnyakov
  (`dirichlet_murmuration.json`). <!-- claim:E1-30 -->
- **Stage M3** replicates **Zubrilina's proved density** for weight-2 modular forms and
  checks the empirical average against the closed-form `M₂(y)` (`zubrilina_murmuration.json`).
  <!-- claim:E1-31 -->
- **Stage M4 + PR-1** test **Sawin–Sutherland Conjecture 1** on the height-ordered family we
  build and count end to end (1048 curves at height ≤ 10⁴; 5042 at ≤ 2¹⁶). The hump and the
  first zero-crossing land within our pre-registered tolerance; the **trough is an openly
  flagged deviation** that stayed put (did not move toward the target) when we extended the
  scale from 10⁴ to 2¹⁶ (`sawin_sutherland_murmuration.json`,
  `ss_x_extension_murmuration.json`; the pre-registration is `docs/preregistered/PR-1.md`).
  <!-- claim:E1-32 -->

## What remains open

The conductor-ordered elliptic-curve density is open. Sawin–Sutherland's height density is a
**conjecture**, not a theorem. Our own height-ordered **trough deviation is unresolved** — we
have shown, on our range, that it does not decay like a finite-size effect should, and we have
*not* claimed to know why; that is logged as an open deliverable, and a follow-up
pre-registration (PR-2) tests one candidate explanation (an over-representation of
higher-rank curves) whose own outcome is still pending. <!-- claim:E1-33 --> Nothing here
proves a conjecture or resolves BSD; "verified" in this project always means *numerically,
over a stated range, cross-checked* — never a formal proof. <!-- claim:E1-34 -->
