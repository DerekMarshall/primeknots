#!/usr/bin/env python3
"""Fetch pinned Cremona ecdata slices into data/cremona/ (M0).

The data files themselves are gitignored (large, third-party, reproducible);
only data/cremona/MANIFEST.json + README.md are committed. This mirrors the
Odlyzko precedent (oracle/fetch_odlyzko.py + data/odlyzko/MANIFEST.json).

Provenance (RESEARCH-M §2 / ARCHITECTURE-M §2): conductor, rank, root number,
bad-prime a_p and minimal model taken from ecdata are ORACLE. Good-prime a_p is
COMPUTED by src/ell (the point of M0); ecdata's aplist good-prime values serve
only as the anchor and as an independent referee.

The pin is a git COMMIT (releases carry no downloadable assets — the data lives
in versioned repo dirs), so per-file sha256 is an exact, reproducible pin.

Usage:
  fetch_ecdata.py            # fetch missing files, then validate all vs manifest
  fetch_ecdata.py --validate # validate only; exit 1 on any mismatch, 0 if clean,
                             #   77 (SKIP) if data files are absent (not fetched yet)
"""
import hashlib
import json
import subprocess
import sys
import urllib.request
from datetime import date
from pathlib import Path

# --- PIN (a change here is a deliberate data-version event; record it in the
#         M0 pinning log, docs/notes/m0-pinning.md) ---
ECDATA_TAG = "2026-04-22"
ECDATA_COMMIT = "25cec5ecfec8b9f016eb1631ac633194c2bed39f"
SOURCE_REPO = "https://github.com/JohnCremona/ecdata"
FORMAT_DOC = f"docs/file-format.txt @ {ECDATA_COMMIT}"
BASE = f"https://raw.githubusercontent.com/JohnCremona/ecdata/{ECDATA_COMMIT}"

# M0 slice: conductors < 10000 (contains the 11a1 anchor + ample sampling).
# M1 extends RANGES to the full N <= 500000 set through this same script.
RANGES = ["00000-09999"]
KINDS = ["allcurves", "aplist"]

ROOT = Path(__file__).resolve().parent.parent
DEST = ROOT / "data" / "cremona"
MANIFEST = DEST / "MANIFEST.json"


def rel_paths():
    return [(k, f"{k}/{k}.{r}", f"{k}.{r}") for k in KINDS for r in RANGES]


def sha256(p):
    h = hashlib.sha256()
    h.update(p.read_bytes())
    return h.hexdigest()


def load_manifest():
    return json.loads(MANIFEST.read_text()) if MANIFEST.exists() else None


def validate():
    m = load_manifest()
    if m is None:
        print("[SKIP] fetch_ecdata --validate: no MANIFEST.json (run without --validate first)")
        return 77
    bad = []
    missing = 0
    for entry in m["files"]:
        p = DEST / entry["file"]
        if not p.exists():
            missing += 1
            print(f"  MISSING {entry['file']}")
            continue
        got = sha256(p)
        if got != entry["sha256"]:
            bad.append(entry["file"])
            print(f"  MISMATCH {entry['file']}: {got} != {entry['sha256']}")
        else:
            print(f"  OK  {entry['file']}  sha256={got[:12]}…  ({entry['bytes']} bytes)")
    if missing == len(m["files"]):
        print("[SKIP] all ecdata files absent — not fetched yet")
        return 77
    if bad or missing:
        print(f"\nFAIL: {len(bad)} mismatch, {missing} missing vs manifest "
              f"(pin {ECDATA_TAG} {ECDATA_COMMIT[:12]})")
        return 1
    print(f"\nOK: all {len(m['files'])} files match manifest (pin {ECDATA_TAG})")
    return 0


def fetch():
    DEST.mkdir(parents=True, exist_ok=True)
    files = []
    changed = False
    for _kind, rel, name in rel_paths():
        out = DEST / name
        if not out.exists():
            url = f"{BASE}/{rel}"
            print(f"  fetch {rel}")
            req = urllib.request.Request(url, headers={"User-Agent": "primeknots-ecdata-fetch"})
            with urllib.request.urlopen(req, timeout=120) as r:
                out.write_bytes(r.read())
            changed = True
        else:
            print(f"  have  {name}")
        files.append({"file": name, "source": rel,
                      "sha256": sha256(out), "bytes": out.stat().st_size})

    existing = load_manifest()
    new = {
        "ecdata_tag": ECDATA_TAG,
        "ecdata_commit": ECDATA_COMMIT,
        "source_repo": SOURCE_REPO,
        "format_doc": FORMAT_DOC,
        "fetch_command": "oracle/fetch_ecdata.py",
        "fetch_date": str(date.today()),
        "provenance": "conductor/rank/root_number/bad-prime a_p/minimal model = oracle; "
                      "good-prime a_p = computed by src/ell (this data referees it)",
        "files": files,
    }
    # Only rewrite the (committed) manifest when file bytes actually changed —
    # keeps re-runs from churning fetch_date into a spurious diff.
    if existing is not None and existing.get("files") == files \
            and existing.get("ecdata_commit") == ECDATA_COMMIT:
        print("manifest unchanged")
    else:
        MANIFEST.write_text(json.dumps(new, indent=2) + "\n")
        print(f"wrote {MANIFEST.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    if "--validate" in sys.argv:
        sys.exit(validate())
    fetch()
    sys.exit(validate())
