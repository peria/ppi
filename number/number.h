#pragma once

#include "base/base.h"

namespace ppi {

// Number class provides basic APIs for high-precision numbers.
class Number {
 public:
  using Digit = uint64;

 protected:
  size_t size_ = 0;
  Digit* digits_ = nullptr;
};

}  // namespace ppi
