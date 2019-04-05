#pragma once

#include "base/base.h"

#ifndef UINT128
#include "number/montgomery.h"
#endif

namespace ppi {
namespace number {

uint64 Power(uint64 a, uint64 e, uint64 m) {
#ifdef UINT128
  uint128 r(1);
  uint128 an(a);
  for (; e; e >>= 1) {
    if (e & 1)
      r = r * an % m;
    an = an * an % m;
  }
  return static_cast<uint64>(r);
#else
  return Montgomery::Power(a, e, m);
#endif
}

}  // namespace number
}  // namespace ppi
