#include "drm/drm.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>

#include "base/base.h"
#include "base/timer.h"
#include "number/real.h"

namespace ppi {
namespace drm {

double Drm::compute(const int64 num_dec, Real* pi) {
  const int64 num_hex = num_dec / std::log10(16) + 2;
  const int64 num_digits = num_hex / 16 + 2;
  const int64 num_terms = numTermsForDigits(num_dec);
  double error = 0;

  int64 half = (num_terms + 1) / 2;
  LOG(INFO) << "Use " << num_terms << " terms to get " << num_dec
            << " digits.";

  Real a, b, c;
  {
    base::Timer timer;
    // Pass a, b, and c as Integer elements into binary split
    error = std::max(error, internal(0, half, &a, &b, &c));
    timer.Stop();
    LOG(INFO) << "Binary Split: " << timer.GetTimeInSec() << " sec.";
    LOG(INFO) << "Sizes: a(" << a.size() << "), b(" << b.size() << ")";
  }
  // c is no longer used.
  c.clear();

  error = std::max(error, postCompute(&a, &b, pi));
  pi->setPrecision(num_digits);
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

}  // namespace drm
}  // namespace ppi
