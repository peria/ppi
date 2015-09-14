#include "number/integer.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>

#include "base/base.h"
#include "fmt/fmt.h"

namespace ppi {
namespace number {

void Add(const Integer& a, const Integer& b, Integer* c) {
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

void Subtract(const Integer& a, const Integer& b, Integer* c) {
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

namespace {

const int64 kMaxFFTSize = 1 << 15;
double g_workarea[2 * kMaxFFTSize];

void Split4In8(const Integer& a, double* da) {
  static const uint64 kMask = (1ULL << 15) - 1;
  const int64 n = a.size();
  for (int64 i = 0; i < n; ++i) {
    uint64 ia = a[i];
    da[8 * i] = ia & kMask;
    da[8 * i + 1] = 0;
    da[8 * i + 2] = (ia >> 15) & kMask;
    da[8 * i + 3] = 0;
    da[8 * i + 4] = (ia >> 30) & kMask;
    da[8 * i + 5] = 0;
    da[8 * i + 6] = ia >> 45;
    da[8 * i + 7] = 0;
  }
}

double Gather4(double* da, Integer* a) {
  // TODO: Evaluate the maximum error in the integration.
  double max_error = 0;
  const int64 n = a->size();
  for (int64 i = 0; i < n; ++i) {
    uint64 ia0 = da[4 * i]     + 0.5;
    max_error = std::max(max_error, std::abs(ia0 - da[4 * i]));
    uint64 ia1 = da[4 * i + 1] + 0.5;
    max_error = std::max(max_error, std::abs(ia1 - da[4 * i + 1]));
    uint64 ia2 = da[4 * i + 2] + 0.5;
    max_error = std::max(max_error, std::abs(ia2 - da[4 * i + 2]));
    uint64 ia3 = da[4 * i + 3] + 0.5;
    max_error = std::max(max_error, std::abs(ia3 - da[4 * i + 3]));
    a->mantissa_[i] = (ia3 << 45) | (ia2 << 30) | (ia1 << 15) | ia0;
  }
  return max_error;
}

}  // namespace

double Mult(const Integer& a, const Integer& b, Integer* c) {
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

}  // namespace number
}  // namespace ppi
