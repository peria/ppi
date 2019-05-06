#include "number/natural.h"

#include "base/allocator.h"

namespace ppi {

Natural::Natural(Digit d)
  : digits_(base::Allocator::Allocate<Digit>(1)),
    size_(1) {
  digits_[0] = d;
}

Natural::Natural(const Natual& other)
  : digits_(base::Allocator::Allocate<Digit>(other.size())),
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

  Digits* ndigits = base::Allocator::Allocate<Digit>(nsize);
  for (int64 i = 0; i < size(); ++i)
    ndigits[i] = digits_[i];
  for (int64 i = size(); i < nsize; ++i)
    ndigits[i] = 0;
  if (digits_)
    base::Allocator::deallocate(digits_);
  digits_ = ndigits;
}

}  // namespace ppi
