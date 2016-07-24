#include "fmt/dwt.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "base/base.h"
#include "fmt/fft.h"
#include "fmt/fmt.h"

namespace ppi {
namespace fmt {

void Dwt::Dwt4(const Direction dir, const int64 n, Complex* a) {
  // TODO: Move config to out side of this method.
  Fft::Config config;
  Fft::Factor(n, &config);

  if (dir == Direction::Forward) {
    double th = M_PI / (2 * n); // q = 1/4
    for (int64 i = 0; i < n; ++i) {
      double ar = a[i].real;
      double ai = a[i].imag;
      double co = std::cos(th * i);
      double si = std::sin(th * i);
      a[i].real = ar * co - ai * si;
      a[i].imag = ar * si + ai * co;
    }
  }

  Fft::Transform(config, dir, a);

  if (dir == Direction::Backward) {
    double th = -M_PI / (2 * n); // q = 1/4
    for (int64 i = 0; i < n; ++i) {
      double ar = a[i].real;
      double ai = a[i].imag;
      double co = std::cos(th * i);
      double si = std::sin(th * i);
      a[i].real = ar * co - ai * si;
      a[i].imag = ar * si + ai * co;
    }
  }
}

void Dwt::Dwt2(const Direction dir, const int64 n, Complex* a) {
  // TODO: Move config to out side of this method.
  Fft::Config config;
  Fft::Factor(n, &config);

  if (dir == Direction::Forward) {
    double th = - M_PI / n;
    for (int64 i = 0; i < n; ++i) {
      double ar = a[i].real;
      double ai = a[i].imag;
      double co = std::cos(th * i);
      double si = std::sin(th * i);
      a[i].real = ar * co - ai * si;
      a[i].imag = ar * si + ai * co;
    }
  }

  Fft::Transform(config, dir, a);

  if (dir == Direction::Backward) {
    double th = M_PI / n;
    for (int64 i = 0; i < n; ++i) {
      double ar = a[i].real;
      double ai = a[i].imag;
      double co = std::cos(th * i);
      double si = std::sin(th * i);
      a[i].real = ar * co - ai * si;
      a[i].imag = ar * si + ai * co;
    }
  }
}

void Dwt::Dwt2Real(const Direction dir, const int64 n, double* a) {
  CHECK(n % 2 == 0);
  Complex* ca = reinterpret_cast<Complex*>(a);

  if (dir == Direction::Backward) {
    const double th = -2 * M_PI / n;
    for (int64 i = 0; i < n / 4; ++i) {
      Complex& x0 = ca[i];
      Complex& x1 = ca[n / 2 - 1 - i];
      double xr = x0.real - x1.real;
      double xi = x0.imag + x1.imag;
      double t = th * (i + 0.5);
      double wr = 1 - std::sin(t);
      double wi = -std::cos(t);
      double ar = (xr * wr - xi * wi) * 0.5;
      double ai = (xr * wi + xi * wr) * 0.5;
      x0.real -= ar;
      x0.imag -= ai;
      x1.real += ar;
      x1.imag -= ai;
    }
  }
  
  Dwt2(dir, n / 2, ca);

  if (dir == Direction::Forward) {
    const double th = -2 * M_PI / n;
    for (int64 i = 0; i < n / 4; ++i) {
      Complex& x0 = ca[i];
      Complex& x1 = ca[n / 2 - 1 - i];
      double xr = x0.real - x1.real;
      double xi = x0.imag + x1.imag;
      double t = th * (i + 0.5);
      double wr = 1 - std::sin(t);
      double wi = std::cos(t);
      double ar = (xr * wr - xi * wi) * 0.5;
      double ai = (xr * wi + xi * wr) * 0.5;
      x0.real -= ar;
      x0.imag -= ai;
      x1.real += ar;
      x1.imag -= ai;
    }
  }
}

}  // namespace fmt
}  // namespace ppi
