#pragma once

#include <ostream>
#include "base/base.h"

namespace ppi {
namespace number {

class Integer;

// IntegerPiece class represents a fixed-length long integer.
class IntegerPiece {
 public:
  IntegerPiece();
  IntegerPiece(uint64* limbs, int64 size);
  IntegerPiece(const Integer& integer);
  IntegerPiece(Integer& integer);

  int64 size() const { return size_; }
  uint64& operator[](int64 index) const { return limbs_[index]; }

  // APIs -----------------------------------------------------------
  // Computes c[n] = a[n] + b[n]
  static uint64 Add(const IntegerPiece& a, const IntegerPiece& b, IntegerPiece* c);

  // Computes c[n] = a[n] - b[n]
  static void Subtract(const IntegerPiece& a, const IntegerPiece& b, IntegerPiece* c);

  // Computes c[f(n+m)] = a[n] * b[m].  f(x) is the least power of 2,
  // which is greater than or equal to x.
  // Returns the maximum error in rounding.
  static double Mult(const IntegerPiece& a, const IntegerPiece& b, IntegerPiece* c);

  // Computes c[n] = a[n] * b. Returns a carried limb.
  static uint64 Mult(const IntegerPiece& a, const uint64 b, IntegerPiece* c);

  // c[n] = a[n] / b
  static void Div(const IntegerPiece& a, const uint64 b, IntegerPiece* c);

  // Outputs hexadecimal representation of this integer to |os|.
  // TODO: Support decimal output
  static void Show(const IntegerPiece& val, std::ostream& os);

  IntegerPiece& operator=(uint64 a);
  
 protected:
  // static ----------------------------------------------------------

  // Split a[m] into d[4n][2], where m <= 2*n.
  // If a[i] is 0x1234567890ABCDEF,
  // da[4*i..4*i+3][0] will be {0xCDEF, 0x90AB, 0x5678, 0x1234},
  // and a[j+n] is 0x1234567890ABCDEF,
  // da[4*j..4*j+3][1] will be {0xCDEF, 0x90AB, 0x5678, 0x1234}.
  // Other valus will be filled with 0.
  static void Split4In8(const IntegerPiece& a, const int64 n, double* da);

  // Gather d[4n*2] into a[2n]. (Length is specified by a.size())
  // If d[8*i..8*i+7] represents {1, 2, 3, 4, 5, 6, 7, 8}, then
  // a[2*i] = 0x7000500030001, a[2*i+n] = 0x8000600040002.
  static double Gather4In8(double* da, IntegerPiece* a);

 private:
  uint64* limbs_;
  int64 size_;
};

std::ostream& operator<<(std::ostream& os, const IntegerPiece& val);

}  // namespace number
}  // namespace ppi
