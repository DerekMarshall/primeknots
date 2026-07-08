#!/usr/bin/env python3
"""Stage 6 LMFDB-independent referee: Karim Belabas's `cubic` enumerator
(Math. Comp. 66 (1997), 1213–1237) — the specialized cubic-field algorithm, a
separate enumeration code path from PARI's general nflist. This replaces the
API-blocked LMFDB twin with an auditable, offline, deterministic referee
(docs/notes/stage6-pinning.md R4).

For each sweep prime set S (all p ≡ 1 mod 4) whose completeness bound
Dmax = ∏ p² is feasible to fully enumerate, run `cubic Dmax`, then count cubic
fields K with: disc(K) NOT a perfect square (⇔ Galois group S3, non-cyclic) and
rad(disc K) ⊆ S. That count is the LMFDB-independent c; it must equal the PARI
nflist count. Sets whose Dmax exceeds the feasible threshold are recorded as
skipped (no silent truncation).

Cache: data/belabas/cubic_s3.txt. The `cubic` binary is built by
oracle/build_belabas.sh (fetches Belabas's sha256-pinned tarball, links libpari);
its path comes from $AT_BELABAS_CUBIC or the default build location.
"""
import math
import os
import subprocess
import sys
from datetime import date
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEST = ROOT / "data" / "belabas" / "cubic_s3.txt"
BIN = os.environ.get("AT_BELABAS_CUBIC", str(ROOT / "build" / "belabas" / "cubic"))
FEASIBLE_MAX = 10_000_000  # Dmax above this: too many cubic fields to enumerate

# Same fixed sweep as the PARI referee (single source of prime sets).
sys.path.insert(0, str(ROOT / "oracle"))
from compute_cubic_counts import SWEEP  # noqa: E402


def rad_subset(n, S):
    """True iff every prime factor of n lies in S (2 ∉ S ⇒ even n fails)."""
    for p in sorted(S):
        while n % p == 0:
            n //= p
    return n == 1


def is_square(n):
    r = math.isqrt(n)
    return r * r == n


def count_belabas(S):
    Dmax = 1
    for p in S:
        Dmax *= p * p
    if Dmax > FEASIBLE_MAX:
        return None, Dmax  # infeasible: skip (recorded, not truncated)
    env = dict(os.environ)
    for lib in ("/opt/homebrew/opt/pari/lib", "/opt/homebrew/opt/gmp/lib",
                "/usr/local/lib"):
        env["DYLD_LIBRARY_PATH"] = lib + ":" + env.get("DYLD_LIBRARY_PATH", "")
    r = subprocess.run([BIN, str(Dmax)], capture_output=True, text=True, env=env)
    if r.returncode != 0:
        print(f"cubic failed for {S}: {r.stderr}", file=sys.stderr)
        sys.exit(1)
    Sset = set(S)
    c = 0
    for line in r.stdout.splitlines():
        if ":" not in line or line.startswith("H"):
            continue
        try:
            d = int(line.split(":")[0])
        except ValueError:
            continue
        if d > 1 and not is_square(d) and rad_subset(d, Sset):  # S3 + rad ⊆ S
            c += 1
    return c, Dmax


def main() -> int:
    check = "--check" in sys.argv[1:]
    if not Path(BIN).exists():
        print(f"[SKIP] belabas_cubic: `cubic` binary not found at {BIN} "
              "(run oracle/build_belabas.sh).")
        return 77
    rows, n_feasible, n_skip = [], 0, 0
    for S in SWEEP:
        c, Dmax = count_belabas(S)
        if c is None:
            rows.append(f"{','.join(map(str, S))} | skip | {Dmax}")
            n_skip += 1
        else:
            rows.append(f"{','.join(map(str, S))} | {c} | {Dmax}")
            n_feasible += 1
            if not check:
                print(f"  S={S} c_belabas={c} (Dmax={Dmax})")
    header = (
        "# Stage 6 Belabas-cubic referee — LMFDB-independent S3 cubic counts\n"
        f"# generated: {date.today().isoformat()}\n"
        "# tool: Karim Belabas `cubic` v1.3 (Math. Comp. 66 (1997) 1213-1237)\n"
        "# method: cubic Dmax; count disc non-square (S3) with rad(disc) subset S\n"
        f"# bound_rule: |disc| <= prod p^2; feasible if Dmax <= {FEASIBLE_MAX} "
        f"({n_feasible} feasible, {n_skip} skipped as too large)\n"
        "# format: primes_csv | c_belabas (or 'skip') | disc_bound\n")
    body = "\n".join(rows) + "\n"
    if check:
        if not DEST.exists():
            print(f"[MISSING] {DEST}"); return 1
        cached = "".join(l for l in DEST.read_text().splitlines(keepends=True)
                         if not l.startswith("#"))
        if cached != body:
            print("MISMATCH: regenerated Belabas counts differ from cache"); return 1
        print(f"OK: {n_feasible} feasible Belabas counts match cache"); return 0
    DEST.parent.mkdir(parents=True, exist_ok=True)
    DEST.write_text(header + body)
    print(f"wrote {DEST} ({n_feasible} feasible, {n_skip} skipped)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
