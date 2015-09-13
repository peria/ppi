#include "fmt/fft.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "base/base.h"

namespace ppi {
namespace fmt {

namespace {
const int64 kMaxSize = 1 << 15;
Complex work[kMaxSize];
}

void Fft::Transfer(const Type type, const int64 n, Complex a[]) {
  if (type == Type::Inverse) {
    for (int64 i = 0; i < n; ++i) {
      a[i].imag = -a[i].imag;
    }
  }

  Complex* x = a;
  Complex* y = work;
  for (int64 width = 1, height = n; height > 1;) {
    height /= 2;
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
        int64 iy1 = iy0 + n / 2;
        y[iy0].real = x0r + xwr;
        y[iy0].imag = x0i + xwi;
        y[iy1].real = x0r - xwr;
        y[iy1].imag = x0i - xwi;
      }
    }
    width *= 2;
    std::swap(x, y);
  }
 
  if (x != a)
    std::memcpy(a, x, sizeof(Complex) * n);

  if (type == Type::Inverse) {
    for (int64 i = 0; i < n; ++i) {
      a[i].real *= 1.0 / n;
      a[i].imag *= -1.0 / n;
    }
  }
}

}  // namespace fmt
}  // namespace ppi
