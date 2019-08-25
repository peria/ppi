#include "number/natural.h"

#include "base/allocator.h"
#include "number/internal/add.h"

namespace ppi {

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

void Natural::add(const Natural& a, const Natural& b, Natural& c) {
  if (a.size() < b.size())
    return add(b, a, c);

  Digit* dig_a = a.digits_;
  Digit* dig_b = b.digits_;
  Digit* dig_c = c.digits_;

  int64 n = a.size();
  if (c.size() < a.size()) {
    dig_c = base::Allocator::allocate<Digit>(a.size());
  }

  Digit carry = internal::add(dig_a, a.size(), dig_b, b.size(), dig_c);
  c.digits_ = dig_c;
  c.size_ = n;

  if (carry)
    c.push_lead(carry);
}

}  // namespace ppi
