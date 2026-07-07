# Twin-independence reviews

CLAUDE.md rule 2 requires each load-bearing function to have two *genuinely
independent* implementations. "Independent" is a claim about the call graph, and
it is cheap to verify by eye — so we do, once per twin pair, and log it here.
This is human review, not just an assertion in a test: a `twin_` test passing
only proves the two implementations *agree*, not that they are independent (two
copies of the same buggy algorithm agree perfectly).

Precedent: every new twin pair gets an entry here before its stage is called
done.

## Stage 0

### Legendre: `legendre_euler` vs `legendre_recip`  (reviewed 2026-07-07)

- `legendre_euler` (src/symbols/legendre.cpp): Euler's criterion. Sole numeric
  engine is `at::core::modpow` (modular exponentiation).
- `legendre_recip` (same file): calls only `jacobi(a % p, p)`.
- `jacobi` (src/symbols/jacobi.cpp): includes only `<utility>` (for std::swap);
  no `core/` header, no `modpow`, no `modmul`. Pure binary reciprocity recursion
  (2-adic factoring + reciprocity sign flips).
- **Verdict: independent.** The reciprocity path never touches modular
  exponentiation. The two share only the `u64` type and the trivial `a % p`
  reduction — no shared algorithm.

### is_prime vs sieve  (reviewed 2026-07-07)

- `is_prime` (src/core/primes.cpp): deterministic Miller–Rabin, uses `modpow` +
  `modmul`.
- Referee: an Eratosthenes sieve, kept in verify/stage0/util.h. No shared code
  with Miller–Rabin. **Independent.**

### modpow vs modpow_naive  (reviewed 2026-07-07)

- Both use the square-and-multiply ladder; the load-bearing difference is the
  modular multiply — Montgomery REDC vs. direct `(u128)a*b % m`. The trivial
  ladder is not where bugs hide; the reduction is, and there the two are fully
  independent. **Independent for the purpose that matters** (noted explicitly
  because, unlike the pairs above, the exponentiation skeleton *is* shared).
- **Shared-ladder caveat CLOSED (2026-07-07):** `oracle_modpow`
  (verify/stage0/oracle_tests.cpp) referees `core::modpow` against PARI/GP's
  `Mod(a,m)^e` over 363 samples spanning odd (Montgomery) and even (direct)
  moduli with large exponents. gp's powering shares no code with ours, so a bug
  in the common ladder cannot survive it. Added at the start of the Stage 1
  session, before any Stage 1 code, per the reviewer's instruction.

### jacobi vs product-of-Legendre-over-factors  (reviewed 2026-07-07)

- `jacobi`: binary recursion. Referee (`jacobi_via_factorization` in
  verify/stage0/symbols_tests.cpp): factor n, multiply `legendre_euler` over the
  prime factors — i.e. Euler's criterion, a different route entirely.
  **Independent.**
