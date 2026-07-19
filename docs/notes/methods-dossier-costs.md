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

### 1b. Web / Fable sessions — ESTIMATE (a FLOOR; role split unavailable → bounding cases)

Token quantity from `methods-dossier-usage.md §2a`: **~7,600 tokens (±30%: 5,300–9,900), a floor**
— the browser-copy pastes strip roles, collapse tool I/O and attachments, truncate the largest
(orchestration) conversation, and omit the blind-transcription conversation entirely. Priced at
**Fable 5 rates: input $10/M, output $50/M** (Derek-supplied). Because the pastes carry **no role
split**, the total is priced by **bounding cases**, not a single blended number:

| Case | Rate | Cost (point) | Cost (±30% on tokens) |
|---|---|--:|--:|
| all-input | $10/M | $0.08 | $0.05–$0.10 |
| all-output | $50/M | $0.38 | $0.27–$0.49 |
| output-heavy blend (**stated assumption**: ~70% output) | 0.7·$50 + 0.3·$10 = $38/M | $0.29 | $0.20–$0.38 |

**Range reported: $0.08–$0.38** (bounding cases at the point token count), or **$0.05–$0.49** with
the ±30% band folded in. The visible paste text is predominantly assistant-authored (the referee
report, the research appraisal, the cost analysis, the relayed agent-prompts), so the output-heavy
end is the likelier regime — but that is stated as an assumption, not assumed away, and **no single
confident number is given**. cache-read tokens are not in a chars÷4 proxy, so the $1.00/M cache
rate does not apply.

**Floor, not the web cost.** Because the §2 token count is a floor (`usage.md §2`), this dollar
figure is a floor: it prices only the text the browser copies preserved. The representative web
cost is higher — by a factor the pastes do not let us bound — and needs the structured export; it
is **not** estimated here (rule 1: not adjusted toward an expected value).

### 1c. Gemini legs — UNMEASURED

Gemini Deep Research (upstream) and Gemini secondary review: **subscription, unmeasured** (no
per-token record; Google subscription, flat). One line, no figure.

**Out-of-pocket vs equivalent.** Actual out-of-pocket = Claude Team seats + Google subscription.
The API-equivalent figure ($1,287.13 Opus Code, measured; + web/Fable ≥ ~$0.1 floor from pastes)
is the transferable estimate, not what was billed.

---

## 2. Compute AWS-equivalent cost

Wall-hours from `methods-dossier-usage.md §3` (recorded). Instance choices and the ~$65 total are
Derek-supplied; the two hourly rates are a **web guesstimate** — us-east-1 **on-demand**, published
list price (Vantage/CloudOptimo/Economize, retrieved 2026-07-19), *not* a console/actual-region
figure. Region and pricing model are an assumption (spot is ~3–5× cheaper; other regions vary
~±10%); Derek's AWS-console check supersedes if an exact figure is wanted.

| Class | Jobs (wall-hours) | Total h | Instance (vCPU) | On-demand rate | AWS-equivalent cost |
|---|---|--:|---|---|--:|
| Box | Rung-2 15.0 + Rung-3 0.28 + regen 0.39 + heavy gate ~1.5 | 17.2 | c7a.12xlarge (48) | ~$2.46/h (est.) | ~$42 (17.2 × 2.4634) |
| Laptop | Rung-1 10.2 + cache-gen 10.2 + abandoned regen 4.9 | 25.3 | c7a.4xlarge (16) | ~$0.82/h (est.) | ~$21 (25.3 × 0.8214) |
| CI (GitHub Actions) | 74 runs, 14.69 wall-h | 14.69 | ubuntu-latest | $0 | **$0** — GitHub Actions is free for public repositories |
| **Total (AWS-equivalent)** | | | | | **~$63 (web-rate estimate; consistent with Derek's ~$65 anchor)** |

Source: wall-hours `methods-dossier-usage.md §3` (PR-1.md, m0b-pinning.md, libm-partial-diff-spec.md,
ERRATA #30, `gh run list`); instance types + ~$65 anchor supplied by Derek; hourly rates a web
guesstimate (c7a.12xlarge $2.4634/h, c7a.4xlarge $0.8214/h — the two are internally consistent at
~$0.0513/vCPU-h, us-east-1 on-demand, retrieved 2026-07-19). CI is $0 because the repository is
public (GitHub Actions minutes are not billed for public repos). The web rates land the total at
~$63, confirming Derek's ~$65 anchor to within the hour/rate rounding.

---

## 3. Grand summary

| Axis | Figure | Basis |
|---|---|---|
| Tokens (LLM API-equivalent) | **~$1.3K** ($1,287.13 Opus Code measured; **+ web/Fable ≥ ~$0.1 floor** — $0.08–$0.38 from lossy pastes, representative figure needs the structured export) | Table 1 |
| Compute (AWS-equivalent) | **~$63** (web-rate estimate: box ~$42 + laptop ~$21 + CI $0; consistent with Derek's ~$65 anchor) | Table 2 |
| Scale anchor (external) | **[SS25] §3.1: "about 140 CPU-years of compute time"** for their 2²⁸ computation | Sawin–Sutherland arXiv:2504.12295 §3.1 (verified in the pinned PDF) |

**Compute-vs-anchor join** (no interpretation). This project's recorded compute, as CPU-hours
(wall-hours × cores in use): box 17.2 h × 48 = 825.6 CPU-h; laptop 25.3 h × 12 threads = 303.6
CPU-h; total ≈ **1,129 CPU-hours ≈ 0.13 CPU-years**. [SS25] §3.1 reports **≈ 140 CPU-years** for
their 2²⁸ computation; this project's compared computation reaches 2¹⁸. The two figures are placed
side by side as a scale anchor; no ratio or inference is drawn.

---

**Provenance summary.** Table 1a: measured token quantities (`methods-dossier-usage.md §1a`) ×
July-2026 Anthropic rates (Derek-supplied). Table 1b: estimate from `usage.md §2a` (~7,600 tokens,
a floor), Fable 5 rates (Derek), priced by bounding cases (no role split), ±30% band carried
through; range $0.08–$0.38 (floor). Table 1c: unmeasured (subscription). Table 2: recorded
wall-hours × web-guesstimate AWS on-demand rates (c7a.12xlarge $2.46/h, c7a.4xlarge $0.82/h,
us-east-1, retrieved 2026-07-19; console-supersedable), CI $0 (public repo). Table 3:
join of Tables 1–2 with [SS25] §3.1 (140 CPU-years, verified in `docs/papers/sawin-sutherland-2504.12295.pdf`).
Compiled 2026-07-19.
