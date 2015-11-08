#include "number/integer.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>
#include <ostream>
#include <vector>

#include "base/base.h"
#include "fmt/fmt.h"

namespace ppi {
namespace number {

namespace {

const int64 kMaxFFTSize = 1 << 15;
double g_workarea[2 * kMaxFFTSize];

const int kMaskBitSize = 16;
const uint64 kMask = (1ULL << kMaskBitSize) - 1;

}  // namespace

Integer::Integer() : std::vector<uint64>() {}

void Integer::normalize() {
  int64 i = size() - 1;
  while (i >= 0 && at(i) == 0) {
    --i;
  }
  ++i;
  erase(begin() + i, end());
}

// static
void Integer::Add(const Integer& a, const Integer& b, Integer* c) {
  DCHECK_EQ(a.size(), b.size());

  const int64 n = a.size();
  uint64 carry = 0;
  for (int64 i = 0; i < n; ++i) {
    uint64 s = b[i] + carry;
    carry = (s < b[i]) ? 1 : 0;
    (*c)[i] = a[i] + s;
    carry += ((*c)[i] < s) ? 1 : 0;
  }
}

// static
void Integer::Subtract(const Integer& a, const Integer& b, Integer* c) {
  DCHECK_EQ(a.size(), b.size());

  const int64 n = a.size();
  uint64 carry = 0;
  for (int64 i = 0; i < n; ++i) {
    uint64 s = a[i] - carry;
    carry = (s > a[i]) ? 1 : 0;
    (*c)[i] = s - b[i];
    carry += ((*c)[i] > s) ? 1 : 0;
  }
}

namespace {

int64 MinPow2(int64 n) {
  static const int64 kCandidates[] = {
      1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7,
      1 << 8, 1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14,
      1 << 15, 1 << 16, 1 << 17, 1 << 18, 1 << 19, 1 << 20, 1 << 21,
      1 << 22, 1 << 23, 1 << 24, 1 << 25, 1 << 26, 1 << 27, 1 << 28,
      1 << 29, 1 << 30,};
  return *std::lower_bound(kCandidates,
                           kCandidates + ARRAY_SIZE(kCandidates), n);
}

}

// static
double Integer::Mult(const Integer& a, const Integer& b, Integer* c) {
  const int64 na = a.size();
  const int64 nb = b.size();
  const int64 n = MinPow2(na + nb) / 2;

  double* da = g_workarea;
  double* db = g_workarea + 8 * n;

  // Split uint64[na] -> double[4n][2]
  Split4In8(a, n, da);
  // FMT it, with q=1/4
  fmt::Fmt::Fmt4(fmt::Fft::Type::Forward, 4 * n, da);

  if (&a == &b) {
    db = da;
  } else {
    Split4In8(b, n, db);
    fmt::Fmt::Fmt4(fmt::Fft::Type::Forward, 4 * n, db);
  }

  for (int64 i = 0; i < 4 * n; ++i) {
    double ar = da[2 * i], ai = da[2 * i + 1];
    double br = db[2 * i], bi = db[2 * i + 1];
    da[2 * i] = ar * br - ai * bi;
    da[2 * i + 1] = ar * bi + ai * br;
  }
  
  fmt::Fmt::Fmt4(fmt::Fft::Type::Inverse, 4 * n, da);

  // Gather double[8n][2] -> int[2n]
  c->resize(2 * n);
  double err = Gather4In8(da, c);

  return err;
}

double Integer::Gather4In8(double* da, Integer* a) {
  DCHECK_EQ(0, a->size() % 2);
  const int64 n = a->size() / 2;

  double err = 0;
  // 1. Dobule -> integral double
  for (int64 i = 0; i < 8 * n; ++i) {
    double dd = da[i];
    da[i] = std::floor(dd + 0.5);
    double e = std::abs(dd - da[i]);
    err = std::max(e, err);
  }

  // 2. Normalize & re-alignment
  uint64 carry = 0;
  double* d = da;
  for (int64 i = 0; i < n; ++i) {
    uint64 ia0 = d[0], ia1 = d[2], ia2 = d[4], ia3 = d[6];
    ia0 += carry;
    ia1 += ia0 >> kMaskBitSize;
    ia2 += ia1 >> kMaskBitSize;
    ia3 += ia2 >> kMaskBitSize;
    carry = ia3 >> kMaskBitSize;
    ia0 &= kMask;
    ia1 &= kMask;
    ia2 &= kMask;
    (*a)[i] = (((((ia3 << kMaskBitSize) + ia2) << kMaskBitSize) + ia1) << kMaskBitSize) + ia0;
    d += 8;
  }
  d = da;
  for (int64 i = 0; i < n; ++i) {
    uint64 ia0 = d[1], ia1 = d[3], ia2 = d[5], ia3 = d[7];
    ia0 += carry;
    ia1 += ia0 >> kMaskBitSize;
    ia2 += ia1 >> kMaskBitSize;
    ia3 += ia2 >> kMaskBitSize;
    carry = ia3 >> kMaskBitSize;
    ia0 &= kMask;
    ia1 &= kMask;
    ia2 &= kMask;
    (*a)[i + n] = (((((ia3 << kMaskBitSize) + ia2) << kMaskBitSize) + ia1) << kMaskBitSize) + ia0;
    d += 8;
  }

  return err;
}

void Integer::Split4In8(const Integer& a, const int64 n, double* da) {
  std::fill_n(da, n * 8, 0.0);

  const int64 na = a.size();
  for (int64 i = 0; i < std::min(n, na); ++i) {
    uint64 ia = a[i];
    da[8 * i] = ia & kMask;
    da[8 * i + 2] = (ia >> kMaskBitSize) & kMask;
    da[8 * i + 4] = (ia >> (kMaskBitSize * 2)) & kMask;
    da[8 * i + 6] = ia >> (kMaskBitSize * 3);
  }
  for (int64 i = n, j = 0; i < na; ++i, ++j) {
    uint64 ia = a[i];
    da[8 * j + 1] = ia & kMask;
    da[8 * j + 3] = (ia >> kMaskBitSize) & kMask;
    da[8 * j + 5] = (ia >> (kMaskBitSize * 2)) & kMask;
    da[8 * j + 7] = ia >> (kMaskBitSize * 3);
  }
}

namespace {
// TODO: Move these constant values to a header in base/.
const int64 kHalfSize = 32;
const int64 kHalfBitMask = (1ULL << kHalfSize) - 1;
}

uint64 Integer::Mult(const Integer& a, const uint32 b, Integer* c) {
  uint64 carry = 0;
  for (size_t i = 0; i < a.size(); ++i) {
    uint64 a_low = a[i] & kHalfBitMask;
    uint64 a_high = a[i] >> kHalfSize;
    uint64 c_low = a_low * b + carry;
    uint64 c_high = a_high * b;
    (*c)[i] = c_low + (c_high << kHalfSize);
    carry = c_high >> kHalfSize;
    if ((*c)[i] < c_low)
      ++carry;
  }
  return carry;
}

void Integer::Div(const Integer& a, const uint32 b, Integer* c) {
  uint64 limb = 0;
  for (int64 i = a.size() - 1; i >= 0; --i) {
    uint64 ia = a[i];
    limb = (limb << kHalfSize) + (ia >> kHalfSize);
    uint64 ic = limb / b;
    limb %= b;
    limb = (limb << kHalfSize) + (ia & kHalfBitMask);
    ic = (ic << kHalfSize) + limb / b;
    limb %= b;
    (*c)[i] = ic;
  }
  c->resize(a.size());
  c->normalize();
}

std::ostream& operator<<(std::ostream& os, const Integer& val) {
  static char buffer[50];
  for (int64 i = val.size() - 1; i >= 0; --i) {
    sprintf(buffer, "%016lx", val[i]);
    os << buffer;
  }
  return os;
}

}  // namespace number
}  // namespace ppi
