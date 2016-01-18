#pragma once

#include "base/base.h"
#include "fmt/fft.h"

namespace ppi {
namespace fmt {

class Fmt {
 public:
  // Compute DWT of |a| with using q=1/4.  Size of a[] is n.
  static void Fmt4(const Fft::Type type, const int64 n, Complex* a);
};

}  // namespace fmt
}  // namespace ppi
