<!-- EXPLAINER E3 — DRAFT for external read-through. NOT linked from README yet.
     Register: docs/explainers/CLAIMS-E.md. Audience: a technical reader who is not a
     number theorist. Rule 1: simplifications are understatements/special cases, never
     inversions. Every mechanism below is pinned to a rule, a test, or a ledger row. -->

# How this project keeps itself honest

The mathematics here (see E1, E2) is only as trustworthy as the machinery that computes it.
This page describes that machinery — the part a software engineer would recognize — for
someone who will never open a pinning log. Every device below is a standing rule in
`CLAUDE.md`, not a nicety.

## The core problem

A from-scratch computation of a deep theorem has a failure mode that ordinary unit tests miss:
you can get the "right" answer for the wrong reason, because you tuned the code until the
expected number appeared. The whole point of a *verification* project is to make that
impossible. Five devices do the work.

## 1. Twins — a result computed one way is a rumour

Every mathematically load-bearing function has **two independent implementations** — different
algorithms, not refactors of each other — that must agree, or an external oracle check.
<!-- claim:E3-1 --> Example: the Frobenius trace `a_p` is computed both by a fast table-based
character sum and by a frozen reference routine, and they are checked to agree across the whole
prime range we use (`twin_ap_fast_vs_charsum_m4`). <!-- claim:E3-2 --> Only after the twins
agree is one of them optimized; the naive twin stays as the referee and is never optimized
too. <!-- claim:E3-3 -->

## 2. Oracles referee, never replace

Established tools — PARI/GP, the LMFDB database, Odlyzko's tables — are allowed to **confirm**
our numbers but are **never called from the computational core**; they live only in the test
and data-generation layers. <!-- claim:E3-4 --> When an oracle is absent (say PARI is not
installed), the affected test **skips visibly** — it prints a SKIP and returns a skip code —
rather than silently passing. <!-- claim:E3-5 --> A subtlety the project is explicit about:
some inputs are legitimately *oracle-provenance data* (for the height-ordered family, the
conductor and root number of each curve come from PARI as trusted input), and those columns are
labelled `oracle`, distinct from the quantities we compute ourselves. <!-- claim:E3-6 -->

## 3. Anchors — specific published values, pinned

Individual, independently published numbers are frozen as regression **anchors**: the Rédei
symbol `[13, 61, 937] = −1`, the trace `a₂ = −2` for curve 11a1, our zeros matching Odlyzko's
table. <!-- claim:E3-7 --> If a refactor ever changes one of these, the suite fails loudly.

## 4. The prime directive — never fit conventions to the expected answer

The most important rule has no code: **if a computation disagrees with a published value, the
bug is in our code or our reading of the mathematical convention — you go back to the cited
source and fix the reading, you never flip a sign until the expected number appears.**
<!-- claim:E3-8 --> Doing the latter would falsify the experiment and defeat the repository's
entire purpose. A sharpened form: where a claim has both an *anchor* test (does the published
value appear?) and an *invariance* test (is the result independent of arbitrary choices?), the
**invariance test is the senior one** — a passing anchor beside a failing invariance means the
convention is still wrong, even though the headline number looks right. <!-- claim:E3-9 -->

## 5. Pre-registration — for the research that has no answer key

Stages that *replicate* known mathematics have an answer key (the published value). The
research-mode work (the open murmurations questions) does not — and there the temptation to
read a pattern into noise is strongest. The defence is **pre-registration**, borrowed from
experimental science: the tolerance, the decision rule, and the significance threshold are
written down and **committed to version control before the data exists**. <!-- claim:E3-10 -->
Two worked examples from this project:

- **PR-1** committed, *before* extending the computation to a larger scale, a single decision
  rule for what would count as "the deviation is a finite-size effect" versus "the deviation
  persists" — and, before ever reading the larger-scale output, committed the interpretation
  fork. When the data came in, the rule was applied verbatim, not renegotiated.
  <!-- claim:E3-11 -->
- **PR-2** committed the exact statistical threshold — computed as a formula in a
  pre-measured quantity — *before* the split data it judges was ever computed. <!-- claim:E3-12 -->

Every exploratory statistic that is *looked at* is written into a **looks ledger**
(`docs/preregistered/LOOKS.md`), because the number of times you looked is what makes any
surprise interpretable; unlogged exploration is the quiet form of the pattern-fitting failure.
<!-- claim:E3-13 -->

## 6. The errata ledger — mistakes are recorded, not erased

When a rule is broken or a subtle error is caught, it is written into a numbered **errata
ledger** (`docs/ERRATA.md`), which currently holds **26** entries. <!-- claim:E3-14 --> Two are
worth naming as illustrations. The *constants incident* (#25) is the reason for a standing rule
that **no floating-point mathematical constant is ever typed from memory** — a typed constant is
invisible to twins, because both twins share it, so it must instead be transcribed from a cited
source to high precision or generated by a checked-in script. <!-- claim:E3-15 --> And #26
records a self-caught protocol breach: a confirmation-class computation that was run outside the
pre-registration protocol, with a tolerance fit after seeing the residuals — logged as an error
even though the result later stood. <!-- claim:E3-16 -->

## 7. Freshness — the published data cannot drift from the code

The JSON that the static viewers render is committed to the repository, and a check asserts it
is **byte-identical** to what the current code emits — so a viewer can never silently show stale
numbers. <!-- claim:E3-17 --> The one field allowed to differ (a build stamp) is normalized
before the comparison; everything else must match exactly, and the emitters are compiled for
cross-platform bit-reproducibility. <!-- claim:E3-18 -->

## What we actually did / what remains open

The result of all this is a repository where every headline number is either twinned, anchored
to a published value, or refereed by an oracle; where the research claims were pre-registered
before their data existed; and where the mistakes are in a ledger rather than in a drawer.
<!-- claim:E3-19 --> What it deliberately does **not** provide is any formal proof or any claim
of a new theorem: "verified" means numerically, over a stated range, cross-checked. The honesty
machinery does not make the open questions closed — it makes the open ones *legible*, so that a
deviation we cannot yet explain (see E1) is on the record as exactly that. <!-- claim:E3-20 -->
