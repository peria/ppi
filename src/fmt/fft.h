#pragma once

#include "base/base.h"
#include "fmt/fmt.h"

namespace ppi {
namespace fmt {

class Fft {
 public:
  struct Config {
    int64 n;
    int exponent[3];
  };

  // Factor n with 2, 3, and 5.
  static void Factor(int64 n, Config* config);

  // Compute DFT of |a|.
  static void Transform(const Config& config, const Direction dir, Complex* a);
  static void TransformReal(const Direction dir, int64 n, double* a);

 private:
  // Compute 1 step of DFT from |x| into |y|.
  static void Radix2(const Config& config, int64 width, int64 height, Complex x[], Complex y[]);
  static void Radix4(const Config& config, int64 width, int64 height, Complex x[], Complex y[]);
};

}  // namespace fmt
}  // namespace ppi
