#pragma once

#include "base/base.h"
#include "number/natural.h"

namespace ppi {
namespace internal {

Digit add(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c);
Digit subtract(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c);
Digit div(const Digit* a, const int64 na, const Digit b, Digit* c);

std::ostream& operator<<(std::ostream& os, const Natural& a);

}  // namespace internal
}  // namespace ppi

