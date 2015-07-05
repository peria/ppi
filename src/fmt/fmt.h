#pragma once

#include "base/base.h"

namespace ppi {
namespace fmt {

class Fmt {
 public:
  enum Type {
    Forward,
    Inverse,
  };

  // Compute DFT of |da|.
  static void Fft(const Type type, const int64 n, double* da);
  
  // Compute DWT of |da| with using q=1/4.
  static void Fmt4(const Type type, const int64 n, double* da);
};

}  // namespace fmt
}  // namespace ppi
