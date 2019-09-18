#include "number/natural.h"

#include <glog/logging.h>

#include "base/allocator.h"
#include "number/internal.h"

namespace ppi {

Natural::Natural()
  : digits_(base::Allocator::allocate<Digit>(0)),
    size_(0) {}

Natural::~Natural() {
  if (digits_)
    base::Allocator::deallocate(digits_);
}

bool Natural::operator<(const Natural& other) const {
  if (size() != other.size())
    return size() < other.size();
  for (int64 i = size() - 1; i >= 0; --i) {
    if ((*this)[i] != other[i])
      return (*this)[i] < other[i];
  }
  // ==
  return false;
}

bool Natural::operator==(const Natural& other) const {
  if (size() != other.size())
    return false;
  for (int64 i = size() - 1; i >= 0; --i) {
    if ((*this)[i] != other[i])
      return false;
  }
  // ==
  return true;
}

void Natural::normalize() {
  int64 n = size() - 1;
  while (n >= 0 && (*this)[n] == 0)
    --n;
  ++n;
  resize(n);
}

void Natural::add(const Natural& a, const Natural& b, Natural& c) {
  if (a.size() < b.size())
    return add(b, a, c);

  Digit* dig_a = a.digits_;
  Digit* dig_b = b.digits_;
  Digit* dig_c = c.digits_;

  const int64 n = a.size();
  if (c.size() < a.size()) {
    dig_c = base::Allocator::allocate<Digit>(a.size());
  }

  Digit carry = internal::add(dig_a, n, dig_b, b.size(), dig_c);
  if (dig_c != c.digits_) {
    base::Allocator::deallocate(c.digits_);
    c.digits_ = dig_c;
  }
  c.size_ = n;

  if (carry)
    c.push_lead(carry);
}

void Natural::subtract(const Natural& a, const Natural& b, Natural& c) {
  DCHECK_GE(a, b);

  Digit* dig_a = a.digits_;
  Digit* dig_b = b.digits_;
  Digit* dig_c = c.digits_;

  const int64 n = a.size();
  if (c.size() < a.size()) {
    dig_c = base::Allocator::allocate<Digit>(a.size());
  }

  internal::subtract(dig_a, n, dig_b, b.size(), dig_c);
  if (dig_c != c.digits_) {
    base::Allocator::deallocate(c.digits_);
    c.digits_ = dig_c;
  }
  c.digits_ = dig_c;
  c.size_ = n;

  c.normalize();
}

void Natural::mult(const Natural& a, const Natural& b, Natural& c) {
  Digit* dig_a = a.digits_;
  Digit* dig_b = b.digits_;
  Digit* dig_c = c.digits_;

  const int64 nc = a.size() + b.size();
  if (c.size() < nc) {
    dig_c = base::Allocator::allocate<Digit>(nc);
  }

  internal::mult(dig_a, a.size(), dig_b, b.size(), dig_c);
  if (dig_c != c.digits_) {
    base::Allocator::deallocate(c.digits_);
    c.digits_ = dig_c;
  }
  c.digits_ = dig_c;
  c.size_ = nc;

  c.normalize();
}

void Natural::mult(const Natural& a, const Digit b, Natural& c) {
  Digit* dig_a = a.digits_;
  Digit* dig_c = c.digits_;

  const int64 nc = a.size();
  if (dig_a == dig_c || c.size() < nc) {
    dig_c = base::Allocator::allocate<Digit>(nc);
  }

  Digit carry = internal::mult(dig_a, a.size(), b, dig_c);
  if (dig_c != c.digits_) {
    base::Allocator::deallocate(c.digits_);
    c.digits_ = dig_c;
  }
  c.digits_ = dig_c;
  c.size_ = nc;

  if (carry)
    c.push_lead(carry);
}

void Natural::div(const Natural& a, const Digit b, Natural& c) {
  Digit* dig_a = a.digits_;
  Digit* dig_c = c.digits_;

  int64 n = a.size();
  if (c.size() < a.size()) {
    dig_c = base::Allocator::allocate<Digit>(a.size());
  }

  internal::div(dig_a, n, b, dig_c);
  if (dig_c != c.digits_) {
    base::Allocator::deallocate(c.digits_);
    c.digits_ = dig_c;
  }
  c.digits_ = dig_c;
  c.size_ = n;

  c.normalize();
}

std::ostream& operator<<(std::ostream& os, const Natural& a) {
  return internal::operator<<(os, a);
}

}  // namespace ppi
