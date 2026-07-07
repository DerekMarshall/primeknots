#include "emit/emit_cs.h"

#include <filesystem>
#include <fstream>
#include <span>
#include <stdexcept>
#include <vector>

#include "classgroup/class_group.h"  // negative_pell_solvable
#include "core/primes.h"
#include "cs/lhs.h"
#include "cs/rhs.h"
#include "emit/json.h"
#include "symbols/legendre.h"

namespace at::emit {

using at::core::i128;
using at::core::u64;
namespace fs = std::filesystem;

namespace {
constexpr int kMaxR = 4;
int lk2(u64 p, u64 q) { return at::symbols::legendre_euler(p, q) == 1 ? 0 : 1; }
// 0 = all-even, 1 = all-odd, 2 = mixed row parities.
int parity_class(const std::vector<u64>& P) {
    int r = static_cast<int>(P.size()), ev = 0, od = 0;
    for (int i = 0; i < r; ++i) {
        int s = 0;
        for (int j = 0; j < r; ++j)
            if (j != i) s ^= lk2(P[i], P[j]);
        if (s) ++od; else ++ev;
    }
    return od == 0 ? 0 : (ev == 0 ? 1 : 2);
}
struct Row { std::vector<u64> primes; int r, parity; long lhs, rhs; };
void gen(const std::vector<u64>& P, std::size_t start, std::vector<u64>& cur,
         i128 bound, std::vector<Row>& out, long& excluded) {
    if (cur.size() >= 2) {
        i128 D = 1;
        for (u64 p : cur) D *= p;
        if (D <= bound) {
            if (at::classgroup::negative_pell_solvable(D)) {
                std::span<const u64> s(cur.data(), cur.size());
                out.push_back({cur, (int)cur.size(), parity_class(cur),
                               at::cs::lhs_partition(s), at::cs::rhs_partition(s)});
            } else {
                ++excluded;
            }
        }
    }
    if ((int)cur.size() >= kMaxR) return;
    for (std::size_t i = start; i < P.size(); ++i) {
        i128 d = 1;
        for (u64 p : cur) d *= p;
        if (d * (i128)P[i] > bound) continue;
        cur.push_back(P[i]);
        gen(P, i + 1, cur, bound, out, excluded);
        cur.pop_back();
    }
}
const char* pname(int p) { return p == 0 ? "all_even" : (p == 1 ? "all_odd" : "mixed"); }
}  // namespace

void emit_stage4(const std::string& out_dir, u64 prime_bound,
                 const std::string& generated_by) {
    // prime_bound caps the discriminant D = ∏ pᵢ.
    std::vector<u64> P;
    for (u64 n = 5; n <= 200; n += 4)
        if (at::core::is_prime(n)) P.push_back(n);
    std::vector<Row> rows;
    long excluded = 0;
    std::vector<u64> cur;
    gen(P, 0, cur, static_cast<i128>(prime_bound), rows, excluded);

    long strata[2][3] = {{0, 0, 0}, {0, 0, 0}};
    bool all_match = true;
    for (const Row& r : rows) {
        strata[r.r % 2][r.parity]++;
        if (r.lhs != r.rhs) all_match = false;
    }

    fs::create_directories(out_dir);
    std::ofstream f(fs::path(out_dir) / "cs_partition.json");
    if (!f) throw std::runtime_error("emit: cannot open cs_partition.json");
    f << "{\n  \"schema\": \"cs_partition/1\",\n";
    f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
    f << "  \"params\": {\"prime_pool_bound\": 200, \"disc_bound\": " << prime_bound
      << ", \"max_r\": " << kMaxR
      << ", \"residue_class\": \"1 mod 4\", \"route\": \"Hirano mod-2 DW (N(eps)=-1)\""
      << ", \"n_included\": " << rows.size() << ", \"n_excluded_pell\": " << excluded
      << "},\n";
    f << "  \"aggregate\": {\"all_match\": " << (all_match ? "true" : "false")
      << ", \"strata\": {\"even_all_even\": " << strata[0][0] << ", \"even_all_odd\": "
      << strata[0][1] << ", \"even_mixed\": " << strata[0][2] << ", \"odd_all_even\": "
      << strata[1][0] << ", \"odd_all_odd\": " << strata[1][1] << ", \"odd_mixed\": "
      << strata[1][2] << "}},\n";
    f << "  \"instances\": [";
    for (std::size_t i = 0; i < rows.size(); ++i) {
        const Row& r = rows[i];
        f << (i ? ",\n    " : "\n    ") << "{\"primes\": [";
        for (std::size_t k = 0; k < r.primes.size(); ++k)
            f << (k ? "," : "") << r.primes[k];
        f << "], \"r\": " << r.r << ", \"z_lhs\": " << r.lhs << ", \"z_rhs\": " << r.rhs
          << ", \"match\": " << (r.lhs == r.rhs ? "true" : "false")
          << ", \"phase_class\": \"" << pname(r.parity) << "\"}";
    }
    f << "\n  ]\n}\n";
}

}  // namespace at::emit
