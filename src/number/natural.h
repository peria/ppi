#pragma once

#include <ostream>

#include "number/number.h"

namespace ppi {

class Natural {
public:
  explicit Natural() = default;
  explicit Natural(Digit d);
  explicit Natural(const Natural& other);

  Digit& operator[](int64 i) const { return digits_[i]; }
  int64 size() const { return size_; }
  void push(const Digit d);
  void resize(const int64 nsize);
  void clear();

  // Operations
  Natural& operator=(const Digit d);
  Natural& operator+=(const Natural& other);
  Natural& operator+=(const Digit d);
  Natural& operator-=(const Natural& other);
  Natural& operator*=(const Natural& other);
  Natural& operator*=(const Digit other);

private:
  Digit* digits_ = nullptr;
  int64 size_ = 0;
};

std::ostream& operator<<(std::ostream& ost, const Natural& a);

}  // namespace ppi
