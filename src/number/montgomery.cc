#include "number/montgomery.h"

#include <glog/logging.h>

#include "base/base.h"

namespace ppi {
namespace number {

namespace {

const int64 kBitSize = 64;
const int64 kHalfBitSize = kBitSize / 2;
const uint64 kLowerMask = (1ULL << kHalfBitSize) - 1;
const uint64 kMSBit = 1ULL << (kBitSize - 1);

uint64 Mult64_128(const uint64 a, const uint64 b, uint64* dst) {
  const uint64 ah = a >> kHalfBitSize;
  const uint64 al = a & kLowerMask;
  const uint64 bh = b >> kHalfBitSize;
  const uint64 bl = b & kLowerMask;

  uint64 ll = al * bl;
  uint64 lh = al * bh;
  uint64 hl = ah * bl;
  uint64 hh = ah * bh;

  hh += (hl >> kHalfBitSize) + (lh >> kHalfBitSize);
  uint64 m = (hl & kLowerMask) + (lh & kLowerMask) + (ll >> kHalfBitSize);
  ll = (ll & kLowerMask);

  hh += ((m + (ll >> kHalfBitSize)) >> kHalfBitSize);
  ll += (m << kHalfBitSize);

  *dst = hh;
  return ll;
}

// Returns b, where a*b mod 2^64 = -1.
uint64 GetNegativeInverse(uint64 a) {
  uint64 b = 1;
  uint64 sum = a;
  for (uint64 bit = 1; bit; bit <<= 1, a <<= 1) {
    if (sum & bit)
      continue;
    sum += a;
    b += bit;
  }
  return b;
}

}  // namespace

Montgomery::Montgomery(uint64 value) : value_(value) {}

Montgomery::Montgomery(uint64 value, uint64 mod) {
  value %= mod;
  for (int64 i = 0; i < kBitSize; ++i) {
    uint64 nvalue = value << 1;
    if (nvalue >= mod || (value & kMSBit))
      nvalue -= mod;
    value = nvalue;
  }
  value_ = value;
}

uint64 Montgomery::Power(uint64 a, uint64 e, uint64 m) {
  DCHECK(m % 2);
  // inverse * m = -1 mod 2^64
  uint64 inverse = GetNegativeInverse(m);

  Montgomery r(1, m);
  Montgomery amont(a, m);
  for (; e; e >>= 1) {
    if (e & 1)
      r = Mult(r, amont, m, inverse);
    amont = Mult(amont, amont, m, inverse);
  }
  return Mult(r, 1, m, inverse);
}

Montgomery Montgomery::Mult(const Montgomery& lhs,
                            const Montgomery& rhs,
                            uint64 mod,
                            uint64 inverse) {
  uint64 thi;
  uint64 tlo = Mult64_128(lhs, rhs, &thi);  // t = abar*bbar.

  uint64 tm = tlo * inverse;
  uint64 tmmhi;
  uint64 tmmlo = Mult64_128(tm, mod, &tmmhi);  // tmm = tm*m.

  // u = t + tmm
  uint64 ulo = tlo + tmmlo;
  uint64 uhi = thi + tmmhi;
  if (ulo < tlo)
    ++uhi;

  // The above addition can overflow. Detect that here.
  bool over = (uhi < thi) || (uhi == thi && ulo < tlo);
  // u >>= 64;
  ulo = uhi;
  if (over || ulo >= mod)
    ulo -= mod;
  return ulo;
}

}  // namespace number
}  // namespace ppi
