#!/usr/bin/env python3
# Quantity 2 of the libm partial-diff (data-note §5 item-4): float partial-sum |Δ|
# between the abandoned laptop 2^17 partials (ckpt) and the committed FreeBSD 2^17
# partials, over the shared curves. Descriptive: max |Δ| + magnitude histogram.
# Also: locate the APC1 a_p-cache payload offset (for quantity 1 payload hashing).
import sys, math, hashlib

LAP = "data/m5/ss_partials_x131072.txt.ckpt"          # laptop, abandoned (8640 curves)
FBS = "data/m5/ss_partials_x131072.txt"               # FreeBSD committed (9014 curves)
NB = 40

def load(path):
    # returns dict: (A,B,N) -> [num_0..num_39]
    d = {}
    with open(path) as f:
        for line in f:
            if not line.startswith("part"):
                continue
            t = line.split()
            # part A B N  num[0..NB-1]  cnt[0..NB-1]
            A, B, N = int(t[1]), int(t[2]), int(t[3])
            nums = [float(x) for x in t[4:4+NB]]
            d[(A, B, N)] = nums
    return d

lap = load(LAP)
fbs = load(FBS)
kl, kf = set(lap), set(fbs)
shared = kl & kf
only_lap = kl - kf
print(f"laptop curves: {len(kl)}")
print(f"freebsd curves: {len(kf)}")
print(f"shared curves: {len(shared)}")
print(f"laptop-only (unmatched, expected 0): {len(only_lap)}")
print(f"laptop subset of freebsd: {kl <= kf}")

# per-(curve,bin) |delta|
maxd = 0.0
maxloc = None
exact = 0
total = 0
# histogram by |delta| magnitude decade
buckets = {}  # exp -> count ; exp = floor(log10(|d|)) ; 'zero' for exact 0
for k in sorted(shared):
    a = lap[k]; b = fbs[k]
    for bi in range(NB):
        dd = abs(a[bi] - b[bi])
        total += 1
        if dd == 0.0:
            exact += 1
            buckets['zero'] = buckets.get('zero', 0) + 1
        else:
            e = math.floor(math.log10(dd))
            buckets[e] = buckets.get(e, 0) + 1
            if dd > maxd:
                maxd = dd; maxloc = (k, bi, a[bi], b[bi])

print(f"\n--- quantity 2: float partial-sum |delta| over {len(shared)} shared curves x {NB} bins = {total} values ---")
print(f"exact-equal float values: {exact}/{total}")
print(f"max |delta|: {maxd:.6e}")
if maxloc:
    k, bi, va, vb = maxloc
    print(f"  at curve (A,B,N)={k}, bin {bi}: laptop={va:.10e} freebsd={vb:.10e}")
print("histogram of |delta| by magnitude (10^e <= |d| < 10^(e+1)):")
for key in sorted(buckets, key=lambda x: (-1e9 if x=='zero' else x)):
    if key == 'zero':
        print(f"  exact 0        : {buckets[key]}")
    else:
        print(f"  10^{key:>4} .. 10^{key+1:<4}: {buckets[key]}")

# also: relative magnitude of max delta vs the values
if maxloc:
    _,_,va,vb = maxloc
    scale = max(abs(va), abs(vb))
    if scale > 0:
        print(f"max |delta| relative to value at that point: {maxd/scale:.3e}")

# --- APC1 payload offset for the laptop a_p cache (quantity 1 helper) ---
print("\n--- APC1 a_p-cache header (laptop .ckpt) payload offset ---")
with open("data/m5/ap_cache_x131072.bin.ckpt", "rb") as f:
    head = f.read(4096)
# header is text lines: APC1\n, hash\n, family...\n, then binary
nl = 0; off = 0
for i, byte in enumerate(head):
    if byte == 0x0A:  # newline
        nl += 1
        if nl == 3:
            off = i + 1
            break
print(f"laptop payload starts at byte offset: {off}")
print(f"header text:\n{head[:off].decode('ascii', 'replace')}")
# payload sha256 (laptop)
h = hashlib.sha256()
with open("data/m5/ap_cache_x131072.bin.ckpt", "rb") as f:
    f.seek(off)
    while True:
        chunk = f.read(1 << 20)
        if not chunk:
            break
        h.update(chunk)
print(f"laptop a_p-cache PAYLOAD sha256 (offset {off}..EOF): {h.hexdigest()}")
