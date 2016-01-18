#pragma once

#include "base/base.h"
#include "fmt/fft.h"

namespace ppi {
namespace fmt {

class Fmt {
 public:
  // Compute DWT of |a| with using q=1/4, which is used in right angle
  // convolutions.  Size of |a| is n.
  static void Fmt4(const Fft::Type type, const int64 n, Complex* a);

  // Compute DWT of |a| with using q=1/2, which is used in nega-cyclic
  // convolutions.  Size of |a| is n.
  static void Fmt2(const Fft::Type type, const int64 n, Complex* a);
  static void Fmt2Real(const Fft::Type type, const int64 n, double* a);
};

}  // namespace fmt
}  // namespace ppi
