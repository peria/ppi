#include <algorithm>
#include <cinttypes>
#include <cstdio>
#include <iostream>
#include <random>

#include "base/base.h"
#include "base/timer.h"
#include "fmt/dft.h"
#include "fmt/rft.h"

namespace {
double* a = nullptr;
double* b = nullptr;
}  // namespace

namespace ppi {
namespace fmt {

double GetRoundingError(int64 k, std::mt19937_64& rng) {
  static const int64 kMask15Bits = (1 << 15) - 1;

  for (int64 i = 0; i < k; ++i) {
    a[i] = rng() & kMask15Bits;
    b[i] = rng() & kMask15Bits;
  }

  Rft rft(k);
  rft.Transform(Direction::Forward, a);
  rft.Transform(Direction::Forward, b);
  a[0] *= b[0];
  a[1] *= b[1];
  for (int64 i = 1; i < k / 2; ++i) {
    double ar = a[2 * i], ai = a[2 * i + 1];
    double br = b[2 * i], bi = b[2 * i + 1];
    a[2 * i] = ar * br - ai * bi;
    a[2 * i + 1] = ar * bi + ai * br;
  }
  rft.Transform(Direction::Backward, a);

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

int main(int, char*[]) {
  std::mt19937_64 rng;
  const int64 kMaxK = 1 << 21;
  a = new double[kMaxK];
  b = new double[kMaxK];
  for (int64 k = 8; k <= 1 << 21; k *= 2) {
    ppi::base::Timer timer;
    double err = 0;
    const int64 n = (kMaxK * 8) / k;
    for (int64 i = 0; i < n; ++i)
      err += ppi::fmt::GetRoundingError(k / 2, rng);
    timer.Stop();

    std::printf("%10" PRId64 "\t%.3e\t%" PRId64 " ms\n", k, err / n,
                timer.GetTimeInMs() / n);
  }
  return 0;
}
