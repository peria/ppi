#pragma once

#include "base/base.h"

namespace ppi {
namespace number {

class Integer {
 public:
  uint64& operator[](int64 n) const { return mantissa_[n]; }
  int64 size() const { return size_; }

  uint64* mantissa_;
  int64 size_;
};

// Computes c[n] = a[n] + b[n]
void Add(const Integer& a, const Integer& b, Integer* c);

// Computes c[n] = a[n] - b[n]
void Subtract(const Integer& a, const Integer& b, Integer* c);

// c[2n] = a[n] * b[n]
void Mult(const Integer& a, const Integer& b, Integer* c);

}  // namespace number
}  // namespace ppi
