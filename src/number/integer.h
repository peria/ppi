#pragma once

#include <vector>
#include <ostream>
#include "base/base.h"

namespace ppi {
namespace number {

// Integer class represents a long integer, using an array of uint64.
// It usually stores digits in hexadecimal format. 16 hexadecimal digts
// are stored in a uint64 element.
// size() shows its length counted in uint64. If size() is zero, then
// it represents a zero. If size() is negative, then it shows NaN.
class Integer : public std::vector<uint64> {
 public:
  Integer();

  // APIs -----------------------------------------------------------
  // Computes c[n] = a[n] + b[n]
  static void Add(const Integer& a, const Integer& b, Integer* c);

  // Computes c[n] = a[n] - b[n]
  static void Subtract(const Integer& a, const Integer& b, Integer* c);

  // Computes c[2n] = a[n] * b[n]. Returns the maximum error in rounding.
  static double Mult(const Integer& a, const Integer& b, Integer* c);

  // Computes c[n] = a[n] * b. Returns a carried limb.
  static uint64 Mult(const Integer& a, const uint32 b, Integer* c);

  // c[n] = a[n] / b
  static void Div(const Integer& a, const uint32 b, Integer* c);

  // Outputs hexadecimal representation of this integer to |os|.
  // TODO: Support decimal output
  static void Show(const Integer& val, std::ostream& os);

 protected:
  void normalize();

  // static ----------------------------------------------------------

  // Split a[n] into d[4n*2]. If a[i] represents 0x1234567890ABCDEF,
  // da[8*i..8*i+7] will be {0xCDEF, 0,  0x90AB, 0,  0x5678, 0,  0x1234, 0}.
  static void Split4In8(const Integer& a, double* da);

  // Gather d[4n*2] into a[2n]. (Length is specified by a.size())
  // If d[8*i..8*i+7] represents {1, 2, 3, 4, 5, 6, 7, 8}, then
  // a[2*i] = 0x7000500030001, a[2*i+n] = 0x8000600040002.
  static double Gather4In8(double* da, Integer* a);
};

std::ostream& operator<<(std::ostream& os, const Integer& val);

}  // namespace number
}  // namespace ppi
