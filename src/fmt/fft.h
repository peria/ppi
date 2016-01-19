#pragma once

#include "base/base.h"

namespace ppi {
namespace fmt {

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
  static void Transform(const Config& config, const Type type, Complex* a);
  static void TransformReal(const Type type, int64 n, double* a);

 private:
  // Compute 1 step of DFT from |x| into |y|.
  static void Transform2(const Config& config, int64 width, int64 height, Complex x[], Complex y[]);
  static void Transform4(const Config& config, int64 width, int64 height, Complex x[], Complex y[]);
};

}  // namespace fmt
}  // namespace ppi
