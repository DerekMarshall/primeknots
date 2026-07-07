# Stage 1 — decisions: sampling policy, determinism, conventions

Recorded per the reviewer's instruction (the twin sampling policy is a decision)
and CLAUDE.md rule 5.

## S1 — Twin cross-check sampling policy

The linking matrix is filled by the fast Legendre twin (`legendre_euler`, via
`modpow`). A full twin recomputation of all ~7.6×10⁸ entries at bound 10⁶ with
the reciprocity twin is not required (reviewer). The referee pass
(`twin_linking_euler_vs_reciprocity`) recomputes with `legendre_recip` (via
`jacobi`, no `modpow`) using:

1. **Leading-block exhaustive:** every off-diagonal entry among the first
   `K = min(n, 200)` primes — the densest, most-reused corner of the matrix
   (K·(K−1) = 39 800 entries at K = 200).
2. **Uniform random sample across the whole matrix:** a fixed deterministic
   count — **200 000** (default) / **2 000 000** (`--extended`) — of entries
   `(i, j)` drawn by a seeded LCG, so any failure is replayable.

Rationale: the leading block guarantees the corner every later stage leans on is
100% twin-checked; the uniform sample gives broad coverage of the tail at fixed
cost. The exact `modpow`-vs-`gp` question (the shared exponentiation ladder) is
closed separately by `oracle_modpow` (Stage 0) and by `oracle_kronecker_linking`
(a gp referee over 5 000 / 20 000 sampled entries here).

The symmetry sweep, by contrast, is **exhaustive over every pair** at the run's
bound — it is quadratic reciprocity verified at scale, not sampled.

## S2 — Bounds: default vs. --extended

- Default test bound **50 000** → **2 549** primes ≡ 1 (mod 4) (π(5·10⁴; 4,1)),
  C(2549,2) = 3 247 426 symmetry pairs. Keeps `ctest -L stage1` at a few seconds.
- `--extended` bound **10⁶** → **39 175** primes (π(10⁶; 4,1)),
  C(39175,2) ≈ 7.67×10⁸ symmetry pairs — the full-scale sweep in ARCHITECTURE §6.

Prime counts are asserted against π(x; 4,1) values obtained from **PARI/GP**, not
from our own sieve (Stage 0 precedent): 80 (10³), 2 549 (5·10⁴), 39 175 (10⁶).

## S3 — Parallel build determinism

`LinkingMatrix::build` partitions **rows** across threads. Row i's bits occupy a
disjoint word range (`i*stride .. i*stride+stride`), so threads never write the
same word — the fill is race-free with no locks. Each entry is a pure function of
`(p_i, p_j)`, and `(i,j)` / `(j,i)` are computed independently (in different
rows). Therefore the result is **bit-identical** for any thread count;
`invariance_build_parallel_vs_serial` proves serial == parallel(hw) ==
parallel(8). Computing both directions (rather than filling the upper triangle
and mirroring) is deliberate: it makes symmetry a genuinely emergent, testable
property.

## S4 — Diagonal convention

`lk(i,i) = 0`. The linking adjacency has no self-loops; there is no Legendre
symbol `(p/p)` to interpret. This is **not** the Rédei matrix — that Stage 3
object has a source-pinned diagonal `[PIN TO SOURCE]` and a different meaning.
Nothing here touches it.
