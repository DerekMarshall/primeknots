#pragma once
#include <chrono>
#include <cstdio>
#include <string>
#include <utility>

// Live progress for long-running verify loops — a STANDING convention (2026-07-16): every
// long M0b/at-scale loop reports done/total + rate + ETA to stderr so a run is never blind
// (no more external rate-probing). Tailable; zero cost when not ticking. Header-only.
namespace at::verify {

class Progress {
  public:
    Progress(std::string label, long long total, long long step = 0)
        : label_(std::move(label)), total_(total),
          step_(step > 0 ? step : (total > 0 ? total / 100 + 1 : 1)),
          t0_(std::chrono::steady_clock::now()) {}

    // Report at most once per `step` items advanced. `detail` (optional) is appended
    // (e.g. a secondary count like a_p evaluated).
    void tick(long long done, long long detail = -1) {
        if (done - last_ < step_ && done < total_) return;
        last_ = done;
        const double el = elapsed();
        const double rate = el > 0 ? done / el : 0.0;
        const double eta = (rate > 0 && total_ > done) ? (total_ - done) / rate : 0.0;
        if (detail >= 0)
            std::fprintf(stderr, "  %s: %lld/%lld (%.1f%%)  %lld done  %.0fs  %.0f/s  ETA %.0fs\n",
                         label_.c_str(), done, total_, 100.0 * done / (total_ ? total_ : 1),
                         detail, el, rate, eta);
        else
            std::fprintf(stderr, "  %s: %lld/%lld (%.1f%%)  %.0fs  %.0f/s  ETA %.0fs\n",
                         label_.c_str(), done, total_, 100.0 * done / (total_ ? total_ : 1),
                         el, rate, eta);
    }

    double elapsed() const {
        return std::chrono::duration<double>(std::chrono::steady_clock::now() - t0_).count();
    }

  private:
    std::string label_;
    long long total_;
    long long step_;
    long long last_ = 0;
    std::chrono::steady_clock::time_point t0_;
};

}  // namespace at::verify
