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
| Laptop partials (abandoned 2¹⁷ ckpt) | `data/m5/ss_partials_x131072.txt.ckpt` | `1255608e9592805b7671501cd6dda52115b330af5debd9f8731e9716e06b782c` | 8809496 | 8640 |
| Laptop a_p cache (2¹⁷ ckpt) | `data/m5/ap_cache_x131072.bin.ckpt` | `88f2481ff1ae3685d0e15f06ba8e82291b74564248ba4e3a1918b3fc6ff07699` | 771585467 | — |
| FreeBSD partials (committed, 2¹⁷ canonical) | `data/m5/ss_partials_x131072.txt` | `5f7a4106e3420884f43eb4f4a06f25c9de06352b08953ddf3490a12cdca4eaf2` | 9205871 | 9014 |

Both files carry `generator_hash b87ebd1e188e15dc2d3a1c2a54e5300dc3c7ff3d4adef3e77d5328b9ff526cde`
(identical statistic source). The partials ckpt header records `complete 0` (an abandoned run),
`threads 12`, `n_curves 8640`, and an `ne_cache` path under `/Users/derekmarshall/...`, a laptop
absolute path written at run time — which corroborates laptop origin from the artifact itself.
Derek attests (2026-07-17) that these are the abandoned laptop 2¹⁷ run; the embedded path and the
`complete 0` flag corroborate that attestation. The host compiler and flags are **not** stamped
anywhere in the artifact or the run log (checked), so they remain unknown (see Toolchains).

## Toolchains pinned

- **Laptop (the ckpt producer).** macOS (darwin 25.5.0), 12 threads (from the header). The
  compiler and flags that built the ckpt are **not stamped** in the artifact or the run log, so
  they are recorded as **unknown / attested**. The current laptop build uses `g++-16 (Homebrew GCC
  16.1.0)` and builds `murm/ss_empirical.cpp` with default fp contraction (not in the
  `-ffp-contract=off` list, `src/CMakeLists.txt:54-55` vs `163-173`) — the likely but unconfirmed
  producer. Because the compiler pair is not established, §5 attributes the measured divergence
  **generically** (fp contraction, libm, platform), not to a named compiler pair.
- **FreeBSD (the comparison side).** clang; version captured at run time (`clang --version` into
  the results). Same statistic source (generator_hash above); fp contraction default likewise.

## Measurement and report (no verdict, no tolerance)

Both quantities are computed over the shared 8640-curve set only.

1. **Integer a_p exact-match count.** Compare the laptop a_p cache against a FreeBSD a_p cache over
   the shared curves. **Providers, stamped.** The laptop a_p cache is the frozen-referee reference
   cache (`ap_charsum`; `APC1` format, task M0b-2b) and covers the full 9014 curves; its partials
   ckpt used `ap_fast` (header). The FreeBSD side uses the canonical run's a_p (located if that
   cache was persisted) or a regeneration, with its provider stamped either way. If the two sides
   differ in provider (charsum reference vs `ap_fast`/`m0b`), quantity 1 is a **cross-provider and
   cross-platform** integer match, stronger than a same-provider check. a_p is provider-independent
   (`ap_fast`, `ap_shanks_mestre`, `ap_charsum` agree exactly), so the count is expected complete.
   The comparison restricts to the shared 8640 curves for consistency with quantity 2. Report:
   total (curve, prime) a_p values compared, count matching exactly, count of mismatches.
2. **Float partial-sum |Δ|.** Compare the laptop partials ckpt against the committed FreeBSD
   partials, per (curve, bin), over the shared curves and 40 bins. Report: max |Δ|, and the
   distribution of |Δ| (counts bucketed by magnitude, decade bins from 0 down through the smallest
   nonzero). The float part needs no fresh FreeBSD partials run; it uses the already-committed
   FreeBSD partials restricted to the shared curves.

The only compute the run requires is the FreeBSD a_p cache for the 8640 curves (for quantity 1);
quantity 2 is a read-only diff of two existing files. Results land in §5 as a short factual
paragraph and are echoed here under a "Results" heading appended after the run.

## Registration

Spec pinned before the FreeBSD run. Derek approves this spec, then the run is kicked. Deviations at
run time (toolchain surprise, curve-key misalignment, nonzero integer mismatch) are recorded here
and reported, not smoothed.
