#include "drm/chudnovsky.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>

#include "base/base.h"
#include "base/timer.h"
#include "number/integer.h"
#include "number/real.h"

namespace ppi {
namespace drm {

double Chudnovsky::postCompute(Real* a, Real* b, Real* pi) {
  Integer::Mult(*a, 640320ULL * 8 * 10005 / 12, a);

  int64 length = std::max(a->size(), b->size()) + 1;
  a->setPrecision(length);
  pi->setPrecision(length);

  double error = 0;
  {
    base::Timer timer;
    error = std::max(error, Real::Inverse(*b, pi));
    error = std::max(error, Real::Mult(*pi, *a, pi));
    timer.Stop();
    LOG(INFO) << "Division: " << timer.GetTimeInSec() << " sec.";
  }

  a->clear();
  b->setPrecision(length);
  {
    base::Timer timer;
    error = std::max(error, Real::InverseSqrt(10005, b));
    error = std::max(error, Real::Mult(*pi, *b, pi));
    timer.Stop();
    LOG(INFO) << "Square Root: " << timer.GetTimeInSec() << " sec.";
  }

  return error;
}

int64 Chudnovsky::numTermsForDigits(int64 num_digits) {
  // Refer https://qiita.com/peria/items/c02ef9fc18fb0362fb89
  return num_digits / std::log10(151931373056000) + 1;
}

void Chudnovsky::setValues(int64 n, Integer* a, Integer* b, Integer* c) {
  // TODO: Take care of overflow in implicit conversion
  if (n == 0) {
    *a = 1;
  } else {
    *a = n * (640320 / 24);
    Integer t(n * 640320);
    Integer::Mult(*a, t, a);
    Integer::Mult(*a, t, a);
  }

  *b = 13591409 + n * 545140134;

  *c = 6 * n + 5;
  Integer::Mult(*c, 6 * n + 1, c);
  Integer::Mult(*c, 2 * n + 1, c);
}

}  // namespace drm
}  // namespace ppi
