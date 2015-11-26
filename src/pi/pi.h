#pragma once

#include "base/base.h"
#include "number/real.h"

namespace ppi {
namespace pi {

using number::Real;
using number::Integer;

class Pi {
 public:
  // Computes pi using arctan formula in O(n^2) algorithm.
  // Returns the maximum rounding error in Multiplications.
  static double Arctan2(Real* pi);

  // Computes pi using arctan formula in O(n(log n)^3) algorithm.
  // Returns the maximum rounding error in Multiplications.
  static double Chudnovsky(Real* pi);

 private:
  static double ChudnovskyInternal(int64 n0, int64 n1, Integer* a0, Integer* b0, Integer* c0);
  static void ChudnovskySetValues(int64 n, Integer* a, Integer* b, Integer* c);
};

}  // namespace pi
}  // namespace ppi
