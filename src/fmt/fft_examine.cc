#include "fmt/fft.h"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <random>

#include "base/base.h"
#include "fmt/fmt.h"
#include "fmt/rft.h"

namespace ppi {
namespace fmt {

double GetRoundingError(int64 k, std::mt19937_64& rng) {
  static const int64 kMask15Bits = (1 << 16) - 1;
  double* a = new double[k];
  double* b = new double[k];

  for (int64 i = 0; i < k; ++i) {
    a[i] = rng() & kMask15Bits;
    b[i] = rng() & kMask15Bits;
  }

  Rft::Transform(Direction::Forward, k, a);
  Rft::Transform(Direction::Forward, k, b);
  a[0] *= b[0];
  a[1] *= b[1];
  for (int64 i = 1; i < k / 2; ++i) {
    double ar = a[2 * i], ai = a[2 * i + 1];
    double br = b[2 * i], bi = b[2 * i + 1];
    a[2 * i    ] = ar * br - ai * bi;
    a[2 * i + 1] = ar * bi + ai * br;
  }
  Rft::Transform(Direction::Backward, k, a);

  double err = 0;
  for (int64 i = 0; i < k; ++i) {
    double d = std::floor(a[i] + 0.5);
    err = std::max(err, std::abs(a[i] - d));
  }

  delete[] a;
  delete[] b;

  return err;
}

}  // namespace fmt
}  // namespace ppi

using ppi::int64;

int main(int, char*[]) {
  std::mt19937_64 rng;
  for (int64 k = 8; k <= 1 << 21; k *= 2) {
    double err = 0;
    int64 n = (1 << 24) / k;
    for (int64 i = 0; i < n; ++i)
      err += ppi::fmt::GetRoundingError(k / 2, rng);

    std::printf("%10ld\t%.3e\n", k, err / n);
  }
  return 0;
}
