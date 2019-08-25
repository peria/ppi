#pragma once

#include "base/base.h"

namespace ppi {

#if defined(__SIZEOF_INT128__)

using uint128 = __uint128_t;

#else

class UInt128 {
public:
  // Allow implicit conversion
  UInt128(uint64 other) {
    u[0] = other;
    u[1] = 0;
  }
  UInt128(const UInt128& other) {
    u[0] = other[0];
    u[1] = other[1];
  }
  uint64 operator() const { return u[0]; }

  UInt128 operator+=(const uint64& other) {
    u[0] += other;
    if (u[0] < other)
      ++u[1];
    return *this;
  }
  UInt128 operator-=(const uint64& other) {
    u[0] -= other;
    if (u[0] > other)
      --u[1];
    return *this;
  }
  UInt128 operator+(const uint64& other) {
    UInt128 self(*this);
    return self += other;
  }
  UInt128 operator-(const uint64& other) {
    UInt128 self(*this);
    return self -= other;
  }

private:
  uint64 u[2] {};
};

using uint128 = UInt128;

#endif

}  // namespace ppi
