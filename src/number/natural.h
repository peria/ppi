#pragma once

#include "number/number.h"

namespace ppi {

class Natural {
public:
  Natural(Digit d);
  Natural(const Natual& other);

  Digit& operator[](int64 i) { return digits_[i]; }
  int64 size() const { return size_; }
  void push(const Digit d);
  void resize(const int64 nsize);
  
private:
  Digit* digits_ = nullptr;
  int64 size_ = 0;
};

}  // namespace ppi
