#include "fmt/rft.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "base/base.h"
#include "base/allocator.h"
#include "base/util.h"
#include "fmt/fft.h"

namespace ppi {
namespace fmt {

void Rft::Transform(const Config& config, const Direction dir, double* a) {
  const int64 n = config.n * 2;
  Complex* ca = reinterpret_cast<Complex*>(a);

  if (dir == Direction::Backward) {
    double x0r = a[0];
    double x0i = a[1];
    a[0] = (x0r + x0i) * 0.5;
    a[1] = (x0r - x0i) * 0.5;
    const double th = -2 * M_PI / n;
    for (int64 i = 1; i < n / 4; ++i) {
      Complex& x0 = ca[i];
      Complex& x1 = ca[n / 2 - i];
      double xr = x0.real - x1.real;
      double xi = x0.imag + x1.imag;
      double wr = 1 - std::sin(th * i);
      double wi = -std::cos(th * i);
      double ar = (xr * wr - xi * wi) * 0.5;
      double ai = (xr * wi + xi * wr) * 0.5;
      x0.real -= ar;
      x0.imag -= ai;
      x1.real += ar;
      x1.imag -= ai;
    }
    ca[n / 4].imag = -ca[n / 4].imag;
  }

  Fft::Transform(config, dir, ca);

  if (dir == Direction::Forward) {
    double x0r = a[0];
    double x0i = a[1];
    a[0] = x0r + x0i;
    a[1] = x0r - x0i;
    const double th = -2 * M_PI / n;
    for (int64 i = 1; i < n / 4; ++i) {
      Complex& x0 = ca[i];
      Complex& x1 = ca[n / 2 - i];
      double xr = x0.real - x1.real;
      double xi = x0.imag + x1.imag;
      double wr = 1 - std::sin(th * i);
      double wi = std::cos(th * i);
      double ar = (xr * wr - xi * wi) * 0.5;
      double ai = (xr * wi + xi * wr) * 0.5;
      x0.real -= ar;
      x0.imag -= ai;
      x1.real += ar;
      x1.imag -= ai;
    }
    ca[n / 4].imag = -ca[n / 4].imag;
  }
}

}  // namespace fmt
}  // namespace ppi
