#!/usr/bin/env python3
"""Referee round C — GATE: digitize the SS25 density (green) curve and locate its trough
numerically, to resolve the A.4 eyeball (~0.85-0.90) against B1's stable 0.805.

Reads a 400-dpi render of an SS25 figure page, isolates the GREEN curve (the RHS density;
the empirical LHS is purple), self-calibrates pixel-x -> u by the curve's own [0,1] span,
and reports the hump (global max) and trough (global min) in u. The hump is the
CALIBRATION CHECK: our D(u) hump is at u=0.475 (B1), so a digitized hump near 0.475
validates the x-mapping and makes the trough position trustworthy.

Usage:  .venv/bin/python docs/notes/referee_digitize.py /tmp/ss_fig1-03.png [--bands N]
        (--bands N splits the page into N stacked subplots by green-pixel y-clusters,
         for Fig 5's P=2,4,8,16 panels; default 1 for Fig 1.)
"""
import sys
import numpy as np
import matplotlib.image as mpimg


def green_mask(img):
    r, g, b = img[:, :, 0], img[:, :, 1], img[:, :, 2]
    # forest/dark green: G clearly dominant, not black, not near-white
    return (g > r + 0.10) & (g > b + 0.10) & (g > 0.20) & (r < 0.80)


def curve_shape(xs, ys, x0, x1, smooth=15):
    """Given green-pixel columns xs and rows ys, return (hump_u, zero_u_approx, trough_u).
    Image y increases downward, so highest point = min y, lowest point = max y."""
    cols = np.arange(x0, x1 + 1)
    yc = np.full(cols.shape, np.nan)
    for i, c in enumerate(cols):
        sel = ys[xs == c]
        if sel.size:
            yc[i] = sel.mean()
    good = ~np.isnan(yc)
    cg, yg = cols[good], yc[good]
    # smooth with EDGE padding (mode="same"/zero-pad pulls the ends toward 0 and
    # fabricates a spurious extreme at the left/right edge — the earlier hump=0.000 bug)
    if smooth > 1 and yg.size > smooth:
        pad = smooth // 2
        ypad = np.pad(yg, pad, mode="edge")
        k = np.ones(smooth) / smooth
        yg = np.convolve(ypad, k, mode="valid")[: yg.size]
    u = (cg - cg.min()) / (cg.max() - cg.min())        # self-calibration: span -> [0,1]
    hump_u = u[int(np.argmin(yg))]                      # highest point (min image-y)
    trough_u = u[int(np.argmax(yg))]                    # lowest point (max image-y)
    # first +->- crossing of the centerline (relative to its own median baseline)
    base = np.median(yg)
    zero_u = None
    hidx = int(np.argmin(yg))
    for i in range(hidx, len(yg) - 1):
        if yg[i] <= base <= yg[i + 1]:                  # going down through baseline
            zero_u = u[i]
            break
    return hump_u, zero_u, trough_u, u, yg


def main():
    path = sys.argv[1]
    bands = 1
    if "--bands" in sys.argv:
        bands = int(sys.argv[sys.argv.index("--bands") + 1])
    img = mpimg.imread(path)
    if img.dtype != np.float32 and img.max() > 1.5:
        img = img / 255.0
    m = green_mask(img)
    ys, xs = np.where(m)
    print(f"# {path}: {xs.size} green pixels, image {img.shape[1]}x{img.shape[0]}")
    if xs.size < 100:
        print("!! too few green pixels — mask miscalibrated"); return

    if bands == 1:
        x0, x1 = xs.min(), xs.max()
        hu, zu, tu, u, yg = curve_shape(xs, ys, x0, x1)
        print(f"hump_u={hu:.3f}  zero_u={zu if zu is None else round(zu,3)}  "
              f"trough_u={tu:.3f}   (hump should be ~0.475 as a calibration check)")
    else:
        # split by y into `bands` stacked panels (Fig 5)
        ymin, ymax = ys.min(), ys.max()
        edges = np.linspace(ymin, ymax + 1, bands + 1)
        labels = [2, 4, 8, 16][:bands]
        for k in range(bands):
            sel = (ys >= edges[k]) & (ys < edges[k + 1])
            bx, by = xs[sel], ys[sel]
            if bx.size < 100:
                print(f"P={labels[k]}: too few pixels"); continue
            hu, zu, tu, u, yg = curve_shape(bx, by, bx.min(), bx.max())
            print(f"P={labels[k]:>2}: hump_u={hu:.3f}  "
                  f"zero_u={zu if zu is None else round(zu,3)}  trough_u={tu:.3f}")


if __name__ == "__main__":
    main()
