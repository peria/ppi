#pragma once

#include "base/base.h"
#include "base/allocator.h"

#include <ostream>

namespace ppi {

using Digit = uint64;

// Representa a non negative integers.  A digit contains 16 decimal digits.
class Natural {
public:
  ~Natural();

  int64 size() const { return size_; }
  Digit& operator[](int64 i) { return digits_[i]; }
  const Digit& operator[](int64 i) const { return digits_[i]; }

  bool operator<(const Natural& other) const;
  bool operator==(const Natural& other) const;
  bool operator>(const Natural& other) const {
    return other < (*this);
  }
  bool operator!=(const Natural& other) const {
    return !((*this) == other);
  }
  bool operator>=(const Natural& other) const {
    return !((*this) < other);
  }
  bool operator<=(const Natural& other) const {
    return !((*this) > other);
  }

  void normalize();

  // Operations including memory size management.
  static void add(const Natural& a, const Natural& b, Natural& c);
  static void subtract(const Natural& a, const Natural& b, Natural& c);
  static void div(const Natural& a, const Digit b, Natural& c);

protected:
  inline void push_lead(const Digit d);
  inline void resize(const int64 n);

private:
  int64 capacity() const {
    return base::Allocator::getSizeInByte(digits_) / sizeof(Digit);
  }

  Digit* digits_ = nullptr;
  int64 size_ = 0;
};

std::ostream& operator<<(std::ostream&, const Natural&);

void Natural::push_lead(const Digit d) {
  int64 n = size();
  resize(n + 1);
  digits_[n] = d;
}

void Natural::resize(const int64 n) {
  if (capacity() > n) {
    Digit* old = digits_;
    digits_ = base::Allocator::allocate<Digit>(n);
    for (int64 i = 0; i < size(); ++i)
      digits_[i] = old[i];
    base::Allocator::deallocate(old);
  }
  size_ = n;
}

}  // namespace ppi
