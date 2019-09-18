#pragma once

#include "base/base.h"
#include "base/allocator.h"

#include <glog/logging.h>

#include <ostream>

namespace ppi {

using Digit = uint64;

// Representa a non negative integers.  A digit contains 16 decimal digits.
// If it represents a 0, its size == 0.
class Natural {
public:
  Natural();
  ~Natural();

  int64 size() const { return size_; }
  Digit& operator[](int64 i) { return digits_[i]; }
  const Digit& operator[](int64 i) const { return digits_[i]; }

  Natural& operator=(const Digit& d) {
    if (d == 0) {
      resize(0);
    } else {
      resize(1);
      (*this)[0] = d;
    }
    return *this;
  }

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
  static void mult(const Natural& a, const Natural& b, Natural& c);
  static void mult(const Natural& a, const Digit b, Natural& c);
  static void div(const Natural& a, const Digit b, Natural& c);

  static constexpr int64 kDigitsPerDigit = 16;

  // This is temporarily public.
  // TODO: Make this protected.
  inline void resize(const int64 n);

protected:
  inline void push_lead(const Digit d);

private:
  int64 capacity() const {
    DCHECK(digits_);
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
  int64 cap = capacity();
  if (cap < n) {
    Digit* old = digits_;
    digits_ = base::Allocator::allocate<Digit>(n);
    for (int64 i = 0; i < size(); ++i)
      digits_[i] = old[i];
    for (int64 i = size(); i < n; ++i)
      digits_[i] = 0;
    base::Allocator::deallocate(old);
  }
  size_ = n;
}

}  // namespace ppi
