#!/usr/bin/env python3
"""Stage 6 PARI referee driver. Runs oracle/gp/cubic_count.gp over the fixed
sweep of prime sets S (all primes ≡ 1 mod 4) and caches, for each S, the count c
of non-Galois (S3) cubic iso classes with rad(disc) ⊆ S, the discriminants, and
whether all are totally real (docs/notes/stage6-pinning.md R3/R4). Output:
data/cubic/s3_counts.json — the PARI referee cache; tests read it and an oracle
test regenerates it to confirm (SKIP if gp absent).

    python3 oracle/compute_cubic_counts.py            # regenerate the cache
    python3 oracle/compute_cubic_counts.py --check     # regenerate, diff vs cache
"""
import argparse
import shutil
import subprocess
import sys
from datetime import date
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent
LIB = HERE / "gp" / "cubic_count.gp"
DEST = ROOT / "data" / "cubic" / "s3_counts.txt"

# Fixed, reproducible sweep. Primes ≡ 1 mod 4; those ≡ 1 mod 12 contribute to k
# (the cyclic-cubic term). Strata: t=1..4; k=0 sets and k>0 sets; c=0 and c≥2.
SWEEP = [
    [5], [13], [17], [29], [37], [61],
    [5, 17], [5, 13], [13, 61], [29, 53], [13, 37], [5, 29],
    [5, 13, 17], [5, 17, 29], [13, 37, 61],
    [5, 13, 17, 29],
]


def run_gp() -> list:
    lib = LIB.read_text()
    script = lib + "\n" + "\n".join(f"runS({S})" for S in SWEEP) + "\n"
    # nbthreads=1: nflist's parallel discriminant-list workers intermittently
    # deadlock under a piped-subprocess stdin, hanging gp with the work already
    # done. Serial is a few seconds slower but reliable.
    r = subprocess.run(
        ["gp", "-q", "--default", "parisizemax=4G", "--default", "nbthreads=1"],
        input=script, capture_output=True, text=True)
    if r.returncode != 0:
        print("gp failed:\n", r.stderr, file=sys.stderr)
        sys.exit(1)
    out = []
    for line in r.stdout.splitlines():
        if not line.startswith("RESULT "):
            continue
        _, _, rest = line.partition("RESULT ")
        pstr, dmax, c, c_bydisc, allreal, dstr = [x.strip() for x in rest.split("|")]
        # two independent enumeration strategies must agree (Rule 1).
        if int(c) != int(c_bydisc):
            print(f"MISMATCH for {pstr}: resolvent c={c} vs by-disc c={c_bydisc}",
                  file=sys.stderr)
            sys.exit(1)
        primes = [int(x) for x in pstr.split(",") if x]
        discs = [int(x) for x in dstr.split(",") if x]
        out.append({
            "primes": primes,
            "disc_bound": int(dmax),
            "c": int(c),
            "all_totally_real": bool(int(allreal)),
            "discs": discs,
        })
    return out


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true", help="diff regenerated vs cached")
    args = ap.parse_args()

    if shutil.which("gp") is None:
        print("[SKIP] compute_cubic_counts: PARI/GP `gp` not found on PATH.")
        return 77  # clean SKIP (oracle absent), never a silent pass

    instances = run_gp()
    if len(instances) != len(SWEEP):
        print(f"expected {len(SWEEP)} results, got {len(instances)}", file=sys.stderr)
        return 1

    def body(insts) -> str:
        lines = []
        for it in insts:
            lines.append("{} | {} | {} | {} | {}".format(
                ",".join(map(str, it["primes"])), it["disc_bound"], it["c"],
                1 if it["all_totally_real"] else 0,
                ",".join(map(str, it["discs"]))))
        return "\n".join(lines) + "\n"

    header = (
        "# Stage 6 PARI referee cache — non-Galois (S3) cubic iso-class counts\n"
        f"# generated: {date.today().isoformat()}\n"
        "# tool: PARI/GP nflist (resolvent enumeration) + polgalois reclassification\n"
        "# bound_rule: |disc| <= prod_{p in S} p^2 (tame v_p(disc) <= 2)\n"
        "# c = iso classes of S3 cubics with rad(disc) subset S; all totally real (R3)\n"
        "# format: primes_csv | disc_bound | c | all_real(0/1) | discs_csv\n")

    if args.check:
        if not DEST.exists():
            print(f"[MISSING] {DEST}"); return 1
        cached_body = "".join(l for l in DEST.read_text().splitlines(keepends=True)
                              if not l.startswith("#"))
        if cached_body != body(instances):
            print("MISMATCH: regenerated counts differ from cache"); return 1
        print(f"OK: {len(instances)} instances match cache"); return 0

    DEST.parent.mkdir(parents=True, exist_ok=True)
    DEST.write_text(header + body(instances))
    print(f"wrote {DEST} ({len(instances)} instances)")
    for it in instances:
        print(f"  S={it['primes']} c={it['c']} real={it['all_totally_real']} discs={it['discs']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
