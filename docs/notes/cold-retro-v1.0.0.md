# Cold Retrospective — `primeknots` @ `v1.0.0`

**Scope of evidence:** git history + committed docs only, commits reachable from tag
`v1.0.0` (`23db3c9`). 229 commits, 2026-07-07 → 2026-07-19 (13 calendar days, 9 active).
HEAD is 2 commits past the tag (DOI backfill, methods-dossier); those are excluded.
Commit positions below are chronological indices `c1…c229`. This is a *cold* read: no
intent is inferred beyond what commit messages and committed docs state. The mathematics
is not evaluated; no process recommendations are offered.

---

## 1. Timeline reconstruction

Phases are commit-cluster inflections in date + dominant file area. Boundaries are exact
commit indices.

| Ph | Label | Idx range | Dates | Commits | Dominant areas (file-touches) | Boundary marker |
|----|-------|-----------|-------|---------|-------------------------------|-----------------|
| **P1** | primeknots ladder (Stages 0–6) | c1–c48 | 07-07 | 48 | `src/{core,symbols,linking,redei,classgroup,cs,zeta,dw}` (53), `verify/` (62), `docs/notes/*-pinning` (20), `src/emit` (18), `oracle/` (11) | ends `d6a6ad3` (Stage 6 Belabas referee) |
| **P2** | publication / CI / deck | c49–c66 | 07-07→08 | 18 | `.github`+`CMake`+root (14), README/deck (23 on 07-08), `viz/data` (12), `ERRATA.md` (3), `verify/freshness` | `2d22c73` (CI) → `1b28d4c` (freshness note) |
| **P3** | M-ladder replication M0–M4 | c67–c115 | 07-09→11 | 49 | `src/{ell,mform,murm}` (54), `verify/m*` (51), `*-pinning` logs, `src/emit`, `viz/data`, ERRATA #19–#26 | `d6834d0` (RESEARCH-M) → `ce5f974` (M4 close-out re-emit) |
| **P4** | M5 research PR-1/2/3 + heavy compute | c116–c164 | 07-11→17 | 49 | `docs/preregistered` (20), `src/murm`, `verify/m5`, `data/m5` caches, `docs/explainers` (12) | `dd6beb0` (M5 opens) → `8f64ba1` (Rung-3 verdict H0) |
| **P5** | data-note authoring + libm-diff + QA start | c165–c202 | 07-17→18 | 38 | `docs/notes/data-note` (58), `docs/notes`, `docs/explainers`, `app/at.cpp` (ss-leakage/ap-sample), Layer-1 QA | `2ff195d` (Checkpoint A) → pre-referee |
| **P6** | referee A–D + eq(2)/eq(1) corrections + release | c203–c229 | 07-18→19 | 27 | `docs/notes/referee-round`, `src/murm/ss_density`+`ss_empirical*`, ERRATA #27–#31, `viz/data` regen, `data/` box-regen, CITATION/Pages | `1185de8` (referee A) → `23db3c9` (Pages fix) |

**Shape.** Two build-heavy front phases (P1, P3) at high commit density on single days
(07-07 = 57 commits incl. post-tag; 07-10 = 34), a long compute-gated middle (P4 spans
07-11→17 with a 3-day gap 07-12→15), and a documentation/correction-heavy tail
(07-18 = 47 commits, 07-19 = 12) where P5 and P6 interleave inside single calendar days.

---

## 2. Quantitative profile

### 2a. Corrective vs forward

| Class | Count | % of 229 | Definition |
|-------|-------|----------|------------|
| Explicit `ERRATA #N`-citing (subject) | 17 | 7.4% | subject names an errata number |
| `fix:` / `fix(` prefixed | 10 | 4.4% | (overlaps errata-citing) |
| `chore: re-emit …` / `reframe` snapshots | 17 | 7.4% | clean-tree re-emits + #28 reframe cascade |
| **Union: corrective / rework** | **54** | **23.6%** | errata ∪ fix ∪ re-emit ∪ hygiene (relativize, scrub, unbreak, drift) |
| Forward work | 175 | 76.4% | remainder |

Caveat on the union: ~9 of the 54 are mechanical `generated_by` clean-tree re-emits
(`f1a2151`, `d09e6b8`, `c60d85d`, `6964da4`, `592ba9e`, `e300c41`, `ce5f974`, `d591f10`,
`2eb5817`), not error corrections; and 2–3 (`e176708`, `44f83cb`) are forward work that
merely *documents* an in-review catch. A tighter "error-correction" count is ~40 (≈17%).

### 2b. Error-introduction → detection latency

Pinnable errata only (introducing commit identifiable). Δcommits = chronological-index
distance; Δdays from commit dates.

| # | Introduced | Detected / Corrected | Δcommits | Δdays | Caught by |
|---|-----------|----------------------|----------|-------|-----------|
| 6 | c25 `c1dee32` 07-07 | c26 `4b10ed0` 07-07 | 1 | 0 | `theorem_compose_group_axioms` (associativity) |
| 7 | c25 `c1dee32` 07-07 | c26 `4b10ed0` 07-07 | 1 | 0 | `theorem_genus_2rank` sweep |
| 15 | ~c25 (stage-3 TUs) 07-07 | c53 `f1d30fd` 07-07 | ~28 | 0 | CI / GCC 2nd compiler |
| 18 | c58 `52e14c3` 07-08 | c63 `f16d927` 07-08 | 5 | 0 | freshness guard, 1st CI run |
| 19 | c76 `0e4cdb4` 07-10 | c81 `44f83cb` 07-10 | 5 | 0 | external referee (from prose) |
| 20 | in-review c86 07-10 | c86 `e176708` 07-10 | 0 | 0 | P4 Hurwitz twin (pre-commit) |
| 21 | c82 `903175b` 07-10 | c93 `095b028` 07-10 | 11 | 0 | branch's 1st CI (2nd compiler) |
| 22 | c83 `a2a5618` 07-10 | c95 `601ecfc` 07-10 | 12 | 0 | local GCC freshness |
| 26 | c108 `e2dbadc` 07-10 | c109 `92c9e86` 07-10 | 1 | 0 | human referee (R0), DAG-corroborated |
| **27** | c67 `d6834d0` 07-09 | c201 `4fd8286` 07-18 | **134** | **9** | source reading at M0b pinning |
| **28** | c106 `d3a7fd4` 07-10 | c208 `7450961` 07-18 (detect) / c212 `a7a5ca2` (fix) | **102 / 106** | **8** | referee C: digitized figure + blind re-transcription |
| 29 | c106 `d3a7fd4` 07-10 | c212 `a7a5ca2` 07-18 | 106 | 8 | bound expired at #28 correction |
| **30** | c112 `afc8e8c` 07-11 | c222 `37d341b` 07-19 | **110** | **8** | source reading (referee-C audit) |
| **31** | c222 `37d341b` 07-19 | c225 `1667bb3` 07-19 | **3** | 0 | `twin_ss_provider_fast_vs_m0b` drift guard, clean-clone gate |

**Distribution.** Δcommits sorted `{0,1,1,1,3,5,5,11,12,28,102,106,110,134}` →
**median ≈ 8 commits; worst-case 134 (#27)**. Δdays → **median 0; worst-case 9 (#27)**.
Two disjoint regimes: **same-session catches** (twin / sweep / CI / referee-in-review)
resolve at 0–12 commits and **0 days**; the **8–9-day cluster is entirely prose +
transcription** (#27 spec prose, #28/#29/#30 formula transcription) — the "twin-blind"
class the ledger itself names (a shared reading corrupts both twins identically).

### 2c. #28 and #31 explicitly

| | #28 (eq (2) products swapped) | #31 (eq (1) fix left incomplete) |
|---|---|---|
| Introduced | c106 `d3a7fd4` (doc origin c99 `aa2b0ae`) | c222 `37d341b` |
| Latent span | c106 → c208 = **102 commits, 8 days** | c222 → c225 = **3 commits, same day** |
| Detected | c208 `7450961` (digitized figure disagrees) | c225 `1667bb3` (drift guard on clean-clone gate) |
| Corrected | c212 `a7a5ca2` | c225 `1667bb3` |
| Detector class | external artifact (figure + blind re-transcription) — a twin could not catch it | in-repo standing invariant (twin), latent because the guarding build predated #30 |

### 2d. Guard-test payoff (introduced → first meaningful fire)

| Guard | Introduced | First fire | Payoff lag | Fired on |
|-------|-----------|-----------|------------|----------|
| `theorem_compose_group_axioms` | c25 `c1dee32` | c26 `4b10ed0` | 1 commit / 0 d | #6 (class-number twin had missed it) |
| `theorem_genus_2rank` | c25–c27 (stage 3) | c26 `4b10ed0` | ~1 commit / 0 d | #7 |
| CI 2nd-compiler (GCC) | c49 `2d22c73` | c53 `f1d30fd` | 4 commits / 0 d | #15; **recurred** #21 at c93 (branch un-CI'd, +40 commits) |
| `freshness_check.py` | c60 `a3e73d4` | c63 `f16d927` | 3 commits / 0 d | #18 — "fired on its first CI run" |
| `twin_ss_provider_fast_vs_m0b` (drift) | c159 `980dca0` | c225 `1667bb3` | **66 commits / 2 d** | #31 (only deferred-payoff guard) |
| `SS_GENERATOR_HASH` | M4 (P3) | — did **not** fire on #31 | (coverage gap) | widened at c225 to close it |

Most guards were introduced defensively and fired ~immediately (0 days). Two exceptions
carry the interesting signal: the drift guard sat 66 commits before its one catch; the
generator-hash *missed* #31 (its coverage excluded the `_m0b` byte-copy) and was widened
in the same commit that the drift guard forced.

---

## 3. Independent verification of (pre-registered → read) ordering

**Method.** Each pre-registration claims its design commit precedes the commit that
produced the data it judges. Verdicts below are re-derived purely from chronological
indices in the DAG, *then* diffed against the notes.

### 3a. Independent DAG verdict (derived before reading the notes' claims)

| Pre-registration | Design commit (idx) | Data/read commit (idx) | Order | Verdict |
|---|---|---|---|---|
| Stage 4 R2/R3 | c33 `c773aee` | c34 `f878a58` (LHS/RHS agree) | 33 < 34 | ✅ |
| Stage 5 R2 (Odlyzko) | c37 `7a38577` | c41 `0685e86` (full-coverage result) | 37 < 41 | ✅ |
| M1 scale-collapse null | — (no committed design) | c76 `0e4cdb4` (pipeline+result) | none | ⚠️ **unverifiable** |
| M2 convergence | c82 `903175b` | c83 `a2a5618` (confirmation) | 82 < 83 | ✅ |
| M3 trace-average | c88 `d30cb5f` | c89 `86edf44` (confirmation) | 88 < 89 | ✅ |
| M4 interpretation fork | c111 `6d50db0` | c112 `afc8e8c` (confirmation run) | 111 < 112 | ✅ |
| PR-1 open + Rung-1 | c116 `dd6beb0` | c120 `4fb7f84` cache → c121 `8c5575a` read | 116<120<121 | ✅ |
| PR-1 Rung-3 clause | c139 `4a17ebe` | c144 `fa4e35a` (Rung-2 read) / c164 `8f64ba1` (Rung-3 read) | 139<144<164 | ✅* |
| PR-2 rank-split threshold | c125 `f7415a4` | c128 `876999f` (split) | 125 < 128 | ✅ |
| PR-3 leakage estimator | c150 `21060a0` | c152 `0d21b62` (measured) | 150 < 152 | ✅ |
| M0b x18 production gate | c161 `00cdb94` | c162 `69c17d5` / c163 `dcd96a6` | 161 < 162 | ✅ |
| libm partial-diff spec | c169 `219b1c4` | c171 `47a8a54` / c174 `4cf914d` | 169 < 174 | ✅ |
| libm sampled cross-plat. | c172 `3b6235d` | c174 `4cf914d` | 172 < 174 | ✅ |
| referee B1/B2/B3 | c204 `2524c10` | c206 `a92481e` (results) | 204 < 206 | ✅ |

`*` **PR-1 Rung-3 caveat (found independently):** the Rung-3 clause `4a17ebe` (c139) is
preceded by the Rung-2 **N/ε oracle cache** `a09bb3e` (c133). Under a strict "any Rung-2
data" reading, 133 < 139 would fail; under a "Rung-2 *statistic was read*" reading it
passes (the read is `fa4e35a`, c144). The commit subject of `4a17ebe` — *"committed
before Rung 2 data"* — is the looser wording and reads as false on its face.

**Independent conclusion.** Every pre-registration with a committed design has
design-before-read in the DAG. The **sole failure is M1**, which has no committed design
at all (`0e4cdb4` bundles pipeline + null + result in one commit). The commit-before-run
rule is introduced at c81 `44f83cb`; every pre-registration *after* it passes, the one
*before* it (M1, c76) is unverifiable.

### 3b. Diff against the notes' own claims

| Claim source | Note's assertion | My DAG verdict | Diff |
|---|---|---|---|
| data-note §5, `dd6beb0`/`4a17ebe`→`8f64ba1` | "Rung-3 clause committed *before the 2¹⁷ rung was read*" | ✅ (139<144) | **Match.** Note's wording ("was read") is *more precise* than commit `4a17ebe`'s ("before Rung 2 data") and resolves the caveat flagged above. |
| data-note §5, PR-2 `f7415a4`→`876999f` | "threshold fixed before any curve was split" | ✅ (125<128) | Match |
| data-note §5, PR-3 `21060a0`→`0d21b62` | pre-registered → read | ✅ (150<152) | Match |
| m1-pinning §P4 / ERRATA #19 | "git record cannot corroborate the order … original design never committed" | ⚠️ unverifiable | **Match** — the note self-reports exactly the one failure the DAG exposes. |

**No discrepancy between the DAG and the notes.** The single ordering the DAG cannot
confirm (M1) is the single one the notes decline to claim.

---

## 4. Churn map

### 4a. Highest edit counts (distinct commits touching)

| File | Edits | | File | Edits |
|---|---|---|---|---|
| `verify/CMakeLists.txt` | 41 | | `docs/ERRATA.md` | 21 |
| `src/CMakeLists.txt` | 36 | | `docs/notes/data-note/CLAIMS-N.md` | 16 |
| `app/at.cpp` | 29 | | `.gitignore` | 15 |
| `docs/notes/data-note/data-note.md` | 24 | | `verify/validate_json.py` | 12 |

The two `CMakeLists` and `app/at.cpp` are structural aggregators (every new stage/test/CLI
verb touches them). `data-note.md` (24) and `ERRATA.md` (21) are the two documents that
accreted continuously.

### 4b. Files edited across the most distinct phases

| File | # phases | Edits | Reading |
|---|---|---|---|
| `.gitignore` | 6 / 6 | 15 | touched in every phase (PDFs, venv, caches, snapshots, username) |
| `app/at.cpp` | 5 | 29 | CLI grew a verb per stage across the whole life |
| `docs/ERRATA.md` | 5 | 21 | ledger spans P2→P6 |
| `verify/CMakeLists.txt` | 4 | 41 | |
| `src/CMakeLists.txt` | 4 | 36 | |
| `verify/freshness_check.py` | 4 | 10 | the guard itself was tuned repeatedly |
| `src/emit/emit_sawin_sutherland.cpp` | 4 | 7 | the M4/M5 emitter, hit by #28 + release |
| `README.md`, `CLAUDE.md`, `docs/papers/README.md` | 4 each | 7/6/5 | governance docs revised across phases |

### 4c. Propagation graph of the #28 correction

One root cause (eq (2) transcription, latent c99/c106) → a 16-commit cascade, c208→c227,
crossing every layer:

```
c208 7450961  DETECT  referee-C gate: digitized figure ≠ our trough → STOP
  ├ c209 c8aa3ab  PARI oracle from same internal quote AGREES @0.805  → twin-blind confirmed
  ├ c210 673e58b  code search: no authors' density code (Option C exhausted)
  └ c211 602d642  REBUILD oracle from paper → 0.870 (matches figure)
c212 a7a5ca2  FIX    src/murm/ss_density.{cpp,h}  (products + ES trace)      [#28,#29]
c213 dabbf44         + verify/m4/ss_density_tests.cpp (term-level anchors)
c214 1aa751d         referee-C prereq: extend B2/B3 invariants
c215 a695027  LEDGER docs/ERRATA.md  (#28 + #29 + sweep)
c216 49ab635         reframe-registry.md  ("draft once, propagate" — the propagation MAP)
c217 0bc0697         reframe: emitter/code/data verdict prose
c218 7e85ab6         fast ss_density twin vs ss_density_term
c219 eac7b81  DATA   viz/data/{sawin_sutherland,ss_rank_split,ss_x_extension}.json
c220 4d48794         explainers + prereg tests → agreement
c221 9ae0123  RATIFY "#28-fold are propagations, not separate errata"
c223 e0e52de         data-note reframe
c224 18c1364         data-note style pass
       … then the eq (1) sibling: c222 #30 (ss_empirical.cpp) → c225 #31 (ss_empirical_m0b.cpp) → c227 box-regen all four rungs
```

File-level confirmation of the #30→#31 incompleteness: `37d341b` (#30) edits
`src/murm/ss_empirical.cpp` **only**; `1667bb3` (#31) adds `src/murm/ss_empirical_m0b.cpp`
+ `CLAUDE.md` (new rule) + widens the hash in `src/CMakeLists.txt`. The repo built its own
propagation map (`reframe-registry.md`, c216) and then formally ratified (c221) that the
cascade is one erratum, not many.

---

## 5. Legibility audit (a stranger's confusion log)

Places where the artifacts alone did not explain themselves. Each is a stranger-cost,
listed as data.

| # | Where | What confused a cold reader | Resolvable only by |
|---|---|---|---|
| L1 | c165 `2ff195d` "B1 + B2 land" vs c204–c206 referee "B1/B2/B3 specs/results" | Same tokens `B1/B2`, two unrelated namespaces (data-note build-items vs referee-round measurements) 40 commits apart | reading both data-note.md and referee-round-2026-07.md |
| L2 | `R1/R2/R3` everywhere | Every stage & PR re-uses `R1/R2/R3`; a bare "R2" (c81 M-ladder, c86 anchors, c118 PR-1, c144 "Rung 2 (R2)") means a different rider each time | the owning pinning log per occurrence |
| L3 | c133 vs c144 both "PR-1 Rung 2" | Two commits titled "Rung 2" (N/ε cache vs confirmation read) 11 apart; "Rung 2" is overloaded | recognizing cache ≠ read |
| L4 | c139 `4a17ebe` "committed before Rung 2 data" | Reads false — the Rung-2 N/ε cache (c133) precedes it; only the data-note's "before the rung was *read*" rescues it | data-note §5 wording |
| L5 | ERRATA #26 (c109) ↔ #28 (c212) | #26 confidently frames the peek's trough *agreement* as the error and the "clean confirmation" *deviation* as truth; #28 silently **re-inverts** it 100 commits later (the deviation was the artifact). The forward reader builds the wrong model | reading #28, then re-reading #26's appended "[Re-inverted by #28]" |
| L6 | c112–c114, c189 "partial agreement / open trough deviation" | The M4/data-note central finding ("persistent trough displacement") is later dissolved as a transcription artifact; a reader of c106–c190 invests in a scientific deviation that is retroactively deleted at c212 | referee-round-2026-07.md + #28 |
| L7 | 9× `chore: re-emit … snapshot at clean tree (generated_by)` | Near-identical subjects; two consecutive identical M1 re-emits (c77, c79) with no stated delta; opaque without the freshness contract | freshness-guard.md + ARCHITECTURE |
| L8 | c209–c211 "referee C step 1 / step C / REBUILD" | Steps mix numbers, letters, and verbs; "Option C exhausted" cites an A/B/C framework defined only in the referee note | referee-round-2026-07.md |
| L9 | CI semantics drift | "CI green" means different coverage at c49 (build+subset), c60 (+freshness), c187 (`5b6e148` exclude m0b gate), c197 (`79efd05` "fast smoke check, not full suite") | diffing each ci.yml change |
| L10 | Undefined-forward shorthands | `M0b` first appears c124 with no forward definition (not in the RESEARCH-M M0–M5 ladder); `x16/x17/x18` = 2¹⁶⁻¹⁸; `(task #67)` at c201 cites an external tracker absent from the repo; "PR-4, retired" (data-note L456) names a pre-registration that never exists in `docs/preregistered/` | m0b-pinning.md; external context (unavailable) |
| L11 | c13 area — `filter-branch` re-sign | publication-audit records history was rewritten to purge PDFs (#13); early hashes cited in older docs need not match the current DAG | publication-audit.md |

The recurring shape: **local label namespaces** (`R*`, `B*`, "Rung 2", "step C") that
collide across phases, and **two findings that changed sign near the end** (#26 via #28;
the M4 "deviation") — both legible only by reading a later erratum backward into an
earlier confident claim.

---

**Data provenance.** `git log v1.0.0` (229 commits), `docs/ERRATA.md` (31 entries),
`docs/preregistered/PR-{1,2,3}.md`, `docs/notes/data-note/{data-note.md,CLAIMS-N.md}`,
`docs/notes/m1-pinning.md §P4`, `docs/RESEARCH-M.md §7`, and per-commit `--name-only`
churn. Latencies computed from commit dates and chronological indices; ordering verdicts
computed from indices independently of the notes, then diffed. Compiled 2026-07-19 from
artifacts only.
