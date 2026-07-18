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
curve identity, not by line position.

## Step 0 — input manifest (provenance)

The two large inputs are gitignored (`/docs/... ` n/a; `data/m5/*.ckpt` and the 771 MB a_p cache
under the repo's `.gitignore`) and are not force-committed. Their provenance is pinned by hash
here. This manifest is the committed record of the diff inputs.

| Role | File (local, gitignored) | sha256 | bytes | curves |
|------|--------------------------|--------|-------|--------|
| Laptop partials (abandoned 2¹⁷ ckpt) | `data/m5/ss_partials_x131072.txt.ckpt` | `1255608e9592805b7671501cd6dda52115b330af5debd9f8731e9716e06b782c` | 8809496 | 8640 |
| Laptop a_p cache (2¹⁷ ckpt) | `data/m5/ap_cache_x131072.bin.ckpt` | `88f2481ff1ae3685d0e15f06ba8e82291b74564248ba4e3a1918b3fc6ff07699` | 771585467 | — |
| FreeBSD partials (committed, 2¹⁷ canonical) | `data/m5/ss_partials_x131072.txt` | `5f7a4106e3420884f43eb4f4a06f25c9de06352b08953ddf3490a12cdca4eaf2` | 9205871 | 9014 |

Both files carry `generator_hash b87ebd1e188e15dc2d3a1c2a54e5300dc3c7ff3d4adef3e77d5328b9ff526cde`
(identical statistic source). **Attestation (Derek, 2026-07-17):** the two `.ckpt` files are the
abandoned laptop 2¹⁷ run. The file headers do not stamp the host, so the laptop provenance is
Derek's attestation, not a property read from the artifact; recorded as such (rule 5).

## Toolchains pinned

- **Laptop (the ckpt producer).** `g++-16 (Homebrew GCC 16.1.0)`, macOS (darwin 25.5.0).
  `murm/ss_empirical.cpp` compiled with **default fp contraction** (not in the `-ffp-contract=off`
  list). Recorded from the current build config; if the Jul-15 build that produced the ckpt used a
  different flag set it will be re-derived and the discrepancy recorded (rule 5).
- **FreeBSD (the comparison side).** clang, version recorded from the run environment at run time
  (`clang --version` captured into the results). Same statistic source (generator_hash above),
  fp contraction default likewise.

## Measurement and report (no verdict, no tolerance)

Both quantities are computed over the shared 8640-curve set only.

1. **Integer a_p exact-match count.** Compare the laptop a_p cache against a FreeBSD a_p cache for
   the same curves (the FreeBSD side is produced or located by the approved run). a_p is provider
   independent (`ap_fast`, `ap_shanks_mestre`, `ap_charsum` all agree exactly), so the provider
   used on each side is immaterial to the integer values. Report: total (curve, prime) a_p values
   compared, count matching exactly, count of mismatches.
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
