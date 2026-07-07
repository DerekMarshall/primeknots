#!/usr/bin/env python3
"""validate-json (ARCHITECTURE.md §5): emit Stage 1 JSON, then validate every
emitted file against its schema in docs/schemas/.

Exit codes: 0 = all valid; 1 = a validation/consistency failure; 77 = SKIP
(jsonschema unavailable) — printed visibly, never a silent pass.
"""
import argparse
import json
import subprocess
import sys
from pathlib import Path

# Emitted file -> schema file. Extend as stages add emitters.
FILE_SCHEMAS = {
    "linking_matrix.json": "linking_matrix.schema.json",
    "linking_graph.json": "linking_graph.schema.json",
    "borromean.json": "borromean.schema.json",
    "classgroups.json": "classgroups.schema.json",
    "cs_partition.json": "cs_partition.schema.json",
}


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--at", required=True, help="path to the `at` binary")
    ap.add_argument("--schema-dir", required=True)
    ap.add_argument("--out", required=True, help="scratch dir for emitted JSON")
    ap.add_argument("--bound", default="500")
    args = ap.parse_args()

    try:
        import jsonschema
    except ImportError:
        print("[SKIP] validate_json: python 'jsonschema' not available; "
              "cannot validate emitted files against their schemas.")
        return 77

    out = Path(args.out)
    out.mkdir(parents=True, exist_ok=True)
    for stage in ("1", "2", "3", "4"):
        # keep validation fast: small bounds (schema-shape check, not full sweeps)
        b = "5000" if stage in ("3", "4") else args.bound
        r = subprocess.run([args.at, "emit", "--stage", stage, "--out", str(out),
                            "--bound", b], capture_output=True, text=True)
        if r.returncode != 0:
            print(f"FAIL: `at emit --stage {stage}` exited", r.returncode, "\n", r.stderr)
            return 1

    schema_dir = Path(args.schema_dir)
    failures = 0
    checked = 0
    for fname, sname in FILE_SCHEMAS.items():
        data_path = out / fname
        schema_path = schema_dir / sname
        if not data_path.exists():
            print(f"FAIL: emitter did not produce {fname}")
            failures += 1
            continue
        data = json.loads(data_path.read_text())
        schema = json.loads(schema_path.read_text())
        try:
            jsonschema.validate(instance=data, schema=schema)
            print(f"OK: {fname} valid against {sname}")
            checked += 1
        except jsonschema.ValidationError as e:
            print(f"FAIL: {fname} violates {sname}: {e.message} (at {list(e.path)})")
            failures += 1

    # Cross-field invariants the schema can't express.
    lm = json.loads((out / "linking_matrix.json").read_text())
    if not (lm["n"] == len(lm["primes"]) == len(lm["rows_base64"])):
        print("FAIL: linking_matrix n / primes / rows_base64 lengths disagree")
        failures += 1
    else:
        print(f"OK: linking_matrix n == len(primes) == len(rows_base64) == {lm['n']}")

    print(f"validate_json: {checked} files valid, {failures} failures")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
