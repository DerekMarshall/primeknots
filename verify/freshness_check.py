#!/usr/bin/env python3
"""Freshness check (ctest label `freshness`). The committed viz/data snapshot is
what GitHub Pages serves; this asserts it is byte-identical to what `at emit`
produces at HEAD, so an emitter can never drift from the published data while the
suite stays green (ARCHITECTURE §5).

Emit is deterministic (a tested property — invariance_build_parallel_vs_serial),
so byte-identical is the standard. The ONE legitimately nondeterministic field is
`generated_by` (a runtime `git describe`); it is normalized structurally — checked
present + well-formed, then blanked on both sides before the byte compare. No
other field is excused; a payload diff is a failure, not something to fuzz over.

Parameters are read from each committed file's own `params` block, so the check
verifies the snapshot against emit-at-the-snapshot's-declared-parameters.

Exit 0 = fresh; 1 = drift (prints which files + first differing path); 77 = SKIP
(the `at` binary is missing).
"""
import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SNAP = ROOT / "viz" / "data"
AT = Path(sys.argv[sys.argv.index("--at") + 1]) if "--at" in sys.argv \
    else ROOT / "build" / "bin" / "at"

GB_RE = re.compile(r'("generated_by"\s*:\s*")([^"]*)(")')


def load(p):
    return json.loads(p.read_text())


def emit_args():
    """Reconstruct each stage's emit command from the committed params blocks."""
    lm = load(SNAP / "linking_matrix.json")["params"]
    lg = load(SNAP / "linking_graph.json")["params"]
    bo = load(SNAP / "borromean.json")["params"]
    cg = load(SNAP / "classgroups.json")["params"]
    cs = load(SNAP / "cs_partition.json")["params"]
    z = load(SNAP / "zeros.json")["params"]
    cache = str(ROOT / "data" / "cubic" / "s3_counts.txt")
    return {
        "1": (["--bound", str(lm["prime_bound"]), "--graph-nodes", str(lg["graph_nodes"])],
              ["linking_matrix.json", "linking_graph.json"]),
        "2": (["--bound", str(bo["prime_bound"])], ["borromean.json"]),
        "3": (["--bound", str(cg["disc_bound"])], ["classgroups.json"]),
        "4": (["--bound", str(cs["disc_bound"])], ["cs_partition.json"]),
        "5": (["--bound", str(int(z["t_max"]))],
              ["zeros.json", "psi_reconstruction.json", "dyn_zeta.json"]),
        "6": (["--cubic-cache", cache], ["dw_s3.json"]),
    }


def norm(text):
    """Blank generated_by's value; return (normalized_text, generated_by_value)."""
    m = GB_RE.search(text)
    val = m.group(2) if m else None
    return GB_RE.sub(r'\1<normalized>\3', text), val


def first_diff(a_path, b_text):
    """Cheap: first JSON path where committed vs fresh differ (generated_by aside)."""
    try:
        A = json.loads(a_path.read_text()); B = json.loads(b_text)
    except Exception:
        return "(unparseable JSON)"
    def walk(x, y, path):
        if type(x) is not type(y):
            return f"{path or '.'}: type {type(x).__name__}≠{type(y).__name__}"
        if isinstance(x, dict):
            for k in sorted(set(x) | set(y)):
                if k == "generated_by":
                    continue
                if k not in x or k not in y:
                    return f"{path}.{k}: present on one side only"
                r = walk(x[k], y[k], f"{path}.{k}")
                if r:
                    return r
        elif isinstance(x, list):
            if len(x) != len(y):
                return f"{path}: length {len(x)}≠{len(y)}"
            for i, (xi, yi) in enumerate(zip(x, y)):
                r = walk(xi, yi, f"{path}[{i}]")
                if r:
                    return r
        elif x != y:
            return f"{path}: {x!r} ≠ {y!r}"
        return None
    return walk(A, B, "") or "(differs only in bytes/whitespace)"


def main():
    if not AT.exists():
        print(f"[SKIP] freshness_check: `at` binary not found at {AT}")
        return 77
    tmp = Path(tempfile.mkdtemp(prefix="freshness_"))
    stages = emit_args()
    drift = []
    for stage, (args, files) in stages.items():
        r = subprocess.run([str(AT), "emit", "--stage", stage, "--out", str(tmp)] + args,
                           capture_output=True, text=True)
        if r.returncode != 0:
            print(f"FAIL: `at emit --stage {stage}` exited {r.returncode}\n{r.stderr}")
            return 1
        for fn in files:
            committed = SNAP / fn
            fresh = tmp / fn
            if not committed.exists():
                drift.append((fn, "not in committed snapshot")); continue
            fresh_text = fresh.read_text()
            cnorm, cval = norm(committed.read_text())
            fnorm, fval = norm(fresh_text)
            # generated_by: structural check on the FRESH emit (present, non-empty str)
            if not fval:
                drift.append((fn, "fresh emit missing/empty generated_by")); continue
            if cnorm != fnorm:
                drift.append((fn, first_diff(committed, fresh_text)))
            else:
                print(f"  OK  {fn}  (generated_by committed={cval!r} fresh={fval!r}, normalized)")

    if drift:
        print("\nFAIL: committed viz/data snapshot has drifted from `at emit` at HEAD:")
        for fn, why in drift:
            print(f"  {fn}: {why}")
        print("\nFix: re-emit the affected stage(s) and commit — never hand-edit the JSON.")
        return 1
    print(f"\nOK: all {sum(len(f) for _, f in stages.values())} snapshot files are "
          "byte-identical to `at emit` at HEAD (generated_by normalized)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
