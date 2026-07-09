#include "ell/ap_cache.h"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <stdexcept>

// The generator hash is defined by CMake (sha256 of ell/ap_fast.cpp). The fallback
// only fires if the build system did not set it; a cache written under the fallback
// is still self-consistent, but the source-tracking guarantee needs the CMake value.
#ifndef AP_GENERATOR_HASH
#define AP_GENERATOR_HASH "AP_GENERATOR_HASH-undefined"
#endif

namespace at::ell {

namespace {
constexpr char kMagic[4] = {'A', 'P', 'C', '1'};

void put_u32(std::ostream& os, uint32_t v) { os.write(reinterpret_cast<const char*>(&v), 4); }
void put_u64(std::ostream& os, uint64_t v) { os.write(reinterpret_cast<const char*>(&v), 8); }
void put_str(std::ostream& os, const std::string& s) {
    put_u64(os, s.size());
    os.write(s.data(), static_cast<std::streamsize>(s.size()));
}
uint32_t get_u32(std::istream& is) { uint32_t v = 0; is.read(reinterpret_cast<char*>(&v), 4); return v; }
uint64_t get_u64(std::istream& is) { uint64_t v = 0; is.read(reinterpret_cast<char*>(&v), 8); return v; }
std::string get_str(std::istream& is) {
    uint64_t n = get_u64(is);
    std::string s(n, '\0');
    is.read(s.data(), static_cast<std::streamsize>(n));
    return s;
}
}  // namespace

const char* ap_generator_hash() { return AP_GENERATOR_HASH; }

void write_ap_cache(const std::string& path, const ApCacheHeader& header,
                    const std::vector<int16_t>& values) {
    std::ofstream os(path, std::ios::binary);
    if (!os) throw std::runtime_error("write_ap_cache: cannot open " + path);
    os.write(kMagic, 4);
    put_u32(os, header.format_version);
    put_str(os, header.generator_hash);
    put_str(os, header.ecdata_sha);
    put_u64(os, header.prime_bound);
    put_str(os, header.curve_set);
    put_u64(os, values.size());
    os.write(reinterpret_cast<const char*>(values.data()),
             static_cast<std::streamsize>(values.size() * sizeof(int16_t)));
    if (!os) throw std::runtime_error("write_ap_cache: write failed for " + path);
}

std::vector<int16_t> read_ap_cache(const std::string& path, ApCacheHeader& out_header) {
    std::ifstream is(path, std::ios::binary);
    if (!is) throw std::runtime_error("read_ap_cache: cannot open " + path);

    char magic[4];
    is.read(magic, 4);
    if (std::memcmp(magic, kMagic, 4) != 0)
        throw std::runtime_error("read_ap_cache: bad magic (not an a_p cache): " + path);

    out_header.format_version = get_u32(is);
    if (out_header.format_version != kApCacheFormatVersion)
        throw std::runtime_error("read_ap_cache: unsupported format version");

    out_header.generator_hash = get_str(is);
    out_header.ecdata_sha = get_str(is);
    out_header.prime_bound = get_u64(is);
    out_header.curve_set = get_str(is);

    // The refusal that matters: a cache whose generator differs from HEAD's is
    // stale and is NEVER silently reused (ARCHITECTURE-M §4).
    if (out_header.generator_hash != ap_generator_hash())
        throw std::runtime_error(
            "read_ap_cache: REFUSED — stale a_p cache: generator hash " +
            out_header.generator_hash + " != current " + ap_generator_hash() +
            " (regenerate the cache; never reuse a stale one)");

    const uint64_t n = get_u64(is);
    std::vector<int16_t> values(n);
    is.read(reinterpret_cast<char*>(values.data()),
            static_cast<std::streamsize>(n * sizeof(int16_t)));
    if (!is) throw std::runtime_error("read_ap_cache: truncated value block in " + path);
    return values;
}

}  // namespace at::ell
