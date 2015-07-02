#pragma once

#include "number/number.h"

namespace ppi {

class Natural {
public:
  Digit& operator[](int64 i) { return digits_[i]; }
  int64 size() const { return size_; }
  
private:
  Digit* digits_;
  int64 size_;
};

}  // namespace ppi
