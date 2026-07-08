#!/usr/bin/env python3
"""Stage 6 LMFDB referee. LMFDB's public API reliably serves only scalar-equality
queries (array/range operators 404 and repeated bad requests trip a reCAPTCHA), so
we query the nf_fields table by disc_abs for a fixed list of discriminants and
record each cubic field's Galois label, disc sign, and ramified primes. That lets
us (a) build an INDEPENDENT LMFDB count for the small sets whose full candidate-disc
set is enumerated ({13}: c=0, {13,61}: c=1), and (b) confirm every PARI-found cubic
of the larger sets is, per LMFDB, a genuine 3T2 totally-real field with matching
ramps. See docs/notes/stage6-pinning.md R4.

Cache: data/lmfdb/cubic_s3.txt (committed, per data/ precedent). Run:
    python3 oracle/fetch_lmfdb.py            # fetch (paced) + write cache
"""
import hashlib
import sys
import time
import urllib.parse
import urllib.request
from datetime import date
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEST = ROOT / "data" / "lmfdb" / "cubic_s3.txt"
API = "https://www.lmfdb.org/api/nf_fields/"


def candidates(primes):
    """All d = prod p^a, a in {0,1,2}, not all zero (rad(d) subset primes)."""
    ds = {1}
    for p in primes:
        ds = {d * p ** a for d in ds for a in (0, 1, 2)}
    ds.discard(1)
    return sorted(ds)


# Discriminants to query: full candidate sets of the independently-counted small
# sets, plus the distinct PARI-found discs of the larger sets (per-field confirm).
DISCS = sorted(set(
    candidates([13]) + candidates([13, 61]) + candidates([29, 53]) +
    [12325, 71825, 357425, 1416389, 2416193, 2723825, 60404825, 229]
))


def query_disc(d):
    url = API + "?" + urllib.parse.urlencode(
        {"degree": 3, "disc_abs": d, "_format": "json"})
    with urllib.request.urlopen(url, timeout=30) as r:
        raw = r.read()
    return raw


def main() -> int:
    import json
    lines, blob = [], b""
    for d in DISCS:
        raw = query_disc(d)
        blob += raw
        try:
            data = json.loads(raw).get("data", [])
        except Exception:
            print(f"[ABORT] non-JSON response at disc_abs={d} (reCAPTCHA/rate-limit?); "
                  "no cache written.", file=sys.stderr)
            return 1
        for f in data:
            if f.get("degree") != 3:
                continue
            ramps = ",".join(str(p) for p in f.get("ramps", []))
            lines.append(f"{d} | {f.get('galois_label')} | {f.get('disc_sign')} | {ramps}")
        time.sleep(2.5)  # pace to stay under the bot-challenge threshold

    sha = hashlib.sha256(blob).hexdigest()
    header = (
        "# Stage 6 LMFDB referee — nf_fields cubic records (scalar disc_abs queries)\n"
        f"# fetched: {date.today().isoformat()}\n"
        f"# api: {API}?degree=3&disc_abs=<D>&_format=json\n"
        f"# discs_queried: {len(DISCS)}  sha256(responses): {sha}\n"
        "# format: disc_abs | galois_label | disc_sign | ramps_csv (one line per field)\n")
    DEST.parent.mkdir(parents=True, exist_ok=True)
    DEST.write_text(header + "\n".join(lines) + "\n")
    print(f"wrote {DEST} ({len(lines)} field records from {len(DISCS)} disc queries)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
