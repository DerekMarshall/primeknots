#!/usr/bin/env python3
"""ss-run progress TUI — a READ-ONLY monitor for the heavy `at ss-run` a_p pass.

Zero performance hit by construction: it parses only the small, append-only run LOG
(the `checkpoint: N/TOTAL curves done (Ts elapsed)` telemetry lines that ss-run already
emits) — never the multi-MB partials/checkpoint files, never any of the run's memory, and
it holds no locks. Default is a one-shot snapshot (no ongoing cost); `--watch S` redraws
every S seconds for your own terminal. It reads NO shape data — only counts + timings — so
it stays R3-clean (no mid-run tea leaves).

    python3 scripts/ss_progress.py --log <run.log> [--watch 10]

Segments (resume cycles) are detected by the per-segment elapsed clock resetting; `banked`
is the running max of N (monotonic), and the rate/ETA use the CURRENT segment's recent
checkpoints. The ETA is labelled a floor, because the high-conductor tail costs O(N²/ln N)
per curve — it only gets slower toward the end.
"""
import argparse
import os
import re
import sys
import time

CKPT = re.compile(r"checkpoint:\s+(\d+)/(\d+)\s+curves done\s+\((\d+)s elapsed\)")
BLOCKS = "▁▂▃▄▅▆▇█"


def parse(logpath):
    pts = []          # (n, total, elapsed) per checkpoint line
    resumes = 0
    done = False
    if not os.path.exists(logpath):
        return pts, resumes, done
    with open(logpath, errors="replace") as f:
        for line in f:
            m = CKPT.search(line)
            if m:
                pts.append((int(m.group(1)), int(m.group(2)), int(m.group(3))))
                continue
            if "RESUMED" in line:            # `at ss-run: RESUMED N/…` (mid-line)
                resumes += 1
            if "WRAPPER DONE" in line:
                done = True
    return pts, resumes, done


def segments(pts):
    """Split checkpoint points where the elapsed clock resets (a new at process)."""
    segs, cur = [], []
    prev_e = -1
    for n, tot, e in pts:
        if e < prev_e:            # elapsed went backwards → new segment
            if cur:
                segs.append(cur)
            cur = []
        cur.append((n, tot, e))
        prev_e = e
    if cur:
        segs.append(cur)
    return segs


def sparkline(vals):
    if not vals:
        return ""
    lo, hi = min(vals), max(vals)
    if hi == lo:
        return BLOCKS[0] * len(vals)
    return "".join(BLOCKS[int((v - lo) / (hi - lo) * (len(BLOCKS) - 1))] for v in vals)


def human(sec):
    sec = int(sec)
    d, sec = divmod(sec, 86400)
    h, sec = divmod(sec, 3600)
    mn, sec = divmod(sec, 60)
    if d:
        return f"{d}d {h}h {mn}m"
    if h:
        return f"{h}h {mn}m"
    return f"{mn}m {sec}s"


def render(logpath, ckpt_hint):
    pts, resumes, wrapper_done = parse(logpath)
    if not pts:
        return f"ss-run progress — no checkpoint telemetry yet in {logpath}\n"
    banked = max(n for n, _, _ in pts)
    total = pts[-1][1]
    segs = segments(pts)
    cur = segs[-1]

    # per-chunk seconds across ALL segments (the climbing-cost sparkline)
    chunk_secs = []
    for seg in segs:
        for i in range(1, len(seg)):
            dn = seg[i][0] - seg[i - 1][0]
            de = seg[i][2] - seg[i - 1][2]
            if dn > 0 and de > 0:
                chunk_secs.append(de / dn)      # seconds per curve in that chunk

    # recent rate from the current segment's last few points
    rate = None
    if len(cur) >= 2:
        dn = cur[-1][0] - cur[max(0, len(cur) - 4)][0]
        de = cur[-1][2] - cur[max(0, len(cur) - 4)][2]
        if dn > 0:
            rate = de / dn                       # s / curve, recent
    remaining = total - banked
    frac = banked / total if total else 0
    width = 44
    fill = int(frac * width)
    bar = "█" * fill + "░" * (width - fill)

    complete = wrapper_done or (ckpt_hint == "final")
    lines = []
    lines.append("┌─ ss-run  (2^17 murmuration a_p pass) ─────────────────────────────┐")
    lines.append(f"│ [{bar}] {frac*100:5.1f}%")
    lines.append(f"│ banked {banked:>5}/{total:<5} curves      remaining {remaining:>5}")
    if complete:
        lines.append("│ STATUS: COMPLETE — final partials written")
    elif rate:
        eta = remaining * rate
        lines.append(f"│ rate  ~{rate:6.1f} s/curve (recent)   ~{rate*64/60:5.1f} min / 64-chunk")
        lines.append(f"│ ETA   ≳ {human(eta):<12} (floor — tail cost rises O(N²/ln N))")
    else:
        lines.append("│ rate  (warming up — need 2+ checkpoints this segment)")
    lines.append(f"│ resumes: {resumes:<3}   segments: {len(segs):<3}   "
                 f"cur-seg elapsed {human(cur[-1][2])}")
    if chunk_secs:
        tail = chunk_secs[-40:]
        lines.append(f"│ s/curve trend {sparkline(tail)}  ({tail[0]:.0f}→{tail[-1]:.0f}s)")
    lines.append("└───────────────────────────────────────────────────────────────────┘")
    return "\n".join(lines) + "\n"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--log", required=True, help="the ss-run log file")
    ap.add_argument("--ckpt", default="", help="optional: final-partials path; if it exists, shows COMPLETE")
    ap.add_argument("--watch", type=float, default=0, help="redraw every N seconds (0 = one-shot)")
    args = ap.parse_args()

    def hint():
        return "final" if (args.ckpt and os.path.exists(args.ckpt)) else ""

    if args.watch <= 0:
        sys.stdout.write(render(args.log, hint()))
        return 0
    try:
        while True:
            sys.stdout.write("\033[2J\033[H")     # clear + home
            sys.stdout.write(render(args.log, hint()))
            sys.stdout.write(f"\n(watching every {args.watch:g}s — Ctrl-C to stop)\n")
            sys.stdout.flush()
            time.sleep(args.watch)
    except KeyboardInterrupt:
        return 0


if __name__ == "__main__":
    sys.exit(main())
