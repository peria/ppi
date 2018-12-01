#include "fmt/rft.h"

#include <glog/logging.h>

#include <cmath>

namespace ppi {
namespace fmt {

Rft::Rft(const int64 n)
    : Dft(n / 2), n_(n) {
  DCHECK_EQ(0, n % 4);
}

void Rft::Transform(const Direction dir, double* a) const {
  Complex* ca = reinterpret_cast<Complex*>(a);

  if (dir == Direction::Backward) {
    double x0r = a[0];
    double x0i = a[1];
    a[0] = (x0r + x0i) * 0.5;
    a[1] = (x0r - x0i) * 0.5;
    const double th = -2 * M_PI / n_;
    for (int64 i = 1; i < n_ / 4; ++i) {
      Complex& x0 = ca[i];
      Complex& x1 = ca[n_ / 2 - i];
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
    ca[n_ / 4].imag = -ca[n_ / 4].imag;
  }

  Dft::Transform(dir, ca);

  if (dir == Direction::Forward) {
    double x0r = a[0];
    double x0i = a[1];
    a[0] = x0r + x0i;
    a[1] = x0r - x0i;
    const double th = -2 * M_PI / n_;
    for (int64 i = 1; i < n_ / 4; ++i) {
      Complex& x0 = ca[i];
      Complex& x1 = ca[n_ / 2 - i];
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
    ca[n_ / 4].imag = -ca[n_ / 4].imag;
  }
}

}  // namespace fmt
}  // namespace ppi
