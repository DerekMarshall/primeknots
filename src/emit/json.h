#pragma once
#include <cstdint>
#include <string>
#include <vector>

// Minimal JSON-emission helpers. src/ carries no third-party JSON library
// (CLAUDE.md), and writing is simple enough to do by hand with ostreams; these
// cover the two fiddly bits: string escaping and base64.
namespace at::emit {

// Escape a string for inclusion inside JSON double quotes.
inline std::string json_escape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 2);
    for (char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    static const char* hex = "0123456789abcdef";
                    out += "\\u00";
                    out += hex[(c >> 4) & 0xF];
                    out += hex[c & 0xF];
                } else {
                    out += c;
                }
        }
    }
    return out;
}

// Standard base64 (RFC 4648) of a byte buffer.
inline std::string base64(const std::vector<std::uint8_t>& in) {
    static const char* T =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve((in.size() + 2) / 3 * 4);
    std::size_t i = 0;
    for (; i + 3 <= in.size(); i += 3) {
        std::uint32_t n = (in[i] << 16) | (in[i + 1] << 8) | in[i + 2];
        out += T[(n >> 18) & 63];
        out += T[(n >> 12) & 63];
        out += T[(n >> 6) & 63];
        out += T[n & 63];
    }
    if (std::size_t rem = in.size() - i; rem == 1) {
        std::uint32_t n = in[i] << 16;
        out += T[(n >> 18) & 63];
        out += T[(n >> 12) & 63];
        out += "==";
    } else if (rem == 2) {
        std::uint32_t n = (in[i] << 16) | (in[i + 1] << 8);
        out += T[(n >> 18) & 63];
        out += T[(n >> 12) & 63];
        out += T[(n >> 6) & 63];
        out += '=';
    }
    return out;
}

}  // namespace at::emit
