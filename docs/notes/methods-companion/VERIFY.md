# methods-companion — verification (claims-ledger-lite)

Every quantitative claim in `methods-companion.md` checked against the committed dossiers before
render (v1.1.0 prep, 2026-07-19). Not a full CLAIMS ledger — a one-pass cross-check.

## Figures cross-checked

| Companion claim | Value | Source (committed) | Verdict |
|---|---|---|---|
| API round-trips | 3,857 | `methods-dossier-usage.md §1a` TOTAL | ✅ match |
| Token table (4 rows) | 197,683 / 7,466,688 / 32,421,769 / 1,793,674,751 | `methods-dossier-costs.md §1a` | ✅ match |
| Build total | ~$1,288 | costs §1a ($1,287.13) | ✅ match |
| Cache re-read / round-trip | ~465,000 | 1,793,674,751 ÷ 3,857 = 464,940 | ✅ match |
| Steering share | 2.6% | 197,683 ÷ 7,466,688 = 2.65% | ✅ **corrected** (was 2.5%) |
| Guard-test latency | 66 commits | `cold-retro-v1.0.0.md §2d` | ✅ match |
| CI hours | 14.7 | usage §3 (14.69 h) | ✅ match |
| Compute hours | 17.2 / 25.3 | costs §2 | ✅ match |
| Compute cost | $42 / $21 / $63 | costs §2 | ✅ match |
| Provider ratio | ~145× | m0b-pinning §7; data-note; CLAIMS-N N2-14 | ✅ match |
| Errata count | 31 | `ERRATA.md` (31 rows) | ✅ match |
| Latency bimodality | 0 d internal; 8–9 d / >100 commits shared-reading | cold-retro §2b (#28 102c/8d, #30 110c/8d) | ✅ match |
| CPU-years | 0.13 vs 140 | costs §3; [SS25] §3.1 | ✅ match |
| Claim markers | 49 | `data-note.md` (49 `claim:` markers, at v1.0.0 and now) | ✅ **corrected** (was 47) |
| Web tokens | ~1.29 M | usage §2 representative (Option A); reproducible ~7,600 floor | ✅ reconciled |
| Web cost | $15–$65 | costs §1b (1.29 M × Fable bounding = $12.9–$64.5) | ✅ reconciled |

## π anecdote — timeline verified

The anecdote's earlier "recalled rather than ledgered: it predates the errata system" was **false**
and was corrected to cite the incident's actual record: **erratum #25, commit `fecb9d9`**
("CONSTANTS INCIDENT... kPi = 3.14159265358979311600, the double's decimal... caught by Derek
reading the diff"). The verb "produced this rule" was verified from git: the initial `CLAUDE.md`
(`fb305d2`) carried **6** non-negotiables and no constants rule; "from memory" first entered
`CLAUDE.md` in `fecb9d9` — so the incident produced the rule (not "hardened" a pre-existing one).

## Style pass

Full S1–S8 pass applied to the author's prose (banned vocab incl. `load-bearing`, minted
aphorisms, negative parallelism, dramatic metaphors, contractions; `Working title:` dropped).
"hilariously" (§3) kept by author's decision. The KaTeX pass no-ops (no math): `render.sh` asserts
zero `katex` spans and that dollar amounts survive as literal text.
