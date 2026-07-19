# Methods dossier — cost accounting

API-equivalent and AWS-equivalent costs for `primeknots` v1.0.0. Three tables, provenance per
row, estimates labelled. Rates are inputs (cited); token/compute quantities are from
`methods-dossier-usage.md`. **These are transferable equivalent-cost figures, not out-of-pocket:**
actual spend was Claude Team seats + a Google subscription (flat, not metered); the
API-equivalent and AWS-equivalent figures are what the same work would cost at the cited public
rates.

---

## 1. LLM API-equivalent cost

Rates: **Anthropic published pricing as of July 2026, supplied by Derek.**

### 1a. Opus Code sessions — MEASURED

Token quantities from `methods-dossier-usage.md §1a` (de-duplicated Claude Code totals).

| Category | Tokens | Rate | Cost |
|---|--:|---|--:|
| input | 197,683 | $5.00 / M | $0.99 |
| output | 7,466,688 | $25.00 / M | $186.67 |
| cache-write (cache-creation) | 32,421,769 | $6.25 / M | $202.64 |
| cache-read | 1,793,674,751 | $0.50 / M | $896.84 |
| **Total (Opus Code, API-equivalent)** | | | **$1,287.13** |

Source: quantities `methods-dossier-usage.md §1a`; rates July-2026 Anthropic pricing (Derek).
The four categories are priced separately and summed only within this single model+session class.

### 1b. Web / Fable sessions — ESTIMATE, PENDING

**PENDING** the `methods-dossier-usage.md §2` estimate (the Claude web export archive is not yet
supplied). When §2 lands, price at **Fable 5 rates: input $10/M, output $50/M, cache-read
$1.00/M** (supplied by Derek). The §2 estimate is a chars÷4 proxy with a **±30% band**; that band
is carried through to dollars (the dollar figure will read `$X ±30%`). The §2 estimate carries no
cache-token breakdown (chars÷4 is input/output only), so the cache-read rate applies only if the
export exposes cache tokens; otherwise the web cost is input+output estimate × the two rates.
No web dollar figure is stated until the archive is parsed.

### 1c. Gemini legs — UNMEASURED

Gemini Deep Research (upstream) and Gemini secondary review: **subscription, unmeasured** (no
per-token record; Google subscription, flat). One line, no figure.

**Out-of-pocket vs equivalent.** Actual out-of-pocket = Claude Team seats + Google subscription.
The API-equivalent figure ($1,287.13 Opus Code, measured; + web pending) is the transferable
estimate, not what was billed.

---

## 2. Compute AWS-equivalent cost

Wall-hours from `methods-dossier-usage.md §3` (recorded). Instance choices and the ~$65 total are
Derek-supplied; **the two hourly rates are unverified — `[RATE-VERIFY]` until Derek checks the AWS
console** (per instruction, this table does not commit specific $/h from memory).

| Class | Jobs (wall-hours) | Total h | Instance (vCPU) | On-demand rate | AWS-equivalent cost |
|---|---|--:|---|---|--:|
| Box | Rung-2 15.0 + Rung-3 0.28 + regen 0.39 + heavy gate ~1.5 | 17.2 | c7a.12xlarge (48) | `[RATE-VERIFY]` | `[RATE-VERIFY]` = 17.2 × rate |
| Laptop | Rung-1 10.2 + cache-gen 10.2 + abandoned regen 4.9 | 25.3 | c7a.4xlarge (16) | `[RATE-VERIFY]` | `[RATE-VERIFY]` = 25.3 × rate |
| CI (GitHub Actions) | 74 runs, 14.69 wall-h | 14.69 | ubuntu-latest | $0 | **$0** — GitHub Actions is free for public repositories |
| **Total (AWS-equivalent)** | | | | | **~$65 (Derek-supplied anchor; per-row pending `[RATE-VERIFY]`)** |

Source: wall-hours `methods-dossier-usage.md §3` (PR-1.md, m0b-pinning.md, libm-partial-diff-spec.md,
ERRATA #30, `gh run list`); instance types + ~$65 total supplied by Derek; hourly rates
`[RATE-VERIFY]`. CI is $0 because the repository is public (GitHub Actions minutes are not billed
for public repos). At the ~$65 anchor and the 17.2 h / 25.3 h split, the two rates are
order-$2/h (box) and order-$1/h (laptop); the exact per-row costs follow once the rates are
verified.

---

## 3. Grand summary

| Axis | Figure | Basis |
|---|---|---|
| Tokens (LLM API-equivalent) | **~$1.3K** ($1,287.13 Opus Code measured; **+ web/Fable PENDING**) | Table 1 |
| Compute (AWS-equivalent) | **~$65** (Derek anchor; per-row `[RATE-VERIFY]`; CI $0) | Table 2 |
| Scale anchor (external) | **[SS25] §3.1: "about 140 CPU-years of compute time"** for their 2²⁸ computation | Sawin–Sutherland arXiv:2504.12295 §3.1 (verified in the pinned PDF) |

**Compute-vs-anchor join** (no interpretation). This project's recorded compute, as CPU-hours
(wall-hours × cores in use): box 17.2 h × 48 = 825.6 CPU-h; laptop 25.3 h × 12 threads = 303.6
CPU-h; total ≈ **1,129 CPU-hours ≈ 0.13 CPU-years**. [SS25] §3.1 reports **≈ 140 CPU-years** for
their 2²⁸ computation; this project's compared computation reaches 2¹⁸. The two figures are placed
side by side as a scale anchor; no ratio or inference is drawn.

---

**Provenance summary.** Table 1a: measured token quantities (`methods-dossier-usage.md §1a`) ×
July-2026 Anthropic rates (Derek-supplied). Table 1b: estimate, pending the §2 archive, Fable 5
rates (Derek), ±30% band carried through. Table 1c: unmeasured (subscription). Table 2: recorded
wall-hours × `[RATE-VERIFY]` AWS on-demand rates (Derek to verify), CI $0 (public repo). Table 3:
join of Tables 1–2 with [SS25] §3.1 (140 CPU-years, verified in `docs/papers/sawin-sutherland-2504.12295.pdf`).
Compiled 2026-07-19.
