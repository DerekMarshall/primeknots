# Methods dossier — token & compute accounting

Token and compute accounting for `primeknots` v1.0.0. **Measurement and estimate are kept
strictly separate.** §1 (Claude Code) is measured from session transcripts. §2 (Claude web) is an
estimate (chars÷4): a representative ≈ 1.29 M-token figure from the full conversations, plus a
reproducible ≈ 7,600-token floor from the lossy committed pastes; both carry a ±30% band and no
input/output split. §3 (compute) is recorded from committed artifacts. §4 is a join, no interpretation. Provenance
is stated on every number. Phases P1–P6 are as defined in `docs/notes/cold-retro-v1.0.0.md §1`.

---

## 1. Claude Code — MEASURED

**Source.** Session JSONL transcripts in
`~/.claude/projects/-Users-derekmarshall-src-primes-research/` (5 files). Parsed per-record:
assistant records carrying `message.usage` (`input_tokens`, `output_tokens`,
`cache_creation_input_tokens`, `cache_read_input_tokens`); date from each record's `timestamp`.

**De-duplication (required).** Two of the five files are a fork: all 10,592 message IDs in
`b6df21ab…` are also in `e8517b19…` (identical first-user record and timestamp,
`2026-07-07T10:50:18.720Z`) — `e8517b19` is a resume that contains `b6df21ab` in full. A naive
sum of all files double-counts 07-07→07-15. All figures below are **de-duplicated by API
message ID** (each usage record counted once). Contribution after de-dup: `e8517b19` 3,837
unique usage records; `b6df21ab` 0; `18692e96…` 20; `2d0e6819…` and `5d79ab7e…` carry no usage
records. Session files: 5; token-bearing after de-dup: 2 distinct conversations (the main
resumed session `e8517b19`, and a short 07-19 session `18692e96`).

**Reporting rule (applied).** The four token categories are never summed into one headline
number. Where a single figure is used it is **input + output**, with cache reported alongside.

### 1a. Per-day (de-duplicated)

Phase column uses the cold-retro §1 date→phase map; `P?|P?` marks a phase-boundary day (the
phase boundary falls mid-day, so that day's tokens are not date-separable between the two phases).

| Date | Phase | input | output | cache-creation | cache-read | assist. responses | user records |
|---|---|--:|--:|--:|--:|--:|--:|
| 2026-07-07 | P1\|P2 | 50,150 | 1,176,200 | 2,213,205 | 271,628,225 | 663 | 692 |
| 2026-07-08 | P2 | 29,506 | 371,719 | 1,966,558 | 189,874,181 | 288 | 302 |
| 2026-07-09 | P3 | 7,173 | 307,306 | 916,211 | 45,848,344 | 150 | 177 |
| 2026-07-10 | P3 | 36,419 | 802,803 | 3,220,151 | 247,553,620 | 499 | 530 |
| 2026-07-11 | P3\|P4 | 46,211 | 615,929 | 1,647,804 | 141,952,155 | 388 | 416 |
| 2026-07-12 | P4 | 18,759 | 421,683 | 3,121,290 | 114,300,438 | 243 | 252 |
| 2026-07-13 | P4 | 602 | 11,912 | 1,519,947 | 5,997,360 | 10 | 10 |
| 2026-07-14 | P4 | 673 | 29,565 | 2,324,191 | 10,109,037 | 16 | 16 |
| 2026-07-15 | P4 | 3,886 | 184,467 | 3,387,052 | 29,680,716 | 78 | 106 |
| 2026-07-16 | P4 | 353 | 538,594 | 3,212,180 | 97,786,679 | 180 | 202 |
| 2026-07-17 | P4\|P5 | 1,703 | 406,586 | 3,163,325 | 76,960,835 | 211 | 234 |
| 2026-07-18 | P5\|P6 | 1,108 | 1,314,561 | 3,067,174 | 280,064,557 | 555 | 673 |
| 2026-07-19 | P6 | 1,140 | 1,285,363 | 2,662,681 | 281,918,604 | 576 | 660 |
| **TOTAL** | | **197,683** | **7,466,688** | **32,421,769** | **1,793,674,751** | **3,857** | **4,270** |

Single-figure form: **input + output = 7,664,371**, alongside cache-creation 32,421,769 and
cache-read 1,793,674,751. "assist. responses" = usage-bearing assistant records (API
round-trips; exceeds the human-prompt count, since one prompt yields multiple tool-use
responses). "user records" includes tool-result records, not only human prompts.

### 1b. Per-phase (de-duplicated; boundary days not split)

Phases P2/P3/P4/P6 have date-isolable days; **P1 and P5 do not** — P1 shares its only calendar
day (07-07) with P2, and P5 shares both its days (07-17, 07-18) with P4 and P6. Boundary days
are their own rows.

| Phase / boundary | days | input | output | cache-creation | cache-read | assist. responses |
|---|---|--:|--:|--:|--:|--:|
| P1\|P2 boundary | 07-07 | 50,150 | 1,176,200 | 2,213,205 | 271,628,225 | 663 |
| P2 | 07-08 | 29,506 | 371,719 | 1,966,558 | 189,874,181 | 288 |
| P3 | 07-09,10 | 43,592 | 1,110,109 | 4,136,362 | 293,401,964 | 649 |
| P3\|P4 boundary | 07-11 | 46,211 | 615,929 | 1,647,804 | 141,952,155 | 388 |
| P4 | 07-12…16 | 24,273 | 1,186,221 | 13,564,660 | 257,874,230 | 527 |
| P4\|P5 boundary | 07-17 | 1,703 | 406,586 | 3,163,325 | 76,960,835 | 211 |
| P5\|P6 boundary | 07-18 | 1,108 | 1,314,561 | 3,067,174 | 280,064,557 | 555 |
| P6 | 07-19 | 1,140 | 1,285,363 | 2,662,681 | 281,918,604 | 576 |

### 1c. ccusage cross-check (verification)

`ccusage@latest session --json` (v20.0.17, run via npx) keys sessions by transcript UUID and
de-duplicates by message; it attributes the fork's shared history to `b6df21ab` and the new work
to `e8517b19`. Summing this project's three token-bearing sessions in ccusage:

| | input | output | cache-creation | cache-read |
|---|--:|--:|--:|--:|
| ccusage (b6df21ab + e8517b19 + 18692e96) | 208,430 | 7,476,100 | 32,468,572 | 1,796,432,359 |
| raw de-dup (this dossier, §1a total) | 197,683 | 7,466,688 | 32,421,769 | 1,793,674,751 |
| agreement | ~5% (smallest category) | 0.13% | 0.14% | 0.15% |

Output/cache agree to <0.2%; input (the smallest category) differs by ~10.7k tokens (~5%),
attributable to differing de-dup keys (message-ID here vs message-ID + request-ID in ccusage).
The per-day/per-phase breakdown (§1a/§1b) is from the raw parse by record timestamp; ccusage's
per-session `lastActivity` dates are session-level and do not give a temporal breakdown.

---

## 2. Claude web — ESTIMATED (chars÷4; representative ≈ 1.29 M, with a reproducible floor)

**Status: estimated (chars÷4), not measured.** The web layer — the Claude Fable
orchestration/referee session plus the external-review conversations (referee rounds, cold-clone,
blind transcription) — has no per-token usage record, so it is estimated by the chars÷4 proxy. The
sources carry no per-message role separation, so **no input/output split is possible — totals
only** (consequence: costs §1b prices the web total by bounding cases, not a blended point). Two
figures are kept distinct — a representative estimate and a reproducible floor:

- **Representative estimate: ≈ 1.29 M tokens (±30%: ~0.90–1.68 M).** chars÷4 over the full four
  web conversations; ≈ 17% of the §1 measured build input+output (7,664,371). This is the figure
  the methods companion cites (§6). It rests on the full browser exports and is **not reproducible
  from the inputs committed in this repo** — the committed pastes are a lossy subset (see the
  floor). Carried per Derek's decision (Option A); the full exports are the basis.
- **Reproducible floor: ≈ 7,600 tokens.** chars÷4 over the *lossy* browser-copy pastes actually
  available here (reproduced into `scratchpad/web/*.txt`, `wc -c`-counted). A hard lower bound —
  the largest conversation is head-truncated, tool I/O and attachments are collapsed, and the
  blind-transcription conversation is absent — retained for reproducibility, **not** representative.

### 2a. Reproducible floor, per conversation (chars÷4 over the committed lossy pastes)

| Tag | In paste? | chars | tokens (÷4) | ±30% band | Basis / bound |
|---|---|--:|--:|--:|---|
| orchestration | yes, **head-truncated** | ~14,700 | ~3,700 | 2,600–4,800 | hard lower bound — paste begins mid-token (`…ATIONS —` = a truncated `MURMURATIONS`); attachments (data-note.md, CLAIMS-N.md, README) + tool I/O collapsed to stubs |
| referee rounds | yes | ~11,200 | ~2,800 | 2,000–3,600 | fairly complete (SS-author prompt + full report + 2²⁸ exchange) |
| cold-clone | yes | ~4,500 | ~1,100 | 800–1,400 | lower bound — 20+ tool runs collapsed (`Ran 20 commands`) |
| blind transcription | **NO** | — | — | — | **NOT SUPPLIED** — named in the export plan and referenced in-paste, no conversation text present; no figure invented |
| **Floor total (3 of 4)** | | **~30,400** | **~7,600** | **5,300–9,900** | **floor** (roles stripped; one conversation absent; largest truncated) |

The gap between the ~7,600 floor and the ~1.29 M representative figure is exactly the browser
copies' losses (collapsed tool I/O, collapsed attachments, truncated head, one missing
conversation): the committed pastes preserve only a fraction of the conversations they sample. The
representative figure is not adjusted toward an expected value (rule 1) — it is the full-export
chars÷4 total that the companion carries; the floor is what this repo's committed inputs
reproduce. Neither is combined with the §1 measured Claude Code tokens into a single number.

---

## 3. Compute — RECORDED (from committed artifacts)

Heterogeneous hardware (12-thread laptop, 48-core FreeBSD box, GitHub ubuntu runner); **not
summed** into one figure. One row per recorded job, source cited.

| Job | Wall time | Hardware | Source |
|---|---|---|---|
| PR-1 Rung 1 (2¹⁶) a_p statistic | 36,748 s ≈ 10.2 h (7,541 s to 2,048 curves + 29,207 s resume) | 12-thread laptop | PR-1.md:323, :329 |
| PR-1 Rung 2 (2¹⁷) a_p statistic | 54,020 s ≈ 15.0 h | 48-core FreeBSD (clang 21, EPYC), chunk 256 | PR-1.md:374 |
| PR-1 Rung 3 (2¹⁸) a_p statistic | 1,014 s ≈ 16.9 min | 48-core box, M0b `ap_shanks_mestre` O(p¹ᐟ⁴) | PR-1.md:434 |
| M0b reference a_p cache generation (2¹⁶/2¹⁷) | 36,860 s | 12-thread laptop, frozen charsum | m0b-pinning.md:252 |
| Box re-generation, all four rungs (corrected binning, ERRATA #30) | 39 + 69 + 272 + 1,013 s = 1,393 s | 48-core box, M0b | docs/ERRATA.md #30 |
| Abandoned laptop clean a_p regen | 4.9 h spent (7.3% of work) → projected ~67 h floor / 3–5 d; abandoned | laptop QR-table | libm-partial-diff-spec.md:114, :124–125 |
| 2¹⁸ heavy tail-weighted twin gate (`-L heavy`, run separately) | ~1–2 h | 48-core box | CLAIMS-N N5-5; m0b-pinning §7 |
| CI (GitHub Actions), workflow `ci.yml`, 74 runs (60 pass / 11 fail / 3 cancel) | 52,884 s wall ≈ 14.69 h | ubuntu-latest, 1 job `build-and-test` | `gh run list --workflow=ci.yml` (retrieved 2026-07-19) |

Notes: PR-1 Rung-1/Rung-2 X^2.65 scaling and the 2¹⁸ ≈ 3.9 d pre-M0b projection are in
PR-1.md:329, :378 (projection, not a run — excluded from the recorded rows). CI wall time is
per-run elapsed on a single job; it is not multiplied by a core factor. The box regen (1,393 s)
is the only compute newly spent in P6.

---

## 4. Derived join — tokens & compute per phase against deliverables & errata

A join of §1b (tokens), §3 (compute), and `cold-retro-v1.0.0.md` §1 (deliverables) / §2b
(errata by introduce/detect commit). No interpretation. Errata are listed by the phase of the
*introducing* commit; a `→Pn` suffix gives the detection phase where it differs (long-latency
transcription errata). Tokens are input+output with cache alongside (per §1's rule); boundary-day
tokens are shown on the joint row, as in §1b.

| Phase | tokens in+out (cc / cr) | compute in phase | deliverables (cold-retro §1) | errata introduced (detected) |
|---|---|---|---|---|
| P1 (07-07, shared w/ P2) | see P1\|P2 row | none recorded | Stages 0–6 ladder (`src/{core,symbols,linking,redei,classgroup,cs,zeta,dw}`, `verify/`) | #1–#12, #14 introduced & detected same-day; #15 introduced (→P2) |
| P1\|P2 boundary (07-07) | 1,226,350 (2,213,205 / 271,628,225) | none | (P1 ladder + P2 CI/deck start) | — |
| P2 (07-08) | 401,225 (1,966,558 / 189,874,181) | none | publication / CI / deck; `freshness_check.py` | #15 detected (CI); #16, #17 (deck); #18 (freshness guard) |
| P3 (07-09,10) | 1,153,701 (4,136,362 / 293,401,964) | none (compute in P4) | M-ladder M0–M4 (`src/{ell,mform,murm}`, `verify/m*`) | #19, #20, #21, #22, #26 same-day; #24; #27 intro (→07-18); #28, #29 intro (→07-18) |
| P3\|P4 boundary (07-11) | 662,140 (1,647,804 / 141,952,155) | (PR-1 rungs begin) | M4 close-out / M5 open (`dd6beb0`) | #30 intro (→P6) |
| P4 (07-12…16) | 1,210,494 (13,564,660 / 257,874,230) | PR-1 Rung-1 36,748 s (2¹⁶), Rung-2 54,020 s (2¹⁷); M0b cache-gen 36,860 s; abandoned laptop regen 4.9 h | M5 PR-1/2/3 (`docs/preregistered`), heavy compute, `data/m5` caches | (none introduced in these days per §2b) |
| P4\|P5 boundary (07-17) | 408,289 (3,163,325 / 76,960,835) | PR-1 Rung-3 1,014 s (2¹⁸); M0b x18 gate | PR-1 Rung-3 verdict H0 (`8f64ba1`); data-note open (`2ff195d`) | #27 detected span begins |
| P5\|P6 boundary (07-18) | 1,315,669 (3,067,174 / 280,064,557) | none | data-note authoring, libm-diff, Layer-1 QA; referee A–D begin | #27 detected (`4fd8286`); #28 detected (`7450961`), fixed (`a7a5ca2`); #29 fixed |
| P6 (07-19) | 1,286,503 (2,662,681 / 281,918,604) | box regen 1,393 s (all four rungs) | eq(2)/eq(1) corrections, ERRATA #30/#31, `viz/data` regen, CITATION/Pages, release | #30 detected (`37d341b`→corrected); #31 introduced (`37d341b`) & detected (`1667bb3`) |

CI wall time (14.69 h over 74 runs) spans P2→P6 and is not phase-attributable from
`gh run list` at day granularity here; it is recorded once in §3.

---

**Provenance summary.** §1 measured from `~/.claude/projects/…/*.jsonl` usage blocks,
de-duplicated by message ID, cross-checked against ccusage 20.0.17. §2 estimated by chars÷4:
representative ≈ 1.29 M tokens (full conversations, Option A) with a reproducible ≈ 7,600-token
floor from the committed lossy pastes; ±30% band, totals only (no role split). §3 recorded
from PR-1.md, m0b-pinning.md,
libm-partial-diff-spec.md, docs/ERRATA.md #30, CLAIMS-N N5-5, and `gh run list`. §4 joins the
three against cold-retro-v1.0.0.md §1/§2b. Compiled 2026-07-19.
