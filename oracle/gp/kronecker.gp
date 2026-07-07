\\ oracle/gp/kronecker.gp — reviewable reference for the Stage 0 kronecker oracle.
\\
\\ The `oracle_kronecker` test (verify/stage0/oracle_tests.cpp) confirms our
\\ Jacobi/Legendre symbols against PARI/GP's built-in kronecker(a,n). For odd
\\ positive n, kronecker(a,n) == our jacobi(a,n); for an odd prime p,
\\ kronecker(a,p) == our legendre(a,p) (both 0 when the prime divides a).
\\
\\ The test emits one `print(kronecker(a,n))` per sampled pair as a single
\\ batch (one gp spawn, thousands of checks — ARCHITECTURE.md §4) and compares
\\ each line to our value. This file documents that computation and lets a
\\ human reproduce a spot check by hand:
\\
\\     gp -q oracle/gp/kronecker.gp
\\
print(kronecker(30, 13))   \\ 30 ≡ 4 (mod 13), a residue          -> 1
print(kronecker(2, 7))     \\ 7 ≡ 7 (mod 8)                        -> 1
print(kronecker(5, 7))     \\ nonresidue mod 7                     -> -1
print(kronecker(1001, 9))  \\ odd composite modulus (Jacobi)      -> 1
print(kronecker(3, 9))     \\ gcd(3,9) > 1                         -> 0
quit
