\\ Stage 6 PARI referee: count iso classes of non-Galois (S3) cubic fields K
\\ with rad(disc K) subset of S, |disc K| <= prod_{p in S} p^2 (the completeness
\\ bound, docs/notes/stage6-pinning.md R4: tame => v_p(disc)<=2).
\\
\\ Enumerate per quadratic resolvent m = squarefree product of a nonempty subset
\\ of S (an S3 cubic's resolvent is Q(sqrt(disc)), so m ranges over subset
\\ products); nflist with a resolvent uses class field theory and stays cheap
\\ even when Dmax is huge. Each field is independently reclassified with polgalois
\\ (order 6 == S3) rather than trusting nflist's own labelling.
\\
\\ Output per S (one line): "RESULT <p1,..> | <Dmax> | <c> | <all_real 0/1> | <d1,..>"

subsets(S) = {my(r=List(),n=#S);for(mask=1,2^n-1,my(t=List());for(i=1,n,if(bittest(mask,i-1),listput(t,S[i])));listput(r,Vec(t)));Vec(r)}

countc(S) = {my(Sset=Set(S),Dmax=prod(i=1,#S,S[i]^2),c=0,discs=List(),allreal=1,subs=subsets(S));for(a=1,#subs,my(m=prod(i=1,#subs[a],subs[a][i]),v=nflist("S3",[1,Dmax],-1,x^2-m));for(i=1,#v,my(d=nfdisc(v[i]),g=polgalois(v[i])[1],ok=1);if(g!=6,ok=0);my(fac=factor(abs(d))[,1]);for(j=1,#fac,if(!setsearch(Sset,fac[j]),ok=0));if(ok,c++;listput(discs,d);if(d<0,allreal=0))));[Dmax,c,vecsort(Vec(discs)),allreal]}

\\ Second, independent enumeration STRATEGY: candidate discriminants d = prod
\\ p^a (a in 0..2, rad(d) subset S, d <= Dmax), count S3 cubics of |disc| = d via
\\ the bare-integer nflist form (disc EXACTLY d). Must equal countc's resolvent-
\\ range count; a disagreement localizes a driver-logic bug (Rule 1).
canddiscs(S) = {my(r=[1]);for(i=1,#S,my(nw=List());for(j=1,#r,for(a=0,2,listput(nw,r[j]*S[i]^a)));r=Vec(nw));my(o=List());for(i=1,#r,if(r[i]>1,listput(o,r[i])));Set(Vec(o))}
countc_bydisc(S) = {my(Dmax=prod(i=1,#S,S[i]^2),cd=canddiscs(S),c=0);for(i=1,#cd,if(cd[i]<=Dmax,my(v=nflist("S3",cd[i]));for(j=1,#v,if(polgalois(v[j])[1]==6,c++))));c}

runS(S) = {my(r=countc(S),cb=countc_bydisc(S),ds=r[3],dstr="");for(i=1,#ds,dstr=Str(dstr,if(i>1,",",""),ds[i]));my(pstr="");for(i=1,#S,pstr=Str(pstr,if(i>1,",",""),S[i]));print("RESULT ",pstr," | ",r[1]," | ",r[2]," | ",cb," | ",r[4]," | ",dstr)}
