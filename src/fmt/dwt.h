#pragma once

#include "base/base.h"
#include "fmt/rft.h"
#include "fmt/fmt.h"

namespace ppi {
namespace fmt {

class Dwt {
 public:
  // Compute DWT of |a| with using q=1/4, which is used in right angle
  // convolutions.  Size of |a| is n.
  static void Dwt4(const Direction dir, const int64 n, Complex* a);

  // Compute DWT of |a| with using q=1/2, which is used in nega-cyclic
  // convolutions.  Size of |a| is n.
  static void Dwt2(const Direction dir, const int64 n, Complex* a);
  static void Dwt2Real(const Direction dir, const int64 n, double* a);
};

}  // namespace fmt
}  // namespace ppi
