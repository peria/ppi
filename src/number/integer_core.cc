#include "number/integer_core.h"

#include <algorithm>
#include <cmath>
#include <ostream>

#include "base/allocator.h"
#include "base/base.h"
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

const int kMaskBitSize = 16;
const uint64 kMask = (1ULL << kMaskBitSize) - 1;

}  // namespace

double IntegerCore::Mult(const uint64* a, const int64 na,
                         const uint64* b, const int64 nb,
                         const int64 nc, uint64* c) {
  const int64 n = nc;
  const int64 nd = n * 4;
  double* da = WorkArea(0, nd);
  double* db = nullptr;

  // Split uint64[na] -> double[4n]
  Split4(a, na, n, da);
  fmt::Fft::TransformReal(fmt::Fft::Type::Forward, nd, da);

  if (a == b) {
    db = da;
  } else {
    db = WorkArea(1, 4 * n);
    Split4(b, nb, n, db);
    fmt::Fft::TransformReal(fmt::Fft::Type::Forward, nd, db);
  }

  da[0] *= db[0];
  da[1] *= db[1];
  for (int64 i = 1; i < 2 * n; ++i) {
    double ar = da[2 * i], ai = da[2 * i + 1];
    double br = db[2 * i], bi = db[2 * i + 1];
    da[2 * i    ] = ar * br - ai * bi;
    da[2 * i + 1] = ar * bi + ai * br;
  }
  
  fmt::Fft::TransformReal(fmt::Fft::Type::Inverse, nd, da);

  // Gather Complex[4n] -> uint64[n]
  return Gather4(da, n, c);
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
