#pragma once

#include "base/base.h"

namespace ppi {

using Digit = uint64;

// Representa a non negative integers.  A digit contains 16 decimal digits.
class Natural {
public:
  int64 size() const { return size_; }
  Digit& operator[](int64 i) { return digits_[i]; }
  const Digit& operator[](int64 i) const { return digits_[i]; }

private:
  Digit* digits_ = nullptr;
  int64 size_ = 0;
};

}  // namespace ppi
