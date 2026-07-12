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
    "zeros.json": "zeros.schema.json",
    "psi_reconstruction.json": "psi_reconstruction.schema.json",
    "dyn_zeta.json": "dyn_zeta.schema.json",
    "dw_s3.json": "dw_s3.schema.json",
    "dirichlet_murmuration.json": "dirichlet_murmuration.schema.json",
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
    for stage in ("1", "2", "3", "4", "5", "6", "m2"):
        # keep validation fast: small bounds (schema-shape check, not full sweeps).
        # Stage 5's bound is the zero-search height t_max — keep it small here.
        # Stage 6 reads its referee cache (no bound); M2 takes no parameters
        # (zero external data — from scratch).
        b = {"3": "5000", "4": "5000", "5": "120"}.get(stage, args.bound)
        cmd = [args.at, "emit", "--stage", stage, "--out", str(out)]
        if stage == "6":
            # absolute cache path (repo root = this script's parent's parent)
            cache = Path(__file__).resolve().parent.parent / "data/cubic/s3_counts.txt"
            cmd += ["--cubic-cache", str(cache)]
        elif stage == "m2":
            pass  # no parameters
        else:
            cmd += ["--bound", b]
        r = subprocess.run(cmd, capture_output=True, text=True)
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

    # M1 (murmurations): its fresh emit needs the gitignored ecdata, so schema-check
    # the COMMITTED snapshot (present in CI) rather than re-emitting.
    m1_file = Path(__file__).resolve().parent.parent / "viz/data/murmuration_curve.json"
    if m1_file.exists():
        m1_schema = schema_dir / "murmuration_curve.schema.json"
        try:
            jsonschema.validate(instance=json.loads(m1_file.read_text()),
                                schema=json.loads(m1_schema.read_text()))
            print("OK: (committed) murmuration_curve.json valid against murmuration_curve.schema.json")
            checked += 1
        except jsonschema.ValidationError as e:
            print(f"FAIL: murmuration_curve.json violates schema: {e.message} (at {list(e.path)})")
            failures += 1

    # M3 (Zubrilina): repo-reproducible but its emit builds a ~3.4e7 Hurwitz sieve (a
    # few seconds), so schema-check the COMMITTED snapshot here and leave the byte-for-
    # byte re-emit to the freshness suite (which guarantees committed == emit at HEAD).
    m3_file = Path(__file__).resolve().parent.parent / "viz/data/zubrilina_murmuration.json"
    if m3_file.exists():
        m3_schema = schema_dir / "zubrilina_murmuration.schema.json"
        try:
            jsonschema.validate(instance=json.loads(m3_file.read_text()),
                                schema=json.loads(m3_schema.read_text()))
            print("OK: (committed) zubrilina_murmuration.json valid against zubrilina_murmuration.schema.json")
            checked += 1
        except jsonschema.ValidationError as e:
            print(f"FAIL: zubrilina_murmuration.json violates schema: {e.message} (at {list(e.path)})")
            failures += 1

    # M4 (Sawin–Sutherland): emit reads the committed COMPUTED run and a schema-checkable
    # snapshot is committed; validate that snapshot (same pattern as M1/M3).
    m4_file = Path(__file__).resolve().parent.parent / "viz/data/sawin_sutherland_murmuration.json"
    if m4_file.exists():
        m4_schema = schema_dir / "sawin_sutherland_murmuration.schema.json"
        try:
            jsonschema.validate(instance=json.loads(m4_file.read_text()),
                                schema=json.loads(m4_schema.read_text()))
            print("OK: (committed) sawin_sutherland_murmuration.json valid against sawin_sutherland_murmuration.schema.json")
            checked += 1
        except jsonschema.ValidationError as e:
            print(f"FAIL: sawin_sutherland_murmuration.json violates schema: {e.message} (at {list(e.path)})")
            failures += 1

    # M5 / PR-1 (X-extension): emit reads the committed extension run; schema-check the
    # committed snapshot here (byte-for-byte re-emit is the freshness suite's job).
    m5_file = Path(__file__).resolve().parent.parent / "viz/data/ss_x_extension_murmuration.json"
    if m5_file.exists():
        m5_schema = schema_dir / "ss_x_extension_murmuration.schema.json"
        try:
            jsonschema.validate(instance=json.loads(m5_file.read_text()),
                                schema=json.loads(m5_schema.read_text()))
            print("OK: (committed) ss_x_extension_murmuration.json valid against ss_x_extension_murmuration.schema.json")
            checked += 1
        except jsonschema.ValidationError as e:
            print(f"FAIL: ss_x_extension_murmuration.json violates schema: {e.message} (at {list(e.path)})")
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
