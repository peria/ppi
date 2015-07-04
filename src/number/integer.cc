#include "number/integer.h"

#include "base/base.h"

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
}  // namespace

void Mult(const Integer& a, const Integer& b, Integer* c) {
  const int64 n = a.size();

  double* da = g_workarea;
  double* db = g_workarea + 8 * n;

  // Split int[n] -> double[4n]
  // Fill 0 in double[4n+1:8n]
  // FMT complex[4n]
  // Gather double[8n] -> int[2n]
}

}  // namespace number
}  // namespace ppi
