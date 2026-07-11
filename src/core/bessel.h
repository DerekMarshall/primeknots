#pragma once

// Bessel function of the first kind J₁(x), from scratch (M4/C4). The Sawin–Sutherland
// murmuration density (Conjecture 1) is a sum of J₁(4π√u·m/q); its emitted snapshot
// must be byte-identical across compilers/platforms (freshness), so J₁ is computed with
// **only IEEE-deterministic operations** (+ − × ÷ and correctly-rounded sqrt) — NO libm
// transcendentals. An in-house IEEE sin/cos backs the large-argument asymptotic; with
// FP contraction disabled this makes J₁ bit-reproducible everywhere (see
// [[cross-compiler-emit-determinism]]). CORRECTNESS (not bit-exactness) is verified by
// the twin against PARI `besselj` (verify/m4). Series for |x| ≤ 12, asymptotic beyond
// (switchover pinned in bessel.cpp). J₁ is odd: J₁(−x) = −J₁(x); J₁(0) = 0.
namespace at::core {

double bessel_j1(double x);

}  // namespace at::core
