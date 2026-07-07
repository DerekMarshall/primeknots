#include "emit/emit_linking.h"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "emit/json.h"
#include "linking/linking_matrix.h"

namespace at::emit {

using at::core::u64;
using at::linking::LinkingMatrix;
namespace fs = std::filesystem;

namespace {

// One row of the F2 matrix as ceil(n/8) little-endian bytes: column j is bit
// (j % 8) of byte (j / 8).
std::vector<std::uint8_t> row_bytes(const at::linking::F2Matrix& m,
                                    std::size_t i, std::size_t n) {
    std::vector<std::uint8_t> bytes((n + 7) / 8, 0);
    for (std::size_t j = 0; j < n; ++j) {
        if (m.get(i, j)) bytes[j >> 3] |= static_cast<std::uint8_t>(1u << (j & 7));
    }
    return bytes;
}

std::ofstream open_out(const std::string& out_dir, const char* name) {
    fs::create_directories(out_dir);
    std::ofstream f(fs::path(out_dir) / name);
    if (!f) throw std::runtime_error(std::string("emit: cannot open ") + name);
    f.setf(std::ios::fixed);
    return f;
}

}  // namespace

void emit_stage1(const std::string& out_dir, u64 prime_bound,
                 std::size_t graph_nodes, const std::string& generated_by) {
    LinkingMatrix M = LinkingMatrix::build(prime_bound);
    const std::size_t n = M.primes.size();

    // ---- linking_matrix.json ----
    {
        std::ofstream f = open_out(out_dir, "linking_matrix.json");
        f << "{\n";
        f << "  \"schema\": \"linking_matrix/1\",\n";
        f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
        f << "  \"params\": {\"prime_bound\": " << prime_bound
          << ", \"n_primes\": " << n << ", \"residue_class\": \"1 mod 4\"},\n";
        f << "  \"n\": " << n << ",\n";
        f << "  \"row_bytes\": " << (n + 7) / 8 << ",\n";

        f << "  \"primes\": [";
        for (std::size_t i = 0; i < n; ++i) f << (i ? "," : "") << M.primes[i];
        f << "],\n";

        f << "  \"rows_base64\": [";
        for (std::size_t i = 0; i < n; ++i) {
            f << (i ? "," : "") << "\"" << base64(row_bytes(M.lk, i, n)) << "\"";
        }
        f << "],\n";

        // Statistics — recorded as empirics, asserted by no test (reviewer).
        std::size_t ones = M.lk.popcount();          // all off-diagonal (diag=0)
        std::size_t total = n > 1 ? n * (n - 1) : 0;  // ordered off-diagonal
        std::size_t dmin = n ? n : 0, dmax = 0, dsum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            std::size_t d = M.lk.row_popcount(i);
            dmin = std::min(dmin, d);
            dmax = std::max(dmax, d);
            dsum += d;
        }
        double density = total ? static_cast<double>(ones) / total : 0.0;
        double dmean = n ? static_cast<double>(dsum) / n : 0.0;
        f.precision(6);
        f << "  \"stats\": {\"off_diagonal_ones\": " << ones
          << ", \"off_diagonal_total\": " << total << ", \"density\": " << density
          << ", \"row_degree_min\": " << (n ? dmin : 0)
          << ", \"row_degree_max\": " << dmax << ", \"row_degree_mean\": " << dmean
          << "}\n";
        f << "}\n";
    }

    // ---- linking_graph.json ----
    {
        std::size_t g = std::min(graph_nodes, n);  // thresholded prime range
        std::ofstream f = open_out(out_dir, "linking_graph.json");
        f << "{\n";
        f << "  \"schema\": \"linking_graph/1\",\n";
        f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
        f << "  \"params\": {\"prime_bound\": " << prime_bound
          << ", \"n_primes\": " << n << ", \"graph_nodes\": " << g
          << ", \"edge_rule\": \"linked pairs: (p_i/p_j) = -1\"},\n";

        f << "  \"nodes\": [";
        for (std::size_t i = 0; i < g; ++i) {
            f << (i ? "," : "") << "{\"id\": " << i << ", \"prime\": " << M.primes[i]
              << ", \"degree\": " << M.lk.row_popcount(i) << "}";
        }
        f << "],\n";

        f << "  \"edges\": [";
        bool first = true;
        for (std::size_t i = 0; i < g; ++i) {
            for (std::size_t j = i + 1; j < g; ++j) {
                if (M.lk.get(i, j)) {
                    f << (first ? "" : ",") << "{\"source\": " << i
                      << ", \"target\": " << j << "}";
                    first = false;
                }
            }
        }
        f << "]\n";
        f << "}\n";
    }
}

}  // namespace at::emit
