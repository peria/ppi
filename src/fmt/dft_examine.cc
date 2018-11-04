#include "fmt/dft.h"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <random>

#include "base/base.h"
#include "base/time.h"
#include "fmt/fmt.h"
#include "fmt/rft.h"

namespace {
double* a = nullptr;
double* b = nullptr;
}

namespace ppi {
namespace fmt {

double GetRoundingError(int64 k, std::mt19937_64& rng) {
  static const int64 kMask15Bits = (1 << 15) - 1;

  for (int64 i = 0; i < k; ++i) {
    a[i] = rng() & kMask15Bits;
    b[i] = rng() & kMask15Bits;
  }

  Config config(k / 2);
  Rft::Transform(config, Direction::Forward, a);
  Rft::Transform(config, Direction::Forward, b);
  a[0] *= b[0];
  a[1] *= b[1];
  for (int64 i = 1; i < k / 2; ++i) {
    double ar = a[2 * i], ai = a[2 * i + 1];
    double br = b[2 * i], bi = b[2 * i + 1];
    a[2 * i] = ar * br - ai * bi;
    a[2 * i + 1] = ar * bi + ai * br;
  }
  Rft::Transform(config, Direction::Backward, a);

  double err = 0;
  for (int64 i = 0; i < k; ++i) {
    double d = std::floor(a[i] + 0.5);
    err = std::max(err, std::abs(a[i] - d));
  }

  return err;
}

}  // namespace fmt
}  // namespace ppi

using ppi::int64;

int main(int, char* []) {
  std::mt19937_64 rng;
  const int64 kMaxK = 1 << 21;
  a = new double[kMaxK];
  b = new double[kMaxK];
  for (int64 k = 8; k <= 1 << 21; k *= 2) {
    double start = ppi::base::Time::Now();
    double err = 0;
    const int64 n = (kMaxK * 8) / k;
    for (int64 i = 0; i < n; ++i)
      err += ppi::fmt::GetRoundingError(k / 2, rng);
    double end = ppi::base::Time::Now();

    std::printf("%10ld\t%.3e\t%.3f ms\n", k, err / n, (end - start) * 1e+3 / n);
  }
  return 0;
}
