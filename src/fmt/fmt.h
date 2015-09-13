#pragma once

#include "base/base.h"
#include "fmt/fft.h"

namespace ppi {
namespace fmt {

class Fmt {
 public:
  // Compute DWT of |da| with using q=1/4.
  static void Fmt4(const Fft::Type type, const int64 n, double da[]);
};

}  // namespace fmt
}  // namespace ppi
