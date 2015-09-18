#include "number/integer.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>

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

void Integer::assign(uint64* mantissa, int64 sz) {
  mantissa_ = mantissa;
  size_ = sz;
}

void Integer::Set(int64 i, const uint64& limb) {
  mantissa_[i] = limb;
}

void Integer::Add(const Integer& a, const Integer& b, Integer* c) {
  DCHECK_EQ(a.size(), b.size());

  const int64 n = a.size();
  uint64 carry = 0;
  for (int64 i = 0; i < n; ++i) {
    uint64 s = b[i] + carry;
    carry = (s < b[i]) ? 1 : 0;
    c->mantissa_[i] = a[i] + s;
    carry += (c->mantissa_[i] < s) ? 1 : 0;
  }
}

void Integer::Subtract(const Integer& a, const Integer& b, Integer* c) {
  DCHECK_EQ(a.size(), b.size());

  const int64 n = a.size();
  uint64 carry = 0;
  for (int64 i = 0; i < n; ++i) {
    uint64 s = a[i] - carry;
    carry = (s > a[i]) ? 1 : 0;
    c->mantissa_[i] = s - b[i];
    carry += (c->mantissa_[i] > s) ? 1 : 0;
  }
}

double Integer::Mult(const Integer& a, const Integer& b, Integer* c) {
  const int64 n = a.size();

  double* da = g_workarea;
  double* db = g_workarea + 8 * n;

  // Split int[n] -> double[4n], where doube[4n+1:8n] = 0
  Split4In8(a, da);
  Split4In8(b, db);

  // FMT complex[4n]
  fmt::Fmt::Fmt4(fmt::Fft::Type::Forward, 4 * n, da);
  fmt::Fmt::Fmt4(fmt::Fft::Type::Forward, 4 * n, db);

  for (int64 i = 0; i < 4 * n; ++i) {
    double ar = da[2 * i], ai = da[2 * i + 1];
    double br = db[2 * i], bi = db[2 * i + 1];
    da[2 * i] = ar * br - ai * bi;
    da[2 * i + 1] = ar * bi + ai * br;
  }
  
  fmt::Fmt::Fmt4(fmt::Fft::Type::Inverse, 4 * n, da);

  // Gather double[8n] -> int[2n]
  c->size_ = 2 * n;
  double err = Gather4(da, c);

  return err;
}

double Integer::Gather4(double* da, Integer* a) {
  // TODO: Evaluate the maximum error in the integration.
  double max_error = 0;
  const int64 n = a->size();
  uint64 carry = 0;
  for (int64 i = 0; i < n; ++i) {
    // Convert double numbers to integers.
    uint64 ia0 = da[4 * i]     + 0.5;
    uint64 ia1 = da[4 * i + 1] + 0.5;
    uint64 ia2 = da[4 * i + 2] + 0.5;
    uint64 ia3 = da[4 * i + 3] + 0.5;

    // Compute rounding error.
    double e0 = std::abs(ia0 - da[4 * i]);
    double e1 = std::abs(ia1 - da[4 * i + 1]);
    double e2 = std::abs(ia2 - da[4 * i + 2]);
    double e3 = std::abs(ia3 - da[4 * i + 3]);
    max_error = std::max(max_error, std::max(std::max(e0, e1), std::max(e2, e3)));

    // Normalize to fit in kMaskBitSize bits.
    ia0 += carry;
    ia1 += (ia0 >> kMaskBitSize);
    ia2 += (ia1 >> kMaskBitSize);
    ia3 += (ia2 >> kMaskBitSize);
    carry = ia3 >> kMaskBitSize;

    // set value
    ia3 = ((ia3 & kMask) << (kMaskBitSize * 3));
    ia2 = ((ia2 & kMask) << (kMaskBitSize * 2));
    ia2 = ((ia1 & kMask) << kMaskBitSize);
    ia0 = ia0 & kMask;

    a->Set(i, ia3 | ia2 | ia1 | ia0);
  }
  return max_error;
}

void Integer::Split4In8(const Integer& a, double* da) {
  const int64 n = a.size();
  for (int64 i = 0; i < n; ++i) {
    uint64 ia = a[i];
    da[8 * i] = ia & kMask;
    da[8 * i + 1] = 0;
    da[8 * i + 2] = (ia >> kMaskBitSize) & kMask;
    da[8 * i + 3] = 0;
    da[8 * i + 4] = (ia >> (kMaskBitSize * 2)) & kMask;
    da[8 * i + 5] = 0;
    da[8 * i + 6] = ia >> (kMaskBitSize * 3);
    da[8 * i + 7] = 0;
  }
}

}  // namespace number
}  // namespace ppi
