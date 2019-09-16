#include "number/internal.h"

#include <glog/logging.h>

#include "number/decimal/decimal.h"
#include "number/uint128.h"

namespace ppi {
namespace internal {

Digit div(const Digit* a, const int64 n, const Digit b, Digit* c) {
  uint128 r = 0;
  for (int64 i = n - 1; i >= 0; --i) {
    r *= kBase;
    r += a[i];
    c[i] = r / b;
    r %= b;
  }
  return r;
}

}  // namespace internal
}  // namespace ppi
