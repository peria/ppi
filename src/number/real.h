#pragma once

#include "base/base.h"
#include "number/integer.h"

namespace ppi {
namespace number {

// Real class describes a real number
//   |mantissa_| * (2^64)^(|exponent_|)
// |size_| figures its precision.
class Real : public Integer {
 public:
  // Compute 1/\sqrt{a}.  Returns the maximum rounding error.
  static double InverseSqrt(uint64 a, Real* val);

  // Methods in Integer class.
  static double Mult(const Real& a, const Real& b, Real* c);

  // Computes a/b.
  static void Div(const Real& a, const uint32 b, Real* c);

 protected:
  int64 exponent_;
};

}  // namespace number
}  // namespace ppi
