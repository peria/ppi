#include "drm/drm.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>

#include "base/base.h"
#include "base/timer.h"
#include "number/real.h"

namespace ppi {
namespace drm {

double Drm::compute(Real* pi) {
  int64 length = pi->precision();
  int64 n = length * 64 / std::log2(151931373056000.0);
  double error = 0;

  int64 half = (n + 1) / 2;
  LOG(INFO) << "Use " << n << " terms to get " << (length * 16)
            << " hex digits.";
  Real a, b, c;

  {
    base::Timer timer;
    // Pass a, b, and c as Integer elements into binary split
    error = std::max(error, Internal(0, half, &a, &b, &c));
    // c is no longer used.
    c.clear();
    timer.Stop();
    LOG(INFO) << "Binary Split: " << timer.GetTimeInSec() << " sec.";
    LOG(INFO) << "Sizes: a(" << a.size() << "), b(" << b.size() << ")";
  }

  Integer::Mult(a, 640320ULL * 8 * 10005 / 12, &a);
  int64 len = std::max(a.size(), b.size()) + 1;
  a.setPrecision(len);
  b.setPrecision(len);
  pi->setPrecision(len);

  {
    base::Timer timer;
    error = std::max(error, Real::Inverse(b, pi));
    error = std::max(error, Real::Mult(*pi, a, pi));
    timer.Stop();
    LOG(INFO) << "Division: " << timer.GetTimeInSec() << " sec.";
  }

  {
    base::Timer timer;
    error = std::max(error, Real::InverseSqrt(10005, &b));
    error = std::max(error, Real::Mult(*pi, b, pi));
    timer.Stop();
    LOG(INFO) << "Square Root: " << timer.GetTimeInSec() << " sec.";
  }

  pi->setPrecision(length);

  return error;
}

double Drm::Internal(int64 n0,
                     int64 n1,
                     Integer* a0,
                     Integer* b0,
                     Integer* c0) {
  Integer a1, b1, c1;
  if (n0 + 1 == n1) {
    int64 n = 2 * n0;
    SetValues(n, a0, b0, c0);
    SetValues(n + 1, &a1, &b1, &c1);

    Integer::Mult(*b0, a1, b0);
    Integer::Mult(*c0, b1, &b1);
    Integer::Subtract(*b0, b1, b0);
    Integer::Mult(*a0, a1, a0);
    Integer::Mult(*c0, c1, c0);
  } else {
    int64 m = (n0 + n1) / 2;
    Internal(n0, m, a0, b0, c0);
    Internal(m, n1, &a1, &b1, &c1);

    Integer::Mult(*b0, a1, b0);
    Integer::Mult(*c0, b1, &b1);
    Integer::Add(*b0, b1, b0);
    Integer::Mult(*a0, a1, a0);
    Integer::Mult(*c0, c1, c0);
  }

  VLOG(2) << n0 << " - " << n1;
  VLOG(2) << *a0;
  VLOG(2) << *b0;
  VLOG(2) << *c0;

  return 0;
}

void Drm::SetValues(int64 n, Integer* a, Integer* b, Integer* c) {
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
