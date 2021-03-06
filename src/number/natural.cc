#include "number/natural.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>
#include <ostream>

#include "base/allocator.h"
#include "base/base.h"
#include "fmt/fmt.h"
#include "fmt/rft.h"

namespace ppi {
namespace number {

namespace {

double* g_workarea[2];

double* WorkArea(int index, int64 size) {
  double*& workarea = g_workarea[index];
  if (base::Allocator::GetSize(workarea) < size) {
    if (workarea)
      base::Allocator::Deallocate(workarea);
    workarea = base::Allocator::Allocate<double>(size);
  }
  return workarea;
}

// TODO: Rename these constants
constexpr int kMaskBitSize = 16;
constexpr uint64 kMask = (1ULL << kMaskBitSize) - 1;
constexpr uint64 kShortBase = 1ULL << 32;
constexpr uint64 kHalfMask = kShortBase - 1;

int64 LeadingZeros(uint64 x) {
  if (x == 0)
    return 64;

  int64 n = 1;
  if ((x >> 32) == 0) {
    n += 32;
    x <<= 32;
  }
  if ((x >> (32 + 16)) == 0) {
    n += 16;
    x <<= 16;
  }
  if ((x >> (32 + 16 + 8)) == 0) {
    n += 8;
    x <<= 8;
  }
  if ((x >> (32 + 16 + 8 + 4)) == 0) {
    n += 4;
    x <<= 4;
  }
  if ((x >> (32 + 16 + 8 + 4 + 2)) == 0) {
    n += 2;
    x <<= 2;
  }
  return n - (x >> 63);
}

// Core part of Div routines to compute an[3] / bn, assuming an[2] < bn.
// Returns the quotient, and stores the "normalized" reminder in cn (if not
// null).
// It requires some conditions written in DCHECK()s.
// Algorithm is described in pp. 159-163 in "Hackers Delight" translated into
// Japanese.
inline uint64 DivCore(const uint64* an,
                      const uint64 bn0,
                      const uint64 bn1,
                      uint64* cn) {
  DCHECK_EQ(1ULL, bn1 >> 31);
  DCHECK_LT(bn0, kShortBase);
  DCHECK_LT(an[0], kShortBase);
  DCHECK_LT(an[1], kShortBase);

  uint64 rem = an[2];
  uint64 q1 = rem / bn1;
  if (q1 >= kShortBase) {
    q1 = kShortBase - 1;
  }
  rem = (rem - q1 * bn1) * kShortBase + an[1];
  if (rem < q1 * bn0) {
    --q1;
    rem += bn1 * kShortBase;
    if (rem < q1 * bn0) {
      --q1;
      rem += bn1 * kShortBase;
    }
  }
  rem -= q1 * bn0;

  uint64 q0 = rem / bn1;
  if (q0 >= kShortBase) {
    q0 = kShortBase - 1;
  }
  rem = (rem - q0 * bn1) * kShortBase + an[0];
  if (rem < q0 * bn0) {
    --q0;
    rem += bn1 * kShortBase;
    if (rem < q0 * bn0) {
      --q0;
      rem += bn1 * kShortBase;
    }
  }

  if (cn)
    *cn = rem - q0 * bn0;
  return q1 * kShortBase + q0;
}

}  // namespace

uint64 Natural::Add(const uint64* a,
                    const uint64* b,
                    const int64 n,
                    uint64* c) {
  uint64 carry = 0;
  for (int64 i = 0; i < n; ++i) {
    uint64 s = b[i] + carry;
    carry = (s < b[i]) ? 1 : 0;
    c[i] = a[i] + s;
    carry += (c[i] < s) ? 1 : 0;
  }
  return carry;
}

uint64 Natural::Add(const uint64* a, uint64 b, const int64 n, uint64* c) {
  int64 i;
  for (i = 0; i < n && b; ++i) {
    uint64 s = a[i] + b;
    b = (s < a[i]) ? 1 : 0;
    c[i] = s;
  }
  for (; i < n; ++i) {
    c[i] = a[i];
  }
  return b;
}

uint64 Natural::Subtract(const uint64* a,
                         const uint64* b,
                         const int64 n,
                         uint64* c) {
  uint64 carry = 0;
  for (int64 i = 0; i < n; ++i) {
    uint64 s = a[i] - carry;
    carry = (s > a[i]) ? 1 : 0;
    c[i] = s - b[i];
    carry += (c[i] > s) ? 1 : 0;
  }
  return carry;
}

uint64 Natural::Subtract(const uint64* a, uint64 b, const int64 n, uint64* c) {
  int64 i;
  for (i = 0; i < n && b; ++i) {
    uint64 s = a[i] - b;
    b = (s > a[i]) ? 1 : 0;
    c[i] = s;
  }
  for (; i < n; ++i) {
    c[i] = a[i];
  }
  return b;
}

double Natural::Mult(const uint64* a,
                     const int64 na,
                     const uint64* b,
                     const int64 nb,
                     const int64 nc,
                     uint64* c) {
  return MultFmt(a, na, b, nb, nc, c);
}

double Natural::MultFmt(const uint64* a,
                        const int64 na,
                        const uint64* b,
                        const int64 nb,
                        const int64 nc,
                        uint64* c) {
  const int64 n = nc;
  const int64 nd = n * 4;
  double* da = WorkArea(0, nd);
  double* db = nullptr;

  fmt::Rft rft(nd);

  // Split uint64[na] -> double[4n]
  Split4(a, na, n, da);
  rft.Transform(fmt::Direction::Forward, da);

  if (a == b) {
    db = da;
  } else {
    db = WorkArea(1, 4 * n);
    Split4(b, nb, n, db);
    rft.Transform(fmt::Direction::Forward, db);
  }

  da[0] *= db[0];
  da[1] *= db[1];
  for (int64 i = 1; i < 2 * n; ++i) {
    double ar = da[2 * i], ai = da[2 * i + 1];
    double br = db[2 * i], bi = db[2 * i + 1];
    da[2 * i] = ar * br - ai * bi;
    da[2 * i + 1] = ar * bi + ai * br;
  }

  rft.Transform(fmt::Direction::Backward, da);

  // Gather Complex[4n] -> uint64[n]
  return Gather4(da, n, c);
}

uint64 Natural::Mult(const uint64* a,
                     const uint64 b,
                     const int64 n,
                     uint64* c) {
  uint64 carry = 0;
#if defined(UINT128)
  uint128 b128 = b;
  for (int64 i = 0; i < n; ++i) {
    uint128 ab = a[i] * b128 + carry;
    c[i] = ab;
    carry = ab >> 64;
  }
#else
  uint64 bl = b & kHalfBitMask;
  uint64 bh = b >> kHalfSize;
  for (int64 i = 0; i < n; ++i) {
    uint64 al = a[i] & kHalfBitMask;
    uint64 ah = a[i] >> kHalfSize;
    uint64 c00 = al * bl;
    uint64 c01 = al * bh;
    uint64 c10 = ah * bl;
    uint64 c11 = ah * bh;
    c01 += c10;
    if (c01 < c10) {
      c11 += 1ULL << kHalfSize;
    }
    uint64 u = c00 + (c01 << kHalfSize);
    if (u < c00) {
      ++c11;
    }
    c11 += c01 >> kHalfSize;

    c[i] = u + carry;
    if (c[i] < u) {
      ++c11;
    }
    carry = c11;
  }
#endif
  return carry;
}

uint64 Natural::Div(const uint64* a, const uint64 b, uint64* c) {
  DCHECK_LT(a[1], b);

  // Normalize numbers to set divisor to have the MSB.
  int64 shift = LeadingZeros(b);
  uint64 bn = b << shift;
  uint64 bn1 = bn >> 32;
  uint64 bn0 = bn & kHalfMask;

  uint64 an0 = a[0];
  uint64 an1 = a[1];
  if (shift) {
    an1 = (a[1] << shift) + (a[0] >> (64 - shift));
    an0 = a[0] << shift;
  }

  uint64 an[]{an0 & kHalfMask, an0 >> 32, an1};
  uint64 q = DivCore(an, bn0, bn1, c);
  if (c)
    *c >>= shift;
  return q;
}

uint64 Natural::Div(const uint64* a, const uint64 b, const int64 n, uint64* c) {
  // Normalize numbers to set the divisor to have the MSB.
  const int64 shift = LeadingZeros(b);
  const uint64 bn = b << shift;
  const uint64 bn1 = bn >> 32;
  const uint64 bn0 = bn & kHalfMask;

  uint64 rem = (a[n - 1] % b) << shift;
  c[n - 1] = a[n - 1] / b;
  for (int64 i = n - 2; i >= 0; --i) {
    uint64 an1 = rem + (a[i] >> (64 - shift));
    uint64 an0 = a[i] << shift;
    uint64 an[4]{an0 & kHalfMask, an0 >> 32, an1};
    c[i] = DivCore(an, bn0, bn1, &rem);
  }
  return rem >> shift;
}

uint64 Natural::Div(const uint64 a, const uint64 b, const int64 n, uint64* c) {
  DCHECK_LT(a, b);

#ifdef UINT128
  uint128 r(a);
  for (int64 i = n - 1; i >= 0; --i) {
    r <<= 64;
    c[i] = r / b;
    r %= b;
  }
  return static_cast<uint64>(r);
#else
  // Normalize numbers to set divisor to have the MSB.
  const int64 shift = LeadingZeros(b);
  const uint64 bn = b << shift;
  const uint64 bn1 = bn >> 32;
  const uint64 bn0 = bn & kHalfMask;

  uint64 rem = a << shift;
  for (int64 i = n - 1; i >= 0; --i) {
    uint64 an[]{0, 0, rem};
    c[i] = DivCore(an, bn0, bn1, &rem);
  }
  return rem >> shift;
#endif  // UINT128
}

void Natural::Split4(const uint64* a,
                     const int64 na,
                     const int64 n,
                     double* ca) {
  static constexpr double kDoubleBase = (1ULL << kMaskBitSize);
  static constexpr double kHalfBase = kDoubleBase / 2;

  for (int64 i = 0; i < na; ++i) {
    uint64 ia = a[i];
    ca[4 * i] = ia & kMask;
    ca[4 * i + 1] = (ia >> kMaskBitSize) & kMask;
    ca[4 * i + 2] = (ia >> (kMaskBitSize * 2)) & kMask;
    ca[4 * i + 3] = ia >> (kMaskBitSize * 3);
  }
  for (int64 i = 4 * na; i < 4 * n; ++i) {
    ca[i] = 0;
  }

  for (int64 i = 0; i < 4 * na - 1; ++i) {
    if (ca[i] >= kHalfBase) {
      ca[i] -= kDoubleBase;
      ca[i + 1] += 1;
    }
  }
}

double Natural::Gather4(double* ca, const int64 n, uint64* a) {
  static constexpr double kDoubleBase = (1ULL << kMaskBitSize);

  // double -> integral double
  double err = 0;
  double carry = 0;
  for (int64 i = 0; i < 4 * n; ++i) {
    double d = std::floor(ca[i] + 0.5);
    err = std::max(err, std::abs(d - ca[i]));
    d += carry;
    carry = std::floor(d / kDoubleBase);
    ca[i] = d - carry * kDoubleBase;
  }
  // Because of cyclic convolution, we may have a carry from
  // MSD to LSD.
  ca[0] += carry;

  // Normalize & re-alignment
  for (int64 i = 0; i < n; ++i) {
    uint64 ia0 = ca[4 * i];
    uint64 ia1 = ca[4 * i + 1];
    uint64 ia2 = ca[4 * i + 2];
    uint64 ia3 = ca[4 * i + 3];
    a[i] = (((((ia3 << kMaskBitSize) + ia2) << kMaskBitSize) + ia1)
            << kMaskBitSize) +
           ia0;
  }

  return err;
}

}  // namespace number
}  // namespace ppi
