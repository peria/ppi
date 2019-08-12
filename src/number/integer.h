#pragma once

#include <ostream>

#include "base/base.h"

namespace ppi {
namespace number {

// Represents a non negative integer in multiple precision format.
class Integer {
 public:
  enum class Base : uint8 {
    kHex,
    kDecimal,
  };

  Integer(const Base = Base::kHex);
  explicit Integer(uint64 value, const Base = Base::kHex);
  explicit Integer(const Integer& other);
  ~Integer();

  uint64& operator[](int64 i) const { return data_[i]; }
  int64 size() const { return static_cast<int64>((*this)[-1]); }
  uint64* data() const { return data_; }

  uint64 leading() const;
  void resize(int64 size);
  void erase(int64 begin, int64 end);
  void clear();
  void insert(int64 from, int64 number, uint64 value);
  void push_leading(uint64 value);
  Base base() const { return base_; }

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

  // Computes c[n] = a[n] * b.
  static void Mult(const Integer& a, const uint64 b, Integer* c);

  // Computes c[n] = a ** b.
  static void Power(const uint64 a, const uint64 b, Integer* c);

  // c[n] = a[n] / b
  static void Div(const Integer& a, const uint64 b, Integer* c);

  // c[n] = a[n] >> b
  static void RBitShift(const Integer& a, const uint64 b, Integer* c);
  // c[n] = a[n] << b
  static void LBitShift(const Integer& a, const uint64 b, Integer* c);

  // Outputs hexadecimal representation of this integer to |os|.
  // TODO: Support decimal output
  static void Show(const Integer& val, std::ostream& os);

  Integer& operator=(const Integer& other);
  Integer& operator=(uint64 a);

 protected:
  void Normalize();

 private:
  uint64* data_;
  const Base base_;
};

std::ostream& operator<<(std::ostream& os, const Integer& val);
std::ostream& operator<<(std::ostream& os, const Integer::Base& base);

}  // namespace number
}  // namespace ppi
