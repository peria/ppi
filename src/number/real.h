#pragma once

#include <ostream>

#include "base/base.h"
#include "number/integer.h"

namespace ppi {
namespace number {

// Real class describes a real number
//   |mantissa_| * (2^64)^(|exponent_|)
// |size_| figures its precision.
class Real : public Integer {
 public:
  // Computes 1/\sqrt{a}.  Returns the maximum rounding error.
  static double InverseSqrt(uint64 a, Real* val);

  // Computes a/b 
  static double Mult(const Real& a, const Real& b, Real* c);
  static void Mult(const Real& a, const uint32 b, Real* c);

  // Computes a/b.
  static void Div(const Real& a, const uint32 b, Real* c);

  int64 exponent() const { return exponent_; }

 protected:
  int64 exponent_;
};

std::ostream& operator<<(std::ostream& os, const Real& a);

}  // namespace number
}  // namespace ppi
