#pragma once

#include "base/base.h"
#include "number/natural.h"

namespace ppi {
namespace internal {

Digit add(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c);

}  // namespace internal
}  // namespace ppi

