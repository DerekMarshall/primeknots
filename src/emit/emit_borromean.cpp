#include "emit/emit_borromean.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "core/primes.h"
#include "emit/json.h"
#include "redei/redei_symbol.h"
#include "symbols/legendre.h"

namespace at::emit {

using at::core::u64;
namespace fs = std::filesystem;

void emit_stage2(const std::string& out_dir, u64 prime_bound,
                 const std::string& generated_by) {
    std::vector<u64> primes;
    for (u64 n = 5; n <= prime_bound; n += 4)
        if (at::core::is_prime(n)) primes.push_back(n);

    auto unlinked = [](u64 a, u64 b) {
        return at::symbols::legendre_euler(a, b) == 1;
    };

    struct Triple { u64 a, b, c; int s; };
    std::vector<Triple> triples;
    u64 borromean = 0;
    for (std::size_t i = 0; i < primes.size(); ++i)
        for (std::size_t j = i + 1; j < primes.size(); ++j) {
            if (!unlinked(primes[i], primes[j])) continue;
            for (std::size_t k = j + 1; k < primes.size(); ++k) {
                if (unlinked(primes[i], primes[k]) && unlinked(primes[j], primes[k])) {
                    int s = at::redei::redei_symbol(primes[i], primes[j], primes[k]);
                    triples.push_back({primes[i], primes[j], primes[k], s});
                    if (s == -1) ++borromean;
                }
            }
        }

    fs::create_directories(out_dir);
    std::ofstream f(fs::path(out_dir) / "borromean.json");
    if (!f) throw std::runtime_error("emit: cannot open borromean.json");

    f << "{\n";
    f << "  \"schema\": \"borromean/1\",\n";
    f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
    f << "  \"params\": {\"prime_bound\": " << prime_bound
      << ", \"residue_class\": \"1 mod 4\", \"n_primes\": " << primes.size()
      << ", \"n_valid_triples\": " << triples.size()
      << ", \"n_borromean\": " << borromean << "},\n";

    // Provenance: the symbol values are backed by the (green) Stage 2 suite.
    f << "  \"provenance\": {\"symbol\": \"src/redei/redei_symbol\", "
         "\"verified_by\": [\"invariance_redei_solution_choice\", "
         "\"invariance_sqrt_branch\", \"theorem_redei_reciprocity_s3\", "
         "\"anchor_redei_13_61_937_is_minus_1\"]},\n";

    f << "  \"primes\": [";
    for (std::size_t i = 0; i < primes.size(); ++i) f << (i ? "," : "") << primes[i];
    f << "],\n";

    // Every pairwise-unlinked triple with its symbol; the viewer filters s==-1.
    f << "  \"triples\": [";
    for (std::size_t i = 0; i < triples.size(); ++i) {
        const Triple& t = triples[i];
        f << (i ? "," : "") << "{\"p\": [" << t.a << "," << t.b << "," << t.c
          << "], \"s\": " << t.s << "}";
    }
    f << "]\n";
    f << "}\n";
}

}  // namespace at::emit
