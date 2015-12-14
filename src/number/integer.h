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
  explicit Integer(uint64 value);

  uint64& operator[](int64 i) const { return data_[i]; }
  int64 size() const { return static_cast<int64>((*this)[-1]); }

  uint64 back() const;
  void resize(int64 size, uint64 value = 0);
  void erase(int64 begin, int64 end);
  void clear();
  void insert(int64 from, int64 number, uint64 value);
  void push_back(uint64 value);

  // APIs -----------------------------------------------------------
  // Computes c[n] = a[n] + b[n]
  static void Add(const Integer& a, const Integer& b, Integer* c);

  // Computes c[n] = a[n] - b[n]
  static void Subtract(const Integer& a, const Integer& b, Integer* c);

  // Computes c[f(n+m)] = a[n] * b[m].  f(x) is the least power of 2,
  // which is greater than or equal to x.
  // Returns the maximum error in rounding.
  static double Mult(const Integer& a, const Integer& b, Integer* c);

  // Computes c[n] = a[n] * b. Returns a carried limb.
  static void Mult(const Integer& a, const uint32 b, Integer* c);

  // c[n] = a[n] / b
  static void Div(const Integer& a, const uint32 b, Integer* c);

  // Outputs hexadecimal representation of this integer to |os|.
  // TODO: Support decimal output
  static void Show(const Integer& val, std::ostream& os);

  Integer& operator=(uint64 a);
  
 protected:
  void Normalize();

  // static ----------------------------------------------------------

  // Split a[m] into d[4n][2], where m <= 2*n.
  // If a[i] is 0x1234567890ABCDEF,
  // da[4*i..4*i+3][0] will be {0xCDEF, 0x90AB, 0x5678, 0x1234},
  // and a[j+n] is 0x1234567890ABCDEF,
  // da[4*j..4*j+3][1] will be {0xCDEF, 0x90AB, 0x5678, 0x1234}.
  // Other valus will be filled with 0.
  static void Split4In8(const Integer& a, const int64 n, double* da);

  // Gather d[4n*2] into a[2n]. (Length is specified by a.size())
  // If d[8*i..8*i+7] represents {1, 2, 3, 4, 5, 6, 7, 8}, then
  // a[2*i] = 0x7000500030001, a[2*i+n] = 0x8000600040002.
  static double Gather4In8(double* da, Integer* a);

 private:
  uint64* data_;
};

std::ostream& operator<<(std::ostream& os, const Integer& val);

}  // namespace number
}  // namespace ppi
