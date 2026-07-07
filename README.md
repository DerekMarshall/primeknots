# primeknots (working name)

Arithmetic topology, executable. From-scratch C++ verification of the theorem chain running from quadratic reciprocity (linking symmetry) through Rédei symbols (triple Milnor invariants, Borromean primes) and the Rédei–Reichardt 4-rank theorem (linking controls homology) to the abelian arithmetic Chern–Simons partition function (linking determines the TQFT), with the Weil explicit formula as the orbit/spectrum capstone. Visualizations via JSON → static D3 viewers.

- `docs/RESEARCH.md` — the mathematics: stage-by-stage theorems, verification targets, normative sources.
- `docs/ARCHITECTURE.md` — the engineering: modules, types, harness, JSON contract, acceptance criteria.
- `CLAUDE.md` — operating rules for agent sessions. Rule 1: never fit conventions to expected answers.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
ctest --test-dir build --output-on-failure
./build/bin/at emit --stage 1 --out viz/data/ && python3 -m http.server -d viz
```

Stage gates are strict: nothing builds on a stage whose verification suite isn't green.
