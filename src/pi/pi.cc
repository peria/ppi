#include "pi/pi.h"

#include <cmath>

#include "base/base.h"
#include "number/real.h"

namespace ppi {
namespace pi {

double Pi::Arctan2(Real* pi) {
  int64 length = pi->precision();
  (*pi) = 0.0;
  pi->setPrecision(length);

  Real a = 16.0 * 5;
  Real sum = 0.0;
  Real b = 0.0;

  a.setPrecision(length);
  sum.setPrecision(length);
  b.setPrecision(length);
  const int64 n5 = length * 64 / std::log2(25) + 2;
  for (int64 i = 0; i < n5; ++i) {
    Real::Div(a, 25, &a);
    Real::Div(a, 2 * i + 1, &b);
    if (i % 2 == 0) {
      Real::Add(sum, b, &sum);
    } else {
      Real::Sub(sum, b, &sum);
    }
  }
  Real::Add(*pi, sum, pi);

  a = 4 * 239.0;
  sum = 0.0;
  b = 0.0;
  a.setPrecision(length);
  sum.setPrecision(length);
  b.setPrecision(length);
  const int64 n239 = length * 64 / std::log2(239 * 239) + 2;
  for (int64 i = 0; i < n239; ++i) {
    Real::Div(a, 239 * 239, &a);
    Real::Div(a, 2 * i + 1, &b);
    if (i % 2 == 0) {
      Real::Add(sum, b, &sum);
    } else {
      Real::Sub(sum, b, &sum);
    }
  }
  Real::Sub(*pi, sum, pi);

  return 0;
}

double Pi::Chudnovsky(Real* pi) {
  int64 length = pi->precision();
  int64 n = length * 64 / std::log2(151931373056000.0);

  int64 half = (n + 1) / 2;
  LOG(INFO) << "Use " << n << " terms to get " << (length * 16) << " hex digits.";
  Real a, b, c;

  // Pass a, b, and c as Integer elements into binary split
  ChudnovskyInternal(0, half, &a, &b, &c);
  VLOG(1) << a;
  VLOG(1) << b;
  VLOG(1) << c;
  // c is no longer used.
  c.clear();

  Integer::Mult(a, Integer(640320ULL * 8 * 10005 / 12), &a);
  int64 len = std::max(a.ssize(), b.ssize());
  if (len < 2)
    len = 2;
  a.setPrecision(len);
  b.setPrecision(len);
  pi->setPrecision(len);

  Real::Inverse(b, pi);
  Real::Mult(*pi, a, pi);
  Real::InverseSqrt(10005, &b);
  Real::Mult(*pi, b, pi);

  pi->setPrecision(length);

  return 0;
}

double Pi::ChudnovskyInternal(int64 n0, int64 n1,
                              Integer* a0, Integer* b0, Integer* c0) {
  Integer a1, b1, c1;
  if (n0 + 1 == n1) {
    int64 n = 2 * n0;
    ChudnovskySetValues(n, a0, b0, c0);
    ChudnovskySetValues(n + 1, &a1, &b1, &c1);

    Integer::Mult(*b0, a1, b0);
    Integer::Mult(*c0, b1, &b1);
    Integer::Subtract(*b0, b1, b0);
    Integer::Mult(*a0, a1, a0);
    Integer::Mult(*c0, c1, c0);
  } else {
    int64 m = (n0 + n1) / 2;
    ChudnovskyInternal(n0, m, a0, b0, c0);
    ChudnovskyInternal(m, n1, &a1, &b1, &c1);

    Integer::Mult(*b0, a1, b0);
    Integer::Mult(*c0, b1, &b1);
    Integer::Add(*b0, b1, b0);
    Integer::Mult(*a0, a1, a0);
    Integer::Mult(*c0, c1, c0);
  }

  return 0;
}

void Pi::ChudnovskySetValues(int64 n, Integer* a, Integer* b, Integer* c) {
  // TODO: Take care of overflow in implicit conversion
  if (n == 0) {
    *a = 1;
  } else {
    *a = n * 640320 / 24;
    Integer t(n * 640320);
    Integer::Mult(*a, t, a);
    Integer::Mult(*a, t, a);
  }

  *b = 13591409 + n * 545140134;

  *c = 6 * n + 1;
  Integer::Mult(*c, 2 * n + 1, c);
  Integer::Mult(*c, 6 * n + 5, c);
}

}  // namespace pi
}  // namespace ppi
