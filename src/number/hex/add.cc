#include "number/internal.h"

#include <glog/logging.h>

namespace ppi {
namespace internal {

namespace {

Digit add111(const Digit a, const Digit b, Digit& c) {
  Digit s = a + c;
  c = (s < a) ? 1 : 0;
  s += b;
  if (s < b)
    c = 1;
  return s;
}

Digit add11(const Digit a, Digit& c) {
  Digit s = a + c;
  c = (s < a) ? 1 : 0;
  return s;
}

}

Digit add(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c) {
  DCHECK_GE(na, nb);

  Digit carry = 0;
  for (int64 i = 0; i < nb; ++i) {
    c[i] = add111(a[i], b[i], carry);
  }
  for (int64 i = nb; i < na; ++i) {
    c[i] = add11(a[i], carry);
  }
  return carry;
}

}  // namespace internal
}  // namespace ppi
