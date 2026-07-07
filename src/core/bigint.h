#pragma once

// src/core/bigint.h — seam for the GMP facade (ARCHITECTURE.md §2).
//
// RESEARCH.md places arbitrary-precision arithmetic no earlier than the
// large-discriminant part of Stage 3 and parts of Stage 4. Stage 0 does not
// use it. This header intentionally declares nothing: it exists only so the
// -DWITH_GMP toggle and the include seam are already in place. The Bigint type
// is authored by the first stage that actually needs values beyond 128 bits.
//
// Deliberately no premature API surface here — that would be Stage 3/4
// scaffolding chosen before the relevant sources have been read.

#ifdef WITH_GMP
#  error "WITH_GMP is ON, but no Bigint implementation exists yet. \
No stage before Stage 3 requires it; build without -DWITH_GMP."
#endif
