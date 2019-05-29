#include "drm/drm.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>

#include "base/base.h"
#include "base/timer.h"
#include "number/real.h"

namespace ppi {
namespace drm {

double Drm::compute(const int64 num_digits, Real* pi) {
  const int64 num_limbs = pi->precision();
  const int64 num_terms = num_limbs * 64 / std::log2(151931373056000.0);
  double error = 0;

  int64 half = (num_terms + 1) / 2;
  LOG(INFO) << "Use " << num_terms << " terms to get " << (num_limbs * 16)
            << " hex digits.";
  Real a, b, c;

  {
    base::Timer timer;
    // Pass a, b, and c as Integer elements into binary split
    error = std::max(error, internal(0, half, &a, &b, &c));
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

  pi->setPrecision(num_limbs);

  return error;
}

double Drm::internal(int64 n0,
                     int64 n1,
                     Integer* a0,
                     Integer* b0,
                     Integer* c0) {
  Integer a1, b1, c1;
  if (n0 + 1 == n1) {
    int64 n = 2 * n0;
    setValues(n, a0, b0, c0);
    setValues(n + 1, &a1, &b1, &c1);

    Integer::Mult(*b0, a1, b0);
    Integer::Mult(*c0, b1, &b1);
    Integer::Subtract(*b0, b1, b0);
    Integer::Mult(*a0, a1, a0);
    Integer::Mult(*c0, c1, c0);
  } else {
    int64 m = (n0 + n1) / 2;
    internal(n0, m, a0, b0, c0);
    internal(m, n1, &a1, &b1, &c1);

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

}  // namespace pi
}  // namespace ppi
