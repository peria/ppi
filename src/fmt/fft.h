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
  struct Config {
    int64 n;
    int exponent[3];
  };

  // Factor n with 2, 3, and 5.
  static void Factor(int64 n, Config* config);

  // Compute DFT of |a|.
  static void Transfer(const Config& config, const Type type, Complex a[]);
};

}  // namespace fmt
}  // namespace ppi
