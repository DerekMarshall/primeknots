# Layer-1 pre-release QA sweep

Mechanical sweep before Checkpoint B (Derek, 2026-07-18). One checklist, one written result per
item. **Findings are recorded here; fixes are separate commits that cite the finding** — nothing
is silently fixed. Items that cannot be verified here are marked *unverifiable-with-reason*, not
skipped. Nothing merges; Checkpoint B is still pending Derek's read.

## Environment (item 1 record)

- **Clean-room:** fresh `git clone` from **origin** (`github.com/DerekMarshall/primeknots`),
  branch `murmurations`, HEAD **`5d37769`**, into a `/tmp` directory outside the worktree.
- **Platform/toolchain (local):** macOS (Darwin 25.5.0), **Apple clang 21.0.0**, CMake + Ninja,
  `python3 -m venv` with `jsonschema` (matches the CI setup).
- **CI:** GitHub Actions `ubuntu-latest`, GNU `g++` + Ninja, per `.github/workflows/ci.yml`.

## Checklist

### 1. Clean-room build + CI green at HEAD — **PASS (build)** / **FINDING (CI)**
Fresh clone @`5d37769`: `cmake -G Ninja -DCMAKE_BUILD_TYPE=Release` configure **OK**, build **OK**,
routine `ctest -LE heavy` **GREEN** (246.8 s). Visible, expected SKIPs (oracle/gitignored-cache
absent in a clean clone, the visible-skip contract): `anchor_zeros_match_odlyzko`,
`oracle_belabas_cubic_regen`, `prereg_ap_cache_reproduces_shape`, `twin_m0b_vs_charsum_x16`,
`twin_m0b_vs_charsum_x17`. **Freshness byte-identical** (49.3 s). No compiler warnings; 3 benign
macOS-`ld` "ignoring duplicate libraries" warnings (`at_core` ×2, `at_mform` ×1 — `verify_m4/m5`
list `at_core` directly while `at_murm` already `PUBLIC`-links it; GNU `ld` on CI does not warn).

**FINDING (CI hammering):** the routine CI `ctest` ran *every* registered test, including
`m5gate.twin_m0b_bruteforce_x18_tailweighted` (~27.9 M `a_p`) — a deliberate ~1–2 h 48-core-box run
(m0b-pinning §7). On the 2-core Actions runner it took hours; recent runs sat `in_progress`
1–2 h+, never promptly green, hammering Actions.
**Fix (applied urgently, Derek's directive):** `.github/workflows/ci.yml` Test step now runs
`ctest -LE heavy`, excluding the `heavy` gate from CI — commit **`5b6e148`**. Five stuck runs were
cancelled (`gh run cancel`).
**Unverifiable-here-by-design:** the heavy m0b gate itself was *not* run in this sweep (multi-hour
on the laptop); its green status is on the 48-core FreeBSD box per m0b-pinning §7.

### 2. Hygiene grep (all committed files, headers included) — **FINDING (absolute paths)**
- Secrets (API keys, tokens, `-----BEGIN … PRIVATE`, `ghp_`/`sk-`/`AKIA`/`xox`): **NONE.**
- Infra (FreeBSD box IP `10.46.6.100`, `tripstack` hostname, `freebsd@` user): **NONE committed.**
- `/home/` paths: none. `/tmp`: only `oracle/pari.cpp:43` (`mkstemp("/tmp/at_gp_XXXXXX")`, the
  legitimate GP-bridge temp file) and `libm-partial-diff-spec.md:176` prose ("kept off `/tmp`") —
  both benign, not leaks.
- **FINDING (absolute laptop paths in committed data headers):** the ss-partials headers embed the
  full laptop `ne_cache` path `/Users/derekmarshall/src/…/ne_cache_x{…}.txt`:
  `data/m5/ss_partials_x65536.txt:16`, `data/m5/ss_partials_x131072.txt.ckpt:16` (written by
  `write_ss_partials` from the invocation path; `libm-partial-diff-spec.md:56` mentions it
  deliberately).
  **Fix — recorded, NOT applied (non-trivial):** the ckpt's sha256 is *pinned* (item 6), so editing
  its header breaks the pin; the field is generator-written. A proper fix relativizes the
  `ne_cache` path in the partials writer, regenerates the non-pinned partials, and re-pins the
  ckpt — a deliberate follow-up, not a mechanical sweep edit. **FLAGGED for Derek.**

### 3. Hash audit — **PASS**
All 9 cited 7-hex commit hashes resolve (`git cat-file -e`): `dd6beb0`, `4a17ebe`, `8f64ba1`,
`f7415a4`, `876999f`, `21060a0`, `0d21b62`, `ce5f974`, `e8396cb`. All 4 (pre-registered → read)
pairs verified in-order (`git merge-base --is-ancestor`): PR-1 `dd6beb0`→`8f64ba1`; Rung-3
`4a17ebe`→`8f64ba1`; PR-2 `f7415a4`→`876999f`; PR-3 `21060a0`→`0d21b62`.

### 4. Link audit — **FINDING (one rotted line number)**
Every intra-repo file path cited in the note + ledgers resolves at HEAD. Line-cites checked:
`ss_empirical.h:17–19` (statistic-formula comment) ✓, `CMakeLists.txt:163–173` (ffp-contract
list) ✓.
**FINDING:** `src/emit/emit_sawin_sutherland.cpp:28` (cited in §5 / N5-3 for `setprecision(12)`)
has ROTTED — the `#include <stdexcept>` added during the math conversion shifted `setprecision(12)`
to **line 29** (line 28 is now `std::ostringstream o;`).
**Fix:** update the cite `:28`→`:29` — commit `fedc198`.

### 5. Placeholder audit — **FINDING (stale placeholders + dirty stamps)**
Sanctioned, kept: release tag `[PLACEHOLDER: v1.0.0]` and Zenodo DOI `[PLACEHOLDER]` (data-note §5,
N5-5, README, HTML).
**FINDING (stale, non-sanctioned):**
- `CLAIMS-N.md:23` status — "§5–§6 and the Abstract are `[PENDING]`" (they are complete).
- `README.md:51` status — "§5/§6/Abstract are stubs marked `[PENDING]`" (complete).
- `data-note.md:2` top editorial comment — "§3–§6 are stubs marked `[PENDING]`" (stripped from the
  render, but stale in source).
- `data-note.md:16` authorship comment — "co-authorship/affiliation PLACEHOLDER; title still
  `[PLACEHOLDER]`" (title is final; sole author decided).
- `figures/make_figures.py:136` — "`# [PENDING]` final print styling … when §3/§4 are drafted"
  (styling done; §3/§4 landed).
**FINDING (dirty version stamps — the clean-release "all-stamps-audit"):** 3 committed `viz/data`
snapshots carry a `-dirty` `generated_by`: `dirichlet_murmuration.json` +
`zubrilina_murmuration.json` (`601ecfc-dirty`), `ss_x_extension_murmuration.json`
(`749c900-dirty`). Freshness normalizes the stamp (not a test failure) — cosmetic provenance.
**Fix:** (a) update the stale status/comments — commit `b835350`; (b) clean-re-emit the 3
snapshots at a clean tree — commit `e09f2e7`.

### 6. `.gitignore` exception — ckpt present + sha — **PASS**
`data/m5/ss_partials_x131072.txt.ckpt` is present in the clean clone; its sha256 =
`1255608e9592805b7671501cd6dda52115b330af5debd9f8731e9716e06b782c`, **exactly** the value pinned in
`docs/notes/libm-partial-diff-spec.md`.

### 7. Copyright — docs/papers PDFs absent from all history — **PASS**
`git log --all --diff-filter=A -- docs/papers/*.pdf` is empty: the third-party PDFs were never
committed (gitignored from the start), not merely currently-ignored.

### 8. CITATION.cff + LICENSE — **PARTIAL** (LICENSE PASS; CITATION.cff unverifiable)
- **LICENSE — PASS:** MIT ("Copyright (c) 2026 Derek Marshall"), consistent with README's
  `[MIT](LICENSE)`. No conflicting license assertion found.
- **CITATION.cff — UNVERIFIABLE:** the file does not exist (a Phase-3 deliverable per
  clean-release-sweep.md + task #73), and `cffconvert` is not installed. Deferred to Phase 3
  (create with title / author / ORCID `0009-0001-8101-3908`, then validate).

### 9. Bounded code touching — **PASS (no src/ action needed)**
- `TODO`/`FIXME`/`XXX` in `src/` + `app/`: **NONE.**
- Dead files: every tracked `src/**/*.cpp` and `app/**/*.cpp` is referenced by a CMakeLists — no
  orphans to delete.
- Compiler warnings in the clean build: **NONE** (only the 3 benign macOS-`ld` duplicate-library
  linker warnings noted in item 1). No `src/` code change made, so freshness re-verify is N/A here.

## clean-release-sweep.md items
- M3 snapshot stamp (`601ecfc-dirty`): folded into the item-5 dirty-stamp finding → clean re-emit.
- All-snapshot-stamps audit: **DONE** — 3 dirty (dirichlet, zubrilina, ss_x_extension); the rest are
  clean (`558efe8`, `81501a0`, `8672b2d`×6, `d591f10`, `f16d927`×3).
- Deck M-stage rows: **DEFERRED** (the RESEARCH/deck extension is deferred; out of Layer-1 scope).
- README scope note: applied — commit `9953a10`.
- CITATION.cff (Phase 3) / Zenodo deposit metadata (Phase 4): deferred (item 8).

## Residual decision recorded
- **Ш (Cyrillic Sha) in the PDF:** the STIX-Two-Text system-font fallback triggers a tectonic
  "may-not-be-reproducible" advisory. **ACCEPTED as residual** (Derek, 2026-07-18): the PDF is a
  gitignored build product (reproducibility moot), vendoring a Cyrillic font is disproportionate,
  and spelling out Ш would degrade the primary HTML's notation.

## Summary
PASS: items 3, 6, 7, 9, LICENSE (8). PASS-with-fix: 1 (build; CI fixed `5b6e148`), 4 (line cite),
5 (placeholders + stamps). FINDING-flagged, not auto-fixed: item 2 (absolute paths in pinned/data
headers). Unverifiable-with-reason: CITATION.cff (Phase 3, no `cffconvert`); the heavy m0b gate
(48-core-box, per m0b-pinning §7).
