# M0 pinning log — a_p machinery and data provenance

Two-phase gate for Stage M0 (RESEARCH-M §2, ARCHITECTURE-M §7). Convention-free
code and the frozen referee do not exist until the [PIN TO SOURCE] items below are
resolved against **sources**, not against RESEARCH-M.md (a paraphrase) or memory.

**Status.** Phase 0 (sources + data) complete — §0 below. Phase 1A (the sign,
model, bad-prime, and column-map pins) is **PENDING the comprehension-gate
sign-off** and is filled in before any `src/ell` code is written.

---

## §0 — Data provenance (Phase 0, complete)

### Cremona ecdata — the pin

- source: https://github.com/JohnCremona/ecdata (releases carry no downloadable
  assets; the data lives in versioned repo dirs, so the pin is a git commit and
  per-file sha256 is exact).
- tag **`2026-04-22`** = commit **`25cec5ecfec8b9f016eb1631ac633194c2bed39f`** (lightweight tag).
- format spec cited below is `docs/file-format.txt` at that commit.
- fetch command: `python3 oracle/fetch_ecdata.py` · fetch date: 2026-07-09.
- checksum record (also in `data/cremona/MANIFEST.json`, validated by the loader):

  | file | sha256 | bytes |
  |---|---|---|
  | `allcurves.00000-09999` | `259f3846329395b371e8079c77a6f1097adaebc98a054974573e241416efa968` | 2146401 |
  | `aplist.00000-09999` | `a1cc11fb2c53b58bcf03600cd67d8d9133a14381f1df9dac7b096c20699eca65` | 3809559 |

M0 fetches the conductor-`< 10000` slice (contains the `11a1` anchor plus ample
sampling); M1 extends `RANGES` in the fetch script to the full N ≤ 500,000 set.

### ecdata file formats (quoted from `docs/file-format.txt` @ pinned commit)

- **allcurves**: `ID AI R T` — one line per curve. `ID = N CLASS NUM`; `AI` =
  "Reduced minimal Weierstrass coefficients in format `[a1,a2,a3,a4,a6]`"; `R` =
  rank; `T` = torsion order. Example: `11 a 1 [0,-1,1,-10,-20] 0 5`.
- **aplist**: `N CLASSCODE AP25 BQ` — one line per **isogeny class**. Per the doc:
  > "AP: `a2 a3 a5 a7 a11 (etc.)` where each ap is the p'th eigenvalue of the
  > newform (integers, separated by whitespace). When p divides N this is the W_p
  > eigenvalue, shown just as `+` or `-`; else the T_p eigenvalue."
  > "AP25: AP for all primes<100 (the first 25 primes)."

  So at **good** p the entry is the T_p eigenvalue = a_p (trace of Frobenius); at
  **bad** p | N it is the Atkin–Lehner W_p sign (`+`/`-`), *not* a_p. This is the
  source for the M0 anchor's good-prime values, and it correctly walls off the
  deferred bad-prime handling.

### Papers (fetched by `oracle/fetch_papers_m.py`; title-page verified; gitignored)

All five arXiv PDFs fetched and title-page verified (manifest:
`docs/papers/MANIFEST-M.json`): [HLOP22] 2204.10140, [LOP23] 2307.00256,
[Z25] 2310.07681, [SS25] 2504.12295, [C24] 2408.12723. Note [C24]'s true title is
"Murmurations and ratios conjecture**s**" (plural), vs RESEARCH-M.md's singular —
arXiv is authoritative. The Sarnak letter [Sar23] is **framing only** (non-normative
for M0); its IAS landing page is Cloudflare-guarded (403 to automated tools) and no
stable direct-PDF path is published, so it is recorded SKIP-with-manual-instructions,
not fetched. None of M0's code depends on it.

---

## §1 — a_p sign and normalization  [PIN TO SOURCE — PENDING Phase 1A]

The off-by-one (point at infinity) and sign errors are silent and global. To be
anchored against verbatim-quoted good-prime a_p for `11a1` from Cremona's aplist
(and cross-checked at LMFDB), with the exact convention `a_p = p + 1 − #E(𝔽_p)`
stated and the referee's Legendre-sum sign derived, not assumed.

## §2 — Model reduction and invariance  [PIN TO SOURCE — PENDING Phase 1A]

long → short Weierstrass validity for p > 3; a_p as an isomorphism invariant at
good p; the `invariance_weierstrass_model` design.

## §3 — Bad primes  [PIN TO SOURCE — PENDING Phase 1A]

HLOP §2's convention at p | N, quoted verbatim; those values are oracle-provenance
from ecdata. **Tate's algorithm at p = 2, 3 is DEFERRED to M0b (its own gate) and
is out of M0 scope.**

## §4 — ecdata column map  [PENDING Phase 1A]

Every consumed column with its provenance label (rank / root number / conductor /
bad-prime a_p = oracle; good-prime a_p = computed).
