#!/usr/bin/env python3
# Parse an APC1 a_p-cache header (src/ell/ap_cache.cpp layout) and report fields +
# exact payload offset. Optionally sha256 the int16 payload. Works on a full file or
# a header-only blob (payload sha skipped if truncated).
import sys, struct, hashlib

def parse(path, do_payload_sha):
    with open(path, "rb") as f:
        data_head = f.read(4096)  # header is well under 4 KB
        assert data_head[:4] == b"APC1", f"bad magic: {data_head[:4]!r}"
        off = 4
        def u32():
            nonlocal off
            v = struct.unpack_from("<I", data_head, off)[0]; off += 4; return v
        def u64():
            nonlocal off
            v = struct.unpack_from("<Q", data_head, off)[0]; off += 8; return v
        def s():
            nonlocal off
            n = u64(); v = data_head[off:off+n]; off += n; return v.decode("ascii","replace")
        fmt = u32()
        gen = s()
        ecd = s()
        pb  = u64()
        cs  = s()
        complete = u32()
        n_done = u64()
        payload_off = off
        print(f"file: {path}")
        print(f"  format_version: {fmt}")
        print(f"  generator_hash: {gen}")
        print(f"  ecdata_sha:     {ecd}")
        print(f"  prime_bound:    {pb}")
        print(f"  curve_set:      {cs}")
        print(f"  complete:       {complete}")
        print(f"  n_done:         {n_done}")
        print(f"  payload_offset: {payload_off}")
        if do_payload_sha:
            h = hashlib.sha256()
            nbytes = 0
            with open(path, "rb") as g:
                g.seek(payload_off)
                while True:
                    c = g.read(1 << 20)
                    if not c: break
                    h.update(c); nbytes += len(c)
            print(f"  payload_bytes:  {nbytes}  (int16 count = {nbytes//2})")
            print(f"  payload_sha256: {h.hexdigest()}")
        return payload_off

if __name__ == "__main__":
    path = sys.argv[1]
    do_sha = (len(sys.argv) > 2 and sys.argv[2] == "sha")
    parse(path, do_sha)
