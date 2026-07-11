// Stage M5 (research mode) — PR-1 X-extension infrastructure twins.
//
// These lock the two properties the 2¹⁶ run's crash-safety checkpoint relies on, BEFORE
// the ~12 h run commits to them (rule 2; PR-1 R2/R3):
//   twin_ss_partials_chunked_vs_single — computing per-curve partials in curve-chunks and
//     concatenating is BYTE-IDENTICAL to a single pass, at a FIXED thread count. (Each
//     prime sits at a maxN-independent index → same thread → same accumulation order; the
//     per-thread partials reduce in fixed thread order.) If this failed, a RESUMED run
//     would silently differ from a clean one — the checkpoint would corrupt the statistic.
//   twin_ss_partials_file_roundtrip — write_ss_partials → read_ss_partials reconstructs
//     the partials bit-for-bit (max_digits10 round-trip), the aggregate shape is identical,
//     and an incomplete (checkpoint) file is REFUSED unless resuming (R3 no-peeking).
//
// Synthetic N/ε: the chunk-invariance and I/O fidelity are independent of whether N is the
// true conductor, so these run with no PARI and no committed cache.
#include "doctest/doctest.h"

#include <algorithm>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

#include "murm/height_family.h"
#include "murm/ss_empirical.h"

using at::core::i64;
using namespace at::murm;

namespace {
// A deterministic, PARI-free test family: real reduced (A,B) from the height enumeration,
// with a pseudo-conductor spread over [200,4200) (so many primes/bins populate) and a
// deterministic ε=±1. NOT the true conductor — irrelevant to the invariants under test.
std::vector<NeRow> synthetic_rows(i64 X) {
    const std::vector<HeightCurve> fam = height_family(X);
    std::vector<NeRow> rows;
    rows.reserve(fam.size());
    for (const HeightCurve& c : fam) {
        const i64 A = c.A, B = c.B;
        const i64 N = 200 + (((A * 131 + B * 197) % 4000) + 4000) % 4000;
        const int eps = (((A + B) % 2) == 0) ? 1 : -1;
        rows.push_back({A, B, N, eps});
    }
    return rows;
}

// Reassemble full partials by concatenating per-chunk partials in canonical row order.
SSPartials reassemble_chunked(const std::vector<NeRow>& rows, double du, int T, std::size_t chunk) {
    const std::size_t C = rows.size();
    SSPartials out;
    out.du = du;
    out.NB = static_cast<int>(1.0 / du + 0.5);  // matches ss_empirical_partials' lround(1/du)
    out.A.resize(C); out.B.resize(C); out.N.resize(C);
    out.num.assign(C, std::vector<double>(out.NB, 0.0));
    out.cnt.assign(C, std::vector<i64>(out.NB, 0));
    for (std::size_t s = 0; s < C; s += chunk) {
        const std::size_t stop = std::min(s + chunk, C);
        std::vector<NeRow> cr(rows.begin() + static_cast<std::ptrdiff_t>(s),
                              rows.begin() + static_cast<std::ptrdiff_t>(stop));
        const SSPartials cp = ss_empirical_partials(cr, du, T);
        for (std::size_t j = 0; j < cr.size(); ++j) {
            const std::size_t c = s + j;
            out.A[c] = cp.A[j]; out.B[c] = cp.B[j]; out.N[c] = cp.N[j];
            out.num[c] = cp.num[j]; out.cnt[c] = cp.cnt[j];
        }
    }
    return out;
}
}  // namespace

TEST_CASE("twin_ss_partials_chunked_vs_single") {
    const std::vector<NeRow> rows = synthetic_rows(3000);
    REQUIRE(rows.size() > 100);
    const double du = 0.025;

    for (int T : {1, 3, 4}) {
        const SSPartials full = ss_empirical_partials(rows, du, T);
        for (int chunk : {17, 64, 200}) {
            const SSPartials asmb = reassemble_chunked(rows, du, T, static_cast<std::size_t>(chunk));
            std::size_t diff_num = 0, diff_cnt = 0;
            for (std::size_t c = 0; c < rows.size(); ++c)
                for (int b = 0; b < full.NB; ++b) {
                    // EXACT double equality is the intent: chunking must not perturb even
                    // the last bit, or a resumed run diverges from a clean one.
                    if (asmb.num[c][b] != full.num[c][b]) ++diff_num;
                    if (asmb.cnt[c][b] != full.cnt[c][b]) ++diff_cnt;
                }
            MESSAGE("T=" << T << " chunk=" << chunk
                    << ": diff_num=" << diff_num << " diff_cnt=" << diff_cnt);
            CHECK(diff_num == 0);
            CHECK(diff_cnt == 0);
        }
    }

    // End-to-end: the aggregate shape from reassembled partials is identical to single-pass.
    const SSPartials full = ss_empirical_partials(rows, du, 4);
    const SSPartials asmb = reassemble_chunked(rows, du, 4, 50);
    const SSEmpirical es = ss_aggregate(full, 3000);
    const SSEmpirical ec = ss_aggregate(asmb, 3000);
    bool shape_eq = es.shape.hump_u == ec.shape.hump_u &&
                    es.shape.zero_u == ec.shape.zero_u &&
                    es.shape.trough_u == ec.shape.trough_u;
    CHECK(shape_eq);
    REQUIRE(shape_eq);
}

TEST_CASE("twin_ss_partials_file_roundtrip") {
    const std::vector<NeRow> rows = synthetic_rows(2000);
    REQUIRE(rows.size() > 50);
    const double du = 0.025;
    const SSPartials P = ss_empirical_partials(rows, du, 3);

    SSPartialsMeta meta;
    meta.generator_hash = ss_generator_hash();
    meta.X = 2000; meta.du = du; meta.NB = P.NB;
    meta.n_curves = static_cast<i64>(P.A.size());
    meta.threads = 3; meta.complete = true; meta.ne_cache = "(synthetic)";

    const std::string path = "ss_partials_roundtrip.tmp";
    write_ss_partials(path, P, meta);

    SSPartialsMeta m2;
    const SSPartials P2 = read_ss_partials(path, m2);
    REQUIRE(P2.A.size() == P.A.size());
    REQUIRE(P2.NB == P.NB);

    std::size_t diff = 0;
    for (std::size_t c = 0; c < P.A.size(); ++c) {
        if (P2.A[c] != P.A[c] || P2.B[c] != P.B[c] || P2.N[c] != P.N[c]) ++diff;
        for (int b = 0; b < P.NB; ++b) {
            if (P2.num[c][b] != P.num[c][b]) ++diff;   // max_digits10 must round-trip exactly
            if (P2.cnt[c][b] != P.cnt[c][b]) ++diff;
        }
    }
    MESSAGE("roundtrip mismatches: " << diff << " over " << P.A.size() << " curves");
    CHECK(diff == 0);

    // Re-aggregation over the read-back partials matches in-memory (shape bit-identical).
    const SSEmpirical ea = ss_aggregate(P, 2000);
    const SSEmpirical eb = ss_aggregate(P2, 2000);
    bool shape_eq = ea.shape.trough_u == eb.shape.trough_u &&
                    ea.shape.hump_u == eb.shape.hump_u &&
                    ea.shape.zero_u == eb.shape.zero_u;
    CHECK(shape_eq);

    // R3 no-peeking: an incomplete (checkpoint) file is REFUSED for analysis, accepted
    // only to RESUME (allow_incomplete=true).
    SSPartialsMeta cmeta = meta;
    cmeta.complete = false;
    const std::string ckpt = "ss_partials_ckpt.tmp";
    write_ss_partials(ckpt, P, cmeta);
    bool refused = false;
    try {
        SSPartialsMeta mx;
        (void)read_ss_partials(ckpt, mx, /*allow_incomplete=*/false);
    } catch (const std::exception&) {
        refused = true;
    }
    CHECK(refused);
    // Same file loads fine when resuming.
    SSPartialsMeta mr;
    const SSPartials Pr = read_ss_partials(ckpt, mr, /*allow_incomplete=*/true);
    CHECK(Pr.A.size() == P.A.size());
    CHECK(mr.complete == false);

    std::remove(path.c_str());
    std::remove(ckpt.c_str());
}
