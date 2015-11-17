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
  Real();
  Real(double d);
  
  // Computes 1/\sqrt{a}.  Returns the maximum rounding error.
  static double InverseSqrt(uint64 a, Real* val);

  // Compute 1/a.
  static double Inverse(const Real& a, Real* val);

  // Comptues c=a+b
  static void Add(const Real& a, const Real& b, Real* c);
  // Computes c+=a
  static void Add(const Real& a, Real* c);

  // Comptues c=a-b
  static void Sub(const Real& a, const Real& b, Real* c);
  
  // Computes c=a*b 
  static double Mult(const Real& a, const Real& b, Real* c);
  static void Mult(const Real& a, const uint32 b, Real* c);

  // Computes c=a/b.
  static void Div(const Real& a, const uint32 b, Real* c);

  int64 exponent() const { return exponent_; }

  // TODO: Implement precision independent from size().
  size_t precision() const { return precision_; }
  void setPrecision(int64 prec);

 protected:
  // Update the number to fill either of following conditions.
  // - size() == 0
  // - size() <= precision() && front() != 0 && back() != 0
  void Normalize();

  // Returns the number zeros in tail.
  int64 TailingZero();
  
  int64 precision_;
  int64 exponent_;
};

std::ostream& operator<<(std::ostream& os, const Real& a);

}  // namespace number
}  // namespace ppi
