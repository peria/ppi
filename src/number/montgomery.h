#pragma once

#include "base/base.h"

namespace ppi {
namespace number {

class Montgomery {
 public:
  // Allow implicit conversion
  Montgomery(uint64 value);
  Montgomery(uint64 value, uint64 mod);

  operator uint64() const { return value_; }
  
  static uint64 Power(uint64 a, uint64 e, uint64 m);
  static Montgomery Mult(const Montgomery& lhs, const Montgomery& rhs, uint64 mod, uint64 inverse);

 private:
  uint64 value_;
};

}  // namespace number
}  // namespace ppi
