#pragma once

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Stage 6 fixture: parse the PARI referee cache data/cubic/s3_counts.txt
// (line format: "primes_csv | disc_bound | c | all_real | discs_csv", '#'
// comments skipped). Path injected via AT_CUBIC_CACHE.

#ifndef AT_CUBIC_CACHE
#define AT_CUBIC_CACHE "data/cubic/s3_counts.txt"
#endif

namespace at::verify::stage6 {

struct CubicInstance {
    std::vector<std::uint64_t> primes;
    long long disc_bound = 0;
    long long c = 0;
    bool all_real = true;
    std::vector<long long> discs;
};

inline std::vector<long long> parse_csv_ll(const std::string& s) {
    std::vector<long long> v;
    std::stringstream ss(s);
    std::string tok;
    while (std::getline(ss, tok, ',')) {
        // trim spaces
        std::size_t a = tok.find_first_not_of(" \t");
        if (a == std::string::npos) continue;
        std::size_t b = tok.find_last_not_of(" \t");
        v.push_back(std::stoll(tok.substr(a, b - a + 1)));
    }
    return v;
}

// Returns empty vector if the cache is absent (caller decides SKIP vs fail).
inline std::vector<CubicInstance> load_cubic_cache() {
    std::vector<CubicInstance> out;
    std::ifstream f(AT_CUBIC_CACHE);
    if (!f) return out;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string p;
        while (std::getline(ss, p, '|')) parts.push_back(p);
        if (parts.size() < 4) continue;
        CubicInstance ci;
        for (long long x : parse_csv_ll(parts[0]))
            ci.primes.push_back(static_cast<std::uint64_t>(x));
        ci.disc_bound = std::stoll(parts[1]);
        ci.c = std::stoll(parts[2]);
        ci.all_real = (std::stoll(parts[3]) != 0);
        if (parts.size() >= 5) ci.discs = parse_csv_ll(parts[4]);
        out.push_back(ci);
    }
    return out;
}

}  // namespace at::verify::stage6
