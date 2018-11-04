#pragma once

#include "base/base.h"
#include "fmt/fmt.h"
#include "fmt/rft.h"

namespace ppi {
namespace fmt {

class Dwt {
 public:
  // Compute DWT of |a| with using q=1/4, which is used in right angle
  // convolutions.  Size of |a| is n.
  static void Dwt4(const Config& config, const Direction dir, Complex* a);

  // Compute DWT of |a| with using q=1/2, which is used in nega-cyclic
  // convolutions.  Size of |a| is n.
  static void Dwt2(const Config& config, const Direction dir, Complex* a);
  static void Dwt2Real(const Config& config, const Direction dir, double* a);
};

}  // namespace fmt
}  // namespace ppi
