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
    if (sum > a[i]) {
      borrow = 1;
      sum += kBase;
    } else {
      borrow = 0;
    }
    c[i] = sum;
  }
  return borrow;
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
      c[i] = kBase - b[i - eb + ec] - 1;
    for (int64 i = std::max<int64>(0, lb - ec); i < ea - ec; ++i)
      c[i] = kBase - 1;
    Digit sub = 1;
    for (int64 i = std::max<int64>(0, ea - ec); i < la - ec; ++i) {
      c[i] = a[i - ea + ec] - sub;
      if (c[i] > kBase) {
        sub = 1;
        c[i] += kBase;
      } else {
        sub = 0;
      }
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
