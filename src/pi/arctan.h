#pragma once

#include "base/base.h"
#include "number/real.h"

namespace ppi {
namespace pi {

using number::Integer;
using number::Real;

class Arctan {
 public:
  // Computes pi using arctan formula in O(n^2) algorithm.
  // Returns the maximum rounding error in Multiplications.
  static double Machin(Real& pi);
};

}  // namespace pi
}  // namespace ppi
