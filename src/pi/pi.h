#pragma once

#include "base/base.h"
#include "number/real.h"

namespace ppi {
namespace pi {

using number::Real;

class Pi {
 public:
  // Computes pi using arctan formula in O(n^2) algorithm.
  // Returns the maximum rounding error in Multiplications.
  static double Arctan2(Real* pi);
};

}  // namespace pi
}  // namespace ppi
