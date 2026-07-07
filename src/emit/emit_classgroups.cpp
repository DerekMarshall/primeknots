#include "emit/emit_classgroups.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <span>
#include <stdexcept>
#include <vector>

#include "classgroup/class_group.h"
#include "classgroup/redei_matrix.h"
#include "core/primes.h"
#include "emit/json.h"
#include "linking/linking_matrix.h"

namespace at::emit {

using at::core::i128;
using at::core::u64;
namespace fs = std::filesystem;

namespace {
constexpr u64 kPoolBound = 200;
constexpr int kMaxT = 4;

i128 isqrt_i128(i128 n) {
    if (n <= 0) return 0;
    i128 x = static_cast<i128>(std::sqrt(static_cast<double>(n)));
    while (x > 0 && x * x > n) --x;
    while ((x + 1) * (x + 1) <= n) ++x;
    return x;
}

// Negative Pell x²−Dy²=−1 solvable ⇔ N(ε)=−1 ⇔ CF period of √D is odd.
bool negative_pell(i128 D) {
    i128 a0 = isqrt_i128(D);
    if (a0 * a0 == D) return false;  // perfect square (never, for our D)
    i128 m = 0, d = 1, a = a0;
    int period = 0;
    do {
        m = d * a - m;
        d = (D - m * m) / d;
        a = (a0 + m) / d;
        ++period;
    } while (a != 2 * a0);
    return (period % 2) == 1;
}

struct Member { i128 D; std::vector<u64> primes; };
void gen(const std::vector<u64>& P, std::size_t start, std::vector<u64>& cur,
         i128 bound, std::vector<Member>& out) {
    if (!cur.empty()) {
        i128 d = 1;
        for (u64 p : cur) d *= p;
        out.push_back({d, cur});
    }
    if (static_cast<int>(cur.size()) >= kMaxT) return;
    for (std::size_t i = start; i < P.size(); ++i) {
        i128 d = 1;
        for (u64 p : cur) d *= p;
        if (d * static_cast<i128>(P[i]) > bound) continue;
        cur.push_back(P[i]);
        gen(P, i + 1, cur, bound, out);
        cur.pop_back();
    }
}
}  // namespace

void emit_stage3(const std::string& out_dir, i128 disc_bound,
                 const std::string& generated_by) {
    at::linking::LinkingMatrix LM = at::linking::LinkingMatrix::build(kPoolBound);
    std::vector<u64> P;
    for (u64 n = 5; n <= kPoolBound; n += 4)
        if (at::core::is_prime(n)) P.push_back(n);
    std::vector<Member> mem;
    std::vector<u64> cur;
    gen(P, 0, cur, disc_bound, mem);

    fs::create_directories(out_dir);
    std::ofstream f(fs::path(out_dir) / "classgroups.json");
    if (!f) throw std::runtime_error("emit: cannot open classgroups.json");
    f.setf(std::ios::fixed);

    // First pass: compute, accumulate aggregates.
    struct Row { i128 D; std::size_t t, r2, r4f, r4l; i128 h, ho; bool pell; std::vector<i128> inv; };
    std::vector<Row> rows;
    i128 pell_count = 0, all_match = 1, s0 = 0, s1 = 0, s2 = 0;
    for (const Member& m : mem) {
        at::classgroup::ClassGroup cg = at::classgroup::ClassGroup::compute(m.D);
        std::span<const u64> ps(m.primes.data(), m.primes.size());
        std::size_t r4f = cg.rank4();
        std::size_t r4l = at::classgroup::fourrank_from_linking(ps, LM);
        bool pell = negative_pell(m.D);
        i128 h = cg.order();
        rows.push_back({m.D, m.primes.size(), cg.rank2(), r4f, r4l, h,
                        pell ? h : h / 2, pell, cg.invariant_factors()});
        if (pell) ++pell_count;
        if (r4f != r4l) all_match = 0;
        if (r4l == 0) ++s0; else if (r4l == 1) ++s1; else ++s2;
    }

    f << "{\n  \"schema\": \"classgroups/1\",\n";
    f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
    f << "  \"params\": {\"prime_pool_bound\": " << kPoolBound
      << ", \"disc_bound\": " << (long long)disc_bound << ", \"max_t\": " << kMaxT
      << ", \"residue_class\": \"1 mod 4\", \"n_discriminants\": " << rows.size()
      << "},\n";
    double pell_frac = rows.empty() ? 0.0 : (double)pell_count / (double)rows.size();
    f.precision(6);
    f << "  \"aggregate\": {\"all_4rank_match\": " << (all_match ? "true" : "false")
      << ", \"strata\": {\"r4_0\": " << (long long)s0 << ", \"r4_1\": " << (long long)s1
      << ", \"r4_ge2\": " << (long long)s2 << "}, \"negative_pell_count\": "
      << (long long)pell_count << ", \"negative_pell_fraction\": " << pell_frac
      << "},\n";
    // R2 note in-band: pell fraction is an unasserted empiric (S22 conj ≈ 0.5806).
    f << "  \"discriminants\": [";
    for (std::size_t i = 0; i < rows.size(); ++i) {
        const Row& r = rows[i];
        f << (i ? ",\n    " : "\n    ") << "{\"D\": " << (long long)r.D << ", \"t\": "
          << r.t << ", \"h_narrow\": " << (long long)r.h << ", \"h_ordinary\": "
          << (long long)r.ho << ", \"rank2\": " << r.r2 << ", \"rank4_form\": "
          << r.r4f << ", \"rank4_link\": " << r.r4l << ", \"match\": "
          << (r.r4f == r.r4l ? "true" : "false") << ", \"pell_neg\": "
          << (r.pell ? "true" : "false") << ", \"invariants\": [";
        for (std::size_t k = 0; k < r.inv.size(); ++k)
            f << (k ? "," : "") << (long long)r.inv[k];
        f << "]}";
    }
    f << "\n  ]\n}\n";
}

}  // namespace at::emit
