#include "drm/chudnovsky.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>

#include "base/base.h"
#include "base/timer.h"
#include "number/real.h"

namespace ppi {
namespace drm {

namespace {

// Refer https://qiita.com/peria/items/c02ef9fc18fb0362fb89
const double kNumDigitsPerTerm = 14.1816474627;

}  // namespace

int64 Chudnovsky::numTermsForDigits(int64 num_digits) {
  return num_digits / kNumDigitsPerTerm + 1;
}

void Chudnovsky::setValues(int64 n, Integer* a, Integer* b, Integer* c) {
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
