# libm partial-diff — pinned spec (data-note §5, item-4)

Commit-before-run spec for the cross-platform float measurement referenced by
`docs/notes/data-note/data-note.md` §5 as `[PENDING item-4 diff]`. This spec commit is the
registration; no `LOOKS.md` entry is required (Derek, 2026-07-17). The measurement is
**descriptive**: it has no verdict clause and no tolerance. It reports two quantities and stops.

## Purpose and framing

The a_p Frobenius trace is a platform-independent integer, proven byte-identical laptop and FreeBSD
(m0b-pinning.md §7, line 237). The emitted density JSON is byte-portable across compilers because
its translation units are built with `-ffp-contract=off` (`src/CMakeLists.txt:163-173`:
`murm/ss_density.cpp`, `emit/emit_sawin_sutherland.cpp`, and the M2/M3 emitters), so it passes the
freshness byte-check on CI's GCC (data-note claim N2-13).

The raw per-curve partials are different. `murm/ss_empirical.cpp` and `murm/ss_empirical_m0b.cpp`
(`src/CMakeLists.txt:54-55`) are **not** in that `-ffp-contract=off` list, so they compile with the
compiler's default fma contraction. Their float content (the `u_mid·lnN/N` weighted sums) can
therefore differ in the low bits across toolchains. This measurement quantifies that difference on
one fixed curve set, and confirms that the integer a_p content underneath it is identical. It
substantiates the §5 sentence: the emitted density is byte-identical cross-platform, while the raw
partials carry a small, characterized float divergence that does not reach the emitted shape.

## Curve set (extensional)

The curve set is not drafted here. It is defined by the artifact: the curves present in the
**abandoned laptop 2¹⁷ partials checkpoint**. That file enumerates to **8640 curves** (derived, not
asserted; `grep -cvE '^#|^$'`). The committed FreeBSD 2¹⁷ run holds 9014 curves, so the 8640 are a
subset; the diff restricts to the intersection of curve keys present in both files, matched by
curve identity, not by line position. The 8640 are expected to be a subset of the 9014, so the
unmatched-key count is expected to be zero; any nonzero unmatched count is a reportable anomaly,
recorded and not smoothed.

## Step 0 — input manifest (provenance)

Both large inputs are gitignored (`*.ckpt`, and `data/m5/ap_cache_*.bin` for the a_p cache). Two
different treatments, by Derek's addendum (2026-07-17):

- The 8.8 MB laptop partials ckpt is **force-committed** via a scoped `.gitignore` exception
  (`!data/m5/ss_partials_x131072.txt.ckpt`, with a comment there). It is quantity 2's input and
  the only diff input not reproducible from source, so committing it keeps the float measurement
  reproducible.
- The 771 MB a_p cache stays **hash-only** (too large to commit; quantity 1 is a run-time
  comparison, not a repo-reproducible read).

Provenance for all inputs is pinned by hash below.

| Role | File (local, gitignored) | sha256 | bytes | curves |
|------|--------------------------|--------|-------|--------|
| Laptop partials (abandoned 2¹⁷ ckpt) | `data/m5/ss_partials_x131072.txt.ckpt` | `ff34a444ada791c118681014bde127540a0e921e64901684adbd233f5151d03c` | 8809432 | 8640 |
| Laptop a_p cache (2¹⁷ ckpt) | `data/m5/ap_cache_x131072.bin.ckpt` | `88f2481ff1ae3685d0e15f06ba8e82291b74564248ba4e3a1918b3fc6ff07699` | 771585467 | — |
| FreeBSD partials (committed, 2¹⁷ canonical) | `data/m5/ss_partials_x131072.txt` | `5f7a4106e3420884f43eb4f4a06f25c9de06352b08953ddf3490a12cdca4eaf2` | 9205871 | 9014 |

Both files carry `generator_hash b87ebd1e188e15dc2d3a1c2a54e5300dc3c7ff3d4adef3e77d5328b9ff526cde`
(identical statistic source). The partials ckpt header records `complete 0` (an abandoned run),
`threads 12`, and `n_curves 8640`. Its `ne_cache` field originally held a laptop absolute path
(`/Users/derekmarshall/…`) written at run time; the Layer-1 QA sweep (2026-07-18) relativized it to
`data/m5/ne_cache_x131072.txt` so the released artifact carries no local home path — data
byte-identical apart from that one comment, ckpt sha **re-pinned** `1255608e…` → `ff34a444…` (the
table above). Derek attests (2026-07-17) that these are the abandoned laptop 2¹⁷ run; the
`complete 0` flag, the 12-thread / 8640-curve header, and the pre-relativization absolute path
preserved in git history corroborate that attestation. The host compiler and flags are **not**
stamped anywhere in the artifact or the run log (checked), so they remain unknown (see Toolchains).

## Toolchains pinned

- **Laptop (the ckpt producer).** macOS (darwin 25.5.0), 12 threads (from the header). The
  compiler and flags that built the ckpt are **not stamped** in the artifact or the run log, so
  they are recorded as **unknown / attested**. The current laptop build uses `g++-16 (Homebrew GCC
  16.1.0)` and builds `murm/ss_empirical.cpp` with default fp contraction (not in the
  `-ffp-contract=off` list, `src/CMakeLists.txt:54-55` vs `163-173`) — the likely but unconfirmed
  producer. Because the compiler pair is not established, §5 attributes the measured divergence
  **generically** (fp contraction, libm, platform), not to a named compiler pair.
- **FreeBSD (the comparison side).** `FreeBSD clang 21.1.8`, FreeBSD 15.1-STABLE amd64 (recorded
  from the box). Same statistic source (generator_hash above). For quantity 1 (integer a_p) the fp
  path is irrelevant: a_p is an integer point-count, so cross-platform identity is the expectation.

## Measurement and report (no verdict, no tolerance)

Both quantities are computed over the shared 8640-curve set only.

1. **Integer a_p exact-match count.** Compare a clean laptop a_p cache against the FreeBSD a_p
   cache, byte-for-byte over the payload. **Provider, corrected (rule 5).** The APC1 `algo` field on
   both caches reads `charsum_referee(ap_fast; spot=ap_charsum)`: values computed by `ap_fast`,
   spot-checked against the frozen `ap_charsum` referee, the *same* algo on both sides. This is a
   same-provider, cross-platform integer comparison, not the cross-provider check an earlier draft
   of this line claimed. **Input, corrected (deviation, 2026-07-18).** The historical laptop a_p
   cache (`ap_cache_x131072.bin.ckpt`, sha256 `88f2481f…`) is a `complete=0` checkpoint that
   `read_ap_cache` refuses as partial, and its payload sha (`4d0c65bc…`) differs from FreeBSD's
   clean cache — an abandoned checkpoint is not a trustworthy full cache. So the laptop side is
   **regenerated clean** (`--X 131072 --threads 13`, `complete=1`, distinct output/checkpoint paths
   so the historical ckpt is untouched) and its int16 payload is compared to FreeBSD's
   (`aec1899b…`). a_p is provider-independent and a platform-independent integer, so an exact
   full-payload match is the expectation; any mismatch is a reportable finding (m0b-pinning §237).
   Report: payload int16 count compared, exact-match count, mismatch count. **Superseded
   2026-07-18** — the full regen was abandoned as a multi-day job; replaced by a tail-weighted
   sampled check (see "Quantity 1 — sampled fallback" below).
2. **Float partial-sum |Δ|.** Compare the laptop partials ckpt against the committed FreeBSD
   partials, per (curve, bin), over the shared curves and 40 bins. Report: max |Δ|, and the
   distribution of |Δ| (counts bucketed by magnitude, decade bins from 0 down through the smallest
   nonzero). The float part needs no fresh FreeBSD partials run; it uses the already-committed
   FreeBSD partials restricted to the shared curves.

The compute the run requires is the clean laptop a_p cache regeneration (quantity 1); quantity 2 is
a read-only diff of two existing files and is already complete (see Results). Results land in §5 as
a short factual paragraph and are echoed under Results below.

## Registration

Spec pinned before the run; Derek approved. Deviations recorded, not smoothed: (i) the provider is
`charsum_referee(ap_fast)` on both sides, not cross-provider — an earlier line was wrong, now fixed;
(ii) the historical laptop a_p cache is a `complete=0` checkpoint unsuitable as a full-cache input,
so the laptop side is regenerated clean (Derek's call, 2026-07-18); (iii) FreeBSD toolchain recorded
as clang 21.1.8 / FreeBSD 15.1-STABLE; (iv) the clean regen was abandoned as a multi-day job (~67h
floor, measured), so quantity 1 is now a tail-weighted sampled check (Derek's call, 2026-07-18),
its 7.3% checkpoint preserved and hashed. **Both quantities complete (2026-07-18):** quantity 2 max
|Δ| = 1.887 × 10⁻¹⁵ (ULP-scale float drift); quantity 1 = 79,268 / 79,268 sampled a_p byte-identical
cross-platform (0 mismatches).

## Quantity 1 — full regen abandoned for a sampled check (deviation, 2026-07-18)

The clean full-cache regeneration (Derek's call, 13 threads) was **abandoned**. Live monitoring
(`docs/notes/apcache_progress.py`, Σp-work-weighted) showed it decelerating as prime size grows
(per-8192-prime-block times 2195s, 6000s, 9450s): at 7.3% of the work in 4.9h it projected to a
**~67h floor, realistically 3-5 days** on the laptop — the QR table is size-`p` and thrashes cache
at large `p`, which the first estimate missed. The tracked job was stopped at **24576/293263 primes
(7.3%)**. Its checkpoint is **preserved, not deleted**:
`data/m5/ap_cache_x131072_laptop_clean.bin.ckpt` (complete=0, n_done=24576, sha256
`bb6e6bc99c3001050cef391dd7bb0b0fb8d061df8fc405e5bb50d4505d6d0490`), gitignored (hash-only), so the
full regen is resumable if ever wanted.

**Replaced by a sampled cross-platform integer check.** Mirrors the 2¹⁸ tail-weighted precedent
(`twin_m0b_bruteforce_x18_tailweighted`): deterministic, tail-weighted, no RNG and no seed.

- **Curve set.** The full 2¹⁷ family (9014 curves) the FreeBSD clean cache covers.
- **Prime tail-weighting.** Partition the 293263 good primes (`3<p≤N`, `p∤disc`) into four
  index-quartiles Q1..Q4 (smallest..largest). Select primes tail-weighted **3:5:7:10** (mirroring
  the x18 30/50/70/100 stratum ratio): Q1 300, Q2 500, Q3 700, Q4 1000 = **2500 primes**, taken at
  a fixed stride within each quartile.
- **Curves per prime.** For each selected prime `p`, the **40 highest-conductor** curves with
  `N≥p` and `p∤disc` (conductor tail-weighting, mirroring x18). About **10⁵ (curve,prime) pairs**
  (fewer only where <40 curves qualify at the largest primes).
- **Computation and comparison.** a_p for each sampled pair is computed by the frozen referee
  `ap_charsum` via a small deterministic sampler, run on **both** laptop (`g++-16`) and FreeBSD
  (`clang 21.1.8`); the two outputs are compared. Cross-check: laptop values are also compared to
  the FreeBSD clean cache (`ap_fast`), a cross-algorithm corroboration.
- **Report.** pairs compared, max prime in the sample, laptop-vs-FreeBSD exact-match count,
  mismatch count. No verdict, no tolerance.

**Standing (§5 framing).** This sampled match is **corroboration on this specific 2¹⁷ curve set**,
not the load-bearing integer-identity claim. The full-strength evidence that a_p is
platform-independent is the exact agreement of `ap_fast`/`ap_charsum`/Shanks–Mestre over the
**full** 2¹⁶ and 2¹⁷ grids (`twin_m0b_vs_charsum_x16`/`x17`, CLAIMS-E E3-2/E3-2b) plus the
byte-identical emitted density (N2-13). §5 cites those as primary and the sample as confirmation.

## Results

**Quantity 2 — float partial-sum |Δ| (complete, 2026-07-18).** Over the shared 8640 curves (laptop
ckpt is a subset of the FreeBSD 9014, 0 unmatched keys) times 40 bins = 345600 float values: 63803
bit-identical; **max |Δ| = 1.887 × 10⁻¹⁵** (at a value near −0.9785, relative 1.9 × 10⁻¹⁵); the |Δ|
distribution peaks at 10⁻¹⁷ to 10⁻¹⁶ and has no value above about 10⁻¹⁴. This is ULP-scale,
consistent with default fma contraction and libm rounding in the raw partials, while the emitted
density (contraction-off) is byte-identical (N2-13). Computed by `docs/notes/libm_float_diff.py`
(run from repo root) over `data/m5/ss_partials_x131072.txt.ckpt` and
`data/m5/ss_partials_x131072.txt`; the a_p cache payloads are parsed by `docs/notes/apc1_parse.py`.

**Quantity 1 — integer a_p match (sampled, complete 2026-07-18).** The deterministic tail-weighted
sample — 2500 primes (3:5:7:10 across index-quartiles, max p = 4,152,803) × up to 40
highest-conductor curves = **79,268 (curve,prime) pairs** — was computed by the frozen referee
`ap_charsum` on the laptop (`g++-16`, macOS, 10 threads) and on FreeBSD (`clang 21.1.8`, FreeBSD
15.1, 48 threads), via `at ap-sample`. The two outputs are **byte-identical**: sha256
`e7ba27d7bc982f378151653b537cde3af22c0d32a3e8b53f3d06b70a4b2600c2` on both sides, so **79,268 /
79,268 a_p match, 0 mismatches** — integer a_p identical cross-platform on this 2¹⁷ curve set. This
is cross-platform, same-referee (`ap_charsum` both sides); cross-algorithm identity
(`ap_fast`↔`ap_charsum`) is covered separately by the full-grid twins. The sample is regenerable
from the committed `at ap-sample` and `ne_cache_x131072.txt`, so the sha pins it (the 2 MB output is
not committed). Primary integer-identity evidence remains the full 2¹⁶/2¹⁷ grid twins +
byte-identical emit (framing above); this sample is corroboration on this curve set.
Reboot-proof copies of both outputs are kept off `/tmp`, at
`data/m5/ap_sample_x131072_{laptop,freebsd}.txt` (gitignored, sha `e7ba27d7…`).
