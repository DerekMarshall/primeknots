#include "ell/ap_fast.h"

#include <algorithm>
#include <atomic>
#include <thread>

namespace at::ell {

using at::core::i64;
using at::core::u128;

QRTable build_qr_table(u64 p) {
    QRTable t;
    t.p = p;
    t.chi.assign(p, -1);          // non-residues default to −1
    t.chi[0] = 0;                 // (0/p) = 0
    for (u64 x = 1; x < p; ++x) {
        t.chi[static_cast<u64>(static_cast<u128>(x) * x % p)] = 1;  // squares are residues
    }
    return t;
}

int ap_fast(const Curve& E, const QRTable& qr) {
    const u64 p = qr.p;
    auto rp = [p](i64 v) -> u64 {
        i64 m = v % static_cast<i64>(p);
        return static_cast<u64>(m < 0 ? m + static_cast<i64>(p) : m);
    };
    auto mul = [p](u64 a, u64 b) -> u64 { return static_cast<u64>(static_cast<u128>(a) * b % p); };
    auto add = [p](u64 a, u64 b) -> u64 { return (a + b) % p; };
    auto sub = [p](u64 a, u64 b) -> u64 { return (a + p - b) % p; };

    const u64 a1 = rp(E.a1), a2 = rp(E.a2), a3 = rp(E.a3), a4 = rp(E.a4), a6 = rp(E.a6);
    const u64 B2 = add(mul(a1, a1), mul(4 % p, a2));
    const u64 B4 = add(mul(2 % p, a4), mul(a1, a3));
    const u64 B6 = add(mul(a3, a3), mul(4 % p, a6));
    const u64 C4 = sub(mul(B2, B2), mul(24 % p, B4));
    const u64 B2cube = mul(mul(B2, B2), B2);
    const u64 C6 = sub(add(sub(0, B2cube), mul(36 % p, mul(B2, B4))), mul(216 % p, B6));
    const u64 A = sub(0, mul(27 % p, C4));
    const u64 B = sub(0, mul(54 % p, C6));

    long long sum = 0;
    for (u64 x = 0; x < p; ++x) {
        const u64 fx = add(add(mul(mul(x, x), x), mul(A, x)), B);
        sum += qr.chi[fx];        // table lookup instead of ap_charsum's modpow
    }
    return static_cast<int>(-sum);
}

std::vector<std::vector<int>> ap_fast_grid(const std::vector<Curve>& curves,
                                           const std::vector<u64>& primes,
                                           bool parallel) {
    std::vector<std::vector<int>> res(curves.size(),
                                      std::vector<int>(primes.size(), 0));
    // Column j (prime primes[j]) is computed independently; a thread owns whole
    // columns, so writes never overlap and the result is deterministic.
    auto do_prime = [&](size_t j) {
        const QRTable qr = build_qr_table(primes[j]);
        for (size_t i = 0; i < curves.size(); ++i) res[i][j] = ap_fast(curves[i], qr);
    };

    if (!parallel) {
        for (size_t j = 0; j < primes.size(); ++j) do_prime(j);
        return res;
    }

    const unsigned hw = std::max(2u, std::thread::hardware_concurrency());
    std::atomic<size_t> next{0};
    auto worker = [&] {
        size_t j;
        while ((j = next.fetch_add(1)) < primes.size()) do_prime(j);
    };
    std::vector<std::thread> pool;
    for (unsigned k = 0; k < hw; ++k) pool.emplace_back(worker);
    for (std::thread& th : pool) th.join();
    return res;
}

}  // namespace at::ell
