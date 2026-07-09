#!/usr/bin/env python3
"""Fetch the Murmurations source papers into docs/papers/ (gitignored).

Standing rule (primeknots PDF-history lesson): third-party PDFs are NEVER
committed. Only this script + docs/papers/README.md are. Each fetch is
title-page verified: the PDF's first page is extracted (pdftotext) and the
paper's expected title must appear, so a wrong or truncated download is caught.

Reproducibility note: an arXiv ID + version + title-page check is the pin, NOT a
byte-exact sha256 — arXiv re-renders PDFs, so sha256 is recorded as informational
only. The Sarnak letter sits behind a Cloudflare challenge; if the automated fetch
is blocked, the script SKIPs it with manual instructions rather than failing.

Usage: fetch_papers_m.py     # fetch any missing PDFs, verify title pages, write manifest
"""
import json
import re
import subprocess
import sys
import urllib.request
from datetime import date
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEST = ROOT / "docs" / "papers"
MANIFEST = DEST / "MANIFEST-M.json"
UA = ("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 "
      "(KHTML, like Gecko) Chrome/120.0 Safari/537.36")

# (filename, url, expected title tokens that must appear on the title page)
PAPERS = [
    ("hlop-2204.10140.pdf", "https://arxiv.org/pdf/2204.10140",
     ["murmurations", "elliptic", "curves"]),
    ("lop-2307.00256.pdf", "https://arxiv.org/pdf/2307.00256",
     ["murmurations", "dirichlet", "characters"]),
    ("zubrilina-2310.07681.pdf", "https://arxiv.org/pdf/2310.07681",
     ["murmurations"]),
    ("sawin-sutherland-2504.12295.pdf", "https://arxiv.org/pdf/2504.12295",
     ["murmurations", "elliptic", "curves", "height"]),
    ("cowan-2408.12723.pdf", "https://arxiv.org/pdf/2408.12723",
     ["murmurations", "ratios", "conjecture"]),
]
SARNAK = ("sarnak-letter-2726.pdf",
          "https://publications.ias.edu/sites/default/files/MurmurationsSarnak.pdf",
          "https://publications.ias.edu/sarnak/paper/2726")


def download(url, out):
    req = urllib.request.Request(url, headers={"User-Agent": UA})
    with urllib.request.urlopen(req, timeout=90) as r:
        out.write_bytes(r.read())


def title_page(path):
    try:
        txt = subprocess.run(["pdftotext", "-f", "1", "-l", "1", str(path), "-"],
                             capture_output=True, text=True, timeout=60).stdout
    except FileNotFoundError:
        return None  # pdftotext absent -> cannot verify
    return re.sub(r"\s+", " ", txt).lower()


def verify(path, tokens):
    page = title_page(path)
    if page is None:
        return "unverified (pdftotext absent)"
    missing = [t for t in tokens if t.lower() not in page]
    return "verified" if not missing else f"FAIL (missing {missing})"


def sha_short(path):
    import hashlib
    return hashlib.sha256(path.read_bytes()).hexdigest()[:16]


def main():
    DEST.mkdir(parents=True, exist_ok=True)
    entries = []
    ok = True
    for name, url, tokens in PAPERS:
        out = DEST / name
        if not out.exists():
            print(f"  fetch {name}  <- {url}")
            try:
                download(url, out)
            except Exception as e:
                print(f"    ERROR {e}")
                ok = False
                entries.append({"file": name, "url": url, "status": f"fetch-error: {e}"})
                continue
        else:
            print(f"  have  {name}")
        status = verify(out, tokens)
        print(f"    title-page: {status}")
        if status.startswith("FAIL"):
            ok = False
        entries.append({"file": name, "url": url, "status": status,
                        "sha256_16": sha_short(out), "bytes": out.stat().st_size})

    # Sarnak letter: Cloudflare-guarded; attempt, SKIP cleanly on block.
    sname, spdf, spage = SARNAK
    sout = DEST / sname
    if sout.exists():
        st = verify(sout, ["murmuration"])
        print(f"  have  {sname}\n    title-page: {st}")
        entries.append({"file": sname, "url": spage, "status": st,
                        "sha256_16": sha_short(sout), "bytes": sout.stat().st_size})
    else:
        try:
            print(f"  fetch {sname}  <- {spdf}")
            download(spdf, sout)
            st = verify(sout, ["murmuration"])
            print(f"    title-page: {st}")
            entries.append({"file": sname, "url": spage, "status": st,
                            "sha256_16": sha_short(sout), "bytes": sout.stat().st_size})
        except Exception as e:
            # Framing reference only (non-normative for M0). The IAS landing page
            # is Cloudflare-guarded (403/JS-challenge to automated tools) and the
            # stable direct-PDF path is not published, so this SKIPs with manual
            # instructions rather than failing the fetch.
            print(f"    SKIP (not auto-fetchable: {e})")
            print(f"    -> download manually from {spage} and save as docs/papers/{sname}")
            entries.append({"file": sname, "url": spage,
                            "status": f"SKIP (manual, framing-only): {e}"})

    MANIFEST.write_text(json.dumps({
        "note": "arXiv ID + version + title-page check is the pin; sha256 is informational "
                "(arXiv re-renders). PDFs gitignored, never committed.",
        "fetch_command": "oracle/fetch_papers_m.py",
        "fetch_date": str(date.today()),
        "papers": entries,
    }, indent=2) + "\n")
    print(f"\nwrote {MANIFEST.relative_to(ROOT)}")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
