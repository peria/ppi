#include "number/internal.h"

#include <glog/logging.h>

#include "number/decimal/decimal.h"

namespace ppi {
namespace internal {

Digit add(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c) {
  DCHECK_GE(na, nb);

  Digit carry = 0;
  for (int64 i = 0; i < nb; ++i) {
    Digit sum = a[i] + b[i] + carry;
    if (sum >= kBase) {
      carry = 1;
      sum -= kBase;
    } else {
      carry = 0;
    }
    c[i] = sum;
  }
  for (int64 i = nb; i < na; ++i) {
    Digit sum = a[i] + carry;
    if (sum >= kBase) {
      carry = 1;
      sum -= kBase;
    } else {
      carry = 0;
    }
    c[i] = sum;
  }
  return carry;
}

}  // namespace internal
}  // namespace ppi
