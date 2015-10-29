#pragma once

#include <ostream>
#include "base/base.h"

namespace ppi {
namespace number {

// Integer class represents a long integer, using an array of uint64.
// It usually stores digits in hexadecimal format. 16 hexadecimal digts
// are stored in a uint64 element.
// size() shows its length counted in uint64. If size() is zero, then
// it represents a zero. If size() is negative, then it shows NaN.
class Integer {
 public:
  Integer();
  Integer(uint64* mantissa, int64 sz);

  // Returns the n-th least significant limb.
  uint64& operator[](int64 n) const { return mantissa_[n]; }
  int64 size() const { return used_size_; }
  // TODO: deprecated method.
  void assign(uint64* mantissa, int64 sz);

  // Computes c[n] = a[n] + b[n]
  static void Add(const Integer& a, const Integer& b, Integer* c);

  // Computes c[n] = a[n] - b[n]
  static void Subtract(const Integer& a, const Integer& b, Integer* c);

  // Computes c[2n] = a[n] * b[n]
  // Returns the maximum error in rounding.
  static double Mult(const Integer& a, const Integer& b, Integer* c);
  static void Split4In8(const Integer& a, double* da);
  static double Gather4(double* da, Integer* a);

  // Computes c[n] = a[n] * b.
  static uint64 Mult(const Integer& a, const uint32 b, Integer* c);

  // c[n] = a[n] / b
  static void Div(const Integer& a, const uint32 b, Integer* c);

  // TODO: Support decimal output
  static void Show(const Integer& val, std::ostream& os);

 protected:
  void normalize();

  uint64* mantissa_;
  int64 used_size_;
  int64 allocated_size_;
};


}  // namespace number
}  // namespace ppi
