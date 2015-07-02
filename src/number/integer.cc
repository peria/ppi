#include "number/integer.h"

#include "base/base.h"

namespace ppi {
namespace number {

void Add(const Integer& a, const Integer& b, Integer* c) {
  DCHECK_EQ(a.size(), b.size());

  int64 n = a.size();
  uint64 carry = 0;
  for (int64 i = 0; i < n; ++i) {
    uint64 s = b[i] + carry;
    carry = (s < b[i]) ? 1 : 0;
    c->mantissa_[i] = a[i] + s;
    carry += (c->mantissa_[i] < s) ? 1 : 0;
  }
}

}  // namespace number
}  // namespace ppi
