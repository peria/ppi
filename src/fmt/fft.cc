#include "fmt/fft.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "base/base.h"
#include "base/allocator.h"

namespace ppi {
namespace fmt {

namespace {

const int64 kMaxSize = 1 << 21;
Complex* g_work = nullptr;

Complex* WorkArea(int64 size) {
  if (base::Allocator::GetSize(g_work) < size * 2) {
    if (g_work)
      base::Allocator::Deallocate(g_work);
    g_work = base::Allocator::Allocate<Complex>(size * 2);
  }
  return g_work;
}

}  // namespace

void Fft::Factor(int64 n, Config* config) {
  const int64 kPrimes[] = {2};
  static_assert(sizeof(kPrimes) != sizeof(Config::exponent),
                "config may have unknown exponents.");

  config->n = n;
  for (size_t i = 0; i < ARRAY_SIZE(kPrimes); ++i) {
    const int64 p = kPrimes[i];
    int64 e = 0;
    for (; n % p == 0; n /= p) {
      ++e;
    }
    config->exponent[i] = e;
  }

  CHECK_LT(0, config->exponent[0]);
}

void Fft::Transform(const Config& config, const Type type, Complex a[]) {
  const int64 n = config.n;
  CHECK_GE(kMaxSize, n);
  if (type == Type::Inverse) {
    for (int64 i = 0; i < n; ++i) {
      a[i].imag = -a[i].imag;
    }
  }

  Complex* x = a;
  Complex* y = WorkArea(n);
  int64 height = n;
  int64 width = 1;

  // Radix-4
  const int64 loop4 = config.exponent[0] / 2;
  for (int64 i = 0; i < loop4; ++i) {
    height /= 4;
    if (i % 2 == 0) {
      Transform4(config, width, height, x, y);
    } else {
      Transform4(config, width, height, y, x);
    }
    width *= 4;
  }
  if (loop4 % 2 == 1)
    std::memcpy(x, y, sizeof(Complex) * n);

  // Radix-2
  if (config.exponent[0] % 2) {
    height /= 2;
    Transform2(config, width, height, x, y);
    std::memcpy(x, y, sizeof(Complex) * n);
  }

  if (type == Type::Inverse) {
    for (int64 i = 0; i < n; ++i) {
      a[i].real *= 1.0 / n;
      a[i].imag *= -1.0 / n;
    }
  }
}

void Fft::Transform2(const Config& config, int64 width, int64 height,
                     Complex x[], Complex y[]) {
  double th = -M_PI / width;
  for (int64 j = 0; j < width; ++j) {
    double wr = std::cos(th * j);
    double wi = std::sin(th * j);
    for (int64 k = 0; k < height; ++k) {
      int64 ix0 = height * 2 * j + k;
      int64 ix1 = ix0 + height;
      double x0r = x[ix0].real, x0i = x[ix0].imag;
      double x1r = x[ix1].real, x1i = x[ix1].imag;
      double xwr = x1r * wr - x1i * wi;
      double xwi = x1r * wi + x1i * wr;

      int64 iy0 = height * j + k;
      int64 iy1 = iy0 + config.n / 2;
      y[iy0].real = x0r + xwr;
      y[iy0].imag = x0i + xwi;
      y[iy1].real = x0r - xwr;
      y[iy1].imag = x0i - xwi;
    }
  }
}

void Fft::Transform4(const Config& config, int64 width, int64 height,
                     Complex x[], Complex y[]) {
  const int64& n = config.n;
  Complex* z = base::Allocator::Allocate<Complex>(n * 2);

  double th = -M_PI / (width * 2);
  for (int64 j = 0; j < width; ++j) {
    double w1r = std::cos(th * j);
    double w1i = std::sin(th * j);
    double w2r = w1r * w1r - w1i * w1i;
    double w2i = 2 * w1r * w1i;
    double w3r = w2r * w1r - w2i * w1i;
    double w3i = w2r * w1i + w2i * w1r;
    for (int64 k = 0; k < height; ++k) {
      int64 ix0 = height * 4 * j + k;
      int64 ix1 = ix0 + height;
      int64 ix2 = ix0 + height * 2;
      int64 ix3 = ix0 + height * 3;

      double x0r = x[ix0].real, x0i = x[ix0].imag;
      double x1r = x[ix1].real, x1i = x[ix1].imag;
      double x2r = x[ix2].real, x2i = x[ix2].imag;
      double x3r = x[ix3].real, x3i = x[ix3].imag;

      double x1w1r = x1r * w1r - x1i * w1i;
      double x1w1i = x1r * w1i + x1i * w1r;
      double x2w2r = x2r * w2r - x2i * w2i;
      double x2w2i = x2r * w2i + x2i * w2r;
      double x3w3r = x3r * w3r - x3i * w3i;
      double x3w3i = x3r * w3i + x3i * w3r;

      double z0p2r = x0r + x2w2r;
      double z0p2i = x0i + x2w2i;
      double z0m2r = x0r - x2w2r;
      double z0m2i = x0i - x2w2i;
      double z1p3r = x1w1r + x3w3r;
      double z1p3i = x1w1i + x3w3i;
      double z1m3r = x1w1i - x3w3i;
      double z1m3i = -(x1w1r - x3w3r);

      int64 iy0 = height * j + k;
      int64 iy1 = height * j + k + n / 4;
      int64 iy2 = height * j + k + n / 2;
      int64 iy3 = height * j + k + n / 4 + n / 2;

      y[iy0].real = z0p2r + z1p3r;
      y[iy0].imag = z0p2i + z1p3i;
      y[iy1].real = z0m2r + z1m3r;
      y[iy1].imag = z0m2i + z1m3i;
      y[iy2].real = z0p2r - z1p3r;
      y[iy2].imag = z0p2i - z1p3i;
      y[iy3].real = z0m2r - z1m3r;
      y[iy3].imag = z0m2i - z1m3i;
    }
  }

  base::Allocator::Deallocate(z);
}

}  // namespace fmt
}  // namespace ppi
