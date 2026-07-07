#!/usr/bin/env python3
"""Fetch Odlyzko's table of the first 100000 nontrivial zeta zeros into
data/odlyzko/ (ARCHITECTURE.md §4: recorded query, date, SHA-256; tests read
the local cache only and SKIP visibly when it is absent).

The raw table (~1.8 MB) is gitignored; this script + the committed
data/odlyzko/MANIFEST.json reproduce and verify it. Run:

    python3 oracle/fetch_odlyzko.py            # fetch if missing, verify checksum
    python3 oracle/fetch_odlyzko.py --verify   # verify only, never fetch

Odlyzko [O]: A. Odlyzko, tables of zeros of the Riemann zeta function.
The file `zeros1` lists imaginary parts gamma_n (n=1..100000) of rho=1/2+i*gamma
to 9 decimal places, one per line, ascending. This is the Stage 5 accuracy
oracle; our Riemann-Siegel zeros are the verified twin.
"""
import argparse
import hashlib
import json
import sys
import urllib.request
from datetime import date
from pathlib import Path

URL = "https://www-users.cse.umn.edu/~odlyzko/zeta_tables/zeros1"
HERE = Path(__file__).resolve().parent.parent  # repo root
DEST = HERE / "data" / "odlyzko" / "zeros1"
MANIFEST = HERE / "data" / "odlyzko" / "MANIFEST.json"


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    h.update(path.read_bytes())
    return h.hexdigest()


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--verify", action="store_true",
                    help="verify local file against MANIFEST; never fetch")
    args = ap.parse_args()

    manifest = json.loads(MANIFEST.read_text()) if MANIFEST.exists() else None

    if not DEST.exists():
        if args.verify:
            print(f"[MISSING] {DEST} not present; run without --verify to fetch")
            return 1
        print(f"fetching {URL} -> {DEST}")
        DEST.parent.mkdir(parents=True, exist_ok=True)
        urllib.request.urlretrieve(URL, DEST)

    digest = sha256(DEST)
    n_lines = sum(1 for _ in DEST.open())
    print(f"{DEST}: {DEST.stat().st_size} bytes, {n_lines} zeros, sha256={digest}")

    if manifest is None:
        MANIFEST.write_text(json.dumps({
            "source": "Odlyzko [O], zeros of the Riemann zeta function",
            "url": URL,
            "fetched": date.today().isoformat(),
            "sha256": digest,
            "n_zeros": n_lines,
            "precision_dp": 9,
            "format": "one imaginary part gamma_n per line, ascending, n=1..N",
        }, indent=2) + "\n")
        print(f"wrote {MANIFEST}")
        return 0

    if digest != manifest["sha256"]:
        print(f"CHECKSUM MISMATCH: got {digest}, manifest {manifest['sha256']}")
        return 1
    print(f"checksum OK against MANIFEST ({manifest['fetched']})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
