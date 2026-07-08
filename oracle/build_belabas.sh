#!/usr/bin/env bash
# Fetch + build Karim Belabas's `cubic` enumerator (Math. Comp. 66 (1997)) as the
# LMFDB-independent Stage 6 S3 referee. Tarball is sha256-pinned; links the system
# libpari (Belabas's cubic uses PARI's bignum types, but the cubic-enumeration
# code path — rcubic.c/ccubic.c — is separate from PARI's nflist). Produces
# build/belabas/cubic. SKIP (exit 77) if libpari/brew pari is unavailable.
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
URL="https://www.math.u-bordeaux.fr/~kbelabas/research/software/cubic-1.3.tgz"
SHA="341708d319a1471b5335ea96c307e28a99da0977ebaf902aa3f87d8a46a331e9"
OUT="$ROOT/build/belabas"

PARIP="$(brew --prefix pari 2>/dev/null || echo /usr/local)"
GMPP="$(brew --prefix gmp 2>/dev/null || echo /usr/local)"
if [ ! -f "$PARIP/include/pari/pari.h" ]; then
  echo "[SKIP] build_belabas: libpari headers not found under $PARIP (brew install pari)."
  exit 77
fi

mkdir -p "$OUT" && cd "$OUT" || exit 1
if [ ! -f cubic-1.3.tgz ]; then
  curl -sL --max-time 60 -o cubic-1.3.tgz "$URL" || { echo "[SKIP] fetch failed"; exit 77; }
fi
got="$(shasum -a 256 cubic-1.3.tgz | cut -d' ' -f1)"
if [ "$got" != "$SHA" ]; then
  echo "CHECKSUM MISMATCH: got $got expected $SHA"; exit 1
fi
rm -rf cubic-1.3 && tar xzf cubic-1.3.tgz
cd cubic-1.3 || exit 1
make all \
  PARIHOME="$PARIP" \
  INCLUDEDIR="-I$PARIP/include/pari -I$GMPP/include" \
  LDFLAGS="-L$PARIP/lib -L$GMPP/lib -Wl,-rpath,$PARIP/lib -Wl,-rpath,$GMPP/lib" \
  LIB="-lpari -lgmp -lm" >/dev/null 2>&1 || { echo "build failed"; exit 1; }
cp -f cubic cubic-noprint "$OUT/"
echo "built $OUT/cubic (Belabas cubic v1.3, sha256 $SHA verified)"
