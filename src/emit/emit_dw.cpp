#include "emit/emit_dw.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "core/int_types.h"
#include "dw/s3_count.h"
#include "emit/json.h"

namespace at::emit {
namespace fs = std::filesystem;
using at::core::u64;

namespace {
struct Row {
    std::vector<u64> primes;
    long long disc_bound = 0, c = 0;
    bool all_real = true;
    std::vector<long long> discs;
};

std::vector<long long> csv_ll(const std::string& s) {
    std::vector<long long> v;
    std::stringstream ss(s);
    std::string tok;
    while (std::getline(ss, tok, ',')) {
        std::size_t a = tok.find_first_not_of(" \t");
        if (a == std::string::npos) continue;
        std::size_t b = tok.find_last_not_of(" \t");
        v.push_back(std::stoll(tok.substr(a, b - a + 1)));
    }
    return v;
}

std::vector<Row> read_cache(const std::string& path) {
    std::vector<Row> out;
    std::ifstream f(path);
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string p;
        while (std::getline(ss, p, '|')) parts.push_back(p);
        if (parts.size() < 4) continue;
        Row r;
        for (long long x : csv_ll(parts[0])) r.primes.push_back(static_cast<u64>(x));
        r.disc_bound = std::stoll(parts[1]);
        r.c = std::stoll(parts[2]);
        r.all_real = std::stoll(parts[3]) != 0;
        if (parts.size() >= 5) r.discs = csv_ll(parts[4]);
        out.push_back(r);
    }
    return out;
}
}  // namespace

void emit_stage6(const std::string& out_dir, const std::string& cubic_cache_path,
                 const std::string& generated_by) {
    fs::create_directories(out_dir);
    std::vector<Row> rows = read_cache(cubic_cache_path);

    bool all_real = true;
    long long max_t = 0;
    for (const Row& r : rows) {
        if (!r.all_real) all_real = false;
        max_t = std::max<long long>(max_t, static_cast<long long>(r.primes.size()));
    }

    std::ofstream f(fs::path(out_dir) / "dw_s3.json");
    f << "{\n  \"schema\": \"dw_s3/1\",\n";
    f << "  \"generated_by\": \"" << json_escape(generated_by) << "\",\n";
    f << "  \"params\": {\"group\": \"S3\", \"space\": \"Spec Z[1/S]\""
      << ", \"pool\": \"primes = 1 mod 4\""
      << ", \"convention\": \"unconstrained infinity (both signatures); all totally real by R3\""
      << ", \"bound_rule\": \"|disc| <= prod p^2\""
      << ", \"referee\": \"PARI/GP nflist; LMFDB twin pending API access\"},\n";
    f << "  \"aggregate\": {\"n_instances\": " << rows.size()
      << ", \"all_totally_real\": " << (all_real ? "true" : "false")
      << ", \"max_t\": " << max_t << "},\n";
    f << "  \"instances\": [";
    for (std::size_t i = 0; i < rows.size(); ++i) {
        const Row& r = rows[i];
        int t = static_cast<int>(r.primes.size());
        int k = at::dw::cyclic_k(r.primes);
        long long H = at::dw::hom_count(t, k, r.c);
        at::dw::Rational z = at::dw::z_dw(t, k, r.c);
        f << (i ? ",\n    " : "\n    ") << "{\"primes\": [";
        for (std::size_t j = 0; j < r.primes.size(); ++j)
            f << (j ? "," : "") << r.primes[j];
        f << "], \"t\": " << t << ", \"k\": " << k << ", \"c\": " << r.c
          << ", \"hom_count\": " << H
          << ", \"z_dw\": {\"num\": " << z.num << ", \"den\": " << z.den << "}"
          << ", \"terms\": {\"trivial\": 1, \"c2\": " << at::dw::c2_term(t)
          << ", \"c3\": " << at::dw::c3_term(k) << ", \"s3\": " << at::dw::s3_term(r.c) << "}"
          << ", \"disc_bound\": " << r.disc_bound
          << ", \"all_real\": " << (r.all_real ? "true" : "false")
          << ", \"discs\": [";
        for (std::size_t j = 0; j < r.discs.size(); ++j) f << (j ? "," : "") << r.discs[j];
        f << "]}";
    }
    f << "\n  ]\n}\n";
}

}  // namespace at::emit
