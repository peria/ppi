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
  ~Integer();

  uint64& operator[](int64 i) const { return data_[i]; }
  int64 size() const { return static_cast<int64>((*this)[-1]); }

  uint64 back() const;
  void resize(int64 size);
  void erase(int64 begin, int64 end);
  void clear();
  void insert(int64 from, int64 number, uint64 value);
  void push_back(uint64 value);

  void release() { data_ = nullptr; }

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

  // Split4XXX and Gather4XXX methods convert integers between 64bit
  // and 16bit * 4.
  // Example:
  // Split {0x4444333322221111} into 4 * 16bit
  // --> {0x1111, 0x2222, 0x3333, 0x4444}
  // Split {0x5555666677778888, 0x4444333322221111} into 4 * 16bit
  // --> {0x7777, 0x5555, 0x3333, 0x1111} (Nega-cyclic)
  static void Split4(const Integer& a, const int64 n, double* ca);
  static double Gather4(double* ca, Integer* a);
  
 private:
  uint64* data_;
};

std::ostream& operator<<(std::ostream& os, const Integer& val);

}  // namespace number
}  // namespace ppi
