#pragma once

#include "base/base.h"

namespace ppi {
namespace number {

class Integer {
 public:
  uint64& operator[](int64 n) const { return mantissa_[n]; }
  int64 size() const { return size_; }
  void assign(uint64* mantissa, int64 sz);
  void Set(int64 i, const uint64& limb);

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
  static void Mult(const Integer& a, const uint32 b, Integer* c);

  // c[n] = a[n] / b
  static void Div(const Integer& a, const uint32 b, Integer* c);

 protected:
  uint64* mantissa_;
  int64 size_;
};

}  // namespace number
}  // namespace ppi
