#include "oracle/pari.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>

#include <unistd.h>

namespace fs = std::filesystem;

namespace oracle {

std::optional<std::string> find_gp() {
    const char* path = std::getenv("PATH");
    if (path == nullptr) return std::nullopt;

    std::string p(path);
    size_t start = 0;
    while (start <= p.size()) {
        size_t colon = p.find(':', start);
        size_t len = (colon == std::string::npos) ? std::string::npos : colon - start;
        std::string dir = p.substr(start, len);
        if (!dir.empty()) {
            std::error_code ec;
            fs::path cand = fs::path(dir) / "gp";
            if (fs::exists(cand, ec) && !fs::is_directory(cand, ec) &&
                ::access(cand.c_str(), X_OK) == 0) {
                return cand.string();
            }
        }
        if (colon == std::string::npos) break;
        start = colon + 1;
    }
    return std::nullopt;
}

std::string run_gp(const std::string& gp_path, const std::string& script) {
    // Write the batch to a temp file and run `gp -q <file>`. Using the resolved
    // absolute gp_path means the invocation does not depend on $PATH (so tests
    // that scrub $PATH to simulate absence are exercising find_gp, not this).
    char tmpl[] = "/tmp/at_gp_XXXXXX";
    int fd = ::mkstemp(tmpl);
    if (fd < 0) throw std::runtime_error("oracle: mkstemp failed");
    {
        ssize_t w = ::write(fd, script.data(), script.size());
        ::close(fd);
        if (w < 0 || static_cast<size_t>(w) != script.size()) {
            ::unlink(tmpl);
            throw std::runtime_error("oracle: failed writing gp script");
        }
    }

    // </dev/null is load-bearing: with -q <file>, gp runs the script then drops to
    // INTERACTIVE mode and blocks reading stdin. popen("r") leaves gp's stdin inherited
    // from this process, which in a detached/background context never reaches EOF — gp
    // then hangs at ~0% CPU forever (observed on the 9014-curve batch; smaller batches
    // slipped through only because their inherited stdin happened to EOF). Redirecting
    // stdin from /dev/null gives that interactive read an immediate EOF, so gp exits
    // cleanly regardless of how this process was launched. No oracle call feeds gp via
    // stdin, so this is safe for every batch.
    std::string cmd = "'" + gp_path + "' -q '" + tmpl + "' </dev/null 2>/dev/null";
    FILE* pipe = ::popen(cmd.c_str(), "r");
    if (pipe == nullptr) {
        ::unlink(tmpl);
        throw std::runtime_error("oracle: popen failed");
    }

    std::string out;
    std::array<char, 4096> buf;
    size_t n;
    while ((n = std::fread(buf.data(), 1, buf.size(), pipe)) > 0) {
        out.append(buf.data(), n);
    }
    int rc = ::pclose(pipe);
    ::unlink(tmpl);
    if (rc != 0) throw std::runtime_error("oracle: gp exited nonzero");
    return out;
}

}  // namespace oracle
