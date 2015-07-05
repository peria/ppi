#include "fmt/fmt.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "base/base.h"

namespace ppi {
namespace fmt {

namespace {
const int64 kMaxSize = 1 << 15;
double work[kMaxSize * 2];
}

void Fmt::Fft(const Type type, const int64 n, double* a) {
  if (type == Type::Inverse) {
    for (int64 i = 0; i < n; ++i) {
      a[2 * i + 1] = -a[2 * i + 1];
    }
  }

  double* x = a;
  double* y = work;
  for (int64 width = 1, height = n; height > 1;) {
    height /= 2;
    double th = -M_PI / width;
    for (int64 j = 0; j < width; ++j) {
      double wr = std::cos(th * j);
      double wi = std::sin(th * j);
      for (int64 k = 0; k < height; ++k) {
        int64 ix0 = height *  2 * j      + k;
        int64 ix1 = height * (2 * j + 1) + k;
        double x0r = x[2 * ix0], x0i = x[2 * ix0 + 1];
        double x1r = x[2 * ix1], x1i = x[2 * ix1 + 1];
        double xwr = x1r * wr - x1i * wi;
        double xwi = x1r * wi + x1i * wr;

        int64 iy0 = height *  j          + k;
        int64 iy1 = height * (j + width) + k;
        y[2 * iy0] = x0r + xwr;
        y[2 * iy0 + 1] = x0i + xwi;
        y[2 * iy1] = x0r - xwr;
        y[2 * iy1 + 1] = x0i - xwi;
      }
    }
    width *= 2;
    std::swap(x, y);
  }
 
  if (x != a)
    std::memcpy(a, x, sizeof(double) * n * 2);

  if (type == Type::Inverse) {
    for (int64 i = 0; i < n; ++i) {
      a[2 * i] *= 1.0 / n;
      a[2 * i + 1] *= -1.0 / n;
    }
  }
}

void Fmt::Fmt4(const Type type, const int64 n, double* da) {
  if (type == Type::Forward) {
    double th = M_PI / 2; // q = 1/4
    for (int64 i = 0; i < n; ++i) {
      double ar = da[2 * i], ai = da[2 * i + 1];
      double co = std::cos(th * i);
      double si = std::sin(th * i);
      da[2 * i] = ar * co - ai * si;
      da[2 * i + 1] = ar * si + ai * co;
    }
  }

  Fft(type, n, da);

  if (type == Type::Inverse) {
    double th = -M_PI / 2; // q = 1/4
    for (int64 i = 0; i < n; ++i) {
      double ar = da[2 * i], ai = da[2 * i + 1];
      double co = std::cos(th * i);
      double si = std::sin(th * i);
      da[2 * i] = ar * co - ai * si;
      da[2 * i + 1] = ar * si + ai * co;
    }
  }
}

}  // namespace fmt
}  // namespace ppi
