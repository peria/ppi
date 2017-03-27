#include "number/integer_core.h"

#include <algorithm>
#include <cmath>
#include <ostream>

#include "base/allocator.h"
#include "base/base.h"
#include "fmt/dwt.h"
#include "fmt/fmt.h"

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

// Core part of Div routines to compute an[4] / bn, assuming an[3]*R+an[2] < bn.
// Returns the quotient, and stores the "normalized" reminder in cn (if not null).
// It requires some conditions written in DCHECK()s.
inline uint64 DivCore(const uint64* an,
                      const uint64 bn, const uint64 bn0, const uint64 bn1,
                      uint64* cn) {
  // Algorithm is described in pp. 159-163 in "Hackers Delight" translated into Japanese.

  DCHECK_EQ(bn, bn1 * kShortBase + bn0);
  DCHECK_EQ(1, bn >> 63);
  DCHECK_LT(bn0, kShortBase);
  DCHECK_LT(bn1, kShortBase);
  DCHECK_LT(an[0], kShortBase);
  DCHECK_LT(an[1], kShortBase);
  DCHECK_LT(an[2], kShortBase);
  DCHECK_LT(an[3], kShortBase);

  uint64 an32 = an[3] * kShortBase + an[2];
  uint64 q1 = an32 / bn1;
  uint64 rhat = an32 - q1 * bn1;
  const uint64 an1 = an[1];
  if (q1 >= kShortBase) {
    --q1;
    rhat += bn1;
  }
  if (q1 * bn0 > rhat * kShortBase + an1) {
    --q1;
    rhat += bn1;
    if (q1 * bn0 > rhat * kShortBase + an1) {
      --q1;
      rhat += bn1;
    }
  }

  uint64 an21 = an[2] * kShortBase + an1 - q1 * bn;
  uint64 q0 = an21 / bn1;
  rhat = an21 - q0 * bn1;
  const uint64 an0 = an[0];
  if (q0 >= kShortBase) {
    --q0;
    rhat += bn1;
  }
  if (q0 * bn0 > rhat * kShortBase + an0) {
    --q0;
    rhat += bn1;
    if (q0 * bn0 > rhat * kShortBase + an0) {
      --q0;
      rhat += bn1;
    }
  }

  if (cn)
    *cn = an21 * kShortBase + an0 - q0 * bn;
  return q1 * kShortBase + q0;
}

}  // namespace

uint64 IntegerCore::Add(const uint64* a, const uint64* b, const int64 n, uint64* c) {
  uint64 carry = 0;
  for (int64 i = 0; i < n; ++i) {
    uint64 s = b[i] + carry;
    carry = (s < b[i]) ? 1 : 0;
    c[i] = a[i] + s;
    carry += (c[i] < s) ? 1 : 0;
  }
  return carry;
}

uint64 IntegerCore::Add(const uint64* a, uint64 b, const int64 n, uint64* c) {
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

uint64 IntegerCore::Subtract(const uint64* a, const uint64* b, const int64 n, uint64* c) {
  uint64 carry = 0;
  for (int64 i = 0; i < n; ++i) {
    uint64 s = a[i] - carry;
    carry = (s > a[i]) ? 1 : 0;
    c[i] = s - b[i];
    carry += (c[i] > s) ? 1 : 0;
  }
  return carry;
}

uint64 IntegerCore::Subtract(const uint64* a, uint64 b, const int64 n, uint64* c) {
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

double IntegerCore::Mult(const uint64* a, const int64 na,
                         const uint64* b, const int64 nb,
                         const int64 nc, uint64* c) {
  const int64 n = nc;
  const int64 nd = n * 4;
  double* da = WorkArea(0, nd);
  double* db = nullptr;

  fmt::Config config(nd / 2);

  // Split uint64[na] -> double[4n]
  Split4(a, na, n, da);
  fmt::Rft::Transform(config, fmt::Direction::Forward, da);

  if (a == b) {
    db = da;
  } else {
    db = WorkArea(1, 4 * n);
    Split4(b, nb, n, db);
    fmt::Rft::Transform(config, fmt::Direction::Forward, db);
  }

  da[0] *= db[0];
  da[1] *= db[1];
  for (int64 i = 1; i < 2 * n; ++i) {
    double ar = da[2 * i], ai = da[2 * i + 1];
    double br = db[2 * i], bi = db[2 * i + 1];
    da[2 * i    ] = ar * br - ai * bi;
    da[2 * i + 1] = ar * bi + ai * br;
  }

  fmt::Rft::Transform(config, fmt::Direction::Backward, da);

  // Gather Complex[4n] -> uint64[n]
  return Gather4(da, n, c);
}

uint64 IntegerCore::Div(const uint64* a, const uint64 b, uint64* c) {
  DCHECK_LT(a[1], b);

  // Normalize numbers to set divisor to have the MSB.
  int64 shift = LeadingZeros(b);

  uint64 an0 = a[0];
  uint64 an1 = a[1];
  if (shift) {
    an1 = (a[1] << shift) + (a[0] >> (64 - shift));
    an0 = a[0] << shift;
  }
  uint64 an[4];
  an[0] = an0 & kHalfMask;
  an[1] = an0 >> 32;
  an[2] = an1 & kHalfMask;
  an[3] = an1 >> 32;

  uint64 bn = b << shift;
  uint64 bn1 = bn >> 32;
  uint64 bn0 = bn & kHalfMask;

  uint64 q = DivCore(an, bn, bn0, bn1, c);
  if (c)
    *c >>= shift;
  return q;
}

uint64 IntegerCore::Div(const uint64 a, const uint64 b, const int64 n, uint64* c) {
  DCHECK_LT(a, b);

  // Normalize numbers to set divisor to have the MSB.
  const int64 shift = LeadingZeros(b);
  const uint64 bn = b << shift;
  const uint64 bn1 = bn >> 32;
  const uint64 bn0 = bn & kHalfMask;

  uint64 an = a << shift;
  for (int64 i = n - 1; i >= 0; --i) {
    uint64 q1 = an / bn1;
    uint64 rhat = an - q1 * bn1;
    if (q1 >= kShortBase) {
      --q1;
      rhat += bn1;
    }
    if (q1 * bn0 > rhat * kShortBase) {
      --q1;
      rhat += bn1;
      if (q1 * bn0 > rhat * kShortBase) {
        --q1;
        rhat += bn1;
      }
    }

    uint64 ans = an * kShortBase - q1 * bn;
    uint64 q0 = ans / bn1;
    rhat = ans - q0 * bn1;
    if (q0 >= kShortBase) {
      --q0;
      rhat += bn1;
    }
    if (q0 * bn0 > rhat * kShortBase) {
      --q0;
      rhat += bn1;
      if (q0 * bn0 > rhat * kShortBase) {
        --q0;
        rhat += bn1;
      }
    }

    c[i] = q1 * kShortBase + q0;
    an = ans * kShortBase - q0 * bn;
  }
  return an >> shift;
}

void IntegerCore::Split4(const uint64* a, const int64 na, const int64 n, double* ca) {
  for (int64 i = 0; i < std::min(n, na); ++i) {
    uint64 ia = a[i];
    ca[4 * i    ] = ia & kMask;
    ca[4 * i + 1] = (ia >> kMaskBitSize) & kMask;
    ca[4 * i + 2] = (ia >> (kMaskBitSize * 2)) & kMask;
    ca[4 * i + 3] = ia >> (kMaskBitSize * 3);
  }
  for (int64 i = 4 * na; i < 4 * n; ++i) {
    ca[i] = 0;
  }

  // Nega-cyclic part
  for (int64 i = n, j = 0; i < na; ++i, ++j) {
    uint64 ia = a[i];
    ca[4 * j    ] -= ia & kMask;
    ca[4 * j + 1] -= (ia >> kMaskBitSize) & kMask;
    ca[4 * j + 2] -= (ia >> (kMaskBitSize * 2)) & kMask;
    ca[4 * j + 3] -= ia >> (kMaskBitSize * 3);
  }
}

double IntegerCore::Gather4(double* ca, const int64 n, uint64* a) {
  double err = 0;
  // 1. dobule -> integral double
  for (int64 i = 0; i < 4 * n; ++i) {
    double d = ca[i];
    ca[i] = std::floor(d + 0.5);
    err = std::max(err, std::abs(d - ca[i]));
  }

  // 2. Normalize & re-alignment
  uint64 carry = 0;
  for (int64 i = 0; i < n; ++i) {
    uint64 ia0 = ca[4 * i    ];
    uint64 ia1 = ca[4 * i + 1];
    uint64 ia2 = ca[4 * i + 2];
    uint64 ia3 = ca[4 * i + 3];
    ia0 += carry;
    ia1 += ia0 >> kMaskBitSize;
    ia2 += ia1 >> kMaskBitSize;
    ia3 += ia2 >> kMaskBitSize;
    carry = ia3 >> kMaskBitSize;
    ia0 &= kMask;
    ia1 &= kMask;
    ia2 &= kMask;
    a[i] = (((((ia3 << kMaskBitSize) + ia2) << kMaskBitSize) + ia1) << kMaskBitSize) + ia0;
  }

  return err;
}

}  // namespace number
}  // namespace ppi
