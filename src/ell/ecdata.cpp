#include "ell/ecdata.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace at::ell {
namespace fs = std::filesystem;

const u64 kAP25Primes[25] = {2,  3,  5,  7,  11, 13, 17, 19, 23, 29, 31, 37, 41,
                             43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};

namespace {

std::vector<std::string> tokens(const std::string& line) {
    std::vector<std::string> t;
    std::istringstream ss(line);
    std::string w;
    while (ss >> w) t.push_back(w);
    return t;
}

// Parse "[a1,a2,a3,a4,a6]" (no spaces) into a Curve.
Curve parse_model(const std::string& s) {
    std::string inner = s;
    inner.erase(std::remove(inner.begin(), inner.end(), '['), inner.end());
    inner.erase(std::remove(inner.begin(), inner.end(), ']'), inner.end());
    std::istringstream ss(inner);
    std::string tok;
    long long a[5] = {0, 0, 0, 0, 0};
    for (int i = 0; i < 5 && std::getline(ss, tok, ','); ++i) a[i] = std::stoll(tok);
    return Curve{a[0], a[1], a[2], a[3], a[4]};
}

// Files in `dir` whose name starts with `prefix` (e.g. "allcurves.").
std::vector<fs::path> data_files(const std::string& dir, const std::string& prefix) {
    std::vector<fs::path> out;
    if (!fs::is_directory(dir)) return out;
    for (const auto& e : fs::directory_iterator(dir)) {
        const std::string name = e.path().filename().string();
        if (name.rfind(prefix, 0) == 0) out.push_back(e.path());
    }
    std::sort(out.begin(), out.end());
    return out;
}

}  // namespace

std::vector<EcCurve> load_ecdata_range(const std::string& dir, u64 N1, u64 N2) {
    // Pass 1 — allcurves: the NUM==1 representative of each class in [N1,N2].
    std::map<std::string, EcCurve> by_label;   // "11a" -> EcCurve
    const auto acs = data_files(dir, "allcurves.");
    if (acs.empty())
        throw std::runtime_error("load_ecdata_range: no allcurves.* under " + dir +
                                 " (run oracle/fetch_ecdata.py)");
    for (const auto& path : acs) {
        std::ifstream f(path);
        std::string line;
        while (std::getline(f, line)) {
            const auto t = tokens(line);
            if (t.size() < 6 || t[2] != "1") continue;   // representative only
            const u64 N = std::stoull(t[0]);
            if (N < N1 || N > N2) continue;
            EcCurve c;
            c.N = N;
            c.label = t[0] + t[1];
            c.model = parse_model(t[3]);
            c.rank = std::stoi(t[4]);
            by_label.emplace(c.label, std::move(c));
        }
    }

    // Pass 2 — aplist: bad-prime Atkin–Lehner signs (AP25 for p<100, BQ for q>100).
    for (const auto& path : data_files(dir, "aplist.")) {
        std::ifstream f(path);
        std::string line;
        while (std::getline(f, line)) {
            const auto t = tokens(line);
            if (t.size() < 2) continue;
            const u64 N = std::stoull(t[0]);
            if (N < N1 || N > N2) continue;
            auto it = by_label.find(t[0] + t[1]);
            if (it == by_label.end()) continue;
            EcCurve& c = it->second;
            // AP25 = the 25 tokens t[2..26]; a '+'/'-' marks a bad prime < 100.
            for (int i = 0; i < 25 && 2 + i < static_cast<int>(t.size()); ++i) {
                const std::string& tok = t[2 + i];
                if (tok == "+") c.bad_w[kAP25Primes[i]] = +1;
                else if (tok == "-") c.bad_w[kAP25Primes[i]] = -1;
            }
            // BQ = trailing "±(q)" tokens for bad primes q > 100.
            for (std::size_t j = 27; j < t.size(); ++j) {
                const std::string& tok = t[j];
                const auto lp = tok.find('('), rp = tok.find(')');
                if ((tok[0] == '+' || tok[0] == '-') && lp != std::string::npos &&
                    rp != std::string::npos && rp > lp + 1) {
                    const u64 q = std::stoull(tok.substr(lp + 1, rp - lp - 1));
                    c.bad_w[q] = (tok[0] == '+') ? +1 : -1;
                }
            }
        }
    }

    std::vector<EcCurve> out;
    out.reserve(by_label.size());
    for (auto& [label, c] : by_label) {
        assert_minimal(c.model, c.N);   // C1 precondition, enforced at load
        out.push_back(std::move(c));
    }
    return out;
}

}  // namespace at::ell
