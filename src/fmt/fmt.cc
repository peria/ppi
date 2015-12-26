#include "fmt/fmt.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "base/base.h"
#include "fmt/fft.h"

namespace ppi {
namespace fmt {

void Fmt::Fmt4(const Fft::Type type, const int64 n, double da[]) {
  // TODO: Move config to out side of this method.
  Fft::Config config;
  Fft::Factor(n, &config);

  Complex* a = pointer_cast<Complex*>(da);
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

}  // namespace fmt
}  // namespace ppi
