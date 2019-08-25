#include "number/internal.h"

#include <glog/logging.h>

#include "number/decimal/decimal.h"

namespace ppi {
namespace internal {

Digit subtract(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c) {
  DCHECK_GE(na, nb);

  Digit borrow = 0;
  for (int64 i = 0; i < nb; ++i) {
    Digit sum = a[i] - b[i] - borrow;
    if (sum > a[i]) {
      borrow = 1;
      sum += kBase;
    } else {
      borrow = 0;
    }
    c[i] = sum;
  }
  for (int64 i = nb; i < na; ++i) {
    Digit sum = a[i] - borrow;
    if (sum >= a[i]) {
      borrow = 1;
      sum += kBase;
    } else {
      borrow = 0;
    }
    c[i] = sum;
  }
  return borrow;
}

}  // namespace internal
}  // namespace ppi
