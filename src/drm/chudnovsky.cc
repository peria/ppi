#include "drm/chudnovsky.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>

#include "base/base.h"
#include "base/timer.h"
#include "number/real.h"

namespace ppi {
namespace drm {

void Chudnovsky::SetValues(int64 n, Integer* a, Integer* b, Integer* c) {
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
