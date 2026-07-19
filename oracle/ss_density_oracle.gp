/* Independent PARI/GP oracle for the Sawin-Sutherland murmuration density, eq (2) of
 * arXiv:2504.12295 (Conjecture 1). Transcribed DIRECTLY from the paper PDF (eq (2) p.2,
 * Lemmas 3/4 pp.4-5, cross-checked against Definitions 5/6 p.9) -- NOT from m4-pinning or
 * ss_density.cpp. Referees that C++ and the digitized figure (CLAUDE.md rule 3).
 *
 *   D(u) = 2*Pi*sqrt(u) * Sum_{q squarefree} Sum_{m>=1}
 *            mu(gcd(m,q)) / (q*m*phi(q/gcd(m,q))) * J_1(4*Pi*sqrt(u)*m/q)
 *            * prod_{p|q} ellhat_{p, 2 v_p(m)}  *  prod_{p|m, p!|q} ell_{p, 2 v_p(m)}
 *
 * REBUILT 2026-07-18 (referee C). The first version had eq (2) WRONG two ways (blind
 * transcription B + this A re-read against the paper): (i) the two products were SWAPPED
 * (it put ellhat on p|m,p!|q and ell on p|m,p|q); (ii) the exponents were wrong -- BOTH
 * products carry 2*v_p(m) (only even nu appear, per the paper's own note on p.9; the n=m^2
 * substitution in the proof doubles the valuations). The nu-domains force it: ellhat takes
 * nonnegative EVEN nu (Lemma 4; incl. nu=0 for p|q,p!|m), ell takes positive nu (Lemma 3).
 * Consequence: ell_{p,nu} vanishes for p>3, nu<10 (weight nu+2<12), so a term survives only
 * when every prime p>3 dividing m but not q has v_p(m)>=5 -- the m-sum is radically sparse.
 *
 * The Hecke eigenvalue sums Sum_{f in S_{nu+2}(SL2Z)} a_p(f) = Tr(T_p|S_{nu+2}) are computed
 * CORRECTLY via mftraceform (verified Tr T2|S12 = -24 = tau(2)); not truncated.
 *
 * Usage:  ORACLE_B=<B> gp -q oracle/ss_density_oracle.gp
 */

default(realprecision, 30);

TrTp_cache = Map();
TrTp(k, p) = {
  if(k < 12 || k % 2 != 0, return(0));
  my(key = Str(k,",",p), v);
  if(mapisdefined(TrTp_cache, key, &v), return(v));
  if(mfdim([1,k], 1) == 0, mapput(TrTp_cache, key, 0); return(0));
  v = mfcoefs(mftraceform([1,k]), p)[p+1];
  mapput(TrTp_cache, key, v);
  v;
}

U(nu, x) = polchebyshev(nu, 2, x);

/* Lemma 3: ell_{p,nu}, nu a positive integer. */
ell(p, nu) = {
  if(p == 2, return( -(2^(-10)/(1 - 2^(-10))) * TrTp(nu+2, 2) ));
  if(p == 3,
    return( 3^(nu/2 - 2) * (U(nu, 3/(2*sqrt(3))) + 4*U(nu, 0) + U(nu, -3/(2*sqrt(3))))
            - ((3^(-10) - 3^(-11))/(1 - 3^(-10))) * TrTp(nu+2, 3) ));
  -((p^(-1) - p^(-2))/(1 - p^(-10))) * TrTp(nu+2, p);          /* p > 3 */
}

/* Lemma 4: ellhat_{p,nu}, nu a nonnegative even integer. */
ellhat(p, nu) = {
  if(p == 2,
    if(nu == 0, return( 2^(-9)/(1 - 2^(-10)) ));
    if(nu == 2, return( -(4 - 2^(-6) + 3*2^(-10))/(1 - 2^(-10)) ));
    return( -(1/(2^10 - 1)) * (3 + TrTp(nu+2, 2)) );
  );
  if(p == 3,
    if(nu == 0, return( (1 - 3^(-10))^(-1) * (2/3 + 4/3^11) ));
    if(nu == 2, return( -(3 - 3^(-7) + 16*3^(-11))/(2*(1 - 3^(-10))) ));
    return( (2/9)*(1 - 3^(-10))^(-1) *
            (3^(nu/2)*(U(nu, 3/(2*sqrt(3))) + 2*U(nu, 0)
                       + 3^(-9)*(U(nu, 2/sqrt(3)) + U(nu, 1/(2*sqrt(3))))) - 3^(-8)) );
  );
  if(nu == 0, return( (1 - p^(-1))/(1 - p^(-10)) ));           /* p > 3 */
  if(nu == 2, return( -(p - p^(-1) + p^(-2) - p^(-8))/((1 - p^(-10))*(p - 1)) ));
  -((p^(-1) - p^(-2))/(1 - p^(-10))) * (p + 1 + TrTp(nu+2, p));
}

/* D(u) truncated to q,m <= B (q squarefree), eq (2) — CORRECTED products. */
D(u, B) = {
  my(s = 0.0, sq = sqrt(u), f4 = 4*Pi*sq);
  for(q = 1, B,
    if(!issquarefree(q), next);
    my(fq = factor(q)[,1]);
    for(m = 1, B,
      my(g = gcd(m, q), ph = eulerphi(q/g), term = moebius(g)/(q*m*ph) * besselj(1, f4*m/q));
      for(i = 1, #fq, my(pp = fq[i]); term *= ellhat(pp, 2*valuation(m, pp)));   /* p | q */
      my(fm = factor(m)[,1]);
      for(i = 1, #fm, my(pp = fm[i]);
        if(q % pp != 0, term *= ell(pp, 2*valuation(m, pp))));                   /* p | m, p!|q */
      s += term;
    );
  );
  2*Pi*sq * s;
}

{
  my(B = eval(getenv("ORACLE_B")));
  if(B == 0, B = 500);
  print("# ss_density_oracle CORRECTED (PARI, from paper eq (2)); B = ", B);
  print("# u : D_oracle(u)");
  for(i = 1, 5, my(u = [0.45, 0.65, 0.805, 0.87, 0.95][i]); print("  ", u, " : ", D(u, B)));
  print("# fine scan [0.78, 0.92] step 0.005");
  my(best_u = 0, best_v = 1e9);
  forstep(u = 0.78, 0.92, 0.005, my(v = D(u, B)); print("  ", u, " : ", v);
    if(v < best_v, best_v = v; best_u = u));
  print("# trough over [0.78,0.92]:  u = ", best_u, "  D = ", best_v);
}
