#include "number/natural.h"

#include "base/allocator.h"
#include "number/internal/mult.h"

namespace ppi {

Natural::Natural(Digit d)
  : digits_(base::Allocator::allocate<Digit>(1)),
    size_(1) {
  digits_[0] = d;
}

Natural::Natural(const Natural& other)
  : digits_(base::Allocator::allocate<Digit>(other.size())),
    size_(other.size()) {
  for (int64 i = 0; i < size(); ++i) {
    (*this)[i] = other[i];
  }
}

void Natural::push(const Digit d) {
  resize(size() + 1);
  digits_[size() - 1] = d;
}

void Natural::resize(const int64 nsize) {
  if (digits_ && nsize <= size()) {
    size_ = nsize;
    return;
  }

  Digit* ndigits = base::Allocator::allocate<Digit>(nsize);
  for (int64 i = 0; i < size(); ++i)
    ndigits[i] = digits_[i];
  for (int64 i = size(); i < nsize; ++i)
    ndigits[i] = 0;
  if (digits_)
    base::Allocator::deallocate(digits_);
  digits_ = ndigits;
}

Natural& Natural::operator=(const Digit d) {
  resize(1);
  digits_[0] = d;
  return *this;
}

// Natural& operator+=(const Natural& other);
// Natural& operator+=(const Digit d);
// Natural& operator-=(const Natural& other);
Natural& operator*=(const Natural& other) {
  int64 n_size = size() + other.size();
  Digit* digits = base::Allocator::allocate(n_size);
  internal::Mult(digits_, size(), other.digits_, other.size(), digits);
}

// Natural& operator*=(const Digit other);

}  // namespace ppi
