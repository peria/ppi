#include "number/internal.h"

#include <glog/logging.h>

#include "number/decimal/decimal.h"
#include "number/uint128.h"

namespace ppi {
namespace internal {

void mult_basic(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c) {
  DCHECK_NE(a, c);
  DCHECK_NE(b, c);

  const int64 n = na + nb;
  for (int64 i = 0; i < n; ++i)
    c[i] = 0;

  for (int64 i = 0; i < na; ++i) {
    for (int64 j = 0; j < nb; ++j) {
      uint128 m = a[i];
      m *= b[j];
      for (int64 k = i + j; m && k < n; ++k) {
        m += c[k];
        c[k] = static_cast<Digit>(m);
        m >>= 64;
      }
    }
  }
}

Digit mult(const Digit* a, const int64 na, const Digit b, Digit* c) {
  DCHECK_NE(a, c);

  Digit carry = 0;
  for (int64 i = 0; i < na; ++i) {
    uint128 m = a[i];
    m *= b;
    m += carry;
    c[i] = static_cast<uint64>(m);
    carry = m >> 64;
  }
  return carry;
}

}  // namespace internal
}  // namespace ppi
