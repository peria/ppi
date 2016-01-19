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
  // and 16bit * 4.  16bit * 4 data are stored in double format.
  // In both format, numbers are represented in little endian.

  // Lower n 64bit integers are stored in real parts of 4n complex numbers,
  // and upper n integers are stored in imaginal parts of them.
  static void Split4Round(const Integer& a, const int64 n, Complex* ca);
  static double Gather4Round(Complex* ca, Integer* a);

  // Stored in serial and nega-cyclic order.
  // So n 64bit integers can be stored in 4n double numbers.
  static void Split4Serial(const Integer& a, const int64 n, double* ca);
  static double Gather4Serial(double* ca, Integer* a);
  
 private:
  uint64* data_;
};

std::ostream& operator<<(std::ostream& os, const Integer& val);

}  // namespace number
}  // namespace ppi
