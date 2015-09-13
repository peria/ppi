#pragma once

#include "base/base.h"

namespace ppi {
namespace fmt {

struct Complex {
  double real;
  double imag;
};

class Fft {
 public:
  enum Type {
    Forward,
    Inverse,
  };

  // Compute DFT of |a|.
  static void Transfer(const Type type, const int64 n, Complex a[]);
};

}  // namespace fmt
}  // namespace ppi
