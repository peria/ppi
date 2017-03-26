#pragma once

#include "base/base.h"
#include "fmt/fmt.h"

namespace ppi {
namespace fmt {

// Computes DFT in complex field.
class Fft {
 public:
  // Compute DFT of |a|.
  static void Transform(const Config& config, const Direction dir, Complex* a);

 private:
  // Compute 1 step of DFT from |x| into |y|.
  static void Radix2(const Config& config, int64 width, int64 height, Complex x[], Complex y[]);
  static void Radix4(const Config& config, int64 width, int64 height, Complex x[], Complex y[]);
};

}  // namespace fmt
}  // namespace ppi
