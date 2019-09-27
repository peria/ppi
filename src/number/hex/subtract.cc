#include "number/internal.h"

#include <glog/logging.h>

namespace ppi {
namespace internal {

namespace {

Digit subtract111(const Digit a, const Digit b, Digit& c) {
  Digit s = a - c;
  c = (s > a) ? 1 : 0;
  Digit t = s - b;
  if (t > s)
    c = 1;
  return t;
}

Digit subtract11(const Digit a, Digit& c) {
  Digit s = a - c;
  c = (s > a) ? 1 : 0;
  return s;
}

}

Digit subtract(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c) {
  DCHECK_GE(na, nb);

  Digit carry = 0;
  for (int64 i = 0; i < nb; ++i) {
    c[i] = subtract111(a[i], b[i], carry);
  }
  for (int64 i = nb; i < na; ++i) {
    c[i] = subtract11(a[i], carry);
  }
  return carry;
}

void fsubtract(const Digit* a, const int64 na, const int64 ea,
               const Digit* b, const int64 nb, const int64 eb,
               const int64, const int64 ec, Digit* c) {
  int64 la = ea + na;
  int64 lb = eb + nb;

  // Case A: B's mantissa is out of A's scope.
  //   aaaaa
  // -       bbb
  if (lb < ea) {
    for (int64 i = 0; i < eb - ec; ++i)
      c[i] = 0;
    for (int64 i = std::max<int64>(0, eb - ec); i < lb - ec; ++i)
      c[i] = -b[i - eb + ec];
    for (int64 i = std::max<int64>(0, lb - ec); i < ea - ec; ++i)
      c[i] = -1;
    Digit sub = 1;
    for (int64 i = std::max<int64>(0, ea - ec); i < la - ec; ++i) {
      c[i] = a[i - ea + ec] - sub;
      sub = (c[i] > a[i - ea + ec]) ? 1 : 0;
    }
    return;
  }

  // Case B: A and B's mantissas are overwrapped.
  //   aaaaa
  // -    bbb
  for (int64 i = 0; i < std::min(ea, eb) - ec; ++i)
    c[i] = 0;
  for (int64 i = std::max<int64>(0, ea - ec); i < std::max(ea, eb) - ec; ++i)
    c[i] = a[i - ea + ec];
  for (int64 i = std::max<int64>(0, eb - ec); i < std::max(ea, eb) - ec; ++i)
    c[i] = kBase - b[i - eb + ec] - 1;

  int64 esub = std::max(ea, eb);
  subtract(&a[esub - ea], la - esub,
           &b[esub - eb], lb - esub,
           &c[esub - ec]);
  // TODO: work for this carry-up.
  if (esub - ec >= 0)
    ++c[esub - ec];
}

}  // namespace internal
}  // namespace ppi
