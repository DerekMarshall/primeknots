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

const char* kM1ExtractName = "m1_extract.txt";

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

// Read the committed derived extract, filtered to [N1,N2]. Enforces the extract's
// declared range coverage and re-checks C1 minimality on every row.
std::vector<EcCurve> read_extract(const std::string& path, u64 N1, u64 N2) {
    std::ifstream f(path);
    std::string line;
    u64 lo = 0, hi = 0;
    bool have_range = false;
    std::vector<EcCurve> out;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') {
            const auto t = tokens(line);
            if (t.size() >= 4 && t[1] == "range") {
                lo = std::stoull(t[2]); hi = std::stoull(t[3]); have_range = true;
            }
            continue;
        }
        const auto t = tokens(line);
        if (t.size() < 9) continue;
        EcCurve c;
        c.label = t[0];
        c.N = std::stoull(t[1]);
        c.model = Curve{std::stoll(t[2]), std::stoll(t[3]), std::stoll(t[4]),
                        std::stoll(t[5]), std::stoll(t[6])};
        c.rank = std::stoi(t[7]);
        if (t[8] != "-") {
            std::istringstream bs(t[8]);
            std::string kv;
            while (std::getline(bs, kv, ',')) {
                const auto colon = kv.find(':');
                if (colon != std::string::npos)
                    c.bad_w[std::stoull(kv.substr(0, colon))] = std::stoi(kv.substr(colon + 1));
            }
        }
        if (c.N >= N1 && c.N <= N2) out.push_back(std::move(c));
    }
    if (have_range && (N1 < lo || N2 > hi))
        throw std::runtime_error("read_extract: requested range [" + std::to_string(N1) + "," +
                                 std::to_string(N2) + "] exceeds extract coverage [" +
                                 std::to_string(lo) + "," + std::to_string(hi) +
                                 "]; regenerate the extract or provide the raw ecdata slices");
    for (auto& c : out) assert_minimal(c.model, c.N);
    return out;
}

}  // namespace

std::vector<EcCurve> load_ecdata_range(const std::string& dir, u64 N1, u64 N2) {
    // Prefer the committed derived extract (repo-reproducible, R2); only parse the
    // gitignored raw slices when it is absent (e.g. when regenerating the extract).
    const fs::path extract = fs::path(dir) / kM1ExtractName;
    if (fs::exists(extract)) return read_extract(extract.string(), N1, N2);
    return load_ecdata_raw(dir, N1, N2);
}

std::vector<EcCurve> load_ecdata_raw(const std::string& dir, u64 N1, u64 N2) {
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

void write_extract(const std::string& path, const std::vector<EcCurve>& curves,
                   u64 lo, u64 hi) {
    std::ofstream f(path);
    f << "# m1_extract v1 — DERIVED DATA (a Modified Version), not an original work.\n";
    f << "# Source:  Cremona ecdata, https://github.com/JohnCremona/ecdata\n";
    f << "#          release tag 2026-04-22, commit 25cec5ecfec8b9f016eb1631ac633194c2bed39f\n";
    f << "# License: Artistic License 2.0 (ecdata's LICENSE) — redistribution of Modified\n";
    f << "#          Versions is permitted with this notice retained. (c) J. Cremona and\n";
    f << "#          contributors. Raw-slice sha256s pinned in data/cremona/MANIFEST.json.\n";
    f << "# Derived by: `at ecdata-extract --lo " << lo << " --hi " << hi << "` (this repo).\n";
    f << "# Contents: one primitive-isogeny-class representative per line — the rows Stage\n";
    f << "#          M1 consumes; good-prime a_p is COMPUTED from the model, not stored.\n";
    f << "# range " << lo << " " << hi << "\n";
    f << "# LABEL N a1 a2 a3 a4 a6 RANK BAD(p:w,...|-)\n";
    for (const EcCurve& c : curves) {
        f << c.label << " " << c.N << " " << c.model.a1 << " " << c.model.a2 << " "
          << c.model.a3 << " " << c.model.a4 << " " << c.model.a6 << " " << c.rank << " ";
        if (c.bad_w.empty()) {
            f << "-";
        } else {
            bool first = true;
            for (const auto& [p, w] : c.bad_w) { f << (first ? "" : ",") << p << ":" << w; first = false; }
        }
        f << "\n";
    }
}

}  // namespace at::ell
