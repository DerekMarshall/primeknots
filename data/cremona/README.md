# data/cremona/ — Cremona ecdata (M-stage primary dataset)

Elliptic curves E/ℚ (Cremona's `ecdata`, all E/ℚ with N ≤ 500,000). The data
files are **not committed** (large, third-party, exactly reproducible); only this
README and `MANIFEST.json` (the checksum pin) are. Mirrors the Odlyzko precedent.

## Fetch / verify

```bash
python3 oracle/fetch_ecdata.py            # fetch missing files, validate vs manifest
python3 oracle/fetch_ecdata.py --validate # validate only (exit 1 mismatch, 77 if absent)
```

## The pin

`ecdata` releases carry no downloadable assets — the data lives in versioned repo
directories — so the pin is a git **commit**, and each file's sha256 in
`MANIFEST.json` is an exact, reproducible check. A change to the pin (tag/commit)
is a deliberate data-version event and must be recorded in `docs/notes/m0-pinning.md`.

- source: https://github.com/JohnCremona/ecdata
- tag `2026-04-22`, commit `25cec5ecfec8b9f016eb1631ac633194c2bed39f`
- format spec: `docs/file-format.txt` at that commit

## M0 slice and file formats

M0 fetches the conductor-`< 10000` slice (contains the `11a1` anchor plus ample
sampling); M1 extends `RANGES` in the fetch script to the full N ≤ 500,000 set.

- `allcurves.00000-09999` — one line per curve: `N CLASS NUM [a1,a2,a3,a4,a6] RANK TORSION`.
  Example: `11 a 1 [0,-1,1,-10,-20] 0 5`.
- `aplist.00000-09999` — one line per **isogeny class** (a_p is an isogeny
  invariant at good p): `N CLASS AP25 [BQ]`, where AP25 is a_p for the first 25
  primes (all p < 100). Per the ecdata format doc, each entry is the **T_p
  eigenvalue (= a_p)** at good p, or the **W_p (Atkin–Lehner) eigenvalue** shown
  as `+`/`-` when p | N. Example (`11 a`): `-2 -1 1 -2 - 4 -2 0 -1 …` (a₂=−2,
  a₃=−1, a₅=1, a₇=−2, then `-` at the bad prime 11, a₁₃=4, …).

## Provenance (RESEARCH-M §2)

- **oracle** (consumed as given): conductor, rank, root number, bad-prime a_p,
  minimal model.
- **computed** (the point of M0): good-prime a_p, by `src/ell`. ecdata's aplist
  good-prime values are used only as the `anchor_ap_11a1` source and as an
  independent referee — never as the computed answer.
