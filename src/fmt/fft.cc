#include "fmt/fft.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "base/base.h"
#include "base/allocator.h"
#include "fmt/fmt.h"

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

void Fft::Transform(const Config& config, const Direction dir, Complex a[]) {
  const int64 n = config.n;
  CHECK_GE(kMaxSize, n);
  if (dir == Direction::Backward) {
    for (int64 i = 0; i < n; ++i) {
      a[i].imag = -a[i].imag;
    }
  }

  Complex* x = a;
  Complex* y = WorkArea(n);
  int64 height = n;
  int64 width = 1;

  // Radix-4
  const int64 log4n = config.log2n / 2;
  for (int64 i = 0; i < log4n; ++i) {
    height /= 4;
    if (i % 2 == 0) {
      Radix4(width, height, x, y);
    } else {
      Radix4(width, height, y, x);
    }
    width *= 4;
  }
  if (log4n % 2 == 1)
    std::memcpy(x, y, sizeof(Complex) * n);

  // Radix-2
  if (config.log2n % 2 == 1) {
    height /= 2;
    Radix2(width, height, x, y);
    std::memcpy(x, y, sizeof(Complex) * n);
  }

  if (dir == Direction::Backward) {
    double inverse = 1.0 / n;
    for (int64 i = 0; i < n; ++i) {
      a[i].real *= inverse;
      a[i].imag *= -inverse;
    }
  }
}

void Fft::Radix2(const int64 width, const int64 height,
                 Complex x[], Complex y[]) {
  const int64 n2 = width * height;
  double th = -M_PI / width;
  for (int64 j = 0; j < width; ++j) {
    double wr = std::cos(th * j);
    double wi = std::sin(th * j);
    for (int64 k = 0; k < height; ++k) {
      int64 ix0 = k * width + j;
      int64 ix1 = ix0 + n2;
      double x0r = x[ix0].real, x0i = x[ix0].imag;
      double x1r = x[ix1].real, x1i = x[ix1].imag;
      double xwr = x1r * wr - x1i * wi;
      double xwi = x1r * wi + x1i * wr;

      int64 iy0 = k * width * 2 + j;
      int64 iy1 = iy0 + width;
      y[iy0].real = x0r + xwr;
      y[iy0].imag = x0i + xwi;
      y[iy1].real = x0r - xwr;
      y[iy1].imag = x0i - xwi;
    }
  }
}

void Fft::Radix4(const int64 width, const int64 height,
                 Complex x[], Complex y[]) {
  const int64 n4 = width * height;
  double th = -M_PI / (2 * width);
  for (int64 j = 0; j < width; ++j) {
    double w1r = std::cos(th * j);
    double w1i = std::sin(th * j);
    double w2r = w1r * w1r - w1i * w1i;
    double w2i = 2 * w1r * w1i;
    double w3r = w2r * w1r - w2i * w1i;
    double w3i = w2r * w1i + w1r * w2i;
    for (int64 k = 0; k < height; ++k) {
      int64 ix0 = k * width + j;
      int64 ix1 = ix0 + n4;
      int64 ix2 = ix0 + n4 * 2;
      int64 ix3 = ix0 + n4 * 3;

      double x0r = x[ix0].real, x0i = x[ix0].imag;
      double x1r = x[ix1].real, x1i = x[ix1].imag;
      double x2r = x[ix2].real, x2i = x[ix2].imag;
      double x3r = x[ix3].real, x3i = x[ix3].imag;

      double x1wr = x1r * w1r - x1i * w1i, x1wi = x1r * w1i + x1i * w1r;
      double x2wr = x2r * w2r - x2i * w2i, x2wi = x2r * w2i + x2i * w2r;
      double x3wr = x3r * w3r - x3i * w3i, x3wi = x3r * w3i + x3i * w3r;

      double x0p2r = x0r + x2wr, x0p2i = x0i + x2wi;
      double x0m2r = x0r - x2wr, x0m2i = x0i - x2wi;
      double x1p3r = x1wr + x3wr, x1p3i = x1wi + x3wi;
      double x1m3r = -x1wi + x3wi, x1m3i = x1wr - x3wr;

      int64 iy0 = k * width * 4 + j;
      int64 iy1 = iy0 + width;
      int64 iy2 = iy0 + width * 2;
      int64 iy3 = iy0 + width * 3;

      y[iy0].real = x0p2r + x1p3r;
      y[iy0].imag = x0p2i + x1p3i;
      y[iy1].real = x0m2r - x1m3r;
      y[iy1].imag = x0m2i - x1m3i;
      y[iy2].real = x0p2r - x1p3r;
      y[iy2].imag = x0p2i - x1p3i;
      y[iy3].real = x0m2r + x1m3r;
      y[iy3].imag = x0m2i + x1m3i;
    }
  }
}

}  // namespace fmt
}  // namespace ppi
