#pragma once

#include "base/base.h"
#include "number/real.h"

namespace ppi {
namespace drm {

using number::Integer;
using number::Real;

class Drm {
 public:
  // Computes pi using arctan formula in O(n(log n)^3) algorithm.
  // Returns the maximum rounding error in Multiplications.
  static double Chudnovsky(Real* pi);

 private:
  static double Internal(int64 n0,
                         int64 n1,
                         Integer* a0,
                         Integer* b0,
                         Integer* c0);
  static void SetValues(int64 n, Integer* a, Integer* b, Integer* c);
};

}  // namespace pi
}  // namespace ppi
