#include "fmt/fmt.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "base/base.h"
#include "fmt/fft.h"

namespace ppi {
namespace fmt {

void Fmt::Fmt4(const Fft::Type type, const int64 n, Complex* a) {
  // TODO: Move config to out side of this method.
  Fft::Config config;
  Fft::Factor(n, &config);

  if (type == Fft::Type::Forward) {
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

  Fft::Transform(config, type, a);

  if (type == Fft::Type::Inverse) {
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

void Fmt::Fmt2(const Fft::Type type, const int64 n, Complex* a) {
  // TODO: Move config to out side of this method.
  Fft::Config config;
  Fft::Factor(n, &config);

  if (type == Fft::Type::Forward) {
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

  Fft::Transform(config, type, a);

  if (type == Fft::Type::Inverse) {
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

void Fmt::Fmt2Real(const Fft::Type type, const int64 n, double* a) {
  CHECK(n % 2 == 0);
  Complex* ca = reinterpret_cast<Complex*>(a);

  if (type == Fft::Type::Inverse) {
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
  
  Fmt2(type, n / 2, ca);

  if (type == Fft::Type::Forward) {
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
