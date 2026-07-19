/* Independent PARI/GP oracle for the Sawin-Sutherland murmuration density, eq (2) of
 * arXiv:2504.12295 (Conjecture 1), implemented VERBATIM from Lemmas 3 and 4.
 *
 * Referee round C, Step 1 (docs/notes/referee-round-2026-07.md). This is a FRESH,
 * independent implementation written with NO reference to src/murm/ss_density.cpp -- it
 * exists to referee that C++ (and the digitized figure) as an external oracle (CLAUDE.md
 * rule 3). The Hecke eigenvalue sums Sum_{f in S_{nu+2}(SL2Z)} a_p(f) = Tr(T_p | S_{nu+2})
 * are computed CORRECTLY here via PARI's modular-forms package (mftraceform), NOT
 * truncated -- that is precisely the term under suspicion in the C++.
 *
 *   D(u) = 2*Pi*sqrt(u) * Sum_{q squarefree} Sum_{m>=1}
 *            mu(gcd(m,q)) / (q*m*phi(q/gcd(m,q))) * J_1(4*Pi*sqrt(u)*m/q)
 *            * prod_{p|m, p!|q} ellhat_{p,2*v_p(m)}  *  prod_{p|m, p|q} ell_{p,v_p(m)}
 *
 * Usage:  gp -q oracle/ss_density_oracle.gp        (runs the referee-C evaluation)
 *         or read() it and call D(u,B), ell(p,nu), ellhat(p,nu), TrTp(k,p).
 */

default(realprecision, 30);

/* Tr(T_p | S_k(SL2Z)), cuspidal, level 1.  0 for k<12 or k odd or dim 0. Cached. */
TrTp_cache = Map();
TrTp(k, p) = {
  if(k < 12 || k % 2 != 0, return(0));
  my(key = Str(k,",",p), v);
  if(mapisdefined(TrTp_cache, key, &v), return(v));
  if(mfdim([1,k], 1) == 0, mapput(TrTp_cache, key, 0); return(0));
  v = mfcoefs(mftraceform([1,k]), p)[p+1];   /* a_p of the cuspidal trace form = Tr T_p */
  mapput(TrTp_cache, key, v);
  v;
}

U(nu, x) = polchebyshev(nu, 2, x);           /* Chebyshev of the 2nd kind, U_nu(x) */

/* Lemma 3: ell_{p,nu}, nu a positive integer. */
ell(p, nu) = {
  if(p == 2, return( -(2^(-10)/(1 - 2^(-10))) * TrTp(nu+2, 2) ));
  if(p == 3,
    return( 3^(nu/2 - 2) * (U(nu, 3/(2*sqrt(3))) + 4*U(nu, 0) + U(nu, -3/(2*sqrt(3))))
            - ((3^(-10) - 3^(-11))/(1 - 3^(-10))) * TrTp(nu+2, 3) ));
  /* p > 3 */
  -((p^(-1) - p^(-2))/(1 - p^(-10))) * TrTp(nu+2, p);
}

/* Lemma 4: ellhat_{p,nu}, nu a nonnegative even integer. */
ellhat(p, nu) = {
  if(p == 2,
    if(nu == 0, return( 2^(-9)/(1 - 2^(-10)) ));
    if(nu == 2, return( -(4 - 2^(-6) + 3*2^(-10))/(1 - 2^(-10)) ));
    return( -(1/(2^10 - 1)) * (3 + TrTp(nu+2, 2)) );          /* nu > 2 */
  );
  if(p == 3,
    if(nu == 0, return( (1 - 3^(-10))^(-1) * (2/3 + 4/3^11) ));
    if(nu == 2, return( -(3 - 3^(-7) + 16*3^(-11))/(2*(1 - 3^(-10))) ));
    return( (2/9)*(1 - 3^(-10))^(-1) *
            (3^(nu/2)*(U(nu, 3/(2*sqrt(3))) + 2*U(nu, 0)
                       + 3^(-9)*(U(nu, 2/sqrt(3)) + U(nu, 1/(2*sqrt(3))))) - 3^(-8)) ); /* nu>2 */
  );
  /* p > 3 */
  if(nu == 0, return( (1 - p^(-1))/(1 - p^(-10)) ));
  if(nu == 2, return( -(p - p^(-1) + p^(-2) - p^(-8))/((1 - p^(-10))*(p - 1)) ));
  -((p^(-1) - p^(-2))/(1 - p^(-10))) * (p + 1 + TrTp(nu+2, p));  /* nu > 2 */
}

/* D(u) truncated to q,m <= B (q squarefree), eq (2) verbatim. */
D(u, B) = {
  my(s = 0.0, sq = sqrt(u), f4 = 4*Pi*sq);
  for(q = 1, B,
    if(!issquarefree(q), next);
    for(m = 1, B,
      my(g = gcd(m, q), ph = eulerphi(q/g), coef = moebius(g)/(q*m*ph));
      my(term = coef * besselj(1, f4*m/q));
      my(fm = factor(m)[,1]);
      for(i = 1, #fm,
        my(pp = fm[i], vp = valuation(m, pp));
        if(q % pp == 0, term *= ell(pp, vp), term *= ellhat(pp, 2*vp));
      );
      s += term;
    );
  );
  2*Pi*sq * s;
}

/* ---- referee-C Step-1 evaluation ---- */
{
  my(B = eval(getenv("ORACLE_B")));
  if(B == 0, B = 500);
  print("# ss_density_oracle (PARI, independent of C++), eq (2) from Lemmas 3/4");
  print("# B = ", B, ", realprecision = ", default(realprecision));
  print("# five anchor points  u : D_oracle(u)");
  for(i = 1, 5,
    my(u = [0.45, 0.65, 0.805, 0.87, 0.95][i]);
    print("  ", u, " : ", D(u, B));
  );
  print("# fine scan [0.78, 0.92] step 0.005");
  my(best_u = 0, best_v = 1e9);
  forstep(u = 0.78, 0.92, 0.005,
    my(v = D(u, B));
    print("  ", u, " : ", v);
    if(v < best_v, best_v = v; best_u = u);
  );
  print("# oracle trough over [0.78,0.92]:  u = ", best_u, "  D = ", best_v);
}
